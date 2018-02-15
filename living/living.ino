/*
 Name:		living.ino
 Created:	14-Feb-18 20:49:12
 Author:	DarkAngel
 Reference
 - RF nRF24L01+
	wiring: D7>CS | D8>CE | (sck) D13>5 | (mosi) D11>6 | (miso) D12>7

 - IR Receiver
	IRM-8601S

 - IO shift
	74HC595
*/
#include <IRremoteInt.h>
#include <IRremote.h>
#include <boarddefs.h>
#include <HouzSonyRemote.h>
#include <HouzIrCodes.h>
#include <HouzInfrared.h>
#include <HouzDevices.h>

//radio setup
#define swLight		5	//wall led indicator
#define rfCE		8	//RF pin 3 (CE)
#define rfCS		7	//RF pin 4 (CS)
RF24 radio(rfCE, rfCS);

//ir setup
#define irRecvPin	6	//IRM-8601S
#define irSndPin	3	//IR Led (can't be changed)
IRrecv irrecv(irRecvPin);
IRsend irsend;

//serial setup
#define serialTx	1	//fixed
#define serialRx	2	//fixed

//lighting setup
#define inSwitch	A0	//wall switch
#define ioClockPin	A0	//74HC595 SH_CP: pin 11
#define ioLatchPin	A0	//74HC595 ST_CP: pin 12
#define ioDataPin	A0	//74HC595 DS: pin 14

HouzDevices houz(bedroom_node, radio, swLight, Serial, ioDataPin, ioLatchPin, ioClockPin);



void setup() {
	houz.radioSend(CMD_STATUS, living_node, 1);
}

// the loop function runs over and over again until power down or reset
void loop() {
	if (houz.radioRead()) { handleCommand(houz.receivedData()); };
	handleSerial();
	switchRead(); 
}

void handleCommand(deviceData device) {
	Serial.println("\r" + houz.deviceToString(device));
}

char serialIn;
void handleSerial() {
	if (Serial.available() == 0) return;
	serialIn = Serial.read();
	Serial.println(serialIn);
	Serial.print(">");
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// I/O
void switchRead() {
	int buttonState;
	buttonState = digitalRead(inSwitch);
	if (buttonState == HIGH) return;

	//houz.statusLedBlink();
	//houz.radioSend(CMD_EVENT, office_switch, 1);
	//houz.radioSend(CMD_VALUE, office_light, getLight(ceilingLightPin) ? 1 : 0);
	delay(500); //debounce
}

