/* LCD Driver
 * File name: LCDdrv.c
 *
 * Author:      Richard Wall
 * Date:        October 6, 2011
 * Revised:     September 23, 2013
 *
 * Description: This character LCD driver is designed to interface with a PIC32
 *              processor using the PMP bus.
*/
#include <plib.h>
#include "CerebotMX7cK.h"
#include "LCDdrv.h"
#include "sw_timer.h"

/* initLCD Function Description ******************************************
 * SYNTAX:          void initLCD(void);
 * KEYWORDS:        LCD, initialize
 * DESCRIPTION:     Initializes character LCD module
 * PARAMETERS:      None
 * RETURN VALUE:    None
 * NOTES:           This interface is implemented on an chipKIT Pro MX7 
 *                  processor board and uses the PIC32 PMP bus.
 * END DESCRIPTION *****************************************************/
void initLCD(void)
{
/* Wait for proper power up. */
    DelayMs( lcdLONG_DELAY );
	
/* Open the PMP port */
    mPMPOpen((PMP_ON | PMP_READ_WRITE_EN | PMP_CS2_CS1_EN |
        	  PMP_LATCH_POL_HI | PMP_CS2_POL_HI | PMP_CS1_POL_HI |
		  PMP_WRITE_POL_HI | PMP_READ_POL_HI),
		 (PMP_MODE_MASTER1 | PMP_WAIT_BEG_4 | PMP_WAIT_MID_15 |
		  PMP_WAIT_END_4),
		  PMP_PEN_0, 0);
			 
/* Wait for the LCD to power up correctly. */
    DelayMs( 3*lcdLONG_DELAY );

/* Set up the LCD function. */
    LCDCommand( LCD_FUNCTION_SET_CMD | LCD_FUNCTION_SET_8_BITS | 
	            LCD_FUNCTION_SET_2_LINES | LCD_FUNCTION_SET_LRG_FONT );
	
/* Turn the display on. */
    LCDCommand( LCD_DISPLAY_CTRL_CMD | LCD_DISPLAY_CTRL_DISPLAY_ON );
	
/* Clear the display. */
    LCDCommand( LCD_CLEAR_DISPLAY_CMD );
    DelayMs( lcdLONG_DELAY );
	
/* Increase the cursor. */
    LCDCommand( LCD_ENTRY_MODE_CMD | LCD_ENTRY_MODE_INCREASE );
    DelayMs( 3*lcdLONG_DELAY );
}

/* putsLCD Function Description ******************************************
 * SYNTAX:          void putsLCD(char *pcString);
 * KEYWORDS:        String, LCD, display, write
 * DESCRIPTION:     Sends one byte at a time to the putcLCD function until the
 *                  string terminating NULL byte is found.
 * PARAMETERS:      char pointer - to text string
 * RETURN VALUE:    None
 * NOTES:           Short delay is inserted after each character is sent
 * END DESCRIPTION *****************************************************/
void putsLCD(char *pcString)
{
    while(*pcString)
    {
        putcLCD(*pcString);
        DelayMs(lcdSHORT_DELAY);
        pcString++;
    }
} /* End of putsLCD*/
	
/* putcLCD Function Description ***********************************************
 * SYNTAX:          void putcLCD(char ch);
 * KEYWORDS:        Character, LCD, display, write
 * DESCRIPTION:     Sends one character to the LCD. ASCII control characters
 *                  LF (new line) puts the cursor at the start of second line
 *                  CR (Carriage return) places cursor at the start of 1st line
 *                  FF (form feed) Clears the display and homes cursor
 *                  TAB Moves cursor to the right 8 positions
 *                  End of line wrap around is implemented
 * PARAMETERS:      char - ASCII text or control character
 * RETURN VALUE:    None
 * NOTES:           None
 * END DESCRIPTION ***********************************************************/
void putcLCD(char ch)
{
	/* Write out each character with appropriate delay between each. */
char c;
    
    switch (ch)
    {
        case '\n':          // point to second line
            LCDGotoRow(1);
            break;
        case '\r':          // home, point to first line
            LCDGotoRow( 0);
            break;
        case '\f':          // home, point to first line
            LCDCommand(LCD_CLEAR_DISPLAY_CMD);
            break;
        case '\t':          // advance next tab (8) positions
            c = LCDGetAddress();
            while( c & 7)
            {
		LCDData(' ');
		DelayMs(lcdSHORT_DELAY);
                c++;
            }
            if ( c > 15)   // if necessary move to second line
            {
                LCDCommand( 0x40);
        	DelayMs(lcdLONG_DELAY);
            }
            break;                
        default:            // print character
            LCDData(ch);
            break;
	} //switch
} /* End of putcLECD */

/* LCDData Function Description ***********************************************
 * SYNTAX:          void LCDData( char cChar );
 * KEYWORDS:        PMP Bus single character write, LCD DDRAM, CGRAM
 * DESCRIPTION:     Send 8 bit data to PMP bus with the LCD RS line high
 * PARAMETERS:      char - data
 * RETURN VALUE:    None
 * NOTES:           None
 * END DESCRIPTION ***********************************************************/
void LCDData( char cChar )
{
    PMPSetAddress( LCD_DATA_ADDRESS );
    PMPMasterWrite( cChar );
    DelayMs( lcdVERY_SHORT_DELAY );
} /* End of LCDData */

/* LCDCommand Function Description *******************************************
 * SYNTAX:          void LCDCommand(char cmd);
 * KEYWORDS:        PMP Bus single character write, LCD control
 * DESCRIPTION:     Send 8 bit data to PMP bus with the LCD RS line low
 * PARAMETERS:      char - data
 * RETURN VALUE:    None
 * NOTES:           None
 * END DESCRIPTION **********************************************************/
void LCDCommand(char cmd)
{
    PMPSetAddress( LCD_COMMAND_ADDRESS );
    PMPMasterWrite( cmd );
    DelayMs( lcdSHORT_DELAY );
} /* End of LCDCommand */

/* LCDGotoRow Function Description ******************************************
 * SYNTAX:          void LCDGotoRow(char Row);
 * KEYWORDS:        LCD, cursor, control
 * DESCRIPTION:     Positions the LCD cursor to the start of row 0 or 1
 * PARAMETERS:      char - row number
 * RETURN VALUE:    None
 * NOTES:           None
 * END DESCRIPTION **********************************************************/
void LCDGotoRow(char Row)
{
    if(Row == 0)
    {
        LCDCommand( LCD_CURSOR_HOME_CMD );
    }
    else
    {
	LCDCommand( LCD_NEW_LINE );
    }
    DelayMs(lcdLONG_DELAY);
}/* End of LCDGotoRow */

/* LCDGetAddress Function Description *****************************************
 * SYNTAX:          char LCDGetAddress(void);
 * KEYWORDS:        LCD, cursor, control
 * DESCRIPTION:     Returns the DDRAM or CGRAM address
 * PARAMETERS:      None
 * RETURN VALUE:    char - RAM Address
 * NOTES:           This function will wait for the busy flag to be zero.
 * END DESCRIPTION ***********************************************************/
char LCDGetAddress(void)
{
int LCDaddress;
    PMPSetAddress( LCD_COMMAND_ADDRESS );
    do
    {
	LCDaddress = PMPMasterRead ();
	DelayMs( lcdSHORT_DELAY  );
    } while (LCDaddress & LCD_BUSY);
    return LCDaddress;
} /* End of LCDGetAddress*/

/*End of LCDdrv.c */
