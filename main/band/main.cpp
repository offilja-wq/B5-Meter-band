#include "Arduino.h"

#include "config.h"
#include "function.h"

#include "Networking_by_B5.h"
#include "esp_log.h"

// Locatie idetificatie voor debuggen
static const char *TAG = "MAIN";

CRGB leds[NUM_LEDS] = {0}; 

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

	FastLED.addLeds<APA102, DATA_PIN, CLOCK_PIN, RGB>(leds, NUM_LEDS);

	digitalWrite(PIN_LED_BAND, 1);
	delay(10000);
}

void loop()
{
	networkBand.handlePing(&pinoutBand);
	delay(10);
}