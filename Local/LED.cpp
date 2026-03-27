#include <FastLED.h>

#define NUM_LEDS  9
#define DATA_PIN  11
#define CLOCK_PIN 13

CRGB leds[NUM_LEDS];

int WelkeLED; //1 is foutmelder, 2 is ademhaling

int STATUS; //1 normaal, 2 probleem, 3 dood

int AdemInUit; //0 is uit, 1 is midden, 2 is in

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

void setup() {
  Serial.begin(9600);
  FastLED.addLeds<APA102, DATA_PIN, CLOCK_PIN, BGR>(leds, NUM_LEDS);
}

long scale(long x, long in_min, long in_max, long out_min, long out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

void loop() {
  int buikademhaling = analogRead(A0);
    Serial.println(buikademhaling);
  if (buikademhaling >= 0 && buikademhaling <= 300) {
    AdemInUit = 0;
  }
  else if (buikademhaling >= 301 && buikademhaling <= 450) {
    AdemInUit = 1;
  }
  else if (buikademhaling >= 451 && buikademhaling <= 1000) {
    AdemInUit = 2;
  }  

  unsigned long now = millis();

  uint8_t knipper = ((now / 100) % 2 == 0) ? 10 : 0;

  float golf = (sin(now / 600.0f) + 1.0f) / 2.0f;


  WelkeLED = 2;

  switch(WelkeLED) {
    case 1:         //Foutmelder
      if(STATUS == 0) {
        for(int i = 0; i < NUM_LEDS; i++) { //normaal, groen golf
        setStrip(i, 0, (golf * 20), 0);
        }
      }
      if(STATUS == 1) {
        for(int i = 0; i < NUM_LEDS; i++) { //low/high, oranje golf
        setStrip(i, (golf * 70), (golf * 40), 0);
        }
      }
      if(STATUS == 2) {     
        for(int i = 0; i < NUM_LEDS; i++) { //probleem, rood knipperend
        setStrip(i, knipper, 0, 0);
        }
      }
      break;
    case 2:         //Buikademhaling golf
      int ledsAan = scale(buikademhaling, 0, 600, 0, NUM_LEDS);
      for (int i = 0; i < NUM_LEDS; i++) {
        if ((i < ledsAan) && (AdemInUit == 0)) { //uit wit
          setStrip(i, 15, 20, 100); 
        } 
        else if ((i < ledsAan) && (AdemInUit == 1)) { //midden lichtblauw
          setStrip(i, 10, 10, 255); 
        }
        else if ((i < ledsAan) && (AdemInUit == 2)) { //in blauw
          setStrip(i, 1, 1, 255); 
        } 
        else {
          setStrip(i, 0, 0, 0);
        }
      }
      break;
  }
// hier nog de formule voor gemiddelde ademhaling gebruiken voor pressure data normal enz
  if (input->Pressure_result == PRESSURE_NORMAL ||
      input->NTC_result == NTC_NORMAL ||
      input->HEARTBEAT_result == HEARTBEAT_NORMAL ||
      input->SATURATION_result == SATURATION_NORMAL) 
  {
    STATUS = 0;
  }
  else if (PRESSURE_LOW ||
           input->Pressure_result == PRESSURE_HIGH ||
           input->NTC_result == NTC_TOO_LOW ||
           input->NTC_result == NTC_TOO_HIGH ||
           input->HEARTBEAT_result == HEARTBEAT_LOW ||
           input->HEARTBEAT_result == HEARTBEAT_HIGH ||
           input->SATURATION_result == SATURATION_LOW ||
           input->SATURATION_result == SATURATION_HIGH)
  {
    STATUS = 1;
  }
  else if (PRESSURE_DEADLY_LOW ||
           input->Pressure_result == PRESSURE_PROBLEMATICALLY_LOW ||
           input->NTC_result == NTC_DANGEROUS || 
           input->NTC_result == NTC_DEAD_HIGH || 
           input->HEARTBEAT_result == HEARTBEAT_DEADLY_LOW ||
           input->HEARTBEAT_result == HEARTBEAT_PROBLEMATICALLY_LOW ||
           input->HEARTBEAT_result == HEARTBEAT_DEADLY_HIGH ||
           input->HEARTBEAT_result == HEARTBEAT_PROBLEMATICALLY_HIGH ||
           input->SATURATION_result == SATURATION_TOO_LOW || 
           input->SATURATION_result == SATURATION_TOO_HIGH) 
  {
    STATUS = 2;
  }
  else if (input->Pressure_result == PRESSURE_NO_REALISTIC_DATA ||
           input->NTC_result == NTC_NO_REALISTIC_DATA || 
           input->NTC_result == NTC_NO_SKIN_CONTACT ||
           input->HEARTBEAT_result == HEARTBEAT_NO_SKIN_CONTACT ||
           input->HEARTBEAT_result == HEARTBEAT_NO_REALISTIC_DATA ||
           input->SATURATION_result == SATURATION_NO_REALISTIC_DATA ||
           input->SATURATION_result == SATURATION_NO_SKIN_CONTACT) 
  {
    STATUS = 3;
  }

}
