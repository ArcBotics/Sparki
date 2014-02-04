#include <Sparki.h> // include the sparki library
 
void setup()
{
}
 
void loop()
{
  sparki.clearLCD(); // erase the LCD
 
  char foo; // declares a variable of type chracter
 
  sparki.print( "foo: "); 
  sparki.println( foo  );
 
  foo = 'b' + 1;
  sparki.print( "'b' + 1: ");
  sparki.println( foo ); // b + 1
 
  foo = 'b' - 1;
  sparki.print( "'b' - 1: ");
  sparki.println( foo ); // b - 1
 
  sparki.updateLCD(); // put what has been drawn onto the screen
  delay(1000); // wait 1 seconds (1000 milliseconds)
}
