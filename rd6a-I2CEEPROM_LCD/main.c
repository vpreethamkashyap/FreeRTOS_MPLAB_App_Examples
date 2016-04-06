/* ****************** Reference Design 6a ********************************
 * Project:     RD6a
 * File name:   main.c
 * Author:      Richard Wall
 * Date:        October 24, 2011
 * Revised      December 22, 2012
 * Revised      September 14, 2013
 * Revised      September 30, 2013  - Corrected to I2C Channel 2
 *              February 20, 2014 - chipKIT Pro MX7
 *
 * Description: An example of FreeRTOS running an the chipKIT Pro MX7 using a
 *              PIC32MX7 processor. The purpose of this code is to program a I2C
 *              EEPROM with 1024 bytes of randomly generated data starting at a
 *              random address and verify that the data was correctly saved.
 *              The test sequence is initiated whenever BTN1 is pressed. Access
 *              to the LCD and EEPROM are protected by mutex semaphores.
 *              
 *              See EEPROM_TEST.txt for functional details.
 *
 * Note:        "eeprom" is a keyword in Microchip XC32 and refers to a
 *              data type. Make certain to avoid using this keyword unless
 *              doing so in the context in which it is intended. See Ch 2.5.9
 *              http://ww1.microchip.com/downloads/en/DeviceDoc/51686F.pdf
 *
 *              LEDH is used for idle task timing only. LEDA is used to time
 *              the data generation task. LEDB is used Used to time EEPROM
 *              write operation. LEDC is used to time the read memory task.
 *
 ****************************************************************************/

/* XC32 System Files */
#include <plib.h>
#include <stdio.h>
#include <string.h>

/* Files to support FreeRTOS */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

/* Files to support Application  */
#include "main.h"
#include "chipKIT_PRO_MX7.h"
#include "FreeRTOS_Common_Tasks.h"
#include "lcddrv.h"
#include "EEPROM_I2C.h"
#include "sw_timer.h"

// #define EEPROM_ADDRESS 	(BYTE) 0x50

/* The priorities of the various application tasks. */
#define mainCHECK_TASK_PRIORITY	( tskIDLE_PRIORITY + 4 )

/* queue to pass counter back and forth */
xQueueHandle QTask_EEPROM_W2R, QTask_EEPROM_R2W; 	
xQueueHandle xQ_Button;
xSemaphoreHandle xLCD_semaphore;    /* Protect access to LCD resource */
xSemaphoreHandle xI2C_semaphore;    /* Protect access to EEPROM resource */

/* Implements the task functionality as described at the top of this file */
static void prvData_Gen( void *pvParameters ) __attribute__((noreturn));
static void prvData_Chk( void *pvParameters ) __attribute__((noreturn));
static void prvButtons( void *pvParameters ) __attribute__((noreturn));

/* main Function Description *************************************************
 * SYNTAX:          int main( void );
 * KEYWORDS:        Initialize, create, tasks, scheduler
 * DESCRIPTION:     This is a typical RTOS setup function. Hardware is
 *                  initialized, tasks are created, and the scheduler is
 *                  started.
 * PARAMETERS:      None
 * RETURN VALUE:    Exit code - used for error handling
 * NOTES:           None
 * END DESCRIPTION *****************************************************/
