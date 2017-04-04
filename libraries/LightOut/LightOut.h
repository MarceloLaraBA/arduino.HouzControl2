#ifndef LightOut_h
#define LightOut_h

#include "Arduino.h"

class LightOut
{

  public:
    LightOut(byte DATA_pin, byte LATCH_pin, byte CLOCK_pin);
    void on(byte lightNum);
    void off(byte lightNum);
    void toggle(byte lightNum);
    void send();

    word output;

  private:
    byte DATA_PIN;
    byte LATCH_PIN;
    byte CLOCK_PIN;
};

#endif
