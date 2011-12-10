#include <QDebug>
#include <QTimer>

#include "RainSensor.h"

#define RAIN_THRESHOLD 1.2    // TODO: What is this?

RainSensor::RainSensor( QObject *parent = 0, ArduinoDevice *arduinoDevice = 0 ) : QThread( parent ), arduinoDevice( arduinoDevice ) {
    /* Set correct affinity */
    moveToThread( this );

    /* Start the thread */
    start();
}

RainSensor::~RainSensor() {

}

void RainSensor::getRainFromArduino() {
    /* BEGIN TESTING CODE */
    float d = 0;

    while ( 1 ) {
        usleep( 70000 );
        emit gotReading( (d += 0.1) + (qrand() % 9) / 10 );
    }
    /* END TESTING CODE */

    char cmd[] = { 'r', '\r' };
    char result[1];

    while ( 1 ) {
        sleep( 3 );    // wait three seconds between queries

        if ( !arduinoDevice->getReading( cmd, sizeof( cmd ), result, sizeof( result ) ) ) {
            /* Getting the reading failed; try again */
            continue;
        }

        float reading = (float) result[0];

        emit gotReading( reading );
        emit gotRainPresent( (reading > RAIN_THRESHOLD) );
    }
}

void RainSensor::run() {
    QTimer::singleShot( 0, this, SLOT( getRainFromArduino() ) );

    exec();
}
