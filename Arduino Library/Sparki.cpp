#include "Sparki.h"
#include <inttypes.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <Arduino.h>

#include <avr/pgmspace.h>
#include <util/delay.h>
#include <stdlib.h>
#include <SPI.h>

#include "SparkiWire.h"
#include "SparkiEEPROM.h"
#include "Sparkii2c.h"


// initialize the RGB timer variables
static volatile uint8_t RGB_vals[3];
static volatile uint16_t RGB_timer;

static volatile uint8_t irSwitch;
static volatile uint8_t irSwitch2;

// variables for communication between the IR read function and its interrupt
#define MAX_IR_PULSE 20000
volatile long timeSinceLastPulse = 0;
volatile long lastPulseTime = 0;
volatile uint16_t pulsesIR[50][2]; // LOW,HIGH
volatile uint8_t currentPulse = 0;
volatile uint8_t haltIRRead = 0;

// shares the values of the accelerometers
volatile float xAxisAccel;
volatile float yAxisAccel;
volatile float zAxisAccel;

// variables for the magnetometer
volatile uint8_t mag_buffer[RawMagDataLength];

// values for the servo
volatile int8_t servo_deg_offset = 0;

SparkiClass sparki;


SparkiClass::SparkiClass()
{
 begin();
}

void SparkiClass::begin( ) {
 
  Serial.begin(9600);
  Serial1.begin(9600);

  // set up the Status LED
  pinMode(STATUS_LED, OUTPUT);
  digitalWrite(STATUS_LED, LOW);

  // Setup Buzzer
  pinMode(BUZZER, OUTPUT);
  digitalWrite(BUZZER, LOW);
  
  // Setup Analog Multiplexer
  pinMode(MUX_ANALOG, INPUT);
  pinMode(MUX_A, OUTPUT);
  pinMode(MUX_B, OUTPUT);
  pinMode(MUX_C, OUTPUT);
  
  // Setup IR Send
  pinMode(IR_SEND, OUTPUT);
  
  // Setup Ultrasonic
  pinMode(ULTRASONIC_TRIG, OUTPUT);
  pinMode(ULTRASONIC_ECHO, INPUT);
  
  // Setup Servo
  pinMode(SERVO, OUTPUT);    
  //startServoTimer();
  if( EEPROM.read(0) > 127) {
    servo_deg_offset = -256+EEPROM.read(0);
  }
  else{
    servo_deg_offset = EEPROM.read(0);
  }
  //servo(SERVO_CENTER);
    
  
  // Setup the SPI bus for the shift register
  // !!! Need to remove the essential functions from the SPI Library, 
  // !!! and include in the main code
  SPI.begin(); 
  SPI.setClockDivider(SPI_CLOCK_DIV2); 

  // Set the shift-register clock select pin to output 
  DDRD |= (1<<5);
  
  // Clear out the shift registers
  PORTD &= 0xDF;    // pull PD5 low
  SPI.transfer(0x00);
  SPI.transfer(0x00);
  PORTD |= 0x20;    // pull PD5 high to latch in spi transfers


  // Setup the IR Switch
  irSwitch = 0;

  beginDisplay();
  updateLCD();

  // BEGIN: Motor Setup

  initMotorControlWord ( SPARKI_MOTOR_ID_MASK_ALL, 0x11 );
  initMotorStatusWord  ( SPARKI_MOTOR_ID_MASK_ALL, 0x00 );

  wheelDiameterUmEff   = SPARKI_WHEEL_DIAMETER_BASELINE_UM;
  wheelSeparationUmEff = SPARKI_WHEEL_SEPARATION_BASELINE_UM;

  driveMmToStepsEffFp = SPARKI_MOTOR_DRIVE_MM_TO_STEPS_FP;
  spinDegToStepsEffFp = SPARKI_MOTOR_SPIN_DEG_TO_STEPS_FP;
  turnRadMmAngleDegToStepsEffFp = SPARKI_TURN_RADIUSMM_ANGLEDEG_TO_STEPS_FP;

  wheelSpeedPercentDefault   = SPARKI_MOTOR_SPEED_DEFAULT_PERCENT;
  gripperSpeedPercentDefault = SPARKI_MOTOR_SPEED_DEFAULT_PERCENT;

  distanceOfZeroMeansInfinity = true;
  gripperSpacingMm = -1;

  // END: Motor Setup
  
  // Set up the scheduler routine to run every 100uS, based off Timer4 interrupt
  cli();          // disable all interrupts
  TCCR4A = 0;
  TCCR4B = 0;
  TCNT4  = 0;

  OCR4A = 48;               // compare match register 16MHz/32/10000Hz
  TCCR4B |= (1 << WGM12);   // CTC mode
  TCCR4B = 0x06;            // CLK/32 prescaler (32 = 2^(0110-1))
  TIMSK4 |= (1 << OCIE4A);  // enable Timer4 compare interrupt A
  sei();             // enable all interrupts
  
  // Setup the IR Remote Control pin and pin interrupt
  noInterrupts();
  pinMode(IR_RECEIVE, INPUT);
  
  // Setup the pin interrupt for INT6 (Pin 7) to trigger the IR function
  EICRB = (EICRB & ~((1 << ISC60) | (1 << ISC61))) | (CHANGE << ISC60);
  EIMSK |= (1 << INT6); 
  
  interrupts();
  
  initAccelerometer();
  
   WireWrite(ConfigurationRegisterB, (0x01 << 5));
   WireWrite(ModeRegister, Measurement_Continuous);  
  readMag(); // warm it up  

}

void SparkiClass::setMux(uint8_t A, uint8_t B, uint8_t C){
	digitalWrite(MUX_A, A);
	digitalWrite(MUX_B, B);
	digitalWrite(MUX_C, C);
	delay(1);
}

/* 
* Light Sensors
*/

int SparkiClass::lightRight(){
	setMux(LIGHT_RIGHT);
	return analogRead(MUX_ANALOG);
}

int SparkiClass::lightCenter(){
	setMux(LIGHT_CENTER);
	return analogRead(MUX_ANALOG);
}

int SparkiClass::lightLeft(){
	setMux(LIGHT_LEFT);
	return analogRead(MUX_ANALOG);
}

/*
* Infrared line sensors
*/


int SparkiClass::edgeRight(){
	setMux(IR_EDGE_RIGHT);
    return readSensorIR(MUX_ANALOG);
}

int SparkiClass::lineRight(){
	setMux(IR_LINE_RIGHT);
    return readSensorIR(MUX_ANALOG);
}

int SparkiClass::lineCenter(){
	setMux(IR_LINE_CENTER);
    return readSensorIR(MUX_ANALOG);
}

int SparkiClass::lineLeft(){
	setMux(IR_LINE_LEFT);
    return readSensorIR(MUX_ANALOG);
}

int SparkiClass::edgeLeft(){
	setMux(IR_EDGE_LEFT);
    return readSensorIR(MUX_ANALOG);
}

int SparkiClass::readSensorIR(int pin){
    int read = 0;
	onIR();
	read = analogRead(pin);
	offIR();
	return read;
}

void SparkiClass::onIR()  // turns off the IR Detection LEDs
{
    irSwitch = 1;
    delay(1); // give time for a scheduler cycle to run
}

void SparkiClass::offIR() // turns off the IR Detection LEDs
{
    irSwitch = 0;
    delay(1); // give time for a scheduler cycle to run
}

int SparkiClass::readBlindSensorIR(int pin0, int pin1, int pin2){
    int read = 0;
    setMux(pin0, pin1, pin2);
    delay(1);
	read = analogRead(MUX_ANALOG);
	delay(1);
	return read;
}

int SparkiClass::diffIR(int pin0, int pin1, int pin2){
    setMux(pin0, pin1, pin2);
    delay(1);
	int readOff = analogRead(MUX_ANALOG);
	delay(10);
	onIR();
	int readOn = analogRead(MUX_ANALOG);
	offIR();
	return readOff-readOn;
}

void SparkiClass::beep(){
    tone(BUZZER, 4000, 200);
}

void SparkiClass::beep(int freq){
    tone(BUZZER, freq, 200);
}

void SparkiClass::beep(int freq, int time){
    tone(BUZZER, freq, time);
}

void SparkiClass::noBeep(){
    noTone(BUZZER);
}


/*
 * motor control (non-blocking, except when moving distances)
 * speed is percent 0-100
*/

void SparkiClass::RGB(uint8_t R, uint8_t G, uint8_t B)
{
    if(R > 100){
        R = 100;
    }
    if(G > 100){
        G = 100;
    }
    if(B > 100){
        B = 100;
    }
	RGB_vals[0] = R;
	RGB_vals[1] = G;
	RGB_vals[2] = B;
}

// ----------------------------------------------------------------------------
// Motor Control: Setup and Support Functions
// ----------------------------------------------------------------------------

void SparkiClass::primeMotors () {

  cli ();
  setupMotorForSteps (
    SPARKI_MOTOR_ID_MASK_WHEEL_LEFT,
    SPARKI_MOTOR_PRIMING_STEPS,
    SPARKI_MOTOR_DIR_COUNTERCLOCKWISE,
    SPARKI_MOTOR_SPEED_DEFAULT_PERCENT,
    true
  );
  setupMotorForSteps (
    SPARKI_MOTOR_ID_MASK_WHEEL_RIGHT,
    SPARKI_MOTOR_PRIMING_STEPS,
    SPARKI_MOTOR_DIR_CLOCKWISE,
    SPARKI_MOTOR_SPEED_DEFAULT_PERCENT,
    true
  );
  setupMotorForSteps (
    SPARKI_MOTOR_ID_MASK_GRIPPER,
    SPARKI_MOTOR_PRIMING_STEPS,
    SPARKI_MOTOR_DIR_COUNTERCLOCKWISE,
    SPARKI_MOTOR_SPEED_DEFAULT_PERCENT,
    true
  );
  sei ();

  startMotorRotation ( SPARKI_MOTOR_ID_MASK_ALL, true );

}

// ----------------------------------------------------------------------------

void SparkiClass::initMotorControlWord (
  uint8_t motorIdMask,
  byte    initValue
) {

  initMotorWord ( motorControlWord, motorIdMask, initValue );

}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void SparkiClass::initMotorStatusWord (
  uint8_t motorIdMask,
  byte    initValue
) {

  initMotorWord ( motorStatusWord, motorIdMask, initValue );

}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void SparkiClass::initMotorWord (
  volatile byte *  motorWord,
           uint8_t motorIdMask,
           byte    initValue
) {

  uint8_t motorIndex;

  motorIdMask &= SPARKI_MOTOR_ID_MASK_ALL;

  motorIndex = 0;
  while ( motorIdMask != 0x00 ) {
    if ( motorIdMask & 0x01 ) {
      motorWord[motorIndex] = initValue;
    }
    motorIdMask >>= 1;
    motorIndex++;
  }

}

// ----------------------------------------------------------------------------

void SparkiClass::modifyMotorControlWordBit (
  byte    motorIdMask,
  byte    bitSelectMask,
  boolean bitValue,
  boolean motorIdIsIndex
) {

  modifyMotorWordBit ( motorControlWord, motorIdMask, bitSelectMask, bitValue, motorIdIsIndex );

}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void SparkiClass::modifyMotorStatusWordBit (
  byte    motorIdMask,
  byte    bitSelectMask,
  boolean bitValue,
  boolean motorIdIsIndex
) {

  modifyMotorWordBit ( motorStatusWord, motorIdMask, bitSelectMask, bitValue, motorIdIsIndex );

}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void SparkiClass::modifyMotorWordBit (
  volatile byte *  motorWord,  // pointer to array of motor words
           byte    motorIdMask,
           byte    bitSelectMask,
           boolean bitValue,
           boolean motorIdIsIndex
) {

  motorIdMask &= SPARKI_MOTOR_ID_MASK_ALL;

  if ( motorIdIsIndex ) {

    if ( bitValue ) motorWord[motorIdMask] |=   bitSelectMask;
    else            motorWord[motorIdMask] &= ~ bitSelectMask;

  }  // if ( motorIdIsIndex )
  else {

    uint8_t motorIndex;

    motorIndex = 0;
    while ( motorIdMask != 0x00 ) {
      if ( motorIdMask & 0x01 ) {
        if ( bitValue ) motorWord[motorIndex] |=   bitSelectMask;
        else            motorWord[motorIndex] &= ~ bitSelectMask;
      }
      motorIdMask >>= 1;
      motorIndex++;
    }

  }  // if ( motorIdIsIndex ) ... else

}

// ----------------------------------------------------------------------------

void SparkiClass::setWheelDiameter (
  uint16_t wheelDiameterUm
) {

  wheelDiameterUmEff = wheelDiameterUm;

  updateDriveMmToStepsFactor ();
  updateSpinDegToStepsFactor ();

  updateTurnRadMmAngleDegToStepsFactor ();

}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void SparkiClass::setWheelSeparation (
  uint16_t wheelSeparationUm
) {

  wheelSeparationUmEff = wheelSeparationUm;

  updateSpinDegToStepsFactor ();

}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void SparkiClass::updateDriveMmToStepsFactor () {

  uint16_t wheelDiaRatioFp;

  wheelDiaRatioFp = ( (uint32_t) SPARKI_WHEEL_DIAMETER_BASELINE_UM << 15 ) / wheelDiameterUmEff;

  driveMmToStepsEffFp = ( (uint32_t) SPARKI_MOTOR_DRIVE_MM_TO_STEPS_FP * wheelDiaRatioFp + ( 1 << 14 ) ) >> 15;

  // NOTE: Above fixed-pint precision of 15 bits assumes that wheel
  // diameter does not exceed 10 bits; with actual Sparki wheel
  // diameter around 51.0mm or 510 mm/10, this requirement is met.

}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void SparkiClass::updateSpinDegToStepsFactor () {

  uint16_t wheelDiaRatioFp;
  uint16_t wheelSepRatioFp;
  uint16_t compoundRatioFp;

  wheelDiaRatioFp = ( (uint32_t) SPARKI_WHEEL_DIAMETER_BASELINE_UM << 15 ) / wheelDiameterUmEff;
  wheelSepRatioFp = ( (uint32_t) wheelSeparationUmEff << 15 ) / SPARKI_WHEEL_SEPARATION_BASELINE_UM;
  compoundRatioFp = ( (uint32_t) wheelDiaRatioFp * wheelSepRatioFp ) >> 15;

  spinDegToStepsEffFp = ( (uint32_t) SPARKI_MOTOR_SPIN_DEG_TO_STEPS_FP * compoundRatioFp + ( 1 << 14 ) ) >> 15;

  // NOTE: Above fixed-pint precision of 15 bits assumes that wheel
  // diameter and separation do not exceed 10 bits each; with
  // Sparki wheel diameter around 51.0mm or 510 mm/10 and separation
  // around 85.5 mm or 855 mm/10, this requirement is met.

}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void SparkiClass::updateTurnRadMmAngleDegToStepsFactor () {

  uint16_t wheelDiaRatioFp;

  wheelDiaRatioFp = ( (uint32_t) SPARKI_WHEEL_DIAMETER_BASELINE_UM << 15 ) / wheelDiameterUmEff;

  turnRadMmAngleDegToStepsEffFp = ( (uint32_t) SPARKI_TURN_RADIUSMM_ANGLEDEG_TO_STEPS_FP * wheelDiaRatioFp + ( 1 << 14 ) ) >> 15;

}

