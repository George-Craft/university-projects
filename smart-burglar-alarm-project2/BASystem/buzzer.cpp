#include <Arduino.h>
#include "buzzer.h"

buzzer ::buzzer(int PinNum):actuator(PinNum)
{
  PinNumber = PinNum;
  buzzerTone = 200;
  buzzerLimit = 30;//30 seconds //1800000; //buzzer limit set to 30 mins
  pinMode(PinNumber, OUTPUT);
}

void buzzer ::beep(int beepTime)
{
  int i;
  for(int i=0;i<beepTime;i++)
  {
    tone(PinNumber, buzzerTone);
    delay(500);
    noTone(PinNumber);
    delay(500);
  } 
}

long buzzer :: getBuzzerLimit()
{
  return buzzerLimit;
}

void buzzer :: High()
{
  tone(PinNumber, buzzerTone);
}

void buzzer :: Low()
{
  noTone(PinNumber);  
}
