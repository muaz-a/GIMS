#ifndef MAIN_H
#define MAIN_H

#define DEBUG

#include "init_lib.h"
#include "xbee.h"
#include "LCD.h"
#include "stm32f10x.h"
#include <stdbool.h>
#include <stdlib.h> 
#include <stdio.h>
#include "string.h"



extern uint8_t buffer[PACKET_LENGTH];
extern uint8_t bsize;
extern int bcount;
extern bool bfull;

#define NUM_DEVICES 2

typedef enum
{
	POWER_UP_1,
	SEND_SYNCH_2,
	WAIT_RXD_3,
	STAGGER_RESP_4,
	STOP_5				// used for testing
} state;

#endif