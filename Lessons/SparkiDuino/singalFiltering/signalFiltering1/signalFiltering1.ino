#include <Sparki.h> // include the sparki library


void setup()
{
}

int average(unsigned int numberOfReadings)
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
    float cm = average(3); // read 3 times the sensor and return the average (or arithmetic mean)
    
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
