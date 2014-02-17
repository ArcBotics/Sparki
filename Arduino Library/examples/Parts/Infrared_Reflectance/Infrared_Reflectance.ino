/*******************************************
  IR Sensors
  
  Sparki has sensors underneath that 
  sense objects in the way, and differences
  in the surface color. This example shows
  the numbers from those on the LCD. Try
  seeing what surfaces affect the numbers.
  
  http://arcbotics.com/products/sparki/parts/infrared-reflectance-sensor/
********************************************/ 
#include <Sparki.h> // include the sparki library

void setup()
{       
}

void loop() {
  sparki.clearLCD(); // wipe the screen
  
  int edgeLeft   = sparki.edgeLeft();   // measure the left edge IR sensor
  int lineLeft   = sparki.lineLeft();   // measure the left IR sensor
  int lineCenter = sparki.lineCenter(); // measure the center IR sensor
  int lineRight  = sparki.lineRight();  // measure the right IR sensor
  int edgeRight  = sparki.edgeRight();  // measure the right edge IR sensor

  // write the measurements to the screen
  sparki.print("Edge Left:   ");
  sparki.println(edgeLeft);
  
  sparki.println();

  sparki.print("Line Left:   ");
  sparki.println(lineLeft);
  
  sparki.print("Line Center: ");
  sparki.println(lineCenter);
  
  sparki.print("Line Right:  ");
  sparki.println(lineRight);
  
  sparki.println();
  
  sparki.print("Edge Right:  ");
  sparki.println(edgeRight);
  
  sparki.updateLCD(); // display all of the information written to the screen
  delay(100);
}
