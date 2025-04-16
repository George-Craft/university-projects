#define osObjectsPublic 	// Define objects in main module
#include "osObjects.h" 		// Includes RTOS object definitions
#include "stm32f4xx.h"    // Includes the stm32f4xx.h header file for compatability and specific definitons for STM32F4 controllers
#include "my_defines.h"   // Includes my_defines for prewritten functions
#include "Thread.h"				// Includes Thread for prewritten threads


int main(void){
SystemCoreClockUpdate(); // Updates system core clock update
osKernelInitialize();	   // Initialise CMSIS-RTOS

	
Initialise_SPI(); // Initialises SPI using Function from my_defines.c
Initialise_GPIOA(); // Initialises GPIOA using Function from my_defines.c
Initialise_GPIOE(); // Initialises GPIOE using Function from my_defines.c
Initialise_GPIOE_I(); // Initialises GPIOE_I using Function from my_defines.c
Enable_SPI();	// Enables SPI using Function from my_defines.c
Initialise_LED_and_Button(); // Initialises LED and Button using Function from my_defines.c
Initialise_ControlReg(); // Initiallses Control Registers using Function from my_defines.c
	
Init_Tilt_Thread(); //Initialises Tilt thread from Thread.c
Init_Button_Thread(); //Initialises Button thread from Thread.c

osKernelStart(); // start thread execution

while(1){}; 		// while loop to prevent program terminating
	
}


