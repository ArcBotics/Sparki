#include <Sparki.h> // include the sparki library

String inputString; //make an empty String called inputString
boolean returnFlag; //flag to check for carriage return
boolean oKSent; //flag to check for OK communication
char commArray [10]; //array to store communication
int arrayCounter = 0; //integer to count through commArray
int boredCounter = 0;

void setup()
{
  Serial1.begin(9600);
}

void loop()
{
  //instructions();
  //if(oKSent){
    readComm();
    makeMove();
  //}
  if(boredCounter > 700000000)
  {
   Serial1.println("Ummmm.... ");
   delay(3000);
   Serial1.println("Hello?"); 
   delay(1000);
   Serial1.println("Are you still there?");
  }
}

void makeMove(){
  for(int i = 0; i <= 9; i++) 
  {
    if(commArray[i] == 'f' || commArray[i] == 'F')
    {
      sparki.moveForward();
      delay(1000); 
      sparki.moveStop();
    }
    else if (commArray[i] == 'r' || commArray[i] == 'R')
    {
      sparki.moveRight(90);
    }
    else if (commArray[i] == 'l' || commArray[i] == 'L')
    {
      sparki.moveLeft(90);
    }
    else if (commArray[i] == 's' || commArray[i] == 'S')
    {
      sparki.moveStop();
      delay(1000);
    }
    else if (commArray[i] != 0) //in case it's a character sparki doesn't understand
    {
      Serial1.print("I'm sorry, I didn't understand the command- ");
      Serial1.println(commArray[i]); //send the character back
    }
  } 
  memset(commArray, 0, sizeof(commArray)); //clear out commArray
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
        //add the character to the arrayCounter space in commArray
        // commArray[arrayCounter] += (char)inByte; //this line changed to
        commArray[arrayCounter] = (char)inByte;
      }
    }
    boredCounter = 0; //reset bored counter
  }
  //debugging/checking to make sure it works
  //for(int i = 0; i <=9; i++)
  //{
  //  Serial1.println(commArray[i]);
  //}
  boredCounter ++; //make bored counter count up
}
/*
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
      Serial1.println("characters for movement commands separated by spaces.");    
      Serial1.println("(F for forward, R for Right, L for Left, S for Stop)");
      Serial1.println("The line below is an example of valid communication.");
      Serial1.println("F R F L F S");
      Serial1.println("Type OK and press Send or Return to continue.");
      Serial1.println();//empty line to separate messages
      delay(2000);
    }
  }
  //end code for instructions
  
}
*/
