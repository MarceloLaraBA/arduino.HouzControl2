/*
 Name:		external_sensors.ino
 Created:	05-Apr-17 21:42:21
 Author:	DarkAngel
*/

#include <OneWire.h>
#include <DallasTemperature.h>
#include <RFencoder.h>
#include <RF24_config.h>
#include <RF24.h>
#include <printf.h>
#include <nRF24L01.h>

//radio setup
#define rfRecvLed 9 //Led
#define rfCE 7      //RF pin 3 (CE)
#define rfCS 8      //RF pin 4 (CS)
#define rfChannel 0x5D   
#define rfPipeMain 0xF0F0F0F0AA
#define rfPipeBedr 0xF0F0F0F066
bool radioReady = false;
RF24 radio(rfCE, rfCS);
RFencoder rfEncoder;


//light sensor
#define lightSensorDeviceId 01
#define lightSensorPin A0

//temperature
#define tempSensorDeviceId 02
#define ONE_WIRE_BUS 2
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature dallasTemp(&oneWire);

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
	radioReady = (rfChannel == radio.getChannel());
	if(radioReady){
		radio.printDetails();
		radio.startListening();
	}
	Serial.print("radio: ");
	Serial.println(radioReady?"online":"offline");

	//light sensor setup
	pinMode(lightSensorPin, INPUT);

	//temperature
	dallasTemp.begin();

	Serial.println("");
	Serial.println("-- runtime --");
}

void loop() {
	radioRead();


	Serial.print("light: ");
	u32 txLight = rfEncoder.encode(lightSensorDeviceId, lightLevel());
	Serial.println(txLight, HEX);

	//temp
	Serial.print("temp: ");
	u32 temp = rfEncoder.encode(tempSensorDeviceId, (temperature() * 100));
	Serial.println(temp, HEX);


	delay(1000);
}



////////////////////////////////////////////////////////////////////////////////////////////////////////////
// RF communication

void radioRead() {
	if (!radioReady) { return; };
	unsigned long rfMessage;
	if (radio.available()) {
		while (radio.available()) {             // While there is data ready
			radio.read(&rfMessage, sizeof(unsigned long));  // Get the payload
			digitalWrite(rfRecvLed, HIGH);       // Notify receive
		}
		Serial.print("RF recv> ");
		delay(500);
		radio.startListening();
		digitalWrite(rfRecvLed, LOW);
	}
}


// Sensor values
u32 lightLevel() {
	return analogRead(lightSensorPin);
}

float temperature() {
	dallasTemp.requestTemperatures();
	return dallasTemp.getTempCByIndex(0);// Why "byIndex"? You can have more than one IC on the same bus. 0 refers to the first IC on the wire
}