#include "DiscBot.h"

#define EDGE_LIMIT 800
DiscBot robot;

void setup() {
  robot.begin();
  Serial.begin(115200);
}

int ir_l = 0;
int ir_m = 0;
int ir_r = 0;

void loop() {
  ir_l = analogRead(IR_LEFT); 
  ir_m = analogRead(IR_MIDDLE); 
  ir_r = analogRead(IR_RIGHT);  
  
  if( (ir_l < EDGE_LIMIT) & (ir_m < EDGE_LIMIT) & (ir_r < EDGE_LIMIT) ){
    // surface is detected on all sensors, its safe to crawl forward a bit
    robot.moveForwardCM(1);
  }
  else{
    ; // dont move, you're about to go over the edge!
  }
  
   
  delay(100);         
}