#include "Arduino.h"

#include "config.h"
#include "function.h"

#include "Networking_by_B5.h"
#include "esp_log.h"

// Locatie idetificatie voor debuggen
static const char *TAG = "MAIN";

// Netwerk class instansieren
Networking &networkReciever = Networking::getInstance();
// Identiteit struct instansieren
Identity identityReciever;


void setup()
{
	// Identiteit eigenschappen
	identityReciever.type = IDENTITY_RECIEVER;

	Serial.begin(115200);
	Serial.setDebugOutput(true);

	// Op wat voor niveau ESP LOG zou loggens
	esp_log_level_set("*", ESP_LOG_INFO);

	// Initialiseren van de netwerk instatie
	networkReciever.setIdentity(identityReciever);
	networkReciever.onReceive(handleNetwork);
	networkReciever.begin();
	createPacket(PACKAGETYPE_CALL_ACKNOWLEDGE);
}

void loop()
{
	// networkReciever.handlePing();

	delay(10);
}


