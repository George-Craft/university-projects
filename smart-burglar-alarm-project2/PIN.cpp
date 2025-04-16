#include <Arduino.h>
#include <string.h>
#include "PIN.h"
#include "Interface.h"


PIN :: PIN(int defaultpin[],int length) //constructor
{
  pinLength=length;
  for(int i=0;i<length;i++)
  {
    userPin[i] = defaultpin[i];
  }
}
PIN :: PIN()
{}

int PIN :: checkPinLength(int enteredPinLength)
{
  if(pinLength == enteredPinLength)
  {
    return 1;
  }else{
    return 2;
  }
}

int PIN :: checkPinDigits(int enteredPin[]) //at the LOGIN - check entered pin is equal to fixed pin/ userPin
{ 
  Serial.println("check pin is equal to fixed pin...");
  //check if userpinlength == enteredpinlength - length is correct so now check eevry digit
  int match=0;
  for(int i=0;i<pinLength;i++) 
  {
    if(userPin[i] == enteredPin[i])
    {
      match=match+1;
    }
  }
  if(match==pinLength)
  {
    //Serial.println("entered pin is correct:)");
    return 1;
  }else{
    //Serial.println("wrong pin!");
    return 2; //right length but wrong digits
  }  
 
  
}


int PIN :: validateLength(int enteredPinlength)
{
  if((enteredPinlength<4)|| (enteredPinlength>7))
    {
      return 2; //pin wrong size 
      //Serial.println("length return 2 wrong size");//delay(1000);
    }else{
      return 1; //correct length
    }      
}

int PIN :: validateDigits(int enteredPin[],int enteredPinlength) //check for one reocurring digit
{
  int reocurringCheck=0;
  for(int i=1;i<enteredPinlength;i++)
  {
    if(enteredPin[0]==enteredPin[i])
    {
      reocurringCheck = reocurringCheck+1;
    }
  }
  if(reocurringCheck == enteredPinlength-1)
  {
    return 2;// disp("ERROR! PIN cannot contain one reoccuring digit")
  }else{
    return 1; //PIN is accepted and verified
  } 
}

void PIN :: printpin(){
  //Serial.print("userPin: ");
  for(int i=0;i<pinLength;i++)
  {
    Serial.print(userPin[i]);
  }
  Serial.println(" ");
  //Serial.print("userpinlength: ");Serial.println(pinLength);
}

int PIN ::getpinLength()
{
  return pinLength;
}

void PIN :: setUserPinandLength(int pinArray[],int length)
{
  for(int i=0;i<length;i++)
  {
    userPin[i]= pinArray[i];
  }
  pinLength = length;
  
  /*Serial.print("userPin is now: ");
  for(int i=0;i<length;i++)
  {
    Serial.print(userPin[i]);
  }
  Serial.println(" ");
  Serial.print("userpinlength is now: ");Serial.println(pinLength);*/
}

void PIN :: pinSetupOrChange()
{
  Interface MatlabPinSetup;
  int pinattempts=0, enteredPinlength,validatelength,validatedigits;
  //SETUP PIN 
  while(pinattempts<2)
  {
    //wait for user to enter pin
    MatlabPinSetup.loop_until_one(0); 
    //wait for length of pin to be sent
    Serial.println("waiting");
    while(Serial.available() ==0){}; //do nothing until recieved smt from matlab
    enteredPinlength = Serial.read();
    Serial.print("length if pin is..."); Serial.println(enteredPinlength); delay(1000);
    //validate length
    validatelength = validateLength(enteredPinlength);
    MatlabPinSetup.sendMatlabAorB(validatelength); //tells MATLAB the validateLength value = 1 :) or a 2:(

    //first flush the serial port
    MatlabPinSetup.flush();

    //if correct length then accept the pin from matlab
    int enteredPin[enteredPinlength];
    if(validatelength==1)
    {
      for(int i=0;i<enteredPinlength;i++)
      {
        enteredPin[i] = MatlabPinSetup.readMATLAB();
      }
      //verify reocurring digits
      validatedigits = validateDigits(enteredPin, enteredPinlength);
      MatlabPinSetup.sendMatlabAorB(validatedigits);
    
    }

    /*//print entered pin from matlab
    Serial.print("enteredPin is: ");
    for(int i=0;i<enteredPinlength;i++)
    {
      Serial.print(enteredPin[i]);
    }
    Serial.println(" ");*/

    MatlabPinSetup.flush(); //first flush the serial port

    //enabling number of attempts to be up to 2
    if((validatelength==1) && (validatedigits==1))
    {
      //Serial.println("pin confirmed!"); //pinExists=1;
      setUserPinandLength(enteredPin, enteredPinlength); //set enteredPin to be the UserPin and its pinLength if setup is successful
      pinattempts=9;//to get out of while loop
    }else{
      //Serial.println("try again/go Home!");
      pinattempts=pinattempts+1;
    }
  }
}

