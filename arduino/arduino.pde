#include <Wire.h>
#include <Servo.h>

#include "AndroidAccessory.h"
#include "CapSense.h"
#include "SHT_15.h"

#define HUMIDITY_POWER_PIN 8
#define DATA_PIN           10
#define CLOCK_PIN          11
#define RAIN_DATA_PIN      A0
#define BAUD_RATE          115200    // this was too fast earlier, but now it works...
#define MESSAGE_SIZE       7

bool accPoweredOn = false;

AndroidAccessory acc( "Google, Inc.",
                      "DemoKit",
                      "DemoKit Arduino Board",
                      "1.0",
                      "http://www.android.com",
                      "0000000012345678" );

void setup() {
    Serial.begin( BAUD_RATE );
    Serial.print( "\r\nStart" );

    pinMode( 24, INPUT );
    pinMode( 25, INPUT );

    pinMode( HUMIDITY_POWER_PIN, OUTPUT );
    digitalWrite( HUMIDITY_POWER_PIN, HIGH );

    //acc.powerOn();    // this now has to be explicitly called once things are ready
}

void loop() {
    float temperature;
    float humidity;
    float rainVoltage;
    float val;

    byte command = 0;    // command sent from computer
    byte message[MESSAGE_SIZE];

    message[0] = 0xFF;    // lets the Android know where the buffer begins

    bool androidAvailable = false;
    
    if ( accPoweredOn ) {
        androidAvailable = acc.isConnected();
    }

    while ( Serial.available() > 0 ) {

        command = Serial.read();

        switch ( command ) {

            case 'z':    // start the Android and return its status

                if ( !accPoweredOn ) {
                    acc.powerOn();
                    accPoweredOn = true;
                }

                Serial.print( (char) acc.isConnected() );

                break;

            case 't':    // read temperature

                sendCommandSHT( gTempCmd, DATA_PIN, CLOCK_PIN );
                waitForResultSHT( DATA_PIN );
                val = getData16SHT( DATA_PIN, CLOCK_PIN );
                skipCrcSHT( DATA_PIN, CLOCK_PIN );

                temperature = -40.0 + 0.018 * val;

                Serial.print( (char) (temperature + 40) );    // offset by 40 to make unsigned

                break;

            case 'h':    // read humidity

                sendCommandSHT( gHumidCmd, DATA_PIN, CLOCK_PIN );
                waitForResultSHT( DATA_PIN );
                val = getData16SHT( DATA_PIN, CLOCK_PIN );
                skipCrcSHT( DATA_PIN, CLOCK_PIN );

                humidity = -4.0 + 0.0405 * val + -0.0000028 * val * val;

                Serial.print( (char) humidity );    // what is the range of humidity?

                break;

            case 'r':

                rainVoltage = analogRead( RAIN_DATA_PIN );

                Serial.print( (char) (0.5 + rainVoltage / 10) );

                break;
        
            case 'c':    // send data from PC to Android

                for ( int i = 1; i < sizeof( message ); i++ ) {
                    while( !Serial.available() ) {
                        delay( 1 );
                    }

                    message[i] = Serial.read();
                }

                if ( androidAvailable ) {
                    acc.write( message, sizeof( message ) );
        
                    /* Signal that the message has been sent */
                    Serial.print( (char) 1 );
                } else {
                    /* Signal that we failed */
                    Serial.print( (char) 0 );
                }

                break;

            default:
                /* I'd rather just do nothing in response */
                break;
        }
    }
}



