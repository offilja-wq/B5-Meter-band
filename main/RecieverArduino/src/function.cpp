#include "function.h"

#include "Arduino.h"

//lib def - only for this file

#include "config.h"

SENSORS getData()
{
  if (mySerial.available()&&(mySerial.read() == START_BYTE)) 
  {
    while (mySerial.available() < sizeof(data));
    mySerial.readBytes((uint8_t*)&data, sizeof(data));
    return data;
  }
}

void updateStrip(SENSORS *input)
{
	unsigned long now = millis();
}

void setStrip(int i, uint8_t RED, uint8_t GREEN, uint8_t BLUE)
{
    switch (i)
    {
    case 1 ... 10:
        CRGB c(b, g, r)
        leds[i-1] = c;
        break;
    
    default:
    for(int iCount = 0; iCount <= 9; iCount++)
        CRGB c(b, g, r)
        leds[i] = c;
        break;
    }
    FastLED.show();
}

void updateStrip()
{

}

void updateDisplay()
{
    
}