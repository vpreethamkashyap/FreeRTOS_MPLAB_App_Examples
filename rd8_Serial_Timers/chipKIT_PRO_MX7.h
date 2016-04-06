/*  File name:  chipKIT PRO MX7.h
 *
 *  Author:     Richard Wall
 *  Date:       August 10, 2013
 *  Rev.        February 2, 2014
 *
*/

#ifndef __chipKIT_PRO_MX7_H__
    #define __chipKIT_PRO_MX7_H__

/* The following definitions are for IO assigned on the chipKIT PRO MX7
 * processor board */

    #define BTN1	BIT_6		/* Port G */
    #define BTN2	BIT_7		/* Port G */
    #define BTN3	BIT_0		/* Port A */

    #define LED1	BIT_12		/* Port G */
    #define LED2	BIT_13		/* Port G */
    #define LED3	BIT_14		/* Port G */
    #define LED4	BIT_15		/* Port G */
    #define BRD_LEDS	(LED1 | LED2 | LED3 | LED4)

    #define LED1_IO(a)  LATG.LATG12 = a
    #define LED2_IO(a)  LATG.LATG13 = a
    #define LED3_IO(a)  LATG.LATG14 = a
    #define LED4_IO(a)  LATG.LATG15 = a

    #define LEDA_IO(a)  LATB.LATB2 = a
    #define LEDB_IO(a)  LATB.LATB3 = a
    #define LEDC_IO(a)  LATB.LATB4 = a
    #define LEDD_IO(a)  LATB.LATB6 = a
    #define LEDE_IO(a)  LATB.LATB7 = a
    #define LEDF_IO(a)  LATB.LATB8 = a
    #define LEDG_IO(a)  LATB.LATB9 = a
    #define LEDH_IO(a)  LATB.LATB10 = a

/* The following definitions are for IO assigned for the PmodSTEP stepper
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
    #define SM_COILS    ( LEDE | LEDF | LEDG | LEDH)


/* Based upon setting in config_bits.h These directly influence timed
 * events using the Tick module.  They also are used for UART I2C, and SPI
 * baud rate generation. */

    #define XTAL	(8000000UL) /* 8 MHz Xtal on chipKIT PRO MX7 */
    #define GetSystemClock()        (80000000ul)	/* Hz */
    #define GetCoreClock()          (GetSystemClock()/2)
    #define GetPeripheralClock()    (GetSystemClock()/8)
    #define SYSTEM_FREQ GetSystemClock()

    #define CORE_MS_TICK_RATE	GetCoreClock()/1000

#endif

void chipKIT_PRO_MX7_Setup(void); /* chipKIT PRO MX7 hardware initialization */

/* End of chipKIT_PRO_MX7.h */
