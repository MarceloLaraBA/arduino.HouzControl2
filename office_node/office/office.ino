/*
Name:		external_sensors.ino
Created:	05-Apr-17 21:42:21
Author:	DarkAngel
*/

#include <HouzDevices.h>
#include <OneWire.h>
#include <DallasTemperature.h>

//light sensor
#define lightSensorPin A0

//temperature
#define ONE_WIRE_BUS 2
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature dallasTemp(&oneWire);

//lights
#define ceilingLightPin 8
#define ceilingLightSwitch 7
#define ceilingLightLed 6

//ir control
#include <boarddefs.h>
#include <IRremote.h>
#include <IRremoteInt.h>
#include <ir_Lego_PF_BitStreamEncoder.h>
//#define irRecvPin  6 //IRM-8601S
//IRrecv irrecv(irRecvPin);
#define irSndPin  3 //IR Led (can't be changed)
IRsend irsend;

//Houz/Radio Setup
#define rfCE 9		//RF pin 3 (CE)
#define rfCS 10		//RF pin 4 (CS)
RF24 radio(rfCE, rfCS);
HouzDevices houz(office_node, radio, ceilingLightLed, Serial);


///////////////////////////
//functional
bool lightOn = 0;
bool airConditionerOn = 0;
int  airConditionerTemp = 24;
//////////////////


void setup() {
	Serial.begin(115200);
	houz.radioSetup();

	//light sensor setup
	pinMode(lightSensorPin, INPUT);

	//temperature
	dallasTemp.begin();

	//IR 

	//ceiling light
	pinMode(ceilingLightPin, OUTPUT);
	pinMode(ceilingLightSwitch, INPUT_PULLUP);
	digitalWrite(ceilingLightPin, HIGH);
}

char serialIn;
void loop() {
	if (houz.radioRead())	handleCommand(houz.receivedData()); 
	if (houz.serialRead())	handleCommand(houz.serialData());
	switchRead(); //lightSwitch touch
}

void handleCommand(deviceData device) {
	switch (device.id) {
	case external_lightSensor:
		Serial.println("external_lightSensor");
		houz.radioSend(CMD_VALUE, external_lightSensor, lightLevel());
		break;

	case external_tempSensor:
		Serial.println("external_tempSensor");
		houz.radioSend(CMD_VALUE, external_tempSensor, temperature() * 100);
		break;

	case office_light:
		Serial.println("office_light");
		if (device.cmd == CMD_SET) setLight(device.payload == 1);
		houz.radioSend(CMD_VALUE, office_light, lightOn? 1 : 0);
		break;

	case office_switchLed:
		Serial.println("office_switchLed");
		if (device.cmd == CMD_SET) analogWrite(ceilingLightLed, (int)device.payload);
		houz.radioSend(CMD_VALUE, office_switchLed, analogRead(ceilingLightLed));
		break;

	case office_AC:
		Serial.println("office_AC");
		if (device.cmd == CMD_SET) {
			airConditionerOn = (device.payload == 1);
			ACsendCommand(airConditionerOn ? acBghPowerOn : acBghPowerOff);
		}
		houz.radioSend(CMD_VALUE, office_AC, airConditionerOn ? 1 : 0);
		break;

	case office_AC_temp:
		if (device.cmd == CMD_SET) {
			airConditionerTemp = (device.payload);
			ACsendCommand(ACtempCode(airConditionerTemp));
		}
		houz.radioSend(CMD_VALUE, office_AC_temp, airConditionerTemp);
		break;

	default:
		Serial.println("unhandled: " + houz.deviceToString(device));
		break;
	}

};


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Sensors
u32 lightLevel() {
	return analogRead(lightSensorPin);
};

float temperature() {
	dallasTemp.requestTemperatures();
	return dallasTemp.getTempCByIndex(0);// Why "byIndex"? You can have more than one IC on the same bus. 0 refers to the first IC on the wire
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// I/O
void switchRead() {
	int buttonState;
	buttonState = digitalRead(ceilingLightSwitch);
	if (buttonState == HIGH) return;

	setLight(!lightOn);

	houz.statusLedBlink();
	houz.radioSend(CMD_EVENT, office_switch, 1);
	delay(500); //debounce
}

bool setLight(bool status) {
	lightOn = status;
	digitalWrite(ceilingLightPin, status ? LOW : HIGH);
	houz.radioSend(CMD_VALUE, office_light, lightOn ? 1 : 0);
	return true;
};

u8 getValue(u8 pin) {
	return analogRead(pin);
}



///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// infrared
void ACsendCommand(unsigned long acCode) {
	//store status
	switch (acCode)
	{
	case acBghPowerOn:
		airConditionerOn = 1;
	case acBghPowerOff:
		airConditionerOn = 0;
	default:
		break;
	}

	//send command
	for (int i = 0; i < 3; i++) {
		irsend.sendLG(acCode, 28);
		delay(100);
	}

	//irrecv.enableIRIn();
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
