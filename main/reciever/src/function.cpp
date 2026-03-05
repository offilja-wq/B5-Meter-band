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

