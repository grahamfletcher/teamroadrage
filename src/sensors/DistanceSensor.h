#ifndef DISTANCESENSOR_H
#define DISTANCESENSOR_H

#include <ftdi.h>
#include <QByteArray>
#include <QSettings>
#include <QThread>

class DistanceSensor : public QThread {
    Q_OBJECT

public:
    DistanceSensor( QObject *parent );
    ~DistanceSensor();

    bool shouldContinue;

signals:
    void gotReading( float distance );

protected:
    void run();

private:
    void setupFTDI();

    struct ftdi_context *ftdi;
    QByteArray buf;
    float intercept;
    float slope;

private slots:
    void monitorSerial();
};
#endif
