#include <Sparki.h> // include the sparki library

void setup()
{
}

void loop()
{
  sparki.clearLCD();
  
  int foo;
  
  foo = 10;
  
  int bar = 4;
  
  sparki.print( "foo: ");
  sparki.println( foo );
  
  sparki.print( "bar: ");
  sparki.println( bar );
  
  sparki.print( "foo + bar:: ");
  sparki.println( foo + bar ); // foo added to bar
  
  sparki.print( "foo * bar: ");
  sparki.println( foo * bar ); // foo multiplied times bar
  
  sparki.print( "foo / bar: ");
  sparki.println( foo / bar ); // foo divided by bar
  
  sparki.updateLCD();
  delay(1000); // wait 0.1 seconds (100 milliseconds)
}
