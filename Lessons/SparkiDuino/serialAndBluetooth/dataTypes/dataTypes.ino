/*******************************************
 Printing different data types

 http://arcbotics.com/lessons/using-usb-serial-and-bluetooth-communications/
 
********************************************/
#include <Sparki.h>  // include the sparki library

int counter = 0;  // integer variable.
float x = 0.0;    // floating point variable.

void setup()
{
}

void loop()
{
  Serial.print("Counter = ");
  Serial.println(counter); // prints an integer value
  
  Serial.print("x = ");
  Serial.println(x); // prints a floating point value
  
  Serial.println(); // prints an empty line
  
  counter++; // increment the counter variable
  x = x + 0.2; // increment the x variable in 0.2 steps
  
  delay(500);
}

