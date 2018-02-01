/*
 Name:		master_logger.ino
 Created:	05-Apr-17
 Updated:	23-Jan-18
 Author:	DarkAngel
*/

// the setup function runs once when you press reset or power the board
#include <HouzDevices.h>

//radio setup
//#define rfRecvLed 47 //Led
//#define rfCE 49      //RF pin 3 (CE)
//#define rfCS 48      //RF pin 4 (CS)

//wiring: D7>CS | D8>CE | (sck)D13>5 | (mosi)D11>6 | (miso)D12>7
#define rfRecvLed 3 //RF online Led
#define rfCE 9      //RF pin 3 (CE)
#define rfCS 10     //RF pin 4 (CS)

RF24 radio(rfCE, rfCS);
HouzDevices houz(server_node, radio, rfRecvLed, Serial);
deviceData device;

void setup() {
	Serial.begin(115200);
	houz.radioSetup();
	Serial.println("\r\n-- master_logger --\r\n");
}

// main loop ////////////////////////////////////////////////////////////////////////////////
void loop() {
	
	// RF commands
	if (houz.radioRead()) {
		handlePacket(action_rfReceived, houz.receivedData());
	};

  // Serial commands
  houz.serialRead();
}

// packet handling ///////////////////////////////////////////////////////////////////////////
void handlePacket(u8 action, deviceData device) {
	Serial.println(houz.packetToString(action, device));
};
