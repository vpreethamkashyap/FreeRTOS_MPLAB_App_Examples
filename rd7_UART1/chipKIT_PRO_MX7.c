/*************************** chipKIT_PRO_MX7 *******************************
**
**  Author:     Richard Wall
**  Date:       August 15, 2013
**  Rev.	February 2, 2014
**
**  Configures the chipKIT PRO MX7 processor board for:
**      Sets LED1 through LED4 for output
**      Sets BTN1, BTN2 and BTN3 for input
**
** **************************************************************************/

#include <plib.h>               /* PIC32 System header file */
/* The config_bits.h must be included once and only once in any project */
#include "config_bits.h"        /* Processor configuration definitions */
#include "chipKIT_PRO_MX7.h"    /* chipKIT PRO MX7  pin definitions */

/* chipKIT_PRO_MX7_Setup FUNCTION DESCRIPTION ********************************
 * SYNTAX:          void chipKIT_PRO_MX7_Setup(void);
 * KEYWORDS:        chipKIT PRO MX7, PIC32, setup
 * Parameters:      None
 * Return:          None
 * Notes:           None
 * END DESCRIPTION ********************************************************/
void chipKIT_PRO_MX7_Setup(void)
{
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Statement configure cache, wait states and peripheral bus clock
 * Configure the device for maximum performance but do not change the PBDIV
 * Given the options, this function will change the flash wait states, RAM
 * wait state and enable prefetch cache but will not change the PBDIV.
 * The PBDIV value is already set via the pragma FPBDIV option above..
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    SYSTEMConfig(GetSystemClock(), SYS_CFG_WAIT_STATES | SYS_CFG_PCACHE);

    DDPCONbits.JTAGEN = 0;  // Statement is required to use Pin RA0 as IO
    PORTSetPinsDigitalIn(IOPORT_A, BTN3);	 /* Set BTN3 as input */

    PORTSetPinsDigitalIn(IOPORT_G, BTN1 | BTN2); /* Set BTN1 & BTN2 as inputs */
    PORTSetPinsDigitalOut(IOPORT_G, BRD_LEDS); 	/* Set BDR LEDs as output */
    PORTClearBits(IOPORT_G, BRD_LEDS);         	/* Turn off LED1 through LED4 */
} /* End of chipKIT_PRO_MX7_Setup */
/* End of chipKIT_PRO_MX7.c */
