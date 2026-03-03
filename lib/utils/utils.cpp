#include "utils.h"

char *macToString(const uint8_t *mac)
{
    static char macStr[18];
    sprintf(macStr, "%02X:%02X:%02X:%02X:%02X:%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    return macStr;
}

void stringToMac(const char *str, uint8_t *mac)
{
    if (!str || !mac) return;
    sscanf(str, "%02X:%02X:%02X:%02X:%02X:%02X", &mac[0], &mac[1], &mac[2], &mac[3], &mac[4], &mac[5]);
}

char *packetToString(const void *packet, size_t size)
{
    static char packetStr[128];
    const uint8_t *data = (const uint8_t *)packet;

    for (size_t i = 0; i < size; i++) {
        sprintf(&packetStr[i * 3], "%02X ", data[i]);
    }

    return packetStr;
}