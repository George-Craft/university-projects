#include <Arduino.h>
#include <string.h>
//#include <stdlib.h>// for the strtoul function to be used in the main ino
#include "transitionTime.h"
#include "Interface.h"

transitionTime :: transitionTime()
{ 
  TransitionTime = 80;
  tLength=2;
}

void transitionTime :: setTransitionTime(int setTime[], int timeLength)
{
  int i;
  TransitionTime = 0;
  for (i = 0; i < timeLength; i++)
  {
    TransitionTime = 10 * TransitionTime + setTime[i];
  }
  tLength=timeLength;
}

int transitionTime :: validateLength(int timeLength)
{
  if((timeLength==2)|| (timeLength==3))
  {
    return 1; //correct length
  }else{
    return 2; //time wrong size 
    Serial.println("length return 2 wrong size");
  }
}

int transitionTime :: validateTransitionTime(int enteredTime[],int timeLength)
{
  if(timeLength == 2)
  {
    if(enteredTime[0] >= 6)
    {
      return 1; //time is accepted and verified
    }
    else
    {
      return 2;// disp("ERROR! time not within range wrong ")
    }
  }
  else if(timeLength == 3) 
  {
    if(enteredTime[0] == 1)
    {
     if((enteredTime[1] == 8) && (enteredTime[2] == 0))
     {
       return 1; //time is accepted and verified
     }
     else if(enteredTime[1] < 8)
     { 
       return 1; //time is accepted and verified
     } 
     else
      {
        return 2;// disp("ERROR! time not within range wrong ")
      }
    }
    else
    {
      return 2;// disp("ERROR! time not within range wrong ")
    }
  }
  else
  {
    return 2;// disp("ERROR! time length wrong ")
  } 
}


int transitionTime :: getTransitionTime()
{
  return TransitionTime;
}

int transitionTime :: getTransitionTimeLength()
{
  return tLength;
}

 void transitionTime :: changeTransitionTime()
{

  Interface MatlabTransitionTime;
  
  int attempts=0, enteredTimelength,validlength,validateinput;
  //CHANGE TRANSITION TIME
  //Serial.println("In change time"); delay(1000);
  while(attempts<2)
  {
    MatlabTransitionTime.flush();
    //wait for user to enter time
    MatlabTransitionTime.loop_until_one(0); 
    //wait for length of time to be sent
    Serial.println("waiting");
    while(Serial.available() ==0){}; //do nothing until recieved smt from matlab
    enteredTimelength = Serial.read();
    Serial.print("length Of TIME INPUT is..."); Serial.println(enteredTimelength); delay(1000);
    //validate length
    validlength = validateLength(enteredTimelength);
    MatlabTransitionTime.sendMatlabXorY(validlength); //tells MATLAB the validLength value = 1 :) or a 2:(

    //first flush the serial port
    MatlabTransitionTime.flush();

    //if correct length then accept the time from matlab
    int enteredTime[enteredTimelength];
    if(validlength==1)
    {
      for(int i=0;i<enteredTimelength;i++)
      {
        enteredTime[i] = MatlabTransitionTime.readMATLAB();
      }
      //verify 60> time <180 digits
      validateinput = validateTransitionTime(enteredTime, enteredTimelength);
      MatlabTransitionTime.sendMatlabXorY(validateinput);
    
    }

    //print entered time from matlab
    Serial.print("entered transition time is: ");
    for(int i=0;i<enteredTimelength;i++)
    {
      Serial.print(enteredTime[i]);
    }
    Serial.println(" ");

    //first flush the serial port
    MatlabTransitionTime.flush();

    //enabling number of attempts to be up to 2
    if((validlength==1) && (validateinput==1))
    {
      Serial.println("time confirmed!"); //pinExists=1;//look out for this in matlab
      //set transition timeif validation is successful
      //print entered time from matlab
      setTransitionTime(enteredTime, enteredTimelength);
      attempts=7;//to get out of while loop
    }else{
      //Serial.println("try again/go Home!");
      attempts=attempts+1;
    }
  }
}
