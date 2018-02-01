

/*
 Name:		external_sensors.ino
 Created:	05-Apr-17 21:42:21
 Author:	DarkAngel
*/

#include <HouzDevices.h>
#include <OneWire.h>
#include <DallasTemperature.h>


//Houz/Radio Setup
#define rfRecvLed 4 //Led
#define rfCE 9      //RF pin 3 (CE)
#define rfCS 10      //RF pin 4 (CS)
RF24 radio(rfCE, rfCS);
HouzDevices houz(external_node, radio, rfRecvLed, Serial);

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
	pinMode(ceilingLightLed, OUTPUT);
	pinMode(ceilingLightSwitch, INPUT_PULLUP);
	analogWrite(ceilingLightLed, 128);
	digitalWrite(ceilingLightPin, HIGH);

	Serial.println("\r\n-- external_node --");
	Serial.println("r\t radio status");
	Serial.println("a\t AC on/off");
	Serial.println("c\t ceiling light on/off");
	Serial.println("l\t send light level");
	Serial.println("t\t send temp level");
	Serial.print(">");

}

char serialIn;
void loop() {
	if (houz.radioRead()) {handleCommand(houz.receivedData());};
	handleSerial();
	switchRead(); //lightSwitch touch
}

void handleCommand(deviceData device) {
	Serial.println("handleCommand: " + houz.deviceToString(device));
	
	switch (device.id){
		case external_lightSensor:
			delay(200);
			houz.radioSend(CMD_VALUE, external_lightSensor, lightLevel());
			break;

		case external_tempSensor:
			delay(200);
			houz.radioSend(CMD_VALUE, external_tempSensor, temperature()*100);
			break;

		case office_light:
			if(device.cmd==CMD_SET){
				setLight(ceilingLightPin, (device.payload==1));
			};
			houz.radioSend(CMD_VALUE, office_light, getLight(ceilingLightPin)?1:0);
			break;

		default:
			Serial.println("unhandled: " + houz.deviceToString(device));
			break;
	}

};

void handleSerial() {
	if (Serial.available() == 0) return;

	serialIn = Serial.read();
	Serial.println(serialIn);
	switch (serialIn)
	{
	case 'r':case 'R':
		houz.radioReady();
		break;

	case 'l':case 'L':
		houz.radioWrite(houz.encode(CMD_VALUE, external_lightSensor, lightLevel()), server_node);
		break;

	case 't':case 'T':
		houz.radioWrite(houz.encode(CMD_VALUE, external_tempSensor, temperature()), server_node);
		break;

	case 'a':case 'A':
		airConditionerOn = !airConditionerOn;
		ACsendCommand(airConditionerOn ? acPowerOn : acPowerOff);
		houz.radioWrite(houz.encode(CMD_VALUE, office_AC, airConditionerOn ? 1 : 0), server_node);
		break;

	case 'c':case 'C':
		setLight(ceilingLightPin, !getLight(ceilingLightPin));
		houz.radioWrite(houz.encode(CMD_VALUE, office_light, getLight(ceilingLightPin) ? 1 : 0), server_node);
		break;

	default:
		Serial.println("?");
		break;
	}

	Serial.print(">");
}


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
	Serial.println("switch> pressed..");
	setLight(ceilingLightPin, !getLight(ceilingLightPin));
	houz.radioSend(CMD_VALUE, office_light, getLight(ceilingLightPin) ? 1 : 0);
	delay(500); //debounce
	
}
bool setLight(u8 pin, bool status) {
	digitalWrite(pin, status ? HIGH : LOW);
	return true;
};
bool getLight(u8 pin) {
	return digitalRead(pin) == HIGH;
};



///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// infrared
void ACsendCommand(unsigned long acCode) {
  //store status
  if (acCode == acPowerOn) { airConditionerOn = 1; }
  if (acCode == acPowerOff) { airConditionerOn = 0; }

  //send command
  for (int i = 0; i < 3; i++) {
    irsend.sendLG(acCode, 28);
    delay(40);
  }
  //irrecv.enableIRIn();

  //display status
  Serial.print("[sent ");
  Serial.print(acCode, HEX);
  Serial.print("] AirConditioner ");
  Serial.print(airConditionerOn == 1 ? "On" : "Off");
  Serial.print(" | temp: ");
  Serial.println(airConditionerTemp);
}

