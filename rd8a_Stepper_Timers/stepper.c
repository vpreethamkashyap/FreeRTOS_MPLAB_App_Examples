/******************  Stepper.c - Stepper Motor Control ***********************
 * Author:      Richard Wall
 * Date:        September 22, 2011
 * Revised:     September 3, 2013
 *              October 3, 2013 - Updated xStartSTEPPERTask - moved all timer
 *                                operations to this file
 *
 * Description: This file contains functions to initialize a stepper motor for
 *              Digilent PmodSTEP and RTOS tasks to control it. Intended to be
 *              implemented using a Digilent chipKIT Pro MX7 processor board.
 *              This is an example of a fully encapsulated stepper motor
 *              control function that uses RTOS timers for speed control.
 *
 *              The stepper motor step interval is changes using the
 *              xTimerChangePeriod statement in the prvStepperStep task.
 * 
 *****************************************************************************/

/* XC32 peripheral library include */
#include <plib.h>

/* FreeRTOS included files */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "timers.h"

/* Reference design included files */
#include "chipKIT_Pro_MX7.h"
#include "stepper.h"

/* Communications between tasks within this file */
static xSemaphoreHandle take_step;  /* Timer signal to take a step */
static xTimerHandle T1Handle;       /* Timer handle */

/* Timer function to initiate a motor step */
static void vStepperCallBack(xTimerHandle T1Handle);

/* Moves the motor a single step  */
static void prvStepperStep(void * data);

/* xStartSTEPPERTask Function Description ************************************
 * SYNTAX:      xQueueHandle xStartSTEPPERTask( void );
 * KEYWORDS:    Stepper Motor, Queue, semaphore
 * DESCRIPTION: Creates the stepper motor step task, a queue for
 *              communicating parameters to the stepper motor step
 *              task, and a binary semaphore for indicating when to
 *              take a step. The timer task is also created and 
 *              started by this function.
 * PARAMETERS:	None
 * RETURN VALUE: QueueHandle
 * NOTES:       None
 * END DESCRIPTION *****************************************************/
portBASE_TYPE xStartSTEPPERTask( xQueueHandle *qHandle  )
{
portBASE_TYPE pdStatus = pdPASS;
long timer_id = 100; 	/* This parameter identified the timer that initiates
                         * the callback */

/* This queue communicate the stepper motor parameters from the read buttons
 * task to the stepper task */
    *qHandle = xQueueCreate( 1, sizeof( xSTEPPER_PARAMS ));

/* This semiphore indicates when it is time to take a step */
    vSemaphoreCreateBinary( take_step);

/* Sets Port B for stepper motor outputs */
    PORTSetPinsDigitalOut(IOPORT_B, SM_COILS);
    PORTClearBits(IOPORT_B, SM_COILS);

/* Add the stepper motor task to the scheduler */
    pdStatus &= xTaskCreate( prvStepperStep, ( signed char * ) "STEPPER",
                configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 3, NULL );

/* Creat a timer handle for the 1ms interval callback function */
    T1Handle = xTimerCreate((const signed char *) "TIMER1",
                            (portTickType) (1000/ portTICK_RATE_MS) ,
                            (unsigned portBASE_TYPE) pdTRUE,
                            &timer_id, vStepperCallBack);

/* Start this timer */
    pdStatus &= xTimerStart(T1Handle, 0);

    return pdStatus;    /* Return the initialization success status */
} /************************ End of xStartSTEPPERTask ***********************/

/* prvStepperStep FUNCTION DESCRIPTION ***************************************
 * SYNTAX:          static void prvStepperStep( void *pdata );
 * KEYWORDS:        Stepper motor step task
 * DESCRIPTION:     This task is blocked by a binary semaphore that is given in
 *                  the vStepperCallBack function. This task also checks if the
 *                  queue from the buttons task is updating the operating
 *                  parameters for mode and direction.
 * PARAMETER 1:     pointer to a void type for passing task parameters
 * RETURN VALUE:    None
 * NOTES:           The stepper motor speed is not controlled from this task.
 *                  LEDB is toggled each step
 * END DESCRIPTION **********************************************************/
static void prvStepperStep( void *pdata )
{
xSTEPPER_PARAMS sm_params;	/* Buffer to receive the SM parameters */
const unsigned char step_code[8] = {0x0A,0x08,0x09,0x01,0x05,0x04,0x06,0x02};
unsigned int step_ptr = 0; // Initialize pointer only on power up
unsigned int sm_output;

/* Initial SM settings until a button is pressed the first time */
    sm_params.Stepper_period = RPM_15;
    sm_params.Stepper_Direction = 1;
    sm_params.Stepper_Mode = 1;

/* Set the timer interval for the initial stepper motor operating step rate */
    xTimerChangePeriod(T1Handle,sm_params.Stepper_period/portTICK_RATE_MS,0);

    while(1)
    {
 /* Wait for Timer to give the binary semaphore to take a step */
        xSemaphoreTake(take_step, portMAX_DELAY);

	LATBINV = LEDB;     /* Signal step */
/* Accept update from queue if any */
 	if(uxQueueMessagesWaiting(smQueueHandle)>0)
        {
/* Update SM Parameters. */
            xQueueReceive(smQueueHandle, &sm_params, 0);
            xTimerChangePeriod(T1Handle,
                        sm_params.Stepper_period/portTICK_RATE_MS, 0);
	}
        step_ptr += (sm_params.Stepper_Direction * sm_params.Stepper_Mode);

/* Force step_ptr to be modulo 8 */
        step_ptr &= ((sizeof(step_code)/sizeof(step_code[0])) - 1);

/* Determine new stepper motor outputs. The shift-left 7 is required to
 * align the bits to the port-pin connections */
        sm_output = ((unsigned int) step_code[step_ptr]) << SM_SHIFT;

/* Write code to stepper IO using an unprotected read modify write operation */
        PORTWrite(IOPORT_B, ((LATB & ~((unsigned int)STEPPER_MASK)))\
                            | sm_output);
    }
} /************************* End of prvStepperStep **************************/

/* vStepperCallBack FUNCTION DESCRIPTION *************************************
 * SYNTAX:          static void vStepperCallBack(xTimerHandle timerhandle);
 * KEYWORDS:        Timer call back function
 * DESCRIPTION:     The timer call back function is accessed by the RTOS Timer.
 *                  The only thing that happens here is to give a blocking
 *                  semaphore to the SM step task.
 *                  to stepper motor task
 * PARAMETER 1:     xTimerHandle timerhandle identifies the particular timer
 *                  that is calling this function it is not used for this
 *                  reference design.
 * RETURN VALUE:    None
 * END DESCRIPTION **********************************************************/
static void vStepperCallBack(xTimerHandle timerhandle)
{
    xSemaphoreGive(take_step);	/* Allow step to be taken */
} /************************ End of vStepperCallBack *************************/

/*----------------------------- End of stepper.c  --------------------------*/
