#ifndef Sparki_h
#define Sparki_h

#include "Arduino.h"
#include "Print.h"

#define SHIFTREG_LATCH      TXLED0   // PD5
#define STATUS_LED          13        
#define BUZZER              11       // PB7
#define ULTRASONIC_ECHO     5        // PC6
#define ULTRASONIC_TRIG     10       // PB6
#define IR_RECEIVE          7        // PE6
#define IR_SEND             6        // PD7

#define SERVO               9        // 
#define SERVO_LEFT          -80
#define SERVO_CENTER        0
#define SERVO_RIGHT         80

// defining the MUX pins
#define MUX_ANALOG		A2 // PF5
#define MUX_A  	        A3 // PF4
#define MUX_B      		A4 // PF1
#define MUX_C        	A5 // PF0

// defining the IR line sensor pins (on the multiplexer)
#define IR_EDGE_RIGHT      LOW, HIGH, LOW	// Mux A2     
#define IR_LINE_RIGHT      HIGH, LOW, LOW	// Mux A1     
#define IR_LINE_CENTER     LOW, LOW, LOW	// Mux A0      
#define IR_LINE_LEFT       HIGH, HIGH, LOW	// Mux A3 
#define IR_EDGE_LEFT       HIGH, LOW, HIGH // Mux A5       

// defining the light sensors
#define LIGHT_RIGHT         HIGH, HIGH, HIGH // Mux A7
#define LIGHT_CENTER        LOW, HIGH, HIGH	 // Mux A6
#define LIGHT_LEFT          LOW, LOW, HIGH	 // Mux A4

// defining battery monitor constants
#define BATTERY_MONITOR     A10       // PB6
#define ADC_REFERENCE       2.56
#define VOLTAGE_DIVIDER     4.7  	 // using 47k and 10k resistors
#define MAX_BAT_V          (VOLTAGE_DIVIDER * ADC_REFERENCE) 

// defining the buttons on the shipped-with remote control
#define REMOTE_ERROR    -2
#define REMOTE_REPEAT   -3
#define REMOTE_POWER    69
#define REMOTE_MENU     71
#define REMOTE_TEST     68
#define REMOTE_PLUS     64
#define REMOTE_BACK     67
#define REMOTE_REWIND   7
#define REMOTE_PLAY     21
#define REMOTE_FF       9
#define REMOTE_0        22
#define REMOTE_MINUS    25
#define REMOTE_C        13
#define REMOTE_1        12
#define REMOTE_2        24
#define REMOTE_3        94
#define REMOTE_4        8
#define REMOTE_5        28
#define REMOTE_6        90
#define REMOTE_7        66
#define REMOTE_8        82
#define REMOTE_9        74

#define STEP_DELAY      1000

// define the shift registers pin output values for the RGB arrays
#define RGB_R 0x01 // pin value of the Red LED on the RGB on the shift register
#define RGB_G 0x02 // pin value of the Green LED on the RGB on the shift register
#define RGB_B 0x04 // pin value of the Blue LED on the RGB on the shift register
#define RGB_SHIFT 1 // which shift register the RGB is on (starts at 0)

#define RGB_RED     100, 0,   0
#define RGB_ORANGE  90,  100, 0
#define RGB_YELLOW  60,  100, 0
#define RGB_GREEN   0,   100, 0
#define RGB_BLUE    0,   0,   100
#define RGB_PINK    90,  0,   100
#define RGB_INDIGO  20,  0,   100
#define RGB_VIOLET  60,  0,   100
#define RGB_WHITE   60,  100, 90
#define RGB_OFF     0,   0,   0

// ----------------------------------------------------------------------------

#define SPARKI_DEBUG_L1_ACTION  0x01
#define SPARKI_DEBUG_L1_STATUS  0x02
#define SPARKI_DEBUG_L2_ACTION  0x04
#define SPARKI_DEBUG_L2_STATUS  0x08
#define SPARKI_DEBUG_L3_ACTION  0x10
#define SPARKI_DEBUG_L3_STATUS  0x20
#define SPARKI_DEBUG_MIDDLE     0x40
#define SPARKI_DEBUG_END        0x80

