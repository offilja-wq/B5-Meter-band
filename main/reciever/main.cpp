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
Pinout pinoutReciever;

void setup()
{
	// Identiteit eigenschappen`
	identityReciever.type = IDENTITY_RECIEVER;
	pinoutReciever.PIN_LED = PIN_LED_RECIEVER;

	Serial.begin(115200);
	Serial.setDebugOutput(true);

	Serial2.begin(115200, SERIAL_8N1, RXD2, TXD2);

	// Op wat voor niveau ESP LOG zou loggens
	esp_log_level_set("*", ESP_LOG_INFO);

	// Initialiseren van de netwerk instatie
	networkReciever.setIdentity(identityReciever);
	networkReciever.onReceive(handleNetwork);
	networkReciever.begin(pinoutReciever);
	createPacket(PACKAGETYPE_CALL_ACKNOWLEDGE);
}

void loop()
{
	if (networkReciever.handlePing(&pinoutReciever)) {
		createPacket(PACKAGETYPE_CALL_ACKNOWLEDGE);
	}

	// delay(20);
}