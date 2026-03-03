#include "Networking_by_B5.h"

static const char *TAG = "NETWORKING";

Networking &Networking::getInstance()
{
    static Networking instance;
    return instance;
}

void Networking::begin()
{
    WiFi.mode(WIFI_STA);
    esp_wifi_set_channel(NETWORK_CHANNEL, WIFI_SECOND_CHAN_NONE);

#if WIFI_DISABLE_POWER_SAVE 
    esp_bt_controller_disable();
    WiFi.setSleep(false); //Wifi sleepmode uit
#endif

    if (esp_now_init() != ESP_OK) 
    {
        ESP_LOGE(TAG, "Error initializing ESP-NOW");
        return;
    }

    esp_now_peer_info_t broadcastPeer = {};
    broadcastPeer.channel = NETWORK_CHANNEL;
    broadcastPeer.encrypt = false;
    memcpy(broadcastPeer.peer_addr, BROADCAST_ADDRESS, sizeof(BROADCAST_ADDRESS));

    if (esp_now_add_peer(&broadcastPeer) != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to add broadcast peer");
        return;
    }

    ESP_LOGI(TAG, "ESP-NOW initialized");
    ESP_LOGI(TAG, "using MAC %s", WiFi.macAddress().c_str());

    WiFi.macAddress(this->mac);

    esp_now_register_recv_cb([](const uint8_t *mac, const uint8_t *data, int len)
                             { Networking::getInstance().handleReceive(mac, data, len); });
}

void Networking::setIdentity(Identity identity)
{
    this->identity = identity;
}

Identity Networking::getIdentity()
{
    return this->identity;
}

void Networking::send(Packet *packet)
{
    memcpy(&packet->state, &this->state, sizeof(State));

    esp_now_send(BROADCAST_ADDRESS, (uint8_t *)packet, sizeof(Packet));

#if NETWORKING_DEBUG
    monitorPacket(this->mac, packet);
#endif
}

void Networking::acknowledge()
{
    static Packet packet = {
        .identity = this->identity,
        .command = COMMAND_ACK};

    this->send(&packet);
}

void Networking::onReceive(ReceiveCallback callback)
{
    this->receiveCallback = callback;
}

void Networking::handleReceive(const uint8_t *mac, const uint8_t *data, int len)
{
    if (sizeof(Packet) != len)
    {
        ESP_LOGE(TAG, "Received packet of invalid size: %d/%d", len, sizeof(Packet));
        return;
    }

    if (!this->receiveCallback)
    {
        ESP_LOGW(TAG, "No receive callback registered");
        return;
    }

    Packet *packet = (Packet *)data;

    this->receiveCallback(mac, packet);
    this->receiveInternal(mac, packet);

#if NETWORKING_DEBUG
    monitorPacket(mac, packet);
#endif
}

void Networking::receiveInternal(const uint8_t *mac, const Packet *packet)
{
    // if (packet->identity.type == IDENTITY_RECIEVER &&
    //     memcmp(mac, this->mac, sizeof(this->mac)) != 0)

    switch (packet->command)
    {
    case COMMAND_RESTART:
        if (memcmp(packet->data, this->mac, sizeof(this->mac)) == 0)
        {
            ESP_LOGI(TAG, "Received restart command");
            esp_restart();
        }
        break;
    case COMMAND_RESET:
        memset(&this->state, 0, sizeof(this->state));
        break;
    }
}

void Networking::handle()
{
    this->handlePing();

#if NETWORKING_DEBUG
    static char buf[32] = {};
    static size_t bufIndex = 0;

    while (Serial.available())
    {
        char ch = Serial.read();

        if (ch == '\n')
        {
            buf[bufIndex] = 0x00;

            switch (buf[0])
            {
            case COMMAND_RESTART:
            case COMMAND_PAIR_CONTROLLER:
            {
                Packet packet = {
                    .identity = this->identity,
                    .command = (CommandType)buf[0],
                };

                stringToMac(&buf[1], packet.data);

                if (memcmp(packet.data, this->mac, sizeof(this->mac)) == 0)
                {
                    this->monitorPacket(this->mac, &packet);

                    this->receiveInternal(this->mac, &packet);
                    this->receiveCallback(this->mac, &packet);
                }
                else
                    this->send(&packet);

                break;
            }
            case COMMAND_RESET:
            {
                static Packet packet = {
                    .identity = this->identity,
                    .command = (CommandType)buf[0],
                };

                this->send(&packet);
                this->receiveInternal(this->mac, &packet);

                break;
            }
            default:
                ESP_LOGW(TAG, "Unknown debug command: 0x%02X", buf[0]);
                break;
            }

            memset(buf, 0, sizeof(buf));
            bufIndex = 0;
        }
        else
        {
            buf[bufIndex++] = ch;
        }
    }
#endif
}

void Networking::handlePing()
{
    static unsigned long lastPing = 0;
    static unsigned long pingInterval = 300;

    static Packet packet = {
        .identity = this->identity,
        .command = COMMAND_PING};

    unsigned long now = millis();

    if (now - lastPing < pingInterval)
        return;

    this->send(&packet);

    lastPing = now;
    pingInterval = random(500, 1000);
}

void Networking::monitorPacket(const uint8_t *mac, const Packet *packet)
{
    if (Serial.availableForWrite() < 64)
        return;

    Serial.print("nwdbg;");
    Serial.print(macToString(mac));
    Serial.print(";");
    Serial.print(packetToString(&packet->identity, sizeof(packet->identity)));
    Serial.print(";");
    Serial.print(packetToString(&packet->state, sizeof(packet->state)));
    Serial.print(";");
    Serial.print(packetToString(&packet->command, sizeof(packet->command)));
    Serial.print(";");
    Serial.println(packetToString(&packet->data, sizeof(packet->data)));
}

void printPacket(const uint8_t *mac, const Packet *packet)
{
    Serial.println("-----");

    Serial.print("Received packet from: ");
    Serial.println(macToString(mac));

    Serial.print("Identity ID: ");
    Serial.println(packet->identity.id);
    Serial.print("Identity Type: ");
    Serial.println(packet->identity.type == IDENTITY_BAND ? "BAND" : "RECIEVER");
    Serial.print("Command: ");
    Serial.println(packet->command);
    Serial.print("Data: ");
    for (int i = 0; i < sizeof(packet->data); i++)
    {
        if (i > 0)
            Serial.print(" ");
        Serial.print(packet->data[i], HEX);
    }
    Serial.println();
}