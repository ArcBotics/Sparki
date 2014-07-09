#include <Sparki.h> // include the sparki library

void setup()
{
  for (int i=0; i<=1; i++)
  {
    sparki.moveForward(10.0);
    sparki.moveLeft(90.0);
    sparki.moveForward(20.0);
    sparki.moveLeft(90.0);
  }

  sparki.moveStop();
}

void loop()
{
}