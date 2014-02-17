/*******************************************
 Status LED test
 
 Sparki has a red status LED, next to the
 power on LED. This code first blinks it
 on then off, then fades it on then off.
 
 http://arcbotics.com/products/sparki/parts/status-led/
********************************************/
#include <Sparki.h>  // include the sparki library

void setup()
{
}

void loop()
{
    digitalWrite(STATUS_LED, HIGH); // will turn the LED on
    delay(500); // wait 0.5 seconds
    digitalWrite(STATUS_LED, LOW); // will turn the LED off
    delay(500); // wait 0.5 seconds
    
    for(int i=0; i<255; i++) // pulse the LED on
    {
      analogWrite(STATUS_LED, i);
      delay(5);
    }
    
    for(int i=255; i>0; i--) // pulse the LED off
    {
      analogWrite(STATUS_LED, i); // will turn the LED 100% on
      delay(5);
    }
}
