/*******************************************
 Basic Radio test
 
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
  radio.begin();
}

char start[] = "Hello"; 
void loop()
{
// Uncomment this code if receiving
  if ( radio.available() ){
    Serial.println( radio.readChar() );
  }

// Uncomment this code if transmitting  
  for(int i=0; i<5; i++){
    radio.writeChar(start[i]);
  } 
  delay(1000);
}
