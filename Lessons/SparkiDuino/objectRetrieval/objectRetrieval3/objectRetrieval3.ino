/*******************************************

 Ojbect Retrieval example: The robot follows
 a line looking for objects that has to be
 retrieved to the course's START mark.

********************************************/

#include <Sparki.h> // include the sparki library

const int threshold = 500; // line sensors thereshold

const int objectDistance = 3; // distance to the object where the gripper has to grip it [cm]
const int objectSize = 5; // diamter of the (cylindrical) objects
const int maxObjects = 2; // number of objects to retrieve

const int gripTime = 1500; // time that takes to the gripper effectively grip the object [milliseconds]
const int turnSpeed = 0; //used to turn the robot over an external center of rotation.

bool  lineLeft = false,
      lineCenter = false,
      lineRight = false;
      
int ping = 0;

int objectIndex = 0;

String state = "undefined";

void readSensors()
{
  //each sensor is 1 if reading white, and 0 if reading black:
  lineLeft =  sparki.lineLeft() > threshold;
  lineCenter = sparki.lineCenter() > threshold;
  lineRight =  sparki.lineRight() > threshold;
  ping = sparki.ping();
}

void showSensorsAndState()
{
  sparki.clearLCD(); // wipe the screen
  
  sparki.print("Line Left: "); // show left line sensor on screen
  sparki.println(lineLeft);
  
  sparki.print("Line Center: "); // show center line sensor on screen
  sparki.println(lineCenter);
  
  sparki.print("Line Right: "); // show right line sensor on screen
  sparki.println(lineRight);

  sparki.print("Ping: "); // ultrasonic ranger on screen
  sparki.print(ping);
  sparki.println(" cm");

  sparki.print(String("objects retrieved = "));
  sparki.println(objectIndex);
  
  sparki.println(String("state = ") + state);
  
  sparki.updateLCD(); // display all of the information written to the screen
}

void gripObject()
{
  sparki.moveStop();
  state = "gripping object";
  sparki.gripperClose();
  delay(gripTime);
  sparki.gripperStop();
}

void releaseObject()
{
  sparki.moveStop();
  state = "releasing object";
  sparki.gripperOpen();
  delay(gripTime);
  sparki.gripperStop();
}

void finished()
{
  //tells the user that the work is done:
  state = "finished";
  sparki.gripperStop();
  sparki.moveStop();
  showSensorsAndState();
  sparki.beep(880, 300);
  delay(300);
  sparki.beep(440, 300);
  delay(300);
  sparki.beep(880, 600);
  delay(600);
}

void centerRobot()
{
  readSensors(); //very important! if this is not done, lineCenter will still be false from the last reading
  while(lineCenter) // this cycle ends when the Center Line sensor detects the black line
  {
    readSensors();
    showSensorsAndState();
  }
}

void turnBack()
{
  state = "turn back";
  sparki.moveLeft(90); //turn left a fixed angle to ensure that the center sensor does not see the line
  sparki.moveLeft(); // turn left until the robot is centered
  centerRobot();
  sparki.beep(); // the line has been found!
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

void followLine(bool approachingObject)
{
  state = "follow line";
  if (approachingObject)
  {
    state = "approaching";
    if (ping <= objectDistance) // if the object is so close, stop the robot
    {
      gripObject();
      turnBack();
      readSensors();
      while (lineLeft || lineCenter || lineRight) // when the START mark is reached the 3 sensors read false (black)
      {
        readSensors();
        followLine(false);
        delay(100);
      }
      sparki.beep(440, 300); // make a sound to tell the START mark is there

      sparki.moveForward((maxObjects - objectIndex) * objectSize); // passes the mark
      releaseObject();
      sparki.moveBackward(objectSize + 2); // the constant number is a small security margin to avoid the object when turning
      turnBack();
      
      objectIndex++;      
      if (objectIndex == maxObjects)
      {
        finished();
        while(true); //work done => program ends here!
      }
    }
  }
  
  if (lineLeft && !lineCenter && lineRight) // if the center line sensor is the only one reading a line
  {
    sparki.moveForward(); // move forward
  }
  else if (!lineLeft) // if the black line is below left line sensor
  {
    moveLeft(); // turn left
  }
  else if (!lineRight) // if the black line is below right line sensor
  {
    moveRight(); // turn right
  }
}

void setup()
{
  sparki.servo(SERVO_CENTER); // rotate the servo to is 0 degree postion (forward)

  //indicates to the user that the program started:
  sparki.beep(440, 300);
  delay(300);
  sparki.beep(880, 500);
}

void loop()
{  
  readSensors();
  followLine(ping < (objectDistance * 3));
  showSensorsAndState();
  delay(100); // wait 0.1 seconds
}

