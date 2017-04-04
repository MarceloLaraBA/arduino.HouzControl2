#include "Arduino.h"
#include "LightOut.h"

LightOut::LightOut(byte DATA_pin, byte LATCH_pin, byte CLOCK_pin){

//set putputs
  pinMode(DATA_pin, OUTPUT);
  pinMode(LATCH_pin, OUTPUT);
  pinMode(CLOCK_pin, OUTPUT);

//save variables
  DATA_PIN = DATA_pin;
  LATCH_PIN = LATCH_pin;
  CLOCK_PIN = CLOCK_pin;

  //initial setup
  output=0;
  send();
}

void LightOut::on(byte lightNum){
  bitSet(output, lightNum);
  send();
}

void LightOut::off(byte lightNum){
  bitClear(output, lightNum);
  send();
}

void LightOut::toggle(byte lightNum){
  bitWrite(output, lightNum, !(bitRead(output, lightNum)));
  send();
}

void LightOut::send(){
  digitalWrite(LATCH_PIN, LOW);
  shiftOut(DATA_PIN, CLOCK_PIN,MSBFIRST, output >> 8);
  shiftOut(DATA_PIN, CLOCK_PIN,MSBFIRST, output & 0xFF);
  digitalWrite(LATCH_PIN, HIGH);
}

