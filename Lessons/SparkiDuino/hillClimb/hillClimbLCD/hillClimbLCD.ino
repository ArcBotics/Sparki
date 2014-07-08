#include <Sparki.h> // include the sparki library
 
void setup()
{
 
}
 
void loop()
{
    float x = sparki.accelX();
    float y = sparki.accelY();
 
    // is Sparki already facing downhill or uphill?
    if ( (x < 1) && (x > -1) )
    {
      if ( y < 0 ) // if sparki is facing uphill
      {
        sparki.moveForward();
      }
      if ( y > 0 ) // if sparki is facing downhill
      {
        sparki.moveBackward();
      }      
    }
    else{ // if Sparki is not facing uphill or downhill, have Sparki turn until it is
      if ( x > 1 ) // if tilted right
      {
        sparki.moveLeft();
      }
      if ( x < -1 ) // if tilted left
      {
        sparki.moveRight();
      }   
    }
    
    sparki.clearLCD(); // wipe the screen
    
    // write the measurements to the screen
    sparki.print("Accel X: "); 
    sparki.println(x);

    sparki.print("Accel Y: "); 
    sparki.println(y);

    sparki.updateLCD(); // display all of the information written to the screen    
    
    delay(100);
}
