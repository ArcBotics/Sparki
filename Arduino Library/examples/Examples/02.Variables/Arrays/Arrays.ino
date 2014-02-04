#include <Sparki.h> // include the sparki library
 
void setup() // code inside these brackets runs first, and only once
{
 
}
 
void loop() // code inside these brackets runs over and over forever
{
   sparki.clearLCD(); // clear the LCD
   
   char charArray[6] = {'h','e','l','l','o','!'};
   
   sparki.println( charArray[0] ); // print the first item in array
   
   //sparki.println( charArray ); // try to print the array
   
   charArray[5] = '\0'; // put an end in the array
   
   sparki.println( charArray ); // print the array with an end
   
   sparki.updateLCD(); // draw new info on LCD
}
