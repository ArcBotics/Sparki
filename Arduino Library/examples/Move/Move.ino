#include "DiscBot.h"

DiscBot robot;
  
void setup() {
  robot.begin();
}

void loop() {
  // Move in a 20cm square
  robot.moveForwardCM(20);
  robot.moveRightDeg(90);

  robot.moveForwardCM(20);
  robot.moveRightDeg(90);

  robot.moveForwardCM(20);
  robot.moveRightDeg(90);

  robot.moveForwardCM(20);
  robot.moveRightDeg(90);
  
  // Move back in a 1ft square
  robot.moveForwardIN(12);
  robot.moveLeftDeg(90);

  robot.moveForwardIN(12);
  robot.moveLeftDeg(90);

  robot.moveForwardIN(12);
  robot.moveLeftDeg(90);

  robot.moveForwardIN(12);
  robot.moveLeftDeg(90);

  
  // Stop after moving in the square
  while(true){
    delay(10);
  }
}