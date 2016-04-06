/* LCD Driver Header
 * File name: LCDdrv.h
 *
 * Author:      Richard Wall
 * Date:        October 6, 2011
 * Revised:     September 23, 2013
 *
 * Description: This character LCD driver is designed to interface with a PIC32
 *              processor using the PMP bus.
*/

#ifndef __LCDdrv__
    #define __LCDdrv__

/* Brief delay to permit the LCD to catch up with commands. */
    #define lcdVERY_SHORT_DELAY                 ( 1 ) /* ms */
    #define lcdSHORT_DELAY                      ( 4 ) /* ms */
    #define lcdLONG_DELAY                       ( 15) /* ms */

/* LCD specific definitions. */
    #define LCD_CLEAR_DISPLAY_CMD		0x01
    #define LCD_CURSOR_HOME_CMD			0x02
    #define LCD_ENTRY_MODE_CMD			0x04
    #define LCD_ENTRY_MODE_INCREASE		0x02
    #define LCD_DISPLAY_CTRL_CMD		0x08
    #define LCD_DISPLAY_CTRL_DISPLAY_ON		0x07
    #define LCD_FUNCTION_SET_CMD		0x20
    #define LCD_FUNCTION_SET_8_BITS		0x10
    #define LCD_FUNCTION_SET_2_LINES		0x08
    #define LCD_FUNCTION_SET_LRG_FONT		0x04
    #define LCD_NEW_LINE			0xC0
    #define LCD_COMMAND_ADDRESS			0x00
    #define LCD_DATA_ADDRESS			0x01
    #define LCD_BUSY				0x80
    #define LCD_FF				0x12

#endif

void initLCD(void);
void putsLCD(char *pcString);
void putcLCD( char ch );
void LCDData( char cChar );	
void LCDCommand(char cmd);
void LCDGotoRow(char Row);
char LCDGetAddress(void);

/* End of LCDdrv.h */


