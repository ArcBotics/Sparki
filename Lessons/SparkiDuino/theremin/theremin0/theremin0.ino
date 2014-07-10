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
