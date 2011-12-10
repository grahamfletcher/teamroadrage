#ifndef RAINSENSOR_H
#define RAINSENSOR_H

#include <QSettings>
#include <QThread>

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

private:
    ArduinoDevice *arduinoDevice;

private slots:
    void getRainFromArduino();
};

#endif
