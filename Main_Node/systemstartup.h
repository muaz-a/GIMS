#ifndef SYSTEMSTARTUP_H
#define SYSTEMSTARTUP_H

#include <stdint.h>
#include "stm32f10x.h"
#include <stdbool.h>
#include "usart.h"
#include "LCD.h";

uint8_t ED1[8] = {0x00, 0x13, 0xA2, 0x00, 0x41, 0xB1, 0x06, 0x93};		// Address of End Device 1
uint8_t ED2[8] = {0x00, 0x13, 0xA2, 0x00, 0x41, 0xB1, 0x15, 0x4B};		// Address of End Device 2


#define ATPacketSize 19
extern uint8_t buffer[30];
extern uint8_t bsize;
extern bool bfull;

void StartUp(void);
void tim4Init(void);
void packetRecieved(void);

#endif