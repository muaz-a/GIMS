#ifndef USART_H
#define USART_H

/* Initialize for USART */
/* Author - Brett Aulie */

#include <stdint.h>
#include "stm32f10x.h"
#include <stdbool.h>

void usartInit(void);
void send_usart(uint16_t data);
void USART3_IRQHandler(void);
uint16_t recieve_usart(void);

#endif
 