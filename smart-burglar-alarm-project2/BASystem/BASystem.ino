#include <Arduino.h>
#include <string.h> //look for in arduino library
#include "PIN.h" //look for in folder in computer
#include "Interface.h"
#include "Sensor.h"
#include "Button.h"
#include "LED.h" 
#include "onLED.h"
#include "Session.h"

//variables
const int foreverIsOne = 1;
const int LEDSIZE = 3;
const int redPin = 11;
const int orangePin = 12;
const int greenArmPin = 13;
const int onGreenPin = 10;
int maltabMessage;
int start;
int wait;
int msgToMatlab;
int msg;
int validateLength;
int validateDigits;
int enteredPinNum=0;
int enteredPinlength;
int one,two,three,four; 
int pinExists=0;
int pinattempts=0;
int loginSuccess=0;
int testledpin=13;

//variables for template
int idleConstant=1; //idle state==1 active state==0
int conButton=0;
int option,sessionComplete;


void setup() {
  Serial.begin(9600);
  pinMode(testledpin,OUTPUT);
  digitalWrite(testledpin,LOW);
}

void loop() 
{
  //class Instantiation
  Interface Matlab;
  Session mainSession;
  onLED powerLED(onGreenPin, "Power LED");
  //OFF: wait for user to press enter to start - put the "Plug socket" in
  Matlab.loop_until_one(0);
  //once user has pressed enter flash led's
  powerLED.flash(4,500,500);  
  //like void loop() - will never leave
  while(foreverIsOne==1) //never change this variable
  {
    mainSession.checkIdleState(conButton); //leaves this session when leaves idle state
    mainSession.enterSession(&idleConstant,&conButton); //this is the PinSetup and Login phase
   
    while(idleConstant==0)
    {
      //button pressed so now in active state - options are shown to screen, wait for option to be chosen in MATLAB
      option = mainSession.manageServiceOptions(&idleConstant,&conButton);
      //act according to option chosen - if *idleconstant changes to 1 goes back to idle 
      sessionComplete= mainSession.manageSession(option);
      idleConstant=sessionComplete; //when session is complete go back to idle

    }//end of idleConstant while loop  
     
  } //end of foreverIsOne while loop
  
} 
  
