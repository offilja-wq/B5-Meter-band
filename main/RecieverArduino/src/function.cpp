#include "function.h"

#include "Arduino.h"

//lib def - only for this file

#include "config.h"

MCUFRIEND_kbv tft;

void displayBegin()
{
    uint16_t ID = tft.readID();
    
    if (ID == 0xD3D3) ID = 0x9486;
    tft.begin(ID);
    tft.setRotation(1);
    tft.fillScreen(ZWART);

    tekenGrafiekOutline();
    tekenRechterkant();
}

void setStrip(int i, uint8_t RED, uint8_t GREEN, uint8_t BLUE)
{
    switch (i)
    {
        case 1 ... 8:
        {
            CRGB c(BLUE, GREEN, RED);
            leds[i-1] = c;
            break;
        }
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

void updateStrip()
{
    delay(1);
}

bool updateDisplay(bool start)
{
    SENSORS Sensors = ontvangData();
    updateBlokken(&Sensors);

    // Sinus variabelen
    int sinusX;
    int vorigeY;
    float hoek;

    int middenY;
    int amplitude;
    int nieuweY;

    if (!start)
    {
        // Startpositie sinus
        sinusX  = GRAFIEK_X + 1;
        vorigeY = GRAFIEK_Y + (GRAFIEK_H / 2);
        start = true;
    }

    // Sinus berekenen
    middenY = GRAFIEK_Y + (GRAFIEK_H / 2);
    //int amplitude = (GRAFIEK_H / 2) - 70;
    amplitude = map(Sensors.PRESSURE_RAW_DATA, 0, 4095, 0, (GRAFIEK_H / 2) - 5);
    nieuweY = middenY - (int)(sin(hoek) * amplitude);

    // Lijn tekenen
    tft.drawLine(sinusX - 1, vorigeY, sinusX, nieuweY, ROOD);

    vorigeY = nieuweY;
    hoek += 0.15;
    sinusX++;

    // Reset als einde grafiek bereikt
    if (sinusX > GRAFIEK_X + GRAFIEK_B - 1) 
    {
        sinusX = GRAFIEK_X + 1;
        hoek = 0.0;
        vorigeY = middenY;

        tft.fillRect(GRAFIEK_X + 1, GRAFIEK_Y + 1, GRAFIEK_B - 2, GRAFIEK_H - 2, LICHTROZE);
        tekenRaster();
    }

    return start;
}

// Update blokken
void updateBlokken(SENSORS *input) 
{
    // Blok 1 - Hartslag
    if(input->Heartbeat_result == HEARTBEAT_NO_SKIN_CONTACT)
    {
        tft.fillRect(191, 38, 75, 12, ROOD);
    }
    else
    {
        tft.fillRect(191, 38, 75, 12, ZWART);
    }
    tft.setTextColor(LICHTROZE);
    tft.setTextSize(2);
    tft.setCursor(194, 40);
    tft.print(input->HEARTBEAT_RAW_DATA);
    tft.print(" BPM");

    // Blok 2 - Saturatie
    if(input->Heartbeat_result == HEARTBEAT_NO_SKIN_CONTACT)
    {
        tft.fillRect(191, 120, 75, 12, ROOD);
    }
    else
    {
        tft.fillRect(191, 120, 75, 12, ZWART);
    }
    tft.fillRect(191, 120, 75, 12, ZWART);
    tft.setTextColor(LICHTROZE);
    tft.setCursor(194, 115);
    tft.print(input->SATURATION_RAW_DATA);
    tft.print(" %");

    // Blok 3 - Temperatuur
    tft.fillRect(191, 193, 75, 12, ZWART);
    tft.setTextColor(LICHTROZE);
    tft.setCursor(194, 190);
    tft.print(input->NTC_RAW_DATA);
    tft.print(" C");
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
    tft.setCursor(GRAFIEK_X + 45, 8);
    tft.print("GRAFIEK");

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

    tft.drawRect(190, 20, 90, 50, GRIJS);
    tft.drawRect(190, 95, 90, 50, GRIJS);
    tft.drawRect(190, 170, 90, 50, GRIJS);

    tft.setTextColor(ROZE);
    tft.setCursor(193, 25); tft.print("Hartslag:");
    tft.setCursor(193, 100); tft.print("Saturatie:");
    tft.setCursor(193, 175); tft.print("Temperatuur:");
}

// CRGB leds[NUM_LEDS];