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
    delay(100);
}
