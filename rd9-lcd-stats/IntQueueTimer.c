/******************  IntQueueTimer.c - Timer ISR  ***********************
 * Author:      Richard Wall
 * Date:        September 22, 2011
 * Revised:     September 3, 2013
 *              September 22, 2013  Uses XC32 peripheral library functions
 *              October 5, 2014 - chipKIT Pro MX7, editorial corrections 
 *
 * Description: Initializes Timers 3 and 4 for periodic interrupts.  Timer 3
 *              only toggles LEDC in this application.  Timer 4 provides a high
 *              frequency counter for reporting task statistics.
 */
#include <plib.h>

/* Scheduler include files. */
#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "queue.h"
#include "semphr.h"
#include "IntQueue.h"

/* Application include files. */
#include "chipKIT_Pro_MX7.h"
#include "IntQueueTimer.h"
//#include "stepper.h"

/* vInitialiseTimerForIntQueue Function Description ************************************
 * SYNTAX:		void vInitialiseTimerForIntQueue( void );
 * KEYWORDS:    Timer, ISR, Interrupt Handler
 * DESCRIPTION: Initializes Timer 3 and Timer 4
 * PARAMETERS:  None
 * RETURN VALUE: None
 * NOTES:       The Timer interrupt ISR calls this function. Timer 1
 *              is used for the tick interrupt, timer 2 is used for the
 *              high frequency interrupt test.  This file therefore
 *              uses timers 3 and 4.
 * END DESCRIPTION *****************************************************/
void vInitialiseTimerForIntQueue( void )
{
    CloseTimer3();              /*Stop Timer 3 */
/* Clear the interrupt as a starting condition. */
    INTClearFlag(INT_T3);
/* Setup timer 3 interrupt priority to be above the kernel priority. */
    ConfigIntTimer3( T3_INT_ON | ( configMAX_SYSCALL_INTERRUPT_PRIORITY - 1 ) );
/* Start Timer 3 */
    OpenTimer3(T3_ON | T3_SOURCE_INT | T3_PS_1_1,\
            ( unsigned short ) ( configPERIPHERAL_CLOCK_HZ\
            / timerINTERRUPT3_FREQUENCY ) - 1);

    CloseTimer4();              /*Stop Timer 4 */
/* Clear the interrupt as a starting condition. */
    INTClearFlag(INT_T4);
/* Setup timer 4 interrupt priority to be above the kernel priority. */
    ConfigIntTimer4( T4_INT_ON | ( configMAX_SYSCALL_INTERRUPT_PRIORITY ) );
/* Start Timer 4. */
    OpenTimer4(T4_ON | T4_SOURCE_INT | T4_PS_1_1,\
            ( unsigned short ) ( configPERIPHERAL_CLOCK_HZ\
            / timerINTERRUPT4_FREQUENCY ) - 1);
}

/* vT3InterruptHandler Function Description ****************************
 * SYNTAX:      void vT3InterruptHandler( void );
 * KEYWORDS:    Timer, ISR, Interrupt Handler
 * DESCRIPTION: Toggles LEDC
 * PARAMETERS:  None
 * RETURN VALUE: None
 * NOTES:       The Timer interrupt ISR calls this function
 * END DESCRIPTION *****************************************************/
void __ISR(_TIMER_3_VECTOR, ipl2) vT3InterruptHandler(void)
{
portBASE_TYPE waiting = pdFALSE;   /* See reference FreeRTOS manual */
    LATBINV = LEDC;
    INTClearFlag(INT_T3);
//  xSemaphoreGiveFromISR(take_step, &waiting);
    portEND_SWITCHING_ISR( waiting );
} /* End of vT3InterruptHandler */

/* vT4InterruptHandler Function Description ************************************
 * SYNTAX:      void vT4InterruptHandler( void );
 * KEYWORDS:    Timer, ISR, Interrupt Handler
 * DESCRIPTION: Increments the timer tick for measuring real time
 *              performance
 * PARAMETERS:  None
 * RETURN VALUE: None
 * NOTES:       The Timer interrupt ISR calls this function
 * END DESCRIPTION *****************************************************/
void __ISR(_TIMER_4_VECTOR, ipl3) vT4InterruptHandler( void )
{
portBASE_TYPE waiting = pdFALSE;   // See reference FreeRTOS manual
    LATBINV = LEDD;
    ++ulHighFrequencyTimerTicks;
    INTClearFlag(INT_T4);
    portEND_SWITCHING_ISR( waiting );
} /* End of vT4InterruptHandler */

/* End of IntQueueTimer.c */