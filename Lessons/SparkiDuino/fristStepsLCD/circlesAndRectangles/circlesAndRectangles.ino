#include <Sparki.h>  // include the sparki library

void setup()
{
}

void loop()
{
  sparki.clearLCD();

  sparki.drawRect(5,5, 30,10);
  sparki.drawRectFilled(15,17, 30,10);  

  sparki.drawCircle(55,30, 5);
  sparki.drawCircle(20,45, 12);

  sparki.drawCircleFilled(90,40, 20);

  sparki.updateLCD();
  delay(1000);  
}
