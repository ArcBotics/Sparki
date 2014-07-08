/*******************************************
 Simple echo program using the Serial (Bluetooth) port

 http://arcbotics.com/lessons/using-usb-serial-and-bluetooth-communications/
 
********************************************/
#include <Sparki.h>  // include the sparki library

void setup()
{
  Serial1.begin(9600);
}

void loop()
{
  if (Serial1.available()) 
  {
    int inByte = Serial1.read();
    Serial.print((char)inByte); 
  }
  if (Serial.available()) 
  {
    int inByte = Serial.read();
    Serial1.print((char)inByte); 
  }
}

