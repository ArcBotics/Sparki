#include <Sparki.h>  // include the sparki library
 
void setup()
{
}
 
void loop()
{
  for (int i = 0; i < 3; i=i+1)
  {
    sparki.beep(); // Sparki beeps!
    delay(300); // wait 0.3 seconds
  }
 
  delay(2000); // wait two seconds (2000 milliseconds)
}