#define SPARKI_DEBUG_MODE       0x00

// ----------------------------------------------------------------------------

#define SPARKI_INTERRUPT_PERIOD_US  200

// ----------------------------------------------------------------------------

#define SPARKI_MOTOR_STEPS_PER_REV      4096
#define SPARKI_MOTOR_STEPS_OF_BACKLASH    48
#define SPARKI_MOTOR_PRIMING_STEPS         8

#define SPARKI_MOTOR_UPDATE_PERIOD_MIN_US   1000
#define SPARKI_MOTOR_UPDATE_PERIOD_MAX_US  10000
// NOTE: Assumptions/Requirements:
//   SPARKI_MOTOR_UPDATE_PERIOD_MIN_US >= SPARKI_INTERRUPT_PEIOD_US
//   SPARKI_MOTOR_UPDATE_PERIOD_MAX_US > SPARKI_MOTOR_UPDATE_PERIOD_MIN_US
// NOTE: From 28BYJ-48 5V stepper motor data sheet:
//   Frequency                    100Hz
//   Idle In-traction Frequency   > 600Hz
//   Idle Out-traction Frequency  > 1000Hz
// Not really sure what these mean...?
// The shortest stepper coil update period that
// seems to work reliabily is 1000us. If the 1000
// ends up being flaky on some Sparki units, the
// setting will need to be changed to 1200. (Or,
// if the interrupt period is reduced to 100us,
// perhaps a motor update period of 900us would
// work to make the motors a little faster?)

#define SPARKI_MOTOR_SPEED_DEFAULT_PERCENT   80
// NOTE: Speed to update period mapping:
//   100% --> effective motor update period == SPARKI_MOTOR_UPDATE_PERIOD_MIN_US
//     0% --> effective motor update period == SPARKI_MOTOR_UPDATE_PERIOD_MAX_US

#define SPARKI_MOTOR_COUNT  3

#define SPARKI_MOTOR_INDEX_WHEEL_LEFT   0
#define SPARKI_MOTOR_INDEX_WHEEL_RIGHT  1
#define SPARKI_MOTOR_INDEX_GRIPPER      2

#define SPARKI_MOTOR_ID_MASK_WHEEL_LEFT   0x01  // 0x01 << SPARKI_MOTOR_INDEX_WHEEL_LEFT
#define SPARKI_MOTOR_ID_MASK_WHEEL_RIGHT  0x02  // 0x01 << SPARKI_MOTOR_INDEX_WHEEL_RIGHT
#define SPARKI_MOTOR_ID_MASK_GRIPPER      0x04  // 0x01 << SPARKI_MOTOR_INDEX_GRIPPER

#define SPARKI_MOTOR_ID_MASK_WHEELS       0x03  // SPARKI_MOTOR_ID_MASK_WHEEL_LEFT | SPARKI_MOTOR_ID_MASK_WHEEL_RIGHT
#define SPARKI_MOTOR_ID_MASK_ALL          0x07  // SPARKI_MOTOR_ID_MASK_WHEELS | SPARKI_MOTOR_ID_MASK_GRIPPER

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#define SPARKI_MOTOR_STATUS_MASK_DIR_IS_CW       0x01
#define SPARKI_MOTOR_STATUS_MASK_ACT_HOLD_IS_ON  0x02
#define SPARKI_MOTOR_STATUS_MASK_BLCOMP_IS_ON    0x04
#define SPARKI_MOTOR_STATUS_MASK_BLCOMP_IS_PEND  0x08
#define SPARKI_MOTOR_STATUS_MASK_MTR_IS_RUNNING  0x80

#define SPARKI_SHIFT_REG_INDEX_MOTOR_WHEEL_LEFT   1
#define SPARKI_SHIFT_REG_INDEX_MOTOR_WHEEL_RIGHT  0
#define SPARKI_SHIFT_REG_INDEX_MOTOR_GRIPPER      0

