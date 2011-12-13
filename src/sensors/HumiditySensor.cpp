#include <QDebug>
#include <QTimer>

#include "HumiditySensor.h"

HumiditySensor::HumiditySensor( QObject *parent = 0, ArduinoDevice *arduinoDevice = 0 ) : QThread( parent ), arduinoDevice( arduinoDevice ) {
    shouldContinue = true;
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

    unsigned char cmd[] = { 'h', '\r' };
    unsigned char result[1];

    while ( shouldContinue ) {
        if ( !arduinoDevice->getReading( cmd, sizeof( cmd ), result, sizeof( result ) ) ) {
            /* Getting the reading failed; try again */
            if ( shouldContinue ) {
                continue;
            }
        }

        emit gotReading( (float) result[0] );

        if ( result[0] > 90 && shouldContinue ) {
            emit gotRainPresent( true );
        }

        // wait 30 seconds
        for ( int i = 0; i < 60; i++ ) {
            usleep( 500000 );

            if ( !shouldContinue ) {
                break;
            }
        }
    }

    qDebug() << "exit HumiditySensor";
    exit( 0 );
}

void HumiditySensor::run() {
    QTimer::singleShot( 0, this, SLOT( getHumidityFromArduino() ) );

    exec();
}
