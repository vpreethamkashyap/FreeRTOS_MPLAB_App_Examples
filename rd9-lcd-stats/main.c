/*  Reference Design RD9 - LCD with statistics reporting
 * Project      RD9
 * File name:   main.c
 * Author:      Richard Wall
 * Date:        October 5, 2012
 * Revised:     December 22, 2012
 *              September, 12, 2013
 *              October 6, 2014 - editorial corrections
 * 
 * Description: This example passes a counter between two tasks.
 * 		each task increments the counter before passing it
 * 		back. If BTN1 is pressed, a message is set to the LCD
 * 		at the rate of one message per 1/2 second. Since the
 * 		LCD is set for a 2 second persistence, the LCD queue
 * 		is soon filled.  Once the LCD queue is filled, the
 * 		ping-pong is slowed from the 1/2 second rate to the
 * 		LCD 2 second rate.  When BTN1 is released, the LCD
 * 		continues to update the display until the LCD queue
 * 		is empty.
 *
 * 		When BTN2 is pressed, the statics are sent to the
 * 		serial terminal using a serial Tx queue one line at a time.
 *
 *  Note:   1. FreeRTOSConfig.h must be modified so that the following define
 *            statement in line 111 is set to a 1.
 *              #define configGENERATE_RUN_TIME_STATS	1
 *
 *          2. The following code must be in tasks.c immediately following
 *            the #include statements.
 *
 *              #if configGENERATE_RUN_TIME_STATS == 1
 *                  #include "IntQueueTimer.h"
 *                  extern volatile unsigned long ulHighFrequencyTimerTicks;
 *              #endif
 *
 *          3. A function is required to read the formated statistics buffer
 *             and transfer this text to the UART. See the function
 *             void vReportStatics(void) shown below.
 *
 *****************************************************************************/

/* XC32 System Files */
#include <plib.h>
#include <stdio.h>
#include <string.h>

/* Files to support FreeRTOS */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

/* Files to support Application */
#include "chipKIT_Pro_MX7.h"
#include "FreeRTOS_Common_Tasks.h"
#include "LCD.h"
#include "serial.h"
#include "IntQueueTimer.h"

/*-----------------------------------------------------------*/
/* UART priority RX is two higher than ideal */
#define UART_PRIORITY ( tskIDLE_PRIORITY + 3 )
/* Baud rate used by the comtest tasks. */
#define UART_BAUD_RATE ( 19200 )

/* The priorities of the various application tasks. */
#define mainCHECK_TASK_PRIORITY  ( tskIDLE_PRIORITY + 1 )

xQueueHandle QTask_1_2, QTask_2_1;  /* queue to pass counter back and forth */
xQueueHandle xlcdQueueHandle;       /* LCD queue */

/*  Implements the 'check' task functionality as described at the top of
 * this file */
static void prvCheckTask1( void *pvParameters ); 
static void prvCheckTask2( void *pvParameters ); 
static void vReportStatics(void);

/* main Function Description **************************************************
 * SYNTAX:      int main( void );
 * KEYWORDS:    Initialize, create, tasks, scheduler
 * DESCRIPTION: This is a typical RTOS setup function. Hardware is
 *              initialized, tasks are created, and the scheduler is
 *              started.
 * PARAMETERS:  None
 * RETURN VALUE: Exit code - used for error handling
 * NOTES:       None
 * END DESCRIPTION ***********************************************************/
int main( void )
{
portBASE_TYPE ulStatus1 = pdPASS;

/* Configure any hardware required for this application. */
    vSetupHardware();

/* Setup to use the external interrupt controller. */
    vInitialiseTimerForIntQueue();

/* Start tasks that generate their own queues - See serial.h for
 * queue declaration*/
    ulStatus1 &= xStartUARTTasks( UART_PRIORITY, UART_BAUD_RATE );

/* Creates task and generates queue for LCD */
    ulStatus1 &= xStartLCDTask(&xlcdQueueHandle);

/* Create the tasks defined within this file. */
    ulStatus1 &= xTaskCreate( prvCheckTask1, (signed char *) "Check1", 
                                configMINIMAL_STACK_SIZE, NULL,
                                mainCHECK_TASK_PRIORITY, NULL );
    ulStatus1 &= xTaskCreate( prvCheckTask2, (signed char *) "Check2", 
                                configMINIMAL_STACK_SIZE, NULL,
                                mainCHECK_TASK_PRIORITY, NULL );
    QTask_1_2 = xQueueCreate(5, sizeof(int));
    QTask_2_1 = xQueueCreate(5, sizeof(int));

/* Finally start the scheduler. */
    if((ulStatus1 == pdPASS) && (QTask_1_2!= NULL) && (QTask_2_1 != NULL))
    {
        vTaskStartScheduler();
    }
/* Will only reach here if there is insufficient heap available to start
 * the scheduler. */
    return 1;
}

