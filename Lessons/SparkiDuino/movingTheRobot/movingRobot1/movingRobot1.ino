#include <Sparki.h> // include the sparki library

void setup()
{
}

void loop()
{
    sparki.motorRotate(MOTOR_LEFT, DIR_CCW, 50);
    sparki.motorRotate(MOTOR_RIGHT, DIR_CW, 100);

    delay(4000); // wait a second (1000 milliseconds)

    sparki.motorRotate(MOTOR_LEFT, DIR_CCW, 100);
    sparki.motorRotate(MOTOR_RIGHT, DIR_CW, 50);

    delay(4000);

    sparki.motorStop(MOTOR_LEFT);
    sparki.motorStop(MOTOR_RIGHT);
}
