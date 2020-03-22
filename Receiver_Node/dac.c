#include "dac.h"

/* Private variables ---------------------------------------------------------*/
volatile uint16_t error = 0;  //initialized at 0 and modified by the functions 
uint16_t sin_data[SIN_ARRAY_SIZE]; //table containing the data to generate the sinewave 

void dacInit(double amplitude, uint16_t frequency)
{
  uint32_t x;
  for (x = 0; x < SIN_ARRAY_SIZE; x++)
  {
    sin_data[x] = GenerateWave(amplitude);
  }
  
  ConfigureGPIO();
  ConfigureGPIOasAnalog();
  ConfigureDAC();
  ConfigureDMA();
  ConfigureTIM6(frequency);
}

void  ConfigureGPIO(void)
{
    //Set the config and mode bits for Port C bit 9 and 8 so they will
    // be push-pull outputs (up to 50 MHz)
    //GREEN(PC9) BLUE(PC8)
    GPIOC->CRH |= GPIO_CRH_MODE9 | GPIO_CRH_MODE8 ;
    GPIOC->CRH &= ~GPIO_CRH_CNF9 & ~GPIO_CRH_CNF8 ;
}

void  ConfigureGPIOasAnalog(void)
{
    /* (1) Enable the peripheral clock of GPIOA */
    /* (2) Select analog mode for PA4 */
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN ; /* (1) */
    GPIOA->CRL |= GPIO_CRL_MODE4;
    GPIOA->CRL &= ~GPIO_CRL_CNF4;
}

void  ConfigureDAC(void)
{
  /* (1) Enable the peripheral clock of the DAC */
  /* (2) Enable DMA transfer on DAC ch1, 
         enable interrupt on DMA underrun DAC, 
         enable the DAC ch1, enable the trigger on ch 1,
         disable the buffer on ch1, 
         and select TIM6 as trigger by keeping 000 in TSEL1 */
  RCC->APB1ENR |= RCC_APB1ENR_DACEN; /* (1) */
  DAC->CR |= (0x1<<13) | DAC_CR_DMAEN1 | DAC_CR_BOFF1 | DAC_CR_TEN1 | DAC_CR_EN1; /* (2) */  

  /* Configure NVIC for DAC */
  /* (3) Enable Interrupt on DAC Channel1 and Channel2 */
  /* (4) Set priority for DAC Channel1 and Channel2 */
  NVIC_EnableIRQ(TIM6_DAC_IRQn); /* (3)*/
  NVIC_SetPriority(TIM6_DAC_IRQn,0); /* (4) */

  DAC->DHR12R1 = 2048; /* Initialize the DAC value to middle point */
}

void ConfigureTIM6(uint16_t frequency)
{
  /* (1) Enable the peripheral clock of the TIM6 */ 
  /* (2) Configures MMS=010 to output a rising edge at each update event */
  /* (3) Select PCLK/2 i.e. 48MHz/2=24MHz */
  /* (4) Set one update event each 1 microsecond */
  /* (5) Enable TIM6 */
  RCC->APB1ENR |= RCC_APB1ENR_TIM6EN; /* (1) */ 
  TIM6->CR2 |= TIM_CR2_MMS_1; /* (2) */
  TIM6->PSC = 0; /* (3) */
  TIM6->ARR = (uint16_t)(66000 / frequency); /* (4) */
  TIM6->CR1 |= TIM_CR1_CEN; /* (5) */
}

