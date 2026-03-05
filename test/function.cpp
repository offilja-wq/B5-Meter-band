#include "function.h"

#include "Arduino.h"

//lib def - only for this file

#include "config.h"
#include "Networking_by_B5.h"

// Locatie idetificatie voor debuggen
static const char *TAG = "MAIN";

// Netwerk class instansieren
Networking &network = Networking::getInstance();
// Identiteit struct instansieren
Identity identity;

unsigned long lastPacket;


int READ_NTC() {
    //NTC_SENSOR_PIN
    return 300;
}

int READ_PRESSURE() {
    //PRESSURE_SENSOR_PIN
    return 200;
}







//RECIEVE

// Print de input pakketen uit ESP-now af
void printInput(const InputData *input)
{
	unsigned long now = millis();

  	Serial.print(String(input->NTC_RAW_DATA)+"\t"+input->PRESSURE_RAW_DATA+"\n");

	lastPacket = millis();
}

// Handelt alle ESP-now paketten af
void handleNetwork(const uint8_t *mac, const Packet *packet)
{
	unsigned long now = millis();
	
	// switch (packet->command)
	// {
	// case COMMAND_INPUT:
	// 	if (packet->identity.type == IDENTITY_BAND)
	// 		printInput((InputData *)packet->data);

	// 	break;
	// default:
	// 	printPacket(mac, packet);
	// 	break;
	// }
	if (now - lastPacket >= 1000)
	{
		Serial.print("No connection/n");
	}
}


// SEND

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

