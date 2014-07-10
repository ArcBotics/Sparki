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

    char text[20];
    String number = String(123);
    number.toCharArray(text, sizeof(text));

    sparki.drawString(40, 4, text);
    sparki.updateLCD(); // put the drawings on the screen
    delay(1000);
}
