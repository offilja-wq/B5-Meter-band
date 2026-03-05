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

typedef enum : uint8_t
{
    PACKAGETYPE_RETRANSMIT = 01,
    PACKAGETYPE_DATA_SEND = 02,
    PACKAGETYPE_COMMAND_RESET = 03,
    PACKAGETYPE_CALL_STATE = 04,
    PACKAGETYPE_CALL_ACKNOWLEDGE = 05

} packageFunctionCode;

typedef enum : bool
{
    PRIORITYSTATE_LOW = false,
    PRIORITYSTATE_HIGH = true
} priorityState;

typedef struct //Verander types in Packet.[name]
{
    uint8_t startOfCommunication;
    uint8_t packetLength;
    bool priorityState;
    
    uint8_t sourceIdentity;
    uint8_t destinationIdentity;
    uint32_t packetCount;
    uint8_t packageFunctionCode;

    uint32_t realData;
    
    uint8_t endOfTransmission;
    bool redundancyCheck; //LRC
} Packet;

// Callback type voor het ontvangen van pakketten
typedef void (*ReceiveCallback)(const uint8_t *mac, const Packet *packet);

class Networking
{
public:
    static Networking &getInstance(); // Singleton

    void begin(); // Initialiseer networking
    // void setIdentity(Identity identity);
    // Identity getIdentity();

    void handle();
    // void handlePing();

    // void send(Packet *packet);
    void createPacket(Packet *packet);

    void handleConnection(Packet *packet);

    void onReceive(ReceiveCallback callback);

    void printPacket(uint8_t *mac, Packet *packet); // Functie om een pakket te debuggen

private:
    void handleReceive(const uint8_t *mac, const uint8_t *data, int len);
    void receiveInternal(const uint8_t *mac, const Packet *packet);

    void monitorPacket(const uint8_t *mac, const Packet *packet);

    uint8_t mac[6];

    ReceiveCallback receiveCallback;
};