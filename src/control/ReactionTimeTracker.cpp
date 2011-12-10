#include <QDebug>
#include <QMutexLocker>
#include <QTime>

#include "ReactionTimeTracker.h"

#define ACCELERATION_THRESHOLD -2.5
#define DEFAULT_REACTION_TIME 0.7

ReactionTimeTracker::ReactionTimeTracker( QObject *parent ) : QObject( parent ), reactionTimeAccumulator( DEFAULT_REACTION_TIME )  {
    timer = new QTime();
    timeSinceBrakingEvent = new QTime();
    timeSinceBrakingEvent->start();
}

ReactionTimeTracker::~ReactionTimeTracker() {
    qDebug() << "~ReactionTimeTracker()";
}

void ReactionTimeTracker::updateTimeHeadway( float time ) {
    QMutexLocker locker( &smallTimeHeadwayMutex );

    /* TODO: What constitutes a "small" time headway */
    if ( time < 3.5 ) {
        smallTimeHeadway = true;
    } else {
        /* Vehicle is out of range; reset everything */
        QMutexLocker locker1( &timerMutex );
        QMutexLocker locker2( &leadVehicleIsStoppingMutex );

        smallTimeHeadway = false;
        leadVehicleIsStopping = false;
        
        if ( timer->elapsed() > 0 ) {
            timer = new QTime();
        }
    }
}

void ReactionTimeTracker::updateLeadVehicleAcceleration( float acceleration ) {
    QMutexLocker locker( &smallTimeHeadwayMutex );

    if ( smallTimeHeadway ) {
        QMutexLocker locker( &timerMutex );

        /* TODO: What is a good acceleration theshold here? */
        if ( acceleration < ACCELERATION_THRESHOLD ) {
            /* Large enough acceleration to start the timer */
            if ( timer->elapsed() == 0 ) {
                timer->start();
            }
        } else if ( acceleration >= -0.5 ) {
            /* False alarm */
            QMutexLocker locker( &leadVehicleIsStoppingMutex );

            leadVehicleIsStopping = false;

            if ( timer->elapsed() > 0 ) {
                timer = new QTime();
            }

            return;
        } else {
            /* They might be braking, but it's not enough to start the timer */
        }

        /* In order to get here, the acceleration has to be less than -0.5 */
        
        if ( timer->elapsed() > 250 ) {
            /* They were slowing for a quarter of a second; they're probably braking */
            QMutexLocker locker( &leadVehicleIsStoppingMutex );
            leadVehicleIsStopping = true;
        } else {
            /* We're still waiting to see if it was a false alarm */
        }
    } else {
        /* A person might not brake if they're a fair distance behind the lead vehicle */
    }
}

void ReactionTimeTracker::updateFollowingVehicleAcceleration( float acceleration ) {
    QMutexLocker locker1( &leadVehicleIsStoppingMutex );
    QMutexLocker locker2( &timeSinceBrakingEventMutex );

    /* We wait at least thirty seconds between events to make sure every event is unique */
    if ( leadVehicleIsStopping && (timeSinceBrakingEvent->elapsed() > 30000) ) {
        if ( acceleration < ACCELERATION_THRESHOLD ) {
            /* Okay, the following vehicle is braking in response to the lead vehicle */
            QMutexLocker locker1( &timerMutex );
            QMutexLocker locker2( &reactionTimeAccumulatorMutex );

            reactionTimeAccumulator = 0.5 * reactionTimeAccumulator + 0.5 * (timer->elapsed() / 1000 );
            emit gotReactionTime( reactionTimeAccumulator );

            timer = new QTime();
            timeSinceBrakingEvent = new QTime();
            timeSinceBrakingEvent->start();
        } else {
            /* The following vehicle hasn't stopped braking yet */
        }
    }
}

