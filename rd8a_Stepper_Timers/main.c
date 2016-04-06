/********************************************************************
 * 		 RD8a - Stepper Motor Control using Timers
 *
 * Author:      Richard Wall - Copyright (C) 2012
 * Date:        October 2, 2012
 * Revised: 	December 22, 2012
 *              October 3, 2013
 *              October 3, 2014 - editorial changes, updated target processor 
 *                                board.
 * 
 * Description:    An example of FreeRTOS running on an chipKIT Pro MX7 using
 *              a PIC32MX7 processor. This example controls the stepper
 *              motor similar to RD 5 except that the RTOS timer API is
 *              used instead of a timer interrupt to control the speed of
 *              the stepper motor. Note the additions to FreeRTOSConfigure.h
 *              for configurations necessary to use timers.
 * 
 *              Files timer.c and timer.h must be added to the project as well.
 *
 *************************************************************************/
/* XC32 System Files */
#include <plib.h>
#include <stdio.h>

/* Files to support FreeRTOS */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"

/* Files to support application  */
#include "chipKIT_Pro_MX7.h"
#include "FreeRTOS_Common_Tasks.h"
#include "stepper.h"

/* Application task and function declarations   */
static void prvButtons( void *pvParameters );

static void decode_buttons(int switch_state, 
                            int *mode,
                            int *speed,
                            int *direction);

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
portBASE_TYPE pdStatus1 = pdPASS;
 
/* Configure any hardware required for this demo. */
    vSetupHardware();

/* Task to convert the button status to stepper motor control parameters */
    pdStatus1 &= xTaskCreate( prvButtons, (signed char *) "BTN1_2",
                            configMINIMAL_STACK_SIZE,
                            NULL, tskIDLE_PRIORITY, NULL );

 /* Starting the stepper motor task will assign the queue handle for sending
  * motor operating parameters the the stepper motor control task. The
  * returned status indicates the success or failure of creating the stepper
  * motor task and the queue. */
    pdStatus1 &= xStartSTEPPERTask(&smQueueHandle);

    if(pdStatus1 == pdPASS)
    {
        vTaskStartScheduler();
    }
    
/* Will only reach here if there is insufficient heap available to start
 * the scheduler. */
    return 1;
}
/* decode_buttons FUNCTION DESCRIPTION ***************************************
 * SYNTAX:          static void prvButtons( void *pvParameters );
 * KEYWORDS:        Change notice, button status, change, decode
 * DESCRIPTION:     Detects button change of status and sends message
 *                  to stepper motor task.  This task also update the
 *                  timer period that controls the SM step rate.
 * PARAMETER 1:     void pointer - data of unspecified data type sent from
 *                  RTOS scheduler - not used
 * RETURN VALUE:    None
 * END DESCRIPTION **********************************************************/
static void prvButtons( void *pvParameters )
{
int buttons;
xSTEPPER_PARAMS sm_params;

    mPORTGSetPinsDigitalIn(BTN1|BTN2);
    mCNOpen(CN_ON, (CN8_ENABLE | CN9_ENABLE), CN_PULLUP_DISABLE_ALL);
    buttons = mPORTGRead();

    for( ;; )
    {
    	while(!INTGetFlag(INT_CN));	/* Wait for button press */
	{
            vTaskDelay(100/portTICK_RATE_MS); /* button sample rate */
	}
	vTaskDelay(20/portTICK_RATE_MS); /* button debounce */
	buttons = mPORTGRead() & (BTN1 | BTN2); 	

	decode_buttons(buttons,&sm_params.Stepper_Mode, 
                        &sm_params.Stepper_period,
                        &sm_params.Stepper_Direction);

/* Update stepper motor operating mode and direction */
        xQueueSend(smQueueHandle, &sm_params, portMAX_DELAY);

/* CLear button Change Notice flag */
        INTClearFlag(INT_CN);
    }
}

/* decode_buttons FUNCTION DESCRIPTION ***************************************
 * SYNTAX:      void decode_buttons(int switch_state, unsigned int *mode,
 *                                  unsigned int *speed, int *direction);
 * KEWORDS:     Decode, buttons
 * DESCRIPTION: Assigns state inputs for stepper motor control
 * Parameter 1: Button inputs
 * Parameter 2: Stepping mode variable pointer --> Full Step==2 or Half Step==1
 * Parameter 3: Stepping rate variable pointer --> Period in ms to delay for
 *                                                 making a step.
 * Parameter 4:	Stepping direction variable pointer -->Either CW==+1 or CCW==-1
 * RETURN VALUE:  none
 * END DESCRIPTION **********************************************************/
static void decode_buttons(int switch_state, int *mode,
                           int *period, int *direction)
{
    switch(switch_state)    // Decode switch state
    {
        case (!BTN2 & !BTN1):
            *period = MSperREV_HS/5/2;	// Full step, CW, 5 RPM
            *mode = FULLSTEP;
            *direction = CW;
            break;
      	case BTN2:              
            *period = MSperREV_HS/10;  // Half step, CCW, 10 RPM
            *mode = HALFSTEP;
            *direction = CW;
            break;
      	case BTN1:				
            *period = MSperREV_HS/20;  	// Half step, CCW, 2 RPM
            *mode = FULLSTEP;
            *direction = CCW;
            break;
      	case (BTN1 | BTN2):		
            *period = MSperREV_HS/10/2;  // Full step, CW, 10 RPM -
            *mode = FULLSTEP;
            *direction = CCW;
            break;
    }
}

/*--------------------------End of main  -----------------------------------*/
