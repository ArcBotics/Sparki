/*******************************************
 Theremin
 
 A theremin is a musical instrument that
 is played by holding your hand in front
 of a sensor. The farther out from the 
 sensor, the higher the tone.
 
 This program does this, using Sparki's
 ultrasonic distance sensor to measure the
 hand distance, and the buzzer to play the
 tone.
********************************************/
#include <Sparki.h> // include the sparki library

void setup() 
{
}

void loop() {
    int note = sparki.ping(); // measure the distance in front of the sensor
    
    if ( note != -1 ) // meke sure there is no error
    {
      sparki.beep(note*10); // plays a note proportional to the distance
    }
    
    if ( note > 100 ) // if the distance is too far, turn off the tone
    {
      sparki.noBeep();
    }
    
    delay(10);
}
