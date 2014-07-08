#include <Sparki.h> // include the sparki library

void setup()
{
  randomSeed(analogRead(0));  
}

void loop()
{
  int distance = random(5, 25);
  int angle = random(30, 360);
  
  sparki.clearLCD();
  sparki.moveForward(distance);
  sparki.moveLeft(angle);
  
  sparki.print("distance = ");
  sparki.println(distance);
  sparki.print("angle = ");
  sparki.println(angle);
  sparki.updateLCD();
  
  delay(100);
}
