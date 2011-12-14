#include <QDebug>
#include <QMutexLocker>
#include <QTimer>

#include "globals.h"
#include "Controller.h"

#define BASE_SAFE_TIME_HEADWAY 1
#define RAIN_HEADWAY_COMPENSATION 0.4
#define ICE_HEADWAY_COMPENSATION 1.5

Controller::Controller( QObject *parent ) : QObject( parent ) {
    guiController = new GUIController( parent );

    /* Start off with the minimum safe time headway */
    currentSafeTimeHeadway = BASE_SAFE_TIME_HEADWAY;

    /* Initialize devices */
    arduinoDevice =       new ArduinoDevice( this );
    androidDevice =       new AndroidDevice( NULL, arduinoDevice );
    cameraDevice =        new CameraDevice( this );
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
    laneDetector        = new LaneDetector( NULL, cameraDevice );
    vehicleDetector     = new VehicleDetector( NULL );

    /* Set correct thread affinities */
    moveThreads();

    /* Connect a zillion signals and slots to make everything work */
    connectSignalsAndSlots();

    QTimer::singleShot( 500, this, SLOT( startThreads() ) );
}

Controller::~Controller() {
    androidDevice->shouldContinue =     false;
    distanceSensor->shouldContinue =    false;
    humiditySensor->shouldContinue =    false;
    rainSensor->shouldContinue =        false;
    speedSensor->shouldContinue =       false;
    temperatureSensor->shouldContinue = false;
    laneDetector->shouldContinue =      false;
    vehicleDetector->shouldContinue =   false;

    androidDevice->wait();
    distanceSensor->wait();
    humiditySensor->wait();
    rainSensor->wait();
    speedSensor->wait();
    temperatureSensor->wait();
    laneDetector->wait();
    vehicleDetector->wait();
}

void Controller::startThreads() {
    androidDevice->start();
    distanceSensor->start();
    humiditySensor->start();
    rainSensor->start();
    speedSensor->start();
    temperatureSensor->start();
    laneDetector->start();
    vehicleDetector->start();
}

void Controller::moveThreads() {
    androidDevice->moveToThread( androidDevice );
    distanceSensor->moveToThread( distanceSensor );
    humiditySensor->moveToThread( humiditySensor );
    rainSensor->moveToThread( rainSensor );
    speedSensor->moveToThread( speedSensor );
    temperatureSensor->moveToThread( temperatureSensor );
    laneDetector->moveToThread( laneDetector );
    vehicleDetector->moveToThread( vehicleDetector );
}

