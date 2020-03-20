#include "stm32f10x.h"
#include "lib387.h"

//initialize clock (24MHz default)
void clockInit(void)
{
    //Initialize clocks
    RCC->CFGR = 0x00050002;     // PLLMUL X3, PREDIV1 is PLL input
    RCC->CR =  0x01010081;      // Turn on PLL, HSE, HSI
    while ((RCC->CR & RCC_CR_PLLRDY) != RCC_CR_PLLRDY)  // Wait for the PLL to stabilize
    {
    }
}

//enable buttons (PB8,9, PC12, PA5)
void butInit(void){
    //PB8,9, PC12, PA5 push buttons (left to right)
    RCC->APB2ENR |= RCC_APB2ENR_IOPBEN | RCC_APB2ENR_IOPAEN | RCC_APB2ENR_IOPCEN;
    //Config and mode bits SW2(PB8) default to Input, no change
}

//enable onboard leds and led strip
void ledInit (void)
{
    //Bit 4: Port C Bit3: Port B Bit 2: Port A
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;

    //Set the config and mode bits PA 9-12 for LEDs push-pull out (up to 50 MHz)
    GPIOA->CRH |= GPIO_CRH_MODE9 | GPIO_CRH_MODE10 | GPIO_CRH_MODE11 | GPIO_CRH_MODE12;
    GPIOA->CRH &= ~GPIO_CRH_CNF9 & ~GPIO_CRH_CNF10 & ~GPIO_CRH_CNF11 & ~GPIO_CRH_CNF12;
    GPIOA->ODR  |= (0xF<<LEDOFFSET);
}

void ledGBInit(void){
    RCC->APB2ENR |=  RCC_APB2ENR_IOPCEN;
    //PC8(blue), 9(green), onboard LEDs (output 50Hz 1100)
    GPIOC->CRH |= GPIO_CRH_MODE9 | GPIO_CRH_MODE8 ;
    GPIOC->CRH &= ~GPIO_CRH_CNF9 & ~GPIO_CRH_CNF8 ;
}

void ledGBSet(unsigned int val) {
    //set val to GREEN_ON, BLUE_OFF, etc.
    GPIOC->BSRR = val;
}

void ledGBTog(unsigned int val) {
    // Use BLUE_ON, GREEN_ON or GPIO_ODR_ODR8, GPIO_ODR_ODR8
    GPIOC->ODR ^= val;
}

//return bit pattern of buttons
uint8_t push_in(void)
{
    uint32_t sw_val;
    uint8_t trunc; 
    sw_val = ((GPIOB->IDR &(GPIO_IDR_IDR8 | GPIO_IDR_IDR9)) >> 8) 
        | ((GPIOC->IDR & GPIO_IDR_IDR12) >> 10)
        | ((GPIOA->IDR & GPIO_IDR_IDR15) >> 12) ;
    sw_val = ~sw_val & 0xF;
    trunc = sw_val;
    return trunc;
}


/**********************************************************************************************
* Function: ledSet
* Programmer: Irwin Chan
* Date: October 2019
* Purpose: Set LED strip according to passed in pattern
* Parameters: 0-OFF, 1-ON for respective light
*   (corrected order to be right to left)
* Return Value: NA
***********************************************************************************************/
void ledSet(unsigned int pattern){
    unsigned int out=0;
    unsigned int i=0;
    out = 0;
    for (i = 0; i < NUMLED; i++) 
    { 
        if((pattern& (0b1 << i))) 
        out |= 1 << ((NUMLED- 1) - i);   
    } 
    //Clear LEDs, set based on pattern
    GPIOA->ODR  |= (0xF<<LEDOFFSET);
    out = out << (16+LEDOFFSET);
    GPIOA->BSRR = out;
}

/**********************************************************************************************
* Function: ledOn
* Programmer: Irwin Chan
* Date: October 2019
* Purpose: Turn on num'th LED from right
* Parameters: num
* Return Value: NA
***********************************************************************************************/
void ledOn(unsigned int num){
    unsigned int position = num & 0xF;
    position = 1 << (NUMLED-position);
    position <<= (16+LEDOFFSET);	//indexed to set bit 9-12
    GPIOA->BSRR = position;
}

/**********************************************************************************************
* Function: ledOff
* Programmer: Irwin Chan
* Date: October 2019
* Purpose: Turn off num'th LED from right
* Parameters: num
* Return Value: NA
***********************************************************************************************/
void ledOff(unsigned int num){
    unsigned int position = num &0xF;
    position = 1 << (NUMLED-position);
    position = position << (LEDOFFSET); //indexed to reset bit 9-12
    GPIOA->BSRR = position;
}