// ----------------------------------------------------------------------------

void SparkiClass::enableActiveHoldForWheels (
  boolean enableActiveHold
) {

  enableActiveMotorHold ( SPARKI_MOTOR_ID_MASK_WHEELS, enableActiveHold );

}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void SparkiClass::enableActiveHoldForGripper (
  boolean enableActiveHold
) {

  enableActiveMotorHold ( SPARKI_MOTOR_ID_MASK_GRIPPER, enableActiveHold );

}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

boolean SparkiClass::activeHoldIsEnabledForWheels () {

  return activeMotorHoldIsEnabledForAny ( SPARKI_MOTOR_ID_MASK_WHEELS );

}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

boolean SparkiClass::activeHoldIsEnabledForGripper () {

  return activeMotorHoldIsEnabledForAny ( SPARKI_MOTOR_ID_MASK_GRIPPER );

}

// ----------------------------------------------------------------------------

void SparkiClass::enableActiveMotorHold (
  byte    motorIdMask, 
  boolean enableActiveHold
) {

  modifyMotorStatusWordBit ( motorIdMask, SPARKI_MOTOR_STATUS_MASK_ACT_HOLD_IS_ON, enableActiveHold );

}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

boolean SparkiClass::activeMotorHoldIsEnabledForAny (
  byte motorIdMask
) {

  boolean activeHoldIsOn;
  uint8_t motorIndex;

  motorIdMask &= SPARKI_MOTOR_ID_MASK_ALL;

  activeHoldIsOn = false;

  motorIndex = 0;
  while ( motorIdMask != 0x00 ) {
    if ( motorIdMask & 0x01 ) {
       activeHoldIsOn = activeHoldIsOn || ( motorStatusWord[motorIndex] & SPARKI_MOTOR_STATUS_MASK_ACT_HOLD_IS_ON );
    }
    motorIdMask >>= 1;
    motorIndex++;
  }

  return activeHoldIsOn;

}

// ----------------------------------------------------------------------------

void SparkiClass::enableBacklashCompForWheels (
  boolean enableBacklashComp
) {

  enableMotorBacklashComp ( SPARKI_MOTOR_ID_MASK_WHEELS, enableBacklashComp );

}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void SparkiClass::enableBacklashCompForGripper (
  boolean enableBacklashComp
) {

  enableMotorBacklashComp ( SPARKI_MOTOR_ID_MASK_GRIPPER, enableBacklashComp );

}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

boolean SparkiClass::backlashCompIsEnabledForWheels () {

  return motorBacklashCompIsEnabledForAny ( SPARKI_MOTOR_ID_MASK_WHEELS );

}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

boolean SparkiClass::backlashCompIsEnabledForGripper () {

  return motorBacklashCompIsEnabledForAny ( SPARKI_MOTOR_ID_MASK_GRIPPER );

}

// ----------------------------------------------------------------------------

void SparkiClass::enableMotorBacklashComp (
  byte    motorIdMask, 
  boolean enableBacklashComp
) {

  if ( enableBacklashComp ) {
    modifyMotorStatusWordBit ( motorIdMask, SPARKI_MOTOR_STATUS_MASK_BLCOMP_IS_ON, true );
  }
  else {
    modifyMotorStatusWordBit (
      motorIdMask,
      SPARKI_MOTOR_STATUS_MASK_BLCOMP_IS_ON | SPARKI_MOTOR_STATUS_MASK_BLCOMP_IS_PEND, 
      false
    );
  }

}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

boolean SparkiClass::motorBacklashCompIsEnabledForAny (
  byte motorIdMask
) {

  boolean backlashCompIsOn;
  uint8_t motorIndex;

  motorIdMask &= SPARKI_MOTOR_ID_MASK_ALL;

  backlashCompIsOn = false;

  motorIndex = 0;
  while ( motorIdMask != 0x00 ) {
    if ( motorIdMask & 0x01 ) {
       backlashCompIsOn = backlashCompIsOn || ( motorStatusWord[motorIndex] & SPARKI_MOTOR_STATUS_MASK_BLCOMP_IS_ON );
    }
    motorIdMask >>= 1;
    motorIndex++;
  }
  
  return backlashCompIsOn;

}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void SparkiClass::performMotorBacklashCompensation (
  byte motorIdMask
) {

  uint8_t  motorIndex;
  byte     motorIdMaskForBlComp;
  uint32_t savedStepCount[SPARKI_MOTOR_COUNT];

  motorIdMask &= SPARKI_MOTOR_ID_MASK_ALL;
  motorIdMaskForBlComp = motorIdMask;

  motorIndex = 0;
  while ( motorIdMask != 0x00 ) {
    if ( motorIdMask & 0x01 ) {
      if ( motorStatusWord[motorIndex] & SPARKI_MOTOR_STATUS_MASK_BLCOMP_IS_PEND ) {
        savedStepCount[motorIndex] = motorRemainingSteps[motorIndex];
        motorRemainingSteps[motorIndex] = SPARKI_MOTOR_STEPS_OF_BACKLASH;
      }
      else {
        motorIdMaskForBlComp &= ~ ( 0x01 << motorIndex );
      }
    }
    motorIdMask >>= 1;
    motorIndex++;
  }

  if ( motorIdMaskForBlComp != 0x00 ) {

    cli ();
    modifyMotorStatusWordBit ( motorIdMaskForBlComp, SPARKI_MOTOR_STATUS_MASK_MTR_IS_RUNNING, true );
    sei ();
    while ( anyMotorIsRunning(motorIdMaskForBlComp) ) delay ( 10 );

    motorIndex = 0;
    while ( motorIdMaskForBlComp != 0x00 ) {
      if ( motorIdMaskForBlComp & 0x01 ) {
        modifyMotorStatusWordBit ( motorIndex, SPARKI_MOTOR_STATUS_MASK_BLCOMP_IS_PEND, false, true );
        motorRemainingSteps[motorIndex] = savedStepCount[motorIndex];
      }
      motorIdMaskForBlComp >>= 1;
      motorIndex++;
    }

  }  // if ( motorIdMaskForBlComp != 0x00 )

}

// ----------------------------------------------------------------------------

uint8_t SparkiClass::motorSpeedPercentToIntEvents (
  uint8_t speedPercent
) {

  uint8_t  slowdownPercent;
  uint32_t updatePeriodExtraUs;
  uint32_t updatePeriodTotalUs;
  uint16_t updatePeriodIntEvents;
  uint8_t  updateIntervalIntEvents;


  if ( speedPercent > 100 ) speedPercent = 100;
  slowdownPercent = 100 - speedPercent;
  updatePeriodExtraUs = (uint32_t) slowdownPercent * (
                          SPARKI_MOTOR_UPDATE_PERIOD_MAX_US -
                          SPARKI_MOTOR_UPDATE_PERIOD_MIN_US
                        ) / 100;
  updatePeriodTotalUs = SPARKI_MOTOR_UPDATE_PERIOD_MIN_US + updatePeriodExtraUs;
  updatePeriodIntEvents = ( ( updatePeriodTotalUs << 1 ) / SPARKI_INTERRUPT_PERIOD_US + 1 ) >> 1;
  updateIntervalIntEvents = updatePeriodIntEvents == 0 ? 0 : updatePeriodIntEvents - 1;

  return updateIntervalIntEvents;

  // NOTE: Bit shifts and +1 in updatePeriodIntEvents expression are to
  // achieve rounding to the nearest half.

  // FIXME (?): Not sure how smart the compiler is... If all the explicit
  // intermediate variables result in lots of unique register allocations
  // then consider trading off human readability for code efficiency and
  // potentially mash everything into one giant expression.

}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

uint8_t SparkiClass::motorIntEventsToSpeedPercent (
  uint16_t updateIntervalIntEvents
) {

  uint16_t updatePeriodIntEvents;
  uint32_t updatePeriodUs;
  uint32_t updatePeriodExtraUs;
  uint32_t updatePeriodExtraUsFp;
  uint16_t slowdownPercentFp;
  uint8_t  speedPercent;

  updatePeriodIntEvents = updateIntervalIntEvents + 1;
  updatePeriodUs = updatePeriodIntEvents * SPARKI_INTERRUPT_PERIOD_US;
  updatePeriodExtraUs = updatePeriodUs - SPARKI_MOTOR_UPDATE_PERIOD_MIN_US;
  updatePeriodExtraUsFp = updatePeriodExtraUs << 7;
  slowdownPercentFp = updatePeriodExtraUsFp * 100 / (
                        SPARKI_MOTOR_UPDATE_PERIOD_MAX_US -
                        SPARKI_MOTOR_UPDATE_PERIOD_MIN_US
                      );
  speedPercent = 100 - ( slowdownPercentFp >> 7 );

  return speedPercent;

  // FIXME (?): Not sure how smart the compiler is... If all the explicit
  // intermediate variables result in lots of unique register allocations
  // then consider trading off human readability for code efficiency and
  // potentially mash everything into one giant expression.

}

// ----------------------------------------------------------------------------
// Motor Control: Primary Control Functions
// ----------------------------------------------------------------------------

uint32_t SparkiClass::driveDistanceToMotorSteps (
  uint16_t distanceToDriveMm
) {

  uint32_t distanceToDriveSteps;

  if ( distanceToDriveMm == 0 && distanceOfZeroMeansInfinity ) {
    distanceToDriveSteps = 0;
    distanceToDriveSteps = ~ distanceToDriveSteps;
  }
  else {

    distanceToDriveSteps = (
        (uint32_t) distanceToDriveMm * 
    //  SPARKI_MOTOR_DRIVE_MM_TO_STEPS_FP
        driveMmToStepsEffFp
      ) >> SPARKI_MOTOR_DRIVE_MM_TO_STEPS_FP_FRACT_BITS;
  // steps = mm * rev/mm * steps/rev
  //       = mm * rev/mm / rev/steps
  //       = mm * SPARKI_MOTOR_DRIVE_MM_TO_STEPS

  }

  return distanceToDriveSteps;

}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

uint32_t SparkiClass::spinAngleToMotorSteps (
  uint16_t angleToSpinDeg
) {

  uint32_t distanceToSpinSteps;

  if ( angleToSpinDeg == 0 && distanceOfZeroMeansInfinity ) {
    distanceToSpinSteps = 0;
    distanceToSpinSteps = ~ distanceToSpinSteps;
  }
  else {

    distanceToSpinSteps = (
        (uint32_t) angleToSpinDeg *
    //  SPARKI_MOTOR_SPIN_DEG_TO_STEPS_FP
        spinDegToStepsEffFp 
      ) >> SPARKI_MOTOR_SPIN_DEG_TO_STEPS_FP_FRACT_BITS;
    // steps = deg * spin/deg * mm/spin * rev/mm * steps/rev
    //       = deg / deg/spin * mm/spin / mm/rev * steps/rev
    //       = ( deg * mm/spin * steps/rev ) / ( deg/spin * mm/rev )
    //       = deg * SPARKI_MOTOR_SPIN_DEG_TO_STEPS

  }

  return distanceToSpinSteps;

}

// ----------------------------------------------------------------------------

void SparkiClass::setupMotorForSteps (
  byte     motorIdMask,
  uint32_t stepCount,
  boolean  stepClockwise,
  uint8_t  speedPercent,
  boolean  stopIfRunning
) {

  // NOTE: Typically, the caller of this function should disable
  // interrupts before calling this function and enable interrupts
  // again after this function's completion.

  uint8_t motorIndex;
  boolean motorIsRunningNow;
  boolean priorDirWasCw;
  boolean blCompWasPending;
  boolean directionIsChanging;
  boolean applyBacklashComp;

  motorIdMask &= SPARKI_MOTOR_ID_MASK_ALL;

  motorIndex = 0;
  while ( motorIdMask != 0x00 ) {

    if ( motorIdMask & 0x01 ) {

      motorIsRunningNow = motorStatusWord[motorIndex] & SPARKI_MOTOR_STATUS_MASK_MTR_IS_RUNNING;
      if ( motorIsRunningNow && stopIfRunning ) {
        modifyMotorStatusWordBit ( motorIndex, SPARKI_MOTOR_STATUS_MASK_MTR_IS_RUNNING, false, true );
        motorIsRunningNow = false;
      }

      if ( motorStatusWord[motorIndex] & SPARKI_MOTOR_STATUS_MASK_BLCOMP_IS_ON ) {
        priorDirWasCw = motorStatusWord[motorIndex] & SPARKI_MOTOR_STATUS_MASK_DIR_IS_CW;
        blCompWasPending = motorStatusWord[motorIndex] & SPARKI_MOTOR_STATUS_MASK_BLCOMP_IS_PEND;
        directionIsChanging =   priorDirWasCw && ! stepClockwise ||
                              ! priorDirWasCw &&   stepClockwise;
        applyBacklashComp = ! blCompWasPending &&   directionIsChanging ||
                              blCompWasPending && ! directionIsChanging;
      } else {
        applyBacklashComp = false;
      }

      modifyMotorStatusWordBit ( motorIndex, SPARKI_MOTOR_STATUS_MASK_DIR_IS_CW, stepClockwise, true );

      motorUpdateIntervalIntEvents[motorIndex] = motorSpeedPercentToIntEvents ( speedPercent );

      if ( ! motorIsRunningNow ||
           motorTimeToUpdateIntEvents[motorIndex] > motorUpdateIntervalIntEvents[motorIndex] ) {
        motorTimeToUpdateIntEvents[motorIndex] = motorUpdateIntervalIntEvents[motorIndex];
      }

      motorRemainingSteps[motorIndex] = stepCount;

      if ( motorIsRunningNow ) {
        if ( applyBacklashComp ) motorRemainingSteps[motorIndex] += SPARKI_MOTOR_STEPS_OF_BACKLASH;
      } else {
        modifyMotorStatusWordBit ( motorIndex, SPARKI_MOTOR_STATUS_MASK_BLCOMP_IS_PEND, applyBacklashComp, true );
      }

      // NOTE/FIXME: Applying backlash compensation to the wheels while
      // they are running may not yield nice results with the current
      // implementation. Improve this?

    }  // if ( motorIdMask & 0x01 )

    motorIdMask >>= 1;
    motorIndex++;

  }  // while ( motorIdMask != 0x00 )

}

