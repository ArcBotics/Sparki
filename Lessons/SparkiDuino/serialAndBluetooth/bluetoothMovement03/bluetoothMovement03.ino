#include <Sparki.h> // include the sparki library

String inputString; //make an empty String called inputString
boolean returnFlag; //flag to check for carriage return
boolean oKSent; //flag to check for OK communication
String commArray [10]; //array to store communication
int arrayCounter = 0; //integer to count through commArray

void setup()
{
  Serial1.begin(9600);
}

void loop()
{
  instructions();
  if(oKSent){
    readComm();
  }
}

void readComm()
{ 
  while (Serial1.available())
  {
    //added this to clear array
    if(returnFlag)
    {
      for(int i = 0; i <= 9; i++) //for loop to go through commArray
      {
        commArray[i] = "";
      }
      returnFlag = false; //reset flag
    }
    //end clearing array code
    
    int inByte = Serial1.read();
    if ((char)inByte == '\n')
    {
      returnFlag = true;
      arrayCounter = 0;
    }
    else
    {
      //here is where the code differs a lot from the previous code
      if(inByte == 32) //if it's a blank space
      {
        arrayCounter ++; //increment array counter to store in new array space
      }
      else
      {
        //add the character to the arrayCounter space in commArray
        commArray[arrayCounter] += (char)inByte; 
      }
    }
  }
  //debugging/checking to make sure it works
  for(int i = 0; i <=9; i++)
  {
    Serial1.println(commArray[i]);
  }
}
void instructions()
{
  //Start code for instructions
  while (oKSent == false)
  {
    //start code for reading in communication
    while (Serial1.available())
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
    //end code for reading in communication

    //start code to check for OK communication
    if ((inputString == "OK" || inputString == "ok" || inputString == "Ok") && returnFlag == true)
    {
      Serial1.println("Have fun sending Sparki movement commands!");
      delay(500);
      oKSent = true; //exit the OK while loop
    }
    //end code to check for OK communication
    else //send instructions once a second
    {
      Serial1.println("Sparki expects to receive communication that are");
      Serial1.println("movement commands (Forward, Right, Left, Stop)");
      Serial1.println("separated by spaces.");
      Serial1.println("Do not send Sparki more than ten commands at a time.");
      Serial1.println("The line below is an example of valid communication.");
      Serial1.println("Forward Right Forward Left Forward");
      Serial1.println("Type OK and press Send or Return to continue.");
      Serial1.println();//empty line to separate messages
      delay(2000);
    }
  }
  //end code for instructions
}

