/* ********************* sw_timer.h **********************************
 * Author:      Rihcard Wall
 * Date:        September 4, 2013
 *
 * Description: Software time delay using the core timer of the PIC32
 *              processor.
 *********************************************************************/
#ifndef __SW_TIMER
    #define __SW_TIMER
#endif

void DelayMs(unsigned int msec);
void DelayUs(unsigned int usec);

/* Endo of sw_timer.h */
