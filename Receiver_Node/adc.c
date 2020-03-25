#include "adc.h"

void ADCInit(void)
{
    RCC->APB2ENR |= RCC_APB2ENR_ADC1EN | RCC_APB2ENR_AFIOEN;

    GPIOA->CRL &= ~GPIO_CRL_CNF1 & ~GPIO_CRL_CNF2 & ~GPIO_CRL_CNF3;

    ADC1->CR2 = 0x1;
}

uint32_t readADC(void)
{
  uint32_t rval;

  ADC1->SQR3 = 3; // 3 is our ADC GPIO pin PA3
  ADC1->CR2 = 0x1;
  
  while( (ADC1->SR & ADC_SR_EOC) != ADC_SR_EOC)
  {    
  }
  rval = ADC1->DR;
  
  return rval;
}
