#pragma once

#include <Arduino.h>

#include "config.h"
#include "Networking_by_B5.h"

//function prototypes

int READ_NTC();

int READ_PRESSURE();

//RECIEVE
void printInput(const InputData *input);

void handleResponseBand(InputData *input);

void createPacket(PACKAGETYPECODE type);

void handleNetwork(const uint8_t *mac, const Packet *packet);