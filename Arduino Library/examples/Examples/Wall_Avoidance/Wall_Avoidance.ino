#include <Sparki.h> // include the sparki library
 
void setup() {
  sparki.servo(SERVO_CENTER); // Center the Servo
}

void loop()
{
    sparki.RGB(RGB_GREEN); // turn the light green
    sparki.moveForward(); // move Sparki forward
    int cm = sparki.ping(); // measures the distance with Sparki's eyes
        
    if(cm != -1) // make sure its not too close or too far
    { 
        if(cm < 20) // if the distance measured is less than 10 centimeters
        {
            sparki.RGB(RGB_RED); // turn the light red
            sparki.beep(); // beep!
            sparki.moveBackward(10); // back up 10 centimeters
            sparki.moveRight(30); // rotate right 30 degrees
        }
    }
    delay(100); // wait 0.1 seconds (100 milliseconds)
}
