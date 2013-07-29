/*******************************************
 Basic Light Sensor test
 
 Sparki has 3 light sensors in the front,
 one each on the left, middle and right.
 This program shows how to read the sensors
 and display the information over the serial
 port.
********************************************/
#include <Sparki.h> // include the robot library

SparkiClass robot; // create robot object to control a robot 

void setup() {
  robot.begin(); // start the robot up
}

void loop() {
  int left = robot.lightLeft(); 	// read the left side light sensor
  int middle = robot.lightCenter();     // read the middle light sensor
  int right = robot.lightRight(); 	// read the right side light sensor
  
  Serial.print("Left "); 
  Serial.println(left); 	// print the value of the left side light sensor
  
  Serial.print("Middle "); 
  Serial.println(middle); 	// print the value of the middle light sensor
  
  Serial.print("Right "); 
  Serial.println(right);	// print the value of the right side light sensor
  
  Serial.println(""); // create a blank line
  
  delay(100); // wait 0.1 seconds (100 milliseconds)
}
