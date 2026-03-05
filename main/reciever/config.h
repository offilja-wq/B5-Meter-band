#pragma once

#include <Arduino.h>

typedef struct //Data holders
{
    int16_t NTC_RAW_DATA;
    int16_t PRESSURE_RAW_DATA;
} InputData2;

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

typedef enum : uint8_t
{
    PACKAGETYPE_RETRANSMIT = 01,
    PACKAGETYPE_DATA_SEND = 02,
    PACKAGETYPE_COMMAND_RESET = 03,
    PACKAGETYPE_CALL_STATE = 04,
    PACKAGETYPE_CALL_ACKNOWLEDGE = 05

} packageTypeCode;


typedef enum : uint8_t {


} packageFuction;





typedef struct //Verander types in Package.[name]
{
    
    uint8_t packageSize;
    uint8_t sourceIdentity;
    uint8_t destinationIdentity;
    uint32_t packageCount;
    uint8_t packageTypeCode;




    bool priorityState;


} Package;
