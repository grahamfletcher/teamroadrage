#ifndef HUMIDITYSENSOR_H
#define HUMIDITYSENSOR_H

#include <QSettings>
#include <QThread>

#include "ArduinoDevice.h"

class HumiditySensor : public QThread {
    Q_OBJECT

public:
    HumiditySensor( QObject *parent, ArduinoDevice *arduinoDevice );
    ~HumiditySensor();

signals:
    void gotReading( float humidity );
    void gotRainPresent( float rain );

protected:
    void run();

private:
    ArduinoDevice *arduinoDevice;

private slots:
    void getHumidityFromArduino();
};

#endif
