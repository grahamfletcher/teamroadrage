#ifndef ARDUINO_DEVICE_H
#define ARDUINO_DEVICE_H

#include <QMutex>
#include <QObject>

class ArduinoDevice : public QObject {
	Q_OBJECT

public:
    ArduinoDevice( QObject* parent );
    ~ArduinoDevice();

    bool getReading( const unsigned char *command, size_t commandLength, unsigned char *result, size_t resultLength, unsigned int delay = 10000 );

private slots:
    void setupSerial();

private:
    QMutex serialMutex;

    int fd;
};
#endif
