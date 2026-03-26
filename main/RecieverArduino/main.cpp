#include "Arduino.h"

#include "config.h"
#include "function.h"

#include <MCUFRIEND_kbv.h>
#include <Adafruit_GFX.h>
#include <SoftwareSerial.h>

MCUFRIEND_kbv tft;

typedef struct 
{
  uint16_t NTC_RAW_DATA;
  uint16_t PRESSURE_RAW_DATA;
  uint16_t HEARTBEAT_RAW_DATA;
  uint16_t SATURATION_RAW_DATA;
} InputData;

CRGB leds[NUM_LEDS] = {0};     // Software gamma mode.

#include <SoftwareSerial.h>
SoftwareSerial mySerial(11, 12);

InputData data;
#define START_BYTE 0xAA

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

// Potmeter
#define POTMETER_PIN A5
float potWaarde = 0.0;

// Sinus variabelen
int sinusX   = 0;
int vorigeY  = 0;
float hoek   = 0.0;

// =====================
//   BLOKKEN UPDATEN
// =====================
void updateBlokken(InputData local) {
  
  // Blok 1 - Hartslag (potmeter)
  tft.fillRect(191, 38, 75, 12, ZWART);
  tft.setTextColor(LICHTROZE);
  tft.setTextSize(1);
  tft.setCursor(194, 39);
  tft.print(local.PRESSURE_RAW_DATA, 0);
  tft.print(" %");

  // Blok 2 - Ademhaling (leeg)
  tft.fillRect(191, 120, 75, 12, ZWART);
  tft.setTextColor(LICHTROZE);
  tft.setCursor(194, 122);
  tft.print("--");

  // Blok 3 - Temperatuur (leeg)
  tft.fillRect(191, 193, 75, 12, ZWART);
  tft.setTextColor(LICHTROZE);
  tft.setCursor(194, 195);
  tft.print("--");
}

// =====================
//   RASTER
// =====================
void tekenRaster() {
  for (int i = 1; i < 5; i++) {
    int y = GRAFIEK_Y + (GRAFIEK_H / 5) * i;
    tft.drawFastHLine(GRAFIEK_X, y, GRAFIEK_B, GRIJS);
  }
  for (int i = 1; i < 4; i++) {
    int x = GRAFIEK_X + (GRAFIEK_B / 4) * i;
    tft.drawFastVLine(x, GRAFIEK_Y, GRAFIEK_H, GRIJS);
  }
}

// =====================
//   GRAFIEK (links)
// =====================
void tekenGrafiekOutline() {
  tft.fillRect(0, 0, 182, SCHERM_H, LICHTROZE);

  tekenRaster();

  // Grafiek kader
  tft.drawRect(GRAFIEK_X, GRAFIEK_Y, GRAFIEK_B, GRAFIEK_H, ZWART);

  // Y-as labels
  tft.setTextColor(ZWART);
  tft.setTextSize(1);
  for (int i = 0; i <= 5; i++) {
    int y = GRAFIEK_Y + (GRAFIEK_H / 5) * i;
    int waarde = 100 - (i * 20);
    tft.setCursor(0, y - 3);
    if (waarde < 100) tft.print(" ");
    tft.print(waarde);
  }

  // X-as labels
  tft.setCursor(GRAFIEK_X, GRAFIEK_Y + GRAFIEK_H + 5);
  tft.print("0");
  tft.setCursor(GRAFIEK_X + GRAFIEK_B - 6, GRAFIEK_Y + GRAFIEK_H + 5);
  tft.print("t");

  // Titel
  tft.setTextColor(ROZE);
  tft.setTextSize(1);
  tft.setCursor(GRAFIEK_X + 25, 8);
  tft.print("GRAFIEK");

  // Scheidingslijn
  tft.drawFastVLine(183, 0, SCHERM_H, BLAUW);
}

// =====================
//   RECHTERKANT
// =====================
void tekenRechterkant() {
  tft.setTextColor(ROZE);
  tft.setTextSize(1);
  tft.setCursor(190, 9);
  tft.print("EXTRA INFO -");

  tft.drawRect(190, 25,  78, 40, GRIJS);
  tft.drawRect(190, 104, 78, 40, GRIJS);
  tft.drawRect(190, 180, 78, 40, GRIJS);

  tft.setTextColor(ROZE);
  tft.setCursor(193, 29);  tft.print("Hartslag:");
  tft.setCursor(193, 110); tft.print("Ademhaling:");
  tft.setCursor(193, 184); tft.print("Temperatuur:");
}

void setup() {
  Serial.begin(9600);
   mySerial.begin(9600);

  uint16_t ID = tft.readID();
  Serial.print("Scherm ID: 0x");
  Serial.println(ID, HEX);

  if (ID == 0xD3D3) ID = 0x9486;
  tft.begin(ID);
  tft.setRotation(3);
  tft.fillScreen(ZWART);

  tekenGrafiekOutline();
  tekenRechterkant();

  // Startpositie sinus
  sinusX  = GRAFIEK_X + 1;
  vorigeY = GRAFIEK_Y + (GRAFIEK_H / 2);
}

InputData ReadData () {
  if (mySerial.available()) 
  {
    if (mySerial.read() == START_BYTE)
    {
      while (mySerial.available() < sizeof(data));
      mySerial.readBytes((uint8_t*)&data, sizeof(data));
      // updateDisplay(data);
        //  nu aangeroepen in loop dus blijft constant updaten
      // Serial.println(data.PRESSURE_RAW_DATA);
      return data;
    }
  }
}

void loop() {
  // Potmeter uitlezen en blokken updaten
  //potWaarde = map(analogRead(POTMETER_PIN), 0, 1023, 0, 100);

      updateBlokken(ReadData());
    Serial.println(data.PRESSURE_RAW_DATA);
//  updateBlokken(data);  //  nu aangeroepen in loop dus blijft constant updaten

  // Sinus berekenen
  int middenY   = GRAFIEK_Y + (GRAFIEK_H / 2);
  // int amplitude = (GRAFIEK_H / 2) - 70;
  int amplitude = map(data.PRESSURE_RAW_DATA, 0, 1023, 0, ((GRAFIEK_H / 2) - 5));  
  int nieuweY   = middenY - (int)(sin(hoek) * amplitude);

  // Lijn tekenen
  tft.drawLine(sinusX - 1, vorigeY, sinusX, nieuweY, ROOD);

  vorigeY = nieuweY;
  hoek   += 0.15;
  sinusX++;

  // Reset als einde grafiek bereikt
  if (sinusX > GRAFIEK_X + GRAFIEK_B - 1) {
    sinusX  = GRAFIEK_X + 1;
    hoek    = 0.0;
    vorigeY = middenY;

    tft.fillRect(GRAFIEK_X + 1, GRAFIEK_Y + 1, GRAFIEK_B - 2, GRAFIEK_H - 2, LICHTROZE);
    tekenRaster();
  }

  delay(10);
}
