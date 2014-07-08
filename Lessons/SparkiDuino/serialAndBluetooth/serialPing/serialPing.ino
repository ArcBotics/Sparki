/*******************************************
 Printing data from the ultrasonic sensor

 http://arcbotics.com/lessons/using-usb-serial-and-bluetooth-communications/
 
********************************************/
#include <Sparki.h>  // include the sparki library


void setup()
{
}

void loop()
{
  Serial.print("Distance = ");
  Serial.print(sparki.ping());
  Serial.println(" cm");
    
  delay(300);
}

