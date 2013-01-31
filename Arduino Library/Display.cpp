#include "Display.h"

#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include "Arduino.h"

#include <Display.h>
#include <SPI.h>

// When the display powers up, it is configured as follows:
//
// 1. Display clear
// 2. Function set: 
//    DL = 1; 8-bit interface data 
//    N = 0; 1-line display 
//    F = 0; 5x8 dot character font 
// 3. Display on/off control: 
//    D = 0; Display off 
//    C = 0; Cursor off 
//    B = 0; Blinking off 
// 4. Entry mode set: 
//    I/D = 1; Increment by 1 
//    S = 0; No shift 
//
// Note, however, that resetting the Arduino doesn't reset the LCD, so we
// can't assume that its in that state when a sketch starts (and the
// Display constructor is called).

Display::Display()
{

}

void Display::begin() {
  // initiate the SPI register
  SPI.begin(); 
  SPI.setClockDivider(SPI_CLOCK_DIV2); 

  _displayfunction = LCD_4BITMODE | LCD_2LINE | LCD_5x8DOTS;
  _currline = 0;
  _numlines = 2;

  delayMicroseconds(50000); 
  // Now we pull both RS and R/W low to begin commands
  shiftRegister(0x00,0x00);
  
  // we start in 8bit mode, try to set 4 bit mode
  command(0x03);
  delayMicroseconds(4500); // wait min 4.1ms

  // second try
  command(0x03);
  delayMicroseconds(4500); // wait min 4.1ms

  // third go!
  command(0x03); 
  delayMicroseconds(150);

  // finally, set to 4-bit interface
  command(0x02); 

  // finally, set # lines, font size, etc.
  command(LCD_FUNCTIONSET | _displayfunction);  

  // turn the display on with no cursor or blinking default
  _displaycontrol = LCD_DISPLAYON | LCD_CURSOROFF | LCD_BLINKOFF;  
  display();

  // clear it off
  clear();

  // Initialize to default text direction (for romance languages)
  _displaymode = LCD_ENTRYLEFT | LCD_ENTRYSHIFTDECREMENT;
  // set the entry mode
  command(LCD_ENTRYMODESET | _displaymode);
}

/********** high level commands, for the user! */
void Display::clear()
{
  command(LCD_CLEARDISPLAY);  // clear display, set cursor position to zero
  delayMicroseconds(2000);  // this command takes a long time!
}

void Display::home()
{
  command(LCD_RETURNHOME);  // set cursor position to zero
  delayMicroseconds(2000);  // this command takes a long time!
}

void Display::setCursor(uint8_t col, uint8_t row)
{
  int row_offsets[] = { 0x00, 0x40, 0x14, 0x54 };
  if ( row >= _numlines ) {
    row = _numlines-1;    // we count rows starting w/0
  }
  
  command(LCD_SETDDRAMADDR | (col + row_offsets[row]));
}

// Turn the display on/off (quickly)
void Display::noDisplay() {
  _displaycontrol &= ~LCD_DISPLAYON;
  command(LCD_DISPLAYCONTROL | _displaycontrol);
}
void Display::display() {
  _displaycontrol |= LCD_DISPLAYON;
  command(LCD_DISPLAYCONTROL | _displaycontrol);
}

// Turns the underline cursor on/off
void Display::noCursor() {
  _displaycontrol &= ~LCD_CURSORON;
  command(LCD_DISPLAYCONTROL | _displaycontrol);
}
void Display::cursor() {
  _displaycontrol |= LCD_CURSORON;
  command(LCD_DISPLAYCONTROL | _displaycontrol);
}

// Turn on and off the blinking cursor
void Display::noBlink() {
  _displaycontrol &= ~LCD_BLINKON;
  command(LCD_DISPLAYCONTROL | _displaycontrol);
}
void Display::blink() {
  _displaycontrol |= LCD_BLINKON;
  command(LCD_DISPLAYCONTROL | _displaycontrol);
}

// These commands scroll the display without changing the RAM
void Display::scrollDisplayLeft(void) {
  command(LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVELEFT);
}
void Display::scrollDisplayRight(void) {
  command(LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVERIGHT);
}

// This is for text that flows Left to Right
void Display::leftToRight(void) {
  _displaymode |= LCD_ENTRYLEFT;
  command(LCD_ENTRYMODESET | _displaymode);
}

// This is for text that flows Right to Left
void Display::rightToLeft(void) {
  _displaymode &= ~LCD_ENTRYLEFT;
  command(LCD_ENTRYMODESET | _displaymode);
}

// This will 'right justify' text from the cursor
void Display::autoscroll(void) {
  _displaymode |= LCD_ENTRYSHIFTINCREMENT;
  command(LCD_ENTRYMODESET | _displaymode);
}

// This will 'left justify' text from the cursor
void Display::noAutoscroll(void) {
  _displaymode &= ~LCD_ENTRYSHIFTINCREMENT;
  command(LCD_ENTRYMODESET | _displaymode);
}

// Allows us to fill the first 8 CGRAM locations
// with custom characters
void Display::createChar(uint8_t location, uint8_t charmap[]) {
  location &= 0x7; // we only have 8 locations 0-7
  command(LCD_SETCGRAMADDR | (location << 3));
  for (int i=0; i<8; i++) {
    write(charmap[i]);
  }
}
/*********** mid level commands, for sending data/cmds */

void Display::command(uint8_t value) {
  _toggle_mode = 0x04;
  send(value);
}

inline size_t Display::write(uint8_t value) {
  _toggle_mode = 0x0C;
  send(value);
  return 1; // assume success
}

/************ low level data pushing commands **********/

void Display::send(uint8_t value){
  // Reverse the byte in-place with magic (as lines D7-D4 are reversed)
  value = (value & 0x0F) << 4 | (value & 0xF0) >> 4;
  value = (value & 0x33) << 2 | (value & 0xCC) >> 2;
  value = (value & 0x55) << 1 | (value & 0xAA) >> 1;
  
  // Split into an upper and lower byte
  uint8_t value_lower, value_upper, toggle;
  value_upper = value>>4;
  value_lower = value & 0x0F;  
  
  // write char to display
  _display_value = value_lower;
  shiftRegister(0x00,_display_value);
  pulseEnable();
  delay(5);
  
  _display_value = value_upper;
  shiftRegister(0x00,_display_value);
  pulseEnable();
  delay(5);
}

void Display::pulseEnable(void){
  shiftRegister(0x00,_display_value);
  shiftRegister(_toggle_mode,_display_value);
  shiftRegister(0x00,_display_value);
}

void Display::shiftRegister(uint8_t outByte1, uint8_t outByte2) {
    PORTD &= 0xDF;    // pull PD5 low
    SPI.transfer(outByte1);
    SPI.transfer(outByte2);
    PORTD |= 0x20;    // pull PD5 high to latch in spi transfers
}


