#ifndef MAIN_H
#define MAIN_H

#define DEBUG
#define STAGGER 10
#define RXDTIM 3
#define SINTIM 30


#define SYNCH '1'
#define SLEEP '2'
#define RESP '3'
#define ERR '4'

#include "init_lib.h"
#include "xbee.h"
#include "LCD.h"
#include "sleep_lib.h"
#include "stm32f10x.h"
#include <stdbool.h>
#include <stdlib.h> 
#include <stdio.h>
#include "string.h"
#include "sin.h"



extern volatile uint8_t buffer[PACKET_LENGTH];
extern uint8_t bsize;
extern int bcount;
extern bool bfull;
extern volatile uint8_t alarm;



typedef enum
{
	POWER_UP_1,
	SEND_SYNCH_2,
	INDEX_3,
	STAGGER_RESP_4,
	LISTEN_5,
	ANALYZE_6
} state;

#endif
