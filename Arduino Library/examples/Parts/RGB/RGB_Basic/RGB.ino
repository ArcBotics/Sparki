/*******************************************
 Basic RGB test
 
 Sparki has a Red, Green, Blue LED on its
 back. Using Red, Green and Blue, you can
 make any color you want. The brightness of
 each color goes from 0 (dark) to 100 (full
 brightness). What colors can you make? 
 
 Here are Sparki's colors:
             R    G    B
 RGB_RED     100, 0,   0
 RGB_ORANGE  90,  100, 0
 RGB_YELLOW  60,  100, 0
 RGB_GREEN   0,   100, 0
 RGB_BLUE    0,   0,   100
 RGB_PINK    90,  0,   100
 RGB_INDIGO  20,  0,   100
 RGB_VIOLET  60,  0,   100
 RGB_WHITE   60,  100, 90
 RGB_OFF     0,   0,   0
 
 
 http://arcbotics.com/products/sparki/parts/rgb-led/
********************************************/
#include <Sparki.h> // include the sparki library

void setup()
{
}

void loop()
{
  sparki.RGB(100,0,0); // Make the LED maximum Red
  delay(500); // wait 0.5 seconds (500 milliseconds)

  sparki.RGB(0,100,0); // Make the LED maximum Green
  delay(500);

  sparki.RGB(0,0,100); // Make the LED maximum Blue
  delay(500);

  sparki.RGB(RGB_WHITE); // Make the LED white (all colors)
  delay(500);

  sparki.RGB(RGB_OFF); // Make the LED white (all colors)
  delay(500);
}
