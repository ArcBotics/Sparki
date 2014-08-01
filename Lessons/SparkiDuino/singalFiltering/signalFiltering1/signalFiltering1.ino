#include <Sparki.h> // include the sparki library

const unsigned int numberOfReadings = 3;

void setup()
{
}

int average()
{
  float result = 0;
  for (unsigned int i=0; i < numberOfReadings; i++)
  {
    result += sparki.ping();
    delay(20);
  }
  return result / numberOfReadings;
}

void loop()
{
    float cm = average();
    
    sparki.clearLCD();
       
    sparki.print("Average: "); 
    sparki.print(cm); // tells the distance to the computer
    sparki.println(" cm"); 
    
    if(cm != -1) // make sure its not too close or too far
    { 
        if(cm < 10) // if the distance measured is less than 10 centimeters
        {
            sparki.beep(); // beep!
        }
    }
    
    sparki.updateLCD();
}
