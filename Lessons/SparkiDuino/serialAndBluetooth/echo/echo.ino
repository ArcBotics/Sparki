/*******************************************
 Simple echo program using the Serial (USB) port

 http://arcbotics.com/lessons/using-usb-serial-and-bluetooth-communications/
 
********************************************/
#include <Sparki.h>  // include the sparki library

void setup()
{
}

void loop()
{
  if (Serial.available()) 
  {
    int inByte = Serial.read();
    Serial.print((char)inByte); 
  }
}

