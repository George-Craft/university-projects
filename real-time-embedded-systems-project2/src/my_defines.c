#include "stm32f4xx.h" // Includes the stm32f4xx.h header file for compatability and specific definitons for STM32F4 controllers

SPI_HandleTypeDef SPI_Params; //SPI parameters structure handle
I2C_HandleTypeDef I2C_Params; //I2C parameters structure handle
I2S_HandleTypeDef I2S_Params; //I2S parameters structure handle

GPIO_InitTypeDef GPIOA_Params; // GPIOA parameters structure handle
GPIO_InitTypeDef GPIOB_Params; // GPIOB parameters structure handle
GPIO_InitTypeDef GPIOD_Params; // GPIOD parameters structure handle
GPIO_InitTypeDef GPIOE_Params; // GPIOE parameters structure handle
GPIO_InitTypeDef GPIOE_Params_I; // GPIOE_I parameters structure handle

uint8_t data_to_send_receive[1]; // Declares an array used to store required LIS3DSH register address or data in.
uint16_t data_size=1; //Declares a variable specifying that only a single address is accessed in each  transaction.
uint32_t data_timeout=1000; //Sets a maximum time of 1000 to wait for the SPI transaction to complete in - this ensures program won’t freeze if there is a problem with the SPI/I2C communication channel.



//Definition for the function to initialise the SPI connection
void Initialise_SPI(void){
	
	RCC->APB2ENR |= RCC_APB2ENR_SPI1EN; // Enable clock for SPI1

    SPI_Params.Instance = SPI1; // Selects SPI1 interface
    SPI_Params.Init.Mode = SPI_MODE_MASTER; // Sets STM32F407 as Master
    SPI_Params.Init.NSS = SPI_NSS_SOFT; // Sets slave to be controlled by the software
    SPI_Params.Init.Direction = SPI_DIRECTION_2LINES; // Sets SPI to full-duplex
    SPI_Params.Init.DataSize = SPI_DATASIZE_8BIT; // Sets the data packet size to 8-bit
    SPI_Params.Init.CLKPolarity = SPI_POLARITY_HIGH; // Sets idle polarity for clock line to high
    SPI_Params.Init.CLKPhase = SPI_PHASE_2EDGE; // Sets the data line to be sampled on the second transition of the clock line
    SPI_Params.Init.FirstBit = SPI_FIRSTBIT_MSB; //Sets the transmission to MSB first
    SPI_Params.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_32; // Sets clock prescaler to divede main APB2 clock by 32 to give an SPI clock of 2.625MHZ, to be less than the max value of 10MHz for SPI.

    HAL_SPI_Init(&SPI_Params); // Configures the SPI using the specified parameters
}

//Definition for the function to initialise pins 5-7 of GPIOA
void Initialise_GPIOA(void) {
	
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN; //Enable clock for GPIOA

    GPIOA_Params.Pin = GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7; // Selects pins 5,6,7
    GPIOA_Params.Alternate = GPIO_AF5_SPI1; // Selects alternate function 5 which corresponds to SPI1
    GPIOA_Params.Mode = GPIO_MODE_AF_PP; // Selects alternate function to push pull
    GPIOA_Params.Speed = GPIO_SPEED_FAST; // Selects fast speed
    GPIOA_Params.Pull = GPIO_NOPULL; //Selects no pull-up or pull down activation

    HAL_GPIO_Init(GPIOA, &GPIOA_Params); // Configures GPIOA into specidified modes in GPIOA_Params
}

//Definition for the function to initialise pins 3 of GPIOE
void Initialise_GPIOE(void){
	
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOEEN; // Enable clock for GPIOE
	
    GPIOE_Params.Pin = GPIO_PIN_3; // Selects pin 3
    GPIOE_Params.Mode = GPIO_MODE_OUTPUT_PP; // Selects push-pull mode
    GPIOE_Params.Speed = GPIO_SPEED_FAST; // Selects fast speed
    GPIOE_Params.Pull = GPIO_PULLUP; // Selects pull-up activation
    HAL_GPIO_Init(GPIOE, &GPIOE_Params); // Configures GPIOE into specified modes in GPIOE_Params
}

