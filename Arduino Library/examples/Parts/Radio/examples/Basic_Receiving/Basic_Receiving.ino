/*******************************************
 Basic Radio test - Receiving
 
 Some versions of Sparki come with an NRF24L01+
 radio module. This code will let you test the
 module on a Sparki, using the serial monitor.
 
 http://arcbotics.com/products/sparki/parts/radio/
********************************************/
#include <SPI.h>
#include <Radio.h>

Radio radio;

void setup()
{
  Serial.begin(9600);
  radio.begin();
}

void loop()
{
  if ( radio.available() ){ // if data is unread from the radio
    Serial.println( radio.readChar() );
  }
}
