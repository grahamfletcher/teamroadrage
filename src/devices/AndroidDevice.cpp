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
	int t;
	unsigned char result = 0;
	unsigned char command[] = {'c', 0, 0, 0, 0, 0, 0};

    /* Start the accessory mode */
    unsigned char z_command = 'z';
    arduinoDevice->getReading( &z_command, sizeof( z_command ), &result, sizeof( result ), 550000 );    //just assumed it worked

    result = 0; //reset the result

    while( shouldContinue ) {
        t = timeElapsedSinceUpdate.elapsed();
        
        if ( t < UPDATE_INTERVAL ) {
            usleep( 1000 * (UPDATE_INTERVAL - t) );    // don't update too quickly
        }

        /* Set ice presence */
        icePresentMutex.lock();
        command[1] = icePresent ? 1 : 0;
		
        icePresentMutex.unlock();
        
        /* Set rain presence */
        rainPresentMutex.lock();
        command[2] = rainPresent ? 1 : 0;
		
        rainPresentMutex.unlock();

        /* Determine alarm condition */
        safeTimeHeadwayMutex.lock();
        timeHeadwayMutex.lock();
        if ( (timeHeadway < safeTimeHeadway) &&
             (timeElapsedSinceAlarm.elapsed() > ALARM_INTERVAL) ) {    // don't sound the alarm too often
            command[3] = 1;
            timeElapsedSinceAlarm.restart();    // we sounded the alarm; reset the timer
        } else{
            command[3] = 0;
		}
        safeTimeHeadwayMutex.unlock();

        /* Determine progress bar percent */
        reactionTimeMutex.lock();
        command[4] = qRound( 100 - (100 / sqrt( 25.4 - reactionTime )) * sqrt( timeHeadway - reactionTime ) );
        reactionTimeMutex.unlock();

        /* Set time headway (multiply by 10) */
        command[5] = qRound( timeHeadway * 10 );
        timeHeadwayMutex.unlock();

        /* Set lead vehicle velocity */
        leadVehicleVelocityMutex.lock();
        command[6] = qRound( leadVehicleVelocity * 5 );
        leadVehicleVelocityMutex.unlock();


        arduinoDevice->getReading( command, sizeof( command ), &result, sizeof( result ), 20000 );

        /* If sending the command was successful, restart the timer */
        if ( result == 1 ) {
            timeElapsedSinceUpdate.restart();
        } else {
            usleep( UPDATE_INTERVAL / 2 );
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

