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
    
    tiltSwitchInit();
    XbeeSleepIOInit();
  
    /*#ifdef DEBUG
    // to output system clock to a GPIO pin
    GPIOA->CRH |= GPIO_CRH_MODE8 | GPIO_CRH_CNF8_1; // Enable PA8 as AFIO output
    GPIOA->CRH &= ~GPIO_CRH_CNF8_0;
    RCC->CFGR |= (1u << 26); // output SYSCLK to MCO (PA8)
    #endif*/
}

void sysTick_LPF_clkInit(void)
{
  // Set PB14 as output
  GPIOB->CRH |= GPIO_CRH_MODE14;
  GPIOB->CRH &= ~GPIO_CRH_CNF14;
  
  SysTick->CTRL = 0x00;
  SysTick->VAL = 0x00;
  SysTick->LOAD = 24e6/6000; // Gives us a 3 kHz signal
  SysTick->CTRL = 0x07;
}

void timerInit(void)
{
  RCC->APB1ENR |= RCC_APB1ENR_TIM2EN; // TIM2 APB1 enable
  TIM2->CR1 |= TIM_CR1_ARPE; // ARR register buffered
  TIM2->ARR = 0;
  TIM2->PSC = 0;
  TIM2->CR1 |= TIM_CR1_CEN; // enable timer, by default upcounter
}

void SysTick_Handler(void)
{
  // Turns PB14 on and off to give us a square wave clock
  GPIOB->ODR ^= GPIO_ODR_ODR14;
}

void RTCAlarm_IRQHandler(void)
{
    if((EXTI->PR & EXTI_PR_PR17) == EXTI_PR_PR17)
    {
        EXTI->PR |= EXTI_PR_PR17;
        if ((RTC->CRL & RTC_CRL_ALRF)==RTC_CRL_ALRF)
        {
            RTC->CRL &= ~RTC_CRL_ALRF;
            alarm = 1;
        }
    }
    else
    {
        NVIC_DisableIRQ(RTCAlarm_IRQn); // Disable
    }
}

void tiltSwitchInit(void)
{  
    // AFIO EN for external interrupt (PB15 for tilt switch)
    RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;
    
    // Set PB15 as push/pull input 
    GPIOB->CRH &= ~GPIO_CRH_MODE15;
    GPIOB->CRH &= ~GPIO_CRH_CNF15_0; // clr bit 0
    GPIOB->CRH |=  GPIO_CRH_CNF15_1; // set bit 1

    // enable external interrupt on PB15 and PA0
    AFIO->EXTICR[3] |= AFIO_EXTICR4_EXTI15_PB; // select source for PB15
    AFIO->EXTICR[0] |= AFIO_EXTICR1_EXTI0_PA; // select source for PA0 (USER)
    EXTI->IMR |= EXTI_IMR_MR15 | EXTI_IMR_MR0; // unmask source
    EXTI->RTSR |= EXTI_RTSR_TR15; // select rising edge for PB15
    EXTI->FTSR |= EXTI_FTSR_TR0; // falling edge for user button (PA0)
    EXTI->PR |= EXTI_PR_PR15 | EXTI_PR_PR0; 
    NVIC->ICPR[1] |= NVIC_ICPR_CLRPEND_8;
    NVIC->ISER[1] |= NVIC_ISER_SETENA_8; // Bit 8 is pos 40 as per pg 131/RM (PB15)
    NVIC->ISER[0] |= NVIC_ISER_SETENA_6; // Bit 6 for EXTI0 for PA0
}

void XbeeSleepIOInit(void)
{
  // Set PA1 as output
  GPIOA->CRL |= GPIO_CRL_MODE1;
  GPIOA->CRL &= ~GPIO_CRL_CNF1;
}
