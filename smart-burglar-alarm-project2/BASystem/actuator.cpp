#include "actuator.h"
#include <Arduino.h>

actuator :: actuator(int PinNum)
{
  PinNumber = PinNum;
}

void actuator :: High()
{
  
  pinMode(PinNumber, OUTPUT);
  actuatorState = 1;
  digitalWrite(PinNumber, HIGH);
}

void actuator :: Low()
{
  pinMode(PinNumber, OUTPUT); 
  actuatorState = 0;
  digitalWrite(PinNumber, LOW);
}