//Definition for the function to initialise pin 0 of GPIOE_I
void Initialise_GPIOE_I(void) {
    
    GPIOE_Params_I.Pin = GPIO_PIN_0; // Selects pin 0 
    GPIOE_Params_I.Mode = GPIO_MODE_IT_RISING; // Selects rising edge trigger detection mode
    GPIOE_Params_I.Speed = GPIO_SPEED_FAST; // Selects fast speed
    HAL_GPIO_Init(GPIOE, &GPIOE_Params_I); // Configures GPIOE_I into specified modes in GPIOE_Params_I
}

//Definition for the function to Enable SPI
void Enable_SPI(void) {
		GPIOE->BSRR = GPIO_PIN_3; // Sets the serial port enable pin to CS high
		__HAL_SPI_ENABLE(&SPI_Params); // Enables SPI
}

//Definition for the function to initialise LEDs and Button
void Initialise_LED_and_Button(void) {
	 RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN;

    GPIOD->MODER |= GPIO_MODER_MODER14_0; // Red LED port 14 to output
    GPIOD->MODER |= GPIO_MODER_MODER12_0; // Green LED port 12 to output
    GPIOD->MODER |= GPIO_MODER_MODER13_0; // Orange LED port 13 to output
    GPIOD->MODER |= GPIO_MODER_MODER15_0; // Blue LED port 15 to output
	
		RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN; // Enable Port A clock
}

//Definition for the Control Register
void Initialise_ControlReg(void){

data_to_send_receive[0] = 0x20; // Address for control register 3 on the LIS3DSH
GPIOE->BSRR = GPIO_PIN_3<<16; // Set SPI communication enable line low to initiate communication
HAL_SPI_Transmit(&SPI_Params,data_to_send_receive,data_size,data_timeout); // Send the address of the register to be read on the LIS3DSH
data_to_send_receive[0] = 0x13; // Sample rate of 3.125hz, continous update, x and y enabled
HAL_SPI_Transmit(&SPI_Params,data_to_send_receive,data_size,data_timeout); // Send the new register value to the LIS3DSH through the SPI channel
GPIOE->BSRR = GPIO_PIN_3; // Set the SPI communication enable line high to signal the end of the communication process
	}

// Definition for the function to blink the LED
void Blink_LED(uint8_t LED_state, uint8_t LED_colour){

	if(LED_state == 1){ // Checks to see if the request is to turn the LED on or off

			GPIOD->BSRR = 1<<LED_colour; // Turn on the LED
	
	}
	else{
		
			GPIOD->BSRR = 1<<(LED_colour+16); // Turn off the LED
	
	}
			
}

uint8_t Return_Angle(uint8_t address){  // Returns current angle from LIS3DSH depending on address 
	
	data_to_send_receive[0] = 0x80 | address; // Address is passed for either the MSB Y-axis (H) data register or the MSB X-axis (H) data register on the LIS3DSH
  GPIOE->BSRR = GPIO_PIN_3 << 16; // Set the SPI communication enable line low to initiate communication
  HAL_SPI_Transmit(&SPI_Params, data_to_send_receive, data_size, data_timeout); // Send the address of the register to be read on the LIS3DSH

  data_to_send_receive[0] = 0x00; // Set a blank address because we are waiting to receive data
  HAL_SPI_Receive(&SPI_Params, data_to_send_receive, data_size, data_timeout); // Get the data from the LIS3DSH through the SPI channel
  GPIOE->BSRR = GPIO_PIN_3; // Set the SPI communication enable line high to signal the end of the communication process
	return(data_to_send_receive[0]);  // Return the data read from the specified LIS3DSH register

}
