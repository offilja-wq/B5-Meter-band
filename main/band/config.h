#pragma once

#include <Arduino.h>

#include <Networking_by_B5.h>

#define NTC_SENSOR_PIN 34
#define PRESSURE_SENSOR_PIN 35

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

















// typedef enum : uint8_t //Geeft "IdentityType" de optie om 'statussen' te hebben.
// {
//     IDENTITY_BAND,
//     IDENTITY_RECIEVER,
//     IDENTITY_OBSERVER
// } IdentityType;

// typedef struct
// {
//     Identity identity;

//     int16_t NTC_RAW_DATA;
//     int16_t PRESSURE_RAW_DATA;

//     CommandType command;
//     uint8_t data[16]; //Inhoud
// } Packet;



