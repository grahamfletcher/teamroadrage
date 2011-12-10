#include <QDebug>
#include <QMutexLocker>

#include "HeadwayKalmanFilter.h"

HeadwayKalmanFilter::HeadwayKalmanFilter( QObject *parent = 0 ) : QObject( parent ) {
    /* Start out assuming there's a lead vehicle? */
    leadVehiclePresent = true;

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
    kalmanFilter.processNoiseCov = *(cv::Mat_<float>( 4, 4 ) << 3, 0,   0,   0,
                                                                0, 1,   0,   0,
                                                                0, 0, 0.8,   0,
                                                                0, 0,   0, 0.6);

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
    QMutexLocker locker( &leadVehicleMutex );

    leadVehiclePresent = vehiclePresent;

    if ( !vehiclePresent ) {
        /* If there's no vehicle, the accumulator should be reset */
        leadVehicleVelocityAccumulator = 0;
        leadVehicleAligned = false;
        return;
    } else {
        leadVehicleAligned = vehicleAligned;
    }
}

void HeadwayKalmanFilter::updateDistanceReading( float distance ) {
    QMutexLocker locker1( &leadVehicleMutex );

    if ( !(leadVehiclePresent && leadVehicleAligned) ) {
        /* If there's no lead vehicle, or it's not aligned, the measurement is useless */
        return;
    }

    QMutexLocker locker2( &kalmanFilterMutex );

    predict();

    kalmanFilter.measurementMatrix = distanceH;
    kalmanFilter.measurementNoiseCov = distanceR;

    distanceMeasurement.at<float>( 0, 0 ) = distance;

    kalmanFilter.correct( distanceMeasurement );

    emitSignals();
}

void HeadwayKalmanFilter::updateSpeedReading( const float speed ) {
    QMutexLocker locker( &kalmanFilterMutex );

    predict();

    kalmanFilter.measurementMatrix = speedH;
    kalmanFilter.measurementNoiseCov = speedR;

    speedMeasurement.at<float>( 0, 0 ) = speed;

    kalmanFilter.correct( speedMeasurement );

    emitSignals();
}

void HeadwayKalmanFilter::emitSignals() {
    float currentDistance  = kalmanFilter.statePost.at<float>( 0, 0 );
    float previousDistance = kalmanFilter.statePost.at<float>( 1, 0 );
    float currentVelocity  = kalmanFilter.statePost.at<float>( 2, 0 );
    float previousVelocity = kalmanFilter.statePost.at<float>( 3, 0 );

    float timeHeadway = currentVelocity < 1 ? INT_MAX : currentDistance / currentVelocity;
    float distanceHeadway = kalmanFilter.statePost.at<float>( 0, 0 );
    float leadVehicleVelocity = ((currentDistance - previousDistance) / dt) + ((currentVelocity + previousVelocity) / 2);
    float leadVehicleAcceleration = 0;    // set below
    float followingVehicleVelocity = kalmanFilter.statePost.at<float>( 2, 0 ) < 1 ? 0 : kalmanFilter.statePost.at<float>( 2, 0 );
    float followingVehicleAcceleration = kalmanFilter.statePost.at<float>( 2, 0 ) < 1 ? 0 : (currentVelocity - previousVelocity) / dt;

    /* Handle all of the tricky stuff for the lead vehicle */
    leadVehicleMutex.lock();
    if ( leadVehiclePresent ) {
        /* We use an accumulator to smooth the data for the lead vehicle velocity; it's really noisy */
        if ( leadVehicleVelocityAccumulator == 0 ) {
            leadVehicleVelocityAccumulator = leadVehicleVelocity;
        } else {
            if ( leadVehicleAligned ) {
                leadVehicleAcceleration = (leadVehicleVelocity - leadVehicleVelocityAccumulator) / dt;
                leadVehicleVelocityAccumulator = 0.9 * leadVehicleVelocityAccumulator + 0.1 * leadVehicleVelocity;    // TODO: are these good values?
            } else {
                /* Assume constant velocity; acceleration was already set to zero */
            }
        }
        emit gotLeadVehicleVelocity( leadVehicleVelocityAccumulator );
        emit gotLeadVehicleAcceleration( leadVehicleAcceleration );
    } else {
        /* There's nothing to update */
    }
    leadVehicleMutex.unlock();

    emit gotTimeHeadway( timeHeadway );
    emit gotDistanceHeadway( distanceHeadway );
    emit gotLeadVehicleVelocity( leadVehicleVelocityAccumulator );
    emit gotLeadVehicleAcceleration( leadVehicleAcceleration );
    emit gotFollowingVehicleVelocity( followingVehicleVelocity );
    emit gotFollowingVehicleAcceleration( followingVehicleAcceleration );
}

void HeadwayKalmanFilter::predict() {
    dt = (float) timeElapsed.elapsed() / 1000;

    kalmanFilter.transitionMatrix.at<float>( 0, 2 ) = -dt;
    kalmanFilter.transitionMatrix.at<float>( 0, 3 ) = dt;

    timeElapsed.restart();

    kalmanFilter.predict();
}


