/*******************************************
 * Sparki Line-following example (modified for maze solving)
 * This code simply helps Sparki move straight down a corridor
 * 
 * Threshold is the value that helps you 
 * determine what's black and white. Sparki's 
 * infrared reflectance sensors indicate white 
 * as close to 900, and black as around 200.
 * This example uses a threshold of 700 for 
 * the example, but if you have a narrow line, 
 * or perhaps a lighter black, you may need to 
 * adjust.
 ********************************************/

#include <Sparki.h> // include the sparki library

void setup() 
{
  //indicates to the user that the program started:
  sparki.beep(440, 300);
  delay(300);
  sparki.beep(880, 500);
}

void loop() {
  int threshold = 700;
  int lean = 15; //variable to get Sparki to lean to the left to find the wall
  boolean leaningLeft, leaningRight; //booleans to keep track of which way Sparki is leaning
  leaningLeft = false; //reset these variables
  leaningRight = false; //reset these variables

  int edgeLeft = sparki.edgeLeft();   // measure the left IR sensor
  int edgeRight = sparki.edgeRight(); // measure the left line IR sensor
  //int lineRight  = sparki.lineRight();  // measure the right IR sensor
  
  if ( edgeLeft < threshold ) // if there is a line below left edge sensor
  {  
    sparki.moveLeft(lean); //have Sparki lean to the right a little to find the wall
    leaningLeft = true;
  }
  if ( edgeRight < threshold ) //if there is a line below right edge sensor
  {
    sparki.moveRight(lean); //have Sparki lean to the left a little to find the wall
    leaningRight = true;
  }
  
  sparki.moveForward();

  sparki.clearLCD(); // wipe the screen

  sparki.print("Edge Left: "); // show left line sensor on screen
  sparki.println(edgeLeft);
  
  sparki.print("Edge Right: "); // show left line sensor on screen
  sparki.println(edgeRight);

  sparki.print("Leaning Left: "); // show center line sensor on screen
  sparki.println(leaningLeft);

  sparki.print("Leaning Right: "); // show right line sensor on screen
  sparki.println(leaningRight);

  sparki.updateLCD(); // display all of the information written to the screen

  delay(100); // wait 0.1 seconds
}

