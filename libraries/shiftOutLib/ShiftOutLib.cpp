#include "Arduino.h"
#include "ShiftOutLib.h"

ShiftOutLib::ShiftOutLib(byte PIN_CLOCK, byte PIN_LATCH, byte PIN_DATA, int SHIFT_COUNT){

  //set output
  pinMode(PIN_CLOCK, OUTPUT);
  pinMode(PIN_LATCH, OUTPUT);
  pinMode(PIN_DATA, OUTPUT);

  //save values
	_PIN_CLOCK=PIN_CLOCK;  
	_PIN_LATCH=PIN_LATCH;
	_PIN_DATA=PIN_DATA;
	_SHIFT_COUNT = SHIFT_COUNT;
	_lastSentValue=0;
  }

void ShiftOutLib::send(int outValue){
	_lastSentValue = outValue;

}

void ShiftOutLib::LastValue(){
	return _lastSentValue;
}
