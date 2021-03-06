/*

This is a library for the SHT_15 Temperature and Humidity sensor. Uses serial and
ACII commands to communicate with serial monitor such as HyperTerminal

*/

#ifndef SHT_15.h
#define SHT_15.h


int gTempCmd  = 0b00000011;
int gHumidCmd = 0b00000101;


int shiftIn(int dataPin, int clockPin, int numBits)
{
   int ret = 0;
   int i;

   for (i=0; i<numBits; ++i)
   {
      digitalWrite(clockPin, HIGH);
      delay(10);  // I don't know why I need this, but without it I don't get my 8 lsb of temp
      ret = ret*2 + digitalRead(dataPin);
      digitalWrite(clockPin, LOW);
   }

   return(ret);
}


void sendCommandSHT(int command, int dataPin, int clockPin)
{
  int ack;

  // Transmission Start
  pinMode(dataPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  digitalWrite(dataPin, HIGH);
  digitalWrite(clockPin, HIGH);
  digitalWrite(dataPin, LOW);
  digitalWrite(clockPin, LOW);
  digitalWrite(clockPin, HIGH);
  digitalWrite(dataPin, HIGH);
  digitalWrite(clockPin, LOW);
           
  // The command (3 msb are address and must be 000, and last 5 bits are command)
  shiftOut(dataPin, clockPin, MSBFIRST, command);

  // Verify we get the correct ack
  digitalWrite(clockPin, HIGH);
  pinMode(dataPin, INPUT);
  ack = digitalRead(dataPin);
  if (ack != LOW) {
//    Serial.println("Ack Error 0");
  }
  digitalWrite(clockPin, LOW);
  ack = digitalRead(dataPin);
  if (ack != HIGH) {
//     Serial.println("Ack Error 1");
  }
}

void waitForResultSHT(int dataPin)
{
  int ack;
  
  pinMode(dataPin, INPUT);
  
  for( int i= 0; i < 100; ++i)
  {
    delay(20);
    ack = digitalRead(dataPin);

    if (ack == LOW)
      break;
  }
  
  if (ack == HIGH) {
//    Serial.println("Ack Error 2");
  }
}

int getData16SHT(int dataPin, int clockPin)
{
  int val;
  
  // Get the most significant bits
  pinMode(dataPin, INPUT);
  pinMode(clockPin, OUTPUT);
  val = shiftIn(dataPin, clockPin, 8);//MSBFIRST );//, 8, 3);
  val *= 256;

  // Send the required ack
  pinMode(dataPin, OUTPUT);
  digitalWrite(dataPin, HIGH);
  digitalWrite(dataPin, LOW);
  digitalWrite(clockPin, HIGH);
  digitalWrite(clockPin, LOW);
           
  // Get the lest significant bits
  pinMode(dataPin, INPUT);
  val |= shiftIn(dataPin, clockPin, 8);//MSBFIRST );//, 8, 3);

  return val;
}

void skipCrcSHT(int dataPin, int clockPin)
{
  // Skip acknowledge to end trans (no CRC)
  pinMode(dataPin, OUTPUT);
  pinMode(clockPin, OUTPUT);

  digitalWrite(dataPin, HIGH);
  digitalWrite(clockPin, HIGH);
  digitalWrite(clockPin, LOW);
}

#endif
