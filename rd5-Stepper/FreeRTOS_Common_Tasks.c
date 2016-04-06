/* ******************* Common FreeRTOS tasks  *****************************
 * Project:         Various
 * File name:       FreeRTOS_Common_Tasks.c
 * Author:          Richard Wall
 * Date:            September 9, 2013
 *
 * Description:     These tasks are commonly used by the applications used in
 *                  the series of reference design.
 *                  The four common tasks are as follows:
 *                  prvSetupHardware - initializes application to run on the
 *                      Cerebot MX7cK processor board
 *
 *                  vApplicationIdleHook - called whenever the scheduler is
 *                      is an idle state (waiting for a task to become ready)
 *
 *                  vApplicationStackOverflowHook - Scheduler attempts to create
 *                      a task but runs out of stack memory
 *
 *                  _general_exception_handler - Error that causes a processor
 *                      fault
 * 
 ****************************************************************************/

#include <plib.h>

/* Scheduler includes.  */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

/* Hardware dependent settings */
#include "chipKIT_Pro_MX7.h"
#include "FreeRTOS_Common_Tasks.h"

void vSetupHardware( void )
{
/*---------------------------------------------------------
 *  	The following lines of code are required regardless of what
 * 	other hardware setup is necessary.
 * ---------------------------------------------------------*/
    chipKIT_PRO_MX7_Setup();

    TRISBCLR = SM_LEDS;     /* Setup for PmodSTEP LED outputs */
    LATBCLR = SM_LEDS;      /* Turn off task LEDs */

/* Enable multi-vector interrupts (Method suggested for XC32 */
    INTConfigureSystem(INT_SYSTEM_CONFIG_MULT_VECTOR);  /* Do only onec */
    INTEnableInterrupts();   /*Do as needed for global interrupt control */

    portDISABLE_INTERRUPTS();
} /* End of prvSetupHardware */

/* vApplicationIdleHook Function Description ********************************
 * SYNTAX:          void vApplicationIdleHook( void );
 * KEYWORDS:        schedule, idle,
 * DESCRIPTION:     Executes when scheduler is idle
 * PARAMETERS:      None
 * RETURN VALUE:    None
 * NOTES:           #define configUSE_IDLE_HOOK 1 must be set in
 *                  FreeRTOSConfig.h
 * END DESCRIPTION ************************************************************/
void vApplicationIdleHook( void )
{
static unsigned long ulIdleCycleCount = 0UL;
    ulIdleCycleCount++;     /* System declared global variable */
    LATBSET = LEDD;         /* For timing instrumentation only */
}

/* vApplicationStackOver Function Description ********************************
 * SYNTAX:                  void vApplicationStackOverflowHook( void );
 * KEYWORDS:                Stack, overflow
 * DESCRIPTION:             Look at pxCurrentTCB to see which task overflowed
 *                          its stack.
 * PARAMETERS:              None
 * RETURN VALUE:            None
 * NOTES:                   See FreeRTOS documentation
 * END DESCRIPTION **********************************************************/
void vApplicationStackOverflowHook( void )
{
    for( ;; );
} /* End of vApplicationStackOver */

/* _general_exception_handler Function Description ****************************
 * SYNTAX:          void _general_exception_handler( unsigned long ulCause,
 *                                              unsigned long ulStatus );
 * KEYWORDS:        Exception, handler
 * DESCRIPTION:     This overrides the definition provided by the kernel.
 *                  Other exceptions should be handled here.
 * PARAMETER 1:     unsigned long - Cause of exception code
 * PARAMETER 2:     unsigned long - status of process
 * RETURN VALUE:    None
 * NOTES:           Proggram will be vectored to here if the any CPU error is
 *                  generated. See FreeRTOS documentation for error codes.
END DESCRIPTION ************************************************************/
void _general_exception_handler( unsigned long ulCause, unsigned long ulStatus )
{
    for( ;; );
} /* End of _general_exception_handler */

/* End of FreeRETOS_Commmon_Tasks.c */
