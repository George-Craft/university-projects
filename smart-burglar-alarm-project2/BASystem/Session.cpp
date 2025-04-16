#include <Arduino.h>
#include <string.h>
#include "Session.h"
#include "Interface.h"
#include "LED.h"
#include "onLED.h"
#include "button.h"
#include "PIN.h"
#include "Sensor.h"
#include "securityInfo.h"
#include "actuator.h"
#include "buzzer.h"
#include "transitionTime.h"

const int redPin = 11;
const int orangePin = 12;
const int greenArmPin = 13;
const int onGreenPin = 10;

//class Instantiation 
Interface Matlabs;
transitionTime TTime;
securityInfo secure(7473565395);
actuator doorLock(50);
Sensor Door(4); 
Sensor PIR(7); 
Button button(2); 
onLED powerLED(onGreenPin, "Green Arm LED");
LED systemLeds[3] = {LED(redPin, "Red LED"), LED(orangePin, "Orange LED"), LED(greenArmPin, "Green LED")};
int num[4] = {1,2,3,4};
PIN pinWorldMain(num, 4);

//in IDLE state waiting for the ON button to pressed, once pressed leaves idle state
void Session :: checkIdleState(int conButton)
{
  int buttonState; 
  button.setInput();
  if(conButton==0)
  {
    while(conButton ==0)
    {
      //set leds and solenoid to the state needed for idle state
      systemLeds[0].Off();
      systemLeds[1].Off(); 
      powerLED.Off();
      systemLeds[2].Off();
      doorLock.Low();
   
      button.waitButtonInput(); //stay here till button is pressed
      powerLED.On(); 
      Matlabs.flush(); 
      conButton=1;
    }
  } 
}

//this is the PinSetup and Login phase
int Session :: enterSession(int * idleConstantpointer,int * conButtonpointer) 
{
  int a=0; int stay=1;
  int pinExists,pinattempts,loginSuccess;
  while(stay==1)
  {
    if(a==0)
    {
      pinExists=0; pinattempts=0; loginSuccess=0;
      a=1;
    }else
    {
      //get pinExists value
      pinExists=Matlabs.readMATLAB(); //get pinExists value from matlab

      if(pinExists==0) //go straight to PIN SETUP
      {
        //SETUP PIN
        pinWorldMain.pinSetupOrChange(); 
        Serial.println("end");
        *conButtonpointer=0; 
        stay=0;    
      }else if(pinExists==1) //go straight to LOGIN
      {
        //Serial.println("login");
        loginSuccess = pinWorldMain.login();
        if(loginSuccess==1)
        {
          //Serial.println(" ");
          Serial.println("SERVICE OPTIONS"); //login is successful so go to service options
          *idleConstantpointer=0;
          stay=0;  
        }else{
          //Serial.println("LOGIN unsuccesful. back to idle");
          *conButtonpointer=0;
          *idleConstantpointer=1;
          stay=0;  
        }
      }
    }
  }
}

//understand from matlab what service options has been chosen
int Session :: manageServiceOptions(int * idleConstantpointer,int * conButtonpointer)
{
  int startOption=0;
  
  while(startOption==0)
  { 
    if(Serial.available()>0)
    { 
      int arrivingdatabyte = Serial.read();  
      if(arrivingdatabyte ==1){
        //Serial.println("1 chosen");
        startOption = 1;  //to get out of loop after if statement complete
        return 1;     
      }else if(arrivingdatabyte ==2){
        //Serial.println("2 chosen2");
        startOption = 1;
        return 2;
      }else if(arrivingdatabyte ==3){ //TRANSITION TIME FUNCTION
        //Serial.println("3 chosen");
        startOption = 1;
        return 3;
      }else if(arrivingdatabyte ==4){ //Securty contact details function
        //Serial.println("4 chosen4");
        startOption = 1;
        return 4;
      }else if(arrivingdatabyte ==5){
        //Serial.println("5 chosen5");
        startOption = 1;
        return 5;
      }else if(arrivingdatabyte ==6){
        //Serial.println("6 chosen6");
        startOption = 1;
        return 6;
      }else if(arrivingdatabyte ==7){
        Serial.println("go back to idle");
        startOption=1;  
        *conButtonpointer=0; 
        *idleConstantpointer =1;
        return 7;       
      }else if (arrivingdatabyte ==8){
        Serial.println("Invalid input. go back to idle");
        startOption=1;
        *conButtonpointer=0;
        *idleConstantpointer =1;
        return 7;
      }
    }
  }
}

