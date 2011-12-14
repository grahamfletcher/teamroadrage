#ifndef RAINSENSOR_H
#define RAINSENSOR_H

#include <QSettings>
#include <QThread>
#include <QTime>

#include "ArduinoDevice.h"

class RainSensor : public QThread {
    Q_OBJECT

public:
    RainSensor( QObject *parent, ArduinoDevice *arduinoDevice );
    ~RainSensor();

    bool shouldContinue;

signals:
    void gotReading( float voltage );
    void gotRainPresent( bool rain );

protected:
    void run();

private slots:
    void getRainFromArduino();

private:
    ArduinoDevice *arduinoDevice;

    QTime *rainTimer;

    bool rainPresent;
};

#endif
