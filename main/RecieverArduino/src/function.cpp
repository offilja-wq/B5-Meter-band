#include "function.h"

#include "Arduino.h"

//lib def - only for this file

#include "config.h"

#define NUM_LEDS  8
#define DATA_PIN  11 // pin aanpassen
#define CLOCK_PIN 13 // pin aanpassen

CRGB leds[NUM_LEDS];

int ledsAan = map(input->PRESSURE_RAW_DATA, 0, 650, 0, NUM_LEDS);

for (int i = 0; i < NUM_LEDS; i++) {
    if (i < ledsAan) {
        leds[i] = CHSV(140, 255, 255);       // LED aan
    } else {
      leds[i] = CHSV(140, 255, 0);         // LED uit
    }
  }

  FastLED.show();


void setStrip(int i, uint8_t RED, uint8_t GREEN, uint8_t BLUE)
{
    switch (i)
    {
    case 1 ... 8:
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