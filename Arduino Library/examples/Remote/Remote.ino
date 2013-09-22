/*
  IR Remote
  
  Sparki has a sensor that lets it receives commands from
  the included remote control. Try moving it around!
 */ 
#include <Sparki.h>

SparkiClass robot;

void setup() {  
  robot.begin(); 
}
// /------^-----\
// |            |
// | 69  70  71 |
// | 68  64  67 |
// |  7  21   9 |
// | 22  25  13 |
// | 12  24  94 |
// |  8  28  90 |
// | 66  82  74 |
// \____________/

void loop() {  
  int code = robot.readIR();

  if(code != -1){
    Serial.println(code);
  }

  switch(code){
    
  // Movement buttons
  case 64: robot.moveForward(); break;
  case 25: robot.moveBackward(); break;
  case 9:  robot.moveRight(); break;
  case 7:  robot.moveLeft(); break;
  case 21: robot.moveStop(); break;

  // Gripper Buttons
  case 13:  robot.gripOpen(); break;
  case 22:  robot.gripClose(); break;

  default:
    break;
  }

}

