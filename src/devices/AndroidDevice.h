#ifndef ANDROIDDEVICE_H
#define ANDROIDDEVICE_H

#include <QMutex>
#include <QSettings>
#include <QThread>
#include <QTime>

#include "ArduinoDevice.h"

class AndroidDevice : public QThread
{
    Q_OBJECT

public:
    AndroidDevice( QObject *parent, ArduinoDevice *arduinoDevice );
    ~AndroidDevice();

public slots:
    /* TemperatureSensor : Controller */
    void updateIcePresent( bool ice );    // gotIcePresent()

    /* HeadwayKalmanFilter : Controller */
    void updateLeadVehicleVelocity( float velocity );    // gotLeadVehicleVelocity()
    void updateTimeHeadway( float time );    // gotTimeHeadway()

    /* RainSensor AND HumiditySensor : Controller */
    void updateRainPresent( bool rain );    // gotRainPresent()

    /* ReactionTimeTracker : Controller */
    void updateReactionTime( float time );    // gotReactionTime()

    /* Controller : Controller */
    void updateSafeTimeHeadway( float headway );    // gotSafeTimeHeadway()

protected:
    void run();

private slots:
    void sendDataToAndroid();

private:
    ArduinoDevice *arduinoDevice;

    QTime timeElapsedSinceAlarm;

    QMutex icePresentMutex;
    bool icePresent;

    QMutex rainPresentMutex;
    bool rainPresent;

    QMutex leadVehicleVelocityMutex;
    float leadVehicleVelocity;

    QMutex reactionTimeMutex;
    float reactionTime;

    QMutex safeTimeHeadwayMutex;
    float safeTimeHeadway;

    QMutex timeHeadwayMutex;
    float timeHeadway;
};

#endif
