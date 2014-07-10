#include <Sparki.h> // include the robot library

int counter = 1;

void setup()
{
}

void loop()
{
    if (counter > 5) //We have to put a limit: why?
        counter = 1;
    sparki.beep(counter*440); // Increasing "A" notes!
    counter++;
    delay(1000); // wait a second (1000 milliseconds)
}