int main( void )
{
/* Flag used by to indicate the task create status (pass/fail) */
unsigned long ulStatus1 = pdPASS;

/* Configure any hardware required for this demo.  */
    vSetupHardware();

    initLCD();
    initI2C(I2C2);

/* Beware  - prvCheckTask uses sprintf so could require more stack. */
    ulStatus1 &= xTaskCreate( prvData_Gen, (signed char *) "GEN_DATA",
                              configMINIMAL_STACK_SIZE, NULL,
                              mainCHECK_TASK_PRIORITY, NULL );
    ulStatus1 &= xTaskCreate( prvData_Chk, (signed char *) "CHK_DATA",
                              configMINIMAL_STACK_SIZE, NULL,
                              mainCHECK_TASK_PRIORITY, NULL );
    ulStatus1 &= xTaskCreate( prvButtons, (signed char *) "BTN1",
                              configMINIMAL_STACK_SIZE, NULL,
                              tskIDLE_PRIORITY, NULL );

/* Semaphores guard access to I2C and LCD */
    xLCD_semaphore = xSemaphoreCreateMutex();
    xI2C_semaphore = xSemaphoreCreateMutex();

/* Continue only id semaphores successfully created */
    if(xLCD_semaphore != NULL && xI2C_semaphore != NULL && ulStatus1 == pdPASS )
    {
/* Create buffer for sending block of data to store in EEPROM */
        QTask_EEPROM_W2R = xQueueCreate(1, sizeof(EEPROM_packet));

/* create buffer for LCD message*/
	QTask_EEPROM_R2W = xQueueCreate(1, LCD_MSG_SIZE);

/* create buffer for button message */
	xQ_Button = xQueueCreate(1, sizeof(int));

/* Finally, if everything is ready to run,  start the scheduler. */
	if((QTask_EEPROM_R2W != NULL) &&
           (QTask_EEPROM_W2R != NULL) &&
           (xQ_Button != NULL))
	{
            vTaskStartScheduler();
	}
    }
/* Will only reach here if there is insufficient heap available to start
 * the scheduler. */
    return 1;
}

/* prvData_Gen Function Description ***************************************
 * SYNTAX:          static prvData_Gen( void *pvParameters );
 * KEYWORDS:        Task, EEPROM, Generate, data
 * DESCRIPTION:     This task generates data set of random BYTE values and
 *                  write to a random address in the EEPROM
 * PARAMETERS:      the void pointer contains no information for this application
 * RETURN VALUE:    None
 * NOTES:           The task is blocked until queue from the Button press
 *                  generates a queue message.
 *
 *                  LEDH is used for idle task timing only. LEDA is used to time
 *                  the data generation task. LEDB is used Used to time EEPROM
 *                  write operation.
  * END DESCRIPTION *****************************************************/
static void prvData_Gen( void *pvParameters )
{
char msg[32]; 
int signal;
EEPROM_packet ee_prom;
I2C_RESULT i2c_result = FALSE;
static char data_set[MEM_BLK_DATA_SIZE];
int mem_address;
int i, len;
unsigned int x;
    while( 1 )
    {
/* Wait for button pressSpec 2.a. */
	xQueueReceive(xQ_Button, &signal, portMAX_DELAY); 	
        PORTClearBits(IOPORT_B, LEDH);  /* Used for idle task timing only */

/* Set LED B - Spec 2.b. */
        PORTSetBits(IOPORT_B, LEDB);
	len = MEM_BLK_DATA_SIZE;

/* Generate random data - Spec 2.c. */
	srand((unsigned int)ReadCoreTimer());	
	for(i=0; i< len; i++)
	{
            x = rand();
            if(x < 0)
            {
		x = -x;
            }
            data_set[i] = (unsigned char) (x % 256);
        }

/* Generate random address - Spec 2.c. */
        mem_address = rand() % 32768;

/* Set LED C - Spec 2.d. */
        PORTSetBits(IOPORT_B, LEDC);   /* Used to time EEPROM write operation */
/*  Wait for EEPROM mutex and then take it - Spec 2.e.  */
        xSemaphoreTake(xI2C_semaphore, portMAX_DELAY);  // lock up EEPROM

/*  Write EEPROM data - Spec 2.f.  */
        PORTClearBits(IOPORT_B, LEDH); /* Used for idle task timing only */
        i2c_result = I2CWrtiteEEPROM(EEPROM_I2C_BUS, EEPROM_ADDRESS,
                                    mem_address, (BYTE *) data_set, len);
/* Clear LED C  - Spec 2.g. */
        PORTClearBits(IOPORT_B, LEDC); /* Used to time EEPROM write operation */
/* Give EEPROM mutes semaphore - Spec 2.h. */
        xSemaphoreGive(xI2C_semaphore); // unlock eeprom
        PORTClearBits(IOPORT_B, LEDH); /* Used for idle task timing only */

/* Send array data to Data Check task - Spec 2.f. */
/* Program data structure */
        ee_prom.mem_addr = mem_address;
        ee_prom.num_bytes = len;
        ee_prom.data_ptr = (char *) data_set;
/* Send check packet to EEPROM Read task Spec 2.h. */
        xQueueSend(QTask_EEPROM_W2R, &ee_prom, portMAX_DELAY);
        PORTClearBits(IOPORT_B, LEDH); /* Used for idle task timing only */

/* Clear LED B  - Spec 2.i. */
        PORTClearBits(IOPORT_B, LEDB); /* Used for idle task timing only */

/* Wait for results from check task - Spec 2.j. */
        xQueueReceive(QTask_EEPROM_R2W, msg, portMAX_DELAY);
        PORTClearBits(IOPORT_B, LEDH); /* Used for idle task timing only */

/* Set LED B  - Spec 2.k. */
        PORTSetBits(IOPORT_B, LEDB); /* Used for idle task timing only */

/* Wait for LCD mutex to be taken- Spec 2.l. */
        xSemaphoreTake(xLCD_semaphore, portMAX_DELAY);
        PORTClearBits(IOPORT_B, LEDH); /* Used for idle task timing only */

/* Write report message to LCD - Spec 2.m. */
        putsLCD(msg);

/* Give back LCD mutex semaphore  - Spec 2.n. */
        xSemaphoreGive(xLCD_semaphore);
        PORTClearBits(IOPORT_B, LEDH); /* Used for idle task timing only */

/* Clear LEDB  - Spec 2.o. */
        PORTClearBits(IOPORT_B, LEDB); /* Used for idle task timing only */
    }
}

