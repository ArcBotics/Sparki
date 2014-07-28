/*******************************************
 RGB Rainbow
 
 Sparki has a Red, Green, Blue LED on its
 back. Using Red, Green and Blue, you can
 make any color you want. This program makes
 the spectrum, all the colors you see in a
 rainbow:
 
 Red, Orange, Yellow, Green, Blue, Indigo and
 Violet (ROY G BIV)
 
 http://arcbotics.com/products/sparki/parts/rgb-led/
********************************************/
#include <Sparki.h> // include the sparki library

void setup()
{
}

void loop()
{ 
  sparki.RGB(RGB_RED);
  delay(500);

  sparki.RGB(RGB_ORANGE);
  delay(500);
  
  sparki.RGB(RGB_YELLOW);
  delay(500);
  
  sparki.RGB(RGB_GREEN);
  delay(500);
  
  sparki.RGB(RGB_BLUE);
  delay(500);
  
  sparki.RGB(RGB_INDIGO);
  delay(500);

  sparki.RGB(RGB_VIOLET);
  delay(500);

  sparki.RGB(RGB_WHITE);
  delay(500);
  
  sparki.RGB(RGB_OFF);
  delay(500);
}
