/*******************************************
 Edge Avoidance
 
 Sparki has an array of infrared sensors
 underneath. It can use these to detect
 if there is anything underneath it. 
 
 This program detects if Sparki is about to
 fall off the edge. If it is, it turns the
 other way before continuing.
********************************************/
#include <Sparki.h> // include the sparki library

void setup()
{       
}

void loop() {
  int edgeLeft   = sparki.edgeLeft();   // measure the left edge IR sensor
  int edgeRight  = sparki.edgeRight();  // measure the right edge IR sensor
  
  int threshold = 200; // if below this value, no surface underneath
  
  if (edgeLeft < threshold) // if no surface underneath left sensor
  {
    sparki.moveRight(20); // turn right
  }

  if (edgeRight < threshold) // if no surface underneath right sensor
  {
    sparki.moveLeft(20); // turn left
  }

  sparki.moveForward(); // move forward
  delay(100); // wait 0.1 seconds
}
