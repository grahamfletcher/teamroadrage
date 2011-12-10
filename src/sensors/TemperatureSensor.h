#ifndef TEMPERATURESENSOR_H
#define TEMPERATURESENSOR_H

#include <QSettings>
#include <QThread>

#include "ArduinoDevice.h"

class TemperatureSensor : public QThread {
    Q_OBJECT

public:
    TemperatureSensor( QObject *parent, ArduinoDevice *arduinoDevice );
    ~TemperatureSensor();

    bool shouldContinue;

signals:
    void gotReading( float temperature );
    void gotIcePresent( bool ice );

protected:
    void run();

private:
    ArduinoDevice *arduinoDevice;

private slots:
    void getTemperatureFromArduino();
};

#endif
