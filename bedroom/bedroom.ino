/*
Reference
- RF nRF24L01+
http://blog.brunodemartino.com.ar/2013/11/arduino-raspberry-pi-talking-wirelessly/
wiring: D7>CS | D8>CE | (sck) D13>5 | (mosi) D11>6 | (miso) D12>7

- IR Receiver
IRM-8601S

*/

#include <RF24_config.h>
#include <nRF24L01.h>
#include <IRremoteInt.h>
#include <IRremote.h>
#include <RF24.h>
#include <printf.h>
#include <SPI.h>

#define acPowerOn 0x880074B
#define acPowerOff 0x88C0051

#define tvPower 0x20DF10EF

//hardware setup
#define irRecvPin 6   //IRM-8601S
#define irSndPin  3   //IR Led (can't be changed)
#define inSwitch A0   //wall switch
#define swLight  5    //wall led indicator
#define lightOut 2    //relay


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
#define rfChannel 0x5D   
#define rfPipeMain 0xF0F0F0F0AA
#define rfPipeBedr 0xF0F0F0F066
RF24 radio(rfCE, rfCS);

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
	radio.openWritingPipe(rfPipeMain);
	radio.openReadingPipe(1, rfPipeBedr);
	radio.printDetails();
	radio.startListening();

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
	unsigned long rfMessage;
	if (radio.available()) {
		while (radio.available()) {             // While there is data ready
			radio.read(&rfMessage, sizeof(unsigned long));  // Get the payload
			digitalWrite(rfReceiveLed, HIGH);       // Notify receive
		}
		Serial.print("RF recv> ");
		handleIrCode(rfMessage);
		delay(500);
		radio.startListening();
		digitalWrite(rfReceiveLed, LOW);
	}
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////
// IR Remote Control
#define irDvr1 0xB59    //dvr: keypad 1
#define irDvr2 0x80B59    //dvr: keypad 2
#define irDvr3 0x40B59    //dvr: keypad 3
#define irDvr4 0xC0B59    //dvr: keypad 4
#define irDvr5 0x20B59    //dvr: keypad 5
#define irDvr6 0xA0B59    //dvr: keypad 6
#define irDvr7 0x60B59    //dvr: keypad 7
#define irDvr8 0xE0B59    //dvr: keypad 8
#define irDvr9 0x10B59    //dvr: keypad 9
#define irDvr0 0xD0B59    //dvr: keypad 0
#define irDvrEnter 0x90B59  //dvr: keypad Enter
#define irDvrA 0xDAB59    //dvr: key A
#define irDvrB 0x4EB59    //dvr: key B

#define irDvrUp 0xFAB59   //dvr: joystick up
#define irDvrRight 0x86B59  //dvr: joystick right
#define irDvrLeft 0x46B59 //dvr: joystick left
#define irDvrDown 0x6B59  //dvr: joystick down
#define irDvrCenter 0x7AB59 //dvr: joystick enter

void infraredRead() {
	decode_results results;
	if (irrecv.decode(&results)) {  // Grab an IR code
		if (results.value != 0xFFFFFFFF) {
			Serial.print("\n\rIR recv> ");
		}

		handleIrCode(results.value);
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

