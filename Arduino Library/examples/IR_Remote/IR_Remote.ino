#include "DiscBot.h"

DiscBot robot;
IRRemote remote(IR_RECEIVE);

void setup() {
  robot.begin();
  Serial.begin(115200);
  delay(1000);
}


void loop() {
  digitalWrite(RGBLED_GREEN, HIGH);
  delay(100);
  digitalWrite(RGBLED_GREEN, LOW);
  
  while(true){
    int rem = remote.listen();
    switch(rem) 
    {
        case REMOTE_PLUS:
            robot.moveForwardCM(10);
            break;
        case REMOTE_MINUS:
            robot.moveBackwardCM(10);
            break;
        case REMOTE_REWIND:
            robot.moveLeftDeg(20);
            break;
        case REMOTE_FF:
            robot.moveRightDeg(20);
            break;
        case REMOTE_BACK:
            robot.beep();
            break;
         case REMOTE_1:
            digitalWrite(RGBLED_RED, HIGH);
            delay(200);
            digitalWrite(RGBLED_RED, LOW);
            break;
         case REMOTE_2:
            digitalWrite(RGBLED_GREEN, HIGH);
            delay(200);
            digitalWrite(RGBLED_GREEN, LOW);
            break;
         case REMOTE_3:
            digitalWrite(RGBLED_BLUE, HIGH);
            delay(200);
            digitalWrite(RGBLED_BLUE, LOW);
            break;           
        default :
            break;
    }
  }
}