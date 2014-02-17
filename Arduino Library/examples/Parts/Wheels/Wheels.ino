/*******************************************
 Basic motor test
 
 Move Sparki forward and backwards, then 
 rotate right and left, then stop. Repeat!
 Can you get Sparki to do any cool motions?
 Maybe draw something by sticking a pen down
 the center?
 
 http://arcbotics.com/products/sparki/parts/wheels/
********************************************/
#include <Sparki.h> // include the sparki library

void setup()
{
}

void loop()
{
  sparki.moveForward();// move the robot forward
  delay(1000); // wait a second (1000 milliseconds)
  
  sparki.moveBackward(); // move the robot backward
  delay(1000);
  
  sparki.moveRight(); // rotate the robot clockwise
  delay(1000);
  
  sparki.moveLeft(); // rotate rhe robot counter-clockwise
  delay(1000);
  
  sparki.moveStop(); // stop all robot wheels
  delay(2000); // wait two seconds (2000 milliseconds)
}
