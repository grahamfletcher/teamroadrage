#include <QDebug>
#include <QTimer>

#include "RainSensor.h"

#define RAIN_THRESHOLD 0.4    // 400 mV
#define RAIN_FACTOR 10
#define RAIN_TIMEOUT 1200000    // 20 minutes

RainSensor::RainSensor( QObject *parent = 0, ArduinoDevice *arduinoDevice = 0 ) : QThread( parent ), arduinoDevice( arduinoDevice ) {
    shouldContinue = true;
    rainPresent = false;

    rainTimer = new QTime();
    rainTimer->start();
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

    unsigned char command = 'r';
    unsigned char result;

    /* Get one reading and ignore it; the first one is always wrong for some reason */
    arduinoDevice->getReading( &command, sizeof( command ), &result, sizeof( result ) );

    /* Get the real readings and loop until it's time to quit */
    while ( shouldContinue ) {
        if ( !arduinoDevice->getReading( &command, sizeof( command ), &result, sizeof( result ) ) ) {
            /* Getting the reading failed; try again */
            if ( shouldContinue ) {
                continue;
            }
        }

        float reading = ((float) result * RAIN_FACTOR) / 1000;    // mV

        if ( shouldContinue ) {
            emit gotReading( reading );
        }

        bool rain = reading > RAIN_THRESHOLD;

        if ( shouldContinue ) {
            if ( rain && !rainPresent ) {
                rainPresent = true;

                rainTimer->restart();

                emit gotRainPresent( true );
            } else if ( !rain && rainPresent && rainTimer->elapsed() > RAIN_TIMEOUT ) {
                rainPresent = false;

                rainTimer = new QTime();

                emit gotRainPresent( false );
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

    qDebug() << "exit RainSensor";
    exit( 0 );
}

void RainSensor::run() {
    QTimer::singleShot( 0, this, SLOT( getRainFromArduino() ) );

    exec();
}

