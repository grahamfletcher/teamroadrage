#include <QDebug>
#include <QTimer>

#include "TemperatureSensor.h"

#define ICE_THRESHOLD 40    // TODO: Is the temperature in degrees Celcius?
#define ICE_OFFSET 40

TemperatureSensor::TemperatureSensor( QObject *parent = 0, ArduinoDevice *arduinoDevice = 0 ) : QThread( parent ), arduinoDevice( arduinoDevice ) {
    shouldContinue = true;

    /* Set correct affinity */
    //moveToThread( this );
}

TemperatureSensor::~TemperatureSensor() {
    qDebug() << "~TemperatureSensor()";
    shouldContinue = false;
    wait();
}

void TemperatureSensor::getTemperatureFromArduino() {
    /* BEGIN TESTING CODE */
    float d = 10;

    while ( shouldContinue ) {
        usleep( 70000 );
        emit gotReading( (d += 0.1) + (qrand() % 9) / 10 );
    }

    exit( 0 );
    return;
    /* END TESTING CODE */


    unsigned char cmd[] = { 't', '\r' };
    unsigned char result[1];

    while ( shouldContinue ) {
        sleep( 3 );//0 );    // the temperature shouldn't change too quickly

        if ( !arduinoDevice->getReading( cmd, sizeof( cmd ), result, sizeof( result ) ) ) {
            /* Getting the reading failed; try again */
            continue;
        }

        emit gotReading( (float) result[0] - ICE_OFFSET );

        if ( (float) result[0] < ICE_THRESHOLD ) {
            emit gotIcePresent( true );
        }
    }

    exit( 0 );
}

void TemperatureSensor::run() {
    QTimer::singleShot( 0, this, SLOT( getTemperatureFromArduino() ) );

    exec();
}