// ----------------------------------------------------------------------------

void SparkiClass::startMotorRotation (
  byte    motorIdMask,
  boolean waitUntilDone
) {

  byte    motorIdMaskSaved;
  uint8_t motorIndex;

  motorIdMask &= SPARKI_MOTOR_ID_MASK_ALL;

  performMotorBacklashCompensation ( motorIdMask );

//cli ();
//modifyMotorStatusWordBit ( motorIdMask, SPARKI_MOTOR_STATUS_MASK_MTR_IS_RUNNING, true );
//sei ();

  motorIdMaskSaved = motorIdMask;
  motorIndex = 0;
  cli ();
  while ( motorIdMask != 0x00 ) {
    if ( motorIdMask & 0x01 ) {
      if ( motorRemainingSteps[motorIndex] > 0 ) {
        modifyMotorStatusWordBit ( motorIndex, SPARKI_MOTOR_STATUS_MASK_MTR_IS_RUNNING, true, true );
      }
    }
    motorIdMask >>= 1;
    motorIndex++;
  }
  sei ();
  motorIdMask = motorIdMaskSaved;

  if ( waitUntilDone ) while ( anyMotorIsRunning(motorIdMask) ) delay ( 10 );

}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void SparkiClass::stopMotorRotation (
  byte motorIdMask
) {

  modifyMotorStatusWordBit ( motorIdMask, SPARKI_MOTOR_STATUS_MASK_MTR_IS_RUNNING, false );

}

// ----------------------------------------------------------------------------

void SparkiClass::rotateMotorsBySteps (
  byte     motorIdMask,
  uint32_t stepCount,
  int8_t   turnDirection,
  boolean  waitUntilDone,
  uint8_t  speedPercent
) {

  cli ();
  setupMotorForSteps ( motorIdMask, stepCount, turnDirection, speedPercent, true );
  sei ();

  startMotorRotation ( motorIdMask, waitUntilDone );

}

// ----------------------------------------------------------------------------

uint8_t SparkiClass::countOfMotorsRunning (
  byte motorIdMask
) {

  boolean motorCount;
  uint8_t motorIndex;

  motorIdMask &= SPARKI_MOTOR_ID_MASK_ALL;

  motorCount = 0;
  motorIndex = 0;
  cli ();
  while ( motorIdMask != 0x00 ) {
    if ( motorIdMask & 0x01 ) {
      if ( motorStatusWord[motorIndex] & SPARKI_MOTOR_STATUS_MASK_MTR_IS_RUNNING ) motorCount++;
    }
    motorIdMask >>= 1;
    motorIndex++;
  }
  sei ();
  
  return motorCount;

}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

boolean SparkiClass::anyMotorIsRunning (
  byte motorIdMask
) {

  boolean motorRunning;
  uint8_t motorIndex;

  motorIdMask &= SPARKI_MOTOR_ID_MASK_ALL;

  motorRunning = false;
  motorIndex = 0;
  cli ();
  while ( motorIdMask != 0x00 ) {
    if ( motorIdMask & 0x01 ) {
      motorRunning = motorRunning || ( motorStatusWord[motorIndex] & SPARKI_MOTOR_STATUS_MASK_MTR_IS_RUNNING );
    }
    motorIdMask >>= 1;
    motorIndex++;
  }
  sei ();
  
  return motorRunning;

}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

boolean SparkiClass::allMotorsAreRunning (
  byte motorIdMask
) {

  boolean motorRunning;
  uint8_t motorIndex;

  motorIdMask &= SPARKI_MOTOR_ID_MASK_ALL;

  motorRunning = true;
  motorIndex = 0;
  cli ();
  while ( motorIdMask != 0x00 ) {
    if ( motorIdMask & 0x01 ) {
      motorRunning = motorRunning && ( motorStatusWord[motorIndex] & SPARKI_MOTOR_STATUS_MASK_MTR_IS_RUNNING );
    }
    motorIdMask >>= 1;
    motorIndex++;
  }
  sei ();
  
  return motorRunning;

}

// ----------------------------------------------------------------------------

void SparkiClass::drive (
  int16_t  distanceToDriveMm,
  boolean  directionIsFoward,
  boolean  waitUntilDone,
  int8_t   speedPercent
) {

  uint32_t distanceToDriveSteps;

  if ( distanceToDriveMm < 0 ) {
    directionIsFoward = ! directionIsFoward;
    distanceToDriveMm = - distanceToDriveMm;
  }

  if ( speedPercent < 0 ) speedPercent = wheelSpeedPercentDefault;

  waitUntilDone = waitUntilDone && distanceToDriveMm > 0;

  distanceToDriveSteps = driveDistanceToMotorSteps ( distanceToDriveMm );

  if ( distanceToDriveSteps > 0 ) {

    cli ();

    setupMotorForSteps (
      SPARKI_MOTOR_ID_MASK_WHEEL_LEFT,
      distanceToDriveSteps,
      directionIsFoward ? SPARKI_MOTOR_DIR_COUNTERCLOCKWISE : SPARKI_MOTOR_DIR_CLOCKWISE,
      speedPercent,
      true
    );

    setupMotorForSteps (
      SPARKI_MOTOR_ID_MASK_WHEEL_RIGHT,
      distanceToDriveSteps,
      directionIsFoward ? SPARKI_MOTOR_DIR_CLOCKWISE : SPARKI_MOTOR_DIR_COUNTERCLOCKWISE,
      speedPercent,
      true
    );

    sei ();

    startMotorRotation ( SPARKI_MOTOR_ID_MASK_WHEELS, waitUntilDone );

  }  // if ( distanceToDriveSteps > 0 )

}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void SparkiClass::driveForward (
  int16_t  distanceToDriveMm,
  boolean  waitUntilDone,
  int8_t   speedPercent
) {

  drive ( distanceToDriveMm, true, waitUntilDone, speedPercent );

}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void SparkiClass::driveBackward (
  int16_t  distanceToDriveMm,
  boolean  waitUntilDone,
  int8_t   speedPercent
) {

  drive ( distanceToDriveMm, false, waitUntilDone, speedPercent );

}

// ----------------------------------------------------------------------------

void SparkiClass::spin (
  int16_t angleToSpinDeg,
  boolean directionIsRight,
  boolean waitUntilDone,
  int8_t  speedPercent
) {

  uint32_t distanceToSpinSteps;

  if ( angleToSpinDeg < 0 ) {
    directionIsRight = ! directionIsRight;
    angleToSpinDeg = - angleToSpinDeg;
  }

  if ( speedPercent < 0 ) speedPercent = wheelSpeedPercentDefault;

  waitUntilDone = waitUntilDone && angleToSpinDeg > 0;

  distanceToSpinSteps = spinAngleToMotorSteps ( angleToSpinDeg );

  if ( distanceToSpinSteps > 0 ) {

    cli ();

    setupMotorForSteps (
      SPARKI_MOTOR_ID_MASK_WHEEL_LEFT,
      distanceToSpinSteps,
      directionIsRight ? SPARKI_MOTOR_DIR_COUNTERCLOCKWISE : SPARKI_MOTOR_DIR_CLOCKWISE,
      speedPercent,
      true
    );

    setupMotorForSteps (
      SPARKI_MOTOR_ID_MASK_WHEEL_RIGHT,
      distanceToSpinSteps,
      directionIsRight ? SPARKI_MOTOR_DIR_COUNTERCLOCKWISE : SPARKI_MOTOR_DIR_CLOCKWISE,
      speedPercent,
      true
    );

    sei ();

    startMotorRotation ( SPARKI_MOTOR_ID_MASK_WHEELS, waitUntilDone );

  }  // if ( distanceToSpinSteps > 0 )

}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void SparkiClass::spinLeft (
  int16_t angleToSpinDeg,
  boolean waitUntilDone,
  int8_t  speedPercent
) {

  spin ( angleToSpinDeg, SPARKI_MOTOR_DIR_COUNTERCLOCKWISE, waitUntilDone, speedPercent );

}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void SparkiClass::spinRight (
  int16_t angleToSpinDeg,
  boolean waitUntilDone,
  int8_t  speedPercent
) {

  spin ( angleToSpinDeg, SPARKI_MOTOR_DIR_CLOCKWISE, waitUntilDone, speedPercent );

}

// ----------------------------------------------------------------------------

void SparkiClass::turn (
  int16_t angleToTurnDeg,
  boolean directionIsRight,
  int16_t radiusOfTurnMm,
  boolean waitUntilDone,
  int8_t  speedPercent
) {

  const uint8_t updatePeriodFractBits = 2;

  int8_t   negRadSpinAngleDeg;
  int8_t   halfWheelSeparationMm;
  int16_t  outerWheelTurnRadiusMm;
  int16_t  innerWheelTurnRadiusMm;
  int32_t  turnRadiusMmToStepsEffFp;
  int32_t  innerWheelDriveDistanceSteps;
  int32_t  outerWheelDriveDistanceSteps;
  int32_t  centerMoveDistanceSteps;
  boolean  outerWheelDirIsForward;
  boolean  innerWheelDirIsForward;
  uint16_t minUpdatePeriodIntEvents;
  uint16_t centerUpdatePeriodIntEv;
  uint16_t outerWheelUpdatePeriodIntEv;
  uint16_t innerWheelUpdatePeriodIntEv;
  byte     outerWheelIdMask;
  uint8_t  outerWheelIndex;
  byte     innerWheelIdMask;
  uint8_t  innerWheelIndex;
  boolean  outerWheelDirIsClockwise;
  boolean  innerWheelDirIsClockwise;

  // Handle corner cases...

  if ( speedPercent < 0 ) speedPercent = wheelSpeedPercentDefault;

  if ( radiusOfTurnMm == 0 ) {
    if ( angleToTurnDeg != 0 ) spin ( angleToTurnDeg, true, waitUntilDone, speedPercent );
    return;
  }

  // Set up special handling of negative turn radius...

  if ( ! SPARKI_WHEEL_TURN_WITH_NEG_RAD_ALTERNATE || radiusOfTurnMm >= 0 ) {
    negRadSpinAngleDeg = 0;
  }
  else {
    if ( directionIsRight && angleToTurnDeg < 0 || ! directionIsRight && angleToTurnDeg > 0 ) {
      negRadSpinAngleDeg = -90;
    }
    else {
      negRadSpinAngleDeg = +90;
    }
    radiusOfTurnMm   = - radiusOfTurnMm;
    directionIsRight = ! directionIsRight;  
    waitUntilDone    = true;
  }

  if ( negRadSpinAngleDeg != 0 ) spin ( + negRadSpinAngleDeg );

  // Calculate turn radii for individual wheels...

  halfWheelSeparationMm = wheelSeparationUmEff / 20;

  outerWheelTurnRadiusMm = radiusOfTurnMm + halfWheelSeparationMm;
  innerWheelTurnRadiusMm = radiusOfTurnMm - halfWheelSeparationMm;

  // Calculate driving distance for center point (pen hole)
  // and for each wheel...

  turnRadiusMmToStepsEffFp = (int32_t) angleToTurnDeg * turnRadMmAngleDegToStepsEffFp;

  centerMoveDistanceSteps = (
    (int32_t) radiusOfTurnMm * turnRadiusMmToStepsEffFp
  ) >> SPARKI_TURN_RADIUSMM_ANGLEDEG_TO_STEPS_FP_FRACT_BITS;

  outerWheelDriveDistanceSteps = (
    (int32_t) outerWheelTurnRadiusMm * turnRadiusMmToStepsEffFp
  ) >> SPARKI_TURN_RADIUSMM_ANGLEDEG_TO_STEPS_FP_FRACT_BITS;

  innerWheelDriveDistanceSteps = (
    (int32_t) innerWheelTurnRadiusMm * turnRadiusMmToStepsEffFp
  ) >> SPARKI_TURN_RADIUSMM_ANGLEDEG_TO_STEPS_FP_FRACT_BITS;

  // Normalize driving distances to positive quantities...

  centerMoveDistanceSteps = centerMoveDistanceSteps < 0 ? - centerMoveDistanceSteps : centerMoveDistanceSteps;

  if ( outerWheelDriveDistanceSteps >= 0 ) {
    outerWheelDirIsForward = true;
  }
  else {
    outerWheelDirIsForward = false;
    outerWheelDriveDistanceSteps = - outerWheelDriveDistanceSteps;
  }

  if ( innerWheelDriveDistanceSteps >= 0 ) {
    innerWheelDirIsForward = true;
  }
  else {
    innerWheelDirIsForward = false;
    innerWheelDriveDistanceSteps = - innerWheelDriveDistanceSteps;
  }

  // Compute speed setting for each wheel...

  minUpdatePeriodIntEvents = ( motorSpeedPercentToIntEvents (          100 ) + 1 ) << updatePeriodFractBits;
  centerUpdatePeriodIntEv  = ( motorSpeedPercentToIntEvents ( speedPercent ) + 1 ) << updatePeriodFractBits;

  outerWheelUpdatePeriodIntEv = centerMoveDistanceSteps * centerUpdatePeriodIntEv / outerWheelDriveDistanceSteps;
  innerWheelUpdatePeriodIntEv = centerMoveDistanceSteps * centerUpdatePeriodIntEv / innerWheelDriveDistanceSteps;

  if ( outerWheelUpdatePeriodIntEv < minUpdatePeriodIntEvents || innerWheelUpdatePeriodIntEv < minUpdatePeriodIntEvents ) {
    if ( outerWheelUpdatePeriodIntEv <= innerWheelUpdatePeriodIntEv ) {
      innerWheelUpdatePeriodIntEv = innerWheelUpdatePeriodIntEv * minUpdatePeriodIntEvents / outerWheelUpdatePeriodIntEv;
      outerWheelUpdatePeriodIntEv = minUpdatePeriodIntEvents;
    }
    else {
      outerWheelUpdatePeriodIntEv = outerWheelUpdatePeriodIntEv * minUpdatePeriodIntEvents / innerWheelUpdatePeriodIntEv;
      innerWheelUpdatePeriodIntEv = minUpdatePeriodIntEvents;
    }
  }

  outerWheelUpdatePeriodIntEv = ( outerWheelUpdatePeriodIntEv + ( 1 << updatePeriodFractBits-1 ) ) >> updatePeriodFractBits;
  innerWheelUpdatePeriodIntEv = ( innerWheelUpdatePeriodIntEv + ( 1 << updatePeriodFractBits-1 ) ) >> updatePeriodFractBits;

  // Drive the turn...

  if ( directionIsRight ) {
    outerWheelIdMask = SPARKI_MOTOR_ID_MASK_WHEEL_LEFT;
    outerWheelIndex  = SPARKI_MOTOR_INDEX_WHEEL_LEFT;
    outerWheelDirIsClockwise = ! outerWheelDirIsForward;
    innerWheelIdMask = SPARKI_MOTOR_ID_MASK_WHEEL_RIGHT;
    innerWheelIndex  = SPARKI_MOTOR_INDEX_WHEEL_RIGHT;
    innerWheelDirIsClockwise =   innerWheelDirIsForward;
  }
  else {
    outerWheelIdMask = SPARKI_MOTOR_ID_MASK_WHEEL_RIGHT;
    outerWheelIndex  = SPARKI_MOTOR_INDEX_WHEEL_RIGHT;
    outerWheelDirIsClockwise =   outerWheelDirIsForward;
    innerWheelIdMask = SPARKI_MOTOR_ID_MASK_WHEEL_LEFT;
    innerWheelIndex  = SPARKI_MOTOR_INDEX_WHEEL_LEFT;
    innerWheelDirIsClockwise = ! innerWheelDirIsForward;
  }

  cli ();

  setupMotorForSteps (
    outerWheelIdMask,
    outerWheelDriveDistanceSteps,
    outerWheelDirIsClockwise ? SPARKI_MOTOR_DIR_CLOCKWISE : SPARKI_MOTOR_DIR_COUNTERCLOCKWISE,
    0,  // speed percent is don't-care; interrupt interval is set directly below
    true
  );
  motorUpdateIntervalIntEvents[outerWheelIndex] = outerWheelUpdatePeriodIntEv - 1;
  motorTimeToUpdateIntEvents[outerWheelIndex]   = outerWheelUpdatePeriodIntEv - 1;

  setupMotorForSteps (
    innerWheelIdMask,
    innerWheelDriveDistanceSteps,
    innerWheelDirIsClockwise ? SPARKI_MOTOR_DIR_CLOCKWISE : SPARKI_MOTOR_DIR_COUNTERCLOCKWISE,
    0,  // speed percent is don't-care; interrupt interval is set directly below
    true
  );
  motorUpdateIntervalIntEvents[innerWheelIndex] = innerWheelUpdatePeriodIntEv - 1;
  motorTimeToUpdateIntEvents[innerWheelIndex]   = innerWheelUpdatePeriodIntEv - 1;

  sei ();

  startMotorRotation ( SPARKI_MOTOR_ID_MASK_WHEELS, waitUntilDone );

  // Finish special handling of negative turn radius...
  
  if ( negRadSpinAngleDeg != 0 ) spin ( - negRadSpinAngleDeg );

}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void SparkiClass::turnLeft (
  int16_t angleToTurnDeg,
  int16_t radiusOfTurnMm,
  boolean waitUntilDone,
  int8_t  speedPercent
) {

  turn ( angleToTurnDeg, false, radiusOfTurnMm, waitUntilDone, speedPercent );

}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void SparkiClass::turnRight (
  int16_t angleToTurnDeg,
  int16_t radiusOfTurnMm,
  boolean waitUntilDone,
  int8_t  speedPercent
) {

  turn ( angleToTurnDeg, true, radiusOfTurnMm, waitUntilDone, speedPercent );

}

