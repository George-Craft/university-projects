#ifndef Sensor_h
#define Sensor_h
#include <Arduino.h>

class Sensor
{
  protected:
    int sensorStatus;
    int pinNum;
  public:
    Sensor(int);
    void setInputPullUp();
    void setInput();
    int getSensorReading();
};

#endif