/**********************************************************************************************
* Function: ledTog
* Programmer: Irwin Chan
* Date: October 2019
* Purpose: Toggle num'th LED from right
* Parameters: num
* Return Value: NA
***********************************************************************************************/
void ledTog(unsigned int num){
    unsigned int position = num &0xF;
    position = 1 << (NUMLED-position);
    position = position <<LEDOFFSET; //indexed for output bit 8-15
    GPIOA->ODR  ^= position;
}

/**********************************************************************************************
* Function: ledStat
* Programmer: Irwin Chan
* Date: October 2019
* Purpose: Return bit pattern corresponding to LED strip
*   0-OFF, 1-ON for respective light
*   (corrected order to be right to left)
* Parameters: NA
* Return Value: bit pattern
***********************************************************************************************/
unsigned int ledStat(void){
    unsigned int temp, i, out=0;
    temp=~(GPIOA->ODR >> LEDOFFSET);
    temp&=0xF;
    for (i = 0; i < NUMLED; i++) 
    { 
        if((temp& (0b1 << i))) 
        out |= 1 << ((NUMLED- 1) - i);   
    }
    return out;
}

/**********************************************************************************************
* Function: ledInv
* Programmer: Irwin Chan
* Date: October 2019
* Purpose: Toggle all LEDs to opposite state
* Parameters: NA
* Return Value: NA
***********************************************************************************************/
void ledInv(void){
    unsigned int position = 0xF << LEDOFFSET; //indexed for output bit 9-12
    GPIOA->ODR  ^= position;
}

//enable lcd
void lcdInit(void)
{
    uint8_t cmd = 0;
    //clocks
    RCC->APB2ENR |=  RCC_APB2ENR_IOPCEN | RCC_APB2ENR_IOPBEN;
    
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
                  GPIO_CRL_MODE4 | GPIO_CRL_MODE5	| GPIO_CRL_MODE6 | GPIO_CRL_MODE7;
    GPIOC->CRL &= ~GPIO_CRL_CNF0 & ~GPIO_CRL_CNF1 & ~GPIO_CRL_CNF2 & ~GPIO_CRL_CNF3 &
                  ~GPIO_CRL_CNF4 & ~GPIO_CRL_CNF5 & ~GPIO_CRL_CNF6 & ~GPIO_CRL_CNF7;
    
    lcd_out(cmd, 0x38);
    lcd_out(cmd, 0x38);
    lcd_out(cmd, 0x38);
    lcd_out(cmd, 0x38);
    lcd_out(cmd, 0x0F);
    lcd_out(cmd, 0x01);
    lcd_out(cmd, 0x06);
    lcd_out(cmd, 0x80);
}

// Output to LCD
void lcd_out(uint8_t cmd, uint8_t data)
{
    if (cmd==0)
        GPIOB->BSRR = LCD_CM_ENA; //RS low, E high
    else
        GPIOB->BSRR = LCD_DM_ENA; //RS high, E high
    // GPIOC->ODR = data; //BAD: may affect upper bits on port C
    GPIOC->ODR &= 0xFF00; //GOOD: clears the low bits without affecting high bits
    GPIOC->ODR |= data; //GOOD: only affects lowest 8 bits of Port C
    for(int i=0; i< 8000; ++i);
    
    if (cmd==0)
        GPIOB->BSRR = LCD_CM_DIS; //RS low, E low
    else
        GPIOB->BSRR = LCD_DM_DIS; //RS high, E low
    for(int i=0; i< 8000; ++i);
}

//enable sistick. Count from reload value to 0, wrap
void sysTickInit(uint32_t ticks)
{
    //24e6 ticks == 1s
    SysTick->CTRL = 0x00;   //disable
    SysTick->VAL = 0x00;    //reset counter
    SysTick->LOAD = ticks;  //set load value
    SysTick->CTRL = 0x03;   //use processor clock (AHB), enable interrupt, enable counter
    
    RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;
	AFIO->EXTICR[0] &= ~AFIO_EXTICR1_EXTI0;
	EXTI->IMR |= EXTI_IMR_MR0;
	EXTI->FTSR |= EXTI_FTSR_TR0;
	NVIC->ISER[0] |= NVIC_ISER_SETENA_6;
}

//disable sistick
void sysTickDis(void){
    NVIC->ICER[0] |= NVIC_ICER_CLRENA_6;
    SysTick->CTRL = 0x00;   //disable
}


void delay(uint32_t count)
{
    int i=0;
    for(i=0; i< count; ++i)
    {
    }
}

