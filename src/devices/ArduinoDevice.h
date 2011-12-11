#ifndef ARDUINO_DEVICE_H
#define ARDUINO_DEVICE_H

#include <QMutex>
#include <QObject>

class ArduinoDevice : public QObject {
public:
    ArduinoDevice( QObject* parent );
    ~ArduinoDevice();
    bool getReading( const unsigned char *command, int commandLength, unsigned char *result, int resultLength );

private:
    void setupSerial();

    QMutex serialMutex;

    int fd;
};
#endif
