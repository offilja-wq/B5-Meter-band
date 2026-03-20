#pragma once

#include <Arduino.h>

#include <FastLED.h>

#define NUM_LEDS 10
#define PIN_DATA 3

FastLED.addLeds<NEOPIXEL, PIN_DATA>(leds, NUM_LEDS);

CRGB leds[NUM_LEDS] = {0};     // Software gamma mode.