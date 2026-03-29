#pragma once

#include <Arduino.h>

#include <MCUFRIEND_kbv.h>
#include <Adafruit_GFX.h>
#include <SoftwareSerial.h>

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
  uint8_t TEMPERATURE;
  uint8_t BREATHRATE;
  uint8_t HEARTRATE;
  uint8_t SATURATION;
  uint16_t NTC_RAW_DATA;
  uint16_t PRESSURE_RAW_DATA;
  uint16_t HEARTBEAT_RAW_DATA;
  uint16_t SATURATION_RAW_DATA;
} SENSORS;

SENSORS data;
#define START_BYTE 0xAA
SoftwareSerial mySerial(12, 11); // RX, TX

MCUFRIEND_kbv tft;

// Kleuren
#define ZWART     0x0000
#define WIT       0xFFFF
#define ROOD      0xF800
#define GROEN     0x07E0
#define BLAUW     0x001F
#define GRIJS     0x8410
#define DGGRIJS   0x2104
#define ROZE      0xF813
#define LICHTROZE 0xFF18

// Grafiek instellingen
#define GRAFIEK_X   35
#define GRAFIEK_Y   20
#define GRAFIEK_B   130
#define GRAFIEK_H   200

#define SCHERM_B    240
#define SCHERM_H    320

// Sinus variabelen
int sinusX   = 0;
int vorigeY  = 0;

// UART buffer
bool receiving = false;
uint8_t buf[sizeof(SENSORS)];
uint8_t index = 0;