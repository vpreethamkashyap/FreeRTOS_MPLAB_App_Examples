/******************************************************************************
 * File name:   serial.c
 * Rev. 1.1     Richard Wall	9/28/2011
 * Revised:     September 24, 2013 - ported to MPLAB X
 *
 * Description: BASIC INTERRUPT DRIVEN SERIAL PORT DRIVER. This driver is
 *              primarily to test the scheduler functionality.  It does
 *              not effectively use the buffers or DMA and is therefore not
 *              intended to be an example of an efficient driver. This
 *              implementation utilizes the MPLAB X peripheral library
 *
 *              This program has been modified to use serial port UART1.
 *              To use UART2 change all references to UART1 to UART2
 *
 *              The StartUARTTask creates two tasks: one to receive a serial
 *              text line (xSerialGetLineTask) and another task to send a
 *              serial text line (xSerialLineSendTask). When the
 *              UART is initialized (xSerialPortInitMinimal), two queues are
 *              created: one for receiving UART characters (xRxedChars) and one 
 *              for transmitting a character. Two Additional queues are created,
 *              CommTxQueueHandle for sending a line of text and 
 *              CommTxQueueHandle for receiving a line of text. The UART
 *              initialization function also sets up the UART for 19200 BAUD
 *              communications and initializes the UART ISR which handles both
 *              UART receive and transmit interrupts.
 *
 *****************************************************************************/

/* Standard include file. */
#include <plib.h>
#include <stdlib.h>
#include <string.h>

/* Scheduler include files. */
#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"

/* Applicationm include files. */
#include "serial.h"

/* The queues used to communicate between tasks and ISR's. */
static xQueueHandle xRxedChars;
static xQueueHandle xTxedChars;

/* Flag used to indicate the tx status. */
static portBASE_TYPE xTxHasEnded;

/*--------------------------------------------------------------------------*/

void xSerialGetCharTask( void *pvParameters  );
void xSerialLineSendTask( void *pvParameters );

/* xStartUARTTasks START Function Description ********************************
 * SYNTAX:          void xStartUARTTasks( unsigned portBASE_TYPE uxPriority,
 *                                         unsigned long ulBaudRate );
 * KEYWORDS:        UART, initialize, create, tasks
 * DESCRIPTION:     Initialize UART1 for UART for specified BAUD
 *                  rate and sets up send and receive queues. This
 *                  is followed by spawning the send and receive
 *                  tasks.
 * PARAMETER 1:     Receive task priority level
 * PARAMETER 2:     Send and receive BAUD rate
 * RETURN VALUE:    None
 * NOTES:           The receive task is one priority level above the transmit
 *                  task priority level.
 * END DESCRIPTION ***********************************************************/
portBASE_TYPE xStartUARTTasks( unsigned portBASE_TYPE uxPriority,
                                unsigned long ulBaudRate )
{
portBASE_TYPE u1Status = pdPASS;
// Initialise the comm port then spawn the Rx and Tx tasks.
    xSerialPortInit( ulBaudRate, UART_Q_LEN );

// The Tx task is spawned with a lower priority than the Rx task.
    u1Status &= xTaskCreate( xSerialSendLineTask, ( signed char * ) "COMTx",
                            configMINIMAL_STACK_SIZE, NULL,
                            uxPriority - 1, ( xTaskHandle * ) NULL );

    u1Status &= xTaskCreate( xSerialGetLineTask, ( signed char * ) "COMRx",
                            configMINIMAL_STACK_SIZE, NULL,
                            uxPriority, ( xTaskHandle * ) NULL );
        
    return u1Status;
} /* End of xStartUARTTasks */

/* xSerialPortInit Function Description ***************************************
 * SYNTAX:  xComPortHandle xSerialPort( unsigned long ulWantedBaud
 * 					unsigned portBASE_TYPE uxQueueLength );
 * KEYWORDS:        UART, initialize, create, tasks
 * DESCRIPTION:     Minimal setup of communications queues.  Configures serial
 *                  port for BAUD rate and interrupts
 * PARAMETER 1:     BAUD rate
 * PARAMETER 2:     queue size
 * RETURN VALUE:    Communications Port Handle
 * NOTES:           None
 * END DESCRIPTION ***********************************************************/
