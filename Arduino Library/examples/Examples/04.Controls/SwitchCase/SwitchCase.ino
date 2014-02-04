#include <Sparki.h>
 
void setup() 
{ 
}
 
void loop() 
{  
  int code = sparki.readIR(); // look for a remote control code
 
  switch(code){
 
  case 66:  
      sparki.RGB(RGB_RED); // make RGB LED Red
      break;
  case 82:  
      sparki.RGB(RGB_GREEN); // make RGB LED Green
      break;
  case 74:  
      sparki.RGB(RGB_BLUE); // make RGB LED Blue
      break;
  default:
      break;
  }
}
