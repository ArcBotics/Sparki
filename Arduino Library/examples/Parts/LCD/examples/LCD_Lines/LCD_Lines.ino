/*******************************************
 Drawing a triangle using liness
  
 http://arcbotics.com/products/sparki/parts/lcd/
********************************************/
#include <Sparki.h>  // include the sparki library

void setup()
{
}

void loop()
{
    sparki.clearLCD(); // wipe the LCD clear
    sparki.drawLine(5,40,30,10);  //left side of trangle
    sparki.drawLine(30,10,55,40); // right side of triangle
    sparki.drawLine(5,40,55,40); // bottom of triangle
    sparki.updateLCD(); // put the drawings on the screen
    delay(1000);
}
