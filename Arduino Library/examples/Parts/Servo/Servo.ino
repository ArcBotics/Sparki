/*******************************************
 Basic Servo test
 
 Move the servo on Sparki's head. The servo
 can rotate from -90 to 90 degrees. -90 is 
 facing left, 0 is facing forward,  and 90
 is facing right.
 
 http://arcbotics.com/products/sparki/parts/servo/
********************************************/
#include <Sparki.h> // include the sparki library

void setup()
{ 
} 
 
void loop() 
{ 
  sparki.servo(SERVO_LEFT); // rotate the servo to is -90 degree postion (left)
  delay(1000); 

  sparki.servo(SERVO_CENTER); // rotate the servo to is 0 degree postion (forward)
  delay(1000); 
  
  sparki.servo(SERVO_RIGHT); // rotate the servo to is 90 degree postion (right)
  delay(1000); 

  sparki.servo(-90); // rotate the servo to is -90 degree postion (left)
  delay(1000); 

  sparki.servo(0); // rotate the servo to is 0 degree postion (forward)
  delay(1000); 
  
  sparki.servo(90); // rotate the servo to is 90 degree postion (right)
  delay(1000);  
} 
