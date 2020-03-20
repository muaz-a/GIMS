#ifndef LCD_H
#define LCD_H

/* LCD Header */

#include <stdint.h>
#include "stm32f10x.h"
#include "init_sw_led.h"


//Commands for Hitachi 44780 compatible LCD controllers
#define LCD_8B2L 0x38 //; Enable 8 bit data, 2 display lines
#define LCD_DCB 0x0F  //; Enable Display, Cursor, Blink
#define LCD_MCR 0x06  //; Set Move Cursor Right
#define LCD_CLR 0x01  //; Home and clear LCD
#define LCD_LN1 0x80  //;Set DDRAM to start of line 1
#define LCD_LN2 0xC0  //; Set DDRAM to start of line 2

// Control signal manipulation for LCDs on 352/384/387 board
// PB0:RS PB1:ENA PB5:R/W*
#define LCD_CM_ENA 0x00210002 //
#define LCD_CM_DIS 0x00230000 //
#define LCD_DM_ENA 0x00200003 //
#define LCD_DM_DIS 0x00220001


void commandToLCD(uint8_t data);

void dataToLCD(uint8_t data);

void initLCD(void);

void initLCDsequence(void);

void stringToLCD(char * message);

void dipsw_LCD(uint16_t sw_val);

void write_stuff(void);

#endif

