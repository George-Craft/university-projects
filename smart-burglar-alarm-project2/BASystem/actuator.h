#ifndef actuator_h
#define actuator_h
#include <Arduino.h>

class actuator
{
  protected:
    int PinNumber;
    bool actuatorState;
  public:
    actuator(int);
    void High();
    void Low();

};

#endif