#include "function.h"

#include "Arduino.h"

//lib def - only for this file

#include "config.h"
#include "Networking_by_B5.h"

static const char *TAG = "MAIN";

MAX30105 particleSensor;

SRC_SENSORS Src_Sensors;

// Activeer sensoren
void startSensor()
{
	while (!particleSensor.begin(Wire, I2C_SPEED_FAST));
 	
	particleSensor.setup(ledBrightness, sampleAverage, ledMode, sampleRate, pulseWidth, adcRange);
  	particleSensor.setPulseAmplitudeRed(0x0A); //Turn Red LED to low to indicate sensor is running
  	particleSensor.setPulseAmplitudeGreen(0); //Turn off Green LED

	while (Serial.available() == 0);
  		Serial.read();

    Src_Sensors.bufferLength = 100;

	for (byte i = 0; i < Src_Sensors.bufferLength; i++)
	{
		Src_Sensors.redBuffer[i] = particleSensor.getRed();
		Src_Sensors.irBuffer[i] = particleSensor.getIR();
		particleSensor.nextSample();
	}

 	 maxim_heart_rate_and_oxygen_saturation(Src_Sensors.irBuffer, Src_Sensors.bufferLength, Src_Sensors.redBuffer, &Src_Sensors.spo2, &Src_Sensors.validSPO2, &Src_Sensors.heartRate, &Src_Sensors.validHeartRate);
}

// Lees NTC
uint16_t READ_NTC()
{
    return analogRead(NTC_SENSOR_PIN);
}

// Lees Pressure
uint16_t READ_PRESSURE()
{
    return analogRead(PRESSURE_SENSOR_PIN);
}



// Lees Heartbeat
uint8_t READ_HEARTBEAT()
{
	while(!Src_Sensors.validHeartRate)
	{
		for (byte i = 25; i < 100; i++)
		{
		Src_Sensors.redBuffer[i - 25] = Src_Sensors.redBuffer[i];
		Src_Sensors.irBuffer[i - 25] = Src_Sensors.irBuffer[i];
		}

		for (byte i = 75; i < 100; i++)
		{
		Src_Sensors.redBuffer[i] = particleSensor.getRed();
		Src_Sensors.irBuffer[i] = particleSensor.getIR();
		particleSensor.nextSample();
		}

    	maxim_heart_rate_and_oxygen_saturation(Src_Sensors.irBuffer, Src_Sensors.bufferLength, Src_Sensors.redBuffer, &Src_Sensors.spo2, &Src_Sensors.validSPO2, &Src_Sensors.heartRate, &Src_Sensors.validHeartRate);
	}
	
	return Src_Sensors.heartRate;

}

// Lees Saturation
uint8_t READ_SATURATION()
{
	while(!Src_Sensors.validSPO2)
	{
		for (byte i = 25; i < 100; i++)
		{
			Src_Sensors.redBuffer[i - 25] = Src_Sensors.redBuffer[i];
			Src_Sensors.irBuffer[i - 25] = Src_Sensors.irBuffer[i];
		}

		for (byte i = 75; i < 100; i++)
		{
			Src_Sensors.redBuffer[i] = particleSensor.getRed();
			Src_Sensors.irBuffer[i] = particleSensor.getIR();
			particleSensor.nextSample();
		}

		maxim_heart_rate_and_oxygen_saturation(Src_Sensors.irBuffer, Src_Sensors.bufferLength, Src_Sensors.redBuffer, &Src_Sensors.spo2, &Src_Sensors.validSPO2, &Src_Sensors.heartRate, &Src_Sensors.validHeartRate);
	}

	return Src_Sensors.spo2;
}

bool checkVingerContact()
{
    return particleSensor.getIR() >= 50000;
}

void setStrip(int i, uint8_t RED, uint8_t GREEN, uint8_t BLUE)
{
	switch(i) {
    	case 1 ... 8 : 
		{
        	CRGB c(BLUE, GREEN, RED); 
        	leds[i-1] = c;
        	break;
		}
    	case 100 :
		{
        	for(int iLocal=0; iLocal <= 8; iLocal++)
			{
          		CRGB c(BLUE, GREEN, RED);
          		leds[iLocal] = c;
        	}

        	break;
		}

    }
    FastLED.show();
}

