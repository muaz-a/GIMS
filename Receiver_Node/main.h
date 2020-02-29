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

uint8_t detect_btn_press(void);

void calcSignal(void);

#endif
