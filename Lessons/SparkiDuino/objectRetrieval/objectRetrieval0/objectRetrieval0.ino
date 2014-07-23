#include <Sparki.h> // include the sparki library

const int threshold = 500; //line sensors thereshold.

bool  lineLeft = false,
      lineCenter = false,
      lineRight = false;


void readSensors()
{
  //each sensor is 1 if reading white, and 0 if reading black:
  lineLeft =  sparki.lineLeft() > threshold;
  lineCenter = sparki.lineCenter() > threshold;
  lineRight =  sparki.lineRight() > threshold;
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
  
  sparki.updateLCD(); // display all of the information written to the screen
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

  if (!lineLeft && !lineCenter && !lineRight) // is the robot over the START mark?
  {
    sparki.beep(440, 300); // make a sound to tell the START mark is there
    sparki.moveForward(); // continue going forward
  }  
  else if (!lineLeft) // if the black line is below left line sensor
  {
    sparki.moveLeft(); // turn left
  }
  else if (!lineRight) // if the black line is below right line sensor
  {
    sparki.moveRight(); // turn right
  }
  else if (lineLeft && !lineCenter && lineRight) // if the center line sensor is the only one reading a line
  {
    sparki.moveForward(); // move forward
  }
  
  showSensorsAndState();
  delay(100); // wait 0.1 seconds
}

