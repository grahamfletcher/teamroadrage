#include <QDebug>
#include <QMutexLocker>
#include <QTimer>

#include "globals.h"
#include "Controller.h"

#define BASE_SAFE_TIME_HEADWAY 1

Controller::Controller( QObject *parent, GUIController *guiController ) : QObject( parent ), guiController( guiController ) {
    /* Start off with the minimum safe time headway */
    currentSafeTimeHeadway = BASE_SAFE_TIME_HEADWAY;

    /* Initialize devices */
    arduinoDevice =       new ArduinoDevice( this );
    androidDevice =       new AndroidDevice( NULL, arduinoDevice );
    obdDevice =           new OBDDevice( this );

    /* Initialize sensors */
    distanceSensor =      new DistanceSensor( NULL );
    humiditySensor =      new HumiditySensor( NULL, arduinoDevice );
    rainSensor =          new RainSensor( NULL, arduinoDevice );
    speedSensor =         new SpeedSensor( NULL, obdDevice );
    temperatureSensor =   new TemperatureSensor( NULL, arduinoDevice );

    /* Initialize other objects */
    headwayKalmanFilter = new HeadwayKalmanFilter( this );
    reactionTimeTracker = new ReactionTimeTracker( this );

    /* Initialize vision objects */
    capture             = new Capture( NULL );
    laneDetector        = new LaneDetector( this );
    vehicleDetector     = new VehicleDetector( this );
}

Controller::~Controller() {

}

void Controller::connectSignalsAndSlots() {
    /* Controller slots */
    QObject::connect( rainSensor,           SIGNAL( gotRainPresent( bool ) ),
                      this,                 SLOT( updateRainPresent( bool ) ) );
    QObject::connect( humiditySensor,       SIGNAL( gotRainPresent( bool ) ),
                      this,                 SLOT( updateRainPresent( bool ) ) );
    QObject::connect( temperatureSensor,    SIGNAL( gotIcePresent( bool ) ),
                      this,                 SLOT( updateIcePresent( bool ) ) );
    QObject::connect( reactionTimeTracker,  SIGNAL( gotReactionTime( float ) ),
                      this,                 SLOT( updateReactionTime( float ) ) );

    /* Android slots */
    QObject::connect( temperatureSensor,    SIGNAL( gotIcePresent( bool ) ),
                      androidDevice,        SLOT( updateIcePresent( bool ) ) );
    QObject::connect( headwayKalmanFilter,  SIGNAL( gotLeadVehicleVelocity( float ) ),
                      androidDevice,        SLOT( updateLeadVehicleVelocity( float ) ) );
    QObject::connect( headwayKalmanFilter,  SIGNAL( gotTimeHeadway( float ) ),
                      androidDevice,        SLOT( updateTimeHeadway( float ) ) );
    QObject::connect( rainSensor,           SIGNAL( gotRainPresent( bool ) ),
                      androidDevice,        SLOT( updateRainPresent( bool ) ) );
    QObject::connect( humiditySensor,       SIGNAL( gotRainPresent( bool ) ),
                      androidDevice,        SLOT( updateRainPresent( bool ) ) );
    QObject::connect( reactionTimeTracker,  SIGNAL( gotReactionTime( float ) ),
                      androidDevice,        SLOT( updateReactionTime( float ) ) );
    QObject::connect( this,                 SIGNAL( gotSafeTimeHeadway( float ) ),
                      androidDevice,        SLOT( updateSafeTimeHeadway( float ) ) );


    /* HeadwayKalmanFilter slots */
    QObject::connect( vehicleDetector,      SIGNAL( gotLeadVehiclePresent( bool, bool ) ),
                      headwayKalmanFilter,  SLOT( updateLeadVehiclePresent( bool, bool ) ) );
    QObject::connect( distanceSensor,       SIGNAL( gotReading( float ) ),
                      headwayKalmanFilter,  SLOT( updateDistanceReading( float ) ) );
    QObject::connect( speedSensor,          SIGNAL( gotReading( float ) ),
                      headwayKalmanFilter,  SLOT( updateSpeedReading( float ) ) );

    /* ReactionTimeTracker slots */
    QObject::connect( headwayKalmanFilter,  SIGNAL( gotTimeHeadway( float ) ),
                      reactionTimeTracker,  SLOT( updateTimeHeadway( float ) ) );
    QObject::connect( headwayKalmanFilter,  SIGNAL( gotLeadVehicleAcceleration( float ) ),
                      reactionTimeTracker,  SLOT( updateLeadVehicleAcceleration( float ) ) );
    QObject::connect( headwayKalmanFilter,  SIGNAL( gotFollowingVehicleAcceleration( float ) ),
                      reactionTimeTracker,  SLOT( updateFollowingVehicleVelocity( float ) ) );

    /* GUIController slots */
    QObject::connect( this,                 SIGNAL( gotSafeTimeHeadway( float ) ),
                      guiController,        SLOT( updateSafeTimeHeadway( float ) ) );
    QObject::connect( this,                 SIGNAL( gotIcePresent( bool ) ),
                      guiController,        SLOT( updateIcePresent( bool ) ) );
    QObject::connect( this,                 SIGNAL( gotRainPresent( bool ) ),
                      guiController,        SLOT( updateRainPresent( bool ) ) );
    QObject::connect( distanceSensor,       SIGNAL( gotReading( float ) ),
                      guiController,        SLOT( updateRawDistance( float ) ) );
    QObject::connect( humiditySensor,       SIGNAL( gotReading( float ) ),
                      guiController,        SLOT( updateHumidity( float ) ) );
    QObject::connect( rainSensor,           SIGNAL( gotReading( float ) ),
                      guiController,        SLOT( updateRainSensorVoltage( float ) ) );
    QObject::connect( rainSensor,           SIGNAL( gotRainPresent( bool ) ),
                      guiController,        SLOT( updateRainPresent( bool ) ) );
    QObject::connect( speedSensor,          SIGNAL( gotReading( float ) ),
                      guiController,        SLOT( updateRawFollowingVehicleVelocity( float ) ) );
    QObject::connect( temperatureSensor,    SIGNAL( gotReading( float ) ),
                      guiController,        SLOT( updateTemperature( float ) ) );
    QObject::connect( headwayKalmanFilter,  SIGNAL( gotTimeHeadway( float ) ),
                      guiController,        SLOT( updateTimeHeadway( float ) ) );
    QObject::connect( headwayKalmanFilter,  SIGNAL( gotDistanceHeadway( float ) ),
                      guiController,        SLOT( updateDistanceHeadway( float ) ) );
    QObject::connect( headwayKalmanFilter,  SIGNAL( gotLeadVehicleVelocity( float ) ),
                      guiController,        SLOT( updateLeadVehicleVelocity() ) );
    QObject::connect( headwayKalmanFilter,  SIGNAL( gotFollowingVehicleVelocity( float ) ),
                      guiController,        SLOT( updateFollowingVehicleVelocity( float ) ) );
    QObject::connect( headwayKalmanFilter,  SIGNAL( gotFollowingVehicleAcceleration( float ) ),
                      guiController,        SLOT( updateFollowingVehicleVelocity( float ) ) );
    QObject::connect( reactionTimeTracker,  SIGNAL( gotReactionTime( float ) ),
                      guiController,        SLOT( updateReactionTime( float ) ) );

    /* Capture slots */

    /* LaneDetector slots */

    /* VehicleDetector slots */


    /* TODO: finish connecting vision objects */
}

