#include <Sparki.h> // include the sparki library

float pingCallBack()
{
  return (float)(sparki.ping());
}

float accelXCallBack()
{
  return sparki.accelX();
}

float accelYCallBack()
{
  return sparki.accelY();
}

float accelZCallBack()
{
  return sparki.accelZ();
}

float average(float (*callBack)(void), unsigned int numberOfReadings, unsigned int readingsDelay)
{
  float result = 0;
  for (unsigned int i=0; i < numberOfReadings; i++)
  {
    result += callBack();
    if (readingsDelay) //Some sensors do not require a delay between readings, while others do.
      delay(readingsDelay);
  }
  return result / numberOfReadings;
}

void setup()
{
}

void loop()
{
    sparki.clearLCD();
       
    sparki.print("Ping: ");
    sparki.println((int)average(pingCallBack, 2, 15));
    sparki.print("Accel X: ");
    sparki.println(average(accelXCallBack, 30, 0));
    sparki.print("Accel Y: ");
    sparki.println(average(accelYCallBack, 30, 0));
    sparki.print("Accel Z: ");
    sparki.println(average(accelZCallBack, 30, 0));

    sparki.updateLCD();
}
