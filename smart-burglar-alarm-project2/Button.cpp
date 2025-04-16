#include <Arduino.h>
#include "Button.h"
#include "Interface.h"

Button :: Button(int PinNum):Sensor(PinNum)
{
  pinNum = PinNum;
  
}

void Button :: waitButtonInput()
{ 
  Interface MatlabButton;
  int buttonWait=0; 
  int buttonState;
  while(buttonWait==0)
  {
    buttonState = getSensorReading();
    if(buttonState==1)
    {
      MatlabButton.sendChar('B');
      buttonWait=1;
    }
  }
}