xComPortHandle xSerialPortInit( unsigned long ulWantedBaud,
				unsigned portBASE_TYPE uxQueueLength )
{
unsigned short usBRG;	// Constant determine by BAUD rate

/* Create the queues used by Send and Received tasks Queue to pass characters
 * from UART interrupt handler to the receive line task - UART_Q_LEN x 1 char */
    xRxedChars = xQueueCreate( uxQueueLength,
                        ( unsigned portBASE_TYPE ) sizeof( signed char ) );

/* Queue to pass characters string characters form send task to UART 
 * interrupt handler - Q_LEN char x 1 */
    xTxedChars = xQueueCreate( (UART_Q_LEN*sizeof( signed char )+1),
			( unsigned portBASE_TYPE ) sizeof( signed char ) );

/* Queue to pass characters from receive line task to the send task
 * - Q_LEN x 1 char */
   CommTxQueueHandle = xQueueCreate( 5,
		( unsigned portBASE_TYPE ) UART_Q_LEN*sizeof( signed char ) );

   CommRxQueueHandle = xQueueCreate( 5,
                ( unsigned portBASE_TYPE ) UART_Q_LEN*sizeof( signed char ) );

// Configure the UART and interrupts.
    usBRG = (unsigned short)(( (float)configPERIPHERAL_CLOCK_HZ \
            / ( (float)4 * (float)ulWantedBaud ) ) - (float)0.5);
    OpenUART1( (UART_EN | UART_BRGH_FOUR), \
                (UART_RX_ENABLE | UART_TX_ENABLE | \
                UART_INT_TX_LAST_CH | UART_INT_RX_CHAR), usBRG );

    ConfigIntUART1( ( configKERNEL_INTERRUPT_PRIORITY + 1 ) | \
                    UART_INT_SUB_PR0 | UART_TX_INT_EN | UART_RX_INT_EN );

    mU1TXClearIntFlag();    /* Clear Tx interrupt flag */
    xTxHasEnded = pdTRUE;   /* Flag signals that Tx interrupts must be enabled*/

/* Only a single port is implemented so we don't need to return anything. */

    return NULL;
} /* End of xSerialPortInitMinima */

/* xSerialGetCharTask Function Description ************************************
 * SYNTAX:          void xSerialGetCharTask( void *pvParameters  );
 * KEYWORDS:        UART, serial, communications, receive, task
 * DESCRIPTION:     This task assembles a string of character form the
 *                  UART interrupt handler until a LF or CR control
 *                  character is receiver. The congrol character is
 *                  discarded and a string terminating null character is
 *                  added at the end to form a proper test string.
 *                  The string is then sent to the line send task.
 * PARAMETER 1:     pointer to data passed from scheduler
 * RETURN VALUE:    None
 * NOTES:           The maximum string length is set by the defined constant
 *                  UART_Q_LEN.
 * END DESCRIPTION ***********************************************************/
void xSerialGetLineTask( void *pvParameters  )
{
( void ) pvParameters;
char Rx_Message[UART_Q_LEN]; 	/* Line queue buffer */
int Rx_Msg_idx;			/* String array index. */

    strcpy(Rx_Message,"\n\rUART 1 ready to receive text data.\n\r");
    xQueueSend( CommTxQueueHandle, &Rx_Message, portMAX_DELAY );

    Rx_Msg_idx = 0;
/* Get the next character from the buffer.  Return false if no characters
 * are available or arrive before xBlockTime expires. */
    for(;;)
    {
	xQueueReceive( xRxedChars, &Rx_Message[Rx_Msg_idx], portMAX_DELAY );
	if(Rx_Message[Rx_Msg_idx] == '\r' ||
                        Rx_Message[Rx_Msg_idx]== '\n' ||
			Rx_Msg_idx == UART_Q_LEN-1 )
        { /* Complete line received */
            if(UART_Q_LEN - 3 > Rx_Msg_idx) /* Add CR and LF */
            {
                Rx_Message[Rx_Msg_idx++] = '\n';
                Rx_Message[Rx_Msg_idx++] = '\r';
            }
            Rx_Message[Rx_Msg_idx] = 0;     /* Add null termination */

            Rx_Msg_idx = 0;                 /* Reset string index */
/*  Send message to line send task to display back on the terminal */
            xQueueSend( CommTxQueueHandle, &Rx_Message[Rx_Msg_idx],\
                        portMAX_DELAY );

/* Send message to the serial input porcessing task */
            xQueueSend( CommRxQueueHandle, &Rx_Message[Rx_Msg_idx],\
                        portMAX_DELAY );
	}
	else
	{
            Rx_Msg_idx++; 	/* Advance string index */
	}
    }
} /* End of xSerialGetCharTask */

