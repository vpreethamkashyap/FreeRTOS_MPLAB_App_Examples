/* File name:   serial.c
 * Author:      Richard Wall 
 * Date:        September 8, 2011
 * Revised:     September 26, 3013
 *              February 21, 2014
 *              October 2, 2014 - editorial corrections to comments
 *
 * Notes:       This program has been modified to use serial port UART1. To use
 *              UART2 change all references to UART1 to UART2
 *
 * BASIC INTERRUPT DRIVEN SERIAL PORT DRIVER for FreeRTOS.
 *
 * NOTE:  This driver is primarily to test the scheduler functionality. It does
 * not effectively use the buffers or DMA and is therefore not intended to be
 * an example of an efficient driver.
 *
*/

/* Standard include file. */
#include <plib.h>
#include <stdlib.h>
#include <string.h>
/* Scheduler include files. */
#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"

/* Application include files. */
#include "serial.h"

/* UART interrupt control bits */
#define serSET_FLAG     ( 1 )
#define serCLR_FLAG     ( 0 )

/* The queues used to communicate between UART Line tasks and ISR's. */
static xQueueHandle xRxedChars; 
static xQueueHandle xTxedChars; 

static xComPortHandle xSerialPortInit( unsigned long ulWantedBaud,
                                        unsigned portBASE_TYPE uxQueueLength );

/* Flag used to indicate the Tx status. */
static portBASE_TYPE xTxHasEnded;

/* uCreateUARTTasks Function Description ***************************************
 * SYNTAX:          void uCreateUARTTasks( unsigned portBASE_TYPE uxPriority,
 *                                          unsigned long ulBaudRate );
 * KEYWORDS:        UART, initialize, create, tasks
 * DESCRIPTION:     Initialize UART1 for UART for specified BAUD
 *                  rate and sets up send and receive queues. This
 *                  is followed by spawning the send and receive
 *                  tasks.
 * PARAMETER 1:     Receive task priority level
 * PARAMETER 2:     Send and receive BAUD rate
 * RETURN VALUE:    portBASE_TYPE ulStatus1 - status of creating task
 * NOTES:           The receive task is one priority level above the transmit
 *                  task priority level.
 * END DESCRIPTION *****************************************************/
portBASE_TYPE uCreateUARTTasks( unsigned portBASE_TYPE uxPriority,
                                unsigned long ulBaudRate )
{
portBASE_TYPE ulStatus1 = pdPASS;

/* Initialise the com port then spawn the Rx and Tx tasks. */
    xSerialPortInit( ulBaudRate, UART_Q_LEN );

/* The Tx task is spawned with a lower priority than the Rx task. */
    ulStatus1 &= xTaskCreate( vSerialLineSendTask, ( signed char * ) "COMTx",
    configMINIMAL_STACK_SIZE, NULL, uxPriority - 1, ( xTaskHandle * ) NULL );
	
    ulStatus1 &= xTaskCreate( vSerialGetLineTask, ( signed char * ) "COMRx",
    configMINIMAL_STACK_SIZE, NULL, uxPriority, ( xTaskHandle * ) NULL );

    return ulStatus1; /* Return success/failure of task creation */
}

/* xSerialPortInit Function Description ********************************
 * SYNTAX:  xComPortHandle xSerialPortInit( unsigned long ulWantedBaud,
 *                         unsigned portBASE_TYPE uxQueueLength );
 * KEYWORDS:        UART, initialize, create, tasks
 * DESCRIPTION:     Minimal setup of communications queues.  Configures serial
 *                  port for BAUD rate and interrupts
 * PARAMETER 1:     BAUD rate
 * PARAMETER 2:     queue size
 * RETURN VALUE:    Comm Port Handle
 * NOTES:           None
 * END DESCRIPTION *****************************************************/
