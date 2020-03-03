#ifndef MAIN_H
#define MAIN_H

#define DEBUG
#define SAMPLED_CYCLES 30

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "clocks.h"
#include "lcd.h"
#include "adc.h"
#include "dac.h"

typedef enum
{
	WAIT_MAIN_1,
	SEND_RESPONSE_2,
	WAIT_MAIN_3,
	WAIT_TIME_4,
	PROCESS_SIG_5
} state;

uint8_t detect_btn_press(void);

void calcSignal(void);

#endif
