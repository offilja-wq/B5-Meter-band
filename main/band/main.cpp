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
Pinout pinoutBand;

void setup()
{
	// Stel de identiteit in
	identityBand.type = IDENTITY_BAND;
	pinoutBand.PIN_LED = PIN_LED_BAND;

	// Initialiseer het netwerk
	networkBand.setIdentity(identityBand);
	networkBand.onReceive(handleNetwork);
	networkBand.begin(pinoutBand);
	createPacket(PACKAGETYPE_CALL_ACKNOWLEDGE);
}

void loop()
{
	networkBand.handlePing(&pinoutBand);
	delay(10);
}