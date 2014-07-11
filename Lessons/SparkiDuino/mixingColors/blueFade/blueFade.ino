#include <Sparki.h> // include the sparki library

int B = 100;

void setup()
{
}

void loop()
{
  sparki.RGB(0, 0, B);

  if (B < 100)
    B+= 5;
  else
    B = 0;
   
  delay(100);
}
