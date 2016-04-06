/******************  Stepper.c - Stepper Motor Control ***********************
 * Author:      Richard Wall
 * Date:        September 22, 2011
 * Revised:     September 3, 2013
 * Description: This file contains functions to initialize a stepper motor for
 *              PmodSTEP and RTOS tasks to control it.
 *****************************************************************************/

#ifndef __STEPPER_INC_H__
    #define __STEPPER_INC_H__
    typedef struct
    {
/* Stepper motor operating parameters */
        int Stepper_Direction;
	int Stepper_Mode;
	int Stepper_period;
    } xSTEPPER_PARAMS;

/* Stepper motor definitions */
    /* Stepper motor control bits */
    #define STEPPER_MASK	(unsigned int) (BIT_7|BIT_8|BIT_9|BIT_10)	

    #define CW 			1
    #define CCW 		-1
    #define FULLSTEP 		2
    #define HALFSTEP 		1
    #define RPM_15  		40
    #define MOTOR_OFF 		0
    #define HALF_STEPS_PER_REV  200
    #define SM_SHIFT		7   /* Shift constant for stepper motor codes */
    #define MSperREV_HS 	60000/HALF_STEPS_PER_REV
#endif

xQueueHandle xStartSTEPPER( void );
void vStepperTask( void *pvParameters );

/* End of stepper.h */