void Controller::connectSignalsAndSlots() {
    /* Controller slots */
    QObject::connect( rainSensor,           SIGNAL( gotRainPresent( bool ) ),
                      this,                 SLOT( updateRainPresent( bool ) ),
                      Qt::DirectConnection );

    //QObject::connect( humiditySensor,       SIGNAL( gotRainPresent( bool ) ),
    //                  this,                 SLOT( updateRainPresent( bool ) ),
    //                  Qt::DirectConnection );

    QObject::connect( temperatureSensor,    SIGNAL( gotIcePresent( bool ) ),
                      this,                 SLOT( updateIcePresent( bool ) ),
                      Qt::DirectConnection );

    QObject::connect( reactionTimeTracker,  SIGNAL( gotReactionTime( float ) ),
                      this,                 SLOT( updateReactionTime( float ) ),
                      Qt::DirectConnection );

    /* Android slots */
    QObject::connect( temperatureSensor,    SIGNAL( gotIcePresent( bool ) ),
                      androidDevice,        SLOT( updateIcePresent( bool ) ),
                      Qt::DirectConnection );

    QObject::connect( headwayKalmanFilter,  SIGNAL( gotLeadVehicleVelocity( float ) ),
                      androidDevice,        SLOT( updateLeadVehicleVelocity( float ) ),
                      Qt::DirectConnection );

    QObject::connect( headwayKalmanFilter,  SIGNAL( gotTimeHeadway( float ) ),
                      androidDevice,        SLOT( updateTimeHeadway( float ) ),
                      Qt::DirectConnection );
    //QObject::connect( distanceSensor,       SIGNAL( gotReading( float ) ),
    //                  androidDevice,        SLOT( updateTimeHeadway( float ) ),
    //                  Qt::DirectConnection );
    //QObject::connect( speedSensor,          SIGNAL( gotReading( float ) ),
    //                  androidDevice,        SLOT( updateLeadVehicleVelocity( float ) ),
    //                  Qt::DirectConnection );

    QObject::connect( rainSensor,           SIGNAL( gotRainPresent( bool ) ),
                      androidDevice,        SLOT( updateRainPresent( bool ) ),
                      Qt::DirectConnection );

    QObject::connect( humiditySensor,       SIGNAL( gotRainPresent( bool ) ),
                      androidDevice,        SLOT( updateRainPresent( bool ) ),
                      Qt::DirectConnection );

    QObject::connect( reactionTimeTracker,  SIGNAL( gotReactionTime( float ) ),
                      androidDevice,        SLOT( updateReactionTime( float ) ),
                      Qt::DirectConnection );

    QObject::connect( this,                 SIGNAL( gotSafeTimeHeadway( float ) ),
                      androidDevice,        SLOT( updateSafeTimeHeadway( float ) ),
                      Qt::DirectConnection );

    /* HeadwayKalmanFilter slots */
    QObject::connect( vehicleDetector,      SIGNAL( gotLeadVehiclePresent( bool, bool ) ),
                      headwayKalmanFilter,  SLOT( updateLeadVehiclePresent( bool, bool ) ),
                      Qt::DirectConnection );

    QObject::connect( distanceSensor,       SIGNAL( gotReading( float ) ),
                      headwayKalmanFilter,  SLOT( updateDistanceReading( float ) ),
                      Qt::DirectConnection );

    QObject::connect( speedSensor,          SIGNAL( gotReading( float ) ),
                      headwayKalmanFilter,  SLOT( updateSpeedReading( float ) ),
                      Qt::DirectConnection );;

    /* ReactionTimeTracker slots */
    QObject::connect( headwayKalmanFilter,  SIGNAL( gotTimeHeadway( float ) ),
                      reactionTimeTracker,  SLOT( updateTimeHeadway( float ) ),
                      Qt::DirectConnection );

    QObject::connect( headwayKalmanFilter,  SIGNAL( gotLeadVehicleAcceleration( float ) ),
                      reactionTimeTracker,  SLOT( updateLeadVehicleAcceleration( float ) ),
                      Qt::DirectConnection );

    QObject::connect( headwayKalmanFilter,  SIGNAL( gotFollowingVehicleAcceleration( float ) ),
                      reactionTimeTracker,  SLOT( updateFollowingVehicleAcceleration( float ) ),
                      Qt::DirectConnection );;

    /* GUIController slots */
    QObject::connect( this,                 SIGNAL( gotSafeTimeHeadway( float ) ),
                      guiController,        SLOT( updateSafeTimeHeadway( float ) ),
                      Qt::DirectConnection );

    QObject::connect( temperatureSensor,    SIGNAL( gotIcePresent( bool ) ),
                      guiController,        SLOT( updateIcePresent( bool ) ),
                      Qt::DirectConnection );

    QObject::connect( rainSensor,           SIGNAL( gotRainPresent( bool ) ),
                      guiController,        SLOT( updateRainPresent( bool ) ),
                      Qt::DirectConnection );

    //QObject::connect( humiditySensor,      SIGNAL( gotRainPresent( bool ) ),
    //                  guiController,       SLOT( updateRainPresent( bool ) ),
    //                  Qt::DirectConnection );

    QObject::connect( distanceSensor,       SIGNAL( gotReading( float ) ),
                      guiController,        SLOT( updateRawDistance( float ) ),
                      Qt::DirectConnection );

    QObject::connect( humiditySensor,       SIGNAL( gotReading( float ) ),
                      guiController,        SLOT( updateHumidity( float ) ),
                      Qt::DirectConnection );

    QObject::connect( rainSensor,           SIGNAL( gotReading( float ) ),
                      guiController,        SLOT( updateRainSensorVoltage( float ) ),
                      Qt::DirectConnection );

    QObject::connect( rainSensor,           SIGNAL( gotRainPresent( bool ) ),
                      guiController,        SLOT( updateRainPresent( bool ) ),
                      Qt::DirectConnection );

    QObject::connect( speedSensor,          SIGNAL( gotReading( float ) ),
                      guiController,        SLOT( updateRawFollowingVehicleVelocity( float ) ),
                      Qt::DirectConnection );

    QObject::connect( temperatureSensor,    SIGNAL( gotReading( float ) ),
                      guiController,        SLOT( updateTemperature( float ) ),
                      Qt::DirectConnection );
    
    QObject::connect( headwayKalmanFilter,  SIGNAL( gotTimeHeadway( float ) ),
                      guiController,        SLOT( updateTimeHeadway( float ) ),
                      Qt::DirectConnection );

    QObject::connect( headwayKalmanFilter,  SIGNAL( gotDistanceHeadway( float ) ),
                      guiController,        SLOT( updateDistanceHeadway( float ) ),
                      Qt::DirectConnection );

    QObject::connect( headwayKalmanFilter,  SIGNAL( gotLeadVehicleVelocity( float ) ),
                      guiController,        SLOT( updateLeadVehicleVelocity( float ) ),
                      Qt::DirectConnection );

    QObject::connect( headwayKalmanFilter,  SIGNAL( gotFollowingVehicleVelocity( float ) ),
                      guiController,        SLOT( updateFollowingVehicleVelocity( float ) ),
                      Qt::DirectConnection );

    QObject::connect( headwayKalmanFilter,  SIGNAL( gotFollowingVehicleAcceleration( float ) ),
                      guiController,        SLOT( updateFollowingVehicleAcceleration( float ) ),
                      Qt::DirectConnection );

    QObject::connect( reactionTimeTracker,  SIGNAL( gotReactionTime( float ) ),
                      guiController,        SLOT( updateReactionTime( float ) ),
                      Qt::DirectConnection );

    QObject::connect( laneDetector,         SIGNAL( gotFrame( _Mat, int, _QReadWriteLock ) ),
                      guiController,        SLOT( updateCurrentFrame( _Mat, int, _QReadWriteLock ) ),
                      Qt::QueuedConnection );

    /* Capture slots */

    /* LaneDetector slots */

    /* VehicleDetector slots */


    /* TODO: finish connecting vision objects */
}

