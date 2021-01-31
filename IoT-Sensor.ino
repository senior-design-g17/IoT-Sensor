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

int i = 0;

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
}

void loop()
{
	if (newPayload)
	{
		DEBUGln(payload.type);
		DEBUGln(payload.data);

		if (radio.sendWithRetry(HUBID, (const void *)(&payload), sizeof(payload), RETRY_COUNT, RETRY_WAIT))
		{
			Serial.println("ACK received!");
			newPayload = false;
		}
	}

	if (radio.receiveDone())
	{
		if (radio.ACKRequested())
		{
			radio.sendACK();
			DEBUGln("ACK sent");
		}
	}

	//radio.sleep();
	// If the payload failed then wait a shorter random time
	if (newPayload)
		LowPower.sleep(random(50, 100)); // give a random amount of time ALOHA-net style
	else
		LowPower.sleep(TEMP_POLL_MS);
}

void UP_ISR()
{
	i++;
	//i = min(99, i);

	// Fill payload
	payload.type = target_temp;
	payload.data = i;
	newPayload = true;
}

void DOWN_ISR()
{
	i--;
	//i = max(66, i);

	// Fill payload
	payload.type = target_temp;
	payload.data = i;
	newPayload = true;
}

void TEMP_ISR()
{
	int rawData = analogRead(TEMP_SNSR);

	// Fill payload
	payload.type = curr_temp;
	payload.data = rawToDeg(rawData);
	newPayload = true;
}

// Temporary mapping check documentation
int rawToDeg(int rawData)
{
	map(rawData, 0, 1024, 32, 125);
}
