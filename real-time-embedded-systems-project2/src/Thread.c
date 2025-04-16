#include "cmsis_os.h"                            // Includes the CMSIS RTOS header file for real time functionality
#include "stm32f4xx.h"												   // Includes the stm32f4xx.h header file for compatability and specific definitons for STM32F4 controllers
#include "my_defines.h"													// Includes my_defines for prewritten functions

// Thread Declarations

void Tilt_Thread (void const *argument); // Declares the function Tilt_Thread that is defined after the declarations
osThreadId tid_Tilt_Thread; // Declares an ID associated with the thread, allows easy reference when using os functions. 
osThreadDef (Tilt_Thread, osPriorityNormal, 1, 0); // Declares the main thread object. Has normal priority, 1 thread instance and 0 stack size.

void Flash_Thread (void const *argument); // Declares the function Flash_Thread that is defined after the declarations
osThreadId tid_Flash_Thread; // Declares an ID associated with the thread, allows easy reference when using os functions. 
osThreadDef (Flash_Thread, osPriorityNormal, 1, 0); // Declares the main thread object. Has normal priority, 1 thread instance and 0 stack size.

void Button_Thread (void const *argument); // Declares the function Button_Thread that is defined after the declarations
osThreadId tid_Button_Thread; // Declares an ID associated with the thread, allows easy reference when using os functions.
osThreadDef (Button_Thread, osPriorityNormal, 1, 0); // Declares the main thread object. Has normal priority, 1 thread instance and 0 stack size.

/*-------------------------------------------------------------------------
 *      Tilt Thread
 *-----------------------------------------------------------------------*/

// Code to define the initialise Tilt thread to be used in main, when initialised in main, the thread is started 
int Init_Tilt_Thread (void){
	
	tid_Tilt_Thread = osThreadCreate (osThread(Tilt_Thread), NULL); // Creates the main thread object and assigns it the thread ID declared for it 
  if(!tid_Tilt_Thread) return(-1); // Checks to ensure the thread has been created.
  
  return(0);
}

void Tilt_Thread (void const *argument){
// Code to define the operation of the thread, i.e. turning on the corresponding lights when the board is tilted

				uint8_t LED_on = 1; // Definition of parameter for LED on
				uint8_t LED_off = 0; // Definition of parameter for LED off
				uint8_t green_LED = 12; // Definition of parameter for green LED (GPIOD pin 12)
				uint8_t red_LED = 14; // Definition of parameter for red LED (GPIOD pin 14)
				uint8_t orange_LED = 13; // Definition of parameter for orange LED (GPIOD pin 13)
				uint8_t blue_LED = 15; // Definition of parameter for blue LED (GPIOD pin 15)
				uint8_t X_Register = 0x29; // Definition of the MSB X-Axis (H) data register address
				uint8_t Y_Register = 0x2B; // Defintion of the MSB Y-axis (H) data register address
	
				uint8_t X_Angle, Y_Angle; // Assigns data type unsigned 8 bit integer to X Angle and Y Angle
	
	
	while(1){
	
		
    X_Angle = Return_Angle(X_Register); // Returns current angle in X axis and sets it to variable X_Angle
		Y_Angle = Return_Angle(Y_Register); // Returns current angle in Y axis and sets it to Variable Y_Angle 

			if (((X_Angle & 0x80) == 0x80) && ((X_Angle & ~0x80) < 0x76)){  //  Checks to see if X axis value is Negative and the magnitude of the value is less than 14 degrees from the level poisiton 
						Blink_LED(LED_on,green_LED); // Turn on Green LED
			}  else{
						Blink_LED(LED_off,green_LED); // Turn off Green LED
			}
			
			if (((X_Angle & 0x80) == 0x00) && ((X_Angle & ~0x80) > 0x10)){ // Checks to see if the X axis value is Positive and the magnitude of the value is greater than 14 degrees from the level poisiton
						Blink_LED(LED_on,red_LED); // Turn on Red LED
			}  else{
						Blink_LED(LED_off,red_LED); // Turn off Red LED
			}

				
			if (((Y_Angle & 0x80) == 0x80) && ((Y_Angle & ~0x80) < 0x76)){ //  Checks to see if Y axis value is Negative and the magnitude of the value is less than 14 degrees from the level poisiton
						Blink_LED(LED_on,blue_LED); // Turn on Blue LED
			}  else{
						Blink_LED(LED_off,blue_LED); // Turn off Blue LED
			}
			
			if (((Y_Angle & 0x80) == 0x00) && ((Y_Angle & ~0x80) > 0x10)){ // Checks to see if the Y axis value is Positive and the magnitude of the value is greater than 14 degrees from the level poisiton
						Blink_LED(LED_on,orange_LED); // Turn on Orange LED
			}  else{
						Blink_LED(LED_off,orange_LED);// Turn on Blue LED
			}

		osThreadYield(); //Tells RTOS to either suspend this thead and run next thread if there is another one is ready. If not this Thread continues.
    }
 }

