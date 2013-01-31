#include "DiscBot.h"

DiscBot robot;
Display lcd;
Accelerometer accel;

float accelG[3];

void setup()
{
  robot.begin();
  lcd.begin();
  accel.begin();
}

void loop()
{  
  accel.readGs(accelG);

  lcd.setCursor(0,0);
  lcd.print("X ");
  lcd.print(accelG[0], 4);
  
  lcd.setCursor(0,1);
  lcd.print("Y ");
  lcd.print(accelG[1], 4);

  Serial.println();
  
  delay(200);  // Delay here for visibility
}