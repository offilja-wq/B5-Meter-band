#include "Arduino.h"

#include "config.h"
#include "function.h

void setup()
{
    Serial.begin(115200);

    FastLED.addLeds<NEOPIXEL, PIN_DATA>(leds, NUM_LEDS);
}

void loop()
{

}