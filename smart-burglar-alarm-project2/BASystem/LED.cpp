#include <Arduino.h>
#include <string.h>
#include "LED.h"


LED :: LED(int pinNum, String name)
{
  ledPin = pinNum;
  ledName = name;
  stateTime = 0;
  pinMode(ledPin, OUTPUT);
}
void LED :: On()
{
  digitalWrite(ledPin,HIGH);
}
void LED :: Off()
{
  digitalWrite(ledPin,LOW);
}

void LED :: displayLedStatus(String name)
 {
  Serial.print("status of ");
  Serial.print(name);
  Serial.print(": ");
  Serial.println(digitalRead(ledPin));
 }
