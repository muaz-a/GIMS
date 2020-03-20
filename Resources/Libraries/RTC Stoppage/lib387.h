#ifndef LIB387_H
#define LIB387_H

#include <stdint.h>
#include "stm32f10x.h"

//Commands for Hitachi 44780 compatible LCD controllers
#define LCD_8B2L 0x38 // ; Enable 8 bit data, 2 display lines
#define LCD_DCB 0x0F // ; Enable Display, Cursor, Blink
#define LCD_MCR 0x06 // ; Set Move Cursor Right
#define LCD_CLR 0x01 // ; Home and clear LCD
#define LCD_LN1 0x80 // ;Set DDRAM to start of line 1
#define LCD_LN2 0xC0 // ; Set DDRAM to start of line 2

// Control signal manipulation for LCDs on 352/384/387 board
// PB0:RS PB1:ENA PB5:R/W*
#define LCD_CM_ENA 0x00210002 //
#define LCD_CM_DIS 0x00230000 //
#define LCD_DM_ENA 0x00200003 //
#define LCD_DM_DIS 0x00220001 //

#define GREEN_ON	0X00000200 //
#define GREEN_OFF	0x02000000 //
#define BLUE_ON	0x00000100 //
#define BLUE_OFF	0x01000000 //
#define LEDOFFSET 9
#define NUMLED 4


void clockInit(void);	//initialize clock (24MHz default)

void butInit(void);	//enable buttons (PB8,9, PC12, PA5)
uint8_t push_in(void); //return bit pattern of buttons

void ledInit(void);	//enable onboard leds and led strip (PA9-12)
void ledSet(unsigned int ); //set strip for bit pattern
void ledOn(unsigned int);   //turn on LED (0-3 from right)
void ledOff(unsigned int);  //turn off LED (0-3 from right)
void ledTog(unsigned int);  //toggle LED (0-3 from right)
unsigned int ledStat(void); //return bit pattern of LEDs
void ledInv(void);  //invert bit pattern of LEDs


void ledGBInit(void);   //enable onboard LEDs (Green PC9, blue PC8)
void ledGBSet(unsigned int);    //Set/reset (BSSR)
void ledGBTog(unsigned int);    //XOR output values (ODR)

void lcdInit(void); //enable lcd (PB0,1,5, PC0-7)
void lcd_out(uint8_t, uint8_t); //send command to LED panel (cmd 0, char 1)

//require systic handler
void sysTickInit(uint32_t);  //enable sistick 24e6 ticks ~ 1s
void sysTickDis(void);  //disable sistick



void delay(uint32_t);   //generic delay

/*
void EXTI9_5_IRQHandler (void)
{
    //clear pending bit for EXTI8
    if(EXTI->PR&EXTI_PR_PR8) EXTI->PR |= EXTI_PR_PR8;
    
    //flip leds
    //GPIOC->ODR ^= (GPIO_ODR_ODR8|GPIO_ODR_ODR9);
}


void SysTick_Handler(void){
    
}
*/
#endif
