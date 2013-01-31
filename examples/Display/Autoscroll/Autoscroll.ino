/*
  Diplay Library - Autoscroll
 
 Demonstrates the use a 8x2 LCD display.
 
 This sketch demonstrates the use of the autoscroll()
 and noAutoscroll() functions to make new text scroll or not.
 
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
#include "DiscBot.h"

// initialize the library with the numbers of the interface pins
Display lcd;

void setup() {
  // set up the LCD's number of columns and rows: 
  lcd.begin();
}

void loop() {
  // set the cursor to (0,0):
  lcd.setCursor(0, 0);
  // print from 0 to 9:
  for (int thisChar = 0; thisChar < 8; thisChar++) {
   lcd.print(thisChar);
   delay(500);
  }

  // set the cursor to (8,1):
  lcd.setCursor(8,1);
  // set the display to automatically scroll:
  lcd.autoscroll();
  // print from 0 to 9:
  for (int thisChar = 0; thisChar < 8; thisChar++) {
    lcd.print(thisChar);
    delay(500);
  }
  // turn off automatic scrolling
  lcd.noAutoscroll();
  
  // clear screen for the next loop:
  lcd.clear();
}
