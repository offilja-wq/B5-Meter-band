#pragma once

#include <Arduino.h>

#include "config.h"
#include "Networking_by_B5.h"

//function prototypes

extern CRGB leds[NUM_LEDS];

extern SRC_SENSORS Src_Sensors;

void startSensor();

uint16_t READ_NTC();

uint16_t READ_PRESSURE();

uint8_t READ_HEARTBEAT();

uint8_t READ_SATURATION();

bool checkVingerContact();

void setStrip(int i, uint8_t RED, uint8_t GREEN, uint8_t BLUE);

void updateStrip();

//RECIEVE
void handleResponseBand(InputData *input);

void createPacket(PACKAGETYPECODE type);

void handleNetwork(const uint8_t *mac, const Packet *packet);