/*
 Name:		master_logger.ino
 Created:	05-Apr-17 21:40:50
 Author:	DarkAngel
*/

// the setup function runs once when you press reset or power the board
#include <HouzDevices.h>

//radio setup
#define rfRecvLed 47 //Led
#define rfCE 49      //RF pin 3 (CE)
#define rfCS 48      //RF pin 4 (CS)

RF24 radio(rfCE, rfCS);
HouzDevices houz(server_node, radio, rfRecvLed, Serial);

void setup() {
	//serial debug
	Serial.begin(115200);
	Serial.println("-- setup --");
	houz.radioSetup(rfRecvLed);

	Serial.println("");
	Serial.println("\r\n-- runtime --");
}

// the loop function runs over and over again until power down or reset
void loop() {
	if (houz.radioRead()) {
		handleDevice(houz.receivedData());
	}
}

void handleDevice(deviceData device) {
	Serial.println(houz.deviceToString(device));
}