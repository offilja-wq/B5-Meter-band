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


void handleInput()
{
	SENSORS Sensors;
	
	static unsigned long lastInput = 0;
	static unsigned long lastHeartbeat = 0;
	
	unsigned long now = millis();

	static InputData previousInput;
	static InputData currentInput;

	static Packet packet = {
		.identity = network2.getIdentity(),
	};

	// Stel de input data in
	if ((Sensors.Ntc_result != NTC_NO_REALISTIC_DATA)&&(Sensors.Pressure_result != NTC_NO_REALISTIC_DATA))
	{
		currentInput.NTC_RAW_DATA = READ_NTC();
		currentInput.PRESSURE_RAW_DATA = READ_PRESSURE();

		// Kopieer de input data naar het pakket
		memcpy(packet.data, &currentInput, sizeof(InputData));
	}
	else
	{
		// Geen realistische data, stuur lege data
		memset(packet.data, 0, sizeof(InputData));
	}

	// Controleer of de input is veranderd
	bool inputChanged = memcmp(&currentInput, &previousInput, sizeof(InputData)) != 0;

	bool shouldUpdate = true;

	// Verstuur het pakket als dat nodig is
	if (shouldUpdate)
	{
		network2.send(&packet);
		previousInput = currentInput;

		lastInput = now;
	}
}

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

	handleInput();

	delay(10);
}