/* prvCheckTask1 Function Description ****************************************
 * SYNTAX:          static void prvCheckTask1( void *pvParameters );
 * KEYWORDS:        Task
 * DESCRIPTION:     This is the master task that starts passing the counter
 *                  to the slave task. If BTN1 is pressed, a message is
 *                  sent to the LCD. LED1 is toggled each time it receives a
 *                  message.
 * PARAMETERS:      the void pointer contains no information for this application
 * RETURN VALUE:    None
 * NOTES:           None
 * END DESCRIPTION **********************************************************/
static void prvCheckTask1( void *pvParameters )
{
static int pingpong = 0;	
static char msg[40]; 
static xLCDMessage LCD_msg;

    strcpy(msg, "\n\rStatistics test ready\n\r");
    xQueueSend( CommTxQueueHandle, msg, portMAX_DELAY );

    LCD_msg.xMinDisplayTime = 1000 / portTICK_RATE_MS;
    LCD_msg.pcMessage = msg;
    while( 1 )
    {
        LATGINV = LED1; 	/* Toggle led  */
        xQueueSend(QTask_1_2, &pingpong, portMAX_DELAY);
        xQueueReceive(QTask_2_1, &pingpong, portMAX_DELAY);
        if(PORTG & BTN1)
        {
            sprintf(msg,"Pingpong: #%d", pingpong);
            LCD_msg.pcMessage = msg;
            xQueueSend( xlcdQueueHandle, &LCD_msg, portMAX_DELAY);
        }
        vTaskDelay( 250 / portTICK_RATE_MS ); /* Delay 1/4 second */
    }
}

/* prvCheckTask2 Function Description *****************************************
 * SYNTAX:          static void prvCheckTask2( void *pvParameters );
 * KEYWORDS:        Task
 * DESCRIPTION:     This is the slave task. After receiving the counter
 *                  data, it increments the count and sends it back
 *                  to the master task. LED2 is toggled each time it receives
 *                  a message.
 * PARAMETERS:      void pointer contains no information for this application
 * RETURN VALUE:    None
 * NOTES:           None
 * END DESCRIPTION ***********************************************************/
static void prvCheckTask2( void *pvParameters )
{
static int pingpong;
	
    for( ;; )
    {
        LATGINV = LED2;    /* Toggle led */
        xQueueReceive(QTask_1_2, &pingpong, portMAX_DELAY);
        ++pingpong;
        xQueueSend(QTask_2_1, &pingpong, portMAX_DELAY);
        vTaskDelay( 250 / portTICK_RATE_MS ); /* Delay 1/4 second */
        if(PORTG & BTN2)
        {
            while(uxQueueMessagesWaiting( CommTxQueueHandle)!=0);
            vReportStatics();
            while(uxQueueMessagesWaiting( CommTxQueueHandle)!=0);
            while(PORTG & BTN2)
            {
                taskYIELD();
            }
        }
    }
}

/* vReportStatics Function Description ***************************************
 * SYNTAX:          static void vReportStatics(void);
 * KEYWORDS:        Statics
 * DESCRIPTION:     Collects RTOS run time statics and sends a report in
 *                  text readable form to the UART
 * PARAMETERS:      None
 * RETURN VALUE:    None
 * NOTES:           This is a blocking function and will remain here until
 *                  the report has been completely sent. The data must be
 *                  read from the queue before the next line of text is
 *                  transferred into the Txbuffer.
 * END DESCRIPTION *****************************************************/

static void vReportStatics(void)
{
#define _LF_    0x0a    /* ASCII line feed  */
signed char cBuffer[256];  /* Must be sized to contain report (175) */
char Tx_Message[UART_Q_LEN];
int buffer_idx = 0;
int line_idx = 0;
int buffer_count;

    strcpy(Tx_Message, "\n\rApplication Task Run Time Statistics\n\r");
    xQueueSend( CommTxQueueHandle, Tx_Message, portMAX_DELAY );
    while(uxQueueMessagesWaiting( CommTxQueueHandle)!=0);
    vTaskGetRunTimeStats(cBuffer);
    LATBSET = LEDA;
    while(cBuffer[buffer_idx] != 0)
    {
        do
        {
            Tx_Message[line_idx++] = cBuffer[buffer_idx++];
        } while((cBuffer[buffer_idx] != 0) && (cBuffer[buffer_idx-1] != _LF_)
                 && (line_idx < UART_Q_LEN-1));

        Tx_Message[line_idx] = 0;
	    xQueueSend( CommTxQueueHandle, Tx_Message, portMAX_DELAY );
        do
        {
            buffer_count = uxQueueMessagesWaiting(CommTxQueueHandle);
        } while(buffer_count!=0);
        line_idx = 0;
    }
    strcpy(Tx_Message, "\n\r");
    xQueueSend( CommTxQueueHandle, Tx_Message, portMAX_DELAY );
    while(uxQueueMessagesWaiting( CommTxQueueHandle)!=0);
    LATBCLR = LEDA;
}
/*------------------ End of main for RD9  --------------------------*/
