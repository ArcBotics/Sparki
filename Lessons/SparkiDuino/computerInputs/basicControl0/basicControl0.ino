#include <Sparki.h>  // include the sparki library

void setup()
{
}

void loop()
{
  if (Serial.available()) 
  {
    int inByte = Serial.read();
    Serial.println((char)inByte);
    
    switch ((char)inByte)
    {
      case 'w':
        sparki.moveForward();
        break;
      case 'd':
        sparki.moveRight();
        break;
      case 'a':
        sparki.moveLeft();
        break;
      case 'x':
        sparki.moveBackward();
        break;
      case 's':
        sparki.moveStop();
        break;
    }
  }
}


