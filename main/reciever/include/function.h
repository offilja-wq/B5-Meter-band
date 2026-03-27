#pragma once

#include <Arduino.h>

#include "config.h"
#include "Networking_by_B5.h"

//function prototypes

SENSORS concludeSensors(InputData *input);

//RECIEVE
void printInput(const InputData *input);

void handleResponseReciever(InputData *input);

void createPacket(PACKAGETYPECODE type);

void localSend(SENSORS *input);

void handleNetwork(const uint8_t *mac, const Packet *packet);