#include "function.h"

#include "Arduino.h"

//lib def - only for this file

#include "config.h"
#include "Networking_by_B5.h"

static const char *TAG = "MAIN";

// Netwerk class instansieren
// Networking &network = Networking::getInstance();
// Identiteit struct instansieren
// Identity identity;

unsigned long lastPacket;


int READ_NTC() {
    //NTC_SENSOR_PIN
    return digitalRead(NTC_SENSOR_PIN);
}

int READ_PRESSURE() {
    //PRESSURE_SENSOR_PIN
    return digitalRead(PRESSURE_SENSOR_PIN);
}

// void printMacAddress(){
//   uint8_t baseMac[6];
//   if (esp_wifi_get_mac(WIFI_IF_STA, baseMac) == ESP_OK) {
//     Serial.printf(
//             "%02x:%02x:%02x:%02x:%02x:%02x\n",
//             baseMac[0], baseMac[1], baseMac[2],baseMac[3], baseMac[4], baseMac[5]);
//   } else {
//     Serial.println("not found");
//   }
// }



// int encodePackage(int type) {


// uint8_t groupID = 0x15;

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


void createPacket(PACKAGETYPECODE type)
{
	static const char *TAG = "MAIN";

	Networking &networkBand = Networking::getInstance();
	Identity identityBand;
	
	SENSORS Sensors;
	
	currentInput.packageTypeCode = type;

	static unsigned long lastInput = 0;
	static unsigned long lastHeartbeat = 0;
	
	unsigned long now = millis();

	static InputData previousInput;
	static InputData currentInput;

	static Packet packet = {
		.identity = networkBand.getIdentity(),
	};

	bool shouldUpdate = true;

	// Stel de input data in
	if ((Sensors.Ntc_result != NTC_NO_REALISTIC_DATA)&&(Sensors.Pressure_result != PRESSURE_NO_REALISTIC_DATA)&&shouldUpdate)
	{
		currentInput.NTC_RAW_DATA = READ_NTC();
		currentInput.PRESSURE_RAW_DATA = READ_PRESSURE();

		// Kopieer de input data naar het pakket
		memcpy(packet.data, &currentInput, sizeof(InputData));
	}
	else
	{
		// Geen realistische data, stuur lege data
	}

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

			break;
		case PACKAGETYPE_COMMAND_RESET:
			// Verwerk resetcommando
			
			break;
		case PACKAGETYPE_CALL_STATE:
			// Verwerk oproepstatus
			break;
		case PACKAGETYPE_CALL_ACKNOWLEDGE:
			// Verwerk oproepbevestiging
			memset(packet.data, 0, sizeof(InputData));
			break;
		default:
			// Onbekend pakkettype
			memset(packet.data, 0, sizeof(InputData));
			break;
	}
	
	// Verstuur het pakket als dat nodig is
	if (shouldUpdate)
	{
		currentInput.startOfCommunication = 01;
		currentInput.packageSize = sizeof(Packet);
		currentInput.packageCount = currentInput.packageCount+1;

		if ((inputChanged)&&(currentInput.packageTypeCode == PACKAGETYPE_DATA_SEND))
		{
			currentInput.PriorityState = true;
		}

		networkBand.send(&packet);
		previousInput = currentInput;

		lastInput = now;

		currentInput.sourceIdentity = 0x15;
		currentInput.destinationIdentity = 0x14;
		currentInput.packageTypeCode = PACKAGETYPE_DATA_SEND;
	}
}