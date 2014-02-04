#include <Sparki.h> // include the sparki library
 
void setup()
{
}
 
void loop()
{
    while( sparki.ping() < 10)
    {
         sparki.moveForward(1);
    }
}
