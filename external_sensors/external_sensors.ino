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

void setup() {
	Serial.begin(115200);
	houz.radioSetup();

	//light sensor setup
	pinMode(lightSensorPin, INPUT);

	//temperature
	dallasTemp.begin();

	Serial.println("\r\n-- external_node --");
	Serial.println("r\t radio status");
	Serial.println("l\t send light level");
	Serial.println("t\t send temp level");
	Serial.println("q\t query sensor values");
}

char serialIn;
void loop() {
	if (houz.radioRead()) {
		handleCommand(houz.receivedData());
	};
	if (Serial.available() > 0) {
		serialIn = Serial.read();
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

		case 'q':case 'Q':
			Serial.println();
			Serial.print("temp: ");
			Serial.println(temperature());
			Serial.print("light: ");
			Serial.println(lightLevel());
			break;

		default:
			Serial.print(serialIn);
			Serial.println("?");
			break;
		}

		Serial.println(">");

	}
}

void handleCommand(deviceData device) {
	Serial.println("handleCommand: " + houz.deviceToString(device));
	
	switch (device.id){
		case external_lightSensor:
			delay(500);
			houz.radioSend(CMD_VALUE, external_lightSensor, lightLevel());
			break;

		case external_tempSensor:
			delay(500);
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