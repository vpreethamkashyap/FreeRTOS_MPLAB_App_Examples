/*  sw_timer.c
 *
 *  Author:     Richard Wall
 *  Date:       August 11, 2013
 *  Revised:    September 6, 2013
 *              February 20, 2014 - chipKIT Pro MX7
 *
 *  Software polling millisecond and microsecond time delay using the core timer
*/

#include <plib.h>
#include "chipKIT_PRO_MX7.h"   /* Provides core clock rate */
#include "sw_timer.h"

/* DelayMs Function Description ***********************************************
 * SYNTAX:      void DelayMs(msec;
 * PARAMETER1:  msec - the number of milliseconds to delay
 * KEYWORDS:    delay, ms, milliseconds, software delay, core timer
 * DESCRIPTION: This is a millisecond delay function that will repeat a
 *              specified number of times.
 * RETURN VALUE: None
 * Notes:       None
 * END DESCRIPTION ************************************************************/

void DelayMs(unsigned int msec)
{
unsigned int tStart, tWait;
    tStart = ReadCoreTimer();
    tWait=((CORE_MS_TICK_RATE) * msec);
    while((ReadCoreTimer() - tStart) < tWait);
}/* End of DelayMs */

/* DelayMs Function Description ***********************************************
 * SYNTAX:      void DelayUs(unsigned int usec);
 * PARAMETER1:  Usec - the number of milliseconds to delay
 * KEYWORDS:    delay, ms, milliseconds, software delay, core timer
 * DESCRIPTION: This is a millisecond delay function that will repeat a
 *              specified number of times.
 * RETURN VALUE: None
 * Notes:       None
 * END DESCRIPTION ************************************************************/
void DelayUs(unsigned int usec)
{
unsigned int tStart, tWait;
    tStart = ReadCoreTimer();
    tWait=((CORE_MS_TICK_RATE) * usec);
    while((ReadCoreTimer() - tStart) < tWait);
} /* End of DelayUs */

/* End of sw_timer.c */
