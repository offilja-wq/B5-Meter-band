#include "function.h"

#include "Arduino.h"

//lib def - only for this file

#include "config.h"
// #include "Networking_by_B5.h"


int READ_NTC() {
    //NTC_SENSOR_PIN
    return digitalRead(NTC_SENSOR_PIN);
}

int READ_PRESSURE() {
    //PRESSURE_SENSOR_PIN
    return digitalRead(PRESSURE_SENSOR_PIN);
}

// int encodePackage(int type) {


// uint8_t groupID = 0x15;

// char package[100] = {0};

// package[0]= 0;

// }
