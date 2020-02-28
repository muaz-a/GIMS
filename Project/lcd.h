#ifndef LCD_H
#define LCD_H

#include <string.h>
#include "stm32f10x.h"

//Commands for Hitachi 44780 compatible LCD controllers
#define LCD_8B2L 0x38 // ; Enable 8 bit data, 2 display lines
#define LCD_DCB 0x0F // ; Enable Display, Cursor, Blink
#define LCD_MCR 0x06 // ; Set Move Cursor Right
#define LCD_CLR 0x01 // ; Home and clear LCD
#define LCD_LN1 0x80 // ;Set DDRAM to start of line 1
#define LCD_LN2 0xC0 // ; Set DDRAM to start of line 2

// Control signal manipulation for LCD
// PB0:RS PB1:ENA PB5:R/W*
#define LCD_CM_ENA 0x00210002
#define LCD_CM_DIS 0x00230000
#define LCD_DM_ENA 0x00200003
#define LCD_DM_DIS 0x00220001

// Initialize the LCD screen
void LCDInit(void);

// Sends single byte of either command or information to the LCD controller
void ToLCD(uint8_t data, uint8_t mode);

// allows to send C-style strings directly to LCD
void stringToLCD(char * message);

// A general purpose countdown timer delay routine
void delay(uint32_t count);

#endif
