#include <Sparki.h>  // include the sparki library

int y = 10;

void setup()
{
}

void loop()
{
    sparki.clearLCD(); // wipe the LCD clear

    if (y < 60)
      y += 10;
    else
      y = 0;

    sparki.drawRectFilled(10,0, 15, y);

    sparki.updateLCD(); // put the drawings on the screen

    delay(150); // delay for the animation
}
