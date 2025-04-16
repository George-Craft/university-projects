#ifndef LED_H
#define LED_H
class LED
{
protected:
    int ledPin;
    String ledName;
    unsigned long stateTime;//how long it should stay in current state
public:
    LED(int, String); 
    void On();
    void Off();
    void displayLedStatus(String);
    
};
#endif
