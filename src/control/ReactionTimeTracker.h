#ifndef REACTIONTIMETRACKER_H
#define REACTIONTIMETRACKER_H

#include <QObject>
#include <QMutex>
#include <QTime>

class ReactionTimeTracker : public QObject {
    Q_OBJECT

public:
    ReactionTimeTracker( QObject *parent );
    ~ReactionTimeTracker();

signals:
    void gotReactionTime( float time );

public slots:
    /* HeadwayKalmanFilter : Controller */
    void updateTimeHeadway( float time );    // gotTimeHeadway()
    void updateLeadVehicleAcceleration( float acceleration );    // gotLeadVehicleAcceleration()
    void updateFollowingVehicleAcceleration( float accleration );    // gotFollowingVehicleAcceleration

private:
    QMutex timerMutex;
    QTime *timer;

    QMutex timeSinceBrakingEventMutex;
    QTime *timeSinceBrakingEvent;

    QMutex smallTimeHeadwayMutex;
    bool smallTimeHeadway;

    QMutex leadVehicleIsStoppingMutex;
    bool leadVehicleIsStopping;

    QMutex reactionTimeAccumulatorMutex;
    float reactionTimeAccumulator;
};
#endif
