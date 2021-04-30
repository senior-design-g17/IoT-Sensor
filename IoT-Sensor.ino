#include <RFM69.h>
#include <SPI.h>
#include <ArduinoLowPower.h>
// #include <string>

#include "pinDefs.h"
#include "serialSettings.h"
#include "radioSettings.h"

char buff[61]; //61 max payload for radio

// Init Radio Object
RFM69 radio;
Payload payload;
bool newPayload = false;

int target = 77;
int lastTemp = 0;

void setup()
{
#ifdef SERIAL_EN
	Serial.begin(SERIAL_BAUD);
	delay(2000);
#endif

	// RADIO
	if (!radio.initialize(FREQUENCY, MYID, NETWORKID))
		DEBUGln("radio.init() FAIL");
	else
		DEBUGln("radio.init() SUCCESS");

	radio.setHighPower(true);
	//radio.setCS(RFM69_CS);

	if (ENCRYPT)
		radio.encrypt(ENCRYPTKEY);

	// Pins
	pinMode(BUTTON_UP, INPUT);
	pinMode(BUTTON_DOWN, INPUT);
	pinMode(TEMP_SNSR, INPUT);

	LowPower.attachInterruptWakeup(BUTTON_UP, UP_ISR, RISING);
	LowPower.attachInterruptWakeup(BUTTON_DOWN, DOWN_ISR, RISING);
	LowPower.attachInterruptWakeup(RTC_ALARM_WAKEUP, TEMP_ISR, CHANGE);

	payload.zoneID = ZONE_ID;

	TEMP_ISR();
}

void loop()
{
	if (newPayload)
	{
		DEBUGln(payload.type);
		DEBUGln(payload.data);

		if (radio.sendWithRetry(HUBID, (const void *)(&payload), sizeof(payload), RETRY_COUNT, RETRY_WAIT))
		{
			DEBUGln("ACK received!");
			newPayload = false;
		}
	}

	if (radio.receiveDone())
	{
		getLoad();
	}

	//radio.sleep();
	// If the payload failed then wait a shorter random time
	if (newPayload)
		LowPower.sleep((int)random(50, 100)); // give a random amount of time ALOHA-net style
	else
		LowPower.sleep((int)TEMP_POLL_MS);
}

Payload getLoad()
{
	Payload load;

	if (radio.DATALEN == sizeof(Payload))
		load = *(Payload *)radio.DATA;

	if (radio.ACKRequested())
	{
		radio.sendACK();
		DEBUGln("ACK sent");
	}

	DEBUGln(load.zoneID);
	DEBUGln(load.type);
	DEBUGln(load.data);

	return load;
}

void UP_ISR()
{
	target++;
	target = min(99, target);

	// Fill payload
	payload.type = target_temp;
	payload.data = target;
	newPayload = true;
}

void DOWN_ISR()
{
	target--;
	target = max(62, target);

	// Fill payload
	payload.type = target_temp;
	payload.data = target;
	newPayload = true;
}

void TEMP_ISR()
{
	int rawData = analogRead(TEMP_SNSR);

	// Fill payload
	payload.type = curr_temp;
	payload.data = rawToDeg(rawData);

	if (lastTemp != payload.data)
	{
		newPayload = true;
		lastTemp = payload.data;
	}
}

// Temporary mapping check documentation
int rawToDeg(int rawData)
{
	// linear slope between 0 C -> 500mV to 100 C -> 1500mV (10 mv/C + 500 mv)
	// 500 mv = 155.15 adc
	// 10 mv/C = 3.103 adc / C

	// returning as fahrenheit
	return CtF((rawData - TEMP_INTERCEPT) / TEMP_SLOPE);
}

int CtF(float C)
{
	return C * 1.8 + 32;
}

// todo make an askForState function
// handle a set temp target funcion