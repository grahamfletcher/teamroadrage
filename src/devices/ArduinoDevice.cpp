
/* Some code borrowed from http://todbot.com/blog/2006/12/06/arduino-serial-c-code-to-talk-to-arduino/ */

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>

#include <QDebug>
#include <QMutexLocker>

#include "globals.h"
#include "ArduinoDevice.h"

#define DEVICE_PATH "/dev/serial/by-id/usb-Arduino__www.arduino.cc__Arduino_Mega_ADK_64934333235351C0F160-if00"

ArduinoDevice::ArduinoDevice( QObject *parent ) : QObject( parent ) {
    setupSerial();
}

ArduinoDevice::~ArduinoDevice() {
    QMutexLocker locker( &serialMutex );
    close( fd );
    qDebug() << "~ArduinoDevice()";
}

void ArduinoDevice::setupSerial() {

    struct termios termOptions;

    if ( (fd = open( DEVICE_PATH, O_RDWR | O_NOCTTY | O_NDELAY )) == -1 ) {
        qDebug() << "Could not open Arduino device";
        return;
    }

    if ( tcgetattr( fd, &termOptions ) < 0 ) {
        qDebug() << "Couldn't get Arduino term attributes";
        return;
    }

    //cfsetispeed( &termOptions, B57600 );
    //cfsetospeed( &termOptions, B57600 );
    cfsetispeed( &termOptions, B115200 );
    cfsetospeed( &termOptions, B115200 );
    
    // 8N1
    termOptions.c_cflag &= ~PARENB;
    termOptions.c_cflag &= ~CSTOPB;
    termOptions.c_cflag &= ~CSIZE;
    termOptions.c_cflag |= CS8;
    // no flow control
    termOptions.c_cflag &= ~CRTSCTS;

    termOptions.c_cflag |= CREAD | CLOCAL;  // turn on READ & ignore ctrl lines
    termOptions.c_iflag &= ~(IXON | IXOFF | IXANY); // turn off s/w flow ctrl

    termOptions.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG); // make raw
    termOptions.c_oflag &= ~OPOST; // make raw

    // see: http://unixwiz.net/techtips/termios-vmin-vtime.html
    termOptions.c_cc[VMIN]  = 0;
    termOptions.c_cc[VTIME] = 20;

    if ( tcsetattr( fd, TCSANOW, &termOptions ) < 0 ) {
        qDebug() << "Couldn't set Arduino term attributes";
        return;
    }
}

bool ArduinoDevice::getReading( const unsigned char *command, int commandLength, unsigned char *result, int resultLength ) {
    QMutexLocker locker( &serialMutex );

    //tcflush( fd, TCIOFLUSH );

    if ( commandLength > 0 && command != NULL ) {
        if ( write( fd, command, commandLength ) != commandLength ) {
            return false;
        }
    }

    /* Some commands want a response; take care of these */
    if ( resultLength > 0 && result != NULL ) {
        int offset = 0;
        int temp = 0;
        int tries = 0;

        do {
            temp = read( fd, (result + offset), (resultLength - offset) );

            if ( temp != -1 ) {
                offset += temp;
            }
           
            if ( offset < resultLength ) {
                usleep( 10000 );    // wait 10 msec

                if ( tries++ >= 1000 ) {
                    qDebug() << "Reading serial data failed for command \'" << (char) command[0] << "\'. ( read:" << offset << "; expected:" << resultLength << ")";
                    return false;
                }
            }
        } while ( offset < resultLength );
    }

    tcflush( fd, TCIOFLUSH );

    return true;
}
