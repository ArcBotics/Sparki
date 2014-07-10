#include <Sparki.h> // include the sparki library

void setup()
{
  sparki.gripperClose();  // close the robot's gripper
  delay(1500);           // for 1.5 seconds (1500 milliseconds)
  sparki.gripperStop();
}

void loop()
{
}
