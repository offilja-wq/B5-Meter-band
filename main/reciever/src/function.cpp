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

	lastPacket = now;
}

void handleResponseReciever(InputData *input)
{
	SENSORS Sensors;
	
	unsigned long now = millis();

	uint32_t oldPackageCount;
	bool ResetSend;
	bool newPacket = (input->packageCount > oldPackageCount);

	if (newPacket)
	{
		lastPacket = now;
		ResetSend = false;
		oldPackageCount = input->packageCount;
		digitalWrite(INTERNAL_LED_BLUE, (((now+lastPacket)/500)%2));
	} else {
		digitalWrite(INTERNAL_LED_BLUE, !newPacket);
		createPacket(PACKAGETYPE_CALL_ACKNOWLEDGE);
	}

	if ((now-lastPacket) > 1000) 
	{
		if(!ResetSend)
		{
			createPacket(PACKAGETYPE_COMMAND_RESET);
			ResetSend = true;
		}

		delay(50);
		createPacket(PACKAGETYPE_CALL_ACKNOWLEDGE);
		return;
	}

	if ((Sensors.Ntc_result == NTC_NO_REALISTIC_DATA)||
		(Sensors.Pressure_result == PRESSURE_NO_REALISTIC_DATA))
		// WIP
	{
		createPacket(PACKAGETYPE_RETRANSMIT);
		return;
	}

	switch (input->packageTypeCode)
	{
		case PACKAGETYPE_DATA_SEND:
			createPacket(PACKAGETYPE_CALL_ACKNOWLEDGE);
			break;
		case PACKAGETYPE_COMMAND_RESET:
			esp_restart;
			break;
		case PACKAGETYPE_CALL_ACKNOWLEDGE:
			createPacket(PACKAGETYPE_DATA_SEND);
			break;
		default:
			createPacket(input->packageTypeCode);
			break;
	}
}

void createPacket(PACKAGETYPECODE type)
{
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

	// Controleer of de input is veranderd
	bool inputChanged = memcmp(&currentInput, &previousInput, sizeof(InputData)) != 0;

	currentInput.startOfCommunication = 01;
	currentInput.packageSize = sizeof(Packet)-1;
	currentInput.sourceIdentity = 0x14;
	currentInput.destinationIdentity = 0x15;
	currentInput.packageCount = currentInput.packageCount+1;

	currentInput.packageTypeCode = type;
	currentInput.PriorityState = (inputChanged&&(currentInput.packageTypeCode == PACKAGETYPE_DATA_SEND));
	
	currentInput.endOfTransmission = 02;
	currentInput.longitudinalRedundancyCheck = networkReciever.checkLRCOutput(&packet);
	
	switch(type)
	{
		case PACKAGETYPE_RETRANSMIT:
			currentInput.packageTypeCode = PACKAGETYPE_CALL_ACKNOWLEDGE;
			break;
		case PACKAGETYPE_DATA_SEND:
			// Verwerk gegevensverzending
			previousInput = currentInput;
			break;
		case PACKAGETYPE_CALL_STATE:
			// Verwerk oproepstatus
			// WIP
			currentInput.packageTypeCode = PACKAGETYPE_CALL_ACKNOWLEDGE;
			break;
	}

	memcpy(packet.data, &currentInput, sizeof(InputData));
	networkReciever.send(&packet);
}

void updateDisplay(InputData *input)
{
	unsigned long now = millis();
}

void updateStrip(InputData *input)
{
	unsigned long now = millis();
}

// Handelt alle ESP-now paketten af
void handleNetwork(const uint8_t *mac, const Packet *packet)
{
	//Local
	handleResponseReciever((InputData*)packet->data);
	printInput((InputData*)packet->data);
	updateDisplay((InputData*)packet->data);
	updateStrip((InputData*)packet->data);
}