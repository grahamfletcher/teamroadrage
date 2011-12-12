#include <QDebug>
#include <QTimer>

#include "RainSensor.h"

#define RAIN_THRESHOLD 0.4    // 400 mV
#define RAIN_FACTOR 10

RainSensor::RainSensor( QObject *parent = 0, ArduinoDevice *arduinoDevice = 0 ) : QThread( parent ), arduinoDevice( arduinoDevice ) {
    shouldContinue = true;

    /* Set correct affinity */
    //moveToThread( this );
}

RainSensor::~RainSensor() {
    qDebug() << "~RainSensor()";
    shouldContinue = false;
    wait();
}

void RainSensor::getRainFromArduino() {
    unsigned char cmd[] = { 'r' };
    unsigned char result[1];

    /* Get one reading and ignore it; the first one is always wrong for some reason */
    while ( !arduinoDevice->getReading( cmd, sizeof( cmd ), result, sizeof( result ) ) ) {
        sleep( 1 );    // hold on one second
    }

    /* Get the real readings and loop until it's time to quit */
    while ( shouldContinue ) {
        sleep( 3 );    // wait three seconds between queries

        if ( !arduinoDevice->getReading( cmd, sizeof( cmd ), result, sizeof( result ) ) ) {
            /* Getting the reading failed; try again */
            continue;
        }

        float reading = ((float) result[0] * RAIN_FACTOR) / 1000;    // mV

        emit gotReading( reading );
        emit gotRainPresent( (reading > RAIN_THRESHOLD) );
    }

    exit( 0 );
}

void RainSensor::run() {
    QTimer::singleShot( 0, this, SLOT( getRainFromArduino() ) );

    exec();
}

    /* BEGIN TESTING CODE */
    /*
    float d = 0;

    while ( shouldContinue ) {
        usleep( 70000 );
        emit gotReading( (d += 0.1) + (qrand() % 9) / 10 );
    }

    exit( 0 );
    return;
    */
    /* END TESTING CODE */
