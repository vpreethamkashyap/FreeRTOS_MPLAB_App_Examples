/*  ******** Reference design #6 LCD Pong Ping with Mutes Semaphores *********
 * Author:      Richard Wall
 * Date:        September 29, 2011
 * Revised:	December 22, 2012
 * Revised:     September 25, 2013
 * 
 * Description: An example of FreeRTOS running on an chipKIT Pro MX7
 *              board with a PIC32MX7 processor. The hardware required for
 *              this project is the Digilent chipKIT Pro MX7 and the PmodCLP
 *              peripheral module.
 *
 *              This example passes a counter value between two tasks.
 *              each task increments the counter before passing it
 *              back. Each task sends a message to the LCD that is
 *              protected by a mutex semaphore.  Commenting out
 *              the semaphore take and give demonstrates how the LCD
 *              text gets messed up without them. (See Line 122 & 161 )
 *
 * Note:        Writing to the LCD is not setup as a task but is a resource
 *              that requires protected use. This is an example on using an
 *              device driver.
 ****************************************************************************/

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
#include "lcddrv.h"

/* The priorities of the various application tasks. */
#define mainCHECK_TASK_PRIORITY     ( tskIDLE_PRIORITY + 4 )

xQueueHandle QTask_1_2, QTask_2_1;  /* Queue to pass counter back and forth */

xSemaphoreHandle xLCD_semaphore;    /* Semaphore for blocking access to LCD */

/* Implements the 'check' task functionality as described at the top of this
 * file*/
static void prvCheckTask1( void *pvParameters ) __attribute__((noreturn));
static void prvCheckTask2( void *pvParameters ) __attribute__((noreturn));

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
/* Flag used by to indicate the task create status (pass/fail) */
unsigned long ulStatus1 = pdPASS;

/* Configure any hardware required for this demo. */
    vSetupHardware();
    initLCD();          /* LCD is not a task but a device driver */

/* prvCheckTask uses sprintf so requires more stack. */
    ulStatus1 &= xTaskCreate( prvCheckTask1, (signed char *) "Check1",
                                configMINIMAL_STACK_SIZE, NULL,
                                mainCHECK_TASK_PRIORITY, NULL );
	
    ulStatus1 &= xTaskCreate( prvCheckTask2, (signed char *) "Check2",
                                configMINIMAL_STACK_SIZE, NULL,
                                mainCHECK_TASK_PRIORITY, NULL );

    xLCD_semaphore = xSemaphoreCreateMutex();

    if(xLCD_semaphore != NULL)
    {
	QTask_1_2 = xQueueCreate(5, sizeof(int)); /* Q depth = 5, 4 bytes wide*/
	QTask_2_1 = xQueueCreate(5, sizeof(int)); /* Q depth = 5, 4 bytes wide*/

	if(ulStatus1 == pdPASS)		/* Finally start the scheduler. */
	{
            vTaskStartScheduler();
	}
    }
/* Will only reach here if there is insufficient heap available to start
 * the scheduler.  */
    return 1;
}  /* End of main */


/* prvCheckTask1 Function Description ***************************************
 * SYNTAX:          static void prvCheckTask1( void *pvParameters );
 * KEYWORDS:        Task, mutex, semaphore
 * DESCRIPTION:     This is the master task that starts passing the counter
 *                  to the slave task. If BTN1 is pressed, a message is
 *                  sent to the LCD. LEDA is toggeled each time it receives a
 *                  message.
 * PARAMETERS:      the void pointer contains no informatio for this task
 * RETURN VALUE:    None
 * NOTES:           None
END DESCRIPTION ************************************************************/
static void prvCheckTask1( void *pvParameters )
{
int pingpong = 0;	
char msg[32]; 

//    PORTSetPinsDigitalOut(IOPORT_G, LED1);  /* Configure pin as an output. */
    while( 1 )
    {
	LATBINV = LEDA;             /*  Toggle led  */

/* Send message to Task 2 */
	xQueueSend(QTask_1_2, &pingpong, portMAX_DELAY); 

/* Claim exclusive use of LCD - comment out next line to see what happens*/
	xSemaphoreTake(xLCD_semaphore, portMAX_DELAY);

	sprintf(msg,"\fTask #1: #%d", pingpong);
	putsLCD(msg);
    	vTaskDelay( 1000 / portTICK_RATE_MS ); 	/* Delay 1 second */
/* Release exclusive use of LCD */
	xSemaphoreGive(xLCD_semaphore);
/*  Wait for message from Task 2 */
	xQueueReceive(QTask_2_1, &pingpong, portMAX_DELAY); 
    }
} /* End of prvCheckTask1 */

/* prvCheckTask2 Function Description ***************************************
 * SYNTAX:          static void prvCheckTask2( void *pvParameters );
 * KEYWORDS:        Task, mutex, semaphore
 * DESCRIPTION:     This is the slave task. After receiving the counter
 *                  data, it increments the count and sends it back
 *                  to the master task. LEDB is toggled each time it receives
 *                  a message.
 * PARAMETERS:      the void pointer contains no information for this task
 * RETURN VALUE:    None
 * NOTES:           None
 * END DESCRIPTION *****************************************************/
static void prvCheckTask2( void *pvParameters )
{
int pingpong;
char msg[32]; 

//    PORTSetPinsDigitalOut(IOPORT_G, LED2);  /* Configure pin as an output. */
    for( ;; )
    {
	LATBINV = LEDB;	/* Toggle led */

/* Wait for message from Task 1 */
	xQueueReceive(QTask_1_2, &pingpong, portMAX_DELAY); 

	++pingpong;		/* Increment message exchange counter */

/* Claim exclusive use of LCD - comment out next line to see what happens*/
	xSemaphoreTake(xLCD_semaphore, portMAX_DELAY);

        LCDGotoRow(1);
	sprintf(msg,"Task #2: #%d", pingpong);
	putsLCD(msg);
	vTaskDelay( 2000 / portTICK_RATE_MS ); 	/* Delay 2 seconds */

/* Release exclusive use of LCD */
	xSemaphoreGive(xLCD_semaphore);

/* Send message to Task 1 */
	xQueueSend(QTask_2_1, &pingpong, portMAX_DELAY); 
    }
} /* End of prvCheckTask2 */

/*--------------------------End of main for RD6 - LCD - mutex ---------------*/
