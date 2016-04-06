/* File name:   serial.h
 * Author:      Richard Wall
 * Date:        September 8, 2011
 * Revised:     September 25, 3013
 *
 * Notes:	This program has been modified to use serial port UART1. To use
 *              UART2 change all references to UART1 to UART2
 *
 * BASIC INTERRUPT DRIVEN SERIAL PORT DRIVER for FreeRTOS .
 *
 * NOTE:  This driver is primarily to test the scheduler functionality. It does
 * not effectively use the buffers or DMA and is therefore not intended to be
 * an example of an efficient driver.
 *
*/

#ifndef __SERIAL_COMMS_H__
    #define __SERIAL_COMMS_H__

    typedef void * xComPortHandle;

    typedef enum
    {
	serCOM1, 
	serCOM2, 
	serCOM3, 
	serCOM4, 
	serCOM5, 
	serCOM6, 
	serCOM7, 
	serCOM8 
    } eCOMPort;

    typedef enum
    {
	serNO_PARITY, 
	serODD_PARITY, 
	serEVEN_PARITY, 
	serMARK_PARITY, 
	serSPACE_PARITY 
    } eParity;

    typedef enum
    {
	serSTOP_1, 
	serSTOP_2 
    } eStopBits;

    typedef enum
    {
	serBITS_5, 
	serBITS_6, 
	serBITS_7, 
	serBITS_8 
    } eDataBits;

    typedef enum
    {
	ser50,		
	ser75,		
	ser110,		
	ser134,		
	ser150,    
	ser200,
	ser300,		
	ser600,		
	ser1200,	
	ser1800,	
	ser2400,   
	ser4800,
	ser9600,		
	ser19200,	
	ser38400,	
	ser57600,	
	ser115200
    } eBaud;

/* Maximum string length +1 */
    #define UART_Q_LEN	120

/* Global variables are declared only once. */
/* Global queues for  sending and receiving UART text strings. */
    xQueueHandle xLineForTx;
    xQueueHandle xLineForRx;

#endif

/* These task are access from outside this file. */
portBASE_TYPE uCreateUARTTasks( unsigned portBASE_TYPE uxPriority,
                                unsigned long ulBaudRate );

/* Tasks for sending and receiving UART text strings */
void vSerialGetLineTask( void *pvParameters  );
void vSerialLineSendTask( void *pvParameters );

/* End of serial.h */
