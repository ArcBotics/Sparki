#include "DiscBot.h"

DiscBot robot;

void setup() {
  robot.begin();
}

void loop() {
  robot.beep();
  delay(1000);
}