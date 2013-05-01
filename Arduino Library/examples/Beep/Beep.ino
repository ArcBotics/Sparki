/*******************************************
 Basic Buzzer test
 
 Sparki has a buzzer on its back. Try
 making it beep!
********************************************/
#include <Sparki.h>  // include the robot library

SparkiClass robot; // create robot object to control a robot 

void setup()
{
  robot.begin(); // start the robot up
}

void loop()
{
    robot.beep(); // Sparki beeps!
    delay(1000); // wait a second (1000 milliseconds)
}