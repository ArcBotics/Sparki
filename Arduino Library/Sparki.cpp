#include "Sparki.h"
#include <inttypes.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include "Arduino.h"

#include "SPI.h"

static int8_t step_dir[3];                 // -1 = ccw, 1 = cw  
static volatile uint8_t speed_index[3];     // counter controlling motor speed 
static uint8_t motor_speed[3];              // stores last set motor speed (0-100%)
static volatile int8_t step_index[3];       // index into _steps array  
static uint8_t _steps_right[9];                   // bytes defining stepper coil activations
static uint8_t _steps_left[9];                   // bytes defining stepper coil activations
static volatile uint32_t remainingSteps[3]; // number of steps before stopping motor
static volatile uint32_t isRunning[3];      // tells if motor is running

static volatile int speedCounter[3];      // variable used in maintaing speed
static volatile int speedCount[3];      // what speedCount is set at when speed cycle resets

static volatile uint8_t shift_outputs[3];      // tells if motor is running

// initialize the RGB timer variables
static volatile uint8_t RGB_vals[3];
static volatile uint16_t RGB_timer;

static volatile uint8_t irSwitch;
static volatile uint8_t irSwitch2;

// keeps track of what's being outputted to the shift registers


//static volatile int speedCounter;

SparkiClass::SparkiClass()
{
 //begin();
 /*
 digitalWrite(STATUS_LED, HIGH);
 delay(500);
 digitalWrite(STATUS_LED, LOW);
 delay(500); 
 digitalWrite(STATUS_LED, HIGH);
 delay(500);
 digitalWrite(STATUS_LED, LOW);
 delay(500);  
 */
}

void SparkiClass::begin( ) {
  Serial.begin(9600);

  // setup the IR Switch
  irSwitch = 0;

  // set up the Status LED
  pinMode(STATUS_LED, OUTPUT);
  digitalWrite(STATUS_LED, LOW);

  // set up servo
  pinMode(SERVO,OUTPUT);
  //myservo.attach(SERVO);
  //myservo.write(90);

  // setup variables for the RGB
  RGB_vals[0] = 0;
  RGB_vals[1] = 0;
  RGB_vals[2] = 0;
  
  RGB_timer = 0;

  // Setup the SPI bus for the shift register
  // !!! Need to remove the essential functions from the SPI Library, 
  // !!! and include in the main code
  SPI.begin(); 
  SPI.setClockDivider(SPI_CLOCK_DIV2); 

  // defining steps for the stepper motors
  _steps_left[0] = 0x10;
  _steps_left[1] = 0x30;
  _steps_left[2] = 0x20;
  _steps_left[3] = 0x60;
  _steps_left[4] = 0x40;
  _steps_left[5] = 0xC0;
  _steps_left[6] = 0x80;
  _steps_left[7] = 0x90;
  _steps_left[8]  = 0x00;

  _steps_right[0] = 0x01;
  _steps_right[1] = 0x03;
  _steps_right[2] = 0x02;
  _steps_right[3] = 0x06;
  _steps_right[4] = 0x04;
  _steps_right[5] = 0x0C;
  _steps_right[6] = 0x08;
  _steps_right[7] = 0x09;
  _steps_right[8] = 0x00;

  
  // Buzzer
  pinMode(BUZZER, OUTPUT);
  digitalWrite(BUZZER, LOW);
  
  // Light Sensors
  //pinMode(LIGHT_LEFT, INPUT);  
  //pinMode(LIGHT_CENTER, INPUT); 
  //pinMode(LIGHT_RIGHT, INPUT); 
  
  // IR Line Sensors
  //pinMode(LINE_LEFT, INPUT);  
  //pinMode(LINE_CENTER, INPUT); 
  //pinMode(LINE_RIGHT, INPUT); 
  
  // IR Remote
  pinMode(IR_RECEIVE, INPUT);
  
  // IR Send
  pinMode(IR_SEND, OUTPUT);
  
  // Stepper
  motor_speed[MOTOR_LEFT] = motor_speed[MOTOR_RIGHT] = motor_speed[MOTOR_GRIPPER] =100;

  // Ultrasonic
  pinMode(ULTRASONIC_TRIG, OUTPUT);
  pinMode(ULTRASONIC_ECHO, INPUT);

  
  // Set up the scheduler routine to run every 100uS, based off Timer4
  
  cli();          // disable all interrupts
  TCCR4A = 0;
  TCCR4B = 0;
  TCNT4  = 0;

  OCR4A = 48;               // compare match register 16MHz/32/10000Hz
  TCCR4B |= (1 << WGM12);   // CTC mode
  TCCR4B = 0x06;            // CLK/32 prescaler (32 = 2^(0110-1))
  TIMSK4 |= (1 << OCIE4A);  // enable timer compare interrupt
  sei();             // enable all interrupts
  
  
  //Timer1.initialize(100); // the scheduler function gets called every 100uS
  //Timer1.attachInterrupt(scheduler);
}

