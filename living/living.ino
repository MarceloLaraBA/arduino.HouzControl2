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

//lighting setup
#define inSwitch	4 	//wall switch
#define ioClockPin	5	//74HC595 SH_CP: pin 11
#define ioLatchPin	6	//74HC595 ST_CP: pin 12
#define ioDataPin	7	//74HC595 DS: pin 14

HouzDevices houz(living_node, radio, swLight, Serial, ioDataPin, ioLatchPin, ioClockPin);

void setup() {
	//io setup
	pinMode(inSwitch, INPUT_PULLUP);

	//serial
	Serial.begin(115200);

	//radio
	houz.radioSetup();

	Serial.println("\r\n-- living_node --");
}

void loop() {
	if (houz.radioRead()) handleCommand(houz.receivedData());
	if (houz.serialRead()) handleCommand(houz.serialData());
	switchRead(); 
	houz.taskManager();
}

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

	case living_light		: // main lights
		Serial.println("[living_light] ");
		if (device.cmd == CMD_SET) houz.setIo(device.payload);
		houz.radioSend(CMD_VALUE, device.id, houz.getIoStatus());
		break;

	case living_AC			: 
		Serial.println("[living_AC]");
		break;

	case living_AC_temp		: 
		Serial.println("[living_AC_temp]");
		break;

	default:		  
		Serial.println("[handleCommand] unknown " + houz.deviceToString(device));
		break;
	}				  
					  
					  
}

void testIo(){
  int i=0;
  for (i=0; i<16; i++)  {
    Serial.print(i);
    houz.setIo(i, true);
    delay(50);
    };
  for (i=0; i<16; i++)  {
    Serial.print(i);
    houz.setIo(i, false);
    delay(50);
    };

};

//int actIo;
//char serialIn;
//bool actStatttt;
//void handleSerial() {
//	if (Serial.available() == 0) return;
//	serialIn = Serial.read();
//	Serial.print("serial> ");
//	Serial.println(serialIn);
//
//    switch (serialIn)
//  {
//	case 's':
//		test();
//		break;
//
//  case 'z':
//    testIo();
//    break;
//    
//	case 'x':
//    actIo++;
//    if(actIo>16) actIo=0;
//		actStatttt = houz.getIo(actIo);
//		Serial.print("io> ");
//		Serial.println(actStatttt);
//		houz.setIo(actIo, !actStatttt);
//		break;
//
//	case 'a':
//	 Serial.println('>a');
//     word myAnim[] = { 0xF, 0xFF, 0xFFF, 0xFFFF, 0x0};
//     houz.ioAnim(5, myAnim, 2000);
//    break;
//  }
//}


void test(){
   for (int i=0; i <= 16; i++){
      houz.setIo(i, true);
      delay(250);
   }
   for (int i=16; i>=5; i--){
      houz.setIo(i, false);
      delay(250);
   }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// I/O
void switchRead() {
	int buttonState;
	buttonState = digitalRead(inSwitch);
	if (buttonState == HIGH) return;
	Serial.println("[switchRead] inSwitch pressed");

	//handle status

	//notify server
	houz.statusLedBlink();
	houz.radioSend(CMD_EVENT, living_switch, 1);
	houz.radioSend(CMD_VALUE, living_light, houz.getIoStatus());
	
	//debounce
	//delay(500); 
}

