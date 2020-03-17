#ifndef MAIN_H
#define MAIN_H

#define DEBUG
#define SAMPLED_CYCLES 30

#define SYNCH '1'
#define SLEEP '2'
#define RESP  '3'
#define ERR   '4'

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "clocks.h"
#include "lcd.h"
#include "adc.h"
#include "dac.h"
#include "xbee.h"
#include "sleep.h"

extern volatile uint8_t alarm;

typedef enum
{
  WAIT_CO_1, // synch at start up - receive a '1' from CO
  SEND_RESPONSE_2, // send back '1' to CO
  WAIT_CO_3, // wait to receive a '2' from CO
  SLEEP_4, // send back '2' go to sleep
  PROCESS_SIG_5, // wake up, tx and rx '3', process signal and tx  
  ERROR_STATE // end up here if there is any error
} state;

uint8_t detect_btn_press(void);

/* Wrapper function that calls calcSignal thrice to retrive 
   median sampled value */
void getSignal(double *amp, double *freq);

/* Calculates amplitude and signal of signal being read from ADC */
void calcSignal(double *amplitude, double *frequency);

/* Expects an array of length 3. Returns 0 if the first element is the median,
   1 if the second element is the median, and 2 if the third element is the median */
int getMedian(double AL3[]); // AL3: Array Length 3

#endif
