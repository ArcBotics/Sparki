/*******************************************
 Basic Servo test
 
 Move the servo on Sparki's head. The servo
 can rotate from 0 to 180 degrees. 90 is
 facing forward, 0 is facing right and 180
 is facing left.
********************************************/
#include <Sparki.h> // include the robot library

SparkiClass robot; // create robot object to control a robot 

void setup() 
{ 
  robot.begin(); // start the robot up
} 
 
void loop() 
{ 
  robot.writeServo(SERVO_RIGHT); // rotate the servo to is 90 degree postion (right)
  delay(1000);  // wait a second (1000 milliseconds)
  
  robot.writeServo(SERVO_CENTER); // rotate the servo to is 0 degree postion (forward)
  delay(1000); 
  
  robot.writeServo(SERVO_LEFT); // rotate the servo to is -90 degree postion (left)
  delay(1000); 
} 
