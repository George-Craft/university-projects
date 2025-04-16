#ifndef onLED_H
#define onLED_H
#include "LED.h"

class onLED : public LED
 {
   private:
    unsigned long onTime;
    unsigned long offTime;
    
   public:
   //constructor
    onLED(int, String);
    // methods
    void on(unsigned long);
    void off(unsigned long);
    void flash(int, unsigned long, unsigned long);
 };

#endif