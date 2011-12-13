#include <QDebug>
#include <QMutexLocker>

#include "HeadwayKalmanFilter.h"

#define ACCUMULATOR_TIME 500    // accumulator time in ms
#define LEAD_VEHICLE_PRESENT_THRESHOLD 0.5
#define LEAD_VEHICLE_ALIGNED_THRESHOLD 0.5

#define ACCUMULATOR_FRACTION_OLD(t) ((float) ( ACCUMULATOR_TIME - qMin( ACCUMULATOR_TIME, t )) / ACCUMULATOR_TIME)
#define ACCUMULATOR_FRACTION_NEW(t) (1 - ACCUMULATOR_FRACTION_OLD(t))
#define UPDATE_ACCUMULATOR( accumulator, value, t ) (accumulator = (ACCUMULATOR_FRACTION_OLD( t ) * accumulator) + (ACCUMULATOR_FRACTION_NEW( t ) * ((float) value)))

HeadwayKalmanFilter::HeadwayKalmanFilter( QObject *parent = 0 ) : QObject( parent ) {
    /* Start out assuming there's a lead vehicle? */
    leadVehiclePresentAccumulator = 1.0;

    /* States:  [ currentDistance,
                  previousDistance,
                  currentFollowingVelocity,
                  previousFollowingVelocity ]
     */

    /* We're going to trick the internals of OpenCV and actually use TWO
       measurements, but we'll initialize it with only one. */
    kalmanFilter = cv::KalmanFilter( 4, 1, 0, CV_32F );

    dt = 0;

    kalmanFilter.transitionMatrix = *(cv::Mat_<float>( 4, 4 ) << 2, -1, -dt,  dt,
                                                                 1,  0,   0,   0,
                                                                 0,  0,   2,  -1,
                                                                 0,  0,   1,   0);

    /* Set the process noise covariance matrix, Q
       NOTE: These values are guesses */
    kalmanFilter.processNoiseCov = *(cv::Mat_<float>( 4, 4 ) << 3, 0, 0, 0,
                                                                0, 1, 0, 0,
                                                                0, 0, 4, 0,
                                                                0, 0, 0, 8);

    /* Create measurement transformation matrices, H, for distance and speed */
    distanceH = *(cv::Mat_<float>( 1, 4 ) << 1, 0, 0, 0);
    speedH =    *(cv::Mat_<float>( 1, 4 ) << 0, 0, 1, 0);

    /* Create measurement noise covariance matrices, R, for distance and speed
       NOTE: These values are guesses */
    distanceR = *(cv::Mat_<float>( 1, 1 ) << 3);
    speedR    = *(cv::Mat_<float>( 1, 1 ) << 0.2);

    /* Create measurement matrices */
    distanceMeasurement = *(cv::Mat_<float>( 1, 1 ) << 0);
    speedMeasurement    = *(cv::Mat_<float>( 1, 1 ) << 0);
}

HeadwayKalmanFilter::~HeadwayKalmanFilter() {
    qDebug() << "~HeadwayKalmanFilter()";
}

void HeadwayKalmanFilter::updateLeadVehiclePresent( bool vehiclePresent, bool vehicleAligned ) {
    QMutexLocker locker( &leadVehiclePresentAlignedMutex );

    int t = leadVehiclePresentAlignedTimer.restart();
    UPDATE_ACCUMULATOR( leadVehiclePresentAccumulator, vehiclePresent, t );

    if ( leadVehiclePresentAccumulator > LEAD_VEHICLE_PRESENT_THRESHOLD ) {
        UPDATE_ACCUMULATOR( leadVehicleAlignedAccumulator, vehicleAligned, t );
    }
}

void HeadwayKalmanFilter::updateDistanceReading( float distance ) {    // distance is in meters
    /* See if the value is usable */
    leadVehiclePresentAlignedMutex.lock();

    if ( (leadVehiclePresentAccumulator < LEAD_VEHICLE_PRESENT_THRESHOLD) || (leadVehicleAlignedAccumulator < LEAD_VEHICLE_ALIGNED_THRESHOLD) ) {
        /* If there's no lead vehicle, or it's not aligned, the measurement is useless */
        leadVehiclePresentAlignedMutex.unlock();

        return;
    }

    leadVehiclePresentAlignedMutex.unlock();


    /* Predict the current state */
    kalmanFilterMutex.lock();

    predict();

    kalmanFilter.measurementMatrix = distanceH;
    kalmanFilter.measurementNoiseCov = distanceR;


    /* Update the distance accumulator */
    distanceHeadwayMutex.lock();

    int t = distanceHeadwayTimer.restart();
    UPDATE_ACCUMULATOR( distanceHeadwayAccumulator, distance, t );


    /* Correct the Kalman filter */
    distanceMeasurement.at<float>( 0, 0 ) = distanceHeadwayAccumulator;

    distanceHeadwayMutex.unlock();

    kalmanFilter.correct( distanceMeasurement );

    kalmanFilterMutex.unlock();

    emitSignals();
}