void ConfigureDMA(void)
{
  /* (1) Enable the peripheral clock on DMA */ 
  /* (2) Configure the peripheral data register address */
  /* (3) Configure the memory address */
  /* (4) Configure the number of DMA tranfer to be performs on DMA channel x */
  /* (5) Configure increment, size (16-bits), interrupts, transfer from memory to peripheral and circular mode */
  /* (6) Enable DMA Channel x */
  RCC->AHBENR |= RCC_AHBENR_DMA1EN; /* (1) */
  DMA1_Channel3->CPAR = (uint32_t) (&(DAC->DHR12R1)); /* (2) */
  DMA1_Channel3->CMAR = (uint32_t)(sin_data); /* (3) */
  DMA1_Channel3->CNDTR = SIN_ARRAY_SIZE; /* (4) */
  DMA1_Channel3->CCR |= DMA_CCR3_MINC | DMA_CCR3_MSIZE_0 | DMA_CCR3_PSIZE_0 \
                      | DMA_CCR3_DIR | DMA_CCR3_TEIE | DMA_CCR3_CIRC; /* (5) */   
  DMA1_Channel3->CCR |= DMA_CCR3_EN; /* (6) */

  /* Configure NVIC for DMA */
  /* (7) Enable Interrupt on DMA Channels x */
  /* (8) Set priority for DMA Channels x */  
  NVIC_EnableIRQ(DMA1_Channel3_IRQn); /* (7) */
  NVIC_SetPriority(DMA1_Channel3_IRQn,3); /* (8) */
}

uint16_t ComputeSinusPolynomialApprox(uint32_t x)
{
float sin_p7;
float angle, angle_p;

  angle = ((float)x)*3.14159/180;
  sin_p7 = angle; 
  angle_p = angle*angle*angle; /* angle_p = angle^3 */
  sin_p7 -= angle_p/6;
  angle_p = angle_p*angle*angle; /* angle_p = angle^5 */
  sin_p7 += angle_p/120;
  angle_p = angle_p*angle*angle; /* angle_p = angle^7 */
  sin_p7 -= angle_p/5040;
  sin_p7 *= 2048;
  return((uint16_t)sin_p7);   
}

uint16_t GenerateWave(double amplitude)
{
static uint16_t i = 0;
uint16_t data = 0;

  if (i < 90)
  {
    data = 0x800 + ComputeSinusPolynomialApprox(i);
    i+=INCREMENT;
  }
  else if (i < 180)  /* PI/2 < i < PI */
  {
    data = 0x800 + ComputeSinusPolynomialApprox(180-i);
    i+=INCREMENT;
  }
  else if (i < (180 + 90))  /* PI < i < 3PI/2 */
  {
    data = 0x800 - ComputeSinusPolynomialApprox(i-180);
    i+=INCREMENT;
  }
  else if (i < 360)  /* 3PI/2 < i < 2PI */
  {
    data = 0x800 - ComputeSinusPolynomialApprox(360-i);
    i+=INCREMENT;
    if (i >= 360)
    {
      i=0;
    }
  }
  return(amplitude * data);
}

/******************************************************************************/
/*                 STM32F0xx Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f072xb.s).                                               */
/******************************************************************************/

/**
  * @brief  This function handles DMA Channel2 and 3 interrupt request.
  *         It only manages DMA error on channel 3
  * @param  None
  * @retval None
  */
void DMA1_Channel3_IRQHandler(void)
{
  
  if ((DMA1->ISR & DMA_ISR_TEIF1) != 0) /* Test if transfer error on DMA channel 1 */
  {
    error |= ERROR_DMA_XFER; /* Report an error on DMA transfer */
    DMA1->IFCR |= DMA_IFCR_CTEIF1; /* Clear the flag */
  }
  else
  {
    error |= ERROR_UNEXPECTED_DMA_IT; /* Report unexpected DMA interrupt occurrence */
  }
}


/**
  * @brief  This function handles DMA Channel1 interrupt request.
  *         It only manages DMA error
  * @param  None
  * @retval None
  */
void TIM6_DAC_IRQHandler(void)
{
  
  if ((DAC->SR & DAC_SR_DMAUDR1) != 0) /* Test if transfer error on DMA and DAC channel 1 */
  {
    error |= ERROR_DAC_DMA_UNDERRUN; /* Report an error on DMA underrun */
    DAC->SR |= DAC_SR_DMAUDR1;
  }
  else
  {
    error |= ERROR_UNEXPECTED_DAC_IT; /* Report unexpected DMA interrupt occurrence */
  }
}
