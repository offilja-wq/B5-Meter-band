typedef struct
{
  uint16_t NTC_RAW_DATA;
  uint16_t PRESSURE_RAW_DATA;
  uint16_t HEARTBEAT_RAW_DATA;
  uint16_t SATURATION_RAW_DATA;
} InputData;

InputData data;
#define START_BYTE 0xAA
#define TXD2 17
#define RXD2 16
#define ADC_PIN 34

void setup() 
{
  Serial.begin(115200);
  Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2);
  Serial.println("ESP32 gestart");
}

void loop() 
{
  uint16_t potValue = analogRead(ADC_PIN);
  data.NTC_RAW_DATA = potValue;
  data.PRESSURE_RAW_DATA = potValue;
  data.HEARTBEAT_RAW_DATA = potValue;
  data.SATURATION_RAW_DATA = potValue;
  updateDisplay(&data);
  delay(100);
}

void updateDisplay(InputData *input)
{
  Serial2.write(START_BYTE);
  Serial2.write((uint8_t*)input, sizeof(InputData));
  Serial.println("Data verzonden");
}