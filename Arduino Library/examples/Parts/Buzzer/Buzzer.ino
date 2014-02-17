/*******************************************
 Basic Buzzer test
 
 Sparki has a buzzer on its back. Try
 making it beep!
 
 http://arcbotics.com/products/sparki/parts/buzzer/
********************************************/
#include <Sparki.h>  // include the sparki library

void setup()
{
}

void loop()
{
    sparki.beep(); // Sparki beeps!
    delay(1000); // wait a second (1000 milliseconds)
}
