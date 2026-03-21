#include "function.h"

#include "Arduino.h"

//lib def - only for this file

#include "config.h"
#include "Networking_by_B5.h"

static const char *TAG = "MAIN";

int READ_NTC()
{
    // Lees NTC
    return analogRead(NTC_SENSOR_PIN);
}

int READ_PRESSURE()
{
    // Lees Pressure
    return analogRead(PRESSURE_SENSOR_PIN);
}

int READ_HEARTBEAT()
{
	// Lees Heartbeat
	return 0;
}

int READ_SATURATION()
{
	// Lees Saturation
	return 0;
}

void setStrip(int i, uint8_t RED, uint8_t GREEN, uint8_t BLUE)
{

}

void updateStrip()
{

}

// RECIEVE
void handleResponseBand(InputData *input)
{
	Networking &networkBand = Networking::getInstance();
	Pinout pinoutBand = networkBand.getPinout();

	unsigned long now = millis();
	uint32_t oldPackageCount;

	// Check of er echt een nieuw pakket is
	bool newPacket = (input->packageCount > oldPackageCount);
	
	if ((!newPacket)||((now-networkBand.lastPacket) > 1000)) 
	{	
		createPacket(PACKAGETYPE_CALL_ACKNOWLEDGE);
		digitalWrite(pinoutBand.PIN_LED, 0);
		return;
	} else {
		digitalWrite(pinoutBand.PIN_LED, (((now+networkBand.lastPacket)/500)%2));
	}

	networkBand.lastPacket = now;

	// Reageer op het pakket
	switch(input->packageTypeCode)
	{
		case PACKAGETYPE_DATA_SEND:
			// Verwerk data
			createPacket(PACKAGETYPE_CALL_ACKNOWLEDGE);
			break;
		case PACKAGETYPE_COMMAND_RESET:
			// Reset
			esp_restart;
			break;
		case PACKAGETYPE_CALL_ACKNOWLEDGE:
			// Verwerk oproepbevestiging
			createPacket(PACKAGETYPE_DATA_SEND);
			break;
		default:
			createPacket(input->packageTypeCode);
			break;
	}

	oldPackageCount = input->packageCount;
}

// TRANSMIT
void createPacket(PACKAGETYPECODE type)
{
	static const char *TAG = "MAIN";

	Networking &networkBand = Networking::getInstance();
	Identity identityBand;

	static InputData previousInput;
	static InputData currentInput;

	static Packet packet = {
		.identity = networkBand.getIdentity(),
	};

	currentInput.packageTypeCode = type;

	// Controleer of de input is veranderd
	bool inputChanged = memcmp(&currentInput, &previousInput, sizeof(InputData)) != 0;

	currentInput.startOfCommunication = 01;
	currentInput.packageSize = sizeof(Packet)-1;
	currentInput.sourceIdentity = 0x14;
	currentInput.destinationIdentity = 0x15;

	currentInput.PriorityState = (inputChanged&&(currentInput.packageTypeCode == PACKAGETYPE_DATA_SEND));
	
	currentInput.endOfTransmission = 02;
	currentInput.longitudinalRedundancyCheck = networkBand.checkLRCOutput(&packet);

	switch(type)
	{
		case PACKAGETYPE_RETRANSMIT:
			// Verwerk hertransmissie
			// de data wordt niet veranderd - geen aanpassing
			currentInput.packageTypeCode = PACKAGETYPE_DATA_SEND;
			break;
		case PACKAGETYPE_DATA_SEND:
			// Verwerk gegevensverzending
			currentInput.NTC_RAW_DATA = READ_NTC();
			currentInput.PRESSURE_RAW_DATA = READ_PRESSURE();
			currentInput.HEARTBEAT_RAW_DATA = READ_HEARTBEAT();
			currentInput.SATURATION_RAW_DATA = READ_SATURATION();

			currentInput.packageCount = currentInput.packageCount+1;
			currentInput.packageTypeCode = type;

			previousInput = currentInput;
			break;
		case PACKAGETYPE_CALL_STATE:
			// Verwerk oproepstatus
			break;
		case PACKAGETYPE_CALL_ACKNOWLEDGE:
			// Verwerk oproepbevestiging
			currentInput.packageCount = currentInput.packageCount+1;
			currentInput.packageTypeCode = type;
			break;
	}

	// Kopieer de input data naar het pakket
	memcpy(packet.data, &currentInput, sizeof(InputData));
	networkBand.send(&packet);
}

void updateStrip(InputData *input)
{
	unsigned long now = millis();
}

// Handelt alle ESP-now paketten af
void handleNetwork(const uint8_t *mac, const Packet *packet)
{
	Networking &networkBand = Networking::getInstance();

	//Local
	handleResponseBand((InputData*)packet->data);
	updateStrip((InputData*)packet->data);
}