#define SPARKI_SHIFT_REG_MASK_MOTOR_WHEEL_LEFT   0xf0
#define SPARKI_SHIFT_REG_MASK_MOTOR_WHEEL_RIGHT  0x0f
#define SPARKI_SHIFT_REG_MASK_MOTOR_GRIPPER      0xf0

#define SPARKI_MOTOR_DIR_CLOCKWISE         true
#define SPARKI_MOTOR_DIR_COUNTERCLOCKWISE  false

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#define SPARKI_WHEEL_DIAMETER_BASELINE_UM    510  // baseline wheel diameter is 51.0mm --> 510um
#define SPARKI_WHEEL_TRACK_WIDTH_UM          855  // baseline wheel separation is 85.5mm --> 855um
#define SPARKI_WHEEL_SEPARATION_BASELINE_UM  SPARKI_WHEEL_TRACK_WIDTH_UM

#define SPARKI_MOTOR_WHEEL_FP_FRACTION_BITS        6       // number of fractional bits for wheel-specific fixed point (fp) numbers
#define SPARKI_MOTOR_WHEEL_CIRCUMFERENCE_MM_FP     0x280E  // 161.792022mm as fixed point, assuming wheel diameter = 51.0mm
#define SPARKI_MOTOR_WHEEL_SPIN_CIRCLE_CIRC_MM_FP  0x4327  // 267.035376mm as fixed point, assuming wheel separation = 85.5mm

#define SPARKI_MOTOR_GRIPPER_OPEN_MAX_MM   70
#define SPARKI_MOTOR_GRIPPER_PINION_TEETH  11

#define SPARKI_GRIPPER_FP_FRACTION_BITS  10      // number of fractional bits for gripper-specific fixed point numbers
#define SPARKI_GRIPPER_TOOTH_STEP_MM_FP  0x0B9A  // 2.9mm as fixed point

// wheel -- fixed-point fraction bits: 6
// -------------------------------------
// diameter           circumference   
// 50.5  CA0  0.00%   158.650429  27AA  0.00%
// 51.0  CC0  0.00%   160.221225  280E  0.00% *
// 51.5  CE0  0.00%   161.792022  2873  0.00%
// 52.0  D00  0.00%   163.362818  28D7  0.00%
//  |     |     |
//  |     |     +--- percent difference between decimal and fixed point
//  |     +--- value as fixed point binary
//  +--- decimal value
//
// * == baseline value

// wheel -- fixed-point fraction bits: 8
// -------------------------------------
// diameter            circumference             drive mm to steps
// 50.5  3280  0.00%   158.650429  9EA7  0.00%   25.817768  19D1  -0.01%
// 51.0  3300  0.00%   160.221225  A039  0.00%   25.564653  1991   0.01% *
// 51.5  3380  0.00%   161.792022  A1CB  0.00%   25.316452  1951   0.00%
// 52.0  3400  0.00%   163.362818  A35D  0.00%   25.073025  1913   0.00%

// spin circle -- fixed-point fraction bits: 6
// -------------------------------------------
// diameter            circumference
// 84.0  1500  0.00%   263.893783  41F9  0.00%
// 84.5  1520  0.00%   265.464579  425E  0.00%
// 85.0  1540  0.00%   267.035376  42C2  0.00%
// 85.5  1560  0.00%   268.606172  4327  0.00% *
// 86.0  1580  0.00%   270.176968  438B  0.00%
// 86.5  15A0  0.00%   271.747765  43F0  0.00%
// 87.0  15C0  0.00%   273.318561  4454  0.00%
// (spin circle diameter == wheel separation)

