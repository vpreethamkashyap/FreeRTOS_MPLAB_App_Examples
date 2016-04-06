/*  Reference Design #3
 * 
 * Author:          Richard Wall
 * Date:            September 21, 2011
 * Revised:         September 3, 2013 - Modified for MPLAB X
 *                  September 21, 2014 - editorial corrections to comments
 * 
 * Description:     RD3 Description:  The operating system creates manages
 *                  multiple tasks that turn on an LED and increments a
 *                  counter. Four tasks toggle LEDs at various rates. Both
 *                  the "time delay" and "delay until OS"" functions block the
 *                  tasks for various periods. The Jacob task toggles LEDA
 *                  each millisecond. The BEN task is created twice (LEDB
 *                  and LEDC). Each creation of the BEN task uses a different
 *                  set of task parameters that controls LEDB to toggle with
 *                  a period of 250 ms and LEDC to toggle with a 750ms period.
 *                  The Cody task is set for a higher priority level than the
 *                  other three and toggles LEDD with a 100ms period.
 *
 *                  The two BEN tasks demonstrate the potential for code
 *                  conservation by using re-entrant functions. The main reason
 *                  the BEN code is re-entrant is that each task uses independent
 *                  resources. In this case - the resource the two tasks cannot
 *                  share is the LED that is being flashed.
 *
 *                  This application uses the idle hook task to determine when
 *                  the scheduler has idle time. Each time the idle hook
 *                  function is called, LEDH is turned on.  Whenever any other
 *                  task resumes running, LEDH is turned off. Since LEDH is
 *                  high (meaning the idle task is running) most of the time,
 *                  very little time is spent executing the code for the four
 *                  tasks.
 *
 *                               
 *****************************************************************************/

/* XC32 System Files */
#include <plib.h>
#include <stdio.h>

/* Files to support FreeRTOS */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

/* Files to support Application  */
#include "chipKIT_Pro_MX7.h"
#include "FreeRTOS_Common_Tasks.h"

/* Simple Tasks that toggle a specific LED when running */
static void prvTestJacob( void *pvParameters );
static void prvTestBen( void *pvParameters );
static void prvTestCody( void *pvParameters );

/* main Function Description ***************************************
 * SYNTAX:		int main( void );
 * KEYWORDS:		Initialize, create, tasks, scheduler
 * DESCRIPTION:         This is a typical RTOS set up function. Hardware is
 * 			initialized, tasks are created, and the scheduler is
 * 			started.
 * PARAMETERS:		None
 * RETURN VALUE:	Exit code - used for error handling
 * NOTES:
 *
 * END DESCRIPTION *****************************************************/
int main( void )
{
int BenTask1[2] = {LEDB, 250};  /* Parameter list to specify the LED and delay*/
int BenTask2[2] = {LEDC, 750};  /* Parameter list to specify the LED and delay*/
int CodyTask[2]	= {LEDD, 100};  /* Parameter list to specify the LED and delay*/

/* Flag used by to indicate the task create status (pass/fail) */
unsigned long ulStatus1 = pdPASS; // Sticky bit reset if did not create task

/* Configure any hardware required for this demo.  */
    vSetupHardware();

/* Create the tasks */
    ulStatus1 &= xTaskCreate( prvTestJacob, "Jacob", configMINIMAL_STACK_SIZE,
                                NULL, tskIDLE_PRIORITY, NULL );
    ulStatus1 &= xTaskCreate( prvTestBen, "Ben1", configMINIMAL_STACK_SIZE,
                                (void *) BenTask1, tskIDLE_PRIORITY, NULL );
    ulStatus1 &= xTaskCreate( prvTestBen, "Ben2", configMINIMAL_STACK_SIZE,
                                (void *) BenTask2, tskIDLE_PRIORITY, NULL );
    ulStatus1 &= xTaskCreate( prvTestCody, "Cody", configMINIMAL_STACK_SIZE,
                                (void *) CodyTask, tskIDLE_PRIORITY+1, NULL );

/*  Finally start the scheduler if all tasks have been scheduled. */
    if(ulStatus1 == pdPASS)
    {
        vTaskStartScheduler();
    }

/* Will only reach here if there is insufficient heap available to start
 * the scheduler. */
    return 0;
} /* End of main */

/* prvTestJacob Function Description ************************************
 * SYNTAX:          static void prvTestJacob( void *pvParameters );
 * KEYWORDS:        RTOS, Task
 * DESCRIPTION:     Toggles LED A each second
 * PARAMETER 1:     void pointer - data of unspecified data type sent from
 *                  RTOS scheduler
 * RETURN VALUE:    None (There is no returning from this function)
 * NOTES:           See FreeRTOS documentation.
 * END DESCRIPTION **************************************************/
static void prvTestJacob( void *pvParameters )
{
unsigned int counter = 0;
    for( ;; )
    {
	LATBCLR = LEDH;                 /* Turn off idle task LED */
	vTaskDelay(1000/portTICK_RATE_MS);
	LATBINV = LEDA;
	++counter;
    }
} /* End of prvTestJacob */

/* prvTestBen Function Description ************************************
 * SYNTAX:          static void prvTestBen( void *pvParameters );
 * KEYWORDS:        RTOS, Task
 * DESCRIPTION:     Toggles LED B or C at the designated rate
 * PARAMETER 1:     void pointer -
 *                  [0] - LED to toggle
 *                  [1] - delay time in ms
 * RETURN VALUE:    None (There is no returning from this function)
 * NOTES:           None
 * END DESCRIPTION *****************************************************/
static void prvTestBen( void *pvParameters )
{
int* param_ptr = pvParameters;
unsigned int counter = 0;

    for( ;; )
    {
    	LATBCLR = LEDH;		/*  Turn off idle task LED */
	vTaskDelay(param_ptr[1]/portTICK_RATE_MS);
	LATBINV = param_ptr[0];
	++counter;
    }
} /* End of prvTestBen */

/* prvTestBen Function Description ***************************************
 * SYNTAX:		static void prvTestBen( void *pvParameters );
 * KEYWORDS:		RTOS, Task
 * DESCRIPTION:         Toggles LED B or C at the designated rate
 * PARAMETER 1:         void pointer -
 *                      [0] - LED to toggle
 *                      [1] - delay time in ms
 * RETURN VALUE:	None (There is no returning from this function)
 * NOTES:		None
 * END DESCRIPTION *****************************************************/

static void prvTestCody( void *pvParameters )
{
int* param_ptr = pvParameters;
unsigned int counter = 0;
portTickType xLastWakeTime;

    xLastWakeTime = xTaskGetTickCount();
    for( ;; )
    {
	LATBCLR = LEDH;			// Turn off idle task LED		
	vTaskDelayUntil(&xLastWakeTime, (param_ptr[1]/portTICK_RATE_MS));
	LATBINV = param_ptr[0];
	++counter;
    }
} /* End of prvTestCody */

/*--------------------------End of main  -----------------------------------*/
