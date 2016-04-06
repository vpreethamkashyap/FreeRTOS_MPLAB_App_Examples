/*  File name:  CerebotMX7cK.h
 *
 *  Author:     Richard Wall
 *  Date:       August 10, 2013
 *
*/

#ifndef __CEREBOT_MX7ck_H__
    #define __CEREBOT_MX7ck_H__

/* The following definitioan are for IO assigned on the Cerebot MX7cK
 * processor board */

    #define BTN1	BIT_6		/* Port G */
    #define BTN2	BIT_7		/* Port G */
    #define BTN3	BIT_0		/* Port A */

    #define LED1	BIT_12		/* Port G */
    #define LED2	BIT_13		/* Port G */
    #define LED3	BIT_14		/* Port G */
    #define LED4	BIT_15		/* Port G */
    #define BRD_LEDS	(LED1 | LED2 | LED3 | LED4)

/* The following definitioan are for IO assigned for the PmodSTEP stepper
 * motor driver board that is commonly used for instrumentation and output
 * indication */

    #define LEDA	BIT_2		/*  Port B */
    #define LEDB	BIT_3		/*  Port B */
    #define LEDC	BIT_4		/*  Port B */
    #define LEDD	BIT_6		/*  Port B */
    #define LEDE	BIT_7		/*  Port B */
    #define LEDF	BIT_8		/*  Port B */
    #define LEDG	BIT_9		/*  Port B */
    #define LEDH	BIT_10		/*  Port B */
    #define SM1		LEDE
    #define SM2		LEDF
    #define SM3		LEDG
    #define SM4		LEDH

    #define SM_LEDS	(LEDA | LEDB | LEDC | LEDD | LEDE | LEDF | LEDG | LEDH)



/* Based upon setting in config_bits.h These directly influence timed
 * events using the Tick module.  They also are used for UART I2C, and SPI
 * baud rate generation. */

    #define GetSystemClock()		(80000000ul)	/* Hz */
    #define GetCoreClock()              (GetSystemClock()/2)
    #define GetPeripheralClock()	(GetSystemClock()/8)	

    #define XTAL	(8000000UL)         /* 8 MHz Xtal on Cerebot MX7cK */
    #define SYS_FREQ    GetSystemClock()    /* System clock rate */
    #define SYSTEM_FREQ GetSystemClock()
    #define CORE_OSC 	GetInstructionClock()  /* Core clock rate */
    #define FPB		GetPeripheralClock()   /* Peripheral bus clock rate */
    #define CORE_MS_TICK_RATE	GetCoreClock()/1000

/* Timer 1 definitions */
    #define PRESCALE            256
    #define TOGGLES_PER_SEC     1000
    #define T1_TICK             (FPB/PRESCALE/TOGGLES_PER_SEC)

#endif

void Cerebot_mx7cK_setup(void);     /* Cerebot MX7cK hardware initialization */

/* End of CerebotMX7cK.h */
