#pragma once

#include "config.h"
#include "Networking_by_B5.h"

#include <Arduino.h>

//function prototypes

int READ_NTC();

int READ_PRESSURE();

// void printMacAddress();

//RECIEVE
void printInput(const InputData *input);

void handleNetwork(const uint8_t *mac, const Packet *packet);