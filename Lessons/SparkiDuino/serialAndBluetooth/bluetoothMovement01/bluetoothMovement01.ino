#include <Sparki.h>  // include the sparki library

String inputString; //make an empty String called inputString
boolean returnFlag; //flag to check for carriage return

void setup()
{
  Serial1.begin(9600);
}

void loop()
{
  if (Serial1.available()) 
  {
  if (returnFlag)
    {
      inputString = "";
      returnFlag = false;
    }
    int inByte = Serial1.read();
  if ((char)inByte == '\n')
    {
      returnFlag = true;
    }
  else
    {
      inputString += (char)inByte; //add the character from bluetooth to string
    }
  }
  Serial1.println(inputString);  //this code is for debugging and checking comm.
  
  //here's the code I'm adding this time around
  if (inputString == "Forward" || inputString == "forward" || inputString == "Go")
  {
    sparki.moveForward();
  }
  else if (inputString == "Left" || inputString == "left")
  {
    sparki.moveLeft();
  }
  else if (inputString == "Right" || inputString == "right")
  {
    sparki.moveRight();
  }
  else if (inputString == "Stop" || inputString == "stop")
  {
    sparki.moveStop();
  }
}
