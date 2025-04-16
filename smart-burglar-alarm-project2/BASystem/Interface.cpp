
#include <Arduino.h>
#include <string.h>
#include "Interface.h"

int Interface::if_available()
{
  int x = 0;
  if(Serial.available()>0){  //keep checking if a 1 is sent or not. when it has then leave loop
      x = Serial.read();
  }
  return x;
}

void Interface::loop_until_one(int start)
{
  Serial.println(" ");
  Serial.println("IN loop_until_one");
  while(start != 1)  //keep looping till a 1 is sent from MATLAB
  { 
    if(Serial.available()>0){  //keep checking if a 1 is sent or not. when it has then leave loop
        start = Serial.read();
    }
  }
  Serial.println("OUT loop_until_one");
}
void Interface::sendMatlabAorB(int msg) //11 or 12
{
    int wait=0;
    while(wait==0) 
    {
        if(msg==1) //correct validation so tell matlab 
        {
            Serial.print("A");
        }else if(msg==2) //incorrect validation so tell matlab the error
        {
            Serial.print("B");    
        }  
        if(Serial.available()>0)
        {
            loop_until_one(0); //leave loop when matlab has received the 1,2 or 3
            wait=1;
        } 
    }
}
void Interface::sendMatlabCorD(int msg) //61 or 62
{
    int wait=0;
    while(wait==0) 
    {
        if(msg==1) //correct validation so tell matlab 
        {
            Serial.print("C");
        }else if(msg==2) //incorrect validation so tell matlab the error
        {
            Serial.print("D");    
        }  
        if(Serial.available()>0)
        {
            loop_until_one(0); //leave loop when matlab has received the 1,2 or 3
            wait=1;
        } 
    }
}

void Interface::sendMatlabEorF(int msg) //63 or 64
{
    int wait=0;
    while(wait==0)
    {
        if(msg==3) //correct validation so tell matlab 
        {
            Serial.print("E");
        }else if(msg==4) //incorrect validation so tell matlab the error
        {
            Serial.print("F");    
        }  
        if(Serial.available()>0)
        {
            loop_until_one(0); //leave loop when matlab has received the 1,2 or 3
            wait=1;
        } 
    }
}


void Interface::sendMatlabG(int msg) //66
{
    int wait=0;
    while(wait==0) 
    {
        if(msg==6) //correct validation so tell matlab 
        {
            Serial.print("G");
        }
        if(Serial.available()>0)
        {
            loop_until_one(0); //leave loop when matlab has received the 1,2 or 3
            wait=1;
        } 
    }
}

void Interface :: sendMatlabXorY(int msg)
{
  int wait=0;
  while(wait==0)
  {
      if(msg==1) //correct validation so tell matlab 
      {
          Serial.print("X");
      }else if(msg==2) //incorrect validation so tell matlab the error
      {
          Serial.print("Y");    
      }  
      if(Serial.available()>0)
      {
          loop_until_one(0); //leave loop when matlab has received the 1,2 or 3
          wait=1;
      } 
  }

}
void Interface :: sendChar(char c)
{
  int start=0;
  while(start!=1)
  {
      Serial.print(c);
      if(Serial.available()>0)
      {
          int arrivingdatabyte = Serial.read();
          start=1;
      }
  }
  Serial.println(" ");
}
void Interface :: sendInt(int i)
{
  int start=0; int var=0;
  while(start!=1)
  {
    if(var==0)
    {
      Serial.print(i);
      var=1;
    }
    if(Serial.available()>0)
    {
        int arrivingdatabyte = Serial.read();
        start=1;
    }
  }
  Serial.println(" ");
}
void Interface::flush()
{
    int start=0;
    while(start!=1)
    {
        Serial.print('R');
        if(Serial.available()>0)
        {
            int arrivingdatabyte = Serial.read();
            start=1;
        }  
    }
    Serial.println(" ");
}
int Interface::readMATLAB()
{
    int readint, start=0;
    while(start == 0)
    {  
      Serial.println("read");
      if(Serial.available()>0){  //keep checking if a 1 is sent or not. when it has then leave loop
          readint = Serial.read();
          start=1;
          //Serial.println("in");
      }
    }
    return readint;
}
