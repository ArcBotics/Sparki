#include <Sparki.h> // include the sparki library

void setup()
{
  for (int i=0; i<=3; i++)
  {
    sparki.moveForward();
    delay(1000);
    sparki.moveLeft();
    delay(2018); // aprox. 90 degrees with the current speed
  }

  sparki.moveStop();
}

void loop()
{
}
