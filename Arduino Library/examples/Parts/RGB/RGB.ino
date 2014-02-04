/*******************************************
 Basic RGB test
 
 Sparki has a Red, Green, Blue LED on its
 back. Using Red, Green and Blue, you can
 make any color you want. The brightness of
 each color goes from 0 (dark) to 100 (full
 brightness). What colors can you make? 
********************************************/
#include <Sparki.h> // include the sparki library

void setup()
{
}

void loop()
{
  sparki.RGB(255,0,0); // Make the LED maximum Red
  delay(500); // wait 0.5 seconds (500 milliseconds)

  sparki.RGB(0,255,0); // Make the LED maximum Green
  delay(500);

  sparki.RGB(0,0,255); // Make the LED maximum Blue
  delay(500);

  sparki.RGB(RGB_WHITE); // Make the LED white (all colors)
  delay(500);

  sparki.RGB(RGB_OFF); // Make the LED white (all colors)
  delay(500);
}
