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
	if (!particleSensor.begin(Wire, I2C_SPEED_FAST)) //Use default I2C port, 400kHz speed
	{
		while (1);
	}

	particleSensor.setup(); //Configure sensor with default settings
	particleSensor.setPulseAmplitudeRed(0x0A); //Turn Red LED to low to indicate sensor is running
	particleSensor.setPulseAmplitudeGreen(0); //Turn off Green LED
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
	if (checkForBeat(particleSensor.getIR()) == true)
	{
		//We sensed a beat!
		long delta = millis() - Src_Sensors.lastBeat;
		Src_Sensors.lastBeat = millis();

		Src_Sensors.beatsPerMinute = 60 / (delta / 1000.0);

		if (Src_Sensors.beatsPerMinute < 255 && Src_Sensors.beatsPerMinute > 20)
		{
		Src_Sensors.rates[Src_Sensors.rateSpot++] = (byte)Src_Sensors.beatsPerMinute; //Store this reading in the array
		Src_Sensors.rateSpot %= Src_Sensors.RATE_SIZE; //Wrap variable

		//Take average of readings
		Src_Sensors.beatAvg = 0;
		for (byte x = 0 ; x < Src_Sensors.RATE_SIZE ; x++)
			Src_Sensors.beatAvg += Src_Sensors.rates[x];
		Src_Sensors.beatAvg /= Src_Sensors.RATE_SIZE;
		}
	}

	return Src_Sensors.beatsPerMinute;

}

// Lees Saturation
uint8_t READ_SATURATION()
{
	return particleSensor.getRed();
}

bool checkVingerContact()
{
	return (particleSensor.getIR() < 50000);
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
			// currentInput.HEARTBEAT_RAW_DATA = READ_HEARTBEAT();
			// currentInput.SATURATION_RAW_DATA = READ_SATURATION();

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
			currentInput.packageTypeCode = PACKAGETYPE_DATA_SEND;
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