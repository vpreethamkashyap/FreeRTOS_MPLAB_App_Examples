/***************************************************************************
 * File name:       LCD.h
 * Author:          Richard Wall
 * Date:            September 13, 2013
 *
 * Description: This file provides character and string writing to a 16X2
 *              character LCD.
 ***************************************************************************/


#ifndef __LCD_H__
    #define __LCD_H__

/* Brief delay to permit the LCD to catch up with commands. */
    #define lcdVERY_SHORT_DELAY	( 1 )
    #define lcdSHORT_DELAY	( 4 / portTICK_RATE_MS )
    #define lcdLONG_DELAY	( 15 / portTICK_RATE_MS )

/* LCD specific definitions. */
    #define LCD_RW
    #define LCD_RS
    #define LCD_CLEAR_DISPLAY_CMD       0x01
    #define LCD_CURSOR_HOME_CMD         0x02
    #define LCD_ENTRY_MODE_CMD          0x04
    #define LCD_ENTRY_MODE_INCREASE     0x02
    #define LCD_DISPLAY_CTRL_CMD        0x08
    #define LCD_DISPLAY_CTRL_DISPLAY_ON	0x04
    #define LCD_FUNCTION_SET_CMD        0x20
    #define LCD_FUNCTION_SET_8_BITS     0x10
    #define LCD_FUNCTION_SET_2_LINES    0x08
    #define LCD_FUNCTION_SET_LRG_FONT   0x04
    #define LCD_NEW_LINE                0xC0
    #define LCD_COMMAND_ADDRESS         0x00
    #define LCD_DATA_ADDRESS            0x01

    #define lcdQUEUE_SIZE               5


/* Create the task that will control the LCD.  Returned is a handle to the queue
on which messages to get written to the LCD should be written. */

    typedef struct
    {
/* The minimum amount of time the message should remain on the LCD without
 * being overwritten. */
        portTickType xMinDisplayTime;

/* A pointer to the string to be displayed. */
        char *pcMessage;

    } xLCDMessage;
#endif /* __LCD_H__ */

/* Task prototypes */
portBASE_TYPE xStartLCDTask(xQueueHandle *xLCD_Queue ); /* Setup LCD task */

/* Move to the first (0) or second (1) row of the LCD. */
void vLCDGotoRow( unsigned short usRow );

void vLCDPutString( char *pcString ); /* Write a string of text to the LCD. */
void vLCDClear( void ); /* Clear the LCD.  */

/* End of lcd.h */
