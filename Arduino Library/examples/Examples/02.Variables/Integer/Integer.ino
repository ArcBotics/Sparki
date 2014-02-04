/*******************************************
 Basic Array test
 
This code will show you how to use basic arrays.
********************************************/
#include <Sparki.h> // include the sparki library

void setup()
{
}

void loop()
{
  sparki.clearLCD(); // erase the LCD

  int foo; // declares a variable of type integer with the name 'foo'

  foo = 10; // assigns the value of 10 to foo
  
  int bar  = 4; // declares an integer with the value of 4
  
  sparki.print( "foo: "); 
  sparki.println( foo  );
  
  sparki.print( "bar: ");
  sparki.println( bar  );
  
  sparki.print( "foo + bar:: ");
  sparki.println( foo + bar ); // foo added to bar
  
  sparki.print( "foo * bar: ");
  sparki.println( foo * bar ); // foo multiplied times bar
  
  sparki.print( "foo / bar: ");
  sparki.println( foo / bar ); // foo divided by bar
  
  sparki.updateLCD(); // put what has been drawn onto the screen
  delay(1000); // wait 1 seconds (1000 milliseconds)
}
