#ifndef MAIN_H
#define MAIN_H


#include "init_lib.h"
#include "usart.h"
#include "LCD.h"
#include "init_sw_led.h"
#include "stm32f10x.h"
#include <stdbool.h>
#include <stdlib.h> 
#include <stdio.h>
#include "string.h"



extern uint8_t buffer[30];
extern uint8_t bsize;
extern bool bfull;

int hex_to_int(uint8_t c);

#endif
