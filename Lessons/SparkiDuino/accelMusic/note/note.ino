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
    
  sparki.beep(abs(y)*100);
    
  delay(abs(x)*50 + 100); // note duration: the 100 number is there to prevent zero duration notes.
}