/* xSerialLineSendTask Function Description ***********************************
 * SYNTAX:          void xSerialLineSendTask( void *pvParameters );
 * KEYWORDS:        UART, serial, communications, send, task
 * DESCRIPTION:     This task recieves a string of text ond or ASCII
 *                  control characters and fills a single character queue.
 *                  After the queue has been filled, the UART transmit interrupt
 *                  is enabled and the interrupt flag is set in initiate
 *                  transmit interrupts.
 * PARAMETER 1:     pointer to data passed from scheduler
 * RETURN VALUE:    None
 * NOTES:           The maximum string length is set by the defined constant
 *                  UART_Q_LEN.
 * END DESCRIPTION ***********************************************************/
void xSerialSendLineTask( void *pvParameters )
{
( void ) pvParameters;
char Tx_Message[UART_Q_LEN];	/* Line queue buffer */
int Tx_Msg_idx; 		/* String array index. */
char CR = '\r';

    for(;;)
    {
/* Wait for message */
	xQueueReceive( CommTxQueueHandle, &Tx_Message, portMAX_DELAY );
	Tx_Msg_idx = 0;		/* Initialize transmit index */

/* Fill queue with characters until end of string or maximum queue width */
	while(Tx_Message[Tx_Msg_idx] && (Tx_Msg_idx < UART_Q_LEN) )
	{
            xQueueSend( xTxedChars, &Tx_Message[Tx_Msg_idx], portMAX_DELAY );
            Tx_Msg_idx++;
	}
	xQueueSend( xTxedChars, &CR, portMAX_DELAY ); /* Add RETURN to queue*/

	if( xTxHasEnded )			/* Start transmit interrupts */
	{
/* Flag signals that Tx interrupts are enabled */
            xTxHasEnded = pdFALSE;
            IFS0bits.U1TXIF = serSET_FLAG;	/* Start Tx Interrupts */
            EnableIntU1TX;			/* Enable Tx Interrupts */
	}
    }
} /* End of xSerialLineSendTask */

/* vU1InterruptHandler Function Description **********************************
 * SYNTAX:          void vU1InterruptHandler( void );
 * KEYWORDS:        UART, serial, communications, interrupt handler
 * DESCRIPTION:     This function is the single UART ISR that handles
 *                  both transmit and receive interrupts.  If a character is
 *                  received, it is immediately send using a queue to the
 *                  that task that assembles the characters into a text string.
 *                  If a Tx interrupt is generated, the Tx buffer status is
 *                  checked. If found to be full, the Tx interrupt flag is
 *                  cleared. If Tx buffer is empty, the queue is check for a
 *                  character waiting to be sent.  If no character is waiting
 *                  in the queue, the Tx interrupts are disabled and the Tx
 *                  interrupt flag is cleared.
 * PARAMETERS:      None
 * RETURN VALUE:    None
 * NOTES:           Local variables are declared as static to conserve
 *                  stack space.
 ****************************************************************************/
void __ISR(_UART_1_VECTOR, ipl3) vU1InterruptHandler( void )
{
static char cChar;
static portBASE_TYPE xHigherPriorityTaskWoken;

    xHigherPriorityTaskWoken = pdFALSE;	/* No higher priority tasks */

/* Are any Rx interrupts pending? */
    if( mU1RXGetIntFlag() )
    {
        while( U1STAbits.URXDA )
	{
/* Retrieve the received character and place it in the queue of
 * received characters.*/
            cChar = U1RXREG;
            xQueueSendFromISR( xRxedChars, &cChar, &xHigherPriorityTaskWoken );
	}
	mU1RXClearIntFlag();
    }

/* Are any Tx interrupts pending? */
    if( mU1TXGetIntFlag() )
    {
        while( !( U1STAbits.UTXBF ) )
	{
            if( xQueueReceiveFromISR( xTxedChars, &cChar,
                                &xHigherPriorityTaskWoken ) == pdTRUE )
            {
/* Send the next character queued for Tx. */
		U1TXREG = cChar;
            }
            else
            {
/* Queue empty, nothing to send. */
		DisableIntU1TX;		/* Disable interrupts */

/* Flag signals that Tx interrupts must be enabled */
		xTxHasEnded = pdTRUE;	
		break; // from while
            }
	}
	mU1TXClearIntFlag();
    }
/* If sending or receiving necessitates a context switch, then switch now. */
    portEND_SWITCHING_ISR( xHigherPriorityTaskWoken );
} /* End of vU1InterruptHandler */

/* End of serial.c */