/* prvData_Chk Function Description ***************************************
 * SYNTAX:          static void prvData_Chk( void *pvParameters );
 * KEYWORDS:        Task
 * DESCRIPTION:     This task reads the EEPROM and compares it against the
 *                  data sent from the data generator task
 * PARAMETERS:      The void pointer contains no information for this
 *                  application
 * RETURN VALUE:    None
 * NOTES:           LEDH is used for idle task timing only. LEDC is used to time
 *                  the read memory task.
 * END DESCRIPTION *****************************************************/
static void prvData_Chk( void *pvParameters )
{
I2C_RESULT i2c_result = FALSE;
static BYTE data_rd[MEM_BLK_DATA_SIZE];
EEPROM_packet ee_prom;
int equal_flag;
char msg[32], report[10]; 

    for( ;; )
    {
/* Waits for message from data generator - Spec 3.a */
        xQueueReceive(QTask_EEPROM_W2R, &ee_prom, portMAX_DELAY);
        PORTClearBits(IOPORT_B, LEDH); /* Used for idle task timing only */

/* Turns on LCD D - Spec 3.b. */
        PORTSetBits(IOPORT_B, LEDD);  /* LEDD is used to time the read memory task */

/* Waits to take LCD mutex semaphore - Spec 3.c. */
        xSemaphoreTake(xLCD_semaphore, portMAX_DELAY);
        PORTClearBits(IOPORT_B, LEDH); /* Used for idle task timing only */

/* Writes starting address to LCD - Spec 3.d. */
        sprintf(msg,"\f0X%04x - %5d", ee_prom.mem_addr, ee_prom.num_bytes );
        putsLCD(msg);
/* Turns off LCD D - Spec 3.e. */
        PORTClearBits(IOPORT_B, LEDD); /* Used for idle task timing only */

/* Gives back LCD mutex semaphore - Spec 3.f. */
        xSemaphoreGive(xLCD_semaphore);
        PORTClearBits(IOPORT_B, LEDH); /* Used for idle task timing only */

/* Set LED E on - Spec 3.g. */
        PORTSetBits(IOPORT_B, LEDE);  /* LEDC is used to time the read memory task */

/* Waits to take the EEPROM mutex semaphore - Spec 3.h. */
        xSemaphoreTake(xI2C_semaphore, portMAX_DELAY);
        PORTClearBits(IOPORT_B, LEDH); /* Used for idle task timing only */

/* Reads EEPROM data - Spec 3.i. */
        i2c_result = I2CReadEEPROM(EEPROM_I2C_BUS, EEPROM_ADDRESS,  ee_prom.mem_addr,
                                    data_rd, ee_prom.num_bytes);
/* Gives back EEPRORM mutex semaphore - Spec 3.j. */
        xSemaphoreGive(xI2C_semaphore);
        PORTClearBits(IOPORT_B, LEDH); /* Used for idle task timing only */

/* Set LED E off - Spec 3.k. */
        PORTClearBits(IOPORT_B, LEDE);

/* Compares EEPROM read data with data sent form data generator - Spec 3.l. */
        if(i2c_result)
        {
            sprintf(msg,"I2C err");
        }
        else
        {
            equal_flag = memcmp(ee_prom.data_ptr, data_rd,  ee_prom.num_bytes);
            if(equal_flag != 0)
                sprintf(report,"FAILED");
            else
                sprintf(report,"PASSED");
        }
/* Turns on LED D - Spec 3.m. */
        PORTSetBits(IOPORT_B, LEDD);  /* LEDD is used to time the LCD write */

/* Waits to take LCD mutex semaphore - Spec 3.n. */
        xSemaphoreTake(xLCD_semaphore, portMAX_DELAY);
        PORTClearBits(IOPORT_B, LEDH); /* Used for idle task timing only */

/* Writes results to LCD - Spec 3.o */
        LCDGotoRow(1);
        putsLCD("Verify - ");

/* Turns on LCD D - Spec 3.p. */
        PORTClearBits(IOPORT_B, LEDD);  /* LEDD is used to time the LCD write */

/* Gives back LCD mutex semaphore - Spec 3.q. */
        xSemaphoreGive(xLCD_semaphore);
        PORTClearBits(IOPORT_B, LEDH); /* Used for idle task timing only */

/* The PASSED or FAILED is actually written by the data generator task. Spec 3.r */
        xQueueSend(QTask_EEPROM_R2W, report, portMAX_DELAY);
        PORTClearBits(IOPORT_B, LEDH); /* Used for idle task timing only */
    }
}

