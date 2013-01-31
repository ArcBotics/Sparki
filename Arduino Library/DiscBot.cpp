#include "DiscBot.h"
#include <inttypes.h>
#include <stdio.h>
#include <string.h>
#include "Arduino.h"

#include "SPI.h"

DiscBot::DiscBot()
{

}

void DiscBot::begin() {
  // Setup the SPI bus for the shift register
  SPI.begin(); 
  SPI.setClockDivider(SPI_CLOCK_DIV2); 

  // defining steps for the stepper motors
  _steps[0] = 0x10;
  _steps[1] = 0x30;
  _steps[2] = 0x20;
  _steps[3] = 0x60;
  _steps[4] = 0x40;
  _steps[5] = 0xC0;
  _steps[6] = 0x80;
  _steps[7] = 0x90;
  _steps[8] = 0x00;

  // Setup the RGBB LED pins
  pinMode(RGBLED_RED, OUTPUT);
  pinMode(RGBLED_GREEN, OUTPUT);
  pinMode(RGBLED_BLUE, OUTPUT);

  digitalWrite(RGBLED_RED, LOW);
  digitalWrite(RGBLED_RED, LOW);
  digitalWrite(RGBLED_RED, LOW);
  
  // Setup Buzzer
  pinMode(BUZZER, OUTPUT);
  digitalWrite(BUZZER, LOW);
  
  // Light Sensors
  pinMode(LIGHT_LEFT, INPUT);  
  pinMode(LIGHT_MIDDLE, INPUT); 
  pinMode(LIGHT_RIGHT, INPUT); 
  
  // IR Line Sensors
  pinMode(IR_LEFT, INPUT);  
  pinMode(IR_MIDDLE, INPUT); 
  pinMode(IR_RIGHT, INPUT); 
  
  // Setup IR Remote
  pinMode(IR_RECEIVE, INPUT);
  
  // Setup IR Send
  pinMode(IR_SEND, OUTPUT);
}

void DiscBot::beep(){
  for(short i=0; i<300; i++){
  	  digitalWrite(BUZZER, HIGH);
	  delayMicroseconds(300);
	  digitalWrite(BUZZER, LOW);
	  delayMicroseconds(300);
	}
}

void DiscBot::moveForwardCM(float CM){
  uint32_t cycles;
  cycles = uint32_t(CM*REVS_PER_CM);
  for(uint32_t i=0; i < cycles; i++){
	  for(uint8_t j=0; j < 8; j++){
		shiftRegister(_steps[7-j],_steps[j]);
		delayMicroseconds(STEP_DELAY);
	  }
  }
  shiftRegister(_steps[8],_steps[8]);
}

void DiscBot::moveForwardIN(float IN){
  uint32_t cycles;
  cycles = uint32_t(IN*REVS_PER_IN);
  for(uint32_t i=0; i < cycles; i++){
	  for(uint8_t j=0; j < 8; j++){
		shiftRegister(_steps[7-j],_steps[j]);
		delayMicroseconds(STEP_DELAY);
	  }
  }
  shiftRegister(_steps[8],_steps[8]);
}

void DiscBot::moveBackwardCM(float CM){
  uint32_t cycles;
  cycles = uint32_t(CM*REVS_PER_CM);
  for(uint32_t i=0; i < cycles; i++){
	  for(uint8_t j=0; j < 8; j++){
		shiftRegister(_steps[j],_steps[7-j]);
		delayMicroseconds(STEP_DELAY);
	  }
  }
  shiftRegister(_steps[8],_steps[8]);
}

void DiscBot::moveBackwardIN(float IN){
  uint32_t cycles;
  cycles = uint32_t(IN*REVS_PER_IN);
  for(uint32_t i=0; i < cycles; i++){
	  for(uint8_t j=0; j < 8; j++){
		shiftRegister(_steps[j],_steps[7-j]);
		delayMicroseconds(STEP_DELAY);
	  }
  }
  shiftRegister(_steps[8],_steps[8]);
}

void DiscBot::moveLeftDeg(float DEG){
  uint32_t cycles;
  cycles = uint32_t(DEG*CYCLES_PER_DEG);
  Serial.println(cycles);
  for(uint32_t i=0; i < cycles; i++){
    for(uint32_t i=0; i<8; i++){
  	  shiftRegister(_steps[7-i],_steps[7-i]);
  	  delayMicroseconds(STEP_DELAY);
    }
  }
  shiftRegister(_steps[8],_steps[8]);
}

void DiscBot::moveRightDeg(float DEG){
  uint32_t cycles;
  cycles = uint32_t(DEG*CYCLES_PER_DEG);
  Serial.println(cycles);
  for(uint32_t i=0; i < cycles; i++){
  	for(uint8_t i=0; i<8; i++){
	  shiftRegister(_steps[i],_steps[i]);
	  delayMicroseconds(STEP_DELAY);
	}
  }
  shiftRegister(_steps[8],_steps[8]);
}

void DiscBot::shiftRegister(uint8_t outByte1, uint8_t outByte2) {
    PORTD &= 0xDF;    // pull PD5 low
    SPI.transfer(outByte1);
    SPI.transfer(outByte2);
    PORTD |= 0x20;    // pull PD5 high to latch in spi transfers
}