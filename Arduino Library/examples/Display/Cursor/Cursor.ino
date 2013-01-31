/*
  LiquidCrystal Library - Cursor
 
 Demonstrates the use a 16x2 LCD display. 
 
 This sketch prints "Hello World!" to the LCD and
 uses the cursor()  and noCursor() methods to turn
 on and off the cursor.
 
 Library originally added 18 Apr 2008
 by David A. Mellis
 library modified 5 Jul 2009
 by Limor Fried (http://www.ladyada.net)
 example added 9 Jul 2009
 by Tom Igoe 
 modified 22 Nov 2010
 by Tom Igoe
 modified 21 Oct 2012
 by Joseph Schlesinger (http://www.arcbotics.com)
 
 This example code is in the public domain.

 */

// include the library code:
#include <Display.h>

// initialize the library with the numbers of the interface pins
Display lcd;

void setup() {
  // set up the LCD's number of columns and rows: 
  lcd.begin();
  // Print a message to the LCD.
  lcd.print("Hello,");
  lcd.setCursor(0,1);
  lcd.print("World!");
}

void loop() {
  // Turn off the cursor:
  lcd.noCursor();
  delay(500);
   // Turn on the cursor:
  lcd.cursor();
  delay(500);
}

