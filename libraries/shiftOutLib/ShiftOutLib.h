#ifndef ShiftOutLib_h
#define ShiftOutLib_h

#include "Arduino.h"

class ShiftOutLib
{
  public:
    ShiftOutLib(byte PIN_CLOCK, byte PIN_LATCH, byte PIN_DATA, byte SHIFT_COUNT);
	void Send(int outValue);
	int LastValue();
	
	private:
    byte _PIN_CLOCK;
    byte _PIN_LATCH;
	byte _PIN_DATA;
	byte _SHIFT_COUNT
	
	int _lastSentValue;
};

#endif