// ----------------------------------------------------------------------------

boolean SparkiClass::wheelsAreRunning () {

  return anyMotorIsRunning ( SPARKI_MOTOR_ID_MASK_WHEELS );

}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void SparkiClass::stopWheels () {

  stopMotorRotation ( SPARKI_MOTOR_ID_MASK_WHEELS );

}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

boolean SparkiClass::wheelsAreDone () {

  byte    motorIdMask;
  uint8_t motorIndex;
  boolean wheelsDone;

  motorIdMask = SPARKI_MOTOR_ID_MASK_WHEELS;

  wheelsDone = true;

  motorIndex = 0;
  while ( motorIdMask != 0x00 ) {
    if ( motorIdMask & 0x01 ) {
      wheelsDone = wheelsDone && 
        ( motorStatusWord[motorIndex] & SPARKI_MOTOR_STATUS_MASK_MTR_IS_RUNNING ) &&
        motorRemainingSteps[motorIndex] == 0;
    }
    motorIdMask >>= 1;
    motorIndex++;
  }

  return wheelsDone;

}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void SparkiClass::restartWheels (
  boolean waitUntilDone
) {

  startMotorRotation ( SPARKI_MOTOR_ID_MASK_WHEELS, waitUntilDone );

}

// ----------------------------------------------------------------------------

void SparkiClass::openGripper (
  int8_t  amountToOpenMm,
  boolean waitUntilDone,
  int8_t  speedPercent
) {

  uint8_t  absAmountToOpenMm;
  uint32_t stepsToMove;

  if ( gripperIsRunning() ) stopGripper ();

  if ( speedPercent < 0 ) speedPercent = gripperSpeedPercentDefault;

  if ( amountToOpenMm != 0 ) {

    if ( amountToOpenMm >= 0 ) {
      absAmountToOpenMm = amountToOpenMm;
      if ( absAmountToOpenMm >= SPARKI_MOTOR_GRIPPER_OPEN_MAX_MM ) {
        amountToOpenMm    =   SPARKI_MOTOR_GRIPPER_OPEN_MAX_MM;
        absAmountToOpenMm =   SPARKI_MOTOR_GRIPPER_OPEN_MAX_MM;
        gripperSpacingMm  =   0;
      }
    }
    else {
      absAmountToOpenMm = - amountToOpenMm;
      if ( absAmountToOpenMm >= SPARKI_MOTOR_GRIPPER_OPEN_MAX_MM ) {
        amountToOpenMm    = - SPARKI_MOTOR_GRIPPER_OPEN_MAX_MM;
        absAmountToOpenMm =   SPARKI_MOTOR_GRIPPER_OPEN_MAX_MM;
        gripperSpacingMm  =   SPARKI_MOTOR_GRIPPER_OPEN_MAX_MM;
      }
    }

    if ( gripperSpacingMm >= 0 ) {
      gripperSpacingMm += amountToOpenMm;
      if ( gripperSpacingMm < 0 ) {
        gripperSpacingMm = 0;
      }
      if ( gripperSpacingMm > SPARKI_MOTOR_GRIPPER_OPEN_MAX_MM ) {
        gripperSpacingMm = SPARKI_MOTOR_GRIPPER_OPEN_MAX_MM;
      }
    }

    stepsToMove = (
        (uint32_t) absAmountToOpenMm *
        SPARKI_MOTOR_GRIP_MM_TO_STEPS_FP
      ) >> SPARKI_MOTOR_GRIP_MM_TO_STEPS_FP_FRACT_BITS;
    // steps = mm * teeth/mm * rev/teeth * steps/rev / 2
    //       = mm / mm/teeth / teeth/rev * steps/rev / 2
    //       = ( mm * steps/rev ) / ( mm/teeth * teeth/rev * 2 )
    //       = mm * SPARKI_MOTOR_GRIP_MM_TO_STEPS

    cli ();

    setupMotorForSteps (
      SPARKI_MOTOR_ID_MASK_GRIPPER,
      stepsToMove,
      amountToOpenMm > 0 ? SPARKI_MOTOR_DIR_COUNTERCLOCKWISE : SPARKI_MOTOR_DIR_CLOCKWISE,
      speedPercent,
      true
    );

    sei ();

    startMotorRotation ( SPARKI_MOTOR_ID_MASK_GRIPPER, waitUntilDone );

  }

}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void SparkiClass::closeGripper (
  int8_t  amountToCloseMm,
  boolean waitUntilDone,
  int8_t  speedPercent
) {

  openGripper ( -amountToCloseMm, waitUntilDone, speedPercent );

}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void SparkiClass::setGripperSpacing (
  uint8_t spacingMm,
  boolean waitUntilDone,
  int8_t  speedPercent
) {

  int8_t amountToOpenMm;

  if ( gripperSpacingMm >= 0 ) {
    amountToOpenMm = (int8_t) spacingMm - gripperSpacingMm;
    openGripper ( amountToOpenMm, waitUntilDone, speedPercent );
  }

}

// ----------------------------------------------------------------------------

boolean SparkiClass::gripperIsRunning () {

  return anyMotorIsRunning ( SPARKI_MOTOR_ID_MASK_GRIPPER );

}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void SparkiClass::stopGripper () {

  uint8_t  distanceNotTraveledMm;

  if ( gripperIsRunning() ) {

    stopMotorRotation ( SPARKI_MOTOR_ID_MASK_GRIPPER );

    if ( gripperSpacingMm >= 0 ) {

      distanceNotTraveledMm = ( 
          motorRemainingSteps[SPARKI_MOTOR_INDEX_GRIPPER] *
          SPARKI_MOTOR_GRIP_STEPS_TO_MM_FP
        ) >> SPARKI_MOTOR_GRIP_STEPS_TO_MM_FP_FRACT_BITS;
      // mm = steps * rev/steps * teeth/rev * mm/teeth * 2
      //    = steps / steps/rev * teeth/rev * mm/teeth * 2
      //    = ( steps * teeth/rev * mm/teeth * 2 ) / steps/rev
      //    = steps * SPARKI_MOTOR_GRIP_STEPS_TO_MM

      if ( motorStatusWord[SPARKI_MOTOR_INDEX_GRIPPER] & SPARKI_MOTOR_STATUS_MASK_DIR_IS_CW ) {
        gripperSpacingMm += distanceNotTraveledMm;
      }
      else {
        gripperSpacingMm -= distanceNotTraveledMm;
      }

    }  // if ( gripperSpacingMm >= 0 )

  }  // if ( gripperIsRunning() )

}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

boolean SparkiClass::gripperIsDone () {

  return motorRemainingSteps[SPARKI_MOTOR_INDEX_GRIPPER] == 0;

}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void SparkiClass::restartGripper (
  boolean waitUntilDone
) {

  startMotorRotation ( SPARKI_MOTOR_ID_MASK_GRIPPER, waitUntilDone );

}

// ----------------------------------------------------------------------------
// Motor Control: Legacy Functions / Backward Compatibility
// ----------------------------------------------------------------------------

void SparkiClass::moveLeft ( float deg ) {
  boolean savedZeroMeansInfinity;
  savedZeroMeansInfinity = distanceOfZeroMeansInfinity;
  distanceOfZeroMeansInfinity = true;
  spinLeft ( (int16_t) deg );
  distanceOfZeroMeansInfinity = savedZeroMeansInfinity;
}
void SparkiClass::moveLeft () {
  spinLeft ();
}

void SparkiClass::moveRight ( float deg ) {
  boolean savedZeroMeansInfinity;
  savedZeroMeansInfinity = distanceOfZeroMeansInfinity;
  distanceOfZeroMeansInfinity = true;
  spinRight ( (int16_t) deg );
  distanceOfZeroMeansInfinity = savedZeroMeansInfinity;
}
void SparkiClass::moveRight () {
  spinRight ();
}

void SparkiClass::moveForward ( float cm ) {
  boolean savedZeroMeansInfinity;
  savedZeroMeansInfinity = distanceOfZeroMeansInfinity;
  distanceOfZeroMeansInfinity = true;
  driveForward ( (int16_t) ( cm * 10 ) );
  distanceOfZeroMeansInfinity = savedZeroMeansInfinity;
}
void SparkiClass::moveForward () {
  driveForward ();
}

void SparkiClass::moveBackward ( float cm ) {
  boolean savedZeroMeansInfinity;
  savedZeroMeansInfinity = distanceOfZeroMeansInfinity;
  distanceOfZeroMeansInfinity = true;
  driveBackward ( (int16_t) ( cm * 10 ) );
  distanceOfZeroMeansInfinity = savedZeroMeansInfinity;
}
void SparkiClass::moveBackward () {
  driveBackward ();
}

void SparkiClass::moveStop () {
  stopWheels ();
}

void SparkiClass::gripperOpen () {
  openGripper ();
}
void SparkiClass::gripperClose () {
  closeGripper ();
}

void SparkiClass::gripperStop () {
  stopGripper ();
}

void SparkiClass::motorRotate ( int motor, int direction, int speed ) {
  rotateMotorsBySteps ( 0x01 < motor, ULONG_MAX, direction == DIR_CW, speed, false );
  delay ( 10 );
}

void SparkiClass::motorStop ( int motor ) {
  stopMotorRotation ( 0x01 < motor );
}

void SparkiClass::motorsRotateSteps ( int leftDir, int rightDir, int speed, uint32_t steps, bool wait ) {
  cli ();
  setupMotorForSteps ( SPARKI_MOTOR_ID_MASK_WHEEL_LEFT,  steps, leftDir  == DIR_CW, speed, true );
  setupMotorForSteps ( SPARKI_MOTOR_ID_MASK_WHEEL_RIGHT, steps, rightDir == DIR_CW, speed, true );
  sei ();
  startMotorRotation ( SPARKI_MOTOR_ID_MASK_WHEELS, wait ); 
}
 
