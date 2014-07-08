/*******************************************
 Simple echo program using the Serial (Bluetooth) port

 http://arcbotics.com/lessons/using-usb-serial-and-bluetooth-communications/
 
********************************************/
#include <Sparki.h>  // include the sparki library

void setup()
{
}

void loop()
{
  if (Serial1.available()) 
  {
    int inByte = Serial1.read();
    Serial1.print((char)inByte); 
  }
}

