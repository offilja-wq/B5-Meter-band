#include "function.h"

#include "Arduino.h"

//lib def - only for this file

#include "config.h"
#include "Networking_by_B5.h"

static const char *TAG = "MAIN";

// Netwerk class instansieren
Networking &network = Networking::getInstance();
// Identiteit struct instansieren
Identity identity;

unsigned long lastPacket;


// uint8_t groupID = 0x15;

// char package[100] = {0};

// package[0]= 0;

// }

// RECIEVE
void printInput(const InputData *input)
{
	unsigned long now = millis();

  	Serial.println(
		String(input->packageSize)+			"\t"+
		String(input->sourceIdentity)+		"\t"+
		String(input->destinationIdentity)+	"\t"+
		String(input->packageCount)+		"\t"+
		String(input->packageTypeCode)+		"\t"+
		String(input->NTC_RAW_DATA)+		"\t"+
		String(input->PRESSURE_RAW_DATA)+	"\t"+
		String(input->PriorityState)
	);

	if (now - lastPacket >= 1000)
	{
		Serial.println("No connection");
	}
	lastPacket = millis();
}

// Handelt alle ESP-now paketten af
void handleNetwork(const uint8_t *mac, const Packet *packet)
{
	unsigned long now = millis();
	
	printInput((InputData *)packet->data);
}

void createPacket()
{
	static const char *TAG = "MAIN";

	Networking &networkReciever = Networking::getInstance();
	Identity identityReciever;
	
	SENSORS Sensors;
	
	static unsigned long lastInput = 0;
	static unsigned long lastHeartbeat = 0;
	
	unsigned long now = millis();

	static InputData previousInput;
	static InputData currentInput;

	static Packet packet = {
		.identity = networkReciever.getIdentity(),
	};

	bool shouldUpdate = true;

	// Stel de input data in
	if (shouldUpdate) // ALS NIEUW PAKKET GEMAAKT MOET WORDEN
	{
		//Invoegen nieuwe data
		currentInput.packageSize=0;
    	currentInput.sourceIdentity=0;
    	currentInput.destinationIdentity=0;
    	currentInput.packageCount=0;
    	currentInput.packageTypeCode=PACKAGETYPE_DATA_SEND;

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

	// Verstuur het pakket als dat nodig is
	if (shouldUpdate)
	{	
		currentInput.packageSize = sizeof(Packet);
		currentInput.packageCount = currentInput.packageCount+1;
		networkReciever.send(&packet);
		previousInput = currentInput;

		lastInput = now;
	}
}