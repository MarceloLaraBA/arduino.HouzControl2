/*
 Name:		LightTemp_test.ino
 Created:	06-Jan-18 16:20:34
 Author:	DarkAngel
*/
#include <RF24_config.h>
#include <RF24.h>
#include <printf.h>
#include <nRF24L01.h>
#include <HouzEnviroment.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#define ONE_WIRE_BUS 2

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
DeviceAddress dallasAddress;

void setup() {
	Serial.begin(115200);


	//dallas setup
	sensors.begin();

	Serial.print("Found ");
	Serial.print(sensors.getDeviceCount(), DEC);
	Serial.println(" devices.");
	if (!sensors.getAddress(dallasAddress, 0)) Serial.println("Unable to find address for Device 0");




}

// the loop function runs over and over again until power down or reset
void loop() {
	
	//temp 
	sensors.requestTemperatures(); // Send the command to get temperatures
	float tempC = sensors.getTempC(dallasAddress);
	Serial.print(tempC);
	Serial.print(",");

	//light
	int ldrData = analogRead(A0);
	Serial.println((float)(ldrData * .01));
}
