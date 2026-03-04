#include "function.h"

#include "Arduino.h"

//lib def - only for this file

#include "config.h"
// #include "Networking_by_B5.h"


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
    return 300;
}

int READ_PRESSURE() {
    //PRESSURE_SENSOR_PIN
    return 200;
}