void HeadwayKalmanFilter::updateSpeedReading( const float speed ) {    // speed is in m/s
    /* Predict the current state */
    kalmanFilterMutex.lock();

    predict();

    kalmanFilter.measurementMatrix = speedH;
    kalmanFilter.measurementNoiseCov = speedR;


    /* Update the speed accumulator */
    followingVehicleVelocityMutex.lock();

    int t = followingVehicleVelocityTimer.restart();
    UPDATE_ACCUMULATOR( followingVehicleVelocityAccumulator, speed, t );


    /* Correct the Kalman filter */
    speedMeasurement.at<float>( 0, 0 ) = followingVehicleVelocityAccumulator;

    followingVehicleVelocityMutex.unlock();

    kalmanFilter.correct( speedMeasurement );

    kalmanFilterMutex.unlock();

    emitSignals();
}

void HeadwayKalmanFilter::emitSignals() {
    QMutexLocker locker( &dtMutex );

    float currentDistance  = kalmanFilter.statePost.at<float>( 0, 0 );
    float previousDistance = kalmanFilter.statePost.at<float>( 1, 0 );
    float currentVelocity  = kalmanFilter.statePost.at<float>( 2, 0 );
    float previousVelocity = kalmanFilter.statePost.at<float>( 3, 0 );

    float timeHeadway = currentVelocity < 1 ? INT_MAX : (currentDistance / currentVelocity);
    float distanceHeadway = kalmanFilter.statePost.at<float>( 0, 0 );
    float leadVehicleVelocity = ((currentDistance - previousDistance) / dt) + ((currentVelocity + previousVelocity) / 2);
    float leadVehicleAcceleration = 0;    // set below
    float followingVehicleVelocity = kalmanFilter.statePost.at<float>( 2, 0 ) < 1 ? 0 : kalmanFilter.statePost.at<float>( 2, 0 );
    float followingVehicleAcceleration = kalmanFilter.statePost.at<float>( 2, 0 ) < 1 ? 0 : (currentVelocity - previousVelocity) / dt;

    /* Handle all of the tricky stuff for the lead vehicle */
    leadVehiclePresentAlignedMutex.lock();
    if ( leadVehiclePresentAccumulator > LEAD_VEHICLE_PRESENT_THRESHOLD ) {
        leadVehicleVelocityAccelerationMutex.lock();

        if ( leadVehicleVelocityAccumulator == 0 ) {
            followingVehicleVelocityMutex.lock();
            leadVehicleVelocityAccumulator = followingVehicleVelocityAccumulator;
            followingVehicleVelocityMutex.unlock();
        } else {
            if ( leadVehicleAlignedAccumulator > LEAD_VEHICLE_ALIGNED_THRESHOLD ) {
                leadVehicleAcceleration = (leadVehicleVelocity - leadVehicleVelocityAccumulator) / dt;

                int t = leadVehicleVelocityAccelerationTimer.restart();
                UPDATE_ACCUMULATOR( leadVehicleVelocityAccumulator, leadVehicleVelocity, t );
                UPDATE_ACCUMULATOR( leadVehicleAccelerationAccumulator, leadVehicleAcceleration, t );
            } else {
                /* Assume constant velocity; acceleration was already set to zero */
            }
        }

        leadVehicleVelocityAccelerationMutex.unlock();
    } else {
        /* There's nothing to update */
    }
    leadVehiclePresentAlignedMutex.unlock();

    emit gotTimeHeadway( timeHeadway );
    emit gotDistanceHeadway( distanceHeadway );
    emit gotLeadVehicleVelocity( leadVehicleVelocityAccumulator );
    emit gotLeadVehicleAcceleration( leadVehicleAccelerationAccumulator );
    emit gotFollowingVehicleVelocity( followingVehicleVelocity );
    emit gotFollowingVehicleAcceleration( followingVehicleAcceleration );
}

void HeadwayKalmanFilter::predict() {
    dtMutex.lock();
    dt = (float) timeElapsed.restart() / 1000;

    kalmanFilter.transitionMatrix.at<float>( 0, 2 ) = -dt;
    kalmanFilter.transitionMatrix.at<float>( 0, 3 ) = dt;

    dtMutex.unlock();

    kalmanFilter.predict();
}


