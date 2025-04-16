#ifndef buzzer_h
#define buzzer_h
#include "actuator.h"

class buzzer : public actuator
{
  private:
    int buzzerLimit;
    int buzzerTone;
  public:
    buzzer(int);
    void beep(int);
    void onTillLimit();
    long getBuzzerLimit();
    void High();
    void Low();
};

#endif