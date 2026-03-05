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
    WiFi.setSleep(false);
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



// typedef struct //Verander types in Packet.[name]
// {
//     uint8_t startOfCommunication;
//     uint8_t packetLength;
//     bool priorityState;

//     uint8_t sourceIdentity;
//     uint8_t destinationIdentity;
//     uint32_t packetCount;
//     uint8_t packageFunctionCode;

//     uint32_t realData;
    
//     uint8_t endOfTransmission;
//     bool redundancyCheck; //LRC
// } Packet;

void Networking::createPacket(const Packet *packet, bool newData, uint8_t sourceIdentity)
{
    Packet.startOfCommunication = 01;
    Packet.packetLength = 113;
    Packet.priorityState = newData;
    
    Packet.sourceIdentity = strtol("00010101");
    Packet.destinationIdentity = strtol("00010101");
    packet.packageFunctionCode = PACKAGETYPE_DATA_SEND;
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
    // switch ()
    // {
    // case COMMAND_RESTART:
    //     if (memcmp(packet->data, this->mac, sizeof(this->mac)) == 0)
    //     {
    //         ESP_LOGI(TAG, "Received restart command");
    //         esp_restart();
    //     }
    //     break;
    // case COMMAND_RESET:
    //     break;
    // }
}

void Networking::handle()
{
    handleConnection();

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
                // Packet packet = {
                //     .identity = this->identity,
                //     .command = (CommandType)buf[0],
                // };

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
                // static Packet packet = {
                //     .identity = this->identity,
                //     .command = (CommandType)buf[0],
                // };

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

void Networking::handleConnection(const Packet *packet)
{
    unsigned long lastPacket = 0;
    unsigned long pingInterval = 300;
    bool newPacketNeeded;
    // static Packet packet = {
    //     .identity = this->identity,
    // };

    unsigned long now = millis();

    if ((now - lastPacket < pingInterval)&&(!newPacketNeeded))
    {
        return;
    }
    
    createPacket(Packet *packet);
    esp_now_send(BROADCAST_ADDRESS, (uint8_t *)packet, sizeof(Packet));
    packet.packetCount = packet->packetCount+1;
    newPacketNeeded = false;

    lastPacket = now;
    pingInterval = random(500, 1000);

    #if NETWORKING_DEBUG
    monitorPacket(this->mac, packet);
    #endif
}

void Networking::monitorPacket(const uint8_t *mac, const Packet *packet)
{
    if (Serial.availableForWrite() < 64)
        return;

    Serial.print("nwdbg;");
    Serial.print(macToString(mac));
    Serial.print(";");
    Serial.println(packetToString(&packet, sizeof(packet)));
}

void printPacket(const uint8_t *mac, const Packet *packet)
{
    Serial.println("-----");

    Serial.print("Received packet from: ");
    Serial.println(macToString(mac));
    Serial.print("Data: ");
    // for (int i = 0; i < sizeof(packet); i++)
    // {
        // if (i > 0)
        Serial.print(" ");
        Serial.print(packet->startOfCommunication, HEX);
        Serial.print("\t");
        Serial.print(packet->packetLength, HEX);
        Serial.print("\t");
        Serial.print(packet->packetCount, HEX);

    // }
    Serial.println();
}

uint8_t startOfCommunication;
    uint8_t packetLength;