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


// int READ_NTC() {
//     //NTC_SENSOR_PIN
//     return digitalRead(NTC_SENSOR_PIN);
// }

// int READ_PRESSURE() {
//     //PRESSURE_SENSOR_PIN
//     return digitalRead(PRESSURE_SENSOR_PIN);
// }

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

// char package[100] = {0};

// package[0]= 0;

// }


int READ_NTC() {
    //NTC_SENSOR_PIN
    return 101;
}

int READ_PRESSURE() {
    //PRESSURE_SENSOR_PIN
    return 202;
}

// RECIEVE
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
	
	printInput((InputData *)packet->data);

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
		Serial.print("No connection\n");
	}
}