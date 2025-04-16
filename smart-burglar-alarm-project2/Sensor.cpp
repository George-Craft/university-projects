#include "sensor.h"
#include <Arduino.h>

Sensor :: Sensor(int PinNum)
{
  pinNum = PinNum;
}
void Sensor :: setInputPullUp()
{
  pinMode(pinNum,INPUT_PULLUP);
}
void Sensor :: setInput()
{
  pinMode(pinNum,INPUT);
}
int Sensor :: getSensorReading()
{
  return digitalRead(pinNum);
}
