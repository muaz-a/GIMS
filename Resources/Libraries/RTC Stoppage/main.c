#include "stm32f10x.h"
#include "sleep_lib.h"
#include "lib387.h"

void led_wigwag(void);

int main(void)
{
    clockInit();  //Sets up the processor clock to run at 24MHz
    //butInit();
    
    //Startup routine
    ledGBInit();
    led_wigwag();
    ledGBSet(GREEN_OFF|BLUE_OFF);
    
    while(1){
        //turn on an LED and STOP
        ledGBSet(BLUE_ON);
        sleep(1);
        ledGBSet(BLUE_OFF);
        //turn on an LED and active wait
        ledGBSet(GREEN_ON);
        delay(12000000);
        ledGBSet(GREEN_OFF);
    }
}


void EXTI9_5_IRQHandler (void)
{
    //PB8: set global to 1, clear reset flag
    if(EXTI->PR&EXTI_PR_PR8) {
        EXTI->PR |= EXTI_PR_PR8;
    }
    //PB9: clear reset flag, set global to 2
    else if (EXTI->PR&EXTI_PR_PR9) {
        EXTI->PR |= EXTI_PR_PR9;
    }
    else
    {
        NVIC_DisableIRQ(EXTI9_5_IRQn);
    }
}


void RTCAlarm_IRQHandler(void)
{
    if((EXTI->PR & EXTI_PR_PR17) == EXTI_PR_PR17)
    {
        EXTI->PR |= EXTI_PR_PR17;
        if ((RTC->CRL & RTC_CRL_ALRF)==RTC_CRL_ALRF){
            RTC->CRL &= ~RTC_CRL_ALRF;
        }
    }
    else
    {
        NVIC_DisableIRQ(RTCAlarm_IRQn);// Disable
    }
}

void led_wigwag()
{
    int i=0;
    ledGBSet(BLUE_ON | GREEN_OFF);
    for(i=0; i< 1200000; ++i);
    ledGBTog(BLUE_ON|GREEN_ON);
    for(i=0; i< 1200000; ++i);
    ledGBTog(BLUE_ON|GREEN_ON);
    
}
