/*******************************************
 Maze Solver
 
 Sparki has an array of infrared sensors
 underneath. It can use these to solve
 a printed maze. More info on the following
 link: 
 
 http://arcbotics.com/lessons/maze-solving/
 
********************************************/

#include <Sparki.h> // include the sparki library

const int threshold = 500; //line sensors thereshold.
const int sensorsDistance = 7; //distance [cm] between the line sensor and the wheels centers plus half the width of the black line.
const int turnSpeed = 40; //used to turn the robot over an external center of rotation.
const int outOfTheLineAngle = 30; //used to go outside the line and then find the branches.

bool  edgeLeft = false,
      lineLeft = false,
      lineCenter = false,
      lineRight = false,
      edgeRight = false;
      
String state = "undefined";

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
  sparki.println(String("state = ") + state);

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


bool isThereALeftBranch()
{
  //both left sensors read black:
  return !lineLeft && !edgeLeft;
}

bool isThereARightBranch()
{
  //both right sensors read black:
  return !lineRight && !edgeRight;
}

bool isThereADeadEnd()
{
  //all sensors read white:
  return edgeLeft && lineLeft && lineCenter && lineRight && edgeRight;
}

void centerRobot()
{
  while(robotIsNotCentered())
  {
    readSensors();
    showSensorsAndState();
  }
  sparki.beep();
}

void turnLeft()
{
  state = "turn left";
  sparki.moveForward(sensorsDistance);
  sparki.moveLeft(outOfTheLineAngle);
  sparki.moveLeft(); //turn left until the robot is centered.
  centerRobot();
}

void turnRight()
{
  state = "turn right";
  sparki.moveForward(sensorsDistance);
  sparki.moveRight(outOfTheLineAngle);
  sparki.moveRight(); //turn right until the robot is centered.
  centerRobot();
}

void turnBack()
{
  state = "turn back";
  sparki.moveForward(sensorsDistance);
  sparki.moveLeft(); //turn left until the robot is centered.
  centerRobot();
}

void mazeSolved()
{
  //tells the user that the maze was solved!
  state = "maze solved!";
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
  state = "follow line";
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
  else if (isThereADeadEnd())
  {
    turnBack();
  }
  else if (!edgeLeft && lineCenter && !edgeRight) //black-white-black is the mark present on the maze's end.
  {
    mazeSolved();
    while(true); //maze solved! => program ends here!
  }
}

void setup()
{
  //indicates to the user that the program started:
  sparki.beep(440, 300);
  delay(300);
  sparki.beep(880, 500);

  //now the user has to center de robot manually, with
  //the lineCenter sensor over the black maze line:
  state = "centering";
  centerRobot();
}

void loop()
{
  readSensors();

  followLine();
  if (isThereALeftBranch())
  {
    turnLeft();
  }
  else if (isThereARightBranch())
  {
    turnRight();
  }
  showSensorsAndState();
}
