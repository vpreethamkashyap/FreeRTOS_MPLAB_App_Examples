/*  Reference design #Serial I/O
 * Project:     RD 7
 * File name:   main.c
 * Author:      Richard Wall
 * Date:        September 28, 2011
 * Revised:     September 23, 2013
 *              October 2, 2014 - editorial corrections and formatting
 *
 * Description: An example of FreeRTOS running on an chipKIT Pro MX7 using a
 *              PIC32MX7 processor. This reference design demonstrates how to
 *              implement UART line base IO.  One task reads a character at a
 *              time and fills a buffer until a FL or CR character is detected
 *              or the buffer has reached its size limit. The message is then
 *              sent to a task that sends the text string back to the UART. RX
 *              and TX interrupts are used to manage character based serial
 *              communications.  The serial communications uses UART 1 at
 *              19200 BAUD N81. A sign of text message is sent to the terminal.
 *
 *              You will not see anything on the terminal screen while entering
 *              text until you press the enter key unless you have the terminal
 *              setup for local echo characters. Lines of text echoed back to
 *              the terminal have CR, and LF appended.
 *
*/

/* XC32 System Files */
#include <plib.h>
#include <stdio.h>

/* Files to support FreeRTOS */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

/* Files to support Application  */
#include "chipKIT_PRO_MX7.h"
#include "FreeRTOS_Common_Tasks.h"
#include "serial.h"

/*-----------------------------------------------------------*/
/* UART priority  RX is one higher than Tx */
#define UART_PRIORITY			( tskIDLE_PRIORITY + 2 )

/* Baud rate used by the comtest tasks.  */
#define UART_BAUD_RATE			( 19200 )

/* Dummy tasks for scheduler to blink LEDs */
static void prvTestTask1( void *pvParameters );
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
portBASE_TYPE ulStatus1 = pdPASS; // Sticky bit reset if did not create task

/* Configure any hardware required for this demo. */
    vSetupHardware();

/* Start UART task - this initializes the UART resource and creates the
 * the semaphores to send and received data - See serial.h */
    ulStatus1 &= uCreateUARTTasks( UART_PRIORITY, UART_BAUD_RATE );

/* Create the tasks defined within this file which have no real function in
 * this example design. */
    ulStatus1 &= xTaskCreate( prvTestTask1, (signed char *) "Tst1",
                    configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY, NULL );

    ulStatus1 &= xTaskCreate( prvTestTask2, (signed char *) "Tst2",
                    configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY, NULL );

/* Finally start the scheduler. */
    if(ulStatus1 == pdPASS)
    {
        vTaskStartScheduler();
    }

/* Will only reach here if there is insufficient heap available to start
 * the scheduler. */
    return 1;
} /* End of main */

/* prvTestTask1 Function Description ***************************************
 * SYNTAX:          static void prvTestTask1( void *pvParameters );
 * KEYWORDS:        LED, toggle, dummy
 * DESCRIPTION:     Dummy task for consuming idle time
 * PARAMETER 1:     pointer to data passed from scheduler
 * RETURN VALUE:    None
 * NOTES:           None
 * END DESCRIPTION *****************************************************/
static void prvTestTask1( void *pvParameters )
{
    for( ;; )
    {
        vTaskDelay( 250 / portTICK_RATE_MS );   /* Delay 1/4 second */
        PORTToggleBits(IOPORT_B, LEDA);         /* Toggle led */
    }
} /* End of prvTestTask1 */

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
    for( ;; )
    {
        vTaskDelay( 250 / portTICK_RATE_MS );   /* Delay 1/4 second */
        PORTToggleBits(IOPORT_B, LEDB);         /* Toggle led */
    }
} /* End of prvTestTask2 */

/*------------------ End of main for RD7 --------------------------*/
