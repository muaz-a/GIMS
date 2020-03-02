#ifndef USART_H
#define USART_H

/* Initialize for USART */
/* Author - Brett Aulie */

#include <stdint.h>
#include "stm32f10x.h"
#include <stdbool.h>

 #define GREEN_ON	0X00000200 //
 #define GREEN_OFF	0x02000000 //
 #define BLUE_ON	0x00000100 //
 #define BLUE_OFF	0x01000000 //

void init_usart(void);

void send_usart(uint16_t sending);

uint16_t recieve_usart(void);

 void USART3_IRQHandler(void);
 
 #endif
 