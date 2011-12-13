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

    /* Start the update timer */
    timeElapsedSinceUpdate.start();
}

AndroidDevice::~AndroidDevice() {
    qDebug() << "~AndroidDevice()";
    shouldContinue = false;
    wait();
}

void AndroidDevice::updateIcePresent( bool ice ) {
    QMutexLocker locker( &icePresentMutex );

    icePresent = ice;
}

void AndroidDevice::updateLeadVehicleVelocity( float velocity ) {
    QMutexLocker locker( &leadVehicleVelocityMutex );

    leadVehicleVelocity = qBound( (float) 0, velocity, (float) 254 );
}

void AndroidDevice::updateRainPresent( bool rain ) {
    QMutexLocker locker( &rainPresentMutex );

    rainPresent = rain;
}

void AndroidDevice::updateReactionTime( float time ) {
    QMutexLocker locker( &reactionTimeMutex );

    reactionTime = qBound( (float) 0.1, time, (float) 25.4 );
}

void AndroidDevice::updateSafeTimeHeadway( float headway ) {
    QMutexLocker locker( &safeTimeHeadwayMutex );

    safeTimeHeadway = qBound( (float) 0, headway, (float) 25.4 );
}

void AndroidDevice::updateTimeHeadway( float headway ) {
    QMutexLocker locker( &timeHeadwayMutex );

    timeHeadway = qBound( (float) 0, headway, (float) 25.4 );
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

    /* Start the accessory mode */
    unsigned char command = 'z';
    unsigned char result = 0;
    
    do {
        arduinoDevice->getReading( &command, sizeof( command ), &result, sizeof( result ), 2000000 );
        //usleep( 100000 );
    } while ( result != 1 && shouldContinue );


    unsigned char result = 0;

    int t;

    while( shouldContinue ) {
        unsigned char buf[] = { 'c', 0, 0, 0, 0, 0, 0 };    // 'c' lets the Arduino know the command is destined for Android

        t = timeElapsedSinceUpdate.elapsed();
        
        if ( t < UPDATE_INTERVAL ) {
            usleep( 1000 * (UPDATE_INTERVAL - t) );    // don't update too quickly
        }

        /* Set ice presence */
        icePresentMutex.lock();
        if ( icePresent ) {
            buf[1]++;
        }
        icePresentMutex.unlock();
        
        /* Set rain presence */
        rainPresentMutex.lock();
        if ( rainPresent ) {
            buf[2]++;
        }
        rainPresentMutex.unlock();

        /* Determine alarm condition */
        safeTimeHeadwayMutex.lock();
        timeHeadwayMutex.lock();
        if ( (timeHeadway < safeTimeHeadway) &&
             (timeElapsedSinceAlarm.elapsed() > ALARM_INTERVAL) ) {    // don't sound the alarm too often
            buf[3]++;
            timeElapsedSinceAlarm.restart();    // we sounded the alarm; reset the timer
        }
        safeTimeHeadwayMutex.unlock();

        /* Determine progress bar percent */
        reactionTimeMutex.lock();
        buf[4] += (100 - (100 / sqrt( 25.4 - reactionTime )) * sqrt( timeHeadway - reactionTime ));
        reactionTimeMutex.unlock();

        /* Set time headway (multiply by 10) */
        buf[5] += (timeHeadway * 10);
        timeHeadwayMutex.unlock();

        /* Set lead vehicle velocity */
        leadVehicleVelocityMutex.lock();
        buf[6] += (leadVehicleVelocity * 5);
        leadVehicleVelocityMutex.unlock();

        qDebug() << buf[0] << (unsigned int) buf[1] << (int) buf[2] << (int) buf[3] << (int) buf[4] << (int) buf[5] << (int) buf[6];

        if ( buf[1] == 2 ) { buf[1] = 1; }
        if ( buf[2] == 2 ) { buf[2] = 1; }
        if ( buf[3] == 2 ) { buf[3] = 1; }

        arduinoDevice->getReading( &buf[0], sizeof( buf ), &result, sizeof( result ), 20000 );

        /* If sending the command was successful, restart the timer */
        if ( result == 1 ) {
            timeElapsedSinceUpdate.restart();
        } else {
            /* We should just immediately send a new command next time */
        }
    }

    qDebug() << "exit AndroidDevice";
    exit( 0 );
}

void AndroidDevice::run() {
    QTimer::singleShot( 100, this, SLOT( sendDataToAndroid() ) );

    /* Start the event loop */
    exec();
}

