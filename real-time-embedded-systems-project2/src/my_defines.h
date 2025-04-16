#include "stm32f4xx.h"

void Initialise_SPI(void);  // Declaration for the function to initialise SPI
void Initialise_GPIOA(void); // Declaration for the function to initialise GPIOA
void Initialise_GPIOE(void); // Declaration for the function to initialise GPIOE
void Initialise_GPIOE_I(void); // Declaration for the function to initialise GPIOE_I
void Enable_SPI(void); // Declaration for the function to Enable SPI
void Initialise_LED_and_Button(void); // Declaration for the function to initialise the LED and Button
void Initialise_ControlReg(void); // Declaration for the function to initialise the Control Register
void Blink_LED(uint8_t,uint8_t); // Declaration for the function to blink the LED - the two inputs are: one for the state and one for the colour LED
uint8_t Return_Angle(uint8_t address); // Declaration for the function to Read the angle - the input is the address of either Y axis or X axis data register
