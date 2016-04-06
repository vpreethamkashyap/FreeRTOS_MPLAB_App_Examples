/*  FreeRtos Reference Design #1
 * Project:     RD1
 * File name:   RD1.c
 * Author:      Richard Wall
 * Date:        August 26, 2013
 * Revised:     September 14, 2013 - FreeRTOS 7.5.2
 *              September 22, 2013 - Editorial updates
 *
 * Description: An example of FreeRTOS running on an the
 *              chipKIT_Pro_MX7 running the PIC32MX7 processor.
 *              The operating system creates manages two
 *              tasks that turn on an LED and increments a
 *              counter.
 *
 * Observable:  Connecting a logic analyzer or oscilloscope to the PmodSTEP
 *              test points for LEDA and LEDB result in square waves 180 
 *              degrees phase from each other. This indicates that each task is
 *              allotted one timer tick to execute. Since both tasks use all of
 *              their allotted task time, the scheduler implements a fair and
 *              equal distribution of operating time. No idle task is scheduled.
*******************************************************************************/

/* Standard includes. */
#include <plib.h>
#include <stdio.h>

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

/* Hardware dependent setting */
#include "chipKIT_Pro_MX7.h"

/* Set up the processor for the example program. */
static void prvSetupHardware( void );

/* Simple Tasks that light a specific LED when running  */
static void prvTestTask1( void *pvParameters );
static void prvTestTask2( void *pvParameters );

/* main Function Description ***************************************
 * SYNTAX:		int main( void );
 * KEYWORDS:		Initialize, create, tasks, scheduler
 * DESCRIPTION:         This is a typical RTOS set up function. Hardware is
 * 			initialized, tasks are created, and the scheduler is
 * 			started.
 * PARAMETERS:		None
 * RETURN VALUE:	Exit code - used for error handling
 * NOTES:		All three buttons are polled using the same code
 *                      for reading the buttons.
 * END DESCRIPTION *****************************************************/
int main( void )
{
    prvSetupHardware();		/*  Configure hardware */

/* Create the tasks then start the scheduler. */

    /* Create the tasks defined within this file. */
    xTaskCreate( prvTestTask1, "Tst1", configMINIMAL_STACK_SIZE,
                                    NULL, tskIDLE_PRIORITY, NULL );
    xTaskCreate( prvTestTask2, "Tst2", configMINIMAL_STACK_SIZE,
                                    NULL, tskIDLE_PRIORITY, NULL );

    vTaskStartScheduler();	/*  Finally start the scheduler. */

/* Will only reach here if there is insufficient heap available to start
 *  the scheduler. */
    return 0;
}  /* End of main */

/* prvTestTask1 Function Description *****************************************
 * SYNTAX:          static void prvTestTask1( void *pvParameters );
 * KEYWORDS:        RTOS, Task
 * DESCRIPTION:     If LEDA is not lit, all LEDs are turned off and LEDA is
 *                  turned on. Increments a counter each time the task is
 *                  resumed.
 * PARAMETER 1:     void pointer - data of unspecified data type sent from
 *                  RTOS scheduler
 * RETURN VALUE:    None (There is no returning from this function)
 * NOTES:           LEDA is switched on and LEDB switched off if LEDA was
 *                  detected as off.
 * END DESCRIPTION ************************************************************/
static void prvTestTask1( void *pvParameters )
{
unsigned int counter = 0;

    for( ;; )
    {
	if(!(LATB & LEDA))      /* Test for LEDA off */
	{
            LATBCLR = SM_LEDS;  /* Turn off all other LEDs */
            LATBSET = LEDA;     /* Turn LEDA on */
            ++counter;          /* Increment task run counter */
	}
    }
}  /* End of prvTestTask1 */

/* prvTestTask2 Function Description *****************************************
 * SYNTAX:          static void prvTestTask2( void *pvParameters );
 * KEYWORDS:        RTOS, Task
 * DESCRIPTION:     If LEDB is not lit, all LEDs are turned off and LEDB is
 *                  turned on. Increments a counter each time the task is
 *                  resumed.
 * PARAMETER 1:     void pointer - data of unspecified data type sent from
 *                  RTOS scheduler
 * RETURN VALUE:    None (There is no returning from this function)
 * NOTES:           LEDB is switched on and LEDA switched off if LEDB was
 *                  detected as off.
 * END DESCRIPTION ************************************************************/
static void prvTestTask2( void *pvParameters )
{
unsigned int counter = 0;
    for( ;; )
    {
	if(!(LATB & LEDB))      /* Test for LEDB off */
	{
            LATBCLR = SM_LEDS;  /* Turn off all other LEDs */
            LATBSET = LEDB;     /* Turn LEDB on */
            ++counter;          /* Increment task run counter */
	}
    }
}  /* End of prvTestTask2 */

/* prvSetupHardware Function Description *************************************
 * SYNTAX:	static void prvSetupHardware( void );
 * KEYWORDS:	Hardware, initialize, configure, setup
 * DESCRIPTION: Initializes hardware specific resources
 * PARAMETERS:	None
 * RETURN VALUE: None
 * NOTES:	Static function - can be called exclusively from 
 * 		within this program.
 * END DESCRIPTION ************************************************************/
static void prvSetupHardware( void )
{
    chipKIT_PRO_MX7_Setup();

/* Set up PmodSTEM LEDs */
    PORTSetPinsDigitalOut(IOPORT_B, SM_LEDS);
    LATBCLR = SM_LEDS;                      /* Clear all SM LED bits */
    LATBSET = LEDA;                         /* Turn on LEDA */
    
/* Enable multi-vector interrupts */
    INTConfigureSystem(INT_SYSTEM_CONFIG_MULT_VECTOR);  /* Do only once */
    INTEnableInterrupts();   /*Do as needed for global interrupt control */
    portDISABLE_INTERRUPTS();
} /* End of prvSetupHardware */

/* vApplicationStackOver Function Description ********************************
 * SYNTAX:          void vApplicationStackOverflowHook( void );
 * KEYWORDS:        Stack, overflow
 * DESCRIPTION:     Look at pxCurrentTCB to see which task overflowed
 *                  its stack.
 * PARAMETERS:      None
 * RETURN VALUE:    None
 * NOTES:           See FreeRTOS documentation
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
 * NOTES:           Program will be vectored to here if the any CPU error is
 *                  generated. See FreeRTOS documentation for error codes.
END DESCRIPTION ************************************************************/
void _general_exception_handler( unsigned long ulCause, unsigned long ulStatus )
{
    for( ;; );
} /* End of _general_exception_handler */
/*--------------------------End of main  -----------------------------------*/

