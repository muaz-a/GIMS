#ifndef INIT_SW_LED_H
#define INIT_SW_LED_H

/* Initialize Switches and LEDs Header*/
/* Author - Brett Aulie */

#include <stdint.h>
#include "stm32f10x.h"

// bit masks for 384 Board LEDs on PA9,PA10,PA11,PA12 using GPIOA_BSRR
// 

#define LED1_ON 0x00000200
#define LED1_OFF 0x02000000
#define LED2_ON 0x00000400
#define LED2_OFF 0x04000000
#define LED3_ON 0x00000800
#define LED3_OFF 0x08000000
#define LED4_ON 0x00001000
#define LED4_OFF 0x10000000

// initialize LED's
void led_IO_init (void);
//read switches on 384 board
uint16_t read_switches(void);

void delay(uint32_t count);
// use switch signals to turn on LEDs
void use_switches(uint16_t sw_val);

// initialize clocks
void clockInit(void);

void turnonled(void);

#endif
