#include <QDebug>
#include <QTimer>

#include "TemperatureSensor.h"

#define ICE_THRESHOLD 40    // TODO: Is the temperature in degrees Celcius?
#define ICE_OFFSET 40

TemperatureSensor::TemperatureSensor( QObject *parent = 0, ArduinoDevice *arduinoDevice = 0 ) : QThread( parent ), arduinoDevice( arduinoDevice ) {
    shouldContinue = true;
}

TemperatureSensor::~TemperatureSensor() {
    qDebug() << "~TemperatureSensor()";
    shouldContinue = false;
    wait();
}

void TemperatureSensor::getTemperatureFromArduino() {
    /* BEGIN TESTING CODE */
    
    float d = 5;

    while ( shouldContinue ) {
        usleep( 70000 );
        emit gotReading( (d -= 0.1) + (qrand() % 9) / 10 );
    }

    exit( 0 );
    return;
    /* END TESTING CODE */


    unsigned char cmd[] = { 't', '\r' };
    unsigned char result[1];

    while ( shouldContinue ) {
        if ( !arduinoDevice->getReading( cmd, sizeof( cmd ), result, sizeof( result ) ) ) {
            /* Getting the reading failed; try again */
            if ( shouldContinue ) {
                continue;
            }
        }

        if ( shouldContinue ) {
            emit gotReading( (float) result[0] - ICE_OFFSET );

            if ( (float) result[0] < ICE_THRESHOLD ) {
                emit gotIcePresent( true );
            }
        }

        // wait 3 seconds
        for ( int i = 0; i < 6; i++ ) {
            usleep( 500000 );
            
            if ( !shouldContinue ) {
                break;
            }
        }
    }

    qDebug() << "exit TemperatureSensor";
    exit( 0 );
}

void TemperatureSensor::run() {
    QTimer::singleShot( 0, this, SLOT( getTemperatureFromArduino() ) );

    exec();
}

