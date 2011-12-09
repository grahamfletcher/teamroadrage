#ifndef OBDDEVICE_H
#define OBDDEVICE_H

#include <ftdi.h>
#include <QMutex>
#include <QObject>
#include <QSettings>
#include <QTime>

class OBDDevice : public QObject {
public:
    OBDDevice( QObject *parent );
    ~OBDDevice();
    bool getReading( unsigned char *command, int commandLength,
                     char *result, int resultLength );

private:
    void setupFTDI();
    void setupOBD();

    QMutex obdMutex;
    
    QTime timeElapsed;

    struct ftdi_context *ftdi;
};

#endif
