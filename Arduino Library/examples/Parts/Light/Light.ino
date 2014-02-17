/*******************************************
 Basic Light Sensor test
 
 Sparki has 3 light sensors in the front,
 one each on the left, middle and right.
 This program shows how to read the sensors
 and display the information on the LCD.
 
 http://arcbotics.com/light-sensor/
********************************************/
#include <Sparki.h> // include the sparki library

void setup() 
{
}

void loop() {
  sparki.clearLCD(); // wipe the screen
  
  int left   = sparki.lightLeft();   // measure the left light sensor
  int center = sparki.lightCenter(); // measure the center light sensor
  int right  = sparki.lightRight();  // measure the right light sensor

  // write the measurements to the screen
  sparki.print("Left   "); 
  sparki.println(left);

  sparki.print("Center ");
  sparki.println(center);
  
  sparki.print("Right  ");
  sparki.println(right);
  
  sparki.updateLCD(); // display all of the information written to the screen
  delay(100);
}
