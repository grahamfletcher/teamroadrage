#include <QDebug>
#include <QTimer>

#include "HumiditySensor.h"

HumiditySensor::HumiditySensor( QObject *parent = 0, ArduinoDevice *arduinoDevice = 0 ) : QThread( parent ), arduinoDevice( arduinoDevice ) {
    shouldContinue = true;

    /* Set correct affinity */
    moveToThread( this );
}

HumiditySensor::~HumiditySensor() {
    qDebug() << "~HumiditySensor()";
    shouldContinue = false;
    wait();
}

void HumiditySensor::getHumidityFromArduino() {
    /* BEGIN TESTING CODE */
    float d = 10;

    while ( shouldContinue ) {
        emit gotReading( (d += 0.1) + (qrand() % 9) / 10 );
        usleep( 1000000 );
    }

    exit( 0 );
    return;
    /* END TESTING CODE */

    char cmd[] = { 'h', '\r' };
    char result[1];

    while ( shouldContinue ) {
        sleep( 30 );    // the humidity shouldn't change too quickly

        if ( !arduinoDevice->getReading( cmd, sizeof( cmd ), result, sizeof( result ) ) ) {
            /* Getting the reading failed; try again */
            continue;
        }

        emit gotReading( (float) result[0] );

        if ( result[0] > 90 ) {
            emit gotRainPresent( true );
        }
    }

    exit( 0 );
}

void HumiditySensor::run() {
    QTimer::singleShot( 0, this, SLOT( getHumidityFromArduino() ) );

    exec();
}
