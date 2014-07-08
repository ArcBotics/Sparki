/*******************************************
 Printing text using the Serial (Bluetooth) port

 http://arcbotics.com/lessons/using-usb-serial-and-bluetooth-communications/
 
********************************************/
#include <Sparki.h>  // include the sparki library

void setup()
{
  Serial1.begin(9600);
}

void loop()
{
  Serial1.println("Hello World");
  delay(1000);
}

