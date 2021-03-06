#ifndef CLOCKS_H
#define CLOCKS_H

#include "stm32f10x.h"
#include "sleep.h"

extern volatile uint8_t alarm;

// Initialize the Cortex M3 clock using the RCC registers
void clockInit(void);

// Initialize port clocks and modes
void portInit(void);

// Setting up ext interrupt for tilt switch input on PB15
// Also sets up USER button (PA0) to reset the tilted device condition
void tiltSwitchInit(void);

// Init PC15 to send sleep signal to Xbee
void XbeeSleepIOInit(void);

// Initialize systick as the clock for the Low Pass Filter
void sysTick_LPF_clkInit(void);

// Initialize TIM2 for frequency measurement
void timerInit(void);

#endif
