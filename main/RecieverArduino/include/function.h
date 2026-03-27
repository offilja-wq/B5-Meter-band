#pragma once

#include <Arduino.h>

#include "config.h"

//function prototypes

extern CRGB leds[NUM_LEDS];

void displayBegin();

void setStrip(int i, uint8_t RED, uint8_t GREEN, uint8_t BLUE);

void updateStrip();

bool updateDisplay(bool start);

SENSORS ontvangData();

void updateBlokken(SENSORS *input);

void tekenRaster();

void tekenGrafiekOutline();

void tekenRechterkant();