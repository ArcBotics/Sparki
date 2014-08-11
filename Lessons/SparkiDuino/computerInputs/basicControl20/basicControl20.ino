#include <Sparki.h>  // include the sparki library

int edgeThreshold = 200;

void setup()
{
  sparki.servo(SERVO_CENTER);
}

//Send an asyncronous event to fire an alarm on the computer side:
void processEvents()
{
  if (sparki.ping() < 5) //If the robot is going to crash against and obstacle, send this message!
  {
    Serial.println("obstacle!");
  }
  
  if (sparki.edgeLeft() < edgeThreshold) //if an edge is detected, send this message!
  {
    Serial.println("leftEdge!");
  }
  
  if (sparki.edgeRight() < edgeThreshold) //if an edge is detected, send this message!
  {
    Serial.println("rightEdge!");
  }
  
  delay(20);
}


void loop()
{
  //Process the asyncronous events:
  processEvents();
  
  //Receives commands from remote computer:
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

