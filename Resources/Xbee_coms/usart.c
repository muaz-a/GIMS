/* Initialize Usart */
/* Author - Brett Aulie */

#include "usart.h"

uint8_t buffer[30];
uint8_t bsize = 30;
bool bfull = false;
int k = 0;
 
 void init_usart(void)
 {
	 // start clocks for ports A,B,C, the USART3 and AFIO
	 RCC->APB2ENR |= RCC_APB2ENR_IOPAEN | RCC_APB2ENR_IOPCEN | RCC_APB2ENR_IOPBEN | RCC_APB2ENR_AFIOEN; 
	 RCC->APB1ENR |= RCC_APB1ENR_USART3EN;
	 
		//GPIOB->CRH = 0x4B00;		//configure PB10 to AFIO PP Output and PB11 to Floating Input
	
	 GPIOB->CRH |= GPIO_CRH_MODE10;
	 GPIOB->CRH |= GPIO_CRH_CNF10_1;
	 GPIOB->CRH &= ~GPIO_CRH_CNF10_0;
	  
	 GPIOC->CRH |= GPIO_CRH_MODE9 | GPIO_CRH_MODE8 ;	// set PC8/9 into push-pull outputs
   GPIOC->CRH &= ~GPIO_CRH_CNF9 & ~GPIO_CRH_CNF8 ;
	 
	 USART3->CR1 |= USART_CR1_UE;
	 USART3->BRR = 0x9c4;
	 USART3->CR1 |= USART_CR1_TE | USART_CR1_RE;
	 USART3->CR1 |= USART_CR1_RXNEIE;
	 NVIC->ISER[1] |= NVIC_ISER_SETENA_7;
 }
 
 void send_usart(uint16_t sending)
 {
	 while((USART3->SR & USART_SR_TXE) != USART_SR_TXE)
	 {

	 }
	 	 USART3->DR = sending;
		 return;
 }
 
 uint16_t recieve_usart(void)
 {
	 
	 if((USART3->SR & USART_SR_RXNE) == USART_SR_RXNE)
	 {
		 return USART3->DR;
		 }
	 
 }
 
 void USART3_IRQHandler(void){

	 GPIOC->ODR |= GPIO_ODR_ODR8;		// set Blue LED onboard
	 buffer[k] = recieve_usart();		// enter first byte to buffer
	 if(buffer[0]==0x7e){						// check to make sure it is a packet
		 if (k==2){
			 bsize = buffer[k]+3;				// get the packet length
		 }
		 if(k==bsize){								// once at end of pecket
			 bfull = true;							// set buffer full flag
			 k=0;												// reset k counter
			 GPIOC->ODR &= 0xEF;				// reset Blue LED onboard
		 }else{
			 bfull = false;							// if not at end increment counter
			k++;
		 }
	 }else {							// if first byte wasn't 0x7E reset counter
		 k = 0;
	 }	 
 }