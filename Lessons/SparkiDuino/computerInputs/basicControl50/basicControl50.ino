#include <Sparki.h>  // include the sparki library

//#define serial Serial
#define serial Serial1

void setup()
{
  serial.begin(9600); // necessary for the Bluetooth.
  sparki.servo(SERVO_CENTER);
}

void loop()
{
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
      case 'r':
        serial.print("<p=");
        serial.print(sparki.ping());
        serial.println("=p>");

        serial.print("<0=");
        serial.print(sparki.edgeLeft());
        serial.println("=0>");

        serial.print("<1=");
        serial.print(sparki.lineLeft());
        serial.println("=1>");

        serial.print("<2=");
        serial.print(sparki.lineCenter());
        serial.println("=2>");

        serial.print("<3=");
        serial.print(sparki.lineRight());
        serial.println("=3>");

        serial.print("<4=");
        serial.print(sparki.edgeRight());
        serial.println("=4>");
        break;
    }
  }
}

