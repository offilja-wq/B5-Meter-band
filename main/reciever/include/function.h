#pragma once

#include "config.h"
#include "Networking_by_B5.h"

//function prototypes

void printInput(const InputData *input);

void handleNetwork(const uint8_t *mac, const Packet *packet);