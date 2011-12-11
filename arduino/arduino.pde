#include <Wire.h>
#include <Servo.h>

#include "AndroidAccessory.h"
#include "CapSense.h"
#include "SHT_15.h"

#define  humidPowerPin  8
#define  theDataPin     10
#define  theClockPin    11
#define  rainDataPin    A0

AndroidAccessory acc( "Google, Inc.",
                      "DemoKit",
                      "DemoKit Arduino Board",
                      "1.0",
                      "http://www.android.com",
                      "0000000012345678" );

void setup();
void loop();

void setup()
{
  Serial.begin(115200);
  Serial.print("\r\nStart");

  pinMode(24,INPUT);
  pinMode(25,INPUT);

  pinMode(humidPowerPin, OUTPUT);
  digitalWrite(humidPowerPin, HIGH);

  acc.powerOn();
}

void loop()
{
  byte err;
  byte idle;
  static byte count = 0;
  byte msg[3];
  long touchcount;

  if (acc.isConnected()) {
    char cmd = 0;
    int val;

    while (Serial.available() > 0) {
      cmd = Serial.read();

      switch (cmd) {
      case 't':  //Read Temperature
          float temp;

          sendCommandSHT( gTempCmd, theDataPin, theClockPin );
          waitForResultSHT( theDataPin);
          val = getData16SHT( theDataPin, theClockPin );
          skipCrcSHT( theDataPin, theClockPin );

          temp = -40.0 + 0.018 * (float) val;

          Serial.print( (char) (temp + 40) );    // offset of 40
          
          break;
      
      case 'h': //Read Humidity
          float humid;

          sendCommandSHT( gHumidCmd, theDataPin, theClockPin );
          waitForResultSHT( theDataPin );
          val = getData16SHT( theDataPin, theClockPin );
          skipCrcSHT( theDataPin, theClockPin );

          humid = -4.0 + 0.0405 * val + -0.0000028 * val * val;

          Serial.print( (char) humid );    // what is the range of humidity?
          
          break; 
          
      case 'c': //Output from PC
          // Read the next 6 bytes from the input buffer into a byte array
          // Then send byte array out with acc.write(array, 5);
          // Assumes PC is sending data coded as bytes, which can be decoded 
          // By the phone.
          
          delay(5);
          
          unsigned char msg[7];
           
          msg[0] = 0xFF;

          for( int i = 1; (Serial.available() > 0) && (i < sizeof(msg)); i++ ) {
            msg[i] = (unsigned char) Serial.read();
            
            delay( 1 );
          }

          /*
          msg[1] = 1;
          msg[2] = 0;
          msg[3] = 1;
          msg[4] = 70;
          msg[5] = 240;
          msg[6] = 200;    // 89.48 mph
           */

          acc.write( msg, sizeof( msg ) );
          
          //Serial.print( "poo" );
          
          Serial.print( "" );
          
          break;
          
      case 'r':
          Serial.print( (char) (analogRead( rainDataPin ) / 10) );    // factor of 10

          break;
          
      default:
          Serial.print( (char) 0 );
      }
    }
  }
}



