#include <Sparki.h> // include the sparki library

const int threshold = 500; //line sensors thereshold.
const int turnSpeed = 40; //used to turn the robot over an external center of rotation.

bool  edgeLeft = false,
      lineLeft = false,
      lineCenter = false,
      lineRight = false,
      edgeRight = false;
      

void readSensors()
{
  //each sensor is 1 if reading white, and 0 if reading black:
  edgeLeft =  sparki.edgeLeft() > threshold;
  lineLeft =  sparki.lineLeft() > threshold;
  lineCenter = sparki.lineCenter() > threshold;
  lineRight =  sparki.lineRight() > threshold;
  edgeRight = sparki.edgeRight() > threshold;
}


void moveLeft()
{
  //turn left at a lower speed:
  sparki.motorRotate(MOTOR_LEFT, DIR_CCW, turnSpeed);
  sparki.motorRotate(MOTOR_RIGHT, DIR_CW, 100);
}

void moveRight()
{
  //turn right at a lower speed:
  sparki.motorRotate(MOTOR_LEFT, DIR_CCW, 100);
  sparki.motorRotate(MOTOR_RIGHT, DIR_CW, turnSpeed);
}

void followLine()
{
  if (lineLeft && !lineRight) //lineLeft detects white and at least lineRight detects black.
  {
    moveRight();
  }
  else if (!lineLeft && lineRight) //lineRight detects white and at least lineLeft detects black.
  {
    moveLeft();
  }
  else if (!lineLeft && !lineCenter && !lineRight) //lineLeft, lineCenter and lineRight detects black.
  {
    sparki.moveForward();
  }
}

void setup()
{
  //indicates to the user that the program started:
  sparki.beep(440, 300);
  delay(300);
  sparki.beep(880, 500);
}

void loop()
{
  readSensors();

  followLine();
}
