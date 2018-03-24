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

#include <HouzSonyRemote.h>
#include <HouzIrCodes.h>
#include <IRremote.h>
#include <HouzInfrared.h>
#include <HouzDevices.h>

//serial setup
#define serialTx	1	//fixed
#define serialRx	2	//fixed

//radio setup
#define swLight		10	//wall led indicator
#define rfCE		9	//RF pin 3 (CE)
#define rfCS		8	//RF pin 4 (CS)
RF24 radio(rfCE, rfCS);

//ir setup
#define irRecvPin	4	//IRM-8601S
#define irSndPin	3	//IR Led (can't be changed)
IRrecv irrecv(irRecvPin);
IRsend irsend;

//lighting controller setup
#define inSwitch	4 	//wall switch
#define ioClockPin	5	//74HC595 SH_CP: pin 11
#define ioLatchPin	6	//74HC595 ST_CP: pin 12
#define ioDataPin	7	//74HC595 DS: pin 14

//lighting logic   controller outputs
int dicroLight;	// ---- ---- XXXX XXXX
int mainLight;	// ---- --XX ---- ----
int spotlLight;	// --XX XX-- ---- ----
int auxLight;	// XX-- ---- ---- ----


HouzDevices houz(living_node, radio, swLight, Serial, ioDataPin, ioLatchPin, ioClockPin);

void setup() {
	Serial.begin(115200);
	houz.setup();

	//io setup
	pinMode(inSwitch, INPUT_PULLUP);
	houz.setIo(0);

	//announce
	//dicroLight	= B11111111;
	//mainLight	= B11;
	//spotlLight	= B1111;
	//auxLight	= B11;
	//renderLights();

}

void loop() {
	if (houz.hasData()) 
		handleCommand(houz.getData()); 
	switchRead(); 
}

void cmdToStr(deviceData device) {

	String ret = "";
	switch (device.cmd)
	{
	case CMD_SET: ret = ret + ""; break;
	default:
		break;
	}


};

void handleCommand(deviceData device) {
	switch (device.id){

	case living_node		: //ping node
		Serial.println("[living_node] ping");
		houz.radioSend(CMD_VALUE, device.id, !device.payload);
		break;

	case living_switchLed	: // switch led intensity
		Serial.println("[living_switchLed] ");
		if (device.cmd == CMD_SET) houz.setIo(device.payload);
		houz.radioSend(CMD_VALUE, device.id, houz.getIoStatus());
		break;

//////////////////
// APPLIANCES
	case living_AC			:
		Serial.println("[living_AC]");
		break;

	case living_AC_temp		: 
		Serial.println("[living_AC_temp]");
		break;

//////////////////
// LIGHTING
	case living_mainLight: // 2x center light
		Serial.println("[main_light] ");

		if (device.cmd == CMD_SET) {
			mainLight = device.payload;
			renderLights();
		};
		//mainLight = (mainLight == B11) ? B00 : B11;

		
		houz.radioSend(CMD_VALUE, device.id, houz.getIoStatus());
		break;

	case living_arrLight: // raw fx controller
		Serial.println("[living_light] ");
		if (device.cmd == CMD_SET) houz.setIo(device.payload);
		houz.radioSend(CMD_VALUE, device.id, houz.getIoStatus());
		break;

	case 0:
		Serial.print("msg: ");
		Serial.println(device.message);
		break;

	default:		  
		Serial.println("[handleCommand] unknown " + houz.deviceToString(device));
		break;
	}				  
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Lighting control
void switchRead() {
	int buttonState;
	buttonState = digitalRead(inSwitch);
	if (buttonState == HIGH) return;
	Serial.println("[switchRead] inSwitch pressed");

	//handle status
	mainLight = (mainLight == B11)?B00:B11;
	renderLights();

	//notify
	houz.statusLedBlink();
	houz.radioSend(CMD_EVENT, living_switch, 1);
	houz.radioSend(CMD_VALUE, living_mainLight, mainLight);
}


int dicroOnAnim[] = {
	B00000000,
	B00000001,
	B00000011,
	B00000111,
	B00001111,
	B00011111,
	B00111111,
	B01111111,
	B11111111
};
int dicroOffAnim[] = {
	B11111111,
	B01111111,
	B00111111,
	B00011111,
	B00001111,
	B00000111,
	B00000011,
	B00000001,
	B00000000
};



void renderLights() {

	unsigned long out = 0;
	// out logic    controller outputs
	// dicroLight	---- ---- XXXX XXXX
	// mainLight	---- --XX ---- ----
	// spotlLight	--XX XX-- ---- ----
	// auxLight		XX-- ---- ---- ----
	out = auxLight;
	out = (out << 4) + spotlLight;
	out = (out << 2) + mainLight;
	out = (out << 8) + dicroLight;

	houz.setIo(out);
	Serial.println("-----------------");
	Serial.println("");
};

void drawBin(u32 inValue) {
	int i;
	for (i = 15; i > -1; i--) {
		Serial.print(bitRead(inValue, i));
	}
	Serial.println("");
};
