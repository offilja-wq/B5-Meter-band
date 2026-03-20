#pragma once

#include <Arduino.h>

#include <WiFi.h>
#include <esp_wifi.h>
#include "esp_bt.h"
#include "esp_now.h"

#include "esp_log.h"
#include "utils.h"

const uint8_t NETWORK_CHANNEL = 2;
const uint8_t BROADCAST_ADDRESS[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

typedef enum : uint8_t
{
    IDENTITY_RECIEVER,
    IDENTITY_BAND,
    IDENTITY_OBSERVER
} IdentityType;

typedef struct
{
    IdentityType type;
} Identity;

typedef struct
{
    Identity identity;
    uint8_t data[20];
} Packet;

typedef enum : uint8_t //Maak de variabele PACKAGETYPECODE
{
    PACKAGETYPE_RETRANSMIT = 01,
    PACKAGETYPE_DATA_SEND = 02,
    PACKAGETYPE_COMMAND_RESET = 03,
    PACKAGETYPE_CALL_STATE = 04,
    PACKAGETYPE_CALL_ACKNOWLEDGE = 05
} PACKAGETYPECODE;

typedef struct //Verander types in InputData.[name]
{
    uint8_t startOfCommunication;
    uint8_t packageSize;
    uint8_t sourceIdentity;
    uint8_t destinationIdentity;
    uint32_t packageCount;
    PACKAGETYPECODE packageTypeCode;

    uint16_t NTC_RAW_DATA;
    uint16_t PRESSURE_RAW_DATA;
    bool PriorityState;

    uint8_t endOfTransmission;
    uint8_t longitudinalRedundancyCheck;

} InputData;

// Callback type voor het ontvangen van pakketten
typedef void (*ReceiveCallback)(const uint8_t *mac, const Packet *packet);

class Networking
{
public:
    static Networking &getInstance(); // Singleton

    void begin(); // Initialiseer networking
    void setIdentity(Identity identity);
    Identity getIdentity();

    bool handlePing();

    void send(Packet *packet);

    void onReceive(ReceiveCallback callback);

    int checkLRCOutput(Packet *packet);
    int checkLRCInput(InputData *input);

private:
    void handleReceive(const uint8_t *mac, const uint8_t *data, int len);
    // void receiveInternal(const uint8_t *mac, const Packet *packet);

    void monitorPacket(const uint8_t *mac, const Packet *packet);

    Identity identity;
    uint8_t mac[6];

    ReceiveCallback receiveCallback;
};

void printPacket(const uint8_t *mac, const Packet *packet); // Functie om een pakket te debuggen