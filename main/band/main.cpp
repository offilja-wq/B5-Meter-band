#include "Arduino.h"

#include "config.h"
#include "function.h"
#include "Networking_by_B5.h"

#include <WiFi.h>
#include <esp_wifi.h>
#include "esp_bt.h"
#include "esp_now.h"


void setup() {
  Serial.begin(115200);
  Serial.setDebugOutput(true);

  identity.type = IDENTITY_BAND;


  WiFi.begin();

  Serial.print("[DEFAULT] ESP32 Board MAC Address: ");

}

void loop() {
    //printMacAddress();
    network.handle();
}