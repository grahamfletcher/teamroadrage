#include <math.h>
#include <QDebug>
#include <QMutexLocker>
#include <QTimer>

#include "globals.h"
#include "AndroidDevice.h"

#define ALARM_INTERVAL 3500    // time in ms between issuing alarms
#define UPDATE_INTERVAL 250    // tim in ms between sending updates to the Android

AndroidDevice::AndroidDevice( QObject *parent, ArduinoDevice *arduinoDevice ) : QThread( parent ), arduinoDevice( arduinoDevice ) {
    shouldContinue = true;

    /* Start the alarm timer */
    timeElapsedSinceAlarm.start();

    /* Set correct affinity */
    //moveToThread( this );

    /* Start the accessory mode */
    unsigned char command = 'z';
    unsigned char result = 0;
    
    do {
        arduinoDevice->getReading( &command, sizeof( command ), &result, sizeof( result ) );
    } while ( result != 1 );

    timeElapsedSinceUpdate.start();
}

AndroidDevice::~AndroidDevice() {
    qDebug() << "~AndroidDevice()";
    shouldContinue = false;
    wait();
}

void AndroidDevice::updateIcePresent( bool ice ) {
    QMutexLocker locker( &icePresentMutex );

    qDebug() << "updateIcePresent()" << ice;

    icePresent = ice;
}

void AndroidDevice::updateLeadVehicleVelocity( float velocity ) {
    QMutexLocker locker( &leadVehicleVelocityMutex );

    qDebug() << "updateLeadVehicleVelocity()" << velocity;

    /* Min: 0, Max: 254 */
    leadVehicleVelocity = velocity < 0 ? 0 : (velocity > 254 ? 254 : velocity);
}

void AndroidDevice::updateRainPresent( bool rain ) {
    QMutexLocker locker( &rainPresentMutex );

    qDebug() << "updateRainPresent()" << rain;

    rainPresent = rain;
}

void AndroidDevice::updateReactionTime( float time ) {
    QMutexLocker locker( &reactionTimeMutex );

    qDebug() << "updateReactionTime()" << time;

    /* Min: 0.1, Max: 25.4 */
    reactionTime = time < 0 ? 0.1 : (time > 25.4 ? 25.4 : time);
}

void AndroidDevice::updateSafeTimeHeadway( float headway ) {
    QMutexLocker locker( &safeTimeHeadwayMutex );

    qDebug() << "updateSafeTimeHeadway()" << headway;

    /* Min: 0, Max: 25.4 */
    safeTimeHeadway = headway < 0 ? 0 : (headway > 25.4 ? 25.4 : headway);
}

void AndroidDevice::updateTimeHeadway( float headway ) {
    QMutexLocker locker( &timeHeadwayMutex );

    qDebug() << "updateTimeHeadway()" << headway;

    /* Min: 0, Max: 25.4 */
    timeHeadway = headway < 0 ? 0 : (headway > 25.4 ? 25.4 : headway);
}

void AndroidDevice::sendDataToAndroid() {
    /*
     * buf[0] = 'c'; let Arduino know we're sending a command
     * buf[1] = (bool); ice present
     * buf[2] = (bool); rain present
     * buf[3] = (bool); play alarm
     * buf[4] = (int); percent for progress bar
     * buf[5] = (float); time headway * 10
     * buf[6] = (int); lead vehicle velocity * 5
     */
    unsigned char buf[7] = { 'c', '0', '0', '0', '0', '0', '0' };    // 'c' lets the Arduino know the command is destined for Android
    unsigned char result = 0;

    int t;

    while( shouldContinue ) {
        t = timeElapsedSinceUpdate.elapsed();
        
        qDebug() << "t = " << t;
        if ( t < UPDATE_INTERVAL ) {
            usleep( 1000 * (UPDATE_INTERVAL - t) );    // don't update too quickly
        }

        /* Set ice presence */
        icePresentMutex.lock();
        buf[1] = icePresent ? (char) 1 : (char) 0;
        icePresentMutex.unlock();
        
        /* Set rain presence */
        rainPresentMutex.lock();
        buf[2] = rainPresent ? (char) 1 : (char) 0;
        rainPresentMutex.unlock();

        /* Determine alarm condition */
        safeTimeHeadwayMutex.lock();
        timeHeadwayMutex.lock();
        if ( (timeHeadway < safeTimeHeadway) &&
             (timeElapsedSinceAlarm.elapsed() > ALARM_INTERVAL) ) {
            buf[3] = (char) 1;
            timeElapsedSinceAlarm.restart();    // don't sound the alarm too often
        } else {
            buf[3] = (char) 0;
        }
        safeTimeHeadwayMutex.unlock();

        /* Determine progress bar percent */
        reactionTimeMutex.lock();
        buf[4] = (char) (100 - (100 / sqrt( 25.4 - reactionTime )) * sqrt( timeHeadway - reactionTime ));
        reactionTimeMutex.unlock();

        /* Set time headway (multiply by 10) */
        buf[5] = (char) (timeHeadway * 10);
        timeHeadwayMutex.unlock();

        /* Set lead vehicle velocity */
        leadVehicleVelocityMutex.lock();
        buf[6] = (char) (leadVehicleVelocity * 5);
        leadVehicleVelocityMutex.unlock();

        arduinoDevice->getReading( buf, sizeof( buf ), &result, sizeof( result ) );

        /* If sending the command was successful, restart the timer */
        if ( result == 1 ) {
            qDebug() << "RESTARTING";
            timeElapsedSinceUpdate.restart();
        } else {
            /* We should just immediately send a new command next time */
        }
    }

    exit( 0 );
}

void AndroidDevice::run() {
    QTimer::singleShot( 100, this, SLOT( sendDataToAndroid() ) );

    /* Start the event loop */
    exec();
}

