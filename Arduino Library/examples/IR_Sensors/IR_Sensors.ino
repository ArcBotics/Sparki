#include "DiscBot.h"

DiscBot robot;

int sensorValue = 0;

void setup() {
  robot.begin();
  Serial.begin(115200);
}

void loop() {
  sensorValue = analogRead(IR_LEFT);  
  Serial.print("IR Left: "); 
  Serial.println(sensorValue);  
  delay(100);                  

  sensorValue = analogRead(IR_MIDDLE);  
  Serial.print("IR Middle: "); 
  Serial.println(sensorValue);  
  delay(100); 

  sensorValue = analogRead(IR_RIGHT);  
  Serial.print("IR Middle: "); 
  Serial.println(sensorValue);  
  delay(100); 
  
  Serial.println("");
  delay(1000);
}
