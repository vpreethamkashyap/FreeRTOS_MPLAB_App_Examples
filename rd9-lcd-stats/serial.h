/******************************************************************************
 * File name:   serial.h
 * Rev. 1.1     Richard Wall	9/28/2011
 * Revised:     September 24, 2013 - ported to MPLAB X
 *
 * This program has been modified to use seria port UART1. To use UART2
 * change all references to UART1 to UART2
 *
 ******************************************************************************/

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
    
/* Hardware setup. */
    #define serSET_FLAG		( 1 )
    #define serCLR_FLAG		( 0 )

    #define UART_Q_LEN		120
    xQueueHandle CommTxQueueHandle; /* Queue for sending UART messages */
    xQueueHandle CommRxQueueHandle; /* Queue for receiving UART messages */

#endif

/* Task prototypes */
portBASE_TYPE xStartUARTTasks( unsigned portBASE_TYPE uxPriority,
                                unsigned long ulBaudRate );
void xSerialGetLineTask( void *pvParameters  );
void xSerialSendLineTask( void *pvParameters );
xComPortHandle xSerialPortInit( unsigned long ulWantedBaud,
                                unsigned portBASE_TYPE uxQueueLength );


