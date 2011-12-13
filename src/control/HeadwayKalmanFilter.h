#ifndef HEADWAYKALMANFILTER_H
#define HEADWAYKALMANFILTER_H

#include <opencv2/core/core.hpp>
#include <opencv2/video/tracking.hpp>
#include <QMutex>
#include <QObject>
#include <QTime>
#include <sys/timeb.h>
#include <sys/types.h>

#include "DistanceSensor.h"
#include "SpeedSensor.h"

class HeadwayKalmanFilter : public QObject {
    Q_OBJECT

public:
    HeadwayKalmanFilter( QObject *parent );
    ~HeadwayKalmanFilter();

signals:
    void gotTimeHeadway( float time );
    void gotDistanceHeadway( float distance );
    void gotLeadVehicleVelocity( float velocity );
    void gotLeadVehicleAcceleration( float acceleration );
    void gotFollowingVehicleVelocity( float velocity );
    void gotFollowingVehicleAcceleration( float acceleration );

public slots:
    /* VehicleDetector : Controller */
    void updateLeadVehiclePresent( bool vehiclePresent, bool vehicleAligned );    // gotLeadVehiclePresent()

    /* DistanceSensor : Controller */
    void updateDistanceReading( float distance );    // gotReading()

    /* SpeedSensor : Controller */
    void updateSpeedReading( float speed );    // gotReading()

private:
    void emitSignals();
    void predict();

    QTime timeElapsed;

    QMutex dtMutex;
    float dt;

    QMutex kalmanFilterMutex;
    cv::KalmanFilter kalmanFilter;

    cv::Mat distanceH;
    cv::Mat speedH;

    cv::Mat distanceR;
    cv::Mat speedR;

    cv::Mat distanceMeasurement;
    cv::Mat speedMeasurement;


    /* The following timers and accumulators are to assiste with smoothing */

    QMutex leadVehiclePresentAlignedMutex;
    QTime leadVehiclePresentAlignedTimer;
    float leadVehiclePresentAccumulator;    // will be interpreted as "true" if > LEAD_VEHICLE_PRESENT_THRESHOLD
    float leadVehicleAlignedAccumulator;    // will be interpreted as "true" if > LEAD_VEHICLE_ALIGNED_THRESHOLD

    //QMutex timeHeadwayMutex;
    //QTime timeHeadwayTimer;
    //float timeHeadwayAccumulator;

    QMutex distanceHeadwayMutex;
    QTime distanceHeadwayTimer;
    float distanceHeadwayAccumulator;

    QMutex leadVehicleVelocityAccelerationMutex;
    QTime leadVehicleVelocityAccelerationTimer;
    float leadVehicleVelocityAccumulator;
    float leadVehicleAccelerationAccumulator;

    QMutex followingVehicleVelocityMutex;
    QTime followingVehicleVelocityTimer;
    float followingVehicleVelocityAccumulator;

    //QMutex followingVehicleAccelerationMutex;
    //QTime followingVehicleAccelerationTimer;
    //float followingVehicleAccelerationAccumulator;
};
#endif

