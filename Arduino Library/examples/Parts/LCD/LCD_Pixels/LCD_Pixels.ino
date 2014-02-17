/*******************************************
 Drawing some pixels
 
 In this case, a tiny smiley face in the
 upper left hand corner
 
 http://arcbotics.com/products/sparki/parts/lcd/
********************************************/
#include <Sparki.h>  // include the sparki library

void setup()
{
}

void loop()
{
    sparki.clearLCD(); // wipe the LCD clear
    //draw a smiley face in the upper left hand corner
    
    // left eye
    sparki.drawPixel(4, 4);
    sparki.drawPixel(4, 5);
    sparki.drawPixel(5, 4);
    sparki.drawPixel(5, 5);
    
    // right eye
    sparki.drawPixel(10, 4);
    sparki.drawPixel(10, 5);
    sparki.drawPixel(11, 4);
    sparki.drawPixel(11, 5);

    //smile
    sparki.drawPixel(4, 8);
    sparki.drawPixel(5, 9);
    sparki.drawPixel(6, 10);
    sparki.drawPixel(7, 11);
    sparki.drawPixel(8, 11);
    sparki.drawPixel(9, 10);
    sparki.drawPixel(10, 9);
    sparki.drawPixel(11, 8);  

    sparki.updateLCD(); // put the drawings on the screen
    delay(1000);
}


