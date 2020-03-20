/* Initialize ADC */

 #include "stm32f10x.h"
 #include "LCD.h"
 #include "init_sw_led.h"
 #include "string.h"

void adc_init(void)
{
	RCC->APB2ENR |= RCC_APB2ENR_IOPAEN | RCC_APB2ENR_IOPCEN | RCC_APB2ENR_IOPBEN | RCC_APB2ENR_ADC1EN | RCC_APB2ENR_AFIOEN;
	
	GPIOA->CRL =  0x00000000;
	
	ADC1->CR2 = 0x00000001;
	
	
}

uint16_t read_adc(uint16_t port)
{
	if(port == 1)
	{
		ADC1->SQR3 = ADC_SQR3_SQ1_0;  // LM35
	}
	else {ADC1->SQR3 = ADC_SQR3_SQ1_1;	// 10k POT
	}
	
	ADC1->CR2 = 0x00000001;
	
	while((ADC1->SR & ADC_SR_EOC) != ADC_SR_EOC)
	{
		
	}
	return((ADC1->DR));
}



void hex2asxii(uint16_t hexval, uint16_t port)
{
	int i;
	uint16_t output;
	uint16_t asciival;
	
	if(port==1)
	{
		hexval = hexval;
	}
	else {hexval = hexval;}
	
	output = hexval;
	
	
	
		for(i=28; i>=0; i=i-4)
	{
		hexval=(output>>i)& 0xF;
	
		if (hexval > 0x9)
		{
			asciival = hexval+0x37;
		}
		else{
			asciival = hexval+0x30;
			}
		/*	
		if(hexval > 0x9)
	{
		dataToLCD(0x31);
		dataToLCD(hexval+38);
	}
	else {
		dataToLCD(hexval+0x30);
	}
		*/
			dataToLCD(asciival);
			
	}
}

void hex2PMW(uint16_t hexval)
{

	int ccr1=0;
	int ccr;
	

	
	ccr1 = hexval/100;
	
}

