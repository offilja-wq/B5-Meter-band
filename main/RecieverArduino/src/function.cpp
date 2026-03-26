#include "function.h"

#include "Arduino.h"

//lib def - only for this file

#include "config.h"


void setStrip(int i, uint8_t RED, uint8_t GREEN, uint8_t BLUE)
{
    switch (i)
    {
    case 1 ... 8:
        CRGB c(BLUE, GREEN, RED);
        leds[i-1] = c;
        break;
    
    default:
    for(int iCount = 0; iCount <= 9; iCount++)
        CRGB c(BLUE, GREEN, RED);
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