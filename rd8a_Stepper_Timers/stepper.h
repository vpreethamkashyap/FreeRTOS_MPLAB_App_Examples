/******************  Stepper.h - Stepper Motor Control ***********************
 * Author:      Richard Wall
 * Date:        September 22, 2011
 * Revised:     September 3, 2013
 *              October 3, 2013 - Updated xStartSTEPPERTask 
 *
 * Description: This file contains functions to initialize a stepper motor for
 *              PmodSTEP and RTOS tasks to control it.
 *****************************************************************************/

#ifndef __STEPPER_INC_H__
    #define __STEPPER_INC_H__

/* Stepper motor operating parameters */
    typedef struct
    {
	int Stepper_Direction;
	int Stepper_Mode;
	int Stepper_period;
    } xSTEPPER_PARAMS;

/* *******  Stepper motor definitions ****** */
    #define HALF_STEPS_PER_REV	200

/* Constant defines the conversion factor from RPM to number of ms per
 * revolution. */
    #define MSperREV_HS 	60000/HALF_STEPS_PER_REV

/* Stepper motor control bits */
    #define STEPPER_MASK    (unsigned int) (BIT_7|BIT_8|BIT_9|BIT_10)
    #define CW              1
    #define CCW             -1
    #define FULLSTEP        2
    #define HALFSTEP        1
    #define MOTOR_OFF       0

/* Initial speed */
    #define RPM_15          MSperREV_HS * FULLSTEP / 15; /* = 40 ms/step */

/* Bit allignment for stepper moto coded to IO port */
    #define SM_SHIFT        7   /* Shift constant for stepper motor codes */

/*Communicates control parametes from buttons to stepper motor control */
    xQueueHandle smQueueHandle;  /* Global variable */

#endif

/* Initializes stepper motor task and queue */
portBASE_TYPE xStartSTEPPERTask( xQueueHandle *qHandle ); /* Global function */

/* End of stepper.h */