bool SparkiClass::areMotorsRunning ( ) {
  return anyMotorIsRunning ( SPARKI_MOTOR_ID_MASK_ALL );
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

int SparkiClass::ping_single(){
  long duration; 
  float cm;
  digitalWrite(ULTRASONIC_TRIG, LOW); 
  delayMicroseconds(2); 
  digitalWrite(ULTRASONIC_TRIG, HIGH); 
  delayMicroseconds(10); 
  digitalWrite(ULTRASONIC_TRIG, LOW); 
  

  uint8_t bit = digitalPinToBitMask(ULTRASONIC_ECHO);
  uint8_t port = digitalPinToPort(ULTRASONIC_ECHO);
  uint8_t stateMask = (HIGH ? bit : 0);
  
  unsigned long startCount = 0;
  unsigned long endCount = 0;
  unsigned long width = 0; // keep initialization out of time critical area
  
  // convert the timeout from microseconds to a number of times through
  // the initial loop; it takes 16 clock cycles per iteration.
  unsigned long numloops = 0;
  unsigned long maxloops = 5000;
	
  // wait for any previous pulse to end
  while ((*portInputRegister(port) & bit) == stateMask)
    if (numloops++ == maxloops)
      return -1;
	
  // wait for the pulse to start
  while ((*portInputRegister(port) & bit) != stateMask)
    if (numloops++ == maxloops)
      return -1;
  
  startCount = micros();
  // wait for the pulse to stop
  while ((*portInputRegister(port) & bit) == stateMask) {
    if (numloops++ == maxloops)
      return -1;
    delayMicroseconds(10); //loop 'jams' without this
    if((micros() - startCount) > 58000 ){ // 58000 = 1000CM
      return -1;
      break;
    }
  }
  duration = micros() - startCount;
  //--------- end pulsein
  cm = (float)duration / 29.0 / 2.0; 
  return int(cm);
}

int SparkiClass::ping(){
  int attempts = 5;
  float distances [attempts];
  for(int i=0; i<attempts; i++){
    distances[i] = ping_single();
    delay(20);
  }
  
  // sort them in order
  int i, j;
  float temp;
 
  for (i = (attempts - 1); i > 0; i--)
  {
    for (j = 1; j <= i; j++)
    {
      if (distances[j-1] > distances[j])
      {
        temp = distances[j-1];
        distances[j-1] = distances[j];
        distances[j] = temp;
      }
    }
  }
  
  // return the middle entry
  return int(distances[(int)ceil((float)attempts/2.0)]); 
}

void SparkiClass::startServoTimer(){
  char oldSREG = SREG;				
  noInterrupts();                                       // Disable interrupts for 16 bit register access
  TCCR1A = 0;                                           // clear control register A 
  TCCR1B = _BV(WGM13);                                  // set mode 8: phase and frequency correct pwm, stop the timer
  ICR1 = 20000;                                         // ICR1 is TOP in p & f correct pwm mode
  TCCR1B &= ~(_BV(CS10) | _BV(CS11) | _BV(CS12));
  TCCR1B |= 0x02;                                       // reset clock select register, and starts the clock
  DDRB |= _BV(PORTB1);                                  // sets data direction register for pwm output pin
  TCCR1A |= _BV(COM1A1);                                // activates the output pin
  interrupts();                                         // re-enable interrupts
  SREG = oldSREG;
}

void SparkiClass::servo(int deg)
{ 
  startServoTimer();
  int duty = int((((float(-deg+servo_deg_offset)*2000/180)+1500)/20000)*1024); // compute the duty cycle for the servo
  //0 = 26
  //180 = 128
  
  unsigned long dutyCycle = 20000;
  dutyCycle *= duty;
  dutyCycle >>= 10;
  
  char oldSREG = SREG;
  noInterrupts();
  OCR1A = dutyCycle;
  
  SREG = oldSREG;
  interrupts();
}

/*
Returns the current IR Code. 
Uses the interrupt on INT6 (PE6, Pin 7) to do the signal reading
If there is no code waiting, pass -1 back immediately.
If there is a code, but its still reading, wait it out then return code
Wipes the current stored code once read.

NEC IR code details here:
http://wiki.altium.com/display/ADOH/NEC+Infrared+Transmission+Protocol
*/

int SparkiClass::readIR(){
    uint8_t code = 0;
    if(currentPulse != 0){ // check there's a code waiting
        while( micros()-lastPulseTime <= MAX_IR_PULSE){
            delayMicroseconds(MAX_IR_PULSE);
        }; // wait for the reading to time out
        // tell the interrupt to not take any more codes
        haltIRRead = 1;
        
        // decode the signal
        for(int i=0; i<8; i++){
            if(pulsesIR[17+i][1] > 1000){
                code |= (1<<i);
            }
        }
        currentPulse = 0; // 'reset' the current IR pulse reading
        haltIRRead = 0;
        return int(code); // return the decoded value
    }
    else{
        return -1; // no signal found, return -1
    }
}

SIGNAL(INT6_vect) {
  if(haltIRRead != 1){
      long currentTime = micros(); // take the current time
      int pinStatus = PINE & (1 << 6); // read the current status of the IR Pin
      timeSinceLastPulse = currentTime-lastPulseTime;
      
      // Tell if its the start of the reading cycle or not (time since last pulse), starts low
      if( (timeSinceLastPulse >= MAX_IR_PULSE) && (pinStatus == LOW)){
        // if reading new pulse, set up. Wipes out the last pulse
        currentPulse = 0;
      }
      else{
          // otherwise, read the current code
          if(pinStatus){ //(PE6 high) pulse has risen from the end of a low pulse
            pulsesIR[currentPulse][0] = timeSinceLastPulse;
          }
          else{ //(PE6 low) pulse has fallen from the end of a high pulse
            pulsesIR[currentPulse][1] = timeSinceLastPulse;
            currentPulse++;
          }  
      }    
      lastPulseTime = currentTime;
  }
}

// setups up timer to pulse 38khz on and off in a pre-described sequence according to NEC
// protocol
// http://wiki.altium.com/display/ADOH/NEC+Infrared+Transmission+Protocol

void SparkiClass::sendIR(uint8_t code){
    // setup PD7 (6) to 38khz on pin  using TIMER4 COMPD

  
  OCR4D = 13;               // compare match register 16MHz/32/38000Hz
  TCCR4D |= (1 << WGM12);   // CTC mode
  TCCR4D = 0x06;            // CLK/32 prescaler (32 = 2^(0110-1))
  TIMSK4 |= (1 << OCIE4D);  // enable Timer4 compare interrupt D - need to switch to PWM
  
  
    
    // go through each bit in byte, pulse appropriate IR
    //leading pulse Xms on, Yms off
    
    //leading pulse of all 0
    for(uint8_t bit = 0; bit < 8; bit++){ // for each bit in the byte
        if(code & (1<<bit) > 0){ // determine if bit is 1
            // bit==1: pulse for Xms on, off for Yms
        }
        else{
            // bit==0: pulse for Xms on, off for Yms
        }
    }
    // re-establish output on Timer 4 for 10khz control loop
}

float SparkiClass::accelX(){
    readAccelData();
    return -xAxisAccel*9.8;
}
float SparkiClass::accelY(){
    readAccelData();
    return -yAxisAccel*9.8;
}
float SparkiClass::accelZ(){
    readAccelData();
    return -zAxisAccel*9.8;
}

float SparkiClass::readMag(){
  WireRead(DataRegisterBegin, RawMagDataLength);
  xAxisMag = ((mag_buffer[0] << 8) | mag_buffer[1]) * M_SCALE;
  zAxisMag = ((mag_buffer[2] << 8) | mag_buffer[3]) * M_SCALE;
  yAxisMag = ((mag_buffer[4] << 8) | mag_buffer[5]) * M_SCALE;    
}

float SparkiClass::compass(){
  readMag();
  
  float heading = atan2(yAxisMag,xAxisMag);
  
  if(heading < 0)
    heading += 2*PI;
  if(heading > 2*PI)
    heading -= 2*PI;
    
  float headingDegrees = heading * 180/M_PI; 
  return headingDegrees;
}

float SparkiClass::magX(){
    readMag();
    return xAxisMag;
}

float SparkiClass::magY(){
    readMag();
    return yAxisMag;
}

float SparkiClass::magZ(){
    readMag();
    return zAxisMag;
}

void SparkiClass::WireWrite(int address, int data){
  Wire.beginTransmission(HMC5883L_Address);
  Wire.write(address);
  Wire.write(data);
  Wire.endTransmission();
}

uint8_t* SparkiClass::WireRead(int address, int length){
  Wire.beginTransmission(HMC5883L_Address);
  Wire.write(DataRegisterBegin);
  Wire.endTransmission();
  
  Wire.beginTransmission(HMC5883L_Address);
  Wire.requestFrom(HMC5883L_Address, RawMagDataLength);

  if(Wire.available() == RawMagDataLength)
  {
	  for(uint8_t i = 0; i < RawMagDataLength; i++)
	  {
		  mag_buffer[i] = Wire.read();
	  }
  }
  Wire.endTransmission();
}

/***********************************************************************************
The Scheduler
Every 200uS (5,000 times a second), we update the 2 shift registers used to increase
the amount of outputs the processor has
***********************************************************************************/

// ISR: interrupt service routine for timer interrupt. This interrupt
// service routine then calls a Sparki library function that performs
// whatever tasks are necessary. By calling the library function instead
// of just placing the library function's code directly into the ISR, the
// code can reference private members of the the Sparki class instance,
// so that low-level, private state does not need to reside and be exposed
// at global scope.

ISR ( TIMER4_COMPA_vect ) { 
  byte oldSREG = SREG;
  TCNT4 = 0;  // clearing the timer interrupt counter 
  sparki.updateShiftReg ();
  SREG = oldSREG;
}

// updateShiftReg: function to update shift register that extends number of
// processor outputs. This function is called automatically through timer
// interrupts. Under normal circumstances, it should not be called by user
// or library code. If it does need to be called; it should be bracketed by
// cli() / sei().

void SparkiClass::updateShiftReg () { 

    byte shift_outputs[2];

	// clear the shift register values so we can re-write them
    shift_outputs[0] = 0x00;
    shift_outputs[1] = 0x00;
    
    // Update the RGB leds
    if(RGB_timer < RGB_vals[0]){ // update Red led
		shift_outputs[RGB_SHIFT] |= RGB_R;
    }
    if(RGB_timer < RGB_vals[1]){ // update Green led
		shift_outputs[RGB_SHIFT] |= RGB_G;
    }
    if(RGB_timer < RGB_vals[2]){ // update Blue led
		shift_outputs[RGB_SHIFT] |= RGB_B;
    }
    RGB_timer++;
    if(RGB_timer == 100){
    	RGB_timer = 0;
    }

    // IR Detection Switch
    if(irSwitch == 0){
    	shift_outputs[1] &= 0xF7;
    }
    else{
    	shift_outputs[1] |= 0x08;
    }

  // BEGIN: Motor Control

  byte    motorIdMask;
  uint8_t motorIndex;
  boolean motorIsOnActiveHold;
  boolean motorIsRunning;

  boolean advanceStepperCoilsNow;
  boolean driveStepperCoilsNow;
  byte    stepperControlLast;
  byte    stepperControlRotated;
  byte    stepperControlNow;
    
  motorIndex = 0;
  motorIdMask = SPARKI_MOTOR_ID_MASK_ALL;

  while ( motorIdMask != 0x00 ) {

    if ( motorIdMask & 0x01 ) {

      motorIsOnActiveHold = motorStatusWord[motorIndex] & SPARKI_MOTOR_STATUS_MASK_ACT_HOLD_IS_ON;
      motorIsRunning = motorStatusWord[motorIndex] & SPARKI_MOTOR_STATUS_MASK_MTR_IS_RUNNING;

      driveStepperCoilsNow = motorIsOnActiveHold || motorIsRunning;

      if ( driveStepperCoilsNow ) {

        stepperControlLast = motorControlWord[motorIndex];

        advanceStepperCoilsNow = motorIsRunning && motorTimeToUpdateIntEvents[motorIndex] == 0;
 
        if ( advanceStepperCoilsNow ) {

          if ( motorStatusWord[motorIndex] & SPARKI_MOTOR_STATUS_MASK_DIR_IS_CW ) {
            stepperControlRotated = ( stepperControlLast << 1 ) | ( stepperControlLast >> 7 );
          }
          else {
            stepperControlRotated = ( stepperControlLast >> 1 ) | ( stepperControlLast << 7 );
          }

          stepperControlNow = stepperControlRotated & stepperControlLast;
          if ( stepperControlNow == 0x00 ) {
            stepperControlNow = stepperControlRotated | stepperControlLast;
          }

          motorControlWord[motorIndex] = stepperControlNow;

          motorRemainingSteps[motorIndex]--;

          if ( motorRemainingSteps[motorIndex] > 0 ) {
            motorTimeToUpdateIntEvents[motorIndex] = motorUpdateIntervalIntEvents[motorIndex];
          }
          else {
            motorStatusWord[motorIndex] &= ~ SPARKI_MOTOR_STATUS_MASK_MTR_IS_RUNNING;
          }

        }  // if (  advanceStepperCoilsNow )
        else {

          stepperControlNow = stepperControlLast;
          if ( motorIsRunning ) motorTimeToUpdateIntEvents[motorIndex]--;

        }  // if (  advanceStepperCoilsNow ) ... else

      }  // if ( driveStepperCoilsNow )
      else {

        stepperControlNow = 0x00;

      }  // if ( driveStepperCoilsNow ) ... else

      switch ( motorIndex ) {
        case SPARKI_MOTOR_INDEX_WHEEL_LEFT:
          shift_outputs[SPARKI_SHIFT_REG_INDEX_MOTOR_WHEEL_LEFT] |= 
            ( stepperControlNow & SPARKI_SHIFT_REG_MASK_MOTOR_WHEEL_LEFT );
          break;
        case SPARKI_MOTOR_INDEX_WHEEL_RIGHT:
          shift_outputs[SPARKI_SHIFT_REG_INDEX_MOTOR_WHEEL_RIGHT] |= 
            stepperControlNow & SPARKI_SHIFT_REG_MASK_MOTOR_WHEEL_RIGHT;
          break;
        case SPARKI_MOTOR_INDEX_GRIPPER:
          shift_outputs[SPARKI_SHIFT_REG_INDEX_MOTOR_GRIPPER] |=
            stepperControlNow & SPARKI_SHIFT_REG_MASK_MOTOR_GRIPPER;
          break;
      }  // switch ( motorIndex )

    }  // if ( motorIdMask & 0x01 )

    motorIdMask >>= 1;
    motorIndex++;

  }  // while ( motorIdMask & 0x01 )

  // END: Motor Control
    
	//output values to shift registers
    PORTD &= ~(1<<5);    // pull PD5 (shift-register latch) low
    SPI.transfer(shift_outputs[1]);
    SPI.transfer(shift_outputs[0]);
    PORTD |= (1<<5);    // pull PD5 (shift-register latch) high 

}

/***********************************************************************************
Display Library
***********************************************************************************/
#define ST7565_STARTBYTES 1

uint8_t is_reversed = 0;

// a handy reference to where the pages are on the screen
const uint8_t pagemap[] = { 3, 2, 1, 0, 7, 6, 5, 4 };

// a 5x7 font table
uint8_t font[] PROGMEM = { 
  0x0, 0x0, 0x0, 0x0, 0x0,       // Ascii 0
  0x7C, 0xDA, 0xF2, 0xDA, 0x7C,  //ASC(01)
  0x7C, 0xD6, 0xF2, 0xD6, 0x7C,  //ASC(02)
  0x38, 0x7C, 0x3E, 0x7C, 0x38, 
  0x18, 0x3C, 0x7E, 0x3C, 0x18, 
  0x38, 0xEA, 0xBE, 0xEA, 0x38, 
  0x38, 0x7A, 0xFE, 0x7A, 0x38, 
  0x0, 0x18, 0x3C, 0x18, 0x0, 
  0xFF, 0xE7, 0xC3, 0xE7, 0xFF, 
  0x0, 0x18, 0x24, 0x18, 0x0, 
  0xFF, 0xE7, 0xDB, 0xE7, 0xFF, 
  0xC, 0x12, 0x5C, 0x60, 0x70, 
  0x64, 0x94, 0x9E, 0x94, 0x64, 
  0x2, 0xFE, 0xA0, 0xA0, 0xE0, 
  0x2, 0xFE, 0xA0, 0xA4, 0xFC, 
  0x5A, 0x3C, 0xE7, 0x3C, 0x5A, 
  0xFE, 0x7C, 0x38, 0x38, 0x10, 
  0x10, 0x38, 0x38, 0x7C, 0xFE, 
  0x28, 0x44, 0xFE, 0x44, 0x28, 
  0xFA, 0xFA, 0x0, 0xFA, 0xFA, 
  0x60, 0x90, 0xFE, 0x80, 0xFE, 
  0x0, 0x66, 0x91, 0xA9, 0x56, 
  0x6, 0x6, 0x6, 0x6, 0x6,
  0x29, 0x45, 0xFF, 0x45, 0x29, 
  0x10, 0x20, 0x7E, 0x20, 0x10, 
  0x8, 0x4, 0x7E, 0x4, 0x8, 
  0x10, 0x10, 0x54, 0x38, 0x10, 
  0x10, 0x38, 0x54, 0x10, 0x10, 
  0x78, 0x8, 0x8, 0x8, 0x8, 
  0x30, 0x78, 0x30, 0x78, 0x30, 
  0xC, 0x1C, 0x7C, 0x1C, 0xC, 
  0x60, 0x70, 0x7C, 0x70, 0x60, 
  0x0, 0x0, 0x0, 0x0, 0x0, 
  0x0, 0x0, 0xFA, 0x0, 0x0, 
  0x0, 0xE0, 0x0, 0xE0, 0x0, 
  0x28, 0xFE, 0x28, 0xFE, 0x28, 
  0x24, 0x54, 0xFE, 0x54, 0x48, 
  0xC4, 0xC8, 0x10, 0x26, 0x46, 
  0x6C, 0x92, 0x6A, 0x4, 0xA, 
  0x0, 0x10, 0xE0, 0xC0, 0x0, 
  0x0, 0x38, 0x44, 0x82, 0x0, 
  0x0, 0x82, 0x44, 0x38, 0x0, 
  0x54, 0x38, 0xFE, 0x38, 0x54, 
  0x10, 0x10, 0x7C, 0x10, 0x10, 
  0x0, 0x1, 0xE, 0xC, 0x0, 
  0x10, 0x10, 0x10, 0x10, 0x10, 
  0x0, 0x0, 0x6, 0x6, 0x0, 
  0x4, 0x8, 0x10, 0x20, 0x40, 
  0x7C, 0x8A, 0x92, 0xA2, 0x7C, 
  0x0, 0x42, 0xFE, 0x2, 0x0, 
  0x4E, 0x92, 0x92, 0x92, 0x62, 
  0x84, 0x82, 0x92, 0xB2, 0xCC, 
  0x18, 0x28, 0x48, 0xFE, 0x8, 
  0xE4, 0xA2, 0xA2, 0xA2, 0x9C, 
  0x3C, 0x52, 0x92, 0x92, 0x8C, 
  0x82, 0x84, 0x88, 0x90, 0xE0, 
  0x6C, 0x92, 0x92, 0x92, 0x6C, 
  0x62, 0x92, 0x92, 0x94, 0x78, 
  0x0, 0x0, 0x28, 0x0, 0x0, 
  0x0, 0x2, 0x2C, 0x0, 0x0, 
  0x0, 0x10, 0x28, 0x44, 0x82, 
  0x28, 0x28, 0x28, 0x28, 0x28, 
  0x0, 0x82, 0x44, 0x28, 0x10, 
  0x40, 0x80, 0x9A, 0x90, 0x60, 
  0x7C, 0x82, 0xBA, 0x9A, 0x72, 
  0x3E, 0x48, 0x88, 0x48, 0x3E, 
  0xFE, 0x92, 0x92, 0x92, 0x6C, 
  0x7C, 0x82, 0x82, 0x82, 0x44, 
  0xFE, 0x82, 0x82, 0x82, 0x7C, 
  0xFE, 0x92, 0x92, 0x92, 0x82, 
  0xFE, 0x90, 0x90, 0x90, 0x80, 
  0x7C, 0x82, 0x82, 0x8A, 0xCE, 
  0xFE, 0x10, 0x10, 0x10, 0xFE, 
  0x0, 0x82, 0xFE, 0x82, 0x0, 
  0x4, 0x2, 0x82, 0xFC, 0x80, 
  0xFE, 0x10, 0x28, 0x44, 0x82, 
  0xFE, 0x2, 0x2, 0x2, 0x2, 
  0xFE, 0x40, 0x38, 0x40, 0xFE, 
  0xFE, 0x20, 0x10, 0x8, 0xFE, 
  0x7C, 0x82, 0x82, 0x82, 0x7C, 
  0xFE, 0x90, 0x90, 0x90, 0x60, 
  0x7C, 0x82, 0x8A, 0x84, 0x7A, 
  0xFE, 0x90, 0x98, 0x94, 0x62, 
  0x64, 0x92, 0x92, 0x92, 0x4C, 
  0xC0, 0x80, 0xFE, 0x80, 0xC0, 
  0xFC, 0x2, 0x2, 0x2, 0xFC, 
  0xF8, 0x4, 0x2, 0x4, 0xF8, 
  0xFC, 0x2, 0x1C, 0x2, 0xFC, 
  0xC6, 0x28, 0x10, 0x28, 0xC6, 
  0xC0, 0x20, 0x1E, 0x20, 0xC0, 
  0x86, 0x9A, 0x92, 0xB2, 0xC2, 
  0x0, 0xFE, 0x82, 0x82, 0x82, 
  0x40, 0x20, 0x10, 0x8, 0x4, 
  0x0, 0x82, 0x82, 0x82, 0xFE, 
  0x20, 0x40, 0x80, 0x40, 0x20, 
  0x2, 0x2, 0x2, 0x2, 0x2, 
  0x0, 0xC0, 0xE0, 0x10, 0x0, 
  0x4, 0x2A, 0x2A, 0x1E, 0x2, 
  0xFE, 0x14, 0x22, 0x22, 0x1C, 
  0x1C, 0x22, 0x22, 0x22, 0x14, 
  0x1C, 0x22, 0x22, 0x14, 0xFE, 
  0x1C, 0x2A, 0x2A, 0x2A, 0x18, 
  0x0, 0x10, 0x7E, 0x90, 0x40, 
  0x18, 0x25, 0x25, 0x39, 0x1E, 
  0xFE, 0x10, 0x20, 0x20, 0x1E, 
  0x0, 0x22, 0xBE, 0x2, 0x0, 
  0x4, 0x2, 0x2, 0xBC, 0x0, 
  0xFE, 0x8, 0x14, 0x22, 0x0, 
  0x0, 0x82, 0xFE, 0x2, 0x0, 
  0x3E, 0x20, 0x1E, 0x20, 0x1E, 
  0x3E, 0x10, 0x20, 0x20, 0x1E, 
  0x1C, 0x22, 0x22, 0x22, 0x1C, 
  0x3F, 0x18, 0x24, 0x24, 0x18, 
  0x18, 0x24, 0x24, 0x18, 0x3F, 
  0x3E, 0x10, 0x20, 0x20, 0x10, 
  0x12, 0x2A, 0x2A, 0x2A, 0x24, 
  0x20, 0x20, 0xFC, 0x22, 0x24, 
  0x3C, 0x2, 0x2, 0x4, 0x3E, 
  0x38, 0x4, 0x2, 0x4, 0x38, 
  0x3C, 0x2, 0xC, 0x2, 0x3C, 
  0x22, 0x14, 0x8, 0x14, 0x22, 
  0x32, 0x9, 0x9, 0x9, 0x3E, 
  0x22, 0x26, 0x2A, 0x32, 0x22, 
  0x0, 0x10, 0x6C, 0x82, 0x0, 
  0x0, 0x0, 0xEE, 0x0, 0x0, 
  0x0, 0x82, 0x6C, 0x10, 0x0, 
  0x40, 0x80, 0x40, 0x20, 0x40, 
  0x3C, 0x64, 0xC4, 0x64, 0x3C, 
  0x78, 0x85, 0x85, 0x86, 0x48, 
  0x5C, 0x2, 0x2, 0x4, 0x5E, 
  0x1C, 0x2A, 0x2A, 0xAA, 0x9A, 
  0x84, 0xAA, 0xAA, 0x9E, 0x82, 
  0x84, 0x2A, 0x2A, 0x1E, 0x82, 
  0x84, 0xAA, 0x2A, 0x1E, 0x2, 
  0x4, 0x2A, 0xAA, 0x9E, 0x2, 
  0x30, 0x78, 0x4A, 0x4E, 0x48, 
  0x9C, 0xAA, 0xAA, 0xAA, 0x9A, 
  0x9C, 0x2A, 0x2A, 0x2A, 0x9A, 
  0x9C, 0xAA, 0x2A, 0x2A, 0x1A, 
  0x0, 0x0, 0xA2, 0x3E, 0x82, 
  0x0, 0x40, 0xA2, 0xBE, 0x42, 
  0x0, 0x80, 0xA2, 0x3E, 0x2, 
  0xF, 0x94, 0x24, 0x94, 0xF, 
  0xF, 0x14, 0xA4, 0x14, 0xF, 
  0x3E, 0x2A, 0xAA, 0xA2, 0x0, 
  0x4, 0x2A, 0x2A, 0x3E, 0x2A,
  0x3E, 0x50, 0x90, 0xFE, 0x92, 
  0x4C, 0x92, 0x92, 0x92, 0x4C, 
  0x4C, 0x12, 0x12, 0x12, 0x4C, 
  0x4C, 0x52, 0x12, 0x12, 0xC, 
  0x5C, 0x82, 0x82, 0x84, 0x5E, 
  0x5C, 0x42, 0x2, 0x4, 0x1E, 
  0x0, 0xB9, 0x5, 0x5, 0xBE, 
  0x9C, 0x22, 0x22, 0x22, 0x9C, 
  0xBC, 0x2, 0x2, 0x2, 0xBC, 
  0x3C, 0x24, 0xFF, 0x24, 0x24, 
  0x12, 0x7E, 0x92, 0xC2, 0x66, 
  0xD4, 0xF4, 0x3F, 0xF4, 0xD4, 
  0xFF, 0x90, 0x94, 0x6F, 0x4, 
  0x3, 0x11, 0x7E, 0x90, 0xC0, 
  0x4, 0x2A, 0x2A, 0x9E, 0x82, 
  0x0, 0x0, 0x22, 0xBE, 0x82, 
  0xC, 0x12, 0x12, 0x52, 0x4C, 
  0x1C, 0x2, 0x2, 0x44, 0x5E, 
  0x0, 0x5E, 0x50, 0x50, 0x4E, 
  0xBE, 0xB0, 0x98, 0x8C, 0xBE, 
  0x64, 0x94, 0x94, 0xF4, 0x14, 
  0x64, 0x94, 0x94, 0x94, 0x64, 
  0xC, 0x12, 0xB2, 0x2, 0x4, 
  0x1C, 0x10, 0x10, 0x10, 0x10, 
  0x10, 0x10, 0x10, 0x10, 0x1C, 
  0xF4, 0x8, 0x13, 0x35, 0x5D, 
  0xF4, 0x8, 0x14, 0x2C, 0x5F, 
  0x0, 0x0, 0xDE, 0x0, 0x0, 
  0x10, 0x28, 0x54, 0x28, 0x44, 
  0x44, 0x28, 0x54, 0x28, 0x10, 
  0x55, 0x0, 0xAA, 0x0, 0x55, 
  0x55, 0xAA, 0x55, 0xAA, 0x55, 
  0xAA, 0x55, 0xAA, 0x55, 0xAA,
  0x0, 0x0, 0x0, 0xFF, 0x0, 
  0x8, 0x8, 0x8, 0xFF, 0x0, 
  0x28, 0x28, 0x28, 0xFF, 0x0, 
  0x8, 0x8, 0xFF, 0x0, 0xFF, 
  0x8, 0x8, 0xF, 0x8, 0xF, 
  0x28, 0x28, 0x28, 0x3F, 0x0, 
  0x28, 0x28, 0xEF, 0x0, 0xFF, 
  0x0, 0x0, 0xFF, 0x0, 0xFF, 
  0x28, 0x28, 0x2F, 0x20, 0x3F, 
  0x28, 0x28, 0xE8, 0x8, 0xF8, 
  0x8, 0x8, 0xF8, 0x8, 0xF8, 
  0x28, 0x28, 0x28, 0xF8, 0x0, 
  0x8, 0x8, 0x8, 0xF, 0x0, 
  0x0, 0x0, 0x0, 0xF8, 0x8,
  0x8, 0x8, 0x8, 0xF8, 0x8,
  0x8, 0x8, 0x8, 0xF, 0x8,
  0x0, 0x0, 0x0, 0xFF, 0x8,
  0x8, 0x8, 0x8, 0x8, 0x8,
  0x8, 0x8, 0x8, 0xFF, 0x8,
  0x0, 0x0, 0x0, 0xFF, 0x28,
  0x0, 0x0, 0xFF, 0x0, 0xFF,
  0x0, 0x0, 0xF8, 0x8, 0xE8,
  0x0, 0x0, 0x3F, 0x20, 0x2F,
  0x28, 0x28, 0xE8, 0x8, 0xE8,
  0x28, 0x28, 0x2F, 0x20, 0x2F,
  0x0, 0x0, 0xFF, 0x0, 0xEF,
  0x28, 0x28, 0x28, 0x28, 0x28,
  0x28, 0x28, 0xEF, 0x0, 0xEF,
  0x28, 0x28, 0x28, 0xE8, 0x28,
  0x8, 0x8, 0xF8, 0x8, 0xF8,
  0x28, 0x28, 0x28, 0x2F, 0x28,
  0x8, 0x8, 0xF, 0x8, 0xF,
  0x0, 0x0, 0xF8, 0x8, 0xF8,
  0x0, 0x0, 0x0, 0xF8, 0x28,
  0x0, 0x0, 0x0, 0x3F, 0x28,
  0x0, 0x0, 0xF, 0x8, 0xF,
  0x8, 0x8, 0xFF, 0x8, 0xFF,
  0x28, 0x28, 0x28, 0xFF, 0x28,
  0x8, 0x8, 0x8, 0xF8, 0x0,
  0x0, 0x0, 0x0, 0xF, 0x8,
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
  0xF, 0xF, 0xF, 0xF, 0xF,
  0xFF, 0xFF, 0xFF, 0x0, 0x0, 
  0x0, 0x0, 0x0, 0xFF, 0xFF,
  0xF0, 0xF0, 0xF0, 0xF0, 0xF0,
  0x1C, 0x22, 0x22, 0x1C, 0x22, 
  0x3E, 0x54, 0x54, 0x7C, 0x28, 
  0x7E, 0x40, 0x40, 0x60, 0x60, 
  0x40, 0x7E, 0x40, 0x7E, 0x40, 
  0xC6, 0xAA, 0x92, 0x82, 0xC6, 
  0x1C, 0x22, 0x22, 0x3C, 0x20, 
  0x2, 0x7E, 0x4, 0x78, 0x4, 
  0x60, 0x40, 0x7E, 0x40, 0x40, 
  0x99, 0xA5, 0xE7, 0xA5, 0x99, 
  0x38, 0x54, 0x92, 0x54, 0x38, 
  0x32, 0x4E, 0x80, 0x4E, 0x32, 
  0xC, 0x52, 0xB2, 0xB2, 0xC, 
  0xC, 0x12, 0x1E, 0x12, 0xC, 
  0x3D, 0x46, 0x5A, 0x62, 0xBC, 
  0x7C, 0x92, 0x92, 0x92, 0x0, 
  0x7E, 0x80, 0x80, 0x80, 0x7E, 
  0x54, 0x54, 0x54, 0x54, 0x54, 
  0x22, 0x22, 0xFA, 0x22, 0x22, 
  0x2, 0x8A, 0x52, 0x22, 0x2, 
  0x2, 0x22, 0x52, 0x8A, 0x2, 
  0x0, 0x0, 0xFF, 0x80, 0xC0, 
  0x7, 0x1, 0xFF, 0x0, 0x0, 
  0x10, 0x10, 0xD6, 0xD6, 0x10,
  0x6C, 0x48, 0x6C, 0x24, 0x6C, 
  0x60, 0xF0, 0x90, 0xF0, 0x60, 
  0x0, 0x0, 0x18, 0x18, 0x0, 
  0x0, 0x0, 0x8, 0x8, 0x0, 
  0xC, 0x2, 0xFF, 0x80, 0x80, 
  0x0, 0xF8, 0x80, 0x80, 0x78, 
  0x0, 0x98, 0xB8, 0xE8, 0x48, 
  0x0, 0x3C, 0x3C, 0x3C, 0x3C,};

// the memory buffer for the LCD
//Sparki Logo
uint8_t st7565_buffer[1024] = {
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,

0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x01,0x01,0x03,0x03,0x07,0x07,0x0F,
0x0F,0x0F,0x0F,0x0F,0x0F,0x07,0x03,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x01,0x0F,0x07,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,

0x03,0x07,0x0F,0x3F,0x7F,0x7F,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFE,0xFE,0xFC,0xFC,0xFC,0xFC,0x78,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x3F,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0x38,0x00,0x00,0x00,0x00,0x00,0x01,0x83,0xE1,0xF3,0x7B,0x7F,0x3F,0x3F,0x1F,
0x1F,0x3F,0xFF,0xFF,0xFF,0x1F,0x3F,0x7F,0x7F,0xFF,0xE7,0xC7,0x87,0x02,0x02,0x02,

0x80,0xE0,0xF0,0xF8,0xF8,0xF8,0xFC,0xFE,0xFE,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0x1F,0x0F,0x07,0x07,0x03,0x03,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x06,0x0F,
0x1F,0x1F,0x1F,0x1F,0x3F,0x3F,0x3F,0x3F,0x3F,0x3F,0x3F,0x3F,0x1F,0x1F,0x0F,0x0F,
0x07,0x03,0x00,0x00,0x3F,0x3F,0x3F,0x3F,0x3F,0x3F,0x3F,0x3F,0x3F,0x3F,0x3F,0x3F,
0x3F,0x1F,0x1F,0x0F,0x07,0x00,0x00,0x00,0x00,0x3F,0x3F,0x3F,0x3F,0x3F,0x1F,0x0F,
0x1F,0x1F,0x1F,0x1F,0x1E,0x1F,0x1F,0x1F,0x0E,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0x03,0x03,0x07,0x07,0x0F,0x1F,0x1F,0x1F,0x0E,0x04,0x80,0x80,0x80,0x80,0x87,
0xCF,0xCF,0xDF,0xDF,0xDF,0xDF,0xE0,0xE0,0xE0,0xE0,0xE0,0xF0,0x70,0x70,0x30,0x00,

0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x80,0x80,0xC0,0xC1,
0xE1,0xE1,0xF1,0xF3,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x7F,0x3F,0x0F,0x00,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0x1F,0x03,0x07,0x07,0x0F,0x0F,0x0F,0xBF,0xFF,0xFF,0xFF,
0xFE,0xF8,0xF0,0x07,0x0F,0x1F,0x1F,0x1F,0x3F,0x3F,0x3C,0x3C,0x38,0x38,0x38,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xC0,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xDF,0x8F,0x8F,0x0F,0x07,0x03,0x01,0x00,0x00,0x00,0x00,0x3F,0xFF,
0xFF,0xFF,0xFF,0xFF,0xF8,0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,

0x00,0x00,0x00,0x0F,0x7F,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFE,0xFE,0xFE,0xFC,0xFC,0xF8,0xF8,0xF0,0xF0,0xE0,0xC0,0x80,0x00,0xC0,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xF7,0xF0,0xF0,0xE0,0xC0,0xC0,0x80,0x80,0x00,
0x00,0x00,0x00,0x80,0xE0,0xE0,0xE0,0xF0,0xF0,0xF0,0xF0,0xF0,0xF0,0xF0,0xF0,0xF0,
0xF0,0xF0,0xF0,0xF0,0xF0,0xF0,0x00,0x00,0x00,0xF0,0xF0,0xF0,0xF0,0xF0,0xF0,0xF0,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xF0,0xF0,0xF0,0xF0,0xF0,0xF0,
0xC0,0x00,0x80,0x80,0xC0,0xE0,0xE0,0xE0,0xE0,0xE0,0x00,0x00,0x00,0xF0,0xF0,0xF0,
0xF0,0xF0,0xF0,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,

0x00,0x00,0x00,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0xE0,0xFC,0xFC,0xFC,0xFC,0xFC,0xF8,0xF8,0x30,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,

0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
};

// reduces how much is refreshed, which speeds it up!
// originally derived from Steve Evans/JCW's mod but cleaned up and
// optimized
#define enablePartialUpdate

#ifdef enablePartialUpdate
static uint8_t xUpdateMin, xUpdateMax, yUpdateMin, yUpdateMax;
#endif



static void updateBoundingBox(uint8_t xmin, uint8_t ymin, uint8_t xmax, uint8_t ymax) {
#ifdef enablePartialUpdate
  if (xmin < xUpdateMin) xUpdateMin = xmin;
  if (xmax > xUpdateMax) xUpdateMax = xmax;
  if (ymin < yUpdateMin) yUpdateMin = ymin;
  if (ymax > yUpdateMax) yUpdateMax = ymax;
#endif
}

void SparkiClass::drawBitmap(uint8_t x, uint8_t y, 
			const uint8_t *bitmap, uint8_t w, uint8_t h) {
  for (uint8_t j=0; j<h; j++) {
    for (uint8_t i=0; i<w; i++ ) {
      if (pgm_read_byte(bitmap + i + (j/8)*w) & _BV(j%8)) {
	my_setpixel(x+i, y+j, WHITE);
      }
    }
  }

  updateBoundingBox(x, y, x+w, y+h);
}

void SparkiClass::moveUpLine() {
    memmove(st7565_buffer, st7565_buffer+128, 1024-128);
    memset(st7565_buffer+1024-128,0,128);
    updateBoundingBox(0, 0, LCDWIDTH-1, LCDHEIGHT-1);
}

uint8_t print_char_x = 0;
uint8_t print_line_y = 0;

void SparkiClass::textWrite(const char* buffer, uint16_t len) {
  for (uint16_t i=0;i<len;i++){
    if(buffer[i] == '\n'){
      print_line_y++;
      print_char_x=0;
    }
    else{
        if(buffer[i] != '\r'){
            drawChar(print_char_x, print_line_y, buffer[i]);
            print_char_x += 6; // 6 pixels wide
            if (print_char_x + 6 >= LCDWIDTH) {
              print_char_x = 0;    // ran out of this line
              print_line_y++;
            }
        }
    }

    if (print_line_y >= (LCDHEIGHT/8)) {
      moveUpLine();
      print_line_y--;
    }
  }
}



void SparkiClass::drawString(uint8_t x, uint8_t line, char *c) {
  while (c[0] != 0) {
    drawChar(x, line, c[0]);
    c++;
    x += 6; // 6 pixels wide
    if (x + 6 >= LCDWIDTH) {
      x = 0;    // ran out of this line
      line++;
    }
    if (line >= (LCDHEIGHT/8)) {
      moveUpLine();
      line--;
    }
  }
}

void SparkiClass::drawString_P(uint8_t x, uint8_t line, const char *str) {
  while (1) {
    char c = pgm_read_byte(str++);
    if (! c)
      return;
    drawChar(x, line, c);
    x += 6; // 6 pixels wide
    if (x + 6 >= LCDWIDTH) {
      x = 0;    // ran out of this line
      line++;
    }
    if (line >= (LCDHEIGHT/8))
      return;        // ran out of space :(
  }
}

void  SparkiClass::drawChar(uint8_t x, uint8_t line, char c) {
  for (uint8_t i =0; i<5; i++ ) {
    st7565_buffer[x + (line*128) ] = pgm_read_byte(font+(c*5)+i);
    x++;
  }

  updateBoundingBox(x, line*8, x+5, line*8 + 8);
}

// bresenham's algorithm - thx wikpedia
void SparkiClass::drawLine(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1) {
  uint8_t steep = abs(y1 - y0) > abs(x1 - x0);
  if (steep) {
    swap(x0, y0);
    swap(x1, y1);
  }

  if (x0 > x1) {
    swap(x0, x1);
    swap(y0, y1);
  }

  // much faster to put the test here, since we've already sorted the points
  updateBoundingBox(x0, y0, x1, y1);

  uint8_t dx, dy;
  dx = x1 - x0;
  dy = abs(y1 - y0);

  int8_t err = dx / 2;
  int8_t ystep;

  if (y0 < y1) {
    ystep = 1;
  } else {
    ystep = -1;}

  for (; x0<=x1; x0++) {
    if (steep) {
      my_setpixel(y0, x0, WHITE);
    } else {
      my_setpixel(x0, y0, WHITE);
    }
    err -= dy;
    if (err < 0) {
      y0 += ystep;
      err += dx;
    }
  }
}

// filled rectangle
void SparkiClass::drawRectFilled(uint8_t x, uint8_t y, uint8_t w, uint8_t h) {

  // stupidest version - just pixels - but fast with internal buffer!
  for (uint8_t i=x; i<x+w; i++) {
    for (uint8_t j=y; j<y+h; j++) {
      my_setpixel(i, j, WHITE);
    }
  }

  updateBoundingBox(x, y, x+w, y+h);
}

// draw a rectangle
void SparkiClass::drawRect(uint8_t x, uint8_t y, uint8_t w, uint8_t h) {
  // stupidest version - just pixels - but fast with internal buffer!
  for (uint8_t i=x; i<x+w; i++) {
    my_setpixel(i, y, WHITE);
    my_setpixel(i, y+h-1, WHITE);
  }
  for (uint8_t i=y; i<y+h; i++) {
    my_setpixel(x, i, WHITE);
    my_setpixel(x+w-1, i, WHITE);
  } 

  updateBoundingBox(x, y, x+w, y+h);
}

// draw a circle outline
void SparkiClass::drawCircle(uint8_t x0, uint8_t y0, uint8_t r) {
  updateBoundingBox(x0-r, y0-r, x0+r, y0+r);

  int8_t f = 1 - r;
  int8_t ddF_x = 1;
  int8_t ddF_y = -2 * r;
  int8_t x = 0;
  int8_t y = r;

  my_setpixel(x0, y0+r, WHITE);
  my_setpixel(x0, y0-r, WHITE);
  my_setpixel(x0+r, y0, WHITE);
  my_setpixel(x0-r, y0, WHITE);

  while (x<y) {
    if (f >= 0) {
      y--;
      ddF_y += 2;
      f += ddF_y;
    }
    x++;
    ddF_x += 2;
    f += ddF_x;
  
    my_setpixel(x0 + x, y0 + y, WHITE);
    my_setpixel(x0 - x, y0 + y, WHITE);
    my_setpixel(x0 + x, y0 - y, WHITE);
    my_setpixel(x0 - x, y0 - y, WHITE);
    
    my_setpixel(x0 + y, y0 + x, WHITE);
    my_setpixel(x0 - y, y0 + x, WHITE);
    my_setpixel(x0 + y, y0 - x, WHITE);
    my_setpixel(x0 - y, y0 - x, WHITE);
    
  }
}

void SparkiClass::drawCircleFilled(uint8_t x0, uint8_t y0, uint8_t r) {
  updateBoundingBox(x0-r, y0-r, x0+r, y0+r);

  int8_t f = 1 - r;
  int8_t ddF_x = 1;
  int8_t ddF_y = -2 * r;
  int8_t x = 0;
  int8_t y = r;

  for (uint8_t i=y0-r; i<=y0+r; i++) {
    my_setpixel(x0, i, WHITE);
  }

  while (x<y) {
    if (f >= 0) {
      y--;
      ddF_y += 2;
      f += ddF_y;
    }
    x++;
    ddF_x += 2;
    f += ddF_x;
  
    for (uint8_t i=y0-y; i<=y0+y; i++) {
      my_setpixel(x0+x, i, WHITE);
      my_setpixel(x0-x, i, WHITE);
    } 
    for (uint8_t i=y0-x; i<=y0+x; i++) {
      my_setpixel(x0+y, i, WHITE);
      my_setpixel(x0-y, i, WHITE);
    }    
  }
}

void SparkiClass::my_setpixel(uint8_t x, uint8_t y, uint8_t color) {
  if ((x >= LCDWIDTH) || (y >= LCDHEIGHT))
    return;

  // x is which column
  if (color) 
    st7565_buffer[x+ (y/8)*128] |= _BV(7-(y%8));  
  else
    st7565_buffer[x+ (y/8)*128] &= ~_BV(7-(y%8)); 
}

// the most basic function, set a single pixel
void SparkiClass::drawPixel(uint8_t x, uint8_t y) {
  if ((x >= LCDWIDTH) || (y >= LCDHEIGHT))
    return;

  // x is which column
  if (WHITE) 
    st7565_buffer[x+ (y/8)*128] |= _BV(7-(y%8));  
  else
    st7565_buffer[x+ (y/8)*128] &= ~_BV(7-(y%8)); 

  updateBoundingBox(x,y,x,y);
}


// the most basic function, get a single pixel
uint8_t SparkiClass::readPixel(uint8_t x, uint8_t y) {
  if ((x >= LCDWIDTH) || (y >= LCDHEIGHT))
    return 0;

  return (st7565_buffer[x+ (y/8)*128] >> (7-(y%8))) & 0x1;  
}

void SparkiClass::beginDisplay() {
  startSPI();
  st7565_init();
  st7565_command(CMD_DISPLAY_ON);
  st7565_command(CMD_SET_ALLPTS_NORMAL);
  st7565_set_brightness(0x00);
}


void SparkiClass::startSPI(){
  // Setup all the SPI pins
  pinMode(SCK, OUTPUT);
  pinMode(MOSI, OUTPUT);
  pinMode(SS, OUTPUT);
  
  digitalWrite(SCK, LOW);
  digitalWrite(MOSI, LOW);
  digitalWrite(SS, HIGH);

  // Warning: if the SS pin ever becomes a LOW INPUT then SPI 
  // automatically switches to Slave, so the data direction of 
  // the SS pin MUST be kept as OUTPUT.
  SPCR |= _BV(MSTR);
  SPCR |= _BV(SPE);
  
  // Set SPI as fast as possible
  SPCR = (SPCR & ~SPI_CLOCK_MASK) | (SPI_CLOCK_DIV2 & SPI_CLOCK_MASK);
  SPSR = (SPSR & ~SPI_2XCLOCK_MASK) | ((SPI_CLOCK_DIV2 >> 2) & SPI_2XCLOCK_MASK);
}

void SparkiClass::st7565_init(void) {
  // set pin directions
  pinMode(LCD_A0, OUTPUT);
  pinMode(LCD_RST, OUTPUT);
  pinMode(LCD_CS, OUTPUT);

  // toggle RST low to reset; CS low so it'll listen to us
  digitalWrite(LCD_CS, LOW);
  _delay_ms(100);
  digitalWrite(LCD_RST, LOW);
  _delay_ms(100);
  digitalWrite(LCD_RST, HIGH);

  // LCD bias select
  st7565_command(CMD_SET_BIAS_7);
  // ADC select
  st7565_command(CMD_SET_ADC_REVERSE);
  // SHL select
  st7565_command(CMD_SET_COM_NORMAL);
  // Initial display line
  st7565_command(CMD_SET_DISP_START_LINE);

  // turn on voltage converter (VC=1, VR=0, VF=0)
  st7565_command(CMD_SET_POWER_CONTROL | 0x4);
  // wait for 50% rising
  _delay_ms(50);

  // turn on voltage regulator (VC=1, VR=1, VF=0)
  st7565_command(CMD_SET_POWER_CONTROL | 0x6);
  // wait >=50ms
  _delay_ms(50);

  // turn on voltage follower (VC=1, VR=1, VF=1)
  st7565_command(CMD_SET_POWER_CONTROL | 0x7);
  // wait
  _delay_ms(10);

  // set lcd operating voltage (regulator resistor, ref voltage resistor)
  st7565_command(CMD_SET_RESISTOR_RATIO | 0x6);
  // initial display line
  // set page address
  // set column address
  // write display data

  // set up a bounding box for screen updates

  updateBoundingBox(0, 0, LCDWIDTH-1, LCDHEIGHT-1);
}

inline void SparkiClass::spiwrite(uint8_t c) {
noInterrupts();
    //  un chip-select the shift registers
    PORTD |= 0x20;    // pull PD5 high to latch in spi transfers
    //  chip-select the screen
    digitalWrite(LCD_CS,LOW);

    SPDR = c; // push the byte to be loaded to the SPI register
    while(!(SPSR & (1<<SPIF))); //wait till the register completes

    digitalWrite(LCD_CS,HIGH);
interrupts();
}
void SparkiClass::st7565_command(uint8_t c) {
  digitalWrite(LCD_A0, LOW);

  spiwrite(c);
}

void SparkiClass::st7565_data(uint8_t c) {
  digitalWrite(LCD_A0, HIGH);

  spiwrite(c);
}
void SparkiClass::st7565_set_brightness(uint8_t val) {
    st7565_command(CMD_SET_VOLUME_FIRST);
    st7565_command(CMD_SET_VOLUME_SECOND | (val & 0x3f));
}


void SparkiClass::updateLCD(void) {
  uint8_t col, maxcol, p;

  /*
  Serial.print("Refresh ("); Serial.print(xUpdateMin, DEC); 
  Serial.print(", "); Serial.print(xUpdateMax, DEC);
  Serial.print(","); Serial.print(yUpdateMin, DEC); 
  Serial.print(", "); Serial.print(yUpdateMax, DEC); Serial.println(")");
  */

  for(p = 0; p < 8; p++) {
    /*
      putstring("new page! ");
      uart_putw_dec(p);
      putstring_nl("");
    */
#ifdef enablePartialUpdate
    // check if this page is part of update
    if ( yUpdateMin >= ((p+1)*8) ) {
      continue;   // nope, skip it!
    }
    if (yUpdateMax < p*8) {
      break;
    }
#endif

     st7565_command(CMD_SET_PAGE | pagemap[p]);


#ifdef enablePartialUpdate
    col = xUpdateMin;
    maxcol = xUpdateMax;
#else
    // start at the beginning of the row
    col = 0;
    maxcol = LCDWIDTH-1;
#endif

    st7565_command(CMD_SET_COLUMN_LOWER | ((col+ST7565_STARTBYTES) & 0xf));
    st7565_command(CMD_SET_COLUMN_UPPER | (((col+ST7565_STARTBYTES) >> 4) & 0x0F));
    st7565_command(CMD_RMW);
    
    for(; col <= maxcol; col++) {
      //uart_putw_dec(col);
      //uart_putchar(' ');
      st7565_data(st7565_buffer[(128*p)+col]);
    }
  }

#ifdef enablePartialUpdate
  xUpdateMin = LCDWIDTH - 1;
  xUpdateMax = 0;
  yUpdateMin = LCDHEIGHT-1;
  yUpdateMax = 0;
#endif
}

// clear everything
void SparkiClass::clearLCD(void) {
  print_char_x = 0;
  print_line_y = 0;
  memset(st7565_buffer, 0, 1024);
  updateBoundingBox(0, 0, LCDWIDTH-1, LCDHEIGHT-1);
}


// this doesnt touch the buffer, just clears the display RAM - might be handy
void SparkiClass::clear_display(void) {
  uint8_t p, c;
  
  for(p = 0; p < 8; p++) {
    /*
      putstring("new page! ");
      uart_putw_dec(p);
      putstring_nl("");
    */

    st7565_command(CMD_SET_PAGE | p);
    for(c = 0; c < 129; c++) {
      //uart_putw_dec(c);
      //uart_putchar(' ');
      st7565_command(CMD_SET_COLUMN_LOWER | (c & 0xf));
      st7565_command(CMD_SET_COLUMN_UPPER | ((c >> 4) & 0xf));
      st7565_data(0x0);
    }     
  }
}


void SparkiClass::readAccelData()
{
  int accelCount[3];
  uint8_t rawData[6];  // x/y/z accel register data stored here
  
  
  readi2cRegisters(0x01, 6, &rawData[0], MMA8452_ADDRESS);  // Read the six raw data registers into data array
  

  // Loop to calculate 12-bit ADC and g value for each axis
  for (uint8_t i=0; i<6; i+=2)
  {
    accelCount[i/2] = ((rawData[i] << 8) | rawData[i+1]) >> 4;  // Turn the MSB and LSB into a 12-bit value
    if (rawData[i] > 0x7F)
    {  
      // If the number is negative, we have to make it so manually (no 12-bit data type)
      accelCount[i/2] = ~accelCount[i/2] + 1;
      accelCount[i/2] *= -1;  // Transform into negative 2's complement #
    }
  }
  xAxisAccel = (float) accelCount[0]/((1<<12)/(2*ACCEL_SCALE));
  yAxisAccel = (float) accelCount[1]/((1<<12)/(2*ACCEL_SCALE));
  zAxisAccel = (float) accelCount[2]/((1<<12)/(2*ACCEL_SCALE));
}

int SparkiClass::initAccelerometer()
{
  uint8_t c = readi2cRegister(0x0D, MMA8452_ADDRESS);  // Read WHO_AM_I register
  if (c == 0x2A){ // WHO_AM_I should always be 0x2
    // Must be in standby to change registers, so we do that
    c = readi2cRegister(0x2A, MMA8452_ADDRESS);
    readi2cRegister(0x2A, c & ~(0x01), MMA8452_ADDRESS);
  
    // Set up the full scale range
    readi2cRegister(0x0E, ACCEL_SCALE >> 2, MMA8452_ADDRESS); 
  
    // Setup the 3 data rate bits, from 0 to 7
    readi2cRegister(0x2A, readi2cRegister(0x2A, MMA8452_ADDRESS) & ~(0x38), MMA8452_ADDRESS);
    if (ACCEL_DATARATE <= 7)
      readi2cRegister(0x2A, readi2cRegister(0x2A, MMA8452_ADDRESS) | (ACCEL_DATARATE << 3), MMA8452_ADDRESS);  
  
    // Set back to active mode to start reading
    c = readi2cRegister(0x2A, MMA8452_ADDRESS);
    readi2cRegister(0x2A, c | 0x01, MMA8452_ADDRESS);
    return 1;
  }
  else{
    return -1;
  }
}

// Read i registers sequentially, starting at address into the dest byte array
void SparkiClass::readi2cRegisters(byte address, int i, byte * dest, uint8_t i2cAddress)
{
  i2cSendStart();
  i2cWaitForComplete();

  i2cSendByte((i2cAddress<<1)); // write 0xB4
  i2cWaitForComplete();

  i2cSendByte(address);	// write register address
  i2cWaitForComplete();

  i2cSendStart();
  i2cSendByte((i2cAddress<<1)|0x01); // write 0xB5
  i2cWaitForComplete();
  for (int j=0; j<i; j++)
  {
    i2cReceiveByte(-1); // -1 = True
    i2cWaitForComplete();
    dest[j] = i2cGetReceivedByte(); // Get MSB result
  }
  i2cWaitForComplete();
  i2cSendStop();

  cbi(TWCR, TWEN); // Disable TWI
  sbi(TWCR, TWEN); // Enable TWI
}

// Read a single byte from address and return it as a byte
byte SparkiClass::readi2cRegister(uint8_t address, uint8_t i2cAddress)
{
  byte data;
  
  i2cSendStart();
  i2cWaitForComplete();
  
  i2cSendByte((i2cAddress<<1)); // Write 0xB4
  i2cWaitForComplete();
  
  i2cSendByte(address);	// Write register address
  i2cWaitForComplete();
  
  i2cSendStart();
  
  i2cSendByte((i2cAddress<<1)|0x01); // Write 0xB5
  i2cWaitForComplete();
  i2cReceiveByte(-1); // -1 = True
  i2cWaitForComplete();
  
  data = i2cGetReceivedByte();	// Get MSB result
  i2cWaitForComplete();
  i2cSendStop();
  
  cbi(TWCR, TWEN);	// Disable TWI
  sbi(TWCR, TWEN);	// Enable TWI
  
  return data;
}

// Writes a single byte (data) into address
void SparkiClass::readi2cRegister(unsigned char address, unsigned char data, uint8_t i2cAddress)
{
  i2cSendStart();
  i2cWaitForComplete();

  i2cSendByte((i2cAddress<<1)); // Write 0xB4
  i2cWaitForComplete();

  i2cSendByte(address);	// Write register address
  i2cWaitForComplete();

  i2cSendByte(data);
  i2cWaitForComplete();

  i2cSendStop();
}

