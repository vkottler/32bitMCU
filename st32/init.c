#include "init.h"

int initialize(void) {

	/* clear PLL_ON, PLLIS2_ON, PLLSAI_ON, HSE_ON */
	RCC->CR &= 0xEAFEFFFF;
	
	/* wait for those to be unlocked */
	while (RCC->CR & (0x2A01 << 16)) {;}
	
	/* configure PLLs */
	RCC->PLLCFGR = 0x24003010;	/* set PLL bits to default	*/
	RCC->PLLCFGR |= PLLN << 6;	/* set PLLN					*/
	RCC->PLLCFGR |= PLLM << 0;	/* set PLLM					*/
	RCC->PLLCFGR |= PLLP << 16;	/* set PLLP					*/
	RCC->PLLCFGR |= PLLQ << 24;	/* set PLLQ					*/
	RCC->PLLCFGR |= PLLR << 28;	/* set PLLR 				*/
	RCC->PLLCFGR |= 0x01 << 22;	/* set HSE as PLL clock src	*/
		
	/* set microcontroller clock outputs */
	RCC->CFGR =  0x0;		/* make sure it is in reset state							*/
	RCC->CFGR |= 0x0 << 30;	/* MCO2 set to SYSCLK										*/
	RCC->CFGR |= 0x7 << 27; /* MCO2 prescalar set to 5									*/
	RCC->CFGR |= 0x3 << 21; /* MCO1 set to PLL											*/
	RCC->CFGR |= 0x7 << 24; /* MCO1 prescalar set to 5									*/
	RCC->CFGR |= 0x8 << 16;	/* RTC prescalar set to 8									*/
	RCC->CFGR |= 0x4 << 13; /* APB2 (high speed, can't exceed 90 MHz) divided by 2		*/
	RCC->CFGR |= 0x5 << 10; /* APB1 (low speed, can't exceed 45 MHz) clock divided by 4	*/
	RCC->CFGR |= 0x0 <<  4; /* AHB prescalar set to 0 									*/
		
	/* enable high-speed external oscillator */
	RCC->CR |= 0x1 << 18;					/* set HSE_BYP, nucleo 144 uses 8 MHz signal from onboard ST-Link	*/
	RCC->CR |= 0x1 << 16;					/* set HSE_ON to enable this clock source							*/
	while (!(RCC->CR & (0x1 << 17))) {;}	/* poll HSE_RDY														*/
		
	/* enable PLL */
	RCC->CR |= 0x1 << 24;					/* set PLL_ON	*/
	while (!(RCC->CR & (0x1 << 24))) {;}	/* poll PLL_RDY	*/
		
	/* switch to PLL as system clock */
	RCC->CFGR |= 0x2;						/* request switch to PLL		*/
	while (!(RCC->CFGR & 0x8)) {;}			/* wait for PLL to be source	*/
		
	/* update core clock variable */
	SystemCoreClockUpdate();
	
	return 0;
}
