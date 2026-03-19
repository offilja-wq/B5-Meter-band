#include "Arduino.h"

#include "config.h"
#include "function.h"

#include "Networking_by_B5.h"
#include "esp_log.h"

// Locatie idetificatie voor debuggen
static const char *TAG = "MAIN";

// Netwerk class instansieren
Networking &networkBand = Networking::getInstance();
// Identiteit struct instansieren
Identity identityBand;

void setup()
{
	// Stel de identiteit in
	identityBand.type = IDENTITY_BAND;

	// Start de seriële communicatie
	Serial.begin(115200);
	Serial.setDebugOutput(true);

	esp_log_level_set("*", ESP_LOG_INFO);

	// Initialiseer het netwerk
	networkBand.setIdentity(identityBand);
	networkBand.onReceive(handleNetwork);
	networkBand.begin();
	createPacket(PACKAGETYPE_CALL_ACKNOWLEDGE);
}

void loop()
{
	// if (networkBand.handlePing()) {
	// 	createPacket(PACKAGETYPE_CALL_ACKNOWLEDGE);
	// }
	delay(10);
}