void updateStrip(InputData *input)
{
	int ledsAan = map(input->PRESSURE_RAW_DATA, 0, 650, 0, NUM_LEDS);

	setStrip(100, 0, 0, 0);

	for (int i = 0; i < ledsAan; i++)
	{
		setStrip(i, 150, 150, 255);
	}
}

// RECIEVE
void handleResponseBand(InputData *input)
{
	Networking &networkBand = Networking::getInstance();
	Pinout pinoutBand = networkBand.getPinout();

	unsigned long now = millis();
	uint32_t oldPackageCount;

	// Check of er echt een nieuw pakket is
	bool newPacket = (input->packageCount > oldPackageCount);
	
	if ((!newPacket)||((now-networkBand.lastPacket) > 1000)) 
	{	
		createPacket(PACKAGETYPE_CALL_ACKNOWLEDGE);
		digitalWrite(pinoutBand.PIN_LED, 0);
		return;
	} else {
		digitalWrite(pinoutBand.PIN_LED, (((now+networkBand.lastPacket)/500)%2));
	}

	networkBand.lastPacket = now;

	// Reageer op het pakket
	switch(input->packageTypeCode)
	{
		case PACKAGETYPE_DATA_SEND:
			// Reageer
			createPacket(PACKAGETYPE_DATA_SEND);
			break;
		case PACKAGETYPE_COMMAND_RESET:
			// Reset
			esp_restart;
			break;
		case PACKAGETYPE_CALL_ACKNOWLEDGE:
			// Verwerk oproepbevestiging
			createPacket(PACKAGETYPE_DATA_SEND);
			break;
		default:
			createPacket(input->packageTypeCode);
			break;
	}

	oldPackageCount = input->packageCount;
}

// TRANSMIT
void createPacket(PACKAGETYPECODE type)
{
	static const char *TAG = "MAIN";

	Networking &networkBand = Networking::getInstance();
	Identity identityBand;

	static InputData previousInput;
	static InputData currentInput;

	static Packet packet = {
		.identity = networkBand.getIdentity(),
	};

	currentInput.packageTypeCode = type;

	// Controleer of de input is veranderd
	bool inputChanged = memcmp(&currentInput, &previousInput, sizeof(InputData)) != 0;

	currentInput.startOfCommunication = 01;
	currentInput.packageSize = sizeof(Packet)-1;
	currentInput.sourceIdentity = 0x14;
	currentInput.destinationIdentity = 0x15;

	currentInput.PriorityState = (inputChanged&&(currentInput.packageTypeCode == PACKAGETYPE_DATA_SEND));
	
	currentInput.endOfTransmission = 02;
	currentInput.longitudinalRedundancyCheck = networkBand.checkLRCOutput(&packet);

	switch(type)
	{
		case PACKAGETYPE_RETRANSMIT:
			// Verwerk hertransmissie
			// de data wordt niet veranderd - geen aanpassing
			currentInput.packageTypeCode = PACKAGETYPE_DATA_SEND;
			break;
		case PACKAGETYPE_DATA_SEND:
			// Verwerk gegevensverzending
			currentInput.NTC_RAW_DATA = READ_NTC();
			currentInput.PRESSURE_RAW_DATA = READ_PRESSURE();
			currentInput.HEARTBEAT_RAW_DATA = READ_HEARTBEAT();
			currentInput.SATURATION_RAW_DATA = READ_SATURATION();

			currentInput.packageCount = currentInput.packageCount+1;
			currentInput.packageTypeCode = type;

			previousInput = currentInput;
			break;
		case PACKAGETYPE_CALL_STATE:
			// Verwerk oproepstatus
			break;
		case PACKAGETYPE_CALL_ACKNOWLEDGE:
			// Verwerk oproepbevestiging
			currentInput.packageCount = currentInput.packageCount+1;
			currentInput.packageTypeCode = type;
			break;
	}

	// Kopieer de input data naar het pakket
	memcpy(packet.data, &currentInput, sizeof(InputData));
	networkBand.send(&packet);
}

// Handelt alle ESP-now paketten af
void handleNetwork(const uint8_t *mac, const Packet *packet)
{
	Networking &networkBand = Networking::getInstance();

	//Local
	handleResponseBand((InputData*)packet->data);
	updateStrip((InputData*)packet->data);
}

CRGB leds[NUM_LEDS];