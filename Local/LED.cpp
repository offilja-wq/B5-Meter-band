#include <FastLED.h>

#define NUM_LEDS  9
#define DATA_PIN  11
#define CLOCK_PIN 13

CRGB leds[NUM_LEDS];

int buikademhaling;
unsigned long now;
uint8_t knipper;
float golf;

int AdemInUit;
int WelkeLED;
int STATUS;

void setup() {
  FastLED.addLeds<APA102, DATA_PIN, CLOCK_PIN, BGR>(leds, NUM_LEDS);
}

void setStrip(int i, int b, int g, int r)
{
    switch(i) {
      case 1 ... 8 :
        CRGB c(b, g, r);
        leds[i-1] = c;
        break;
      default :
        for(int iCount=0; iCount <= 8; iCount++)
        {
          CRGB c(b, g, r);
          leds[i-1] = c;
        }
        break;
    }
     FastLED.show();
}


void formulesLED(InputData *input) {
    buikademhaling = input->PRESSURE_RAW_DATA, 0, 650, 0, NUM_LEDS

    now = millis();
    knipper = ((now / 100) % 2 == 0) ? 10 : 0;
    golf = (sin(now / 600.0f) + 1.0f) / 2.0f;
}

void updateAdemStatus() {
    if (buikademhaling <= 300) {
        AdemInUit = 0;
    }
    else if (buikademhaling <= 450) {
        AdemInUit = 1;
    }
    else {
        AdemInUit = 2;
    }
}

void updateStatus() {

    // STATUS 0 — alles normaal
    if (input->Pressure_result   == PRESSURE_NORMAL ||
        input->NTC_result        == NTC_NORMAL ||
        input->HEARTBEAT_result  == HEARTBEAT_NORMAL ||
        input->SATURATION_result == SATURATION_NORMAL)
    {
        STATUS = 0;
        return;
    }

    // STATUS 1 — low/high
    if (input->Pressure_result   == PRESSURE_LOW ||
        input->Pressure_result   == PRESSURE_HIGH ||
        input->NTC_result        == NTC_TOO_LOW ||
        input->NTC_result        == NTC_TOO_HIGH ||
        input->HEARTBEAT_result  == HEARTBEAT_LOW ||
        input->HEARTBEAT_result  == HEARTBEAT_HIGH ||
        input->SATURATION_result == SATURATION_LOW ||
        input->SATURATION_result == SATURATION_HIGH)
    {
        STATUS = 1;
        return;
    }

    // STATUS 2 — gevaarlijke waarden
    if (input->Pressure_result   == PRESSURE_DEAD ||
        input->NTC_result        == NTC_DANGEROUS ||
        input->NTC_result        == NTC_DEAD_HIGH ||
        input->HEARTBEAT_result  == HEARTBEAT_DEADLY_LOW ||
        input->HEARTBEAT_result  == HEARTBEAT_PROBLEMATICALLY_LOW ||
        input->HEARTBEAT_result  == HEARTBEAT_DEADLY_HIGH ||
        input->HEARTBEAT_result  == HEARTBEAT_PROBLEMATICALLY_HIGH ||
        input->SATURATION_result == SATURATION_TOO_LOW ||
        input->SATURATION_result == SATURATION_TOO_HIGH)
    {
        STATUS = 2;
        return;
    }

    // STATUS 3 — geen realistische data
    if (input->Pressure_result   == PRESSURE_NO_REALISTIC_DATA ||
        input->NTC_result        == NTC_NO_REALISTIC_DATA ||
        input->NTC_result        == NTC_NO_SKIN_CONTACT ||
        input->HEARTBEAT_result  == HEARTBEAT_NO_SKIN_CONTACT ||
        input->HEARTBEAT_result  == HEARTBEAT_NO_REALISTIC_DATA ||
        input->SATURATION_result == SATURATION_NO_REALISTIC_DATA ||
        input->SATURATION_result == SATURATION_NO_SKIN_CONTACT)
    {
        STATUS = 3;
        return;
    }
}

void showFoutmelder() {
    for (int i = 0; i < NUM_LEDS; i++) {
        if (STATUS == 0) {
            setStrip(i, 0, (golf * 20), 0);  // groen golf
        }
        else if (STATUS == 1) {
            setStrip(i, (golf * 70), (golf * 40), 0); // oranje golf
        }
        else if (STATUS == 2 || STATUS == 3) {
            setStrip(i, knipper, 0, 0); // rood knipper
        }
    }
}

void showBuikademhaling(InputData *input) {
    int ledsAan = map(input->PRESSURE_RAW_DATA, 0, 650, 0, NUM_LEDS);

    for (int i = 0; i < NUM_LEDS; i++) {
        if (i < ledsAan) {
            if (AdemInUit == 0)      setStrip(i, 15, 20, 100);  // uit wit
            else if (AdemInUit == 1) setStrip(i, 10, 10, 255);  // midden lichtblauw
            else                     setStrip(i, 1, 1, 255);    // in blauw
        } else {
            setStrip(i, 0, 0, 0);
        }
    }
}

void updateLeds() {
    switch (WelkeLED) {
        case 1: showFoutmelder(); break;
        case 2: showBuikademhaling(); break;
    }
}

void loop() {
    formulesLED();
    updateAdemStatus();
    updateStatus();
    updateLeds();
}