/* LCD Stuff */


 #include "LCD.h"

/*
* Name: commandToLCD
* Type: PUBLIC
* Parameters: a single byte of command information for the LCD controller
* Returns: nothing
* Description: This function generates control timing and data signals to send one command byte to the LCD
* Author: Dave Duguid
*/

void initLCD(void)
{		
			// set GPIO Ports B,C that LCD uses
			RCC->APB2ENR |=  RCC_APB2ENR_IOPCEN | RCC_APB2ENR_IOPBEN;
		
			// set GPIO Ports B,C so output General Purpose push-pull @ 50Mhz (0011)
	
			GPIOB->CRL = (GPIO_CRL_MODE0 | GPIO_CRL_MODE1 | GPIO_CRL_MODE5);

			GPIOC->CRL = (GPIO_CRL_MODE0 | GPIO_CRL_MODE1 | GPIO_CRL_MODE2 | GPIO_CRL_MODE3 | GPIO_CRL_MODE4 | GPIO_CRL_MODE5
									| GPIO_CRL_MODE6 | GPIO_CRL_MODE7); 
	

	
			initLCDsequence();
	
}

void commandToLCD(uint8_t data)
{
	int i = 8000;
 GPIOB->BSRR = LCD_CM_ENA;						//RS low, E high
//  GPIOC->ODR = data;							//BAD: may affect upper bits on port C
	
		GPIOC->ODR &= 0xFF00;						//GOOD: clears the low bits without affecting high bits
		GPIOC->ODR |= data;							//GOOD: only affects lowest 8 bits of Port C
	
	
	delay(i);											

 GPIOB->BSRR = LCD_CM_DIS; 					//RS low, E low
	i=80000;
 delay(i);
}

/*	
* Name: dataToLCD
* Type: PUBLIC
* Parameters: a single byte of command information for the LCD controller
* Returns: nothing
* Description: This function generates control timing and data signals to send one command byte to the LCD
* Author: Dave Duguid Modified by Brett Aulie
*/

void dataToLCD(uint8_t data)
{
	int i = 8000;
  GPIOB->BSRR = LCD_DM_ENA;						//RS low, E high
 //  GPIOC->ODR = data;							//BAD: may affect upper bits on port C
	
		GPIOC->ODR &= 0xFF00;						//GOOD: clears the low bits without affecting high bits
		GPIOC->ODR |= data;							//GOOD: only affects lowest 8 bits of Port C
	
	
  delay(i);												

	GPIOB->BSRR = LCD_DM_DIS; 					//RS low, E low
	i=80000;
	delay(i);
}




void initLCDsequence(void)
{

	
	int i = 90000;
	delay(i);
	
	commandToLCD(LCD_8B2L);
	delay(i);
	commandToLCD(LCD_8B2L);
	delay(i);
	commandToLCD(LCD_8B2L);
	delay(i);
	commandToLCD(LCD_8B2L);
	delay(i);
	
	
	commandToLCD(LCD_DCB);
	delay(i);
	
	
	commandToLCD(LCD_CLR);
	delay(i);

	
	commandToLCD(LCD_MCR);
		delay(i);
	
}


void stringToLCD(char * message)
{
	int i=0;
	uint16_t messageLength = strlen(message);
	for (i=0; i<messageLength; ++i)
	{
		if(i>15)
		{commandToLCD(0x7);}
		dataToLCD(*message);
		++message;
		delay(10000);
	}

	commandToLCD(LCD_MCR);
}


void delay(uint32_t count)
{
	for(int i = 0; i <count; i++)
	{
		
	}
	return;
}

