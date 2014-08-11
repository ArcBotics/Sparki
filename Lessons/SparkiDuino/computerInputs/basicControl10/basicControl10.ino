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
      //Actions:
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
        
      //Sensor readings:
      case 'p':
        Serial.print("ping=");
        Serial.println(sparki.ping());
        break;
      case '0':
        Serial.print("edgeL=");
        Serial.println(sparki.edgeLeft());
        break;
      case '1':
        Serial.print("lineL=");
        Serial.println(sparki.lineLeft());
        break;
      case '2':
        Serial.print("center=");
        Serial.println(sparki.lineCenter());
        break;
      case '3':
        Serial.print("lineR=");
        Serial.println(sparki.lineRight());
        break;
      case '4':
        Serial.print("edgeR=");
        Serial.println(sparki.edgeRight());
        break;
    }
  }
}

