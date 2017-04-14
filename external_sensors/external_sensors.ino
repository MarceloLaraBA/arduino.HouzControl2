/*
 Name:		external_sensors.ino
 Created:	05-Apr-17 21:42:21
 Author:	DarkAngel
*/

#include <HouzDevices.h>
#include <OneWire.h>
#include <DallasTemperature.h>


//Radio Setup
#define rfRecvLed 9 //Led
#define rfCE 7      //RF pin 3 (CE)
#define rfCS 8      //RF pin 4 (CS)
RF24 radio(rfCE, rfCS);


//light sensor
#define lightSensorPin A0

//temperature
#define ONE_WIRE_BUS 2
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature dallasTemp(&oneWire);

//Houz Setup
HouzDevices houz(external_node, radio, rfRecvLed, Serial);

void setup() {
	Serial.begin(115200);
	houz.radioSetup(rfRecvLed);

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



	float temp = temperature();

	u32 txTemp = houz.encode(CMD_QUERY, external_tempSensor, 0xAAAA);
	Serial.print("encoded: 0x");
	Serial.print(txTemp, HEX);

	deviceData device;
	device = houz.decode(txTemp);

	Serial.print(" | decoded: [id:");
	Serial.print(device.id, HEX);
	Serial.print("|cmd:");
	Serial.print(device.cmd, HEX);
	Serial.print("|payload:");
	Serial.print(device.payload, HEX);
	Serial.print("]");

	Serial.println();
	delay(1000);
}

void handleCommand(deviceData device) {
	Serial.println("handleCommand");
	switch (device.id)
	{
	case external_lightSensor:
		switch (device.cmd)
		{
		case CMD_QUERY:
			houz.radioSend(CMD_VALUE, external_lightSensor, lightLevel());
			break;
		default:
			break;
		}
		break;
	default:
		break;
	}

};




// Sensor values
u32 lightLevel() {
	return analogRead(lightSensorPin);
}

float temperature() {
	dallasTemp.requestTemperatures();
	return dallasTemp.getTempCByIndex(0);// Why "byIndex"? You can have more than one IC on the same bus. 0 refers to the first IC on the wire
}