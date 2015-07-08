#include <Sparki.h> // include the sparki library

String inputString; //make an empty String called inputString
boolean returnFlag; //flag to check for carriage return
boolean oKSent; //flag to check for OK communication
String commArray [10]; //array to store word communication
int commNumArray[10]; //array to store number communication
int arrayCounter = 0; //integer to count through commArray
boolean commReady; //boolean to tell Sparki it is ready to receive

void setup()
{
  Serial1.begin(9600);
}

void loop()
{
  instructions();
  readComm();
}

void readComm()
{
  while (Serial1.available())
  {
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
        commArray[arrayCounter] += (char)inByte; 
      }
    }
  }
  //for loop to make Sparki take action
  for(int i = 0; i <=9; i = i+2)
  {
    if (commArray[i] == "Forward" || commArray[i] == "forward")
    {
      sparki.moveForward();
    }
    if else (commArray[i] == "Right" || commArray[i] == "right")
    {
      sparki.moveRight((int)commArray[i+1]);
    }
    if else (commArray[i] == "Left" || commArray[i] == "left")
    {
      sparki.moveLeft((int)commArray[i+1]);
    }
    if else (commArray[i] == "Stop" || commArray[i] == "stop")
    {
      sparki.moveStop((int)commArray[i+1]);
    }
    else
    {
      Serial1.println("Commands done or unrecognizable."); 
    }
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
      Serial1.println("Sparki expects to receive communication that is");
      Serial1.println("a movement command (Forward, Right, Left, Stop)");
      Serial1.println("followed by a number to control that command.");
      Serial1.println("Do not send Sparki more than five commands at a time.");
      Serial1.println("The line below is an example of valid communication.");
      Serial1.println("Forward 10 Right 90 Forward 10 Left 90 Forward 10");
      Serial1.println("Type OK and press Send or Return to continue.");
      Serial1.println();//empty line to separate messages
      delay(2000);
    }
  }
  //end code for instructions
}


