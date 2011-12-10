#include <QDebug>
#include <QTimer>

#include "TemperatureSensor.h"

#define ICE_THRESHOLD 40    // TODO: Is the temperature in degrees Celcius?

TemperatureSensor::TemperatureSensor( QObject *parent = 0, ArduinoDevice *arduinoDevice = 0 ) : QThread( parent ), arduinoDevice( arduinoDevice ) {
    /* Set correct affinity */
    moveToThread( this );

    /* Start the thread */
    start();
}

TemperatureSensor::~TemperatureSensor() {

}

void TemperatureSensor::getTemperatureFromArduino() {
    /* BEGIN TESTING CODE */
    float d = 10;

    while ( 1 ) {
        usleep( 70000 );
        emit gotReading( (d += 0.1) + (qrand() % 9) / 10 );
    }
    /* END TESTING CODE */

    char cmd[] = { 't', '\r' };
    char result[1];

    while ( 1 ) {
        sleep( 30 );    // the temperature shouldn't change too quickly

        if ( !arduinoDevice->getReading( cmd, sizeof( cmd ), result, sizeof( result ) ) ) {
            /* Getting the reading failed; try again */
            continue;
        }

        emit gotReading( (float) result[0] );

        if ( (float) result[0] < ICE_THRESHOLD ) {
            emit gotIcePresent( true );
        }
    }
}

void TemperatureSensor::run() {
    QTimer::singleShot( 0, this, SLOT( getTemperatureFromArduino() ) );

    exec();
}
