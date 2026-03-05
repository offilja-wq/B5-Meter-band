#include "Arduino.h"

#include "config.h"
#include "function.h"

#include "Networking_by_B5.h"
#include "esp_log.h"

// Locatie idetificatie voor debuggen
static const char *TAG = "MAIN";

// Netwerk class instansieren
Networking &network2 = Networking::getInstance();
// Identiteit struct instansieren
Identity identity2;

void setup()
{
	// Identiteit eigenschappen
	identity2.type = IDENTITY_RECIEVER;

	Serial.begin(115200);
	Serial.setDebugOutput(true);

	// Op wat voor niveau ESP LOG zou loggens
	esp_log_level_set("*", ESP_LOG_INFO);

	// Initialiseren van de netwerk instatie
	network2.setIdentity(identity2);
	network2.onReceive(handleNetwork);
	network2.begin();
}

void loop()
{
	network2.handle();

	delay(10);
}