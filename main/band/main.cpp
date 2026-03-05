#include "Arduino.h"

#include "config.h"
#include "function.h"

#include "Networking_by_B5.h"
#include "esp_log.h"


static const char *TAG = "MAIN";

Networking &network = Networking::getInstance();

void handleInput()
{
	SENSORS Sensors;
	
	static unsigned long lastInput = 0;
	static unsigned long lastHeartbeat = 0;
	
	unsigned long now = millis();

	static Packet previousInput;
	static Packet currentInput;

	// Stel de input data in
	if ((Sensors.Ntc_result != NTC_NO_REALISTIC_DATA)&&(Sensors.Pressure_result != NTC_NO_REALISTIC_DATA))
	{
		// currentInput.NTC_RAW_DATA = READ_NTC();
		// currentInput.PRESSURE_RAW_DATA = READ_PRESSURE();

		// Kopieer de input data naar het pakket
		memcpy(Packet.realData, &currentInput, sizeof(Packet));
	}
	else
	{
		// Geen realistische data, stuur lege data
		memset(Packet.realData, 0, sizeof(Packet));
	}

	// Controleer of de input is veranderd
	bool inputChanged = memcmp(&currentInput, &previousInput, sizeof(Packet)) != 0;

	bool shouldUpdate = true;

	// Verstuur het pakket als dat nodig is
	if (shouldUpdate)
	{
		network.createPacket(Packet *packet);
    	esp_now_send(BROADCAST_ADDRESS, (uint8_t *)packet, sizeof(Packet));
    	packet.packetCount = packet->packetCount+1;
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


	// Start de seriële communicatie
	Serial.begin(115200);
	Serial.setDebugOutput(true);

	esp_log_level_set("*", ESP_LOG_INFO);

	// Initialiseer het netwerk
	network.onReceive(handleNetwork);
	network.begin();

	ESP_LOGI(TAG, "Firmware Version: %s", BP32.firmwareVersion());
}

void loop()
{
	// Werk het netwerk bij
	network.handleConnection();

	handleInput();

	delay(100);
}