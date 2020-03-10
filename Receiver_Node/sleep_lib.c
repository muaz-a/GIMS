#include "sleep_lib.h"

void sleep(uint32_t seconds){
    int dacOn = 0;
    int adcOn = 0;
    /*
    **  Checks for ADC, DAC, disables
    **  Remember which ones to re-enable
    */
    if ((DAC->CR & DAC_CR_EN1)==DAC_CR_EN1){
        DAC->CR &= ~DAC_CR_EN1;
        dacOn++;
    }
    if ((DAC->CR & DAC_CR_EN2)==DAC_CR_EN2){
        DAC->CR &= ~DAC_CR_EN2;
        dacOn = dacOn+2;
    }
    if ((ADC1->CR2 & ADC_CR2_ADON)==ADC_CR2_ADON){
        ADC1->CR2 &= ~ADC_CR2_ADON;
        adcOn = adcOn+1;
    }
    if ((ADC2->CR2 & ADC_CR2_ADON)==ADC_CR2_ADON){
        ADC2->CR2 &= ~ADC_CR2_ADON;
        adcOn = adcOn+2;
    }
    
    //Setup STOP mode
    SCB->SCR |= SCB_SCR_SLEEPDEEP;
    PWR->CR &= ~PWR_CR_PDDS; //set to stop mode
    PWR->CR |= PWR_CR_LPDS; //set low power mode
    PWR->CR |= PWR_CR_CWUF; //clear WUF
    
    //Enable exit conditions, STOP
    //EXTI8Init(); //Enable to allow early exit
    Configure_RTC(seconds);
    __WFI();
    
    Disable_RTC();
    //EXTI8Dis();
    clockInit();
    
    //currently not re-enabling DAC/ADC on exit
}

/**
void EXTI8Init(void)
{
    //Set External interrupt source to PB8 and PB9
    butInit();
    RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;
    AFIO->EXTICR[2] |= AFIO_EXTICR3_EXTI8_PB | AFIO_EXTICR3_EXTI9_PB;
    EXTI->IMR |= EXTI_IMR_MR8 | EXTI_IMR_MR9;  //turn on interrupt masking
    EXTI->RTSR |= EXTI_RTSR_TR8 | EXTI_RTSR_TR9; //Set rising edge setting
    EXTI->PR |= EXTI_PR_PR8 | EXTI_PR_PR9;
    NVIC->ICPR[0] |= NVIC_ICPR_CLRPEND_23;
    NVIC->ISER[0] |= NVIC_ISER_SETENA_23;
}


//disable interrupt
void EXTI8Dis(void){
    NVIC->ICER[0] |= NVIC_ICER_CLRENA_23;
    NVIC->ICPR[0] |= NVIC_ICPR_CLRPEND_23;
    EXTI->IMR &= ~(EXTI_IMR_MR8 | EXTI_IMR_MR9);
}
**/

void Configure_RTC(uint32_t seconds)
{
  /* Enable the peripheral clock RTC */
  /* (1) Enable the LSI */
  /* (2) Wait while it is not ready */
  /* (3) Enable PWR clock */
  /* (4) Enable write in RTC domain control register */
  /* (5) LSI for RTC clock */
  /* (6) Disable PWR clock */
    RCC->CSR |= RCC_CSR_LSION; /* (1) */
    while((RCC->CSR & RCC_CSR_LSIRDY)!=RCC_CSR_LSIRDY) /* (2) */
    {
        /* add time out here for a robust application */
    }
    RCC->APB1ENR |= RCC_APB1ENR_PWREN | RCC_APB1ENR_BKPEN; /* (3) */
    PWR->CR |= PWR_CR_DBP; /* (4) */
    RCC->BDCR |= RCC_BDCR_BDRST;
    RCC->BDCR &= ~RCC_BDCR_BDRST;
    RCC->BDCR = (RCC->BDCR & ~RCC_BDCR_RTCSEL) | RCC_BDCR_RTCEN | RCC_BDCR_RTCSEL_1; /* (5) */
    
    RCC->APB1ENR &=~ RCC_APB1ENR_PWREN; /* (6) */
    
  /* Configure RTC */
  /* (7) Write access for RTC regsiters */
  /* (8) Disable wake up timerto modify it */
  /* (9) Wait until it is allow to modify wake up reload value */
  /* (10) Modify wake upvalue reload counter to have a wake up each 1Hz */
  /* (11) Enable wake up counter and wake up interrupt */
  /* (12) Disable write access */
    
    while((RTC->CRL & RTC_CRL_RSF) != RTC_CRL_RSF); //wait for synch
    while((RTC->CRL & RTC_CRL_RTOFF) != RTC_CRL_RTOFF); //wait for RTC avail
    RTC->CRL |= RTC_CRL_CNF;
    RTC->CRH &= ~RTC_CRH_ALRIE; 
    RTC->CRL &= ~RTC_CRL_CNF;
    while((RTC->CRL & RTC_CRL_RTOFF) != RTC_CRL_RTOFF);
    RTC->CRL |= RTC_CRL_CNF; /* (7) */
    while((RTC->CRL & RTC_CRL_RTOFF) != RTC_CRL_RTOFF);
    RTC->PRLH |= 0x0; //fTR = 40KHz / (PR+1)
    RTC->PRLL = 39999; /* (10) */ //
    RTC->CNTH = 0;
    RTC->CNTL = 0;
    RTC->ALRH = 0;
    RTC->ALRL = seconds-1;
    RTC->CRL &= ~RTC_CRL_ALRF;
    RTC->CRH |= RTC_CRH_ALRIE; /* (11) */
    RTC->CRL &= ~RTC_CRL_CNF; /* (12) */
    while((RTC->CRL & RTC_CRL_RTOFF) != RTC_CRL_RTOFF) /* (9) */
    {
        /* add time out here for a robust application */
    }

    /* Configure exti and nvic for RTC IT */
    /* (13) unmask line 20 */
    /* (14) Rising edge for line 20 */
    /* (15) Set priority */
    /* (16) Enable RTC_IRQn */
    EXTI->IMR |= EXTI_IMR_MR17; /* (13) */
    EXTI->RTSR |= EXTI_RTSR_TR17; /* (14) */
    EXTI->PR |= EXTI_PR_PR17;
    NVIC->ICPR[1] |= NVIC_ICPR_CLRPEND_9;
    NVIC->ISER[1] |= NVIC_ISER_SETENA_9;
    
}

void Disable_RTC(void) {
    NVIC->ICER[1] |= NVIC_ICER_CLRENA_9;
    EXTI->PR &= ~EXTI_PR_PR17;
    EXTI->IMR &= ~EXTI_IMR_MR17;
    RTC->CRL &=~ RTC_CRL_ALRF;
    while((RTC->CRL & RTC_CRL_RTOFF) != RTC_CRL_RTOFF); //wait for RTC avail
    RTC->CRL |= RTC_CRL_CNF;
    RTC->CRH &= ~RTC_CRH_ALRIE; 
    RTC->CRL &= ~RTC_CRL_CNF;
    while((RTC->CRL & RTC_CRL_RTOFF) != RTC_CRL_RTOFF);
}
