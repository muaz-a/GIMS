#include "systemstartup.h"

uint8_t packetSend[ATPacketSize] = {0x7E, 0x00, 0x0F, 0x17, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0x02, 0x4E, 0x49, 0x53} ;


void StartUp(void)
{
	// send packet to find device names
	for(int i=0; i < ATPacketSize; i++)
	{
		send_usart(packetSend[i]);
	}
	TIM2->CR1 |= TIM_CR1_CEN;
	while((TIM2->SR & TIM_SR_UIF) != 1)
	{
		if(bfull)
		{
			
		}
		
	}
}

void tim4Init(void)
{
	RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
	
	TIM2->CR1 |= TIM_CR1_OPM;
	TIM2->DIER |= TIM_DIER_UIE;
	TIM2->PSC |= 59999;		// counter frequency = 400hz;
	TIM2->ARR |= 600;
		
	
}

void packetRecieved(void)
{
	uint8_t usebuffer[30];
	uint8_t usebsize = 30;
	uint8_t frametype;
	uint16_t switches = 0;
	uint8_t rxdRF[10];
	int rxdsize= 0;
	bool device1, device2, IO;
	int i=0,j=0;
	int k = 0, m = 0, t = 0;
	
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