#include <Sparki.h> // include the sparki library

char foundLetter; // variable to hold the letter we are looking for

void setup() // code inside these brackets runs first, and only once
{

}

void loop() // code inside these brackets runs over and over forever
{
   sparki.clearLCD(); // clear the LCD

   char charArray[6] = {'h','e','l','l','o','!'};

   //sparki.println( charArray[0] ); // print the first item in array

   //sparki.println( charArray ); // try to print the array

   charArray[7] = '\0'; // put an end in the array

   sparki.println( charArray ); // print the array with an end
   
   for(int i = 0; i <= 7; i++) // this line looks through all seven values of the array
   {
     if(charArray[i] == 'l' || charArray[i] == 'L') // asks the question- is this value one of the letters we are looking for?
     {
       foundLetter = charArray[i]; // store the letter
       sparki.println(i); // print the index number of the letter we found
       i = 8; //skip to the end of for loop
     }
   }
   
   sparki.println(foundLetter);

   sparki.updateLCD(); // draw new info on LCD
}




