#ifndef ADC_H
#define ADC_H

#include "stm32f10x.h"

// Initialize ADC
void ADCInit(void);

// Read value of ADC
uint32_t readADC(void);
	
#endif