// spin deg to steps -- fixed-point fraction bits: 8
// -------------------------------------------------
//            50.5       51.0       51.5       52.0         50.5  51.0  51.5  52.0           50.5    51.0    51.5    52.0
// 84.0  18.925413  18.739869  18.557929  18.379487   84.0  12ED  12BD  128F  1261   84.0   0.00%  -0.01%   0.00%   0.00%
// 84.5  19.038064  18.851416  18.668393  18.488889   84.5  130A  12DA  12AB  127D   84.5   0.01%   0.00%   0.00%   0.00%
// 85.0  19.150715  18.962963  18.778857  18.598291   85.0  1327  12F7  12C7  1299   85.0   0.01%   0.01%  -0.01%   0.00%
// 85.5  19.263366  19.074510  18.889320  18.707692   85.5  1343  1313  12E4  12B5   85.5  -0.01%   0.00%   0.01%   0.00% *
// 86.0  19.376018  19.186057  18.999784  18.817094   86.0  1360  1330  1300  12D1   86.0  -0.01%   0.01%   0.00%   0.00%
// 86.5  19.488669  19.297603  19.110248  18.926496   86.5  137D  134C  131C  12ED   86.5   0.00%   0.00%   0.00%   0.00%
// 87.0  19.601320  19.409150  19.220712  19.035897   87.0  139A  1369  1339  1309   87.0   0.00%   0.01%   0.01%   0.00%
//                                                                  *

// turn angle and radius to steps -- fixed-point bits: 12
// ------------------------------------------------------
// 50.0  0.455111  748  -0.01%
// 50.5  0.450605  736   0.02%
// 51.0  0.446187  724   0.02% *
// 51.5  0.441855  712   0.01%
// 52.0  0.437607  700  -0.02%

// gripper -- fixed-point fraction bits: 10
// ----------------------------------------
// tooth separation
// 2.70  ACD   0.01%
// 2.80  B33  -0.01%
// 2.90  B9A   0.01% *
// 3.00  C00   0.00%
// 3.10  C66  -0.01%
// 3.20  CCD   0.01%

// gripper -- fixed-point fraction bits: 16
// ----------------------------------------
// tooth separation     grip mm to steps           steps to grip mm
// 2.70  2B333  0.00%   68.956229  44F4CB  0.00%   0.014502  3B6  -0.04%
// 2.80  2CCCD  0.00%   66.493506  427E56  0.00%   0.015039  3DA   0.04%
// 2.90  2E666  0.00%   64.200627  40335C  0.00%   0.015576  3FD   0.02% *
// 3.00  30000  0.00%   62.060606  3E0F84  0.00%   0.016113  420   0.00%
// 3.10  3199A  0.00%   60.058651  3C0F04  0.00%   0.016650  443  -0.02%
// 3.20  33333  0.00%   58.181818  3A2E8C  0.00%   0.017188  466  -0.04%

#define SPARKI_MOTOR_DRIVE_MM_TO_STEPS_FP             0x1991  // must be consistent with baseline wheel diameter
#define SPARKI_MOTOR_DRIVE_MM_TO_STEPS_FP_FRACT_BITS  8
#define SPARKI_MOTOR_DRIVE_MM_MAX_BITS                16

#define SPARKI_MOTOR_SPIN_DEG_TO_STEPS_FP             0x1313  // must be consistent with baseline wheel diameter and separation
#define SPARKI_MOTOR_SPIN_DEG_TO_STEPS_FP_FRACT_BITS  8
#define SPARKI_MOTOR_SPIN_DEG_MAX_BITS                16

#define SPARKI_TURN_RADIUSMM_ANGLEDEG_TO_STEPS_FP             0x724  // must be consistent with baseline wheel diameter
#define SPARKI_TURN_RADIUSMM_ANGLEDEG_TO_STEPS_FP_FRACT_BITS  12
#define SPARKI_TURN_RADIUS_DEFAULT_MM                         75
#define SPARKI_WHEEL_TURN_WITH_NEG_RAD_ALTERNATE              false

#define SPARKI_MOTOR_GRIP_MM_TO_STEPS_FP             0x40335C  // must be consistent with gripper tooth separation
#define SPARKI_MOTOR_GRIP_MM_TO_STEPS_FP_FRACT_BITS  16
#define SPARKI_MOTOR_GRIP_MM_MAX_BITS                8

