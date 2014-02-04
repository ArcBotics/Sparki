#include <Sparki.h> // include the sparki library
 
void setup()
{
 
}
 
void loop()
{
    sparki.RGB(GREEN); // turn the light green
    int cm = sparki.ping(); // measures the distance with Sparki's eyes
        
    if(cm != -1) // make sure its not too close or too far
    { 
        if(cm < 20) // if the distance measured is less than 10 centimeters
        {
            sparki.RGB(RED); // turn the light red
            sparki.beep(); // beep!
            sparki.moveBackward(10); // back up 10 centimeters
            sparki.moveRight(30); // rotate right 30 degrees
            delay(1000);
        }
    }
    delay(100); // wait 0.1 seconds (100 milliseconds)
}
