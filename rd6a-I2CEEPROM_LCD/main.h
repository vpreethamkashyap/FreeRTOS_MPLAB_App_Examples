/* ****************** Reference Design 6a ********************************
 * File name:   RD6a.h
 * Author:      Richard Wall
 * Date:        October 24, 2011
 * Revised	December 22, 2012
 * Revised      September 5, 2013
 *
 * Description: An example of FreeRTOS running an the
 *              chipKIT Pro MX7 using a PIC32MX7 processor.
 *              The purpose of this code is to program a I2C
 *              EEPROM with 1024 bytes of randomly generated
 *              data starting at a random address and verify
 *              that the data was correctly saved. See EEPROM_TEST.txt
 *              for functional details. Access to the LCD and
 *              EEPROM are protected by mutex semaphores.
 *
 ****************************************************************************/
#ifndef __MAIN_H__
    #define __MAIN_H__
	
    #define MEM_BLK_DATA_SIZE 1024

    #define LCD_MSG_SIZE 40     /* LCD message buffer size */

    typedef struct
    {
        unsigned int mem_addr;
        unsigned int num_bytes;
        char *data_ptr;         /* A pointer to the string to be displayed. */
    } EEPROM_packet;
#endif

/* End of main.h */