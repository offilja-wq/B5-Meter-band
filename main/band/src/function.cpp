#include "function.h"

#include "Arduino.h"

//lib def - only for this file

#include "config.h"
#include "Networking_by_B5.h"


int READ_NTC() {
    //NTC_SENSOR_PIN
    
    return 0; //data
}

int READ_PRESSURE() {
    //PRESSURE_SENSOR_PIN
    
    return 0; //data
}

void printMacAddress(){
  uint8_t baseMac[6];
  if (esp_wifi_get_mac(WIFI_IF_STA, baseMac) == ESP_OK) {
    Serial.printf(
            "%02x:%02x:%02x:%02x:%02x:%02x\n",
            baseMac[0], baseMac[1], baseMac[2],baseMac[3], baseMac[4], baseMac[5]);
  } else {
    Serial.println("not found");
  }
}
