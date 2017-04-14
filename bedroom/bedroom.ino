/*
Reference
- RF nRF24L01+
http://blog.brunodemartino.com.ar/2013/11/arduino-raspberry-pi-talking-wirelessly/
wiring: D7>CS | D8>CE | (sck) D13>5 | (mosi) D11>6 | (miso) D12>7

- IR Receiver
IRM-8601S

*/

#include <HouzDevices.h>
#include <IRremote.h>

//hardware setup
#define irRecvPin	6	//IRM-8601S
#define irSndPin	3	//IR Led (can't be changed)
#define inSwitch	A0	//wall switch
#define swLight		5	//wall led indicator
#define swLightLvl	200	//wall led indicator level
#define lightOut	2	//relay


//functional
bool lightOn = 0;
bool airConditionerOn = 0;
int  airConditionerTemp = 24;
bool tvOn = 0;

//TODO: dim wallSwlight
//ir setup
IRrecv irrecv(irRecvPin);
IRsend irsend;

//radio setup
#define rfReceiveLed 9  //
#define rfCE 8      //RF pin 3 (CE)
#define rfCS 7      //RF pin 4 (CS)
RF24 radio(rfCE, rfCS);
HouzDevices devices(bedroom_node, radio, rfReceiveLed, Serial);


void setup() {
	//serial debug
	Serial.begin(115200);
	Serial.println("-- setup --");

	//radio setup
	//Serial.println("::RF setup");
	//printf_begin();
	//radio.begin();
	//radio.setPALevel(RF24_PA_HIGH);
	//radio.setChannel(rfChannel);
	//radio.openWritingPipe(rfPipeMain);
	//radio.openReadingPipe(1, rfPipeBedr);
	//radio.printDetails();
	//radio.startListening();

	//ir setup
	Serial.println("\n\r::IR setup");
	irrecv.enableIRIn();

	//switch setup
	pinMode(inSwitch, INPUT_PULLUP);
	pinMode(lightOut, OUTPUT);
	pinMode(swLight, OUTPUT);
	digitalWrite(swLight, LOW);
	digitalWrite(lightOut, lightOn);

	//debug
	Serial.println("");
	Serial.println("-- ready --");
	digitalWrite(swLight, HIGH);
}

void loop()
{
	radioRead();  //handle RF
	infraredRead(); //handle IR
	switchRead(); //lightSwitch touch
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// RF communication

void radioRead() {
	//unsigned long rfMessage;
	//if (radio.available()) {
	//	while (radio.available()) {             // While there is data ready
	//		radio.read(&rfMessage, sizeof(unsigned long));  // Get the payload
	//		digitalWrite(rfReceiveLed, HIGH);       // Notify receive
	//	}
	//	Serial.print("RF recv> ");
	//	handleIrCode(rfMessage);
	//	delay(500);
	//	radio.startListening();
	//	digitalWrite(rfReceiveLed, LOW);
	//}
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////
// IR Remote Control


void infraredRead() {
	decode_results results;
	if (irrecv.decode(&results)) {  // Grab an IR code
		if (results.value != 0xFFFFFFFF) {
			Serial.print("\n\rIR recv> ");
			handleIrCode(results.value);
		}
		irrecv.resume();              // Prepare for the next value
	}

}
void handleIrCode(unsigned long irCode) {

	if (irCode == 0xFFFFFFFF) { return; }
	switch (irCode)
	{
		//turn light on
	case irDvrCenter:
		toggleLightRelay();
		break;

		//turn on/off AC
	case irDvrA:
		airConditionerOn = !airConditionerOn;
		ACsendCommand(airConditionerOn ? acPowerOn : acPowerOff);
		break;

		//TODO: AC temp up
		//TODO: AC temp down

		//TV Power
	case tvPower:
		Serial.println("IR snd> tvPower");
		irsend.sendLG(tvPower, 28);
		irrecv.enableIRIn();
		break;

	case 0x6B59: //down
		Serial.println("down");
		break;


	default:
		Serial.print("unknown: 0x");
		Serial.println(irCode, HEX);
		break;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Wall switch

void switchRead() {
	int buttonState;
	buttonState = digitalRead(inSwitch);
	if (buttonState != HIGH) {
		Serial.println("switch> pressed..");
		toggleLightRelay();
		delay(500); //debounce
	}

}
void toggleLightRelay() {
	lightOn = !lightOn;
	Serial.print("relay>");
	Serial.println(lightOn);
	digitalWrite(lightOut, lightOn);
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Air Conditioner
void ACsendCommand(unsigned long acCode) {
	//store status
	if (acCode == acPowerOn) { airConditionerOn = 1; }
	if (acCode == acPowerOff) { airConditionerOn = 0; }

	//send command
	for (int i = 0; i < 3; i++) {
		irsend.sendLG(acCode, 28);
		delay(40);
	}
	irrecv.enableIRIn();

	//display status
	Serial.print("[sent ");
	Serial.print(acCode, HEX);
	Serial.print("] AirConditioner ");
	Serial.print(airConditionerOn == 1 ? "On" : "Off");
	Serial.print(" | temp: ");
	Serial.println(airConditionerTemp);
}

