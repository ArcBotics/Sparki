#include <Sparki.h> // include the sparki library

void setup() 
{
}

void loop() {
  int threshold = 500;
  
  int lineLeft   = sparki.lineLeft();   // measure the left IR sensor
  int lineCenter = sparki.lineCenter(); // measure the center IR sensor
  int lineRight  = sparki.lineRight();  // measure the right IR sensor

  if ( lineLeft > threshold ) // if line is below left line sensor
  {  
    sparki.moveRight(); // turn right
  }

  if ( lineRight > threshold ) // if line is below right line sensor
  {  
    sparki.moveLeft(); // turn left
  }

  // if the center line sensor is the only one reading a line
  if ( (lineCenter > threshold) && (lineLeft < threshold) && (lineRight < threshold) )
  {
    sparki.moveRight(); // move forward
  }  
  
  delay(100); // wait 0.1 seconds
}

