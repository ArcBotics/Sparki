/*******************************************
 Drawing two circles

 http://arcbotics.com/products/sparki/parts/lcd/
********************************************/
#include <Sparki.h>  // include the sparki library

void setup()
{
}

void loop()
{
    sparki.clearLCD(); // wipe the LCD clear
    sparki.drawCircle(10,10,5); //small empty circle in upper-left
    sparki.drawCircleFilled(90,40,20); // big filled circle in lower-right
    sparki.updateLCD(); // put the drawings on the screen
    delay(1000);
}
