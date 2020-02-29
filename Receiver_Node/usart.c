/* Initialize Usart */
/* Author - Brett Aulie */

#include "usart.h"

/* Globals to transfer data from USART */
uint8_t usartBuf[30];
uint8_t bufSize = 30;
bool bufFull = false;

static int k = 0;
 
 void usartInit(void)
 {
	 // start clocks for ports A,B,C, the USART3 and AFIO
	 RCC->APB2ENR |= RCC_APB2ENR_IOPAEN | RCC_APB2ENR_IOPCEN | RCC_APB2ENR_IOPBEN | RCC_APB2ENR_AFIOEN; 
	 RCC->APB1ENR |= RCC_APB1ENR_USART3EN;	 	
	
	 GPIOB->CRH |= GPIO_CRH_MODE10;
	 GPIOB->CRH |= GPIO_CRH_CNF10_1;
	 GPIOB->CRH &= ~GPIO_CRH_CNF10_0;	  
	 
	 USART3->CR1 |= USART_CR1_UE;
	 USART3->BRR = 0x9c4;
	 USART3->CR1 |= USART_CR1_TE | USART_CR1_RE;
	 USART3->CR1 |= USART_CR1_RXNEIE;
	 NVIC->ISER[1] |= NVIC_ISER_SETENA_7;
 }
 
 void send_usart(uint16_t data)
 {
	 while((USART3->SR & USART_SR_TXE) != USART_SR_TXE)
	 {

	 }
	 USART3->DR = data;
	 return;
 }
 
 uint16_t recieve_usart(void)
 {	 
	 if((USART3->SR & USART_SR_RXNE) == USART_SR_RXNE)
	 {
		 return USART3->DR;
	 }	 
 }
 
 void USART3_IRQHandler(void)
{
	usartBuf[k] = recieve_usart();		// enter first byte to usartBuf
	
	if(usartBuf[0] == 0x7e)
	{						// check to make sure it is a packet
	 if (k == 2){
		 bufSize = usartBuf[k] + 3;				// get the packet length
	 }
	 
	  if (k==bufSize)
	  {								// once at end of packet
		 bufFull = true;							// set usartBuf full flag
		 k = 0;												// reset k counter
	  } else
	  {
		 bufFull = false;							// if not at end increment counter
		 k++;
	  }
	} else 
	{							// if first byte wasn't 0x7E reset counter
	 k = 0;
	}	 
}