#include <Sparki.h> // include the sparki library

char glop = 100; // declares a variable of type character

void setup()
{
}

void loop()
{
  sparki.clearLCD(); // erase the LCD

  sparki.println(glop); // print character value

  sparki.println(int(glop)); //print ASCII value

  glop ++; // add one to character value

  sparki.updateLCD(); // put what has been drawn onto the screen
  delay(1000); // wait 1 seconds (1000 milliseconds)
}