//act accordingly to what service options was chosen and procede to that option
int Session :: manageSession(int serviceNumber)
{
  //if service option 1 is chosen - changePinchosen
  if(serviceNumber ==1)
  {
    pinWorldMain.pinSetupOrChange();
  }
  //if service option 2 is chosen - ?
  if(serviceNumber ==2)
  {
    

  }
  //if service option 3 is chosen - transitiontimechosen
  if (serviceNumber == 3) 
  {
    //send transition time to matlab to proced with service option 6
    int transitionTime,tTLength;
    tTLength = TTime.getTransitionTimeLength();
    if(tTLength==2)
    {
      Matlabs.sendChar('2');
    }
    else if(tTLength==3)
    {
      Matlabs.sendChar('3');
    }
    Matlabs.flush();
    transitionTime=TTime.getTransitionTime();
    Matlabs.sendInt(transitionTime);
    Serial.println("done");
    //end of sending transition time - start service option 6
    
    int TTstart = 0;
    int idleTTloop = 0;
    while (idleTTloop == 0) {
      while (TTstart == 0) {
        int TTchoice = Matlabs.readMATLAB();
        if (TTchoice == 1) {
          TTime.changeTransitionTime();
          TTstart = 1;     //Get out of loop after function completes
          idleTTloop = 1;  //Return to idle as function has been completed
        } else if (TTchoice == 2)
        {
          idleTTloop = 1; //back to idle
          TTstart = 1; //Out of transition time loop
        }else
        {
          //Incorrect input
          Matlabs.loop_until_one(0);
          idleTTloop = 1; //back to idle
          TTstart = 1; //Out of transition time loop
        }
      }
    }
    return idleTTloop;
  }
  //if service option 4 is chosen - Securitycontactchosen
  if(serviceNumber == 4)
  {
    securityInfo SCsecure(secure.getContactNumber());
    int idleSCloop = 0;
    int SCstart = 0;
    while (idleSCloop == 0){
      while(SCstart == 0){      
        int SCchoice = Matlabs.readMATLAB();
        if (SCchoice == 1){
          SCsecure.changeContactDetails();
          SCstart = 1;
          idleSCloop = 1;
        }else if (SCchoice == 2){
          SCstart = 1;
          idleSCloop = 1;
        }else{
          //incorrect input
          Matlabs.loop_until_one(0);
        }
      }
    }
    return idleSCloop;
  }
  // if option 5 has been chosen - checkStatusOfSensorchosen
  if(serviceNumber == 5)
  {
    Matlabs.flush(); //flush the port to ensure accurate readings
    Door.setInputPullUp();
    PIR.setInput();
    //check status of window
    if (Door.getSensorReading() == HIGH)
    {
      Matlabs.sendChar('O'); //window open
    }else if(Door.getSensorReading() == LOW)
    {
      Matlabs.sendChar('C'); //window closed
    }
    //check status of PIR sensor
    if (PIR.getSensorReading() == HIGH)
    {
      Matlabs.sendChar('O'); //motion detected
    }else if(PIR.getSensorReading() == LOW)
    {
      Matlabs.sendChar('C'); //no motion detected
    }

    button.waitButtonInput();
    Matlabs.flush();
    return 1;
  }
  //if service option 6 is chosen - armDisarmchosen
  if(serviceNumber ==6)
  {
    //send transition time to matlab to proced with service option 6
    int transitionTime,tTLength;
    tTLength = TTime.getTransitionTimeLength();
    if(tTLength==2)
    {
      Matlabs.sendChar('2');
    }
    else if(tTLength==3)
    {
      Matlabs.sendChar('3');
    }
    Matlabs.flush();
    transitionTime=TTime.getTransitionTime();
    Matlabs.sendInt(transitionTime);
    Serial.println("done");
    //end of sending transition time - start service option 6
    Door.setInputPullUp();
    PIR.setInput();
    buzzer Alarm(9);
    int loop=0;
    int doorCheck;
    int startLeave = 0;
    int sensorCheck = 0;
    int windowCheck;
    int idleConstant=0;
    Matlabs.flush();
    while (idleConstant == 0)
    {
      while (sensorCheck != 1)
      {
        if(Door.getSensorReading() == LOW) //Check if the door is closed
        {
          Matlabs.sendMatlabCorD(1);// window closed
          windowCheck = 1;
        }else if (Door.getSensorReading() == HIGH)
        {
          Matlabs.sendMatlabCorD(2);//window open
          windowCheck = 2;
        }
        sensorCheck = Matlabs.if_available();
      }
      //all sensors ready prepare - countdown and then check if door is closed
      Matlabs.flush();
      int startSen = 0; int sensorCheck2 = 0;
      while (startSen == 0)
      {
        if (windowCheck == 1)
        {
          systemLeds[1].On();
          Matlabs.loop_until_one(0); //wait for matlab to tell us countdown is done
          systemLeds[1].Off(); //countdown finished
          //check if door is closed behind the user
          while (sensorCheck2 == 0)
          {
            if(Door.getSensorReading() == HIGH)
            {
              Matlabs.sendMatlabCorD(2);//Door open behind him
              Serial.println("out");
              doorLock.Low();
              doorCheck=1;
              sensorCheck2=1;
            }else if (Door.getSensorReading() == LOW)
            {
              Serial.println("before");
              Matlabs.sendMatlabCorD(1);//front Door closed behind him
              doorLock.High();//Lock solenoid
              doorCheck=2;
              Serial.println("after");
              sensorCheck2=1;
            }
            //sensorCheck2 = Matlabs.if_available();
          }
          Serial.println("flushg");
          Matlabs.flush();

          int sensorCheck3 = 0;
          while (sensorCheck3 == 0)
          {
            if (doorCheck == 1)//door open
            {
              systemLeds[0].On();//Red LED on
              Alarm.beep(5);
              sensorCheck3 = 1;
              startSen = 1; //leave loop and go to idle
              idleConstant = 1;
            }else if (doorCheck == 2)
            {
              systemLeds[2].On(); //put ArmLed on
              sensorCheck3 = 1;
            }
          }

          Matlabs.loop_until_one(0); //Waiting for 1 (line 92)
          //enter Armed state
          int once=0; int PIRSensorTrigger=0; int WindowSensorTrigger=0;
          if (doorCheck == 2)
          {
            Serial.println("armed state");
            Serial.println("enters line 265");
            while (loop == 0)
            {
              while((PIR.getSensorReading() == LOW) && Door.getSensorReading() == LOW)
              {
                Serial.print('Z');
                if(once==0)
                {
                  Serial.println("checking for intruder...");
                  once=1;
                }
                //Armed state checking sensors
              }
              if((PIR.getSensorReading() == HIGH ))
              {
                //Serial.println("pir sensor high");
                delay(12000);
                if((PIR.getSensorReading() == HIGH ))
                {
                  //Serial.println("INTRUDERRRRRRR OR USER");
                  PIRSensorTrigger=1;
                  loop=1;

                }  
              }
              if((Door.getSensorReading()==HIGH))
              {
                //Serial.println("door open");
                Serial.println("INTRUDERRRRRRR OR USER");
                WindowSensorTrigger=1;
                loop=1;
              }
            }
          }
          //Sensor deteceted: start timer for entering PIN/FACE recognition
         if(doorCheck==2)
         {
            Matlabs.sendChar('A');
            if(WindowSensorTrigger==1)
            {
              Matlabs.sendChar('W');
            }else if(PIRSensorTrigger=1)
            {
              Matlabs.sendChar('P');
            }
            Matlabs.flush();
            Serial.println("Intruder detected");
            /*
            //face starts here
            
            
            //face ends here
            */
            doorLock.Low(); //unlock solenoid so user can enter house to enter PIN

            Matlabs.loop_until_one(0); //sync with matlab for after face recorgnition
            //timer_start for PIN entry
            //variables needed for this sesction
            int constant_biggerloop=0;
            int buzzerLoop=0;
            int intruderloop=0;
            int AlarmTrigger=0;
            int buttonState=0;
            int pinDisarmSuccess=0;
            long TimerStart, TimerOff, CurrentTime = 0, AlarmStart, AlarmEnd;
            int PINCorrect=0 , PINAttempts = 0, TypeTime = 0, DelayTime;  
            DelayTime = TTime.getTransitionTime(); //get transiiton time value for the amount of time user has to enter the pin  
            long DTime = (long) DelayTime;
            TimerStart = millis();
            Serial.println(TimerStart);
            TimerOff = TimerStart + (DTime*1000);
            Serial.println(TimerOff);
            while(intruderloop==0)
            {
              systemLeds[2].Off(); //turn green LED
              systemLeds[1].On(); //turn RED led on
              CurrentTime = millis();
              while ((CurrentTime < TimerOff) && (PINCorrect == 0) && (PINAttempts < 5))//while time not up and incorrect pin attempts less than 5
              {
                //Serial.println("nothing pressed");
                buttonState = button.getSensorReading();
                if(buttonState==1)
                {
                  Matlabs.sendChar('B');
                  Matlabs.flush();
                  //receive length of pin and check if same as user_Pin.
                  pinDisarmSuccess = pinWorldMain.checkPinDisarm(); //sends a 1 if pin correct so disarm system
                  Matlabs.flush();

                  if(pinDisarmSuccess==1)
                  {
                    //Serial.println("Disarming System - go to idle");
                    PINCorrect = 1;    // if pin is correct , return a 1
                  }
                  else
                  {
                    PINAttempts = PINAttempts + 1; //record pinattempts
                    PINCorrect = 0;
                  }
                }

                CurrentTime = millis();
              }

              //tell matlab whether pin is correct or weather to turn on alarm
              
              if ((CurrentTime > TimerOff ) && (PINAttempts < 5) && (PINCorrect == 0))
              {
                Serial.println("TIME UP BOZO * WHOOP WHOOP WHOOP *");
                AlarmTrigger = 1;
                intruderloop=1;
                Matlabs.sendMatlabEorF(3);
              }
              else if ((PINAttempts > 4) && (CurrentTime < TimerOff ) && ( PINCorrect == 0))
              {
                Serial.println("YOU ENTERED WRONG PIN TOO MANY TIMES * WHOOP WHOOP WHOOP *");
                AlarmTrigger = 1;
                intruderloop=1;
                Matlabs.sendMatlabEorF(4); 
              }
              else if (PINCorrect == 1)
              {
                //tell matlab pin is correct
                Serial.println("Disabling alarm * disabled alarm*");
                intruderloop=1;
                idleConstant=1;
                startSen = 1;
              }
            }
            systemLeds[1].Off();
            //turn buzzer on as pin incorrect or time is up to enter pin
            //if goes past buzzer on limit turn of and disarm
            //if pin entered correct turn buzzer off and disarm
            if(AlarmTrigger==1) //if not system disarmed and idle
            {
              buzzerLoop=0; pinDisarmSuccess=2;
              DelayTime =  Alarm.getBuzzerLimit(); 
              Serial.println(DelayTime); 
              long DTime = (long) DelayTime; Serial.println(DTime); 
              TimerStart = millis();
              TimerOff = TimerStart + (DTime*1000);
              Serial.println(TimerOff);

              systemLeds[0].On(); //turn red led on
              Alarm.High();//turn buzzer on

              while(buzzerLoop==0)
              {
                CurrentTime = millis();
                while((CurrentTime < TimerOff) && (pinDisarmSuccess==2))
                {
                  
                  CurrentTime = millis();
                  buttonState = button.getSensorReading();
                  if(buttonState==1)
                  {
                    Matlabs.sendChar('B');
                    Matlabs.flush();
                    //receive length of pin and check if same as user_Pin.
                    pinDisarmSuccess = pinWorldMain.checkPinDisarm(); //sends a 1 if pin correct so disarm system
                    Serial.println("left pindisarm function");
                    Matlabs.flush();

                    if(pinDisarmSuccess==1)
                    {
                      Serial.println("pinSuccess - Disarming System - go to idle");
                    }
                  }

                }
                Serial.println("times up/pin correct");
                if(pinDisarmSuccess==2) //if pin unsuccessful and runs out of time then disarm
                {
                  Serial.println("sending G's");
                  Matlabs.sendMatlabG(6);                
                }
                Alarm.Low();
                buzzerLoop=1;
                idleConstant=1;
                startSen=1;
              }//end of buzzer loop
            }//end of alarm trigger=1
          }
        

        }else if (windowCheck == 2)
        {
          Serial.println("window check is 2 go back to idle");
          systemLeds[1].On();
          startSen = 1;
          idleConstant = 1; //Go back to idle state
        }
      } //startSen loop
      
    }//idleconstant loop
    return idleConstant;
  }//if service 6

  Serial.println("End of manage session");
   
}// end manage session

