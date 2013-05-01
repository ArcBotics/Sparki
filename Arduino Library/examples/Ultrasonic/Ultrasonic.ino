/*******************************************
 Basic Ultrasonic test
 
 Send the distance Sparki's eyes are reading
 over the serial connection. Sparki will
 beep when something is too close.
 if it measures -1, that means the sensor
 is either too close or too far from an object
********************************************/
#include <Sparki.h> // include the robot library

SparkiClass robot; // create robot object to control a robot 

void setup()
{
  robot.begin(); // start the robot up
}

void loop()
{
    int cm = robot.ping(); // measures the distance with Sparki's eyes
    
    Serial.println(cm); // tells the distance to the computer
    
    if(cm != -1) // make sure its not too close or too far
    { 
        if(cm < 10) // if the distance measured is less than 10 centimeters
        {
            robot.beep(); // beep!
        }
    }
    delay(100); // wait 0.1 seconds (100 milliseconds)
}