void Controller::updateRainPresent( bool rain ) {
    QMutexLocker locker( &rainMutex );

    if ( rain && !rainPresent ) {
        /* Value changed */
        QMutexLocker locker( &currentSafeTimeHeadwayMutex );

        currentSafeTimeHeadway += RAIN_HEADWAY_COMPENSATION;

        emit gotSafeTimeHeadway( currentSafeTimeHeadway );
    } else if ( !rain && rainPresent ) {
        /* Value changed */
        QMutexLocker locker( &currentSafeTimeHeadwayMutex );

        currentSafeTimeHeadway -= RAIN_HEADWAY_COMPENSATION;

        emit gotSafeTimeHeadway( currentSafeTimeHeadway );
    }

    rainPresent = rain;
}

void Controller::updateIcePresent( bool ice ) {
    QMutexLocker locker( &icePresentMutex );

    if ( ice && !icePresent ) {
        QMutexLocker locker( &currentSafeTimeHeadwayMutex );

        currentSafeTimeHeadway += ICE_HEADWAY_COMPENSATION;

        emit gotSafeTimeHeadway( currentSafeTimeHeadway );
    } else if ( !ice && icePresent ) {
        /* If we detected ice earlier, it's probably still there, but that's not the Controller's responsibility */
        QMutexLocker locker( &currentSafeTimeHeadwayMutex );

        currentSafeTimeHeadway -= ICE_HEADWAY_COMPENSATION;

        emit gotSafeTimeHeadway( currentSafeTimeHeadway );
    }

    icePresent = ice;
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

