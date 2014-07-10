#include <Sparki.h> // include the sparki library

void setup() 
{
}

void loop() {
  int left   = sparki.lightLeft();   // measure the left light sensor
  int center = sparki.lightCenter(); // measure the center light sensor
  int right  = sparki.lightRight();  // measure the right light sensor

  if ( (center > left) && (center > right) ){  // if the center light is the strongest
    sparki.moveForward(); // move Sparki Forward
  }

  if ( (left > center) && (left > right) ){    // if the left light is the strongest
    sparki.moveLeft(); // move Sparki Left
  }

  if ( (right > center) && (right > left) ){   // if the right light is the strongest
    sparki.moveRight(); // move Sparki Right
  }

  delay(100); // wait 0.1 seconds
}
