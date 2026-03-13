#include "function.h"

#include "Arduino.h"

//lib def - only for this file

#include "config.h"
#include "Networking_by_B5.h"

static const char *TAG = "MAIN";

unsigned long lastPacket;

int READ_NTC() {
    //NTC_SENSOR_PIN
    return analogRead(NTC_SENSOR_PIN);
}

int READ_PRESSURE() {
    //PRESSURE_SENSOR_PIN
    return analogRead(PRESSURE_SENSOR_PIN);
}

// RECIEVE
void printInput(InputData *input)
{
	Networking &networkBand = Networking::getInstance();
	unsigned long now = millis();

  	Serial.print(
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
		String(input->longitudinalRedundancyCheck)+ "\t"+
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
	
	printInput((InputData*)packet->data);
}


void createPacket(PACKAGETYPECODE type)
{
	static const char *TAG = "MAIN";

	Networking &networkBand = Networking::getInstance();
	Identity identityBand;
	
	SENSORS Sensors;

	static unsigned long lastInput = 0;
	static unsigned long lastHeartbeat = 0;
	
	unsigned long now = millis();

	static InputData previousInput;
	static InputData currentInput;

	static Packet packet = {
		.identity = networkBand.getIdentity(),
	};

	currentInput.packageTypeCode = type;

	// Controleer of de input is veranderd
	bool inputChanged = memcmp(&currentInput, &previousInput, sizeof(InputData)) != 0;
	
	switch(type)
	{
		case PACKAGETYPE_RETRANSMIT:
			// Verwerk hertransmissie
			// de data wordt niet veranderd - geen aanpassing
			break;
		case PACKAGETYPE_DATA_SEND:
			// Verwerk gegevensverzending

			currentInput.NTC_RAW_DATA = READ_NTC();
			currentInput.PRESSURE_RAW_DATA = READ_PRESSURE();

			// Kopieer de input data naar het pakket
			memcpy(packet.data, &currentInput, sizeof(InputData));

			previousInput = currentInput;
			lastInput = now;

			break;
		case PACKAGETYPE_COMMAND_RESET:
			// Verwerk resetcommando
			esp_restart();
			return;

			break;
		case PACKAGETYPE_CALL_STATE:
			// Verwerk oproepstatus
			break;
		case PACKAGETYPE_CALL_ACKNOWLEDGE:
			// Verwerk oproepbevestiging
			memset(packet.data, 0, sizeof(InputData));
		break;
	}
	
	// Verstuur het pakket als dat nodig is
	
	currentInput.startOfCommunication = 01;
	currentInput.packageSize = sizeof(Packet)-1;
	currentInput.sourceIdentity = 0x14;
	currentInput.destinationIdentity = 0x15;

	currentInput.packageCount = currentInput.packageCount+1;
	currentInput.packageTypeCode = type;
	currentInput.PriorityState = (inputChanged&&(currentInput.packageTypeCode == PACKAGETYPE_DATA_SEND));
	
	currentInput.endOfTransmission = 02;
	currentInput.longitudinalRedundancyCheck = networkBand.calculateLRCOutput(&packet);
	
	networkBand.send(&packet);
}