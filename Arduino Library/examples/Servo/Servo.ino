/*******************************************
 Basic Servo test
 
 Move the servo on Sparki's head. The servo
 can rotate from 0 to 180 degrees. 90 is
 facing forward, 0 is facing right and 180
 is facing left.
********************************************/
#include <Sparki.h> // include the robot library
#include <Servo.h> // include the servo library

SparkiClass robot; // create robot object to control a robot 
Servo myservo;  // create servo object to control a servo 

void setup() 
{ 
  robot.begin(); // start the robot up
  myservo.attach(18); // start the servo up
} 
 
void loop() 
{ 
  myservo.write(0); // rotate the servo to is 0 degree postion (right)
  delay(1000);  // wait a second (1000 milliseconds)
  
  myservo.write(90); // rotate the servo to is 90 degree postion (forward)
  delay(1000); 
  
  myservo.write(180); // rotate the servo to is 180 degree postion (left)
  delay(1000); 
} 