/*******************************************
 Drawing a bitmap
 image created using bmp2asm:
 http://www.piclist.com/techref/microchip/bmp2asm.htm

 http://arcbotics.com/products/sparki/parts/lcd/
********************************************/
#include <Sparki.h>  // include the sparki library

void setup()
{
}

//define little image of Sparki
// sets up the special variable type needed for images
const static unsigned char __attribute__ ((progmem)) sparki_image[]={ 
0xF0,0x08,0xFE,0x03,0x01,0x3F,0x21,0xA1,0xA1,0x21,0x3F,0x01,0x03,0xFE,0x08,0xF0,
0x01,0x1A,0x27,0x20,0xE2,0x2E,0x2F,0x22,0x22,0x2F,0x2E,0xE2,0x20,0x27,0x1A,0x01,};

void loop()
{
    sparki.clearLCD(); // wipe the LCD clear
    sparki.drawBitmap(20, 20, sparki_image, 16, 16);
    sparki.updateLCD(); // put the drawings on the screen
    delay(1000);
}


