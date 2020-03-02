/* Initialize and interact with switches and LEDs */
/* Author - Brett Auile */


 #include "init_sw_led.h"
 
// Delay Function by Dave Duguid
void delay(uint32_t count)
{
    int i=0;
    for(i=0; i< count; ++i)
    {
    }
}

void led_IO_init (void)
{
		//Enable peripheral clocks for various ports and subsystems
		//Bit 4: Port C Bit3: Port B Bit 2: Port A
		RCC->APB2ENR |=  RCC_APB2ENR_IOPCEN | RCC_APB2ENR_IOPBEN
        | RCC_APB2ENR_IOPAEN ;

			//Set the config and mode bits for 384 Board buttons on Port A,B,C
			// Write a 0xB (1011) into configuration and mode bits for PA9,PA10,PA11,PA12
			GPIOA->CRH |= GPIO_CRH_MODE9 | GPIO_CRH_MODE10 | GPIO_CRH_MODE11
							| GPIO_CRH_MODE12;
			GPIOA->CRH |= GPIO_CRH_CNF9_0 | GPIO_CRH_CNF10_0 | GPIO_CRH_CNF11_0
								| GPIO_CRH_CNF12_0;	
		GPIOC->CRH |= GPIO_CRH_MODE9 | GPIO_CRH_MODE8 ;
    GPIOC->CRH &= ~GPIO_CRH_CNF9 & ~GPIO_CRH_CNF8 ;
			GPIOA->ODR = 0x00001E00;
}


uint16_t read_switches(void)
{
	uint16_t sw_val;
	sw_val = (((GPIOA->IDR & (GPIO_IDR_IDR6 | GPIO_IDR_IDR7))>>6)			// dip switch 1.1, 1.2
					|((GPIOC->IDR & (GPIO_IDR_IDR10 | GPIO_IDR_IDR11))>>8)		// dip switch 1.3, 1.4
					|((GPIOB->IDR & (GPIO_IDR_IDR8 | GPIO_IDR_IDR9))>>4)			// red, black switch sw2, sw3
					|((GPIOC->IDR & (GPIO_IDR_IDR12))>>6)										// blue switch sw4
					|((GPIOA->IDR & (GPIO_IDR_IDR5))<<2))											// green switch sw5
					& 0xFF;
					return (sw_val);					// sw_val = sw5,sw4,sw3,sw2,sw1.4,sw1.3,sw1.2,sw1.1
}

void turnonled(void)
{
	int i  = 2400000;  
	
	GPIOA->ODR = 0x00000A00;
	delay(i);
	GPIOA->ODR = 0x00001400;
	delay(i);
}


void use_switches(uint16_t sw_val)
{
	
	
if ((sw_val & 0x4) ==0x4){
	GPIOA->ODR &= ~(0x200);				// set LED1 if switch1.3 is on
}else {										
	GPIOA->ODR |= 0x200;
}
if ((sw_val & 0x8) ==0x8){			// set LED2 if switch1.4 is on
	GPIOA->ODR &= ~(0x400);
}else {
	GPIOA->ODR |= 0x400;
}
	
}




