/*  sw_timer.c
 *
 *  Author:     Richard Wall
 *  Date:       August 11, 2013
 *  Revised:    September 9, 2013
 *
 *  Software polling millisecond and microsecond time delay using the core timer
*/

#include <plib.h>
#include "CerebotMX7cK.h"   /* Provides core clock rate */
#include "sw_timer.h"

/* DelayMs Function Description ***********************************************
 * SYNTAX:		void DelayMs(msec;
 * PARAMETER1:		msec - the number of milliseconds to delay
 * KEYWORDS:		delay, ms, milliseconds, software delay, core timer
 * DESCRIPTION:         This is a millisecond delay function that will repeat a
 * 			specified number of times.
 * RETURN VALUE:	None
 * Notes:		Maximum possible delay is 49.71 days
 * END DESCRIPTION ************************************************************/

void DelayMs(unsigned int msec)
{
unsigned int tStart, tWait;
    tStart = ReadCoreTimer();
    tWait=((CORE_MS_TICK_RATE) * msec);
    while((ReadCoreTimer() - tStart) < tWait);
}/* End of DelayMs */

/* DelayMs Function Description ***********************************************
 * SYNTAX:		void DelayUs(unsigned int usec);
 * PARAMETER1:		Usec - the number of milliseconds to delay
 * KEYWORDS:		delay, ms, microseconds, software delay, core timer
 * DESCRIPTION:         This is a microsecond delay function that will delay
 * 			the total time in while loop.
 * RETURN VALUE:	None
 * Notes:		CORE_MS_TICK_RATE/1000 yeilds the number of core timer 
 *                      ticks per microsecond. Total possible delay is 107.37
 *                      seconds.
 * END DESCRIPTION ************************************************************/
void DelayUs(unsigned int usec)
{
unsigned int tStart, tWait;
    tStart = ReadCoreTimer();
    tWait=((CORE_MS_TICK_RATE/1000) * usec);
    while((ReadCoreTimer() - tStart) < tWait);
} /* End of DelayUs */

/* End of sw_timer.c */
