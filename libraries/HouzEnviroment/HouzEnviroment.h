#ifndef HouzEnviroment_h
#define HouzEnviroment_h

#include "Arduino.h"

class HouzEnviroment
{

  public:
    HouzEnviroment();
    bool isDay();
    bool doorIsOpen();
    void setDaySensorPin(byte PIN);
    void setDoorSensorPin(byte PIN);

  private:
    byte DAY_SENSOR_PIN;
    byte DOOR_SENSOR_PIN;

};

#endif
