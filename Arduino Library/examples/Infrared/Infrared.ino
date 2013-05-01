/*
  IR Sensors
  
  Sparki has sensors underneath that 
  sense objects in the way.
 */ 
#include <Sparki.h>

SparkiClass robot;

void setup() {       
  robot.begin();  
}

void loop() {
  Serial.print(robot.edgeRight());
  Serial.print("\t");
  Serial.print(robot.lineRight());
  Serial.print("\t");
  Serial.print(robot.lineCenter());
  Serial.print("\t");
  Serial.print(robot.lineLeft());
  Serial.print("\t");
  Serial.print(robot.edgeLeft());
  Serial.println();
  delay(100);
}