/*
* Infrared line sensors
*/


int SparkiClass::edgeRight(){
    return readIR(EDGE_RIGHT);
}

int SparkiClass::lineRight(){
    return readIR(LINE_RIGHT);
}

int SparkiClass::lineCenter(){
	return readIR(LINE_CENTER);
}

int SparkiClass::lineLeft(){
	return readIR(LINE_LEFT);
}

int SparkiClass::edgeLeft(){
	return readIR(EDGE_LEFT);	
}

int SparkiClass::readIR(int pin){
    int read=0;
	onIR();
	read = analogRead(pin);
	offIR();
	return read;
}

void SparkiClass::beep(){
  for(short i=0; i<300; i++){
      digitalWrite(BUZZER, HIGH);
      delayMicroseconds(300);
      digitalWrite(BUZZER, LOW);
      delayMicroseconds(300);
    }
}

/*
 * motor control (non-blocking)
 * speed is percent 0-100
*/

void SparkiClass::RGB(uint8_t R, uint8_t G, uint8_t B)
{
	RGB_vals[0] = R;
	RGB_vals[1] = G;
	RGB_vals[2] = B;
}


void SparkiClass::moveRight(float deg)
{
  float turn = 21.388888*deg;
  rotateRight();
  delay(long(turn));
  moveStop();
}

void SparkiClass::moveRight()
{
  motorRotate(MOTOR_LEFT, DIR_CCW, 100);
  motorRotate(MOTOR_RIGHT, DIR_CCW, 100);
  
}

void SparkiClass::moveLeft(float deg)
{
  float turn = 21.388888*deg;
  rotateLeft();
  delay(long(turn));
  moveStop();
}

void SparkiClass::moveLeft()
{
  motorRotate(MOTOR_LEFT, DIR_CW, 100);
  motorRotate(MOTOR_RIGHT, DIR_CW, 100);
  
}

void SparkiClass::moveForward(float cm)
{
  float run = 22.2222222*cm;
  moveForward();
  delay(run);
  moveStop();
}

void SparkiClass::moveForward()
{
  motorRotate(MOTOR_LEFT, DIR_CCW, 100);
  motorRotate(MOTOR_RIGHT, DIR_CW, 100);
  
}

void SparkiClass::moveBackward(float cm)
{
  float run = 22.2222222*cm;
  moveBackward();
  delay(run);
  moveStop();
}

void SparkiClass::moveBackward()
{
  motorRotate(MOTOR_LEFT, DIR_CW, 100);
  motorRotate(MOTOR_RIGHT, DIR_CCW, 100);
}

void SparkiClass::moveStop()
{
  motorStop(MOTOR_LEFT);
  motorStop(MOTOR_RIGHT);
}


void SparkiClass::gripOpen()
{
  motorRotate(MOTOR_GRIPPER, DIR_CCW, 100);
}
void SparkiClass::gripClose()
{
  motorRotate(MOTOR_GRIPPER, DIR_CW, 100);
}
void SparkiClass::gripStop()
{
  motorStop(MOTOR_GRIPPER);
}



void SparkiClass::motorRotate(int motor, int direction,  int speed)
{
   //Serial.print("Motor ");Serial.print(motor); Serial.print(" rotate, dir= "); Serial.println(direction);
   uint8_t oldSREG = SREG;
   cli();
   motor_speed[motor] = speed;  
   step_dir[motor] = direction;  
   remainingSteps[motor] = ULONG_MAX; // motor stops after this many steps, almost 50 days of stepping if motor not stopped
   isRunning[motor] = true;
   speedCount[motor] = int(100.0/float(motor_speed[motor])*10.0);
   speedCounter[motor] = speedCount[motor];
   SREG = oldSREG; 
   delay(10);
}


void SparkiClass::motorStop(int motor)
{
   //Serial.println("Motor Stop"); 
   motor_speed[motor] = 0;  
   setSteps(motor, 0);
}

