#pragma once

#include <Arduino.h>

#include <Networking_by_B5.h>

#include <FastLED.h>

#include <Wire.h>
#include "MAX30105.h"
#include "heartRate.h"
#include "spo2_algorithm.h"

// Led builtin
#define PIN_LED_BAND 2

// analog sensors
#define NTC_SENSOR_PIN 32
#define PRESSURE_SENSOR_PIN 33

// ledstrip
#define NUM_LEDS  8
#define DATA_PIN  25 // DI verander naar goeie pin
#define CLOCK_PIN 26 // CI verander naar goeie pin

// MAX30105
#define ledBrightness 60
#define sampleAverage 4
#define ledMode 2
#define sampleRate 100
#define pulseWidth 411
#define adcRange 4096

typedef struct
{
    const byte RATE_SIZE = 4; //Increase this for more averaging. 4 is good.
    byte rates[4]; //Array of heart rates
    byte rateSpot = 0;
    long lastBeat = 0; //Time at which the last beat occurred

    float beatsPerMinute;
    int beatAvg;
    int redLed;
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
    PRESSURE_TOO_SLOW,
    PRESSURE_SLOW,
    PRESSURE_NORMAL,
    PRESSURE_FAST,
    PRESSURE_TOO_FAST,
    PRESSURE_NO_SKIN_CONTACT,
    PRESSURE_NO_REALISTIC_DATA
} PRESSURE_RESULT;

typedef enum : uint8_t
{
    HEARTBEAT_DEAD_HIGH,
    HEARTBEAT_PROBLEMATICALLY_HIGH,
    HEARTBEAT_HIGH,
    HEARTBEAT_NORMAL,
    HEARTBEAT_LOW,
    HEARTBEAT_PROBLEMATICALLY_LOW,
    HEARTBEAT_DEAD_LOW,
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