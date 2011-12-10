#ifndef GUICONTROLLER_H
#define GUICONTROLLER_H

#include <opencv2/core/core.hpp>
#include <QObject>
#include <QSettings>

#include "InspectorWindow.h"
#include "VideoWindow.h"

class GUIController : public QObject {
    Q_OBJECT

public:
    GUIController( QObject *parent );
    ~GUIController();

public slots:
    /* Controller : Controller */
    void updateSafeTimeHeadway( float time );    // gotSafeTimeHeadway()
    void updateIcePresent( bool ice );    // gotIcePresent()
    void updateRainPresent( bool rain );    // gotRainPresent()

    /* DistanceSensor : Controller */
    void updateRawDistance( float distance );    // gotReading()

    /* HumiditySensor : Controller */
    void updateHumidity( float humidity );    // gotReading()

    /* RainSensor : Controller */
    void updateRainSensorVoltage( float voltage );    // gotReading()

    /* SpeedSensor : Controller */
    void updateRawFollowingVehicleVelocity( float velocity );    // gotReading()

    /* TemperatureSensor : Controller */
    void updateTemperature( float temperature );    // gotReading()

    /* HeadwayKalmanFilter : Controller */
    void updateTimeHeadway( float time );    // gotTimeHeadway()
    void updateDistanceHeadway( float distance );    // gotDistanceHeadway()
    void updateLeadVehicleVelocity( float velocity );    // gotLeadVehicleVelocity()
    void updateFollowingVehicleVelocity( float velocity );    // gotFollowingVehicleVelocity()
    void updateFollowingVehicleAcceleration( float acceleration );    // gotFollowingVehicleAcceleration()

    /* ReactionTimeTracker : Controller */
    void updateReactionTime( float time );    // gotReactionTime()

    /* Capture : Controller */
    void updateCurrentFrame( cv::Mat &frame, int type );

private:
    InspectorWindow *inspectorWindow;
    VideoWindow *videoWindow;
};
#endif

