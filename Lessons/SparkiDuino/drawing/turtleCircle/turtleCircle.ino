#include <Sparki.h> // include the sparki library

void setup()
{
  for (int i=0; i<=90; i++)
  {
    sparki.clearLCD();

    sparki.moveForward(1.0);
    delay(100);
    sparki.moveLeft(4.0);
    delay(100);
    
    sparki.println(i);
    sparki.updateLCD();
  }
  
  sparki.moveStop();
}

void loop()
{
}
