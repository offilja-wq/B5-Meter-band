#include "function.h"

#include "Arduino.h"

//lib def - only for this file

#include "config.h"
#include "Networking_by_B5.h"

static const char *TAG = "MAIN";

unsigned long lastPacket;

// RECIEVE
void printInput(InputData *input)
{

	Networking &networkReciever = Networking::getInstance();
	unsigned long now = millis();

  	Serial.println(
		String(input->startOfCommunication)+		"\t"+
		String(input->packageSize)+					"\t"+
		String(input->sourceIdentity)+				"\t"+
		String(input->destinationIdentity)+			"\t"+
		String(input->packageCount)+				"\t"+
		String(input->packageTypeCode)+				"\t"+
		String(input->NTC_RAW_DATA)+				"\t"+
		String(input->PRESSURE_RAW_DATA)+			"\t"+
		String(input->PriorityState)+				"\t"+
		String(input->endOfTransmission)+			"\t"+
		String(input->longitudinalRedundancyCheck)+ "\t"
	);

	if (now - lastPacket >= 1000)
	{
		Serial.println("No connection");
	}

	lastPacket = millis();
}

void handleResponseReciever(InputData *input)
{
	unsigned long now = millis();

	uint32_t oldPackageCount;
	bool newPacket = (input->packageCount > oldPackageCount)||(input->packageCount == 0);

	if (input->packageTypeCode == PACKAGETYPE_COMMAND_RESET)
	{
		esp_restart;
	}
	if ((now-lastPacket) > 1000) 
	{
		createPacket(PACKAGETYPE_CALL_ACKNOWLEDGE);
		return;
	}
	if (!newPacket) 
	{
		return;
	}
	createPacket(input->packageTypeCode);
	lastPacket = now;
}

void createPacket(PACKAGETYPECODE type)
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

	currentInput.packageTypeCode = type;

	// Controleer of de input is veranderd
	bool inputChanged = memcmp(&currentInput, &previousInput, sizeof(InputData)) != 0;

	Serial.println(type);

	switch(type)
	{
		case PACKAGETYPE_RETRANSMIT:
			currentInput.packageCount = currentInput.packageCount+1;
			memset(packet.data, 0, sizeof(InputData));
			type = PACKAGETYPE_CALL_ACKNOWLEDGE;
			break;
		case PACKAGETYPE_DATA_SEND:
			// Verwerk gegevensverzending
			currentInput.packageCount = currentInput.packageCount+1;
			memset(packet.data, 0, sizeof(InputData));
			previousInput = currentInput;
			break;
		case PACKAGETYPE_CALL_STATE:
			// Verwerk oproepstatus
			break;
		case PACKAGETYPE_CALL_ACKNOWLEDGE:
			// Verwerk oproepbevestiging
			currentInput.packageCount = currentInput.packageCount+1;
			memset(packet.data, 0, sizeof(InputData));
			break;
	}
	
	// Verstuur het pakket als dat nodig is
	
	currentInput.startOfCommunication = 01;
	currentInput.packageSize = sizeof(Packet)-1;
	currentInput.sourceIdentity = 0x14;
	currentInput.destinationIdentity = 0x15;

	currentInput.packageTypeCode = type;
	currentInput.PriorityState = (inputChanged&&(currentInput.packageTypeCode == PACKAGETYPE_DATA_SEND));
	
	currentInput.endOfTransmission = 02;
	currentInput.longitudinalRedundancyCheck = networkReciever.checkLRCOutput(&packet);

	networkReciever.send(&packet);
}

// Handelt alle ESP-now paketten af
void handleNetwork(const uint8_t *mac, const Packet *packet)
{
	handleResponseReciever((InputData*)packet->data);
	printInput((InputData*)packet->data);
}