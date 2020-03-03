#ifndef SYSTEMSTARTUP_H
#define SYSTEMSTARTUP_H

#include <stdint.h>
#include "stm32f10x.h"
#include <stdbool.h>
#include "usart.h"
#include "LCD.h"




#define ATPacketSize 19
extern uint8_t buffer[30];
extern uint8_t bsize;
extern bool bfull;

void StartUp(void);
void tim4Init(void);
void packetRecieved(void);

#endif