#define SPARKI_MOTOR_GRIP_STEPS_TO_MM_FP             0x03FD
#define SPARKI_MOTOR_GRIP_STEPS_TO_MM_FP_FRACT_BITS  16
#define SPARKI_MOTOR_GRIP_STEPS_MAX_BITS             24

// ----------------------------------------------------------------------------

// defines for left and right motors
#define MOTOR_LEFT    0  // deprecated, to be removed?
#define MOTOR_RIGHT   1  // deprecated, to be removed?
#define MOTOR_GRIPPER 2  // deprecated, to be removed?

// defines for direction
#define DIR_CCW -1       // deprecated, to be removed?
#define DIR_CW   1       // deprecated, to be removed?

// ----------------------------------------------------------------------------

//includes for the LCD 

#define swap(a, b) { uint8_t t = a; a = b; b = t; }

#define BLACK 0
#define WHITE 1

#define LCDWIDTH 128
#define LCDHEIGHT 64

#define CMD_DISPLAY_OFF   0xAE
#define CMD_DISPLAY_ON    0xAF

#define CMD_SET_DISP_START_LINE  0x60
#define CMD_SET_PAGE  0xB0

#define CMD_SET_COLUMN_UPPER  0x10
#define CMD_SET_COLUMN_LOWER  0x00

#define CMD_SET_ADC_NORMAL  0xA0
#define CMD_SET_ADC_REVERSE 0xA1

#define CMD_SET_DISP_NORMAL 0xA6
#define CMD_SET_DISP_REVERSE 0xA7

#define CMD_SET_ALLPTS_NORMAL 0xA4
#define CMD_SET_ALLPTS_ON  0xA5
#define CMD_SET_BIAS_9 0xA2 
#define CMD_SET_BIAS_7 0xA3

#define CMD_RMW  0xE0
#define CMD_RMW_CLEAR 0xEE
#define CMD_INTERNAL_RESET  0xE2
#define CMD_SET_COM_NORMAL  0xC0
#define CMD_SET_COM_REVERSE  0xC8
#define CMD_SET_POWER_CONTROL  0x28
#define CMD_SET_RESISTOR_RATIO  0x20
#define CMD_SET_VOLUME_FIRST  0x81
#define  CMD_SET_VOLUME_SECOND  0
#define CMD_SET_STATIC_OFF  0xAC
#define  CMD_SET_STATIC_ON  0xAD
#define CMD_SET_STATIC_REG  0x0
#define CMD_SET_BOOSTER_FIRST  0xF8
#define CMD_SET_BOOSTER_234  0
#define  CMD_SET_BOOSTER_5  1
#define  CMD_SET_BOOSTER_6  3
#define CMD_NOP  0xE3
#define CMD_TEST  0xF0

#define LCD_A0 4
#define LCD_RST 12
#define LCD_CS 17

// SPI definitions
#define SPI_MODE_MASK 0x0C  // CPOL = bit 3, CPHA = bit 2 on SPCR
#define SPI_CLOCK_MASK 0x03  // SPR1 = bit 1, SPR0 = bit 0 on SPCR
#define SPI_2XCLOCK_MASK 0x01  // SPI2X = bit 0 on SPSR
#define SPI_CLOCK_DIV2 0x04

// Magnetometer definitions
#define Measurement_Continuous 0x00  
#define DataRegisterBegin 0x03
#define HMC5883L_Address 0x1E
#define DataRegisterBegin 0x03
#define RawMagDataLength 6
#define ConfigurationRegisterB 0x01
#define ModeRegister 0x02
#define M_SCALE 0.92

// Accelerometer definitions
#define MMA8452_ADDRESS 0x1D  // SA0 is high, 0x1C if low
#define ACCEL_SCALE 2  // Sets full-scale range to +/-2, 4, or 8g. Used to calc real g values.
#define ACCEL_DATARATE 0 //Set the output data rate: 0=800Hz, 1=400, 2=200, 3=100, 4=50, 5=12.5, 6=6.25, 7=1.56

