#include "lcd.h"

void LCDInit(void)
{  
    //PB0: LCD rs Mode 11, CNF 00
    GPIOB->CRL |= GPIO_CRL_MODE0;
    GPIOB->CRL &= ~GPIO_CRL_CNF0;
  
    //PB1: LCD E Mode 11, CNF 00
    GPIOB->CRL |= GPIO_CRL_MODE1;
    GPIOB->CRL &= ~GPIO_CRL_CNF1;
  
    //PB5: LCD r/w_n Mode 11, CNF 00
    GPIOB->CRL |= GPIO_CRL_MODE5;
    GPIOB->CRL &= ~GPIO_CRL_CNF5;
  
    //PC7-PC0 LCD read inputs Mode 11, CNF 00
    GPIOC->CRL |= GPIO_CRL_MODE0 | GPIO_CRL_MODE1 | GPIO_CRL_MODE2 | GPIO_CRL_MODE3 |
                  GPIO_CRL_MODE4 | GPIO_CRL_MODE5  | GPIO_CRL_MODE6 | GPIO_CRL_MODE7;
    GPIOC->CRL &= ~GPIO_CRL_CNF0 & ~GPIO_CRL_CNF1 & ~GPIO_CRL_CNF2 & ~GPIO_CRL_CNF3 &
                  ~GPIO_CRL_CNF4 & ~GPIO_CRL_CNF5 & ~GPIO_CRL_CNF6 & ~GPIO_CRL_CNF7;
  
    ToLCD(LCD_8B2L, 0);
    ToLCD(LCD_8B2L, 0);
    ToLCD(LCD_8B2L, 0);
    ToLCD(LCD_8B2L, 0);
    ToLCD(LCD_DCB, 0);
    ToLCD(LCD_CLR, 0);
    ToLCD(LCD_MCR, 0);  
}

void ToLCD(uint8_t data, uint8_t mode)
{
  if (mode == 0)
  {
    GPIOB->BSRR = LCD_CM_ENA; //RS low, E high - command mode enabled
    // GPIOC->ODR = data; //BAD: may affect upper bits on port C
    GPIOC->ODR &= 0xFF00; //GOOD: clears the low bits without affecting high bits
    GPIOC->ODR |= data; //GOOD: only affects lowest 8 bits of Port C
    delay(8000);
    GPIOB->BSRR = LCD_CM_DIS; //RS low, E low
    delay(80000);
  }
  else if (mode == 1)
  {
    GPIOB->BSRR = LCD_DM_ENA; //RS low, E high - data mode enabled
    GPIOC->ODR &= 0xFF00; //GOOD: clears the low bits without affecting high bits
    GPIOC->ODR |= data; //GOOD: only affects lowest 8 bits of Port C
    delay(8000);
    GPIOB->BSRR = LCD_DM_DIS; //RS low, E low
    delay(80000);
  }
  
  return;
}

void stringToLCD(char * message)
{
  int i=0;
  uint16_t messageLength = strlen(message);
  for (i=0; i<messageLength; ++i)
  {
    ToLCD(*message, 1);
    ++message;
  }
}

// MHA: Remember that each loop iteration takes about 4 clock ticks!
void delay(uint32_t count)
{
    int i=0;
    for(i=0; i<count; ++i)
    {
    }
}
