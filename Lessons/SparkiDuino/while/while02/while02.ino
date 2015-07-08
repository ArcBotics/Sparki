#include <Sparki.h> // include the sparki library

int loopCount = 0; // variable to count loops

void setup()
{
}

void loop()
{

 sparki.moveForward(10);

    while( loopCount == 10)
    {
         sparki.moveRight();
    }

 loopCount = loopCount + 1; // count the number of loops
}
