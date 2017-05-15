#ifndef _GPIO__H__

/* 11 GPIO ports (A -> K)
 *
 * Defaults:
 *
 * A15: pulled up						JTDI
 * A14: pulled down						JTCK-SWCLK
 * A13: pulled up		very high speed	JTMS-SWDIO
 *  B4: pulled up						NJTRST
 *  B3: 				very high speed	JTDO/T RACES WO
 *
 */

#define USER_SW	PC13	/* can be configured for PA0 */
#define LED1	PB0		/* can be configured for PA5 */
#define LED2	PB7		/* */
#define LED3	PB14	/* */

#endif