int PIN :: login()
{
  Interface MatlabLogin;
  int pinattempts=0, enteredPinlength, checkl=0, checkd=0, checkFinal;
  while(pinattempts<2)
  {
    //wait for user to enter pin
    MatlabLogin.loop_until_one(0); 
    //receive length of pin and check if same as user_Pin.
    while(Serial.available() ==0){}; //do nothing until recieved smt from matlab
    enteredPinlength = Serial.read();
    checkl = checkPinLength(enteredPinlength);
    MatlabLogin.sendMatlabAorB(checkl); //tell matlab whether the length is same or not. if not obvs incorrect
    MatlabLogin.flush();

    if(checkl ==1) //if length same as user pin,accept pin from matlab and check every digit
    {
      while(Serial.available() ==0){}; //do nothing until recieved smt from matlab
      //accept pin from matlab
      int enteredPin[enteredPinlength]; 
      for(int i=0;i<enteredPinlength;i++)
      {
        enteredPin[i] = MatlabLogin.readMATLAB();
      }
      //print entered pin from matlab
      Serial.print("enteredPin is: ");
      for(int i=0;i<enteredPinlength;i++)
      {
        Serial.print(enteredPin[i]);
      }
      Serial.println(" ");
      //PRINT user PIN AND LENGTH
      printpin();
      Serial.print("length of pin: ");Serial.println(enteredPinlength);
      checkFinal = checkPinDigits(enteredPin);  //check the loginEnteredpin with the userpin
      MatlabLogin.sendMatlabAorB(checkFinal); //tell matlab whther the login pin is correct or not
      MatlabLogin.flush();

    }else if(checkl==2)
    {
      //Serial.print("length of pin: ");Serial.println(enteredPinlength); //shoulde be not equal to userpin
      checkFinal=2;
    }  
    
    if(checkFinal==1)
    {
      pinattempts=9; //:) leave login
      //Serial.println("left LOGIN"); 
      return 1; //login successful
    }else if(checkFinal==2)
    {
      pinattempts=pinattempts+1; //Serial.println("try again/go Home!");
      if(pinattempts==2)
      {
        //Serial.println("left LOGIN"); 
        return 2; //login unsuccessful 
      } 
    }  
  } 
}

int PIN :: checkPinDisarm()
{
  Interface MatlabDisarm;
  int pinattempts=0, enteredPinlength, checkl=0, checkd=0, checkFinal;
 
  //wait for user to enter pin
  MatlabDisarm.loop_until_one(0); 
  //receive length of pin and check if same as user_Pin.
  while(Serial.available() ==0){}; //do nothing until recieved smt from matlab

  enteredPinlength = Serial.read();
  checkl = checkPinLength(enteredPinlength);
  MatlabDisarm.sendMatlabCorD(checkl); //tell matlab whether the length is same or not. if not obvs incorrect
  MatlabDisarm.flush();

  if(checkl ==1) //if length same as user pin,accept pin from matlab and check every digit
  {
    while(Serial.available() ==0){}; //do nothing until recieved smt from matlab
    //accept pin from matlab
    int enteredPin[enteredPinlength]; 
    for(int i=0;i<enteredPinlength;i++)
    {
      enteredPin[i] = MatlabDisarm.readMATLAB();
    }
    /*//print entered pin from matlab
    Serial.print("enteredPin is: ");
    for(int i=0;i<enteredPinlength;i++)
    {
      Serial.print(enteredPin[i]);
    }
    Serial.println(" ");
    //PRINT user PIN AND LENGTH
    printpin();
    Serial.print("length of pin: ");Serial.println(enteredPinlength);*/

    checkFinal = checkPinDigits(enteredPin);  //check the loginEnteredpin with the userpin
    MatlabDisarm.sendMatlabCorD(checkFinal); //tell matlab whether the login pin is correct or not via sending a C or D
    MatlabDisarm.flush();

  }else if(checkl==2)
  {
    //Serial.print("length of pin: ");Serial.println(enteredPinlength); //shoulde be not equal to userpin
    checkFinal=2;
  }  
  
  if(checkFinal==1)
  { 
    return 1; //pin successful
  }else if(checkFinal==2)
  {
    return 2; //failure pin
  }
  
}



