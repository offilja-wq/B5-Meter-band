#include <FastLED.h>

#define NUM_LEDS  9
#define DATA_PIN  11
#define CLOCK_PIN 13

CRGB leds[NUM_LEDS];

int WelkeLED; //1 is foutmelder, 2 is ademhaling

int STATUS; //1 normaal, 2 probleem, 3 dood

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
    STATUS = 0;
  }
  else if (buikademhaling >= 301 && buikademhaling <= 400) {
    STATUS = 1;
  }
  else if (buikademhaling >= 401 && buikademhaling <= 1000) {
    STATUS = 2;
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
        if ((i < ledsAan) && (STATUS == 0)) { //normaal, blauw
          setStrip(i, 0, 0, 150); 
        } 
        else if ((i < ledsAan) && (STATUS == 1)) { //low/high, paars
          setStrip(i, 70, 0, 40); 
        }
        else if ((i < ledsAan) && (STATUS == 2)) { //probleem, rood 
          setStrip(i, 150, 0, 0); 
        } 
        else {
          setStrip(i, 0, 0, 0);
        }
      }
      break;
  }
}