xComPortHandle xSerialPortInit( unsigned long ulWantedBaud, 
                                unsigned portBASE_TYPE uxQueueLength )
{
unsigned short usBRG;	/* Constant determine by BAUD rate */

/* Create the queues used by Send and Received tasks */
/* Queue to pass characters from UART interrupt handler to the receive line
 * task - UART_Q_LEN x 1 char*/
    xRxedChars = xQueueCreate( uxQueueLength*sizeof( signed char ),
                        ( unsigned portBASE_TYPE ) sizeof( signed char ) );

/* Queue to pass characters string characters form send taskto UART
 * interrupt handler - UART_Q_LEN  x 1 char */

    xTxedChars = xQueueCreate( (UART_Q_LEN*sizeof( signed char )+1),
                        ( unsigned portBASE_TYPE ) sizeof( signed char ) );

/* Queue to pass characters from receive line task to the send task -
 * 1 X UART_Q_LEN chars */
    xLineForTx = xQueueCreate( sizeof( signed char ),
                ( unsigned portBASE_TYPE ) UART_Q_LEN*sizeof( signed char ) );

/* Configure the UART and interrupts. */
    usBRG = (unsigned short)(( (float)configPERIPHERAL_CLOCK_HZ / ( (float)4 \
                            * (float)ulWantedBaud ) ) - (float)0.5);
    OpenUART1( (UART_EN | UART_BRGH_FOUR), (UART_RX_ENABLE | UART_TX_ENABLE \
                        | UART_INT_TX_LAST_CH | UART_INT_RX_CHAR), usBRG );

    ConfigIntUART1( ( configKERNEL_INTERRUPT_PRIORITY + 1 ) | UART_INT_SUB_PR0 \
                        | UART_TX_INT_EN | UART_RX_INT_EN );

    mU1TXClearIntFlag();  /* Clear Tx interrupt flag */
    xTxHasEnded = pdTRUE; /* Flag signals that Tx interrupts must be enabled*/

/* Only a single port is implemented so we don't need to return anything. */
    return NULL;
} /* End of xSerialPortInitMinimal */

/* vSerialGetLineTask Function Description ***********************************
 * SYNTAX:      void vSerialGetLineTask( void *pvParameters  );
 * KEYWORDS:    UART, serial, text, communications, receive, task
 *
 * DESCRIPTION: This task assembles a string of character form the
 *              UART interrupt handler until a LF or CR control
 *              character is received. The control characters are
 *              discarded and a string terminating null character is
 *              added at the end to form a proper test string.
 *              The string is then sent to the line send task or what
 *              ever task is waiting for a serial text string.
 * 
 * PARAMETER 1: pointer to data passed from scheduler
 * RETURN VALUE: None
 * NOTES:       The maximum string length is set by the defined constant
 *              UART_Q_LEN.

 *              In this example, the CR LF control characters are
 *              inserted back into the text string and the string is
 *              echoed back to the UART.
 * END DESCRIPTION *****************************************************/
void vSerialGetLineTask( void *pvParameters  )
{
( void ) pvParameters;
char Rx_Message[UART_Q_LEN]; 	// Line queue buffer
int Rx_Msg_idx;					// String array index.

    strcpy(Rx_Message,"\n\rUART test ready to receive text.\n\r");
    xQueueSend( xLineForTx, Rx_Message, portMAX_DELAY );

    Rx_Msg_idx = 0;
/* Get the next character from the buffer.  Return false if no characters
 * are available or arrive before xBlockTime expires. */
    for(;;)
    {
        xQueueReceive( xRxedChars, &Rx_Message[Rx_Msg_idx], portMAX_DELAY );
        if(Rx_Message[Rx_Msg_idx] == '\r' ||
            Rx_Message[Rx_Msg_idx]== '\n' ||
            Rx_Msg_idx == UART_Q_LEN-1 )
	{  /* Complete line received */
            if(UART_Q_LEN > Rx_Msg_idx + 3) /* Add CR and LF is buffer space */
            {
                Rx_Message[Rx_Msg_idx++] = '\n'; /* Add LF to string  */
                Rx_Message[Rx_Msg_idx++] = '\r'; /* Add CR to string  */
            }
            Rx_Message[Rx_Msg_idx] = 0; /* Add null termination */
            Rx_Msg_idx = 0;		/* Reset string index */
/* Send message to line send task */
//          xQueueSend( xLineForTx, &Rx_Message[Rx_Msg_idx], portMAX_DELAY );
            xQueueSend( xLineForTx, Rx_Message, portMAX_DELAY );
        }
        else
        {
            Rx_Msg_idx++; 	/* Advance string index */
        }
    }
} /* End of vSerialGetLineTask */

