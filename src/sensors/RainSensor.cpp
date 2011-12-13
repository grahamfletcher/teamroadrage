#include <QDebug>
#include <QTimer>

#include "RainSensor.h"

#define RAIN_THRESHOLD 0.4    // 400 mV
#define RAIN_FACTOR 10

RainSensor::RainSensor( QObject *parent = 0, ArduinoDevice *arduinoDevice = 0 ) : QThread( parent ), arduinoDevice( arduinoDevice ) {
    shouldContinue = true;
}

RainSensor::~RainSensor() {
    qDebug() << "~RainSensor()";
    shouldContinue = false;
    wait();
}

void RainSensor::getRainFromArduino() {
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

    unsigned char cmd = 'r';
    unsigned char result;

    /* Get one reading and ignore it; the first one is always wrong for some reason */
    //while ( !arduinoDevice->getReading( &cmd, sizeof( cmd ), &result, sizeof( result ) ) ) {
    //    sleep( 1 );    // hold on one second
    //    qDebug() << "here, still...";
    //}

    /* Get the real readings and loop until it's time to quit */
    while ( shouldContinue ) {
        if ( !arduinoDevice->getReading( &cmd, sizeof( cmd ), &result, sizeof( result ) ) ) {
            /* Getting the reading failed; try again */
            if ( shouldContinue ) {
                continue;
            }
        }

        float reading = ((float) result * RAIN_FACTOR) / 1000;    // mV

        if ( shouldContinue ) {
            emit gotReading( reading );
            emit gotRainPresent( (reading > RAIN_THRESHOLD) );
        }

        // wait 3 seconds
        for ( int i = 0; i < 6; i++ ) {
            usleep( 500000 );

            if ( !shouldContinue ) {
                break;
            }
        }
    }

    qDebug() << "exit RainSensor";
    exit( 0 );
}

void RainSensor::run() {
    QTimer::singleShot( 0, this, SLOT( getRainFromArduino() ) );

    exec();
}

