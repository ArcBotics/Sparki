#include <Sparki.h>  // include the sparki library

int blinkCounter;

int x = 0; //integer to keep track of how much Sparki's eyes have moved 

boolean lookRight; //flag to trigger looking right
boolean lookLeft; //flag to trigger looking left
boolean lookCenterFromRight; //flag to tell Sparki to look back at the middle after looking right
boolean lookCenterFromLeft; //flag to tell Sparki to look back at the middle after looking left

void setup()
{
}

void loop()
{
  sparki.clearLCD(); // wipe the LCD clear

  //start drawing eyes code
  if(blinkCounter > 195)
    blinkEyes();
  else if(lookRight || lookCenterFromRight || lookLeft || lookCenterFromLeft)
    lookEyes();
  else
    drawEyes();
  //end drawing eyes code
    //draw eyes (these will never move)
  sparki.drawCircle(32, 25, 20); //eye 1
  sparki.drawCircle(96, 25, 20); //eye 2

  sparki.updateLCD(); // put the drawings on the screen

  delay(100); // delay for the animation

  blinkCounter ++; //make counter count up
  if(blinkCounter == 200){
    blinkCounter = 0; //reset counter
  }

  if(blinkCounter == 100)
  {
    lookRight = true; 
  }
  if(blinkCounter == 150)
  {
    lookLeft = true; 
  }


}

void drawEyes()
{


  sparki.drawCircleFilled(32, 25, 5); //pupil 1
  sparki.drawCircleFilled(96, 25, 5); //pupil 2

  sparki.drawLine(15,15, 49,15);//eyelid 1
  sparki.drawLine(79,15, 112,15);//eyelid 2 
}

void blinkEyes()
{

  sparki.drawLine(12,24, 52,24);//eyelid 1
  sparki.drawLine(76,24, 115,24);//eyelid 2 

  sparki.drawLine(12,25, 52,25);//eyelid 3
  sparki.drawLine(76,25, 115,25);//eyelid 4 
}

void lookEyes()
{

  sparki.drawCircleFilled(32 + x, 25, 5); //pupil 1 (add X to change position
  sparki.drawCircleFilled(96 + x, 25, 5); //pupil 2 (add X to change position

  sparki.drawLine(15,15, 49,15);//eyelid 1
  sparki.drawLine(79,15, 112,15);//eyelid 2 

  if(lookRight)
  {
    sparki.moveRight(3); //actually move Sparki
    x++; //move pupils the to the right
    if(x >= 15)//keep the pupils inside the eyes
    {
      lookRight = false; //done looking right
      lookCenterFromRight = true; //now look back to the center
    }
  }

  if(lookCenterFromRight)
  {
    x--; //move the pupils to the left
    if(x == 0) //if Sparki is looking back at the center
      lookCenterFromRight = false; //turn off looking to center
  }

  if(lookLeft)
  {
    sparki.moveLeft(3); //actually move Sparki
    x--; //move pupils the to the left
    if(x <= -15)//keep the pupils inside the eyes
    {
      lookLeft = false; //done looking right
      lookCenterFromLeft = true; //now look back to the center
    }
  }

  if(lookCenterFromLeft)
  {
    x++; //move the pupils to the right
    if(x == 0) //if Sparki is looking back at the center
      lookCenterFromLeft = false; //turn off looking to center
  }
}










