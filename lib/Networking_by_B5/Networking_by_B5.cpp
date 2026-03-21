#include "Networking_by_B5.h"

static const char *TAG = "NETWORKING";

Networking &Networking::getInstance()
{
    static Networking instance;
    return instance;
}

void Networking::begin(Pinout pinout)
{
    // Stuur pinout data naar globaal struct van library
    this->pinout = pinout;
    pinMode(pinout.PIN_LED, OUTPUT);

    // Start de seriële communicatie
	Serial.begin(115200);
	Serial.setDebugOutput(true);

	esp_log_level_set("*", ESP_LOG_INFO);
    
    // Stel wifi module in
    WiFi.mode(WIFI_STA);
    esp_wifi_set_channel(NETWORK_CHANNEL, WIFI_SECOND_CHAN_NONE);
    WiFi.setSleep(false);

    // Check ESP-NOW
    if (esp_now_init() != ESP_OK)
    {
        ESP_LOGE(TAG, "Error initializing ESP-NOW");
        return;
    }

    // Activeer communicatie band 
    esp_now_peer_info_t broadcastPeer = {};
    broadcastPeer.channel = NETWORK_CHANNEL;
    broadcastPeer.encrypt = false;
    memcpy(broadcastPeer.peer_addr, BROADCAST_ADDRESS, sizeof(BROADCAST_ADDRESS));

    // Check band activatie
    if (esp_now_add_peer(&broadcastPeer) != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to add broadcast peer");
        return;
    }

    ESP_LOGI(TAG, "ESP-NOW initialized");
    ESP_LOGI(TAG, "using MAC %s", WiFi.macAddress().c_str());

    WiFi.macAddress(this->mac);

    // Zet data in register met activatie functie
    esp_now_register_recv_cb([](const uint8_t *mac, const uint8_t *data, int len)
                             { Networking::getInstance().handleReceive(mac, data, len); });
}

void Networking::setIdentity(Identity identity)
{
    // Stuur identiteit naar globaal struct van library
    this->identity = identity;
}

Identity Networking::getIdentity()
{
    // Stuur identiteit terug
    return this->identity;
}

Pinout Networking::getPinout()
{
    // Stuur pinout terug
    return this->pinout;
}

void Networking::send(Packet *packet)
{
    // Verzend pakket
    esp_now_send(BROADCAST_ADDRESS, (uint8_t *)packet, sizeof(Packet));

#if NETWORKING_DEBUG
    monitorPacket(this->mac, packet);
#endif
}

void Networking::onReceive(ReceiveCallback callback)
{
    // Voer locale functie in als trigger functie
    this->receiveCallback = callback;
}

void Networking::handleReceive(const uint8_t *mac, const uint8_t *data, int len)
{
    // Check lengte van het pakket
    if (sizeof(Packet) != len)
    {
        ESP_LOGE(TAG, "Received packet of invalid size: %d/%d", len, sizeof(Packet));
        return;
    }

    // Geen trigger functie opgegeven
    if (!this->receiveCallback)
    {
        ESP_LOGW(TAG, "No receive callback registered");
        return;
    }
    
    // Geeft de trigger functie de ruwe data van het pakket
    Packet *packet = (Packet *)data;
    this->receiveCallback(mac, packet);
}

bool Networking::handlePing(Pinout *localPinout)
{
	unsigned long now = millis();
	unsigned long lastPing;

    // geef elke 500ms true terug, anders false
    if(((now - lastPing) > 500))
	{       
		lastPing = now;
        return true;
        digitalWrite(localPinout->PIN_LED, 0);
	} else {
        return false;
    }
}

int Networking::checkLRCOutput(Packet *packet)  
{   
    uint8_t LRC;
    uint8_t *data = (uint8_t*)packet;

    // Ga elke byte af en toets deze aan de LRC beginwaarde
    for(uint8_t i = 0; i < sizeof(Packet) - 1; i++)
    {
        LRC ^= data[i];
    }
    return LRC;
}

int Networking::checkLRCInput(InputData *input)
{
    uint8_t LRC = 0;
    uint8_t *data = (uint8_t*)input;

    // Ga elke byte af en toets deze aan de LRC beginwaarde
    for (size_t i = 0; i < sizeof(InputData) - 1; i++)
    {
        LRC ^= data[i];
    }

    return LRC;
}

void Networking::monitorPacket(const uint8_t *mac, const Packet *packet)
{
    if (Serial.availableForWrite() < 64)
        return;

    Serial.print("nwdbg;");
    Serial.print(macToString(mac));
    Serial.print(";");
    Serial.println(packetToString(&packet->data, sizeof(packet->data)));
}

void printPacket(const uint8_t *mac, const Packet *packet)
{
    Serial.println("-----");

    Serial.print("Received packet from: ");
    Serial.println(macToString(mac));
    Serial.print("Data: ");
    for (int i = 0; i < sizeof(packet->data); i++)
    {
        if (i > 0)
            Serial.print(" ");
        Serial.print(packet->data[i], HEX);
    }
    Serial.println();
}