#include <Arduino.h>
#include <string.h>
#include "securityInfo.h"
#include "Interface.h"

securityInfo :: securityInfo(unsigned long ContactNum)
{
  contactNum = ContactNum;
}

void securityInfo :: setContactDetails(int setNum[], int numLength)
{
  int i;
  int num = 0;
  for (i = 0; i < numLength; i++)
  {
    num = 10 * num + setNum[i];
  }
  contactNum = (unsigned long)num;
}

long securityInfo :: getContactNumber()
{
  return contactNum;
}
int securityInfo :: validateLength(int timeLength)
{
  if(timeLength == 10)
  {
    return 1; //correct length
  }else
  {
    return 2; //time wrong size 
    Serial.println("length return 2 wrong size");
  }
}
int securityInfo :: validateContactNumber(int enteredNum[])
{
  if (enteredNum[0]!=0)
  {
    return 1; //contact accepted
  }
  else
  {
    return 2; //contact not accepted
  }  
}
void securityInfo :: changeContactDetails()
{
  Interface MatlabContactInfo;
  int attempts=0, enteredNumlength,validlength,validateinput=0;
  //CHANGE CONTACT NUM
  while(attempts<2)
  {
    //wait for user to enter contact num
    MatlabContactInfo.loop_until_one(0); 
    //wait for length of contact num to be sent
    Serial.println("waiting");
    while(Serial.available() ==0){}; //do nothing until recieved smt from matlab
    enteredNumlength = Serial.read();
    Serial.print("length Of num INPUT is..."); Serial.println(enteredNumlength); delay(1000);
    //validate length
    validlength = validateLength(enteredNumlength);
    MatlabContactInfo.sendMatlabXorY(validlength); //tells MATLAB the validLength value = 1 :) or a 2:(

    //first flush the serial port
    MatlabContactInfo.flush();

    //if correct length then accept the time from matlab
    int enteredNum[enteredNumlength];
    if(validlength==1)
    {
      for(int i=0;i<enteredNumlength;i++)
      {
        enteredNum[i] = MatlabContactInfo.readMATLAB();
      }
      //verify thst contact num doesnt begin with 0 to prevent error: invalid octal digit
      validateinput = validateContactNumber(enteredNum);
      MatlabContactInfo.sendMatlabXorY(validateinput);
    
    }

    //first flush the serial port
    MatlabContactInfo.flush();

    if (validateinput == 1)
    {
        //print entered number from matlab
      Serial.print("entered contact num is: ");
      for(int i=0;i<enteredNumlength;i++)
      {
        Serial.print(enteredNum[i]);
      }
      Serial.println(" ");      
    }
    
    //first flush the serial port
    MatlabContactInfo.flush();

    //enabling number of attempts to be up to 2
    if((validlength==1) && (validateinput==1))
    {
      Serial.println("contact num confirmed!"); 
      //set contact info if validation is successful
      //print entered time from matlab
      setContactDetails(enteredNum, enteredNumlength);
      attempts=7;//to get out of while loop
    }else{
      //Serial.println("try again/go Home!");
      attempts=attempts+1;
    }
  }
}

