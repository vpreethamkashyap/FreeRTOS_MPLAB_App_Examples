
/*  Reference Design #4 - Queues
 * Project:     RD4
 * File name:   main
 * Author:      Richard Wall
 * Date:        September 23, 2013
 *              September 25, 2014 - editorial corrections
 *
 * Description: 4.  RD4 Description:  The operating system manages three
 *                  tasks that toggles an assigned LED and based upon which
 *                  button is pressed.  The single button (prvbutton) task
 *                  serves three button different tasks. The operation of each
 *                  button task is determined by the argument values in
 *                  pvParameters. The program uses a queue (xButtonQueue)
 *                  to send a message from the button task to the led (prvled)
 *                  task to indicate which led to toggle.   The xButtonQueue
 *                  depth is set to 10.
 *
 *                  The xButtonQueue task is passed parameters to indicate which
 *                  button to monitor (port and bit), which PORT B LED to toggle
 *                  and which PORT B LED to have the prvled task toggle. The
 *                  xButtonQueue task is created three times with parameters to
 *                  specify BTN1, BTN2, and BTN3.
 *
 *                  The prvled task is passed parameter to indicate which PORT
 *                  to use for toggling the LEDs.
 *
 *                  When BTN1 is pressed, the xButtonQueue task toggles LEDA
 *                  each 100ms and sends a qMwessage to the prvled task to
 *                  toggle LEDB. The prvled task has a delay of 250 ms.  Hence
 *                  while BTN1 is pressed LEDA blinks on or off each 100ms
 *                  indicating that a message is being sent to the prvled task.
 *                  The prvled task is taking messages out of the queue slower
 *                  than messages are being put in to the queue buffer. When the
 *                  queue buffer is filled, both LED A and LEDB will blink at
 *                  the 250 ms rate. This shows that as the prvled task reads a
 *                  message from the queue buffer, memory is available for the
 *                  prvled task to place another message into the queue buffer.
 *                  Once BTN1 is released, LEDA stops blinking but LEDB
 *                  continues to blink until the queue buffer is emptied.
 *
 *                  The above description applied to BTN2 that controls LEDC
 *                  and LEDD and also BTN3 that controls LEDE and LEDF. See
 *                  the declarations for Button1 through Button3 in main.
 *                  All three button tasks use the same message queue to pass
 *                  data the the prvled task. Hence holding down two or three
 *                  buttons fills the queue buffer faster.
 *
 *                  This operation uses the idle hook task to determine when
 *                  the scheduler has idle time. LEDH is set each time the OS
 *                  enters the idle state and is turned off whenever the OS
 *                  returns to an application task.
 *
 *                  Note: This program DOES NOT perform a "press on - press off"
 *                  operation.  This functionality is left to the motivated
 *                  student as an assignment.
 * 
 *****************************************************************************/

/* XC32 System Files */
#include <plib.h>
#include <stdio.h>

/* Files to support FreeRTOS */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

/* Files to support Application */
#include "chipKIT_Pro_MX7.h"
#include "FreeRTOS_Common_Tasks.h"

/* This task is started multiple time - each instance processes a
 * different button */
static void prvbutton( void *pvParameters );

/*  Simple Tasks that toggles a specific LED when the associated button is
 * pressed. It is possible to have both buttons simultaneously pressed  */
static void prvled( void *pvParameters );

/* The queue that contains the value of the LED to toggle */
static xQueueHandle xButtonQueue;  // Queues must be global

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
/* Task parameter array that indicates the IO port for button inputs,
 * the button bit position, and the LED associated with the button */
int Button1[4] = {IOPORT_G, BTN1, LEDB, LEDA};
int Button2[4] = {IOPORT_G, BTN2, LEDD, LEDC};
int Button3[4] = {IOPORT_A, BTN3, LEDF, LEDE};

/* Specifies the port used for LED outputs used by prvled task */
int task_led = IOPORT_B;

// Flag used by to indicate the task create status (pass/fail)
unsigned long ulStatus1 = pdPASS;

/* Configure processor board  */
    vSetupHardware();

