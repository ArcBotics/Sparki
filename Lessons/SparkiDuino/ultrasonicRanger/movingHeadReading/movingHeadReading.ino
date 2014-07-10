#include <Sparki.h>  // include the sparki library

int left = 0, center = 0, right = 0;
int interval = 800;

void setup()
{
}

void updateSensorReadings()
{
  // refresh all the display data at once:
  sparki.clearLCD();

  sparki.print("Left = ");
  sparki.println(left);

  sparki.print("Center = ");
  sparki.println(center);

  sparki.print("Right = ");
  sparki.println(right);

  sparki.updateLCD();
}

void loop()
{
  sparki.servo(SERVO_LEFT); // rotate the servo to is -90 degree postion (left)
  delay(interval); 
  left = sparki.ping();
  updateSensorReadings();

  sparki.servo(SERVO_CENTER); // rotate the servo to is 0 degree postion (forward)
  delay(interval); 
  center = sparki.ping();
  updateSensorReadings();

  sparki.servo(SERVO_RIGHT); // rotate the servo to is 90 degree postion (right)
  delay(interval); 
  right = sparki.ping();
  updateSensorReadings(); 
}
