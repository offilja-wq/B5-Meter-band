#include "Arduino.h"

#include "config.h"
#include "function.h"

#include "Networking_by_B5.h"
#include "esp_log.h"


// Locatie idetificatie voor debuggen
static const char *TAG = "MAIN";

// Netwerk class instansieren
Networking &network = Networking::getInstance();
// Identiteit struct instansieren
Identity identity;

unsigned long lastPacket;

// Handelt de input pakketen uit ESP-now af
void handleInput(const InputData *input)
{
	unsigned long now = millis();

  	Serial.print(String(input->NTC_RAW_DATA)+"\t"+input->PRESSURE_RAW_DATA+"\n");

	lastPacket = millis();
}

// Handelt alle ESP-now paketten af
void handleNetwork(const uint8_t *mac, const Packet *packet)
{
	// Switched over de pakket type, om hem juist af te handelen
	switch (packet->command)
	{
	case COMMAND_INPUT:
		if (packet->identity.type == IDENTITY_BAND)
			handleInput((InputData *)packet->data);

		break;
	case COMMAND_PING:
	default:
		printPacket(mac, packet);
		break;
	}
}

void setup()
{
	// Identiteit eigenschappen
	identity.type = IDENTITY_RECIEVER;

	Serial.begin(115200);
	Serial.setDebugOutput(true);

	// Op wat voor niveau ESP LOG zou loggens
	esp_log_level_set("*", ESP_LOG_INFO);

	// Initialiseren van de netwerk instatie
	network.setIdentity(identity);
	network.onReceive(handleNetwork);
	network.begin();
}

void loop()
{
	unsigned long now = millis();

	// Als de laatste ESP-now pakket te lang geleden is
	if (now - lastPacket >= 1000)
	{
		Serial.print("No connection/n");
	}

	network.handle();

	delay(10);
}

