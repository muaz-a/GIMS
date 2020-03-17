#ifndef SLEEP_LIB_H
#define SLEEP_LIB_H

#include "stm32f10x.h"
#include "clocks.h"
#include "lcd.h"
#include "xbee.h"
#include "adc.h"

void sleep(uint32_t);
void Configure_RTC(uint32_t);
void Disable_RTC(void);
//require interrupt handler
void EXTI8Init(void); //enable interrupt (PB8)
void EXTI8Dis(void);    //disable interrupt (PB8)
#endif
