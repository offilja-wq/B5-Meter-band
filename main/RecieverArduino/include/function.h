#pragma once

#include <Arduino.h>

#include "config.h"

//function prototypes

SENSORS getData();

void updateStrip(SENSORS *input);

void setStrip(int i, uint8_t RED, uint8_t GREEN, uint8_t BLUE);

void updateDisplay(SENSORS *input);