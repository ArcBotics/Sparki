#include <Sparki.h> // include the sparki library

float B = 0.0;

void setup()
{
}

void loop()
{
  B += 0.1;
  sparki.RGB(0, 0, 100*abs(sin(B)));
  delay(100);
}
