/* *********** Reference Design #8 Serial I/O with Timers *******************
 * Author:      Richard Wall
 * Date:        September 28, 2011
 * Revised:     October 4, 2013 - Updated comments and partition tasks to
 *              sundry files.
 *              February 25, 2014 - MPLAB X 2.0 and XC32 1.31.  chipKIT_Pro_MX7
 *              October 2, 2014 - editorial corrections, added notes for using
 *                                  timers
 *
 * Description: An example of FreeRTOS running on a chipKIT Pro MX7 using the
 *              PIC32MX7 processor. This reference design uses the same serial
 *              code as RD7 for the serial communications. A timer is started
 *              that starts a task at a specific interval. The tick
 *              count is reported to the serial terminal each second.
 *              Timers.c and Timers.h must be added to this project. The bit
 *              to use timers must be set in FreeRTOSConfig.h as shown here.
 *
 *              #define configUSE_TIMERS    1 //  If timers are used
 *
 *              Two tasks are flashing different LEDs at the same rate. The
 *              rate that prvReportTicks flashes LED1 is controlled by the
 *              FreeRTOS timer API. The rate that prvTestTask2 flashes LED2
 *              is controlled by  a vTaskDelay task.  Eventually, LED1 and LED2
 *              would not be synchronized if the two tasks ran at different
 *              rates.
 *
 *              What's the difference:
 *                  vTaskDelay slows down a task or function.
 * 
 *                  Timers initiate a function or a task that usually completes
 *                  within the timer interval.
 *
 *              Change FreeRTOSConfig.h to - #define configUSE_TIMERS 1
 * 
 ***************************************************************************/

/* XC32 System Files */
#include <plib.h>
#include <stdio.h>

/* Files to support FreeRTOS */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"

/* Files to support Application  */
#include "chipKIT_PRO_MX7.h"
#include "FreeRTOS_Common_Tasks.h"
#include "serial.h"

/* UART priority  RX is one higher than Tx */
#define UART_PRIORITY	( tskIDLE_PRIORITY + 2 )
/* Baud rate used by the comtest tasks.  */
#define UART_BAUD_RATE	( 19200 )

unsigned long ulIdleCycleCount = 0UL;   /* Used by idle hook */

static xTimerHandle T1Handle;   /* Timer control handle */

/* Tasks for scheduler to blink LEDs */
static void prvReportTicks(  xTimerHandle T1Handle );
static void prvTestTask2( void *pvParameters );

/* main Function Description ***************************************
 * SYNTAX:          int main( void );
 * KEYWORDS:        Initialize, create, tasks, scheduler
 * DESCRIPTION:     This is a typical RTOS setup function. Hardware is
 *                  initialized, tasks are created, and the scheduler is
 *                  started.
 * PARAMETERS:      None
 * RETURN VALUE:    Exit code - used for error handling
 * NOTES:           None
 * END DESCRIPTION *****************************************************/
int main( void )
{
unsigned long ulStatus1 = pdPASS; /* Sticky bit reset if did not create task */
long timer_id[1] = {100};         /* Timer ID */

/* Configure any hardware required for this demo. */
    vSetupHardware();

/* Start UART task for reporting time */
    ulStatus1 &= uCreateUARTTasks( UART_PRIORITY, UART_BAUD_RATE );

/* This creates a 1000 ms timer that the scheduler calls the function
 * prvReportTicks to generate periodic execution of this task.  */

    T1Handle = xTimerCreate((const signed char *) "TIM1", (portTickType) 1000,
		(unsigned portBASE_TYPE) pdTRUE, timer_id, prvReportTicks);

/* Create the tasks defined within this file. */
    ulStatus1 &= xTaskCreate( prvTestTask2, (signed char *) "Tst2",
              configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY, NULL );

/* Finally start the scheduler. */
    xTimerStart(T1Handle, 0);	/* Start the time based tasks */

    if(ulStatus1 == pdPASS)
    {
	vTaskStartScheduler();
    }

/* Will only reach here if there is insufficient heap available to start
 * the scheduler. */
    return 1;
} /* End of main */

/* prvReportTicks Function Description **********************************
 * SYNTAX:          static void prvReportTicks( void );
 * KEYWORDS:        LED, toggle, RTOS ticks
 * DESCRIPTION:     Callback function to report the Tick counter
 * PARAMETER 1:     pointer to data passed from scheduler
 * RETURN VALUE:    None
 * NOTES:           This is not a task in a conventional sense.
 *                  This function contains local auto variables and runs from
 *                  start to completion. This is a function that is called by
 *                  an OS task which in turn sends a message to another OS
 *                  task.
 * END DESCRIPTION *****************************************************/
static void prvReportTicks( xTimerHandle T1Handle )
{
char TxMsg[40];
unsigned long tick;

    PORTToggleBits(IOPORT_G, LED1);     /* Toggle LED1 on chipKIT board */
    tick = xTaskGetTickCount();
    sprintf(TxMsg,"vRegTest1 %ld \n\r", tick);
    xQueueSend( xLineForTx, TxMsg, 0);
} /* End of prvReportTicks */

/* prvTestTask2 Function Description ***************************************
 * SYNTAX:          static void prvTestTask2( void *pvParameters );
 * KEYWORDS:        LED, toggle, dummy
 * DESCRIPTION:     Dummy task for consuming idle time
 * PARAMETER 1:     pointer to data passed from scheduler
 * RETURN VALUE:    None
 * NOTES:           None
 * END DESCRIPTION *****************************************************/
static void prvTestTask2( void *pvParameters )
{
extern void vRegTest2( unsigned long * );

    for( ;; )
    {
        vTaskDelay( 250 / portTICK_RATE_MS ); /* Delay 1/4 second */
        PORTToggleBits(IOPORT_G, LED2); /* Toggle LED2 on chipKIT board */
    }
} /* End of prvTestTask2 */

/*--------------------------End of main  RD8 Serial Timers ------------------*/

