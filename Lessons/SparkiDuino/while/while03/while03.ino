#include <Sparki.h> // include the sparki library

int threshold = 700; // to check the color below the sensor
int leftLine; // to hold sensor value for left line sensor
int centerLine; // to hold sensor value for center line sensor
int rightLine; // to hold sensor value for right line sensor

void setup()
{
}

void loop()
{
  centerLine = sparki.lineCenter(); // read middle sensor
  sparki.RGB( 0, 0, 0 ); //turn off LED
  sparki.moveForward(1);

    while( centerLine < threshold ) // check middle value
  {
    sparki.moveForward(1);
    centerLine = sparki.lineCenter(); // read middle sensor
    sparki.RGB( 100, 0, 0 ); // LED lights up red

    // read sensors so Sparki can enter other while loops
    leftLine = sparki.lineLeft(); // read left sensor
    rightLine = sparki.lineRight(); // read right sensor

    // below is nested inside first while
    while( leftLine < threshold ) // check left value
    {
      leftLine = sparki.lineLeft(); // read left sensor
      sparki.RGB( 0, 100, 0 ); // LED lights up green
    }

    // below is also nested inside first while
    // BUT not inside the second one
    while( rightLine < threshold ) //check right value
    {

      sparki.RGB( 0, 0, 100 ); // LED lights up blue
    }
  }
}


