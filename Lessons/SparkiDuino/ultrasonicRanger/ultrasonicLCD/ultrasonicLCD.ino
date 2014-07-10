#include <Sparki.h>  // include the sparki library

void setup()
{
}

void loop()
{
    sparki.clearLCD(); // wipe the LCD clear
    sparki.print(sparki.ping()); // reads the ultrasonic ranger and prints the result in the LCD
    sparki.println(" cm");
    sparki.updateLCD(); // put the drawings on the screen
    delay(100);
}
