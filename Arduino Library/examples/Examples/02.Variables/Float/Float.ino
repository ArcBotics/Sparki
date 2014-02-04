#include <Sparki.h> // include the sparki library

void setup()
{
}

void loop()
{
  sparki.clearLCD(); // erase the LCD

  float foo; // declares a variable of type integer with the name 'foo'

  foo = 10; // assigns the value of 10.0 to foo
  
  float bar = 4; // declare a float with the value of 4.0
  
  sparki.print( "foo: "); 
  sparki.println( foo  );
  
  sparki.print( "bar: ");
  sparki.println( bar ); 
  
  sparki.print( "foo + bar: ");
  sparki.println( foo + bar ); // foo added to bar
  
  sparki.print( "foo * bar: ");
  sparki.println( foo * bar ); // foo multiplied times bar
  
  sparki.print( "foo / bar: ");
  sparki.println( foo / bar ); // foo divided by bar
  
  sparki.updateLCD(); // put what has been drawn onto the screen
  delay(1000); // wait 1 seconds (1000 milliseconds)
}
