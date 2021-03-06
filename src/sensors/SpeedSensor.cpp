#include <QDebug>
#include <QTimer>

#include "SpeedSensor.h"

SpeedSensor::SpeedSensor( QObject *parent = 0, OBDDevice *obdDevice = 0 ) : QThread( parent ), obdDevice( obdDevice ) {
    shouldContinue = true;
}

SpeedSensor::~SpeedSensor() {
    qDebug() << "~SpeedSensor()";
    shouldContinue = false;
    wait();
}

void SpeedSensor::getSpeedFromOBD() {
    /* BEGIN TESTING CODE */
    /*float d = 10;

    while ( shouldContinue ) {
        usleep( 70000 );
        emit gotReading( (d += 0.1) + (qrand() % 9) / 10 );
    }

    exit( 0 );
    return;*/
    /* END TESTING CODE */

    unsigned char cmd_010D1[] = { '0', '1', '0', 'D', '1', '\r' };
    char obdResult[9];

    bool okay;
    int speed;
    char speedBuffer[] = { '0', '0', 0 };

    while ( shouldContinue ) {
        if ( !obdDevice->getReading( cmd_010D1, sizeof( cmd_010D1 ), obdResult, sizeof( obdResult ) ) ) {
            /* Getting the reading failed; try again */
            if ( shouldContinue ) {
                continue;
            }
        }
        
        speedBuffer[0] = obdResult[4];
        speedBuffer[1] = obdResult[5];

        okay = false;
        speed = QString( speedBuffer ).toInt( &okay, 16 );

        if ( okay && shouldContinue ) {
            emit gotReading( (float) speed / 3.6 );    // convert to m/s
        } else {
            /* Something went wrong when extracting the speed;
               maybe got NO_DATA */
        }
    }

    qDebug() << "exit SpeedSensor";
    exit( 0 );
}

void SpeedSensor::run() {
    QTimer::singleShot( 0, this, SLOT( getSpeedFromOBD() ) );

    /* Start the event loop */
    exec();
}
