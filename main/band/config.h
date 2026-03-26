#pragma once

#include <Arduino.h>

#include <Networking_by_B5.h>

#include <FastLED.h>

#define PIN_LED_BAND 15

#define NTC_SENSOR_PIN 2
#define PRESSURE_SENSOR_PIN 3

#define NUM_LEDS 8
#define DATA_PIN 39
#define CLOCK_PIN 40

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

typedef enum : uint8_t
{
    HEARTBEAT_DEADLY_HIGH,
    HEARTBEAT_PROBLEMATICALLY_HIGH,
    HEARTBEAT_HIGH,
    HEARTBEAT_NORMAL,
    HEARTBEAT_LOW,
    HEARTBEAT_PROBLEMATICALLY_LOW,
    HEARTBEAT_DEADLY_LOW,
    HEARTBEAT_NO_SKIN_CONTACT,
    HEARTBEAT_NO_REALISTIC_DATA
} HEARTBEAT_RESULT;

typedef enum : uint8_t
{
    SATURATION_TOO_HIGH,
    SATURATION_HIGH,
    SATURATION_NORMAL,
    SATURATION_LOW,
    SATURATION_TOO_LOW,
    SATURATION_NO_SKIN_CONTACT,
    SATURATION_NO_REALISTIC_DATA
} SATURATION_RESULT;

typedef struct
{
    NTC_RESULT Ntc_result;
    PRESSURE_RESULT Pressure_result;
    HEARTBEAT_RESULT Heartbeat_result;
    SATURATION_RESULT Saturation_Result;
    uint16_t NTC_RAW_DATA;              // Place Filler
    uint16_t PRESSURE_RAW_DATA;         // Place Filler
    uint16_t HEARTBEAT_RAW_DATA;         // Place Filler
    uint16_t SATURATION_RAW_DATA;       // Place Filler
} SENSORS;