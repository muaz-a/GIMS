

 #include "stm32f10x.h"
 #include "SPI.h"
 
 void spi_init(void)
 {
	 
	 	 // start clocks for ports A,B,C, the SPI and AFIO
	 RCC->APB2ENR |= RCC_APB2ENR_IOPAEN | RCC_APB2ENR_IOPCEN | RCC_APB2ENR_IOPBEN | RCC_APB2ENR_AFIOEN;
	 RCC->APB1ENR |= RCC_APB1ENR_SPI2EN;


	 GPIOB->CRH |= GPIO_CRH_MODE15;		// PB15 in AFIO P/P (1011)
	 GPIOB->CRH |= GPIO_CRH_CNF15_1;
	 GPIOB->CRH &= ~GPIO_CRH_CNF15_0;
	 
	 GPIOB->CRH |= GPIO_CRH_MODE13;		// PB13 in AFIO P/P (1011)
	 GPIOB->CRH |= GPIO_CRH_CNF13_1;
	 GPIOB->CRH &= ~GPIO_CRH_CNF13_0;
	 
	 GPIOB->CRH |= GPIO_CRH_MODE12;		// PB12 in GPIO P/P (0011)
	 GPIOB->CRH |= GPIO_CRH_CNF12;
	 GPIOB->CRH &= ~GPIO_CRH_CNF12;

	 GPIOB->ODR |= SPI2_SS_HI;
	 SPI2->CR2 = 0x0;
	 SPI2->CR1 = 0x0;
	 SPI2->CR1 = 0x0000032c;
	 SPI2->CR1 |= SPI_CR1_SPE;
	 
	 
 }
 
 
 uint16_t use_spi(uint16_t data)
 {
	 uint16_t recieved;
	 
	 GPIOB->ODR &= SPI2_SS_HI;
	  if((SPI2->SR & SPI_SR_TXE) == SPI_SR_TXE)
		{
			SPI2->DR = data;
		}
	 if((SPI2->SR & SPI_SR_RXNE) == SPI_SR_RXNE)
	 {
		 recieved = SPI2->DR;
	 }
	 GPIOB->ODR |= SPI2_SS_HI;
	 return recieved;
 }
 
 
 
 