void SparkiClass::motorsRotateSteps( int leftDir, int rightDir,  int speed, uint32_t steps, bool wait)
{
  uint8_t oldSREG = SREG;
  cli();
  motor_speed[MOTOR_LEFT] = motor_speed[MOTOR_RIGHT] = speed;  
  step_dir[MOTOR_LEFT] = leftDir;   
  step_dir[MOTOR_RIGHT] = rightDir; 
  remainingSteps[MOTOR_LEFT] = remainingSteps[MOTOR_RIGHT] = steps;
  isRunning[MOTOR_LEFT] = isRunning[MOTOR_RIGHT] = true;

  SREG = oldSREG; 
  if( wait)
  {
    while ( areMotorsRunning() ){
      delay(10);  // remainingSteps is decrimented in the timer callback     
    }
  }  
}
 
 // returns true if one or both motors a still stepping
 bool SparkiClass::areMotorsRunning()
 {
   bool result;
   uint8_t oldSREG = SREG;
   cli();
   result =  isRunning[MOTOR_LEFT] || isRunning[MOTOR_RIGHT] || isRunning[MOTOR_GRIPPER] ;
   SREG = oldSREG; 
   return result;
 }
/*
void SparkiClass::servo(int pos){
	myservo.write(pos);
}
*/

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
  unsigned long maxloops = 500;
	
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

void SparkiClass::onGrabIR()  // turns off the IR Detection LEDs
{
    irSwitch2 = 1;
    delay(1); // give time for a scheduler cycle to run
}

void SparkiClass::offGrabIR() // turns off the IR Detection LEDs
{
    irSwitch2 = 0;
    delay(1); // give time for a scheduler cycle to run
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

 /*
  * private functions
  */
 
 // set the number if steps for the given motor 


void SparkiClass::setSteps(uint8_t motor, uint32_t steps)
{
   uint8_t oldSREG = SREG;
   cli();
   remainingSteps[motor] = steps; // motor stops after this many steps
   isRunning[motor] = steps > 0;
   SREG = oldSREG;    
}

uint32_t SparkiClass::getSteps(uint8_t motor )
{
   uint8_t oldSREG = SREG;
   cli();
   uint32_t steps = remainingSteps[motor];
   SREG = oldSREG;    
   return steps;
}

/***********************************************************************************
The Scheduler
Every 100uS (10,000 times a second), we update the 2 shift registers used to increase
the amount of outputs the processor has
***********************************************************************************/

ISR(TIMER4_COMPA_vect)          // interrupt service routine that wraps a user defined function supplied by attachInterrupt
{
//void SparkiClass::scheduler(){ 
    uint8_t oldSREG = SREG;
    // Clear the timer interrupt counter 
    TCNT4=0;
    
	// clear the shift register values so we can re-write them
    shift_outputs[0] = 0x00;
    shift_outputs[1] = 0x00;
    shift_outputs[2] = 0x00;
    
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
    if(RGB_timer == 256){
    	RGB_timer = 0;
    }

    // IR Detection Switch
    if(irSwitch == 1){
    	shift_outputs[0] |= 0x08;
    }
    
    if(irSwitch2 == 1){
        shift_outputs[2] |= 0x08;
    }

    //// Motor Control ////
    
    //   Determine what state the stepper coils are in
	for(byte motor=0; motor<3; motor++){
		if( remainingSteps[motor] > 1 ){ // check if finished stepping     
			if( speedCounter[motor] == 0) { // 
				step_index[motor] += step_dir[motor];
				remainingSteps[motor]--;
				speedCounter[motor] = speedCount[motor];
			}
			else{
				speedCounter[motor] = speedCounter[motor]-1;
			}
			
		}
		else {  // if this was the last step
			remainingSteps[motor] = 0;  
			isRunning[motor] = false;
			step_index[motor] = 8;
			speedCounter[motor] = -1;
		}     
		
		//   keep indicies from rolling over or under
		if( step_index[motor] >= 8){
			step_index[motor] = 0;
		}
		else if( step_index[motor] < 0){
			step_index[motor] = 7;
		}
	}

    shift_outputs[1] |= _steps_right[step_index[MOTOR_RIGHT]];
    shift_outputs[1] |= _steps_left[step_index[MOTOR_GRIPPER]];
    
    shift_outputs[2] |= _steps_left[step_index[MOTOR_LEFT]];
    

	//output values to shift registers
    PORTD &= 0xDF;    // pull PD5 low
    SPI.transfer(shift_outputs[2]);
    SPI.transfer(shift_outputs[1]);
    //SPI.transfer(shift_outputs[0]);
    PORTD |= 0x20;    // pull PD5 high to latch in spi transfers
    SREG = oldSREG;
}