#pragma once

#include <Arduino.h>

#include <Networking_by_B5.h>

#include <FastLED.h>

#include <Wire.h>
#include "MAX30105.h"
#include "spo2_algorithm.h"

// Led builtin
#define PIN_LED_BAND 15

// analog sensors
#define NTC_SENSOR_PIN 1
#define PRESSURE_SENSOR_PIN 10

// ledstrip
#define NUM_LEDS  8
#define DATA_PIN  3 // DI verander naar goeie pin
#define CLOCK_PIN 5 // CI verander naar goeie pin

// MAX30105
#define ledBrightness 60
#define sampleAverage 4
#define ledMode 2
#define sampleRate 100
#define pulseWidth 411
#define adcRange 4096

typedef struct
{
    uint32_t irBuffer[100];
    uint32_t redBuffer[100];
    int32_t bufferLength;
    int32_t spo2;
    int8_t validSPO2;
    int32_t heartRate;
    int8_t validHeartRate;
} SRC_SENSORS;

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

struct SENSORS
{
    NTC_RESULT Ntc_result;
    PRESSURE_RESULT Pressure_result;
    HEARTBEAT_RESULT Heartbeat_result;
    SATURATION_RESULT Saturation_Result;
    uint16_t NTC_RAW_DATA;
    uint16_t PRESSURE_RAW_DATA;
    uint8_t HEARTBEAT_RAW_DATA;
    uint8_t SATURATION_RAW_DATA;
};