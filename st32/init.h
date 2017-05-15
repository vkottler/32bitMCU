#ifndef _INIT__H__

#include <stdint.h>
#include "stm32f7xx.h"

/** System Clock source            = PLL (HSE w/ HSE_BYP, 8 MHz)
  * SYSCLK(Hz)                     = 160000000 (160 MHz)
  * HCLK(Hz)                       = 160000000 (160 MHz)
  * AHB, APB1, APB2 Prescalars     = 1, 4, 2
  *
  * Constraints:
  * PLLN[8:0] - 50 <= PLLN <= 432 (f_VCO must be between 100-432 MHz)
  * PLLM[5:0] - 2 <= PLLM <= 63 (f_PLL_in must be 1-2 MHz)
  * PLLP[1:0] - do not exceed 180 MHz (0 = 2, 1 = 4, 2 = 6, 3 = 8)
  * PLLQ[3:0] - 2 <= PLLQ <= 15 goal is to create 48 MHz (USB, SDMMC RNG clock src)
  * PLLR[2:0] - 2 <= PLLR <= 7 DSI clock src 
  */
#define PLLN	160	/* f_VCO = f_PLL_in * (PLLN / PLLM) */
#define PLLM 	4	/* 320 MHz = 8 MHz * (320 / 8)		*/
#define PLLP	0	/* f_PLL_out = f_VCO / PLLP	(2)		*/
#define PLLQ	10	/* f_USB_SDMMC_RNG = f_VCO / PLLQ	*/
#define PLLR	5	/* f_PLL_DSI_out = f_VCO / PLLR		*/

extern uint32_t SystemCoreClock;
extern void SystemCoreClockUpdate(void);

int initialize(void);

#endif