class SparkiClass : public Print {

public:
  SparkiClass();
  int ping_single();
  int ping();
  void begin();
  void beep();
  void beep(int);
  void beep(int, int);
  void noBeep();
  void RGB(uint8_t,uint8_t,uint8_t);

  void setMux(uint8_t, uint8_t, uint8_t);
  
// Light level sensors
  int lightRight();
  int lightCenter();
  int lightLeft();

// Infrared reflectance sensors
  int edgeRight();
  int lineRight();
  int lineCenter();
  int lineLeft();  
  int edgeLeft();
  
  int readSensorIR(int);
  int readBlindSensorIR(int,int,int);
  int diffIR(int,int,int);
  

// Infrared Remote sensor
  int readIR();

// Infrared Remote sensor
  void sendIR(uint8_t);

// Wire Functions
  void WireWrite(int, int);
  uint8_t* WireRead(int, int);

// Compass
  float compass();

// Magnetometer
  float readMag();
  float xAxisMag;
  float zAxisMag;
  float yAxisMag;  
  float magX();
  float magY();
  float magZ();

// Accelerometer
  int accelGood;
  float xAxisAccel;
  float zAxisAccel;
  float yAxisAccel;  
  float accelX();
  float accelY();
  float accelZ();
  
  void readAccelData();
  int initAccelerometer();
 
// I2C functions 
  void readi2cRegisters(uint8_t, int, uint8_t *, uint8_t);
  uint8_t readi2cRegister(uint8_t, uint8_t);
  void readi2cRegister(unsigned char, unsigned char, uint8_t);

// Servo Functions
  void startServoTimer();
  void servo(int);
  int8_t servo_deg_offset;

    // BEGIN: Motor Control (wheels and gripper)

    boolean distanceOfZeroMeansInfinity;
    uint8_t wheelSpeedPercentDefault;
    uint8_t gripperSpeedPercentDefault;

    void setWheelDiameter (
      uint16_t wheelDiameterUm
    );
    void setWheelSeparation (
      uint16_t wheelSeparationUm
    );

    void primeMotors ();

    void enableActiveHoldForWheels (
      boolean enableActiveHold
    );
    void enableActiveHoldForGripper (
      boolean enableActiveHold
    );
    boolean activeHoldIsEnabledForWheels ();
    boolean activeHoldIsEnabledForGripper ();

    void enableBacklashCompForWheels (
      boolean enableBacklashComp
    );
      void enableBacklashCompForGripper (
      boolean enableBacklashComp
    );
    boolean backlashCompIsEnabledForWheels ();
    boolean backlashCompIsEnabledForGripper ();

    void driveForward (
      int16_t distanceToDriveMm = 0,
      boolean waitUntilDone = true,
      int8_t  speedPercent = -1
    );

    void driveBackward (
      int16_t distanceToDriveMm = 0,
      boolean waitUntilDone = true,
      int8_t  speedPercent = -1
    );

    void spinLeft (
      int16_t angleToSpinDeg = 0,
      boolean waitUntilDone = true,
      int8_t  speedPercent = -1
    );
    void spinRight (
      int16_t angleToSpinDeg = 0,
      boolean waitUntilDone = true,
      int8_t  speedPercent = -1
    );

    void turnLeft (
      int16_t angleToTurnDeg = 90,
      int16_t radiusOfTurnMm = SPARKI_TURN_RADIUS_DEFAULT_MM,
      boolean waitUntilDone = true,
      int8_t  speedPercent = -1
    );
    void turnRight (
      int16_t angleToTurnDeg = 90,
      int16_t radiusOfTurnMm = SPARKI_TURN_RADIUS_DEFAULT_MM,
      boolean waitUntilDone = true,
      int8_t  speedPercent = -1
    );

    boolean wheelsAreRunning ();
    void stopWheels ();

    boolean wheelsAreDone ();
    void restartWheels (
      boolean waitUntilDone
    );