void Controller::updateRainPresent( bool rain ) {
    QMutexLocker locker( &rainMutex );

    if ( rain ) {
        rainTimer.restart();

        if ( !rainPresent ) {
            /* Value changed */
            QMutexLocker locker( &currentSafeTimeHeadwayMutex );

            currentSafeTimeHeadway += 0.4;

            emit gotSafeTimeHeadway( currentSafeTimeHeadway );
            emit gotRainPresent( true );
        }
    } else if ( rainPresent && (rainTimer.elapsed() > 1200000) ) {    // 20 minutes
        /* Value changed, and the 20 minute timer expired */
        QMutexLocker locker( &currentSafeTimeHeadwayMutex );

        currentSafeTimeHeadway -= 0.4;

        emit gotSafeTimeHeadway( currentSafeTimeHeadway );
        emit gotRainPresent( false );
    }

    rainPresent = rain;
}

void Controller::updateIcePresent( bool ice ) {
    QMutexLocker locker( &icePresentMutex );

    if ( ice && !icePresent ) {
        QMutexLocker locker( &currentSafeTimeHeadwayMutex );

        currentSafeTimeHeadway += 1.0;

        icePresent = true;

        emit gotSafeTimeHeadway( currentSafeTimeHeadway );
        emit gotIcePresent( true );
    } else {
        /* If we detected ice earlier, it's probably still there */
    }
}

void Controller::updateReactionTime( float time ) {
    QMutexLocker locker( &reactionTimeMutex );

    if ( (time > 0.5) && (time < 3) ) {
        float difference = time - reactionTime;

        currentSafeTimeHeadway += difference;

        reactionTime = time;

        emit gotSafeTimeHeadway( currentSafeTimeHeadway );
    }
}

