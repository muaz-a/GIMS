#include "clocks.h"

void clockInit(void)
{
    // Initialize clocks
		RCC->CFGR = 0x00050002;     // MHA: see 'ConfiguringSystemClocks.odt' for more info
    RCC->CR =  0x01010081;      // Turn on PLL, HSE, HSI
		// We have a 24 MHz clock derived from 8 MHz HSE with a PLL x 3 multiplier.
    while ((RCC->CR & RCC_CR_PLLRDY) != RCC_CR_PLLRDY)  // Wait for the PLL to stabilize
    {
    }
}

void portInit(void)
{
    // Enable clocks on APB2 for ports A, B, C
    RCC->APB2ENR |=  RCC_APB2ENR_IOPAEN | RCC_APB2ENR_IOPBEN | RCC_APB2ENR_IOPCEN;
	
		#ifdef DEBUG
		// Enable GPIO PB10 as output
		GPIOB->CRH |= GPIO_CRH_MODE10;
		GPIOB->CRH &= ~GPIO_CRH_CNF10;
	
		GPIOB->BSRR = 0x40000000; // set PB10 low at start
		#endif
	
		#ifdef DEBUG
		GPIOA->CRH |= GPIO_CRH_MODE8 | GPIO_CRH_CNF8_1; // Enable PA8 as AFIO output
		GPIOA->CRH &= ~GPIO_CRH_CNF8_0;
		RCC->CFGR |= (1u << 26); // output SYSCLK to MCO (PA8)
		#endif
}

void timerInit(void)
{	
	RCC->APB1ENR |= RCC_APB1ENR_TIM2EN; // TIM2 APB1 enable		
	TIM2->CR1 |= TIM_CR1_ARPE; // ARR register buffered
	TIM2->ARR = 0;
	TIM2->PSC = 0;
	TIM2->CR1 |= TIM_CR1_CEN; // enable timer, by default upcounter	
}