    void openGripper (
      int8_t  amountToOpenMm = SPARKI_MOTOR_GRIPPER_OPEN_MAX_MM,
      boolean waitUntilDone = true,
      int8_t  speedPercent = -1
    );

    void closeGripper (
      int8_t  amountToCloseMm = SPARKI_MOTOR_GRIPPER_OPEN_MAX_MM,
      boolean waitUntilDone = true,
      int8_t  speedPercent = -1
    );

    void setGripperSpacing (
      uint8_t spacingMm,
      boolean waitUntilDone = true,
      int8_t  speedPercent = -1
    );

    boolean gripperIsRunning ();
    void stopGripper ();

    boolean gripperIsDone ();
    void restartGripper (
      boolean waitUntilDone
    );

    // END: Motor Control (wheels and gripper)

    // BEGIN: Motor Control Legacy Interface (deprecated)

    void moveForward(float);
    void moveForward();
    void moveBackward(float);
    void moveBackward();
    void moveLeft(float);
    void moveLeft();
    void moveRight(float);
    void moveRight();
    void moveStop();
    void gripperOpen();
    void gripperClose();
    void gripperStop();

    // individual motor control (non-blocking)
    // speed range is percent 0-100
    void motorRotate( int motor, int direction,  int speed);
    void motorStop(int motor);

    // combined motor control using step count
    // this function blocks by default but returns after starting motors if wait = false
    void motorsRotateSteps( int leftDir, int rightDir,  int speed, uint32_t steps, bool wait= true);
   
    // returns true if one or both motors a still stepping
    bool areMotorsRunning();
     
    // END: Motor Control Legacy Interface

  void onIR();
  void offIR();

// Display Functions
  void st7565_init(void);
  void beginDisplay();
  void st7565_command(uint8_t c);
  void st7565_data(uint8_t c);
  void st7565_set_brightness(uint8_t val);
  void clear_display(void);
  
  void clearLCD();
  void updateLCD();
  
  void moveUpLine();
  
  uint8_t readPixel(uint8_t x, uint8_t y);
  
  void textWrite(const char* buffer, uint16_t len=0);

  /* Play nice with Arduino's Print class */
  virtual size_t write(uint8_t b) {
    textWrite((const char *)&b, 1);
    return 1;
  }
  virtual size_t write(const uint8_t *buffer, size_t size) {
    textWrite((const char *)buffer, size);
    return size;
  }
  
  void drawPixel(uint8_t x, uint8_t y);  
  void drawCircleFilled(uint8_t x0, uint8_t y0, uint8_t r);
  void drawCircle(uint8_t x0, uint8_t y0, uint8_t r);
  void drawRect(uint8_t x, uint8_t y, uint8_t w, uint8_t h);
  void drawRectFilled(uint8_t x, uint8_t y, uint8_t w, uint8_t h);
  void drawLine(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1);
  void drawChar(uint8_t x, uint8_t line, char c);
  void drawString(uint8_t x, uint8_t line, char *c);
  void drawString_P(uint8_t x, uint8_t line, const char *c);
  void drawBitmap(uint8_t x, uint8_t y, 
		  const uint8_t *bitmap, uint8_t w, uint8_t h);

  void updateShiftReg ();
  // updateShiftReg: function to update shift register that extends number of
  // processor outputs. This function is called automatically through timer
  // interrupts. Under normal circumstances, it should not be called by user
  // or library code. If it does need to be called; it should be bracketed by
  // cli() / sei().

private:

    // BEGIN: Motor Control, Private Assets

    volatile byte     motorControlWord             [SPARKI_MOTOR_COUNT];
    volatile byte     motorStatusWord              [SPARKI_MOTOR_COUNT];
             uint32_t motorRemainingSteps          [SPARKI_MOTOR_COUNT];
             uint16_t motorUpdateIntervalIntEvents [SPARKI_MOTOR_COUNT];
             uint16_t motorTimeToUpdateIntEvents   [SPARKI_MOTOR_COUNT];
             uint16_t wheelDiameterUmEff;
             uint16_t wheelSeparationUmEff;
             uint16_t driveMmToStepsEffFp;
             uint16_t spinDegToStepsEffFp;
             uint16_t turnRadMmAngleDegToStepsEffFp;
             int8_t   gripperSpacingMm;

