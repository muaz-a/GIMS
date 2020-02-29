/* Initialize prototypes */
/* Author - Brett Aulie */


#include "init_lib.h" 
 
// CLOCK ANd TIMING FUNCTIONS

/*
* Name: void clockInit ()
* Paramaters : none
* Description: This function will initialize the device internal
*								clock to 24 Mhz
*/
void clockInit(void)
{
	uint32_t temp = 0x00;
	
	RCC->CFGR = 0x0005002;		// Output PLL/2 as MCO,
														// PLLMUL X3, PREDIV1 is PLL input
	
	
    RCC->CR =  0x01010081;      // Turn on PLL, HSE, HSI

    while (temp != 0x02000000)  // Wait for the PLL to stabilize
    {
        temp = RCC->CR & 0x02000000; //Check to see if the PLL lock bit is set
    }


}		

		

		

		
		
		
