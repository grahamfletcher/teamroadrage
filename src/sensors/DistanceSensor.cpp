#include <QDebug>
#include <QRegExp>
#include <QTimer>

#include "globals.h"
#include "DistanceSensor.h"

#define VID 0x0403
#define PID 0x6001
#define MANUFACTURER "FTDI"
#define DESCRIPTION "FT232R USB UART"
#define BAUDRATE 19200
#define BUF_SIZE (14 * 2) // (14 bytes/measurement) * (2 measurements)


DistanceSensor::DistanceSensor( QObject *parent = 0 ) : QThread( parent ) {
    intercept = settings->value( "DistanceSensor/intercept" ).toDouble();
    slope = settings->value( "DistanceSensor/slope" ).toDouble();
    slope = slope > 0 ? slope : 1;

    buf.resize( BUF_SIZE );

    /* Create and setup the FTDI context */
    setupFTDI();

    /* Flush the read and write buffers, for safety */
//    ftdi_usb_purge_buffers( ftdi );


    /* Set correct affinity */
    moveToThread( this );

    /* Start the thread */
    start();
}

DistanceSensor::~DistanceSensor() {
    /* Release the FTDI context */
    ftdi_usb_close( ftdi );
    ftdi_free( ftdi );
}

void DistanceSensor::setupFTDI() {
    if ( !(ftdi = ftdi_new()) ) {
        qDebug() << __FILE__ << __LINE__ << ": Failed to create ftdi context";
        return;
    }

    if ( ftdi_set_interface( ftdi, INTERFACE_ANY ) < 0 ) {
        qDebug() << __FILE__ << __LINE__ << ": Unable to set interface: " << ftdi_get_error_string( ftdi );
        return;
    }

    if ( ftdi_usb_open_desc( ftdi, VID, PID, DESCRIPTION, NULL ) < 0 ) {
        qDebug() << __FILE__ << __LINE__ << ": Unable to open ftdi device: " << ftdi_get_error_string( ftdi );
        return;
    }

    if ( ftdi_set_baudrate( ftdi, BAUDRATE ) < 0 ) {
        qDebug() << __FILE__ << __LINE__ << ": Unable to set baudrate: " << ftdi_get_error_string( ftdi );
        return;
    }

    if ( ftdi_set_line_property( ftdi, BITS_8, STOP_BIT_1, NONE ) < 0 ) {
        qDebug() << __FILE__ << __LINE__ << ": Unable to set line parameters: " << ftdi_get_error_string( ftdi );
        return;
    }
}

void DistanceSensor::monitorSerial() {
    /* BEGIN TESTING CODE */
    float d = 10;

    while ( 1 ) {
        usleep( 70000 );
        emit gotReading( (d += 0.1) + (qrand() % 9) / 10 );
    }
    /* END TESTING CODE */


    int offset = 0;

    while( 1 ) {
        do {
            usleep( (100000 * 8 * BUF_SIZE) / BAUDRATE );
            offset = ftdi_read_data( ftdi, (unsigned char*) buf.data(), BUF_SIZE );
        } while ( offset < BUF_SIZE );

        offset = 0;

        /* Get rid of any useless or null characters that would mess up the conversion to a string */
        for ( int i = 0; i < BUF_SIZE; i++ ) {
            char c = buf.data()[i];
            if ( c < 48 || c > 57 ) {
                buf.data()[i] = '-';
            }
        }

        /* Extract the numbers using a regular expression, and find the minimum, non-zero value */
        QRegExp reg_exp( "([^-]{4})" );
        QString str( buf );

        if ( reg_exp.indexIn( str, 0 ) != -1 ) {
            bool okay = false;
            int distance = reg_exp.cap( 1 ).toInt( &okay, 10 );
            if ( okay ) {
                emit gotReading( (distance - intercept) / slope );
            } else {
                /* Couldn't convert the reading to an integer */
            }
        } else {
            /* Couldn't find a valid reading */
        }
    }
}

void DistanceSensor::run() {
    QTimer::singleShot( 0, this, SLOT( monitorSerial() ) );

    exec();
}