/*-------------------------------------------------------------------------
 *      Flash Thread
 *-----------------------------------------------------------------------*/
 
// Code to define the initialise Flash thread to be used in main, when initialised in main, the thread is started
int Init_Flash_Thread (void){
	
	tid_Flash_Thread = osThreadCreate (osThread(Flash_Thread), NULL); // Creates the main thread object and assigns it the thread ID declared for it
  if(!tid_Flash_Thread) return(-1); // Checks to ensure the thread has been created.
  
  return(0);
}

void Flash_Thread (void const *argument){
// Code to define the operation of the thread, i.e. flashing the lights in the given pattern
	
				uint8_t LED_on = 1; // Definition of parameter for LED on
				uint8_t LED_off = 0; // Definition of parameter for LED off
				uint8_t green_LED = 12; // Definition of parameter for green LED (GPIOD pin 12)
				uint8_t red_LED = 14; // Definition of parameter for red LED (GPIOD pin 14)
				uint8_t orange_LED = 13; // Definition of parameter for orange LED (GPIOD pin 13)
				uint8_t blue_LED = 15; // Definition of parameter for blue LED (GPIOD pin 15)
				
			Blink_LED(LED_on,orange_LED); // These 4 lines turn on the orange, blue, green and red LED
			Blink_LED(LED_on,blue_LED);
			Blink_LED(LED_on,green_LED);
			Blink_LED(LED_on,red_LED);
			osDelay(1000);							// Waits one second
			Blink_LED(LED_off,orange_LED); // These 4 lines turn off the orange, blue, green and red LED
			Blink_LED(LED_off,blue_LED);
			Blink_LED(LED_off,green_LED);
			Blink_LED(LED_off,red_LED);
			osDelay(1000);							//Waits one second
	while(1){ 											//This Repeatedly Blinks the Red LED infinitley until a new thread is called
		
		Blink_LED(LED_on,red_LED);
		osDelay(500);
		Blink_LED(LED_off,red_LED);
		osDelay(500);
		osThreadYield();   //Tells RTOS to either suspend this thead and run next thread if there is another one is ready. If not this Thread continues.
	}
	
}

/*-------------------------------------------------------------------------
 *      Button Thread
 *-----------------------------------------------------------------------*/

// Code to define the initialise Button thread to be used in main, when initialised in main, the thread is started
int Init_Button_Thread (void){
	
	tid_Button_Thread = osThreadCreate (osThread(Button_Thread), NULL); // Creates the main thread object and assigns it the thread ID declared for it
  if(!tid_Button_Thread) return(-1); // Checks to ensure the thread has been created.
  
  return(0);
}

void Button_Thread(void const *argument){
	// Code to define the operation of the thread, i.e. switching between tilt switch and 'standby mode'.
				
				uint8_t ii; //Assigns data type unsigned 8 bit integer to ii (while loop variable)
	
	while(1){
		
		if(((GPIOA->IDR)&0x00000001) == 0x00000001){ // if button is pressed
			osDelay(100); // wait 0.1 seconds to act as a switch debounce
			while(((GPIOA->IDR)&0x00000001) == 0x00000001){} // while button is pressed, wait until it is released
			osThreadTerminate(tid_Tilt_Thread); // terminates tilt thread
			Init_Flash_Thread(); // initialises flash thread
			osDelay(2000); // wait for 2 seconds
			ii = 0; 	// reset while loop variable
			while(ii == 0){ // continue flashing until if statement is met
				if(((GPIOA->IDR)&0x00000001) == 0x00000001){ // if button is pressed
					osDelay(100);	//wait 0.1 seconds to act as a switch debounce
					while(((GPIOA->IDR)&0x00000001) == 0x00000001){} // while button is pressed, wait until it is released
					ii = 1; // breakout of while loop
					}
				}
			osThreadTerminate(tid_Flash_Thread); // terminates flash thread
			Init_Tilt_Thread(); // initialises tilt thread
			}
			osThreadYield(); //Tells RTOS to either suspend this thead and run next thread if there is another one is ready. This will always occur as we initialise TIlt thread
		}
	
}
