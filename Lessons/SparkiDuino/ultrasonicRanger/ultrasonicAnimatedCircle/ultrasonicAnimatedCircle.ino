#include <Sparki.h>  // include the sparki library

int diameter = 0;

void setup()
{
}

void loop()
{
  // refresh all the display data at once:
  sparki.clearLCD();

  diameter = sparki.ping()*32/350; // 350 is the maximum aprox. reading (3.5 m)
  if (diameter > 32) // max vertical diameter, since there are just 64 pixels
  {
    sparki.beep();
    diameter = 32;
  }

  sparki.drawCircleFilled(63,32, diameter);

  sparki.updateLCD();

  delay(150); 
}
