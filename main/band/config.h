#pragma once

#include <Arduino.h>

#include <Networking_by_B5.h>

#define NTC_SENSOR_PIN 2
#define PRESSURE_SENSOR_PIN 3

typedef enum : uint8_t 
{
    NTC_DEAD_HIGH,
    NTC_DANGEROUS,
    NTC_TOO_HIGH,
    NTC_NORMAL,
    NTC_TOO_LOW,
    NTC_NO_SKIN_CONTACT,
    NTC_NO_REALISTIC_DATA
} NTC_RESULT;

typedef enum : uint8_t
{
    PRESSURE_BREATH_IN,
    PRESSURE_BREATH_OUT,
    PRESSURE_NO_SKIN_CONTACT,
    PRESSURE_NO_REALISTIC_DATA
} PRESSURE_RESULT;

struct SENSORS
{
    NTC_RESULT Ntc_result;
    PRESSURE_RESULT Pressure_result;
};