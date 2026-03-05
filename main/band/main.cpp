#include "Arduino.h"

#include "config.h"
#include "function.h"

#include "Networking_by_B5.h"
#include "esp_log.h"


static const char *TAG = "MAIN";

Networking &network = Networking::getInstance();
Identity identity;

void handleInput()
{
	SENSORS Sensors;
	
	static unsigned long lastInput = 0;
	static unsigned long lastHeartbeat = 0;
	
	unsigned long now = millis();

	static InputData previousInput;
	static InputData currentInput;

	static Packet packet = {
		.identity = network.getIdentity(),
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
		network.send(&packet);
		previousInput = currentInput;

		lastInput = now;
	}
}

void handleNetwork(const uint8_t *mac, const Packet *packet)
{
	delay(1);
}

void setup()
{
	// Stel de identiteit in
	identity.type = IDENTITY_BAND;

	// Start de seriële communicatie
	Serial.begin(115200);
	Serial.setDebugOutput(true);

	esp_log_level_set("*", ESP_LOG_INFO);

	// Initialiseer het netwerk
	network.setIdentity(identity);
	network.onReceive(handleNetwork);
	network.begin();

	ESP_LOGI(TAG, "Firmware Version: %s", BP32.firmwareVersion());
}

void loop()
{
	// Werk het netwerk bij
	network.handle();

	handleInput();

	delay(10);
}