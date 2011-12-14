
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
#include <QTimer>

#include "globals.h"
#include "ArduinoDevice.h"

#define BAUD_RATE B57600
#define MAX_READ_ATTEMPTS 10
#define READ_DELAY_INTERVAL 10000
#define DEVICE_PATH "/dev/serial/by-id/usb-Arduino__www.arduino.cc__Arduino_Mega_ADK_64934333235351C0F160-if00"

ArduinoDevice::ArduinoDevice( QObject *parent ) : QObject( parent ) {
    /* Ensure that the constructor will not block; setup the serial port later */
    QTimer::singleShot( 0, this, SLOT( setup_serial() ) );
}

ArduinoDevice::~ArduinoDevice() {
    QMutexLocker locker( &serialMutex );
    if(fd !=0){
		close(fd);
		}
    qDebug() << "~ArduinoDevice()";
}

void ArduinoDevice::setupSerial() {
	QMutexLocker locker(&serialMutex);
    struct termios termOptions;

    if ( (fd = open( DEVICE_PATH, O_RDWR | O_NOCTTY | O_NDELAY )) == -1 ) {
        qDebug() << "Could not open Arduino device";
        return;
    }

    if ( tcgetattr( fd, &termOptions ) < 0 ) {
        qDebug() << "Couldn't get Arduino term attributes";
        return;
    }

    cfsetispeed( &termOptions, BAUD_RATE );
    cfsetospeed( &termOptions, BAUD_RATE );

    
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

bool ArduinoDevice::getReading( const unsigned char *command, size_t commandLength, unsigned char *result, size_t resultLength, unsigned int delay ) {

	if(fd == 0){
	qDebug()<<__FILE__ << __LINE__ << "NULL file descriptor";
	return false;
	}

    QMutexLocker locker( &serialMutex );

    int temp;

    tcflush( fd, TCIOFLUSH );

    if ( commandLength > 0 && command != NULL ) {
        temp = write( fd, command, commandLength );
        
        if ( (unsigned) temp != commandLength ) {
            return false;
        }
    }

    /* Some commands want a response; take care of these */
    if ( resultLength > 0 && result != NULL ) {
        unsigned int offset = 0;
        unsigned int attempts = 0;

        usleep( delay );    // sometimes the Arduino takes a while to give back an answer

        do {
            temp = read( fd, (result + offset), (resultLength - offset) );

            if ( temp >= 0 ) {
                offset += temp;
            }
           
            if ( offset < resultLength ) {
                usleep( READ_DELAY_INTERVAL );    // wait a while longer

                if ( ++attempts >= MAX_READ_ATTEMPTS){
                    qDebug( "Reading serial data failed for command \'%c\'. Expected %d bytes, got %d bytes.", command[0], resultLength, offset );
                    return false;
                }
            }
        } while ( offset < resultLength );
    }

    tcflush( fd, TCIOFLUSH );

    return true;
}
