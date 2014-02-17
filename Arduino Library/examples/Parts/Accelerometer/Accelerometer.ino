/*******************************************
 Basic Accelerometer Sensor test
 
 Sparki has a 3-Axis accelerometer. It is 
 used to detect the acceleration Sparki is 
 experiencing. Usually, this mostly means the
 force of gravity. It can do this in all 
 3 XYZ axis, which are left/right (X), 
 forward/backwards (Y), and up/down (Z). This 
 is the same part that smartphones use to tell
 how you’re tilting them.
 
 This program shows how to read the sensor
 and display the information on the LCD.
 
 http://arcbotics.com/products/sparki/parts/accelerometer/
********************************************/
#include <Sparki.h> // include the sparki library

void setup() 
{
}

void loop() {
  sparki.clearLCD(); // wipe the screen
  
  float x  = sparki.accelX();   // measure the accelerometer x-axis
  float y  = sparki.accelY();   // measure the accelerometer y-axis
  float z  = sparki.accelZ();   // measure the accelerometer z-axis

  // write the measurements to the screen
  sparki.print("Accel X: "); 
  sparki.println(x);

  sparki.print("Accel Y: "); 
  sparki.println(y);
  
  sparki.print("Accel Z: "); 
  sparki.println(z);
  
  sparki.updateLCD(); // display all of the information written to the screen
  delay(100);
}
