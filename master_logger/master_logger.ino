/*
 Name:		master_logger.ino
 Created:	05-Apr-17 21:40:50
 Author:	DarkAngel
*/

// the setup function runs once when you press reset or power the board
#include <RFencoder.h>
#include <RF24_config.h>
#include <RF24.h>
#include <printf.h>
#include <nRF24L01.h>

//radio setup
#define rfRecvLed 47 //Led
#define rfCE 49      //RF pin 3 (CE)
#define rfCS 48      //RF pin 4 (CS)
#define rfChannel 0x5D   
#define rfPipeMain		0xF0F0F0F0AA
#define rfPipeBedr		0xF0F0F0F066
#define rfPipeSensors	0xF0F0F0F011
bool radioReady = false;
RF24 radio(rfCE, rfCS);
RFencoder rfEncoder;

void setup() {
	//serial debug
	Serial.begin(115200);
	Serial.println("-- setup --");

	//radio setup
	Serial.println("::RF setup");
	printf_begin();
	radio.begin();
	radio.setPALevel(RF24_PA_HIGH);
	radio.setChannel(rfChannel);
	radio.openWritingPipe(rfPipeSensors);
	radio.openReadingPipe(1, rfPipeMain);
	radioReady = (rfChannel == radio.getChannel());
	if (radioReady) {
		radio.printDetails();
		radio.startListening();
	}
	Serial.print("radio: ");
	Serial.println(radioReady ? "online" : "offline");

	Serial.println("");
	Serial.println("-- runtime --");

}

// the loop function runs over and over again until power down or reset
void loop() {
	radioRead();
}

void radioRead() {
	unsigned long rfMessage;
	if (radio.available()) {
		while (radio.available()) {             // While there is data ready
			radio.read(&rfMessage, sizeof(unsigned long));  // Get the payload
			digitalWrite(rfRecvLed, HIGH);       // Notify receive
		}
		handleRecvMsg(rfMessage);
		delay(100);
		radio.startListening();
		digitalWrite(rfRecvLed, LOW);
	}
}

void handleRecvMsg(u32 recMsg) {
	Serial.print("> 0x");
	Serial.println(recMsg, HEX);

	//decode message
	decode_result dev;
	rfEncoder.decode(recMsg, &dev);
	printDecodedValues(dev);}

// debug
void printDecodedValues(decode_result dec) {
	if (dec.hasData) {
		Serial.print("[dev:");
		Serial.print(dec.deviceId);
		Serial.print("|val:");
		Serial.print(dec.devicePayload);
		Serial.println("]");
	}
	else {
		Serial.println("[unknown data]");
	}
}