#include <Sparki.h>  // include the sparki library

void setup()
{
}

void loop()
{
  sparki.clearLCD(); // wipe the LCD clear

  sparki.drawLine(0,0, 127,63);
  sparki.drawLine(0,63, 127,0);        

  sparki.updateLCD(); // put the drawings on the screen    
  delay(1000);  
}
