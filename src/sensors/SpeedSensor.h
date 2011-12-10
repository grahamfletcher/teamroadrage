#ifndef SPEEDSENSOR_H
#define SPEEDSENSOR_H

#include <QSettings>
#include <QThread>

#include "OBDDevice.h"

class SpeedSensor : public QThread {
    Q_OBJECT

public:
    SpeedSensor( QObject *parent, OBDDevice *obdDevice );
    ~SpeedSensor();

    bool shouldContinue;

signals:
    void gotReading( float speed );

protected:
    void run();

private:
    OBDDevice *obdDevice;

private slots:
    void getSpeedFromOBD();
};

#endif
