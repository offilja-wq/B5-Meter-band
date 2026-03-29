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

void setup() 
{
  Serial.begin(115200);
  mySerial.begin(9600); // 9600, kut met 115200

  uint16_t ID = tft.readID();
  // Serial.print("Scherm ID: 0x");
  // Serial.println(ID, HEX);
  
  if (ID == 0xD3D3) ID = 0x9486;
  tft.begin(ID);
  tft.setRotation(1);
  tft.fillScreen(ZWART);

  tekenGrafiekOutline();
  tekenRechterkant();

  // Startpositie sinus
  sinusX  = GRAFIEK_X + 1;
  vorigeY = GRAFIEK_Y + (GRAFIEK_H / 2);
}

void loop() 
{
  ontvangData();
  updateBlokken();

  // Sensor waarde schalen naar scherm
  int nieuweY = map(data.PRESSURE_RAW_DATA, 0, 700, GRAFIEK_Y + GRAFIEK_H - 2, GRAFIEK_Y + 2);

  // Lijn tekenen
  tft.drawLine(sinusX - 1, vorigeY, sinusX, nieuweY, ROOD);

  // Update vorige punt
  vorigeY = nieuweY;

  // Volgende X positie
  sinusX++;

  // Reset als einde grafiek bereikt
  if (sinusX > GRAFIEK_X + GRAFIEK_B - 1) 
  {
    sinusX  = GRAFIEK_X + 1;

    tft.fillRect(GRAFIEK_X + 1, GRAFIEK_Y + 1, GRAFIEK_B - 2, GRAFIEK_H - 2, LICHTROZE);
    tekenRaster();
  }
}

// UART data lezen
void ontvangData()
{
  while (mySerial.available())
  {
    uint8_t b = mySerial.read();
    // Serial.println(receiving); // debug
    if (!receiving)
    {
      if (b == START_BYTE)
      {
        receiving = true;
        index = 0;
      }
    }
    else
    {
      buf[index++] = b;
      if (index >= sizeof(SENSORS))
      {
        memcpy(&data, buf, sizeof(SENSORS));
        receiving = false;
        // Serial.println(data.PRESSURE_RAW_DATA); // debug
      }
    }
  }
}

// Update blokken
void updateBlokken() 
{
  tft.setTextColor(LICHTROZE);
  tft.setTextSize(2);

  // Blok 1 - Hartslag
  tft.fillRect(191, 40, 98, 14, ZWART);
  tft.setCursor(194, 40);
  tft.print(data.HEARTBEAT_RAW_DATA);
  tft.print(" BPM");

  // Blok 2 - Saturatie
  tft.fillRect(191, 93, 98, 14, ZWART);
  tft.setCursor(194, 93);
  tft.print(data.PRESSURE_RAW_DATA); // nu tijdelijk als pressure om te testen
  tft.print(" %");

  // Blok 3 - Temperatuur
  tft.fillRect(191, 146, 98, 14, ZWART);
  tft.setCursor(194, 146);
  tft.print(data.NTC_RAW_DATA);
  tft.print(" C");

  // Blok 4 - Ademhaling
  tft.fillRect(191, 199, 98, 14, ZWART);
  tft.setCursor(194, 199);
  tft.print(data.SATURATION_RAW_DATA); // tijdelijk als saturatie om te testen
  tft.print(" B/M");
}

// Raster
void tekenRaster() 
{
  for (int i = 1; i < 5; i++) 
  {
    int y = GRAFIEK_Y + (GRAFIEK_H / 5) * i;
    tft.drawFastHLine(GRAFIEK_X, y, GRAFIEK_B, GRIJS);
  }
  for (int i = 1; i < 4; i++) 
  {
    int x = GRAFIEK_X + (GRAFIEK_B / 4) * i;
    tft.drawFastVLine(x, GRAFIEK_Y, GRAFIEK_H, GRIJS);
  }
}

// Grafiek
void tekenGrafiekOutline() 
{
  tft.fillRect(0, 0, 182, SCHERM_H, LICHTROZE);
  
  tekenRaster();

  // Grafiek kader
  tft.drawRect(GRAFIEK_X, GRAFIEK_Y, GRAFIEK_B, GRAFIEK_H, ZWART);

  // Y-as labels
  tft.setTextColor(ZWART);
  tft.setTextSize(1);
  for (int i = 0; i <= 5; i++) 
  {
    int y = GRAFIEK_Y + (GRAFIEK_H / 5) * i;
    int waarde = 100 - (i * 20);
    tft.setCursor(10, y - 3);
    tft.print(waarde);
  }

  // X-as labels
  tft.setCursor(GRAFIEK_X - 2, GRAFIEK_Y + GRAFIEK_H + 6); 
  tft.print("0");
  tft.setCursor(GRAFIEK_X + 32, GRAFIEK_Y + GRAFIEK_H + 6); 
  tft.print("tijd in sec");
  tft.setCursor(GRAFIEK_X + GRAFIEK_B - 6, GRAFIEK_Y + GRAFIEK_H + 6); 
  tft.print("10");

  // Titel
  tft.setTextColor(ROZE);
  tft.setCursor(GRAFIEK_X + 34, 8); 
  tft.print("Ademhaling");

  // Scheidingslijn
  tft.drawFastVLine(183, 0, SCHERM_H, BLAUW);
}

// Rechterkant
void tekenRechterkant() 
{
  tft.setTextColor(ROZE);
  tft.setTextSize(1);
  tft.setCursor(193, 8); 
  tft.print("EXTRA INFO -");

  tft.drawRect(190, 20, 105, 41, GRIJS);
  tft.drawRect(190, 73, 105, 41, GRIJS);
  tft.drawRect(190, 126, 105, 41, GRIJS);
  tft.drawRect(190, 179, 105, 41, GRIJS);

  tft.setCursor(193, 25); tft.print("Hartslag:");
  tft.setCursor(193, 78); tft.print("Saturatie:");
  tft.setCursor(193, 131); tft.print("Temperatuur:");
  tft.setCursor(193, 184); tft.print("Ademhaling:");
}