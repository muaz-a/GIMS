/* Interrupts */


 #include "init_interrupts.h"

void systick_init(void)
{
	SysTick->CTRL=0x00;
	SysTick->VAL=0x08;
	SysTick->LOAD=24000000;
	SysTick->CTRL=0x07;
}

void exti0_init(void)
{
	RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;
	AFIO->EXTICR[0] &= AFIO_EXTICR1_EXTI0;
	EXTI->IMR |= EXTI_IMR_MR0;
	EXTI -> FTSR |= EXTI_FTSR_TR0;
	NVIC->ISER[0] |= NVIC_ISER_SETENA_6;
}
