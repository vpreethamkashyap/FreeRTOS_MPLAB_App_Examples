/***************************************************************************
 * File name:   LCD.c
 * Author:      Richard Wall
 * Date:        September 13, 2013
 *              October 5, 2014 - editorial corrections
 *
 * Description: This file provides character and string writing to a 16X2
 *              character LCD.
 ***************************************************************************/

/* peripheral library include */
#include <plib.h>

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

/* LCD included files. */
#include "lcd.h"

/*
 * The LCD is written to by more than one task so is controlled by this
 * 'gatekeeper' task.  This is the only task that is actually permitted to
 * access the LCD directly.  Other tasks wanting to display a message send
 * the message to the gatekeeper.
 */
static void vLCDTask( void *pvParameters );

/* Setup the peripherals required to communicate with the LCD. */
static void prvSetupLCD( void );

/* The queue used to send messages to the LCD task - global declaration */
xQueueHandle xLCDQueue;

/* Private LCD access functions. */
static void prvLCDCommand( char cCommand );
static void prvLCDData( char cChar );
/*-----------------------------------------------------------*/

portBASE_TYPE xStartLCDTask(xQueueHandle *xLCD_Queue  )
{
portBASE_TYPE ulStatus1 = pdFAIL;
/* Create the queue used by the LCD task.  Messages for display on the LCD
 * are received via this queue. */
    xLCDQueue = xQueueCreate( lcdQUEUE_SIZE, sizeof( xLCDMessage ));
    if(xLCDQueue != NULL )
    {
        ulStatus1 = pdPASS;
        *xLCD_Queue = xLCDQueue;
/* Start the task that will write to the LCD.  The LCD hardware is
 * initialised from within the task itself so delays can be used. */
        ulStatus1 &= xTaskCreate( vLCDTask, ( signed char * ) "LCD",
                            configMINIMAL_STACK_SIZE, NULL,
                            tskIDLE_PRIORITY + 1, NULL );
    }
    return ulStatus1;
}
/*-----------------------------------------------------------*/

void vLCDGotoRow( unsigned short usRow )
{
    if(usRow == 0)
    {
        prvLCDCommand( LCD_CURSOR_HOME_CMD );
    }
    else
    {
        prvLCDCommand( LCD_NEW_LINE );
    }
}
/*-----------------------------------------------------------*/

static void prvLCDCommand( char cCommand )
{
    PMPSetAddress( LCD_COMMAND_ADDRESS );
    PMPMasterWrite( cCommand );
    vTaskDelay( lcdSHORT_DELAY );
}
/*-----------------------------------------------------------*/

void prvLCDData( char cChar )
{
    PMPSetAddress( LCD_DATA_ADDRESS );
    PMPMasterWrite( cChar );
    vTaskDelay( lcdVERY_SHORT_DELAY );
}
/*-----------------------------------------------------------*/

void vLCDPutString( char *pcString )
{
/* Write out each character with appropriate delay between each. */
    while(*pcString)
    {
        prvLCDData(*pcString);
        pcString++;
        vTaskDelay(lcdSHORT_DELAY);
    }
}
/*-----------------------------------------------------------*/

void vLCDClear(void)
{
    prvLCDCommand(LCD_CLEAR_DISPLAY_CMD);
}
/*-----------------------------------------------------------*/

static void prvSetupLCD(void)
{
/* Wait for proper power up. */
    vTaskDelay( lcdLONG_DELAY );
	
/* Open the PMP port */
    mPMPOpen((PMP_ON | PMP_READ_WRITE_EN | PMP_CS2_CS1_EN |
             PMP_LATCH_POL_HI | PMP_CS2_POL_HI | PMP_CS1_POL_HI |
             PMP_WRITE_POL_HI | PMP_READ_POL_HI),
            (PMP_MODE_MASTER1 | PMP_WAIT_BEG_4 | PMP_WAIT_MID_15 |
             PMP_WAIT_END_4),
             PMP_PEN_0, 0);
			 
/* Wait for the LCD to power up correctly. */
    vTaskDelay( lcdLONG_DELAY );
    vTaskDelay( lcdLONG_DELAY );
    vTaskDelay( lcdLONG_DELAY );

/* Set up the LCD function. */
    prvLCDCommand( LCD_FUNCTION_SET_CMD | LCD_FUNCTION_SET_8_BITS | LCD_FUNCTION_SET_2_LINES | LCD_FUNCTION_SET_LRG_FONT );
	
/* Turn the display on. */
    prvLCDCommand( LCD_DISPLAY_CTRL_CMD | LCD_DISPLAY_CTRL_DISPLAY_ON );
	
/* Clear the display. */
    prvLCDCommand( LCD_CLEAR_DISPLAY_CMD );
    vTaskDelay( lcdLONG_DELAY );
	
/* Increase the cursor. */
    prvLCDCommand( LCD_ENTRY_MODE_CMD | LCD_ENTRY_MODE_INCREASE );
    vTaskDelay( lcdLONG_DELAY );
    vTaskDelay( lcdLONG_DELAY );
    vTaskDelay( lcdLONG_DELAY );
}
/*-----------------------------------------------------------*/

static void vLCDTask(void *pvParameters)
{
xLCDMessage xMessage;
unsigned short usRow = 0;

/* Initialise the hardware.  This uses delays so must not be called prior
 * to the scheduler being started. */
    prvSetupLCD();

/* Welcome message. */
    vLCDPutString( "Cerebot MX7cK" );

    for(;;)
    {
/* Wait for a message to arrive that requires displaying. */
    while( xQueueReceive( xLCDQueue, &xMessage, portMAX_DELAY ) != pdPASS );

/* Clear the current display value. */
	    vLCDClear();

/* Switch rows each time so we can see that the display is still being
 * updated. */
        vLCDGotoRow( usRow & 0x01 );
        usRow++;
        vLCDPutString( xMessage.pcMessage );

/* Delay the requested amount of time to ensure the text just written
 * to the LCD is not overwritten. */
        vTaskDelay( xMessage.xMinDisplayTime );		
    }
}

/* End of lcd.c */