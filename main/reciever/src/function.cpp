#include "function.h"

#include "Arduino.h"

//lib def - only for this file

#include "config.h"
#include "Networking_by_B5.h"

static const char *TAG = "MAIN";

SENSORS concludeSensors(InputData *input)
{
	SENSORS Sensors;
	
	// Input
	input->NTC_RAW_DATA;
	input->PRESSURE_RAW_DATA;
	input->HEARTBEAT_RAW_DATA;
	input->SATURATION_RAW_DATA;

	// Output
	Sensors.Ntc_result;
	Sensors.Pressure_result;
	Sensors.Heartbeat_result;
	Sensors.Saturation_Result;
	
	switch (input->NTC_RAW_DATA)
	{
	case 43 ... 70:
		Sensors.Ntc_result = NTC_DEAD_HIGH;
		break;
	case 40 ... 42:
		Sensors.Ntc_result = NTC_DANGEROUS;
		break;
	case 38 ... 39:
		Sensors.Ntc_result = NTC_TOO_HIGH;
		break;
	case 35 ... 37:
		Sensors.Ntc_result = NTC_NORMAL;
		break;
	case 25 ... 34:
		Sensors.Ntc_result = NTC_TOO_LOW;
		break;
	default:
		Sensors.Ntc_result = NTC_NO_REALISTIC_DATA;
		break;
	}

	switch (input->PRESSURE_RAW_DATA) // INPUT AANPASSEN NAAR JUISTE WAARDEN
	{
	case 500 ... 700:
		Sensors.Pressure_result = PRESSURE_BREATH_IN;
		break;
	case 100 ... 499:
		Sensors.Pressure_result = PRESSURE_BREATH_OUT;
		break;
	default:
		Sensors.Pressure_result = PRESSURE_NO_SKIN_CONTACT;
		break;
	}

		switch (input->HEARTBEAT_RAW_DATA) // INPUT AANPASSEN NAAR JUISTE WAARDEN
	{
	case 181 ... 220:
		Sensors.Heartbeat_result = HEARTBEAT_DEADLY_HIGH;
		break;
	case 151 ... 180:
		Sensors.Heartbeat_result = HEARTBEAT_PROBLEMATICALLY_HIGH;
		break;
	case 101 ... 150:
		Sensors.Heartbeat_result = HEARTBEAT_HIGH;
		break;
	case 60 ... 100:
		Sensors.Heartbeat_result = HEARTBEAT_NORMAL;
		break;
	case 40 ... 59:
		Sensors.Heartbeat_result = HEARTBEAT_LOW;
		break;
	case 30 ... 39:
		Sensors.Heartbeat_result = HEARTBEAT_PROBLEMATICALLY_LOW;
		break;
	case 5 ... 29:
		Sensors.Heartbeat_result = HEARTBEAT_DEADLY_LOW;
		break;
	case 0 ... 4:
		Sensors.Heartbeat_result = HEARTBEAT_NO_SKIN_CONTACT;
		break;
	default:
		Sensors.Heartbeat_result = HEARTBEAT_NO_REALISTIC_DATA;
		break;
	}

	switch (input->SATURATION_RAW_DATA) // INPUT AANPASSEN NAAR JUISTE WAARDEN
	{
	case 61 ... 70:
		Sensors.Saturation_Result = SATURATION_TOO_HIGH;
		break;
	case 51 ... 60:
		Sensors.Saturation_Result = SATURATION_HIGH;
		break;
	case 41 ... 50:
		Sensors.Saturation_Result = SATURATION_NORMAL;
		break;
	case 31 ... 40:
		Sensors.Saturation_Result = SATURATION_LOW;
		break;
	case 11 ... 20:
		Sensors.Saturation_Result = SATURATION_TOO_LOW;
		break;
	case 10 ... 0:
		Sensors.Saturation_Result = SATURATION_NO_SKIN_CONTACT;
		break;
	default:
		Sensors.Saturation_Result = SATURATION_NO_REALISTIC_DATA;
		break;
	}
	
	return Sensors;
}

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

	if (now - networkReciever.lastPacket >= 1000)
	{
		Serial.println("No connection");
	}
}

void handleResponseReciever(InputData *input)
{
	Networking &networkReciever = Networking::getInstance();
	Pinout pinoutReciever = networkReciever.getPinout();
	
	unsigned long now = millis();

	uint32_t oldPackageCount;
	bool ResetSend;
	bool newPacket = (input->packageCount > oldPackageCount);

	if (newPacket)
	{
		networkReciever.lastPacket = now;
		ResetSend = false;
		oldPackageCount = input->packageCount;
		digitalWrite(pinoutReciever.PIN_LED, (((now+networkReciever.lastPacket)/500)%2));
	} else {
		digitalWrite(pinoutReciever.PIN_LED, 0);
		createPacket(PACKAGETYPE_CALL_ACKNOWLEDGE);
	}

	if ((now-networkReciever.lastPacket) > 1000) 
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

	SENSORS Sensors = concludeSensors(input);

	if ((Sensors.Ntc_result == NTC_NO_REALISTIC_DATA)||
		(Sensors.Pressure_result == PRESSURE_NO_REALISTIC_DATA)||
		(Sensors.Heartbeat_result == HEARTBEAT_NO_REALISTIC_DATA)||
    	(Sensors.Saturation_Result == SATURATION_NO_REALISTIC_DATA))
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

	networkReciever.lastPacket = now;
}

// TRANSMIT
void createPacket(PACKAGETYPECODE type)
{
	Networking &networkReciever = Networking::getInstance();
	Identity identityReciever;

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