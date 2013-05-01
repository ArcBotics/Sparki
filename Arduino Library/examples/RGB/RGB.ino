/*******************************************
 Basic RGB test
 
 Sparki has a Red, Green, Blue LED on its
 back. Using Red, Green and Blue, you can
 make any color you want. The brightness of
 each color goes from 0 (dark) to 255 (full
 brightness). What colors can you make? 
********************************************/
#include <Sparki.h> // include the robot library

SparkiClass robot; // create robot object to control a robot 

void setup()
{
  robot.begin(); // start the robot up
}

void loop()
{
  robot.RGB(255,0,0); // Make the LED maximum Red
  delay(500); // wait 0.5 seconds (500 milliseconds)
  
  robot.RGB(0,255,0); // Make the LED maximum Green
  delay(500);
  
  robot.RGB(0,0,255); // Make the LED maximum Blue
  delay(500);
}