#include <Sparki.h> // include the sparki library

const int threshold = 500; //line sensors thereshold.
const int sensorsDistance = 7; //distance [cm] between the line sensor and the wheels centers plus half the width of the black line.
const int turnSpeed = 40; //used to turn the robot over an external center of rotation.
const int outOfTheLineAngle = 30; //used to go outside the line and then find the branches.

const int FOLLOWING_LINE = 10;
const int TURNING_LEFT = 20;
const int TURNING_RIGHT = 30;
const int TURNING_BACK = 40;
const int CENTERING = 50;
const int FINISHED = 1000;

int state = FOLLOWING_LINE;

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

void showSensorsAndState()
{
  sparki.clearLCD();

  sparki.print("eL = ");
  sparki.println(edgeLeft);
  sparki.print("lL = ");
  sparki.println(lineLeft);
  sparki.print("lC = ");
  sparki.println(lineCenter);
  sparki.print("lR = ");
  sparki.println(lineRight);
  sparki.print("eR = ");
  sparki.println(edgeRight);
  sparki.println();

  //##Print state.

  sparki.updateLCD();
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

bool robotIsNotCentered()
{
  //when centered, the lineLeft, lineCenter and lineRight sensors read black (using De Morgan here):
  return !edgeLeft || lineLeft || lineCenter || lineRight || !edgeRight;
}

void finished()
{
  state = FINISHED;

  //tells the user that the work is done!
  sparki.moveStop();
  showSensorsAndState();
  sparki.beep(880, 300);
  delay(300);
  sparki.beep(440, 300);
  delay(300);
  sparki.beep(880, 600);
  delay(600);
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
  else if (edgeLeft && !lineLeft && !lineCenter && !lineRight && edgeRight) //lineLeft, lineCenter and lineRight detects black.
  {
    sparki.moveForward();
  }
  else if (edgeLeft && lineLeft && lineCenter && lineRight && edgeRight) // all sensors read white: dead end.
  {
    state = TURNING_BACK;
  }
  else if (!lineLeft && !edgeLeft)  // both left sensors read black: left branch.
  {
    state = TURNING_LEFT;
  }
//  else if (!lineRight && !edgeRight)  // both right sensors read black.
//  {
//  }
  else if (!edgeLeft && lineCenter && !edgeRight) //black-white-black is the mark present on the maze's end.
  {
    finished();
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
  
  switch (state)
  {
    case FOLLOWING_LINE:
      followLine();
      break;

    case TURNING_LEFT:
      sparki.moveForward(sensorsDistance);
      sparki.moveLeft(outOfTheLineAngle);
      sparki.moveLeft(); //turn left until the robot is centered.
      state = CENTERING;
      break;

    case TURNING_RIGHT:
      sparki.moveForward(sensorsDistance);
      sparki.moveRight(outOfTheLineAngle);
      sparki.moveRight(); //turn right until the robot is centered.
      state = CENTERING;
      break;
      
    case TURNING_BACK:
      sparki.moveLeft(); //turn left until the robot is centered.
      state = CENTERING;
      break;

    case CENTERING:
      if (robotIsNotCentered())
      {
        readSensors();
        showSensorsAndState();
      }
      else
      {
        sparki.beep();
        state = FOLLOWING_LINE;
      }
      break;

    case FINISHED:
      //Program will keep doing nothing forever.
      break;
  }

  showSensorsAndState();
}