/* vSerialLineSendTask Function Description *********************************
 * SYNTAX:          void vSerialLineSendTask( void *pvParameters );
 * KEYWORDS:        UART, serial, communications, send, task
 * DESCRIPTION:     This task receives a string of text and or ASCII
 *                  control characters and fills a single character queue,
 *                  xTxedChars, that has a depth of UART_Q_LEN and a width of 1.
 *                  After this [UART_Q_LEN x 1] queue has been filled,
 *                  the UART transmit interrupt is enabled and the interrupt
 *                  flag is set to initiate the first transmit interrupt. The
 *                  UART interrupt takes the characters one at a time off
 *                  the xTxedChars queue until it is empty. Once the xTxedChars
 *                  queue is empty, the UART transmit interrupts are disabled.
 *                  See the function description for vU1InterruptHandler below
 *                  for additional details.
 * PARAMETER 1:     pointer to data passed from scheduler
 * RETURN VALUE:    None
 * NOTES:           The maximum string length is set by the defined constant
 *                  UART_Q_LEN.
 * END DESCRIPTION ***********************************************************/
void vSerialLineSendTask( void *pvParameters )
{
( void ) pvParameters;
char Tx_Message[UART_Q_LEN];	// Line queue buffer
int Tx_Msg_idx; 				// String array index.
char CR = '\r';

    for(;;)
    {
/* Wait for message */
        xQueueReceive( xLineForTx, &Tx_Message, portMAX_DELAY );
        Tx_Msg_idx = 0;		/* Initialize transmit index */

/* Fill queue with characters until end of string or maximum queue width */
        while(Tx_Message[Tx_Msg_idx] && (Tx_Msg_idx < UART_Q_LEN) )
        {
            xQueueSend( xTxedChars, &Tx_Message[Tx_Msg_idx], portMAX_DELAY );
            Tx_Msg_idx++;
        }
        xQueueSend( xTxedChars, &CR, portMAX_DELAY ); /* Add RETURN to queue */

        if( xTxHasEnded )			/* Start transmit interrupts */
        {
            xTxHasEnded = pdFALSE;		/* Flag signals that Tx
                                                 * interrupts are enabled */
            IFS0bits.U1TXIF = serSET_FLAG;	/* Start Tx Interrupts */
            EnableIntU1TX;			/* Enable Tx Interrupts */
        }
    }
} /* End of vSerialLineSendTask */

/* vU1InterruptHandler Function Description **********************************
 * SYNTAX:          void vU1InterruptHandler( void );
 * KEYWORDS:        UART, serial, communications, interrupt handler
 * DESCRIPTION:     This function is the single UART ISR that handles
 *                  both transmit and receive interrupts.  If a character is
 *                  received, it is immediately send using a queue to the
 *                  that task that assembles the characters into a text string.
 *                  If a Tx interrupt is generated, the Tx buffer status is
 *                  checked.if found to be full, the Tx interrupt flag is
 *                  cleared. If Tx buffer is empty, the queue is check for a
 *                  character waiting to be sent.  If no character is waiting
 *                  in the queue, the Tx interrupts are disabled and the Tx
 *                  interrupt flag is cleared.
 * PARAMETERS:      None
 * RETURN VALUE:    None
 * NOTES:           Local variables are declared as static to conserve
 *                  stack space.
 * END DESCRIPTION *****************************************************/
void __ISR(_UART_1_VECTOR, ipl2) vU1InterruptHandler(void)
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
 * received characters. */
	
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
                xTxHasEnded = pdTRUE;	/*Flag signals that Tx interrupts must
                                         * be enabled */
                break; /* Exit from while loop for clearing Tx buffer */
            }
        }
        mU1TXClearIntFlag();
    }
/* If sending or receiving necessitates a context switch, then switch now. */
    portEND_SWITCHING_ISR( xHigherPriorityTaskWoken );
} /* End of vU1InterruptHandler*/

/* End of serial.c */