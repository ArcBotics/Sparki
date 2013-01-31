#include "DiscBot.h"

DiscBot robot;

int sensorValue = 0;

void setup() {
  robot.begin();
  Serial.begin(115200);
}

void loop() {
  sensorValue = analogRead(LIGHT_LEFT);  
  Serial.print("Light Left: "); 
  Serial.println(sensorValue);  
  delay(100);                  

  sensorValue = analogRead(LIGHT_MIDDLE);  
  Serial.print("Light Middle: "); 
  Serial.println(sensorValue);  
  delay(100); 

  sensorValue = analogRead(LIGHT_RIGHT);  
  Serial.print("Light Middle: "); 
  Serial.println(sensorValue);  
  delay(100); 
  
  Serial.println("");
  delay(1000);
}
