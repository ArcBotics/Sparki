#include <Sparki.h>  // include the sparki library

//#define serial Serial
#define serial Serial1

int edgeThreshold = 200;

void setup()
{
  serial.begin(9600); // necessary for the Bluetooth.
  sparki.servo(SERVO_CENTER);
}

//Send an asyncronous event to fire an alarm on the computer side:
void processEvents()
{
  if (sparki.ping() < 5) //If the robot is going to crash against and obstacle, send this message!
  {
    serial.println("obstacle!");
  }
  
  if (sparki.edgeLeft() < edgeThreshold) //if an edge is detected, send this message!
  {
    serial.println("leftEdge!");
  }
  
  if (sparki.edgeRight() < edgeThreshold) //if an edge is detected, send this message!
  {
    serial.println("rightEdge!");
  }
  
  delay(20);
}


void loop()
{
  //Process the asyncronous events:
  processEvents();
  
  //Receives commands from remote computer:
  if (serial.available()) 
  {
    int inByte = serial.read();
    serial.println((char)inByte);
    
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
        serial.print("ping=");
        serial.println(sparki.ping());
        break;
      case '0':
        serial.print("edgeL=");
        serial.println(sparki.edgeLeft());
        break;
      case '1':
        serial.print("lineL=");
        serial.println(sparki.lineLeft());
        break;
      case '2':
        serial.print("center=");
        serial.println(sparki.lineCenter());
        break;
      case '3':
        serial.print("lineR=");
        serial.println(sparki.lineRight());
        break;
      case '4':
        serial.print("edgeR=");
        serial.println(sparki.edgeRight());
        break;
    }
  }
}