/* Set up queue for a single integer - ten deep */
    xButtonQueue = xQueueCreate(10, sizeof(int));

    // Create the tasks defined within this file.
    ulStatus1 &= xTaskCreate( prvbutton, "BTN1", configMINIMAL_STACK_SIZE, 
                                Button1, tskIDLE_PRIORITY + 1, NULL );
    ulStatus1 &= xTaskCreate( prvbutton, "BTN2", configMINIMAL_STACK_SIZE, 
                                Button2, tskIDLE_PRIORITY + 1, NULL );
    ulStatus1 &= xTaskCreate( prvbutton, "BTN3", configMINIMAL_STACK_SIZE,
                                Button3, tskIDLE_PRIORITY + 1, NULL );
    ulStatus1 &= xTaskCreate( prvled, "LED_1_3", configMINIMAL_STACK_SIZE,
                                &task_led, tskIDLE_PRIORITY, NULL );

/*  Finally start the scheduler if all tasks have been scheduled. */
    if(ulStatus1 == pdPASS)
    {
        vTaskStartScheduler();
    }
/*  Will only reach here if there is insufficient heap available to start
 * the scheduler. */
    return 1;
}

/* prvbutton Function Description ***************************************
 * SYNTAX:		static void prvbutton( void *pvParameters );
 * KEYWORDS:		button, sense
 * DESCRIPTION:         Sends a message to prvled when ever a button is pressed
 *                      to toggle a specific LED.
 * PARAMETER 1:         void pointer - integer type - button to sense
 *                          pvParameters[0] - Port that is to be read
 *                          pvParameters[1] - Bit that is to be read
 *                          pvParameters[2] - LED to toggle
 * RETURN VALUE:	None (There is no returning from TASK functions)
 * NOTES:		Both delays and delay until functions used in this
 *                      task.
 * END DESCRIPTION *****************************************************/
static void prvbutton( void *pvParameters )
{
int* btn_ptr = pvParameters;
int btn_a, btn_b;   /* memory for contact de-bouncing */
int led;
portTickType xLastWakeTime;

    xLastWakeTime = xTaskGetTickCount();   /* Remember initial delay time */
    for( ;; )
    {
        btn_a = PORTReadBits(btn_ptr[0], btn_ptr[1]);
        led = btn_ptr[2]; /* Set which LED is to be toggled by prv led task */
        if(btn_a)	/* Process only if button is pressed */
        {
            do                                  /* de-bounce button contacts */
            {
                btn_b = btn_a;			/* Remember old button state */
                vTaskDelay(20/portTICK_RATE_MS); /* 20 ms switch bounce */
                btn_a = PORTReadBits(btn_ptr[0], btn_ptr[1]);
            } while(btn_a != btn_b);
/* send queue message to prvled to tell it to toggle a specific LED */
            xQueueSend(xButtonQueue, &led, portMAX_DELAY);
            LATBCLR = LEDH;         /* For timing instrumentation only */
            LATBINV = btn_ptr[3];               /* Toggle second LED */
        }
        /*  100 ms sample interval */
	vTaskDelayUntil(&xLastWakeTime, (100/portTICK_RATE_MS));
        LATBCLR = LEDH;         /* For timing instrumentation only */
    }
}

/* prvled Function Description ***************************************
 * SYNTAX:          static void prvled( void *pvParameters );
 * KEYWORDS:        Delay, task, queue
 * DESCRIPTION:     Toggles the LED sent in the xButtonQueue each time the
 *                  task receives a message. The toggling of the LED is
 *                  delayed 250ms to simulate an exaggerated process time. 
 * PARAMETER 1:     void pointer - integer type  pvParameters[0] - LED IO 
 *                  Port
 * RETURN VALUE:    None (There is no returning from this function)
 * NOTES:           portMAX_DELAY is defined as "forever"
 * END DESCRIPTION *****************************************************/
static void prvled( void *pvParameters )
{
int* led_ptr = pvParameters;
int led;

    for( ;; )
    {
	xQueueReceive( xButtonQueue, &led, portMAX_DELAY ); /* Wait forever */
        LATBCLR = LEDH;         /* For timing instrumentation only */
        PORTToggleBits(led_ptr[0], led);    /* Toggle the indicated LED */
	vTaskDelay(250/portTICK_RATE_MS);   /* Minimum display time */
    }
} /* End of prvled */

/*--------------------------End of main for RD4 -----------------------------*/

