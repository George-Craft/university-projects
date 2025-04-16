#include <Arduino.h>
#include <string.h>
#include "onLED.h"

onLED :: onLED(int pinNum, String name):LED(pinNum,name)
{
  ledPin = pinNum;
  ledName = name;   
  onTime = 0;
  offTime = 0;
  pinMode(ledPin, OUTPUT);
       
}
void onLED:: on(unsigned long StateTime)
{
  digitalWrite(ledPin,HIGH);
  displayLedStatus(ledName);
  delay(StateTime);
  digitalWrite(ledPin,LOW);
}
void onLED :: off(unsigned long StateTime)
{
  digitalWrite(ledPin,LOW);
  displayLedStatus(ledName);
  delay(StateTime);
  digitalWrite(ledPin,HIGH);
}
void onLED::flash(int flashLimit, unsigned long OnTime, unsigned long OffTime) //flashes on and off 
{
  int i;
  onTime = OnTime; //how long led stays on
  offTime = OffTime; //how long led stays off
  for(i=0;i<flashLimit;i++)
  {
    on(onTime);
    off(offTime);
  }
  Off();
  
}