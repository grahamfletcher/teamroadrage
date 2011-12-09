#include <math.h>
#include <QDebug>
#include <QMutexLocker>
#include <QTimer>

#include "globals.h"
#include "AndroidDevice.h"

#define ALARM_INTERVAL 5000    // in ms

AndroidDevice::AndroidDevice( QObject *parent, ArduinoDevice *arduinoDevice ) : QThread( parent ), arduinoDevice( arduinoDevice ) {
    /* Start the alarm timer */
    timeElapsedSinceAlarm.start();

    /* Set correct affinity */
    moveToThread( this );

    /* Start the thread */
    start();
}

AndroidDevice::~AndroidDevice() {

}

void AndroidDevice::updateIcePresent( bool ice ) {
    QMutexLocker locker( &icePresentMutex );

    icePresent = ice;
}

void AndroidDevice::updateLeadVehicleVelocity( float velocity ) {
    QMutexLocker locker( &leadVehicleVelocityMutex );

    /* Min: 0, Max: 254 */
    leadVehicleVelocity = velocity < 0 ? 0 : (velocity > 254 ? 254 : velocity);
}

void AndroidDevice::updateRainPresent( bool rain ) {
    QMutexLocker locker( &rainPresentMutex );

    rainPresent = rain;
}

void AndroidDevice::updateReactionTime( float time ) {
    QMutexLocker locker( &reactionTimeMutex );

    /* Min: 0.1, Max: 25.4 */
    reactionTime = time < 0 ? 0.1 : (time > 25.4 ? 25.4 : time);
}

void AndroidDevice::updateSafeTimeHeadway( float headway ) {
    QMutexLocker locker( &safeTimeHeadwayMutex );

    /* Min: 0, Max: 25.4 */
    safeTimeHeadway = headway < 0 ? 0 : (headway > 25.4 ? 25.4 : headway);
}

void AndroidDevice::updateTimeHeadway( float headway ) {
    QMutexLocker locker( &timeHeadwayMutex );

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
     * buf[6] = (int); lead vehicle velocity
     */
    char buf[7];
    buf[0] = 'c';    // let Arduino know the command is destined for Android

    while( 1 ) {
        usleep( 100000 );    // wait 100 ms

        /* Set ice presence */
        icePresentMutex.lock();
        buf[1] = icePresent ? 1 : 0;
        icePresentMutex.unlock();
        
        /* Set rain presence */
        rainPresentMutex.lock();
        buf[2] = rainPresent ? 1 : 0;
        rainPresentMutex.unlock();

        /* Determine alarm condition */
        safeTimeHeadwayMutex.lock();
        timeHeadwayMutex.lock();
        if ( (timeHeadway < safeTimeHeadway) &&
             (timeElapsedSinceAlarm.elapsed() > ALARM_INTERVAL) ) {
            buf[3] = 1;
            timeElapsedSinceAlarm.restart();    // don't sound the alarm too often
        } else {
            buf[3] = 0;
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
        buf[6] = (char) leadVehicleVelocity;
        leadVehicleVelocityMutex.unlock();
    }

    arduinoDevice->getReading( buf, sizeof( buf ), NULL, 0 );
}

void AndroidDevice::run() {
    QTimer::singleShot( 0, this, SLOT( sendDataToAndroid() ) );

    /* Start the event loop */
    exec();
}
