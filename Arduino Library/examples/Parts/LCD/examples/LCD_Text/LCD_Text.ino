/*******************************************
 Drawing some text
 
 http://arcbotics.com/products/sparki/parts/lcd/
********************************************/
#include <Sparki.h>  // include the sparki library

void setup()
{
}

void loop()
{
    sparki.clearLCD(); // wipe the LCD clear
    sparki.drawChar(10, 1, 'a');
    sparki.drawChar(20, 2, 'b');
    sparki.drawChar(30, 3, 'c');
    
    sparki.drawString(40, 4, "123");
    sparki.updateLCD(); // put the drawings on the screen
    delay(1000);
}


