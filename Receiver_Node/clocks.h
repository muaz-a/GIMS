#ifndef CLOCKS_H
#define CLOCKS_H

#define DEBUG

#include "stm32f10x.h"

// Initialize the Cortex M3 clock using the RCC registers
void clockInit(void);

// Initialize port clocks and modes
void portInit(void);

// Initialize TIM2 for frequency measurement
void timerInit(void);

#endif
