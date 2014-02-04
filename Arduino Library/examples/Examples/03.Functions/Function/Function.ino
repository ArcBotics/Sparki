/*******************************************
 Function
 
 This is the basic code for a function.
 With this function, two numbers are added.
 Sparki displays the result on its LCD.
********************************************/
#include <Sparki.h> // include the sparki library

void setup() // code inside these brackets runs first, and only once
{
}

void loop() // code inside these brackets runs over and over forever
{
  sparki.clearLCD();
  
  int sum;
  sum = addition(3, 5);
  sparki.println(sum);
  
  sparki.updateLCD();
}

int addition( int a, int b )
{
    int r;
    r = a + b;
    return r;
}
