/******************************************************************************
 * CMSIS Cortex-M7 Device Peripheral Access Layer System Source File.
 *
 *   This file provides two functions and one global variable to be called from 
 *   user application:
 *      - SystemInit(): This function is called at startup just after reset and 
 *                      before branch to main program. This call is made inside
 *                      the "startup_stm32f7xx.s" file.
 *
 *      - SystemCoreClock variable: Contains the core clock (HCLK), it can be used
 *                                  by the user application to setup the SysTick 
 *                                  timer or configure other parameters.
 *                                     
 *      - SystemCoreClockUpdate(): Updates the variable SystemCoreClock and must
 *                                 be called whenever the core clock is changed
 *                                 during program execution.
 *****************************************************************************/

#include "stm32f7xx.h"

/* Default value of the External oscillator in Hz */
#ifndef	HSE_VALUE
#define HSE_VALUE	((uint32_t)8000000) 
#endif

/* Value of the Internal oscillator in Hz */
#ifndef	HSI_VALUE
#define HSI_VALUE	((uint32_t)16000000) 
#endif

/************************* Miscellaneous Configuration ************************/
/* Uncomment the following line if you need to relocate your vector Table in
   Internal SRAM. */
/* #define VECT_TAB_SRAM */
#define VECT_TAB_OFFSET  0x00 /* Vector Table base offset, multiple of 0x200. */
/******************************************************************************/

/* This variable is updated in three ways:
	1) by calling CMSIS function SystemCoreClockUpdate()
	2) by calling HAL API function HAL_RCC_GetHCLKFreq()
	3) each time HAL_RCC_ClockConfig() is called to configure the system clock frequency 
		Note: If you use this function to configure the system clock; then there
			  is no need to call the 2 first functions listed above, since SystemCoreClock
			  variable is updated automatically.
*/
uint32_t SystemCoreClock = 16000000;
const uint8_t AHBPrescTable[16] = {0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 3, 4, 6, 7, 8, 9};
const uint8_t APBPrescTable[8] = {0, 0, 0, 0, 1, 2, 3, 4};

/*
 * Setup the microcontroller system
 * Initialize the Embedded Flash Interface, the PLL and update the 
 * SystemFrequency variable.
 */
void SystemInit(void) {
	
	/* FPU settings ------------------------------------------------------------*/
#if (__FPU_PRESENT == 1) && (__FPU_USED == 1)
	SCB->CPACR |= ((3UL << 10*2)|(3UL << 11*2));  /* set CP10 and CP11 Full Access */
#endif
	
	/* Reset the RCC clock configuration to the default reset state ------------*/
	RCC->CR |= (uint32_t)0x00000001;	/* Set HSION bit */
	RCC->CFGR = 0x00000000;				/* Reset CFGR register */
	RCC->CR &= (uint32_t)0xFEF6FFFF;	/* Reset HSEON, CSSON and PLLON bits */
	RCC->PLLCFGR = 0x24003010;			/* Reset PLLCFGR register */
	RCC->CR &= (uint32_t)0xFFFBFFFF;	/* Reset HSEBYP bit */
	RCC->CIR = 0x00000000;				/* Disable all interrupts */

	/* Configure the Vector Table location add offset address ------------------*/
#ifdef VECT_TAB_SRAM
	SCB->VTOR = RAMDTCM_BASE | VECT_TAB_OFFSET;	/* Vector Table Relocation in Internal SRAM */
#else
	SCB->VTOR = FLASH_BASE | VECT_TAB_OFFSET;		/* Vector Table Relocation in Internal FLASH */
#endif
}

/**
  * @brief  Update SystemCoreClock variable according to Clock Register Values.
  *         The SystemCoreClock variable contains the core clock (HCLK), it can
  *         be used by the user application to setup the SysTick timer or configure
  *         other parameters.
  *           
  * @note   Each time the core clock (HCLK) changes, this function must be called
  *         to update SystemCoreClock variable value. Otherwise, any configuration
  *         based on this variable will be incorrect.         
  *     
  * @note   - The system frequency computed by this function is not the real 
  *           frequency in the chip. It is calculated based on the predefined 
  *           constant and the selected clock source:
  *             
  *           - If SYSCLK source is HSI, SystemCoreClock will contain the HSI_VALUE(*)
  *                                              
  *           - If SYSCLK source is HSE, SystemCoreClock will contain the HSE_VALUE(**)
  *                          
  *           - If SYSCLK source is PLL, SystemCoreClock will contain the HSE_VALUE(**) 
  *             or HSI_VALUE(*) multiplied/divided by the PLL factors.
  *         
  *         (*) HSI_VALUE is a constant defined in stm32f7xx_hal_conf.h file (default value
  *             16 MHz) but the real value may vary depending on the variations
  *             in voltage and temperature.   
  *    
  *         (**) HSE_VALUE is a constant defined in stm32f7xx_hal_conf.h file (default value
  *              25 MHz), user has to ensure that HSE_VALUE is same as the real
  *              frequency of the crystal used. Otherwise, this function may
  *              have wrong result.
  *                
  *         - The result of this function could be not correct when using fractional
  *           value for HSE crystal.
  */
void SystemCoreClockUpdate(void) {
	
	uint32_t tmp = 0, pllvco = 0, pllp = 2, pllsource = 0, pllm = 2;
  
	/* Get SYSCLK source -------------------------------------------------------*/
	tmp = RCC->CFGR & RCC_CFGR_SWS;

	switch (tmp) {
		
		/* HSI used as system clock source */
		case 0x00: SystemCoreClock = HSI_VALUE; break;
		
		/* HSE used as system clock source */
		case 0x04: SystemCoreClock = HSE_VALUE; break;
		
		/* PLL used as system clock source */
		case 0x08:  

			/* PLL_VCO = (HSE_VALUE or HSI_VALUE / PLL_M) * PLL_N
			   SYSCLK = PLL_VCO / PLL_P */    
			pllsource = (RCC->PLLCFGR & RCC_PLLCFGR_PLLSRC) >> 22;
			pllm = RCC->PLLCFGR & RCC_PLLCFGR_PLLM;
			
			/* HSE used as PLL clock source */
			if (pllsource != 0)
				pllvco = (HSE_VALUE / pllm) *
				((RCC->PLLCFGR & RCC_PLLCFGR_PLLN) >> 6);
      
			/* HSI used as PLL clock source */
			else
				pllvco = (HSI_VALUE / pllm) * 
				((RCC->PLLCFGR & RCC_PLLCFGR_PLLN) >> 6);      
			
			pllp = (((RCC->PLLCFGR & RCC_PLLCFGR_PLLP) >>16) + 1 ) *2;
			SystemCoreClock = pllvco/pllp;
			break;
		
		default: SystemCoreClock = HSI_VALUE; break;
	}
	
	/* Compute HCLK frequency --------------------------------------------------*/
	/* Get HCLK prescaler */
	tmp = AHBPrescTable[((RCC->CFGR & RCC_CFGR_HPRE) >> 4)];
	/* HCLK frequency */
	SystemCoreClock >>= tmp;
}