/* prvButtons FUNCTION DESCRIPTION ********************************************
 * SYNTAX:          static void prvButtons( void *pvParameters );
 * KEYWORDS:        Change notice, button status, change, decode
 * DESCRIPTION:     Detect an LED1 turn on (controlled by BTN1) and sends a
 *                  message to the data generator
 * PARAMETER 1:     void pointer - data of unspecified data type sent from
 *                  RTOS scheduler - not used
 * RETURN VALUE:    none
 * END DESCRIPTION **********************************************************/
static void prvButtons( void *pvParameters )
{
int buttons;

/* Setup button change detection */
    mCNOpen(CN_ON, (CN8_ENABLE), CN_PULLUP_DISABLE_ALL);
    buttons = PORTRead(IOPORT_G) & BTN1;
    for( ;; )
    {
/* Wait for button Press - Spec 1.a. */
        while(!INTGetFlag(INT_CN));
	{
            vTaskDelay(100/portTICK_RATE_MS); /* button sample rate */
	}
	vTaskDelay(50/portTICK_RATE_MS); /*  button de-bounce */
        buttons = PORTRead(IOPORT_G) & BTN1;

/* Button operates in PUSH-ON / PUSH-OFF fashion - Spec 1.a.i.1 */
        if(buttons)
	{		
/* Toggle LED1	- Spec 1.c. */
            PORTToggleBits(IOPORT_G, LED1);
/* Set LEDA	- Spec 1.d. */
            PORTSetBits(IOPORT_B, LEDA);
/* Send message to GEN_DATA task - Spec 1.e. */
            xQueueSend(xQ_Button, &buttons, portMAX_DELAY);
            INTClearFlag(INT_CN);
/* Clear LEDA	- Spec 1.f. */
            PORTClearBits(IOPORT_B, LEDA);
	}
    }
} /* End of prvButtons */

/*--------------------------End of main for RD6a ----------------------------*/

