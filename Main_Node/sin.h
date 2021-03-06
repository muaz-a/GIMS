#ifndef SIN_H
#define SIN_H
#include <stdio.h>
#include <stdint.h>
#include "stm32f10x.h"

/* following define is the step to generate the sinewave, must be a divider of 90.
   The lower INCREMENT, the lower the sinewave frequency. */
#define INCREMENT 5
#define SIN_ARRAY_SIZE 360/INCREMENT

#define TIM6_DAC_IRQn (IRQn_Type)DMA1_Channel3_IRQn
/* Delay value : short one is used for the error coding, long one (~1s) in case 
   of no error or between two bursts */
#define SHORT_DELAY 100
#define LONG_DELAY 1000

/* Error codes used to make the orange led blinking */
#define ERROR_DAC_DMA_UNDERRUN 0x01
#define ERROR_DMA_XFER 0x02
#define ERROR_UNEXPECTED_DMA_IT 0x04
#define ERROR_UNEXPECTED_DAC_IT 0x08

void sine(uint8_t);
void sineOff(void);
void sinGen(uint8_t);
void ConfigureDAC(void);
void ConfigureTIM6(void);
void ConfigureDMA(void);
uint16_t ComputeSinusPolynomialApprox(uint32_t x);
uint16_t GenerateWave(uint8_t);

#endif
