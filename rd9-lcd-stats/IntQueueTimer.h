#ifndef __INTQUEUETIMER_H__
    #define  __INTQUEUETIMER_H__

    #include "FreeRTOSConfig.h"

    #define timerINTERRUPT3_FREQUENCY 1000UL
    #define timerINTERRUPT4_FREQUENCY 10000UL

    void vInitialiseTimerForIntQueue( void );
//    void vT3InterruptHandler( void );
//    void vT4InterruptHandler( void );

    volatile unsigned long ulHighFrequencyTimerTicks;

#endif
