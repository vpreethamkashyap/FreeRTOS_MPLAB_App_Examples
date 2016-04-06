/******************  Stepper.c - Stepper Motor Control ***********************
 * Author:      Richard Wall
 * Date:        September 22, 2011
 * Revised:     September 3, 2013
 *              September 22, 2013  Change Timer3 to using XC32 peripheral
 *              library macros and functions.
 * Description: This file contains functions to initialize a stepper motor for
 *              PmodSTEP and RTOS tasks to control it.
 *****************************************************************************/

/* Peripheral library include  */
#include <plib.h>

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "chipKIT_Pro_MX7.h"

/* Application included files. */
#include "stepper.h"

static xSemaphoreHandle smTake_Step;
static xQueueHandle smQueueHandle;

static void vInitialiseTimerForIntQueue( void );

/* Moves the motor a single step */
static void prvStepperStepTask(void * data);

/* xStartSTEPPERTask Function Description ************************************
 * SYNTAX:      xQueueHandle xStartSTEPPERTask( void );
 * KEYWORDS:    Stepper Motor, Queue, semaphore
 * DESCRIPTION: Creates the stepper motor step task, a queue for
 *              communicating parameters to the stepper motor step
 *              task, and a binary semaphore for indicating when to
 *              take a step.
 * PARAMETERS:  None
 * RETURN VALUE: QueueHandle
 * NOTES:       None
 * END DESCRIPTION *****************************************************/
xQueueHandle xStartSTEPPER( void )
{
/* This queue communicate the stepper motor parameters from the read
 * buttons task to the stepper task */
    smQueueHandle = xQueueCreate( 1, sizeof( xSTEPPER_PARAMS ));

/* This semiphore indicates when it is time to tak a step */
    vSemaphoreCreateBinary( smTake_Step);

/* Sets Port B for stepper motor outputs to zero */
    PORTClearBits(IOPORT_B, SM_LEDS);

/* Adds the stepper motor task to the scheduler  */
    xTaskCreate( prvStepperStepTask, ( signed char * ) "STEPPER",
                configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 3, NULL );

    return smQueueHandle;
} /* End of xStartSTEPPERTask */
/*-----------------------------------------------------------*/

/* prvStepperStep Function Description ************************************
 * SYNTAX:      static void prvStepperStep( void *pdata );
 * KEYWORDS:    Stepper Motor, step
 * DESCRIPTION: Creates the stepper motor step task, a queue for
 *              communicating parameters to the stepper motor step
 *              task, and a binary semaphore for indicating when to
 *              take a step.
 * PARAMETERS:  None
 * RETURN VALUE: QueueHandle
 * NOTES:       None
 * END DESCRIPTION *****************************************************/
static void prvStepperStepTask( void *pdata )
{
xSTEPPER_PARAMS sm_params;
const unsigned char step_code[8] = {0x0A,0x08,0x09,0x01,0x05,0x04,0x06,0x02};
unsigned int step_ptr = 0; /* Initialize pointer only on power up */
unsigned int portb_code, sm_output;
unsigned int delay_counter;

    vInitialiseTimerForIntQueue();
    sm_params.Stepper_period = 100;
    sm_params.Stepper_Direction = 1;
    sm_params.Stepper_Mode = 1;
    delay_counter = sm_params.Stepper_period;
    while(1)
    {
        PORTClearBits(IOPORT_B, LEDD);          /* Clear idle running flag */

 /* wait for semaphore to take a step */
        xSemaphoreTake(smTake_Step, portMAX_DELAY);

/* Signal semaphore from timer interrupt event */
        PORTToggleBits(IOPORT_B, LEDA);
	
/* Update pointer */
        if(!delay_counter--)
        {
/* Signal delay counter at zero to mark a step event */
            PORTToggleBits(IOPORT_B, LEDB);
/* Accept msg from queue if any */
            if(uxQueueMessagesWaiting(smQueueHandle)>0)  
            {  
                xQueueReceive(smQueueHandle, &sm_params, 0);
                delay_counter = sm_params.Stepper_period;
            }
            delay_counter = sm_params.Stepper_period;

 /* Stepper Motor position cotrol FSM */
            step_ptr += (sm_params.Stepper_Direction * sm_params.Stepper_Mode);
 /* Force step_ptr modulo 8 */
            step_ptr &= ((sizeof(step_code)/sizeof(step_code[0])) - 1);	

/* Determine new stepper motor code and shift to place correct bits correctly */
            sm_output = step_code[step_ptr] << 7;

/* Read the PmodSTEP output pins and mask to clear the stepper motor bits*/
            portb_code = PORTRead(IOPORT_B) & ~((unsigned int)STEPPER_MASK);

/* Write the new stepper motor output bits to the PmodSTEP */
            PORTWrite(IOPORT_B, (portb_code | sm_output));
        }
    }
} /* End of prvStepperStep */

/* vInitialiseTimerForIntQueue Function Description ***************************
 * SYNTAX:      static void vInitialiseTimerForIntQueue( void );
 * KEYWORDS:    Timer, Interrupt
 * DESCRIPTION: Initializes Timer 3 for 1ms interrupts
 * PARAMETERS:  None
 * RETURN VALUE: None
 * NOTES:       Peripheral library functions are used
 * END DESCRIPTION ***********************************************************/
static void vInitialiseTimerForIntQueue( void )
{
/* Timer 1 is used for the tick interrupt, timer 2 is used for the high
 * frequency interrupt test.  Therefore this application uses timers 3. */
#define timerINTERRUPT3_FREQUENCY 1000  /* in Hz */

    CloseTimer3();              /*Stop Timer 3 */
/* Clear the interrupt as a starting condition. */
    INTClearFlag(INT_T3);
/* Setup timer 3 interrupt priority to be above the kernel priority. */
    ConfigIntTimer3(T3_INT_ON | (configMAX_SYSCALL_INTERRUPT_PRIORITY - 1));
/* Start Timer 3 */
    OpenTimer3(T3_ON | T3_SOURCE_INT | T3_PS_1_1,\
              ( unsigned short ) ( configPERIPHERAL_CLOCK_HZ\
                / timerINTERRUPT3_FREQUENCY ) - 1);
} /* End of vInitialiseTimerForIntQueue */

/* vT3InterruptHandler Function Description *********************************
 * SYNTAX:      void vT3InterruptHandler( void );
 * KEYWORDS:    Timer, ISR, Interrupt Handler
 * DESCRIPTION: Sends a semaphore each interrupt
 * PARAMETERS:  None
 * RETURN VALUE: None
 * NOTES:       LEDC is toggled for timing instrumentation
 *                      See Note 1 in RD6 Notes.txt for Interrupts in FreeRTOS
 * END DESCRIPTION *****************************************************/
void __ISR(_TIMER_3_VECTOR, ipl2) vT3InterruptHandler(void)
{
portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE; /* See project documentation*/
    INTClearFlag(INT_T3);

    PORTToggleBits(IOPORT_B, LEDC); /* Toggle the Timer 3 event bit */

    xSemaphoreGiveFromISR(smTake_Step, &xHigherPriorityTaskWoken);
    portEND_SWITCHING_ISR( xHigherPriorityTaskWoken );
} /* End of vT3InterruptHandler */

/* End of stepper.c */


