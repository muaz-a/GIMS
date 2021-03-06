#include "sleep.h"

volatile uint8_t alarm = 0;

void sleep(uint32_t seconds)
{    
    // Setup STOP mode
    SCB->SCR |= SCB_SCR_SLEEPDEEP;
    PWR->CR &= ~PWR_CR_PDDS; // set to stop mode
    PWR->CR |= PWR_CR_LPDS; // set low power mode
    PWR->CR |= PWR_CR_CWUF; // clear WUF

    // Send Xbee to sleep
    XbeeSleep();

    //Enable exit conditions, STOP
    //EXTI8Init(); //Enable to allow early exit
    Configure_RTC(seconds);
    alarm = 0;
    backToSleep = 0;
    __WFI();

    while(!alarm)
    {
        __WFI();
        backToSleep = 0;
    }

    // Wake Xbee up
    XbeeWake();

    Disable_RTC();
    clockInit();
    portInit();
    ADCInit();
    usartInit();
    sysTick_LPF_clkInit();

#ifdef DEBUG
    LCDInit();
#endif
}

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
    NVIC->ISER[1] |= NVIC_ISER_SETENA_7;      // reenable UART Interrupt
    EXTI->PR &= ~EXTI_PR_PR17;
    EXTI->IMR &= ~EXTI_IMR_MR17;
    RTC->CRL &=~ RTC_CRL_ALRF;
    while((RTC->CRL & RTC_CRL_RTOFF) != RTC_CRL_RTOFF); //wait for RTC avail
    RTC->CRL |= RTC_CRL_CNF;
    RTC->CRH &= ~RTC_CRH_ALRIE;
    RTC->CRL &= ~RTC_CRL_CNF;
    while((RTC->CRL & RTC_CRL_RTOFF) != RTC_CRL_RTOFF);
}
