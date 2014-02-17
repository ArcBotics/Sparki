/*******************************************
 Basic Ultrasonic test
 
 Show the distance Sparki's eyes are reading
 on the LCD. Sparki will beep when something
 is too close. If it measures -1, that means 
 the sensor is either too close or too far 
 from an object
 
 http://arcbotics.com/products/sparki/parts/ultrasonic-range-finder/
********************************************/
#include <Sparki.h> // include the sparki library

void setup()
{
}

void loop()
{
    sparki.clearLCD();
    
    int cm = sparki.ping(); // measures the distance with Sparki's eyes
    
    sparki.print("Distance: "); 
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
    delay(100); // wait 0.1 seconds (100 milliseconds)
}
