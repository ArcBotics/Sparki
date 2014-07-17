#include <Sparki.h> // include the sparki library

void setup()
{
  sparki.servo(SERVO_CENTER); // makes the ultrasonic sensor to point forward
}

void loop()
{
  while (sparki.ping() > 15)  // here we can set different distances to detect objects, 
                              //depending on the size of our perimeter
  {
    sparki.moveLeft(); // rotates the robot to the left
    delay(150); // this brief delay is to give enough time to the ultrasonic sensor to work
  }
  delay(250); // instead of stopping immediately, rotates a bit more to center the object

  sparki.moveStop(); // stops all the robot's movements
  delay(100); //waits a few milliseconds before start moving, just to be nice with the motors
  
  while (sparki.lineCenter() > 700)  // here we can set different thresholds to detect the line
  {
    sparki.moveForward(); // rotates the robot to the left
  }

  sparki.moveStop(); // stops all the robot's movements
  delay(100); //waits a few milliseconds before start moving, just to be nice with the motors    
  
  sparki.moveBackward();
  delay(1000); // the bigger this number is, the bigger the distance that the Sparki goes backward
  
  sparki.moveLeft(); // just in case: with this, we prevent Sparki to detect again the same object
  delay(400);
}
