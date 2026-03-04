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

typedef enum : uint8_t
{
    COMMAND_PING = 0x00,
    COMMAND_INPUT = 0x01,

    COMMAND_RESET = 0x03,

    COMMAND_RESTART = 0x10,
    COMMAND_PAIR_CONTROLLER = 0x11,

    COMMAND_ACK = 0xFF
} CommandType;

typedef struct
{
    Identity identity;

    CommandType command;
    uint8_t data[16];
} Packet;

typedef struct
{
    int16_t NTC_RAW_DATA;
    int16_t PRESSURE_RAW_DATA;
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

    void handle();
    void handlePing();

    void send(Packet *packet);
    void acknowledge();

    void onReceive(ReceiveCallback callback);

private:
    void handleReceive(const uint8_t *mac, const uint8_t *data, int len);
    void receiveInternal(const uint8_t *mac, const Packet *packet);

    void monitorPacket(const uint8_t *mac, const Packet *packet);

    Identity identity;
    uint8_t mac[6];

    ReceiveCallback receiveCallback;
};

void printPacket(const uint8_t *mac, const Packet *packet); // Functie om een pakket te debuggen