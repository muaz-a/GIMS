#include "systemstartup.h"

//uint8_t packetSend[ATPacketSize] = {0x7E, 0x00, 0x0F, 0x17, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFE, 0x02, 0x4E, 0x49, 0x53} ;
uint8_t packetSend[ATPacketSize] = {0x7E, 0x00, 0x0F, 0x17, 0x01, 0x00, 0x13, 0xA2, 0x00, 0x41, 0xB1, 0x06, 0x93, 0xFF, 0xFE, 0x02, 0x4E, 0x49, 0x11};
//uint8_t packetSend[22] = {0x7E, 0x00, 0x12, 0x10, 0x01, 0x00, 0x13, 0xA2, 0x00, 0x41, 0xB1, 0x06, 0x93, 0xFF, 0xFE, 0x00, 0x00, 0x54, 0x65, 0x73, 0x74, 0x11};
uint8_t ED1[8] = {0x00, 0x13, 0xA2, 0x00, 0x41, 0xB1, 0x06, 0x93};		// Address of End Device 1
uint8_t ED2[8] = {0x00, 0x13, 0xA2, 0x00, 0x41, 0xB1, 0x15, 0x4B};		// Address of End Device 2
	uint8_t usebuffer[30];
	uint8_t usebsize = 30;
	uint8_t frametype;
	uint16_t switches = 0;
	uint8_t rxdRF[20];
	int rxdsize= 0;
	bool device1, device2, IO;
	int i=0,j=0;
	int m = 0, t = 0;

void StartUp(void)
{
	// send packet to find device names
	 GPIOC->ODR |= GPIO_ODR_ODR9;		// set Blue LED onboard
	for(int p=0; p < 19; p++)
	{
		send_usart(packetSend[p]);
	}
	//TIM2->CR1 |= TIM_CR1_CEN;
	//TIM2->SR &= ~TIM_SR_UIF;	
	//while((TIM2->SR & TIM_SR_UIF) != 1)
	//{
	while(1)
	{
		if(bfull)
		{
			packetRecieved();
		}
		
	}
	TIM2->CR1 &= ~TIM_CR1_CEN;
}

void tim4Init(void)
{
	RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
	

	TIM2->DIER |= TIM_DIER_UIE;
	TIM2->PSC = 59999;		// counter frequency = 400hz;
	TIM2->ARR = 600;
	TIM2->SR &= ~TIM_SR_UIF;	
	
}

void packetRecieved(void)
{
	usebsize = bsize;
	for(m=0; m<usebsize;m++)
	{
		usebuffer[m] = buffer[m];
	}
		frametype = usebuffer[3];
	
	for(i=0; i < 8;i++)				// check sending address starting on byte 5 (index 4)
	{			
		if(usebuffer[i+4]==ED1[i])		// check for End Device 1 match, set flag if matches
			{		
				device1 = true;
			} else 
			{
				device1 = false;
			}
		if (usebuffer[i+4] == ED2[i])				// check for End Device 2 match, set flag if matches
			{		
				device2 = true;
			} else
			{ 
				device2 = false;
			}
	}
	if(frametype == 0x90)
		{
			for(j=15;j<usebsize;j++)
			{
				rxdRF[j-15] = usebuffer[j];
				rxdsize++;
			}
			if(device1)
				{						// if device 1 matches, print to LCD
					commandToLCD(LCD_CLR);
					stringToLCD("End Device 1:");
				}
			if(device2)
				{						// if device 2 matches, print to LCD
					commandToLCD(LCD_CLR);
					stringToLCD("End Device 2:");
				}
			commandToLCD(LCD_LN2);		// print if pin9 was ON or OFF depending on IO flag
			for(t=0; t<	rxdsize; t++)
				{
					dataToLCD(rxdRF[t]);
				}				
		
		commandToLCD(LCD_LN1);		// reset cursor and buffer full flag
		bfull= false;

	}
	
}