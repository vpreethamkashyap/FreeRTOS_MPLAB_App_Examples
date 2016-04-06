/******************  Reference design #5 - Stepper Motor Control ***********
 * Author:      Richard Wall
 * Date:        September 22, 2011
 * Revised:     September 9, 2013
 * Description: This program controls a stepper motor based upon the status of
 *              BTN1 and BTN2.  The Button task (prvButtons) detects a change
 *              of button status, decodes the button controls, and determines
 *              the values of step delay, direction, and step mode.  These
 *              values are passed via a queue to the stepper motor control task
 *              (prvStepperStep).  The stepper motor control task is blocked
 *              from execution until a semaphore is sent from the Timer 3 ISR.
 *              The queue for messages from the button detection task is checked
 *              each step to determine if new operating parameters have been
 *              set by pressing BTN1 or BTN2.
 *
 *              The operation uses the idle hook task to determine when the
 *              scheduler has idle time. The configUSE_IDLE_HOOK bit in
 *              FreeRTOSconfig.h must be set to 1.
 *
 *              A possible design modification is to control LED1 and LED2 when
 *              BTN1 and BTN2 are pressed
 *
 * Instrumentation:
 *              LEDA - Step clock
 *              LEDB - Step period
 *              LEDC - Timer3 interrupt
 *              LEDD - Idle task
*****************************************************************************/

/* XC32 System Files */
#include <plib.h>
#include <stdio.h>

/* Files to support FreeRTOS */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

/* Files to support Application */
#include "chipKIT_Pro_MX7.h"
#include "FreeRTOS_Common_Tasks.h"
#include "stepper.h"

/* Setup the processor ready for the demo. */

static void prvButtons( void *pvParameters );
static xQueueHandle smQueueHandle;
static void decode_buttons(int switch_state, 
				int *mode, 
				int *speed, 
				int *direction);

/* main Function Description ***************************************
 * SYNTAX:		int main( void );
 * KEYWORDS:		Initialize, create, tasks, scheduler
 * DESCRIPTION:         This is a typical RTOS set up function. Hardware is
 *                      initialized, tasks are created, and the scheduler is
 *                      started.
 * PARAMETERS:          None
 * RETURN VALUE:        Exit code - used for error handling
 * NOTES:
 *
 * END DESCRIPTION *****************************************************/
int main( void )
{
unsigned long pdStatus1 = pdPASS; // Sticky bit reset if did not create task

/* Configure any hardware required for this demo.  */
    vSetupHardware();

/* Create the tasks defined within this file.  */
    pdStatus1 &= xTaskCreate( prvButtons,
                              (const signed char * const) "BTN1_2",
                              configMINIMAL_STACK_SIZE, NULL,
                              tskIDLE_PRIORITY, NULL );

/* Creates stepper motor task and returns handle */
    smQueueHandle = xStartSTEPPER(); /* Handle is needed for queue routing */

/* Finally start the scheduler. */
    if((pdStatus1 == pdPASS) && (smQueueHandle != NULL))
    {
        vTaskStartScheduler();
    }

/* Will only reach here if there is insufficient heap available 
 * to start the scheduler. */
    return 0;
} /* End of main */

/* decode_buttons FUNCTION DESCRIPTION ****************************************
 * SYNTAX:          static void prvButtons( void *pvParameters );
 * KEYWORDS:        Change notice, button status, change, decode
 * DESCRIPTION:     Detects button change of status and sends message
 *                  to stepper motor task
 * PARAMETER 1:     void pointer - data of unspecified data type sent from
 *                  RTOS scheduler - not used
 * RETURN VALUE:    None
 * Notes:           Although this function uses the change notice flag, the
 *                  change notice interrupt is not enabled. The CN interrupt
 *                  flag is polled to see if BTN1 or BTN2 has change position.
 *                  Push-on / push-off button operation is not implemented.
 * END DESCRIPTION **********************************************************/
static void prvButtons( void *pvParameters )
{
int buttons;
xSTEPPER_PARAMS sm_params;

/* Initialize CN detection */
    mCNOpen(CN_ON, (CN8_ENABLE | CN9_ENABLE), CN_PULLUP_DISABLE_ALL);
    buttons = mPORTGRead(); /* Initialize the buttons variable so CN flag so the
                             * CN flag will be set.  */
    for( ;; )
    {
        while(!INTGetFlag(INT_CN))
        {
            vTaskDelay(20/portTICK_RATE_MS);    /* Button sample rate */
            PORTClearBits(IOPORT_B, LEDD);      /* Clear idle running flag */

        }
        vTaskDelay(20/portTICK_RATE_MS);        /* 20 ms button debounce */
        PORTClearBits(IOPORT_B, LEDD);          /* Clear idle running flag */
        buttons = PORTReadBits((IOPORT_G), (BTN1 | BTN2));

        if(buttons & BTN1)
            PORTSetBits(IOPORT_G, LED1);
        else
            PORTClearBits(IOPORT_G, LED1);

        if(buttons & BTN2)
            PORTSetBits(IOPORT_G, LED2);
        else
            PORTClearBits(IOPORT_G, LED2);
/* Set the parameters in the stepper motor control structure */
        decode_buttons(buttons,&sm_params.Stepper_Mode,
                        &sm_params.Stepper_period,
                        &sm_params.Stepper_Direction);

/* Send the new parameters to the stepper motor control task*/
        xQueueSend(smQueueHandle, &sm_params, portMAX_DELAY);
        PORTClearBits(IOPORT_B, LEDD);  /* Clear idle running flag */
        INTClearFlag(INT_CN);           /* CLear the CN interrupt flag */
    }
} /* End of prvButtons */

/* decode_buttons FUNCTION DESCRIPTION ***************************************
 * SYNTAX:        void decode_buttons(int switch_state, unsigned int *mode,
 *                                      unsigned int *speed, int *direction);
 * KEYWORDS:        Decode, buttons
 * DESCRIPTION:     Assigns state inputs for stepper motor control
 * Parameter 1:     Button inputs
 * Parameter 2:     Stepping mode variable pointer -->
 *                      Full Step==2 or Half Step==1.
 * Parameter 3:     Stepping rate variable pointer -->
 *                      Period in ms to delay
 *                  for making a step.
 * Parameter 4:     Stepping direction variable pointer -->
 *                      Either CW==+1 or CCW==-1.
 * RETURN VALUE:    None
 * Notes:           None
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
} /* End of decode_buttons */

/*--------------------------End of main for RD5 --------------------------*/
