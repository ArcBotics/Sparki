#include <Sparki.h> // include the sparki library

void setup()
{
    sparki.motorRotate(MOTOR_LEFT, DIR_CCW, 100);
    sparki.motorRotate(MOTOR_RIGHT, DIR_CW, 25);

    delay(22000); //give enough time to make the drawing

    sparki.motorStop(MOTOR_LEFT);
    sparki.motorStop(MOTOR_RIGHT);
}

void loop()
{
}
