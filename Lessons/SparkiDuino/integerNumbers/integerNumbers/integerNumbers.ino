#include <Sparki.h> // include the sparki library

void setup()
{
}

void loop()
{
  sparki.clearLCD();
  
  int foo;
  
  foo = 10;
  
  int blerp = 4;
  
  sparki.print( "foo: ");
  sparki.println( foo );
  
  sparki.print( "blerp: ");
  sparki.println( blerp );
  
  sparki.print( "foo + blerp:: ");
  sparki.println( foo + blerp ); // foo added to bar
  
  sparki.print( "foo * blerp: ");
  sparki.println( foo * blerp ); // foo multiplied times bar
  
  sparki.print( "foo / blerp: ");
  sparki.println( foo / blerp ); // foo divided by bar
  
  sparki.updateLCD();
  delay(1000); // wait 0.1 seconds (100 milliseconds)
}