    void initMotorControlWord (
      uint8_t motorIdMask, 
      byte    initValue
    );
    void initMotorStatusWord (
      uint8_t motorIdMask, 
      byte    initValue
    );
    void initMotorWord (
      volatile byte *  motorWord,
               uint8_t motorIdMask, 
               byte    initValue
    );

    void modifyMotorControlWordBit (
      byte    motorIdMask, 
      byte    bitSelectMask, 
      boolean bitValue,
      boolean motorIdIsIndex = false
    );
    void modifyMotorStatusWordBit (
      byte    motorIdMask, 
      byte    bitSelectMask, 
      boolean bitValue,
      boolean motorIdIsIndex = false
    );
    void modifyMotorWordBit (
      volatile byte *  motorWord,
               byte    motorIdMask, 
               byte    bitSelectMask, 
               boolean bitValue,
               boolean motorIdIsIndex = false
    );

    void updateDriveMmToStepsFactor ();
    void updateSpinDegToStepsFactor ();
    void updateTurnRadMmAngleDegToStepsFactor ();

    uint8_t motorSpeedPercentToIntEvents (
      uint8_t  speedPercent
    );
    uint8_t motorIntEventsToSpeedPercent (
      uint16_t updateIntervalIntEvents
    );

    uint32_t driveDistanceToMotorSteps (
      uint16_t distanceToDriveMm
    );

    uint32_t spinAngleToMotorSteps (
      uint16_t angleToSpinDeg
    );

    void enableActiveMotorHold (
      byte    motorIdMask, 
      boolean enableActiveHold
    );
    boolean activeMotorHoldIsEnabledForAny (
      byte motorIdMask
    );

    void enableMotorBacklashComp (
      byte    motorIdMask, 
      boolean enableBacklashComp
    );
    boolean motorBacklashCompIsEnabledForAny (
      byte motorIdMask
    );
    void performMotorBacklashCompensation (
      byte motorIdMask
    );

    void setupMotorForSteps (
      byte     motorIdMask,
      uint32_t stepCount,
      boolean  stepClockwise,
      uint8_t  speedPercent = -1,
      boolean  stopIfRunning = true
    );

    void startMotorRotation (
      byte    motorIdMask,
      boolean waitUntilDone = true
    );
    void stopMotorRotation (
      byte motorIdMask
    );

    void rotateMotorsBySteps (
      byte     motorIdMask,
      uint32_t stepCount,
      int8_t   turnDirection,
      boolean  waitUntilDone = true,
      uint8_t  speedPercent = -1
    );

    void drive (
      int16_t distanceToDriveMm,
      boolean directionIsFoward = true,
      boolean waitUntilDone = true,
      int8_t  speedPercent = -1
    );

    void spin (
      int16_t angleToSpinDeg,
      boolean directionIsRight = true,
      boolean waitUntilDone = true,
      int8_t  speedPercent = -1
    );

    void turn (
      int16_t angleToTurnDeg,
      boolean directionIsRight= true,
      int16_t radiusOfTurnMm = SPARKI_TURN_RADIUS_DEFAULT_MM,
      boolean waitUntilDone = true,
      int8_t  speedPercent = -1
    );

    uint8_t countOfMotorsRunning (
      byte motorIdMask
    );
    boolean anyMotorIsRunning (
      byte motorIdMask
    );
    boolean allMotorsAreRunning (
      byte motorIdMask
    );

    // END: Motor Control, Private Assets

// Display Functions
  int8_t sid, sclk, a0, rst, cs;
  void startSPI();
  void spiwrite(uint8_t c);
  void my_setpixel(uint8_t x, uint8_t y, uint8_t color);
};

extern SparkiClass sparki;

#endif

