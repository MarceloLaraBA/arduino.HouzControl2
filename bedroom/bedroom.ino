/*
Reference
- RF nRF24L01+
http://blog.brunodemartino.com.ar/2013/11/arduino-raspberry-pi-talking-wirelessly/
wiring: D7>CS | D8>CE | (sck) D13>5 | (mosi) D11>6 | (miso) D12>7

- IR Receiver
IRM-8601S

*/

#include <HouzInfrared.h>
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
bool lightOn = 1;
bool airConditionerOn = 0;
int  airConditionerTemp = 24;
bool tvOn = 0;

//TODO: dim wallSwlight
//ir setup
IRrecv irrecv(irRecvPin);
IRsend irsend;

//radio setup
#define rfRecvLed 9 //RF Led
#define rfCE 8      //RF pin 3 (CE)
#define rfCS 7      //RF pin 4 (CS)
RF24 radio(rfCE, rfCS);
HouzDevices houz(bedroom_node, radio, swLight, Serial);


void setup() {
	//serial debug
	Serial.begin(115200);

	//ir setup
	Serial.println("\n\r::IR setup");
	irrecv.enableIRIn();

	//radio
	houz.radioSetup();

	//switch setup
	pinMode(inSwitch, INPUT_PULLUP);
	pinMode(lightOut, OUTPUT);
	setCeilingLight(1);

	//debug
	Serial.println("\r\n-- bedroom_node --");
}

void loop()
{
	if (houz.radioRead()) handleCommand(houz.receivedData()); 
	infraredRead();  //handle IR
	switchRead();   //lightSwitch touch
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// RF communication

void handleCommand(deviceData device) {
	Serial.println("\r" + houz.deviceToString(device));
	switch (device.id) {
	case bedroom_light:
    Serial.println("bedroom_light");
		if (device.cmd == CMD_SET){
			setCeilingLight(device.payload == 1);
		}else{
			delay(200);
			houz.radioSend(CMD_VALUE, bedroom_light, lightOn ? 1 : 0);
		};
		break;
	case bedroom_AC:
		Serial.println("bedroom_AC");
		if (device.cmd == CMD_SET)
			ACsendCommand(device.payload == 1 ? acBghPowerOn : acBghPowerOff);
		else
			delay(200);
			houz.radioSend(CMD_VALUE, bedroom_AC, airConditionerOn ? 1 : 0);
		break;
	case bedroom_AC_temp:
		Serial.println("bedroom_AC_temp");
		if (device.cmd == CMD_SET) {
			airConditionerTemp = (device.payload);
			ACsendCommand(ACtempCode(airConditionerTemp));
		}
		else
			delay(200);
			houz.radioSend(CMD_VALUE, bedroom_AC_temp, airConditionerTemp);
		break;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// IR Remote Control


void infraredRead() {
	decode_results results;
	if (irrecv.decode(&results)) {  // Grab an IR code
		if (results.value != 0xFFFFFFFF) {
			Serial.print("\n\rIR recv> ");
			handleIrCode(results.value);
			houz.statusLedBlink();
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
		houz.radioSend(CMD_EVENT, bedroom_ir, bedroom_light);
		setCeilingLight(!lightOn);
		break;

	//turn on/off AC
	case irDvrA:
		houz.radioSend(CMD_EVENT, bedroom_switch, bedroom_AC);
		airConditionerOn = !airConditionerOn;
		ACsendCommand(airConditionerOn ? acBghPowerOn : acBghPowerOff);
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
		houz.radioSend(CMD_EVENT, bedroom_ir, irCode);
		Serial.println(irCode, HEX);
		break;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Wall switch

void switchRead() {
	int buttonState;
	buttonState = digitalRead(inSwitch);
	if (buttonState == HIGH) return;

	Serial.println("switch> pressed..");
	setCeilingLight(!lightOn);
	houz.radioSend(CMD_EVENT, bedroom_switch, bedroom_light);
	delay(500); //debounce
}



////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Ceiling Light
void setCeilingLight(bool status) {
	lightOn = status;
	digitalWrite(lightOut, lightOn? LOW: HIGH);
	delay(200);
	houz.radioSend(CMD_VALUE, bedroom_light, status ? 1 : 0);
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Air Conditioner

void ACsendCommand(unsigned long acCode) {
	//store status
	if (acCode == acBghPowerOn) { 
		airConditionerOn = 1; 
		delay(200);
		houz.radioSend(CMD_VALUE, bedroom_AC, 1);
	}
	if (acCode == acBghPowerOff) { 
		airConditionerOn = 0; 
		delay(200);
		houz.radioSend(CMD_VALUE, bedroom_AC, 0);
	}

	//send command
	for (int i = 0; i < 3; i++) {
		irsend.sendLG(acCode, 28);
		delay(40);
	}
	irrecv.enableIRIn();

}
unsigned long ACtempCode(u8 temp) {
	switch (temp)
	{
	case 18: return acBghTemp18;
	case 19: return acBghTemp19;
	case 20: return acBghTemp20;
	case 21: return acBghTemp21;
	case 22: return acBghTemp22;
	case 23: return acBghTemp23;
	case 24: return acBghTemp24;
	case 25: return acBghTemp25;
	case 26: return acBghTemp26;
	default: return acBghTemp24;
	}
}

