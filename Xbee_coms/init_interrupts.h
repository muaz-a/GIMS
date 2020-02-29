#ifndef INIT_INTERRUPT_H
#define INIT_INTERRUPT_H

/* Interrupts Header*/

#include <stdint.h>
#include "stm32f10x.h"
 
// initialize systick internal interrupt
void systick_init(void);

// initialize external interrupt
void exti0_init(void);

#endif
