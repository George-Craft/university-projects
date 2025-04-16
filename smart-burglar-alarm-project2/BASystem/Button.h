#ifndef Button_h
#define Button_h
#include "Sensor.h"

class Button : public Sensor
{
  public:
    Button(int);
    void waitButtonInput();
};

#endif