/*******************************************
 Basic Buzzer test

 Sparki has a buzzer on its back. Try
 making it beep!
********************************************/
#include <Sparki.h>  // include the robot library

void setup()
{
}

void loop()
{
    sparki.beep(); // Sparki beeps!
    delay(1000); // wait a second (1000 milliseconds)
}
