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
	
	pinMode(NTC_SENSOR_PIN, INPUT);
	pinMode(PRESSURE_SENSOR_PIN, INPUT);

	// Initialiseer het netwerk
	networkBand.setIdentity(identityBand);
	networkBand.onReceive(handleNetwork);
	networkBand.begin(pinoutBand);
	createPacket(PACKAGETYPE_CALL_ACKNOWLEDGE);

	// startSensor();

	// pinMode(LED_STATUS, OUTPUT);

	// #ifdef LED_STATUS
	// 	digitalWrite(LED_STATUS, HIGH);
	// #endif

	// Initialiseer de ledstrip
	FastLED.addLeds<APA102, DATA_PIN, CLOCK_PIN, BGR>(leds, NUM_LEDS);
}

void loop()
{
	networkBand.handlePing(&pinoutBand);
	delay(10);
}