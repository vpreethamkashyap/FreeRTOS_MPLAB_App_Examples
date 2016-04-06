/* ******************* Reference Design #2  *****************************
 * Project:         RD2
 * File name:       main.c
 * Author:          Richard Wall
 * Date:            September 21, 2011
 * Revised:         September 3, 2013 - MPLAB X
 *                  September 21, 2014 - corrected spelling errors
 * 
 * Description:     An example of FreeRTOS running a PIC32MX7 on an the
 *                  chipKIT Pro MX7 processor board. This build uses 
 *                  MPLAB X. The operating system manages three tasks that
 *                  that turn on an LED and increments a counter.
 *                  One task is scheduled twice and uses parameters
 *                  sent to the task when scheduled to determine which LED
 *                  to turn on.  The operation uses the idle hook task
 *                  to determine if the scheduler has any idle time. Since all
 *                  tasks are set to the same priority as the idle task - they
 *                  all get the same allotment of CPU time.
 *
 * Observations:    Since all three tasks are the same priority, they are
 *                  allotted equal run times of 1ms (the basic RTOS tick rate.)
 *                  The idlehook task requires only 5us.
 *
 *
 ****************************************************************************/

/* Standard Includes */
#include <plib.h>
#include <stdio.h>

/* Scheduler includes.  */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

/* Hardware dependent settings */
#include "chipKIT_Pro_MX7.h"
#include "FreeRTOS_Common_Tasks.h"

/*Simple Tasks that light a specific LED when running */
static void prvTestJacob( void *pvParameters );
static void prvTestBen( void *pvParameters );


/*  Flag used by prvTestTask1() and prvTestTask2() to indicate their
 * status (pass/fail). */
unsigned long ulStatus1 = pdPASS;

/******************************************************
 * Create the tasks then start the scheduler.
 ******************************************************/
int main( void )
{
int BenTask1 = LEDB;
int BenTask2 = LEDC;

/* Configure hardware for chipKIT Pro MX7 processor board */
	vSetupHardware();

/* Create the tasks defined within this file. The status result
 * indicates whether the task was successfully created. The successive
 * ANDing of the status bit combines the results of the three processes.
 * If any results are not pdPASS, the scheduler will not be started. */

    ulStatus1 = xTaskCreate( prvTestJacob, "Tst1", configMINIMAL_STACK_SIZE,
                                    NULL, tskIDLE_PRIORITY, NULL );

/* Note that task Ben is created twice.  This means that a common task
 * has two different actions. The same code is executed each time the
 * task is active. */
    ulStatus1 &= xTaskCreate( prvTestBen, "Tst2", configMINIMAL_STACK_SIZE,
                                 (void *) &BenTask1, tskIDLE_PRIORITY, NULL );
    ulStatus1 &= xTaskCreate( prvTestBen, "Tst3", configMINIMAL_STACK_SIZE,
                                  (void *) &BenTask2, tskIDLE_PRIORITY, NULL );

/* Finally start the scheduler if all tasks have been scheduled. */
    if(ulStatus1 == pdPASS)
    {
        vTaskStartScheduler();
    }

/* Will only reach here if there is insufficient heap available to start
 * the scheduler. */
    return 0;
} /* End of main */

/* prvTestTaskJacob Function Description *************************************
 * SYNTAX:          static void prvTestJacob( void *pvParameters );
 * KEYWORDS:        RTOS, Task
 * DESCRIPTION:     If LEDA is not lit, all LEDs are turned off and LEDA is
 *                  turned on. Increments a counter each time the task is
 *                  resumed.
 * PARAMETER 1:     void pointer - data of unspecified data type sent from
 *                  RTOS scheduler
 * RETURN VALUE:    None (There is no returning from this function)
 * NOTES:           See FreeRTOS documentation.
 * END DESCRIPTION ************************************************************/
static void prvTestJacob( void *pvParameters )
{
unsigned int counter = 0;

    for( ;; )
    {
        LATGCLR = LEDH;		/* Turn off idle task LED */
/* Since the task runs in a continues loop as long as the schedule has this
 * task running, we want to count only the times we are resuming the task from
 * one of the other scheduled tasks but not the idle task.*/
	if(!(LATB & LEDA))      /* Test for task LED A on - if yes skip next */
	{
            LATBCLR = SM_LEDS;  /* Clear all other SM LEDs */
            LATBSET = LEDA;     /* Set task LED A on */
            ++counter;          /* Increment task service counter. */
            if(counter > 9)
                counter = 0;
	}
    }
}   /* End of prvTestJacob */

/* prvTestBen Function Description ********************************************
 * SYNTAX:          static void prvTestBen( void *pvParameters );
 * KEYWORDS:        RTOS, Task
 * DESCRIPTION:     If LEDB is not lit, all LEDs are turned off and the
 *                  LED specified in the passed parameter is turned on.
 *                  Increments a counter each time the task is
 *                  resumed.
 * PARAMETER 1:     void pointer - specifying which LED to set on
 *                  RTOS scheduler
 * RETURN VALUE:    None (There is no returning from this function)
 * NOTES:           This example demonstrates 2 methods to type the
 *                  void pointer. See FreeRTOS documentation.
 *  DESCRIPTION ************************************************************/
static void prvTestBen( void *pvParameters )
{
int* task_ledp = pvParameters;      /* generate pointer to parameter list */
int task_led;                       /* alt. method - see below */
unsigned int counter = 0;

    for( ;; )
    {
        LATGCLR = LEDH;             /* Turn off idle task LED */
	task_led = *((int *) pvParameters); /* Alt. Retrieve task parameters */

/* Since the task runs in a continues loop as long as the schedule has this
 * task running, we want to count only the times we are resuming the task from
 * one of the other scheduled tasks but not the idle task.*/
	if(!(LATB & *task_ledp))   /* Test for task LED on - if yes skip next */
	{                         
            LATBCLR = SM_LEDS;     /* Turn all other task LEDs */
            LATBSET = *task_ledp;  /* Set the specified LED on */
            ++counter;             /* Increment task service counter. */
            if(counter > 9)
                counter = 0;
	}
    }
} /* End of prvTestBen */

 /*--------------------------End of main  -----------------------------------*/

