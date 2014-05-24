/*******************************************
  IR Remote
  
  Sparki has a sensor that lets it receives
  commands from the included remote control.
  Try moving it around!
  
  http://arcbotics.com/infrared-remote-receiver/
********************************************/
#include <Sparki.h> // include the sparki library

void setup() 
{ 
  sparki.clearLCD();
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

void loop() 
{  
  int code = sparki.readIR();

  if(code != -1){
    sparki.print("Received code: ");
    sparki.println(code);
  }

  switch(code){
    
  // Movement buttons
  case 70: sparki.moveForward(); break;
  case 21: sparki.moveBackward(); break;
  case 67: 
  case 71: sparki.moveRight(); break;
  case 68:
  case 69: sparki.moveLeft(); break;
  case 64: sparki.moveStop(); 
           sparki.gripperStop();
           break;

  // Gripper Buttons
  case 9:  sparki.gripperOpen(); break;
  case 7:  sparki.gripperClose(); break;

  // buzzer
  case 74: sparki.beep(); break;

  // Servo
  case 90: sparki.servo(SERVO_LEFT); break;
  case 28: sparki.servo(SERVO_CENTER); break;
  case 8: sparki.servo(SERVO_RIGHT); break;

  // RGB LED
  case 25: sparki.RGB(RGB_OFF); break;
  case 12: sparki.RGB(RGB_RED); break;
  case 24: sparki.RGB(RGB_GREEN); break;
  case 94: sparki.RGB(RGB_BLUE); break;
  
  default:
    break;
  }

  sparki.updateLCD();
}

