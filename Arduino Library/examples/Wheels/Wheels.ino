/*******************************************
 Basic motor test
 
 Move Sparki forward and backwards, then 
 rotate right and left, then stop. Repeat!
 Can you get Sparki to do any cool motions?
 Maybe draw something by sticking a pen down
 the center?
********************************************/
#include <Sparki.h> // include the robot library

SparkiClass robot; // create robot object to control a robot 

void setup()
{
  robot.begin(); // start the robot up
}

void loop()
{
  robot.moveForward();// move the robot forward
  delay(1000); // wait a second (1000 milliseconds)
  
  robot.moveBackward(); // move the robot backward
  delay(1000);
  
  robot.rotateRight(); // rotate rhe robot clockwise
  delay(1000);
  
  robot.rotateLeft(); // rotate rhe robot counter-clockwise
  delay(1000);
  
  robot.moveStop(); // stop all robot wheels
  delay(2000); // wait two seconds (2000 milliseconds)
}