/*******************************************
 Basic Gripper test

 Sparki has two little grippers it can be 
 used to grab objects and drag them around.
 See what you can grab with them!
********************************************/
#include <Sparki.h> // include the robot library

void setup()
{
}

void loop()
{
  sparki.gripperOpen();  // open the robot's gripper
  delay(1000);           // for 1 second (1000 milliseconds)

  sparki.gripperClose(); // close the robot's gripper
  delay(1000);           // for 1 second (1000 milliseconds)

  sparki.gripperStop();  // stop the grippers from moving
  delay(1000);           // for 1 second (1000 milliseconds)
}
