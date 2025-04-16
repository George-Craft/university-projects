#ifndef PIN_H
#define PIN_H

//HOW DO WE MAKE METHODS PRIVATE - cant do if i have to instantiate in my methods like pinSetup
class PIN 
{
  protected: 
    int userPin[7];
    int pinLength;
    int pinAttempts;
    int pinSetupAttempts;

  public:
    PIN(int defaultpin[],int length); //constructor
    PIN();
    int checkPinLength(int enteredPinLength);
    int checkPinDigits(int enteredPin[]);
    int validateLength(int enteredPinlength);
    int validateDigits(int enteredPin[],int enteredPinlength);
    void printpin();
    int getpinLength();
    void setUserPinandLength(int pinArray[],int length);
    void pinSetupOrChange();
    int login();
    int checkPinDisarm();


};
#endif
