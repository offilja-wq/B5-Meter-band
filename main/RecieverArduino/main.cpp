#include "Arduino.h"

#include "config.h"
#include "function.h"

SoftwareSerial mySerial(RX, TX); // RX, TX

bool receiving = false;
uint8_t buf[sizeof(SENSORS)];
uint8_t index = 0;

bool start = false;

// UART data lezen
SENSORS ontvangData()
{
    SENSORS Sensors;

    while (mySerial.available())
    {
        uint8_t b = mySerial.read();
        if (receiving)
        {
            buf[index++] = b;
            if (index >= sizeof(SENSORS))
            {
                memcpy(&Sensors, buf, sizeof(SENSORS));
                receiving = false;
            }
        }
        else
        {
            if (b == START_BYTE)
            {
                receiving = true;
                index = 0;
            }
        }
        Serial.println(Sensors.PRESSURE_RAW_DATA);
    }
}

void setup() 
{
    Serial.begin(9600);
    mySerial.begin(9600);
    displayBegin();
}

void loop() 
{
  updateDisplay(start);
  updateStrip();
}