#include <Sparki.h>  // include the sparki library

int x = 0;

void setup()
{
}

void loop()
{
    sparki.clearLCD(); // wipe the LCD clear

    if (x < 127)
      x++;
    else
      x = 0;

    sparki.drawCircleFilled(x,32, 10); // small filled circle vertically centered

    sparki.updateLCD(); // put the drawings on the screen

    delay(100); // delay for the animation
}
