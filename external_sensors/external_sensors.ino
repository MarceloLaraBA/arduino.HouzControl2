/*
 Name:		external_sensors.ino
 Created:	05-Apr-17 21:42:21
 Author:	DarkAngel
*/

#include <HouzDevices.h>
#include <OneWire.h>
#include <DallasTemperature.h>


//Houz/Radio Setup
#define rfRecvLed 9 //Led
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

void setup() {
	Serial.begin(115200);
	houz.radioSetup();

	//light sensor setup
	pinMode(lightSensorPin, INPUT);

	//temperature
	dallasTemp.begin();

	Serial.println("\r\n-- runtime --");
}

void loop() {
	if (houz.radioRead()) {
		handleCommand(houz.receivedData());
	};
}

void handleCommand(deviceData device) {
	Serial.println("handleCommand");
	switch (device.id){
	
	case external_lightSensor:
		delay(100);
		houz.radioSend(CMD_VALUE, external_lightSensor, lightLevel());
		break;

	case external_tempSensor:
		delay(100);
		houz.radioSend(CMD_VALUE, external_tempSensor, temperature()*100);
		break;

	default:
		Serial.println("unhandled: " + houz.deviceToString(device));
		break;
	}

};



///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Sensor values
u32 lightLevel() {
	return analogRead(lightSensorPin);
}

float temperature() {
	dallasTemp.requestTemperatures();
	return dallasTemp.getTempCByIndex(0);// Why "byIndex"? You can have more than one IC on the same bus. 0 refers to the first IC on the wire
}