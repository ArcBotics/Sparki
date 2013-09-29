#include <Sparki.h>  // include the robot library

SparkiClass robot;

// a bitmap of a 16x16 Sparki icon, used for testdrawbitmap
#define LOGO16_GLCD_HEIGHT 16 
#define LOGO16_GLCD_WIDTH  16 

static unsigned char __attribute__ ((progmem)) logo16_glcd_bmp[]={0xF0,0x08,0xFE,0x03,0x01,0x3F,
0x21,0xA1,0xA1,0x21,0x3F,0x01,0x03,0xFE,0x08,0xF0,0x01,0x1A,0x27,0x20,0xE2,0x2E,0x2F,0x22,0x22,
0x2F,0x2E,0xE2,0x20,0x27,0x1A,0x01};

void setup()   {                
  // initialize the display
  robot.begin();
  delay(1000);
}

void loop()                     
{
  // draw a single pixel
  robot.clear();        // clear the display buffer so that we can write new things on it
  robot.setpixel(10, 10, BLACK);
  robot.display();        // show the changed display buffer data on the LCD (it doesn't show on the LCD until here)
  delay(2000);

  // draw many lines
  robot.clear();
  testdrawline();
  robot.display();       // show the lines
  delay(2000);

  // draw rectangles
  robot.clear();
  testdrawrect();
  robot.display();
  delay(2000);

  // draw multiple rectangles
  robot.clear();
  testfillrect();
  robot.display();
  delay(2000);

  // draw mulitple circles
  robot.clear();
  testdrawcircle();
  robot.display();
  delay(2000);

  // draw a black circle, 10 pixel radius, at location (32,32)
  robot.clear();
  robot.fillcircle(32, 32, 10, BLACK);
  robot.display();
  delay(2000);

  // draw the first ~120 characters in the font
  robot.clear();
  testdrawchar();
  robot.display();
  delay(2000);

  // draw a string at location (0,0)
  robot.clear();
  robot.drawstring(0, 0, "Lorem ipsum dolor sit amet, consectetur adipisicing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation");
  robot.display();
  delay(2000);
  robot.clear();

  // draw a bitmap icon and 'animate' movement
  testdrawbitmap(logo16_glcd_bmp, LOGO16_GLCD_HEIGHT, LOGO16_GLCD_WIDTH);
}

void testdrawchar(void) {
  for (uint8_t i=0; i < 168; i++) {
    robot.drawchar((i % 21) * 6, i/21, i);
  }    
}

void testdrawcircle(void) {
  for (uint8_t i=0; i<64; i+=2) {
    robot.drawcircle(63, 31, i, BLACK);
  }
}


void testdrawrect(void) {
  for (uint8_t i=0; i<64; i+=2) {
    robot.drawrect(i, i, 128-i, 64-i, BLACK);
  }
}

void testfillrect(void) {
  for (uint8_t i=0; i<64; i++) {
      // alternate colors for moire effect
    robot.fillrect(i, i, 128-i, 64-i, i%2);
  }
}

void testdrawline() {
  for (uint8_t i=0; i<128; i+=4) {
    robot.drawline(0, 0, i, 63, BLACK);
  }
  for (uint8_t i=0; i<64; i+=4) {
    robot.drawline(0, 0, 127, i, BLACK);
  }

  robot.display();
  delay(1000);

  for (uint8_t i=0; i<128; i+=4) {
    robot.drawline(i, 63, 0, 0, WHITE);
  }
  for (uint8_t i=0; i<64; i+=4) {
    robot.drawline(127, i, 0, 0, WHITE);
  }
}

#define NUMFLAKES 10
#define XPOS 0
#define YPOS 1
#define DELTAY 2

void testdrawbitmap(const uint8_t *bitmap, uint8_t w, uint8_t h) {
  uint8_t icons[NUMFLAKES][3];
  srandom(666);     // whatever seed
 
  // initialize
  for (uint8_t f=0; f< NUMFLAKES; f++) {
    icons[f][XPOS] = random() % 128;
    icons[f][YPOS] = 0;
    icons[f][DELTAY] = random() % 5 + 1;
  }

  while (1) {
    // draw each icon
    for (uint8_t f=0; f< NUMFLAKES; f++) {
      robot.drawbitmap(icons[f][XPOS], icons[f][YPOS], logo16_glcd_bmp, w, h, BLACK);
    }
    robot.display();
    delay(50);
    
    // then erase it + move it
    for (uint8_t f=0; f< NUMFLAKES; f++) {
      robot.drawbitmap(icons[f][XPOS], icons[f][YPOS],  logo16_glcd_bmp, w, h, 0);
      // move it
      icons[f][YPOS] += icons[f][DELTAY];
      // if its gone, reinit
      if (icons[f][YPOS] > 64) {
	icons[f][XPOS] = random() % 128;
	icons[f][YPOS] = 0;
	icons[f][DELTAY] = random() % 5 + 1;
      }
    }
  }
}