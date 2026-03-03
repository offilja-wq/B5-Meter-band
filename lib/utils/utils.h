#pragma once

#include <Arduino.h>

char *macToString(const uint8_t *mac);
void stringToMac(const char *str, uint8_t *mac);
char *packetToString(const void *packet, size_t size);