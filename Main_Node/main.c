#include "main.h"

int main(void)
{
	clockInit();
	init_usart();
	initLCD();
	led_IO_init();
	tim4Init();
	StartUp();

	
	
}

