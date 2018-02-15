/*
 Name:		master_logger.ino
 Created:	05-Apr-17
 Updated:	23-Jan-18
 Author:	DarkAngel
*/
#include <HouzDevices.h>

// radio setup //////////////////////////////////////////////////////////////
//wiring: D8>CS | D9>CE | (sck)D13>5 | (mosi)D11>6 | (miso)D12>7
#define rfRecvLed 10 //RF online Led
#define rfCE 9   //RF pin 3 (CE)
#define rfCS 8  //RF pin 4 (CS)
RF24 radio(rfCE, rfCS);
HouzDevices houz(server_node, radio, rfRecvLed, Serial);
deviceData device;

// ir setup
// #define irRecvPin	6	//IRM-8601S
// IRrecv irrecv(irRecvPin);



void setup() {
	Serial.begin(115200);
	houz.radioSetup();
//	irrecv.enableIRIn();
	Serial.println("\r\n-- master_logger --\r\n");
}
bool airConditionerOn;

// main loop ////////////////////////////////////////////////////////////////////////////////
void loop() {

	// RF commands
	if (houz.radioRead()) {
      Serial.println(houz.packetToString(action_rfReceived, houz.receivedData()));
	};

  // Serial commands
  houz.serialRead();
}

// packet handling ///////////////////////////////////////////////////////////////////////////
void handlePacket(u8 action, deviceData device) {
	Serial.println(houz.packetToString(action, device));
};

