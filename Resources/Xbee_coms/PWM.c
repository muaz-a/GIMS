/* PWM.c */

 #include "stm32f10x.h"
 #include "PWM.h"
 
 
 void PWM_init(void)
 {
	  //Enable peripheral clocks for various ports and subsystems
    //Bit 4: Port C Bit3: Port B Bit 2: Port A
    RCC->APB2ENR |=  RCC_APB2ENR_IOPAEN ;
	  RCC->APB2ENR |= RCC_APB2ENR_TIM1EN;

    // Write a 0xB ( 1011b ) into the configuration and mode bits for PA8 (AFIO)
    GPIOA->CRH |= GPIO_CRH_CNF8_1 | GPIO_CRH_MODE8_1 | GPIO_CRH_MODE8_0 ;
    GPIOA->CRH &= ~GPIO_CRH_CNF8_0 ;
	 
	
	 
	 TIM1->CR1 = TIM_CR1_CEN;
	 TIM1->CR2 = TIM_CR2_OIS1;
	 TIM1->EGR = TIM_EGR_UG;
	 TIM1->CCMR1 = TIM_CCMR1_OC1M_2|TIM_CCMR1_OC1M_1|TIM_CCMR1_OC1PE|TIM_CCMR1_OC1FE;
	 TIM1->CCER = TIM_CCER_CC1E;
	 TIM1->PSC = 2399;
	 TIM1->ARR = 100;
	 TIM1->CCR1 = 50;
	 TIM1->BDTR = TIM_BDTR_MOE|TIM_BDTR_OSSI;
	 TIM1->CR1 = TIM_CR1_CEN;
	 
	 
 }
 
 void PWM_Update(uint16_t PWM)
 {
	 TIM1->CCR1 = PWM;
	 TIM1->EGR = TIM_EGR_UG;
 }
 
 

 