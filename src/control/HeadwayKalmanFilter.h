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
    float dt;

    QMutex leadVehicleMutex;
    bool leadVehiclePresent;
    bool leadVehicleAligned;

    QMutex kalmanFilterMutex;
    cv::KalmanFilter kalmanFilter;

    cv::Mat distanceH;
    cv::Mat speedH;

    cv::Mat distanceR;
    cv::Mat speedR;

    cv::Mat distanceMeasurement;
    cv::Mat speedMeasurement;

    float leadVehicleVelocityAccumulator;
};
#endif

