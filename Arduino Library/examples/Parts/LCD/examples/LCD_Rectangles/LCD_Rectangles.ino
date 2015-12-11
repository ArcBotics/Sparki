/*******************************************
 Drawing two rectangles

 http://arcbotics.com/products/sparki/parts/lcd/
********************************************/
#include <Sparki.h>  // include the sparki library

void setup()
{
}

void loop()
{
    sparki.clearLCD(); // wipe the LCD clear
    sparki.drawRect(5,5,30,10); //small empty rectangle in upper-left
    sparki.drawRectFilled(20,20,100,50); // big rectangle in lower-right
    sparki.updateLCD(); // put the drawings on the screen
    delay(1000);
}
