/* 
 * File:   FreeRTOS_Common_Tasks.h
 * Author: Richard Wall
 *
 * Created on September 9, 2013, 11:51 AM
 *
 * Notes:   See FreeRTOS_Common_Tasks.c
 *
 */

#ifndef __FREERTOS_COMMON_TASKS_H__
    #define	__FREERTOS_COMMON_TASKS_H__

#endif	/* __FREERTOS_COMMON_TASKS_H__ */

void vSetupHardware( void );
void vApplicationIdleHook( void );
void vApplicationStackOverflowHook( void );
void _general_exception_handler( unsigned long ulCause, unsigned long ulStatus);

/* End of FreeRTOS_Common_Tasks */