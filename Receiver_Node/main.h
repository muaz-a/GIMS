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
#include "xbee.h"

typedef enum
{
  WAIT_CO_1,
  SEND_RESPONSE_2,
  WAIT_CO_3,
  WAIT_TIME_4,
  PROCESS_SIG_5,
  ERROR_STATE
} state;

typedef enum
{
  SYNCH = '1',
  REQ = '2',
  RESP = '3',
  OTHER = '4'
} message;

uint8_t detect_btn_press(void);

void calcSignal(void);

#endif
