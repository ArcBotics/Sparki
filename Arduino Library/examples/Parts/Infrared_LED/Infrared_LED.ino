/*******************************************
 Infrared LED Send Code example
 
 Sparki has an infrared LED. You can use it
 to communicate with other Sparkis and other
 devices that use infrared remote controls.
 Other Sparkis can receive the code using
 sparki.readIR();
 
 This example has Sparki sending a code every
 second. The code increases each time by 1.
 When the code reaches 255, the biggest number
 it can send, it goes back to zero and starts
 all over again.
 
 http://arcbotics.com/products/sparki/parts/infrared-led/
********************************************/
#include <Sparki.h> // include the sparki library

void setup()
{
}

int code = 0;
void loop()
{
  sparki.sendIR(code);
  code = code + 1; // increase code by 1
  
  // set code back to 0 if it goes over 255 - the highest number sendIR can send
  if(code = 255){ 
    code = 0;
  }
  
  delay(1000); // wait one second
}
