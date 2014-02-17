/*******************************************
 Printing text

 http://arcbotics.com/products/sparki/parts/lcd/
********************************************/
#include <Sparki.h>  // include the sparki library

void setup()
{
}

void loop()
{
    sparki.clearLCD(); // wipe the LCD clear
    sparki.print("abc");
    sparki.println("def");
    
    sparki.println("123");
    sparki.println("456");
    sparki.updateLCD(); // put the drawings on the screen
    delay(1000);
}
