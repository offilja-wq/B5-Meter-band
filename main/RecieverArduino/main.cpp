#include "Arduino.h"

#include "config.h"

CRGB leds[NUM_LEDS];

void setStrip(int i, uint8_t RED, uint8_t GREEN, uint8_t BLUE)
{
  switch (i)
  {
    case 1 ... 8:
      CRGB c(BLUE, GREEN, RED);
      leds[i-1] = c;
      break;
    case 100:
    {
      for(int iLocal = 0; iLocal <= 9; iLocal++)
      {
        CRGB c(BLUE, GREEN, RED);
        leds[iLocal] = c;
        break;
      }
    }
  }
  FastLED.show();
}

void updateStrip(int state)
{
  unsigned long now = millis();

  uint8_t flash = ((now / 100) % 2 == 0) ? 100 : 0;

  float wave = (sin(now / 600.0f) + 1.0f) / 2.0f;

  switch(state) {
    case 0 :
    {
      for(int i = 0; i < NUM_LEDS; i++) { //normaal, groen wave
      setStrip(i, 0, (wave * 20), 0);
      }
    }
    case 1 :
    {
      for(int i = 0; i < NUM_LEDS; i++) { //low/high, oranje wave
      setStrip(i, (wave * 70), (wave * 40), 0);
      }
    }
    case 2 :
    {     
      for(int i = 0; i < NUM_LEDS; i++) { //probleem, rood flashend
      setStrip(i, flash, 0, 0);
      }
    }
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

int getState()
{
    // STATUS 0 — alles normaal
    if (data.Pressure_result   == PRESSURE_NORMAL ||
        data.Ntc_result        == NTC_NORMAL ||
        data.Heartbeat_result  == HEARTBEAT_NORMAL ||
        data.Saturation_result == SATURATION_NORMAL)
    {
        return 0;
    }

    // STATUS 1 — low/high
    if (data.Pressure_result   == PRESSURE_SLOW ||
        data.Pressure_result   == PRESSURE_FAST ||
        data.Ntc_result        == NTC_TOO_LOW ||
        data.Ntc_result        == NTC_TOO_HIGH ||
        data.Heartbeat_result  == HEARTBEAT_LOW ||
        data.Heartbeat_result  == HEARTBEAT_HIGH ||
        data.Saturation_result == SATURATION_LOW ||
        data.Saturation_result == SATURATION_HIGH)
    {
        return 1;
    }

    // STATUS 2 — gevaarlijke waarden
    if (data.Pressure_result   == PRESSURE_DEAD_FAST ||
        data.Pressure_result   == PRESSURE_DEAD_SLOW ||
        data.Ntc_result        == NTC_DANGEROUS ||
        data.Ntc_result        == NTC_DEAD_HIGH ||
        data.Heartbeat_result  == HEARTBEAT_DEAD_LOW ||
        data.Heartbeat_result  == HEARTBEAT_PROBLEMATICALLY_LOW ||
        data.Heartbeat_result  == HEARTBEAT_DEAD_HIGH ||
        data.Heartbeat_result  == HEARTBEAT_PROBLEMATICALLY_HIGH ||
        data.Saturation_result == SATURATION_TOO_LOW ||
        data.Saturation_result == SATURATION_TOO_HIGH)
    {
        return 2;
    }

    // STATUS 3 — geen realistische data
    if (data.Pressure_result   == PRESSURE_NO_REALISTIC_DATA ||
        data.Ntc_result        == NTC_NO_REALISTIC_DATA ||
        data.Ntc_result        == NTC_NO_SKIN_CONTACT ||
        data.Heartbeat_result  == HEARTBEAT_NO_SKIN_CONTACT ||
        data.Heartbeat_result  == HEARTBEAT_NO_REALISTIC_DATA ||
        data.Saturation_result == SATURATION_NO_REALISTIC_DATA ||
        data.Saturation_result == SATURATION_NO_SKIN_CONTACT)
    {
        return 3;
    }
}

// Update blokken
void updateBlokken() 
{
  tft.setTextColor(LICHTROZE);
  tft.setTextSize(2);

  // Blok 1 - Hartslag
  if (data.vingerContact)
  {
    tft.fillRect(191, 40, 53, 14, ZWART);
  }
  else
  {
    tft.fillRect(191, 40, 53, 14, ZWART);
  }

  tft.setCursor(194, 40);
  tft.print(data.HEARTRATE);

  // Blok 2 - Saturatie
  if (data.vingerContact)
  {
    tft.fillRect(191, 93, 53, 14, ZWART);
  }
  else
  {
    tft.fillRect(191, 93, 53, 14, ZWART);
  }

  tft.fillRect(191, 93, 53, 14, ZWART);
  tft.setCursor(194, 93);
  tft.print(data.SATURATION);

  // Blok 3 - Temperatuur
  tft.fillRect(191, 146, 53, 14, ZWART);
  tft.setCursor(194, 146);
  tft.print(data.TEMPERATURE);

  // Blok 4 - Ademhaling
  tft.fillRect(191, 199, 53, 14, ZWART);
  tft.setCursor(194, 199);
  tft.print(data.BREATHRATE);
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

  tft.setCursor(193, 25);   
  tft.print("Hartslag:");
  tft.setCursor(193, 78);   
  tft.print("Saturatie:");
  tft.setCursor(193, 131);  
  tft.print("Temperatuur:");
  tft.setCursor(193, 184); 
  tft.print("Ademhaling:");

  // Eenheden, om flikkeringen te verminderen
  tft.setTextColor(LICHTROZE);
  tft.setTextSize(2);

  tft.setCursor(244, 40);   
  tft.print("BPM");
  tft.setCursor(244, 93);   
  tft.print("%");
  tft.setCursor(244, 146);  
  tft.print("C");
  tft.setCursor(244, 199);  
  tft.print("B/M");
}

void setup() 
{
  Serial.begin(115200);
  mySerial.begin(9600); // 9600, kut met 115200

  FastLED.addLeds<NEOPIXEL, PIN_DATA>(leds, NUM_LEDS);

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
  updateStrip(getState());
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

  // Serial.println(
	// 	String(data.NTC_RAW_DATA)+				"\t"+
	// 	String(data.PRESSURE_RAW_DATA)+			"\t"+
	// 	String(data.HEARTBEAT_RAW_DATA)+			"\t"+
	// 	String(data.SATURATION_RAW_DATA)+			"\t"
	// );
}