#include <QDebug>
#include <QMutexLocker>
#include <QString>

#include "OBDDevice.h"

#define VID 0x0403
#define PID 0x6001
#define MANUFACTURER "ScanTool.net LLC"
#define DESCRIPTION "OBDLink SX"
#define BAUDRATE 115200

OBDDevice::OBDDevice( QObject *parent = 0 ) : QObject( parent ) {
    /* Create and setup the FTDI context */
    setupFTDI();
    qDebug() << "setupFTDI()";

    /* Setup the OBD interpreter */
    setupOBD();
    qDebug() << "setupOBD()";

    /* Flush the read and write buffers, for safety */
    ftdi_usb_purge_buffers( ftdi );

    /* Start the timer */
    timeElapsed.start();
}

OBDDevice::~OBDDevice() {
    /* Release the FTDI context */
    ftdi_usb_close( ftdi );
    ftdi_free( ftdi );

    qDebug() << "~OBDDevice()";
}

void OBDDevice::setupFTDI() {
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

void OBDDevice::setupOBD() {
    int offset = 0;
    unsigned char buf[32];    // just picked a power of two for whatever reason

    /* Each of the commands needs to be pre-allocated WITHOUT a null-terminating character */
    unsigned char cmd_ATZ[] = { 'A', 'T', 'Z', '\r' };
    unsigned char cmd_ATE0[] = { 'A', 'T', 'E', '0', '\r' };
    unsigned char cmd_ATL0[] = { 'A', 'T', 'L', '0', '\r' };
    unsigned char cmd_ATS0[] = { 'A', 'T', 'S', '0', '\r' };
    unsigned char cmd_ATSPA3[] = { 'A', 'T', 'S', 'P', 'A', '3', '\r' };
    unsigned char cmd_010D1[] = { '0', '1', '0', 'D', '1', '\r' };

    /* Stop any previous OBD queries and clear the read buffer */
    ftdi_write_data( ftdi, (unsigned char*) "", sizeof( "" ) );
    usleep( 10000 );
    ftdi_read_data( ftdi, buf, sizeof( buf ) );

    /* Send "AT Z\r" to reset the device */
    ftdi_write_data( ftdi, cmd_ATZ, sizeof( cmd_ATZ ) );

    usleep( 800000 );
    
    do {
        usleep( 100000 );
        offset += ftdi_read_data( ftdi, (buf + offset),
                                  (sizeof( buf ) - offset) );
    } while ( offset < 21 );

    offset = 0;

    /* Send "AT E 0\r" to disable command echo */
    ftdi_write_data( ftdi, cmd_ATE0, sizeof( cmd_ATE0 ) );
    /* Send "AT L 0\r" to disable linefeeds */
    ftdi_write_data( ftdi, cmd_ATL0, sizeof( cmd_ATL0 ) );
    /* Send "AT S 0\r" to disable spaces */
    ftdi_write_data( ftdi, cmd_ATS0, sizeof( cmd_ATS0 ) );
    /* Send "AT SP A 3\r" to try ELM237 protocol 3 */
    ftdi_write_data( ftdi, cmd_ATSPA3, sizeof( cmd_ATSPA3 ) );
    
    /* Clear the read buffer of the output from previous comands */
    do {
        usleep( 1 );
        offset += ftdi_read_data( ftdi, (buf + offset),
                                  (sizeof( buf ) - offset) );
    } while ( offset < 24 );

    offset = 0;

    /* Send "010D1\r" (arbitrary) to confirm the protocol selection */
    ftdi_write_data( ftdi, cmd_010D1, sizeof( cmd_010D1 ) );

    do {
        usleep( 5000 );
        offset += ftdi_read_data( ftdi, (buf + offset),
                                  (sizeof( buf ) - offset ) );
    } while ( offset < 9 );
}

bool OBDDevice::getReading( unsigned char *command, int commandLength, char *result, int resultLength ) {
    QMutexLocker locker( &obdMutex );

    int t = timeElapsed.elapsed();

    if ( t < 50 ) {
        /* Sending commands too fast results in NO DATA */
        usleep( (50 - t) * 1000 );
    }

    int offset = 0;

    result[ resultLength - 1 ] = '\0';

    ftdi_write_data( ftdi, command, commandLength );
    
    do {
        usleep( 1000 );
        offset += ftdi_read_data( ftdi, ((unsigned char*) result + offset), 
            (resultLength - offset) );
    } while ( offset < resultLength );

    if ( result[0] == 'N' && result[1] == 'O' &&
         result[2] == ' ' && result[2] == 'D' ) {
        /* Got NO DATA */
        return false;
    }

    if ( result[ resultLength - 1 ] != '>' ) {
        /* Something went wrong */
        return false;
    }

    timeElapsed.restart();

    return true;
}

