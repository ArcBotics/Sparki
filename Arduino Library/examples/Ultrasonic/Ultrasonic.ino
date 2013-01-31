#include "DiscBot.h"

DiscBot robot;
Ultrasonic ultrasonic(ULTRASONIC_TRIG, ULTRASONIC_ECHO);
Display lcd;
  
void setup() {
  robot.begin();
  lcd.begin();
  Serial.begin(115200); 
  lcd.print("Range: ");
}

void loop() {
    long rangeMsec = ultrasonic.timing();
    float cmMsec = ultrasonic.convert(rangeMsec, Ultrasonic::CM);    
    lcd.setCursor(0,1);    
    lcd.print(cmMsec);
    lcd.print("cm      ");
    if(cmMsec < 5){
      robot.beep();
    }
    delay(100);

}