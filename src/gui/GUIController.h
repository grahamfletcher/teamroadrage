#ifndef GUICONTROLLER_H
#define GUICONTROLLER_H

#include <QObject>
#include <QSettings>

#include "globals.h"
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
    void updateCurrentFrame( _Mat frame, int type, _QReadWriteLock lock );

private:
    InspectorWindow *inspectorWindow;
    VideoWindow *videoWindow;
};
#endif

