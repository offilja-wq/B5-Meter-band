#include "Arduino.h"

#include "config.h"
#include "function.h"

#include "Networking_by_B5.h"
#include "esp_log.h"

// Locatie identificatie voor debuggen
static const char *TAG = "MAIN";

// Netwerk class instansieren
Networking &networkReceiver = Networking::getInstance();
// Identiteit struct instansieren
Identity identityReceiver;
Pinout pinoutReceiver;

void setup()
{
	// Identiteit eigenschappen`
	identityReceiver.type = IDENTITY_RECIEVER;
	pinoutReceiver.PIN_LED = PIN_LED_RECIEVER;

	Serial.begin(115200);
	Serial.setDebugOutput(true);

	Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2);

	// Op wat voor niveau ESP LOG zou loggens
	esp_log_level_set("*", ESP_LOG_INFO);

	// Initialiseren van de netwerk instatie
	networkReceiver.setIdentity(identityReceiver);
	networkReceiver.onReceive(handleNetwork);
	networkReceiver.begin(pinoutReceiver);
	createPacket(PACKAGETYPE_CALL_ACKNOWLEDGE);
}

void loop()
{
	if (networkReceiver.handlePing(&pinoutReceiver)) {
		createPacket(PACKAGETYPE_CALL_ACKNOWLEDGE);
	}
}