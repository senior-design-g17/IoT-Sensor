#include <RFM69.h>
#include <SPI.h>
// #include <string>

#include "pinDefs.h"
#include "serialSettings.h"
#include "radioSettings.h"

char buff[61]; //61 max payload for radio

// Init Radio Object
RFM69 radio;

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
	pinMode(BUTTON_UP, OUTPUT);
	pinMode(BUTTON_DOWN, OUTPUT);

	attachInterrupt(digitalPinToInterrupt(BUTTON_UP), UP_ISR, RISING);
	attachInterrupt(digitalPinToInterrupt(BUTTON_DOWN), DOWN_ISR, RISING);
}

int i = 0;
int temp = 0;
bool newData = false;

void loop()
{
	if (newData)
	{	
		DEBUGln(i);

		String sendbuffer = String(i);

		if (radio.sendWithRetry(HUBID, sendbuffer.c_str(), sendbuffer.length(), RETRY_COUNT, RETRY_WAIT))
		{
			newData = false;
			Serial.println("ACK received!");
		}
	}

	// change to sleep
	//radio.sleep();
}

void UP_ISR()
{
	newData = true;
	i++;
}

void DOWN_ISR()
{
	newData = true;
	i--;
}
