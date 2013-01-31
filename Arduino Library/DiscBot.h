#ifndef DiscBot_h
#define DiscBot_h

#include "DiscBot.h"
#include "Ultrasonic.h"
#include "Accelerometer.h"
#include "Display.h"
#include "TimerOne.h"
#include "IRRemote.h"
#include "SPI.h"

#define SHIFTREG_LATCH		TXLED0 	// PD5
#define RGBLED_RED			9		// PB5
#define RGBLED_BLUE			5		// PC6
#define RGBLED_GREEN		10		// PB6
#define BUZZER    			11		// PB7
#define ULTRASONIC_TRIG		17		// PB0
#define ULTRASONIC_ECHO		7		// PE6
#define IR_RECEIVE			13		// PC7
#define IR_SEND				6		// PD7


// defining the IR line sensor pins
#define IR_LEFT    			A6		// PD4
#define IR_MIDDLE    		A11		// PD6
#define IR_RIGHT    		A8		// PB4

// defining the light sensors
#define LIGHT_LEFT    		A3		// PF4
#define LIGHT_MIDDLE    	A4		// PF1
#define LIGHT_RIGHT    		A5		// PF0

// defining the buttons on the shipped-with remote control
#define REMOTE_ERROR	-2
#define REMOTE_REPEAT	-3
#define REMOTE_POWER	69
#define REMOTE_MENU		71
#define REMOTE_TEST		68
#define REMOTE_PLUS		64
#define REMOTE_BACK		67
#define REMOTE_REWIND	7
#define REMOTE_PLAY		21
#define REMOTE_FF		9
#define REMOTE_0		22
#define REMOTE_MINUS	25
#define REMOTE_C		13
#define REMOTE_1		12
#define REMOTE_2		24
#define REMOTE_3		94
#define REMOTE_4		8
#define REMOTE_5		28
#define REMOTE_6		90
#define REMOTE_7		66
#define REMOTE_8		82
#define REMOTE_9		74

#define STEP_DELAY		1000

// properties about the robot
#define WHEEL_DIAMETER_CM 		5.30
#define WHEEL_CIRCUMFERENCE_CM	16.65044
#define CYLCES_PER_REV	205.0
#define REVS_PER_CM		12.3119869
#define REVS_PER_IN		31.2724468543

#define CYCLES_DEG_90	107
#define CYCLES_PER_DEG	1.18888888889

class DiscBot {

public:
  DiscBot();
	
  void begin();
  void beep();

  void moveForwardCM(float);
  void moveForwardIN(float);
  void moveForwardFT(float);
  
  void moveBackwardCM(float);
  void moveBackwardIN(float);
  void moveBackwardFT(float);
  
  void moveLeftDeg(float);
  void moveRightDeg(float);
  
  uint8_t _steps[8]; 
  
private:

  void shiftRegister(uint8_t, uint8_t);
};

#endif

