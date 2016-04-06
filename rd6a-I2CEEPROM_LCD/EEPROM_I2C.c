/*
** 24LC256 EEPROM I2C Library
** Richard Wall
** October 13, 2011
** Rev: October 26, 2012 - RWW. Edited notes and comments
** Rev: January 31, 2014 - RWW. Update peripheral library functions to XC32
**
** Notes:   This version of the I2C interface to a 24LC256 I2C EEPROM uses
**          the XC32 peripheral library functions and is loosely based on the
**          Microchip XC32 peripheral library example "i2c_master". Unlike the
**          Microchip example that is limited in scope, this version has no
**          restrictions on the starting memory write address or the size of
**          block of data written to the EEPROM. (See comments below concerning
**          paging.)
**
**          Error messages are displayed over the serial port using the
**          printf function. These statements may be commented out without
**          any loss of functionality or performance.  However, you must deal
**          with the exception is some fashion.  Most errors end up in an
**          infinite loop.
**
**      The I2CWriteEEPROM function has three distinctive write phases.
**          Phase 1: Write the I2C device address and starting memory
**              address. A STOP sequence is issued after header
**              bytes are written to the I2C device.  However, the
**              I2C device must acknowledge each transfer.  If the
**              acknowledge is not receive by the master, this and
**              all subsequent phases for the EEPROM write are
**              aborted.
**          Phase 2: Write the I2C data.  This is limited to a maximum
**              of 64 bytes and all must be to addresses within one
**              page boundary.  (Page boundaries are memory addresses
**              modulo 64.) I2C device must acknowledge each byte
**              transfer. If the acknowledge is not receive by the
**              master, this and all subsequent phases for the
**              EEPROM write are aborted. The write cycle is terminated
**              using a stop-sequence.  However, at this point, the data
**              has not been transferred to the non-volatile memory portion of
**              of the EEPROM.  Phase 3 is required to wait for that operation
**              to be completed.
**          Phase 3: Using a "repeated-start sequence" write a single byte
**              containing the device address and checking the acknowledge
**              status bit until cleared (active low signal). This operation
**              is handled by the wait_i2c_xfer function and returns the
**              number of write-byte cycles before detecting an
**              acknowledgement form the slave device.
**      Note 1:	The count value is consistently 64. This is probably
**              related to the fact that entire page (64 bytes) is
**              written regardless of bytes in the transfer.
**
**      The I2CReadEEPROM function is capable of reading all 32768
**      bytes of data with a single read operation. This function
**      is comprised of three phases.
**          Phase 1: Write the I2C device address and starting memory
**              address. A STOP sequence is not issued after header
**              bytes are written to the I2C device. Instead of the STOP and
**              START sequence, a REPEATED START sequence is used. The I2C
**              device must acknowledge each transfer.  If the acknowledge
**              is not receive by the master, this and all subsequent
**              phases for the EEPROM read are  aborted.
**          Phase 2: After each of the N-1 bytes are read into the receiving
**              buffer, an ACK (acknowledgement) bit is sent to the slave
**              device by the master.
**          Phase 3: After the Nth byte is read from the I2C slave device and
**              stored into the receiving buffer, an NAK ( not
**              acknowledgement) bit is sent to the slave device by
**              the master.  This signals the slave device that the
**              master has completed the read operation.
******************************************************************************/

#include <plib.h>
#include "chipKIT_PRO_MX7.h"
#include "EEPROM_I2C.h"

/* initI2C FUNCTION DESCRIPTION ***********************************************
 *
 * SYNTAX:      void initI2C(I2C_MODULE i2c_port);
 * KEYWORDS:    i2c, bit rate generator
 * DESCRIPTION:	Opens I2C channel 1 and sets the i2c bit rate based upon PBCLK
 * Parameter 1:	I2C_MODULE i2c_port: I2C1 or I2C2
 * RETURN VALUE: None
 * NOTES:       The desired bit rate. Must be less than	400,000 or the bit rate
 *              of the slowest I2C device on the I2C bus.
END DESCRIPTION **************************************************************/
void initI2C(I2C_MODULE i2c_port)
{
unsigned int actualClock;
char pc = '%';

/* Set Desired Operation Frequency */
    actualClock = I2CSetFrequency(i2c_port,GetPeripheralClock(),I2C_CLOCK_FREQ);
    if ( abs(actualClock-I2C_CLOCK_FREQ) > I2C_CLOCK_FREQ/10 )
    {
        printf(" I2C%d clock frequency = %ld Hz error exceeds 10%c\r\n", 
                i2c_port+1, (unsigned long) actualClock, pc );
    }
    else
    {
        printf(" I2C%d clock frequency = %ld Hz\r\n", i2c_port+1,
              (unsigned long) actualClock);
    }
    I2CEnable(i2c_port, TRUE);
} /* End of initI2C */

/* I2CReadEEPROM FUNCTION DESCRIPTION ****************************************
 *
 * SYNTAX:      I2C_RESULT I2CReadEEPROM(I2C_MODULE i2c_port, int DeviceAddress,
 *                                 int mem_addr, BYTE *i2cData, int len);
 * KEYWORDS:    Read EEPROM, device address, memory address
 * DESCRIPTION:	Reads the number of specified bytes from the EEPROM starting
 *              at the specified memory address (0x0000 to 0x7FFF).
 * Parameter 1:	I2C_MODULE - i2c port: I2C1 or I2C2
 * Parameter 2:	EEPROM Device address
 * Parameter 3:	EEPROM memory starting address
 * Parameter 4:	Array of 8 bit data to be read from the EEPROM
 * Parameter 5:	Number of data bytes to read
 * RETURN VALUE: I2C_RESULT - i2c transaction results
 * NOTES:       There is no restriction on the number of bytes that can
 *              be read in a single operation
END DESCRIPTION **************************************************************/
I2C_RESULT I2CReadEEPROM(I2C_MODULE i2c_port, BYTE DeviceAddress,
                  int mem_addr, BYTE *i2cData, int len)
{
BYTE                header[3];
I2C_7_BIT_ADDRESS   SlaveAddress;
int                 headerIndex = 0;
int                 dataIndex = 0;
I2C_RESULT          i2c_result;
BOOL                okay;

/* Set up EEPROM header using write sequence */
    I2C_FORMAT_7_BIT_ADDRESS(SlaveAddress, DeviceAddress, I2C_WRITE);
    header[0] = SlaveAddress.byte;
    header[1] = (BYTE) (mem_addr >> 8);
    header[2] = (BYTE) (mem_addr);
    headerIndex = 0;

/* Write starting address - this must use a START sequence but not a STOP. A
 * REPEATED START sequence will bother terminate the address write and
 * start the memory read phase. The address write phase polls acknowledgement
 * from slave device to ascertain the device is communicating.
 *
 */
    okay = StartTransfer(i2c_port, FALSE); /* FALSE - no repeated start */
    i2c_result = I2C_SUCCESS;

    while((headerIndex < 3)  && (i2c_result == I2C_SUCCESS) && okay)
    {
        if (TransmitOneByte(i2c_port, header[headerIndex++]))
        {
/* Verify that the byte was acknowledged */
            if(!I2CByteWasAcknowledged(i2c_port))
            {
                printf("Error: Sent byte was not acknowledged\n");
                i2c_result |= I2C_ERROR;
            }
        }
        else
        {
            i2c_result |= I2C_ERROR;
        }
    }

/*The read cycle continues only if successful in setting the starting address*/

    if(i2c_result == I2C_SUCCESS)
    {
        okay = StartTransfer(i2c_port, TRUE); /* TRUE - repeated start */
        if((i2c_result == I2C_SUCCESS) && okay)
        {
            I2C_FORMAT_7_BIT_ADDRESS(SlaveAddress, DeviceAddress, I2C_READ);
            if(!TransmitOneByte( i2c_port, SlaveAddress.byte ))
            {
                if(!I2CByteWasAcknowledged(i2c_port))
                {
                    printf("Error: Sent byte was not acknowledged\n");
                    i2c_result = I2C_ERROR;
                }
            }
            dataIndex = 0;  /* Reset the data array index */
        }
/* Read all but the last byte with ACK bit set */
	while((dataIndex < len-1) && (i2c_result == I2C_SUCCESS))
	{
            i2c_result |=  ReceiveOneByte(i2c_port, &i2cData[dataIndex++],TRUE);
	}
/* Read the last byte without ACK bit set */
        if(i2c_result == I2C_SUCCESS)
        {
            i2c_result |= ReceiveOneByte(i2c_port, &i2cData[dataIndex],FALSE );
        }
    }
    StopTransfer(i2c_port); /* Terminate the EEPROM transfer */
    return i2c_result;
}

/* wait_i2c_xfer FUNCTION DESCRIPTION ***************************************
 *
 * SYNTAX:      int wait_i2c_xfer(I2C_MODULE i2c_port, int DeviceAddress);
 * KEYWORDS:	Write EEPROM, acknowledgement, write cycle complete
 * DESCRIPTION:	Continuously writes the device address to the slave
 * 		device until the ACK bit is received from the slave
 * 		device. A continuous write cycle is used to test ACK
 * 		bit that indicates that write is complete
 * Parameter 1:	I2C_MODULE - i2c port: I2C1 or I2C2
 * Parameter 2: EEPROM Device address
 * RETURN VALUE: Number of cycles needed to complete transfer
 * NOTES:	This is a blocking function.  Time-out based upon core
 * 		timer or loop count should be implemented for critical
 * 		timed systems.  100 cycles are typically required
END DESCRIPTION **************************************************************/
int wait_i2c_xfer(I2C_MODULE i2c_port, BYTE DeviceAddress)
{
int count;
BOOL i2c_ack;
I2C_7_BIT_ADDRESS   SlaveAddress;

    I2C_FORMAT_7_BIT_ADDRESS(SlaveAddress, DeviceAddress, I2C_WRITE);
    count = 0;	// Track how many cycles are needed
    do
    {
        StartTransfer(i2c_port, FALSE); /* FALSE  = no repeated start */
        TransmitOneByte(i2c_port, SlaveAddress.byte);
        while(!I2CTransmissionHasCompleted(i2c_port));
        i2c_ack = I2CByteWasAcknowledged(i2c_port);
        StopTransfer( i2c_port );
	++count;
    } while(!i2c_ack);
    return count;
}

/* I2CWrtiteEEPROM FUNCTION DESCRIPTION *************************************
 *
 * SYNTAX:      I2C_RESULT I2CWrtiteEEPROM(I2C_MODULE i2c_port,
 *              int DeviceAddress, int mem_addr, char *i2cData, int len);
 * KEYWORDS:	Write EEPROM, acknowledgement, write cycle complete
 * DESCRIPTION:	Writes a block of data to the 24LC256 EEPROM
 * Parameter 1:	I2C_MODULE - i2c port: I2C1 or I2C2
 * Parameter 2:	EEPROM Device address
 * Parameter 3:	EEPROM memory starting  address
 * Parameter 4:	pointer to source of memory data array
 * Parameter 5:	number of bytes to write
 * RETURN VALUE: I2C_RESULT - i2c transaction results
END DESCRIPTION **************************************************************/
I2C_RESULT I2CWrtiteEEPROM(I2C_MODULE i2c_port, BYTE DeviceAddress,
                            int mem_addr, BYTE *i2cData, int len)
{
unsigned char header[3];
int headerIndex, dataIndex, pgIndex, initial_mem_addr, maxPg;
I2C_7_BIT_ADDRESS   SlaveAddress;
int count;
I2C_RESULT i2c_result;
BOOL okay;

/* Calculate the number of pages needed and round up always */
//  maxPg = ((float) (len + mem_addr % 64) / MAX_PAGES) + 0.99;
    maxPg = ((len + mem_addr % 64) / MAX_PAGES) + 1;
    dataIndex = 0;
/* Set variable to remain within one page length of data */
    pgIndex = (mem_addr + dataIndex) % MAX_PAGES;
    initial_mem_addr = mem_addr;
    i2c_result = I2C_SUCCESS;

    while((dataIndex < len) && (i2c_result == I2C_SUCCESS))
    {
        I2C_FORMAT_7_BIT_ADDRESS(SlaveAddress, DeviceAddress, I2C_WRITE);
        header[0] = SlaveAddress.byte;
        header[1] = (unsigned char) (mem_addr >> 8);
        header[2] = (unsigned char) (mem_addr);
        headerIndex = 0;

/* Send device and memory address */
        okay = StartTransfer(i2c_port, FALSE); /* FALSE = no repeated start */
        while((headerIndex < 3)  && (i2c_result == I2C_SUCCESS) && okay)
        {
            if (TransmitOneByte(i2c_port, header[headerIndex++]))
            {
/* Verify that the byte was acknowledged */
                if(!I2CByteWasAcknowledged(i2c_port))
                {
                    printf("Error: Sent byte was not acknowledged\n");
                    i2c_result |= I2C_ERROR;
                }
            }
            else
            {
                i2c_result |= I2C_ERROR;
            }
        }

        while((pgIndex < (MAX_PAGES)) && (i2c_result == I2C_SUCCESS) &&\
                            okay && (dataIndex < len))
	{
            okay = TransmitOneByte(i2c_port, i2cData[dataIndex] );
            if(okay)
            {
                dataIndex ++;
/* Set variable to remain within one page length of data */
                pgIndex = (initial_mem_addr + dataIndex) % MAX_PAGES;
                if (pgIndex == 0)
                    pgIndex = 64;
            }
            else
            {
                i2c_result = I2C_ERROR;
            }
        }

        StopTransfer( i2c_port );       // Send the Stop condition

    pgIndex = 0;

	if (dataIndex < len)
    {
/* Calculate starting address of next page */
            mem_addr = mem_addr + (MAX_PAGES - (mem_addr % MAX_PAGES));
    }

/* This function wait for a block of data to be transferred to the EEPROM.
 * The wait_iwc_xfer returns the number of attempts to write the device address
 * to the EEPROM before the byte write is acknowledged by the EEPROM device.
 * The count value is for performance testing and debugging only. */
        if(i2c_result == I2C_SUCCESS)
        {
            count =  wait_i2c_xfer( EEPROM_I2C_BUS , DeviceAddress );
/* Diagnostics only - comment out this statement for production code */
//          printf("Wait count = %d\n\r", count);
	    }
    }
    return i2c_result;
}
/******************************************************************************
 * Function:    BOOL StartTransfer( I2C_MODULE i2c_port, BOOL restart );
 * Summary:     Starts (or restarts) a transfer to/from the EEPROM.
 * Description: This routine starts (or restarts) a transfer to/from the EEPROM,
 *              waiting (in a blocking loop) until the start (or re-start)
 *              condition has completed.
 *
 * Precondition: The I2C module must have been initialized.
 * Parameters:  restart - If FALSE, send a "Start" condition
 *                      - If TRUE, send a "Restart" condition
 *              i2c_port- I2C1 or I2C2
 *
 * Returns:     TRUE    - If successful
 *              FALSE   - If a collision occurred during Start signalling
 * Remarks:     This is a blocking routine that waits for the bus to be idle
 *              and the Start (or Restart) signal to complete.
END DESCRIPTION **************************************************************/

BOOL StartTransfer( I2C_MODULE i2c_port, BOOL restart )
{
I2C_STATUS  status;

/* Send the Start (or Restart) sequence */
    if(restart)
    {
        I2CRepeatStart(i2c_port);
    }
    else
    {
/* Wait for the bus to be idle, then start the transfer */
        while( !I2CBusIsIdle(i2c_port) );

        if(I2CStart(i2c_port) != I2C_SUCCESS)
        {
            printf("Error: Bus collision during transfer Start\n");
            return FALSE;
        }
    }

/* Wait for the START or REPEATED START to complete */
    do
    {
        status = I2CGetStatus(i2c_port);
    } while ( !(status & I2C_START) );

    return TRUE;
}

/*******************************************************************************
 * Function:    BOOL TransmitOneByte( I2C_MODULE i2c_port, BYTE data )
 * Summary:     This transmits one byte to the EEPROM.
 * Description: This transmits one byte to the EEPROM, and reports errors for
 *              any bus collisions.
 * Precondition: The transfer must have been previously started.
 *
 * Parameters:  i2c_port    - I2C_MODULE
 *              data        - Data byte to transmit
 *
 * Returns:     TRUE    - Data was sent successfully
 *              FALSE   - A bus collision occurred
 *
 * Remarks:     This is a blocking routine that waits for the transmission to
 *              complete.
END DESCRIPTION **************************************************************/
BOOL TransmitOneByte( I2C_MODULE i2c_port, BYTE data )
{
/* Wait for the transmitter to be ready */
    while(!I2CTransmitterIsReady(i2c_port));
/* Transmit the byte */
    if(I2CSendByte(i2c_port, data) == I2C_MASTER_BUS_COLLISION)
    {
        printf("Error: I2C Master Bus Collision\n");
        return FALSE;
    }
/* Wait for the transmission to finish */
    while(!I2CTransmissionHasCompleted(i2c_port));
    return TRUE;
}
/******************************************************************************
 * Function:    void StopTransfer( I2C_MODULE i2c_port )
 * Summary:     Stops a transfer to/from the EEPROM.\
 *
 * Description: This routine Stops a transfer to/from the EEPROM, waiting (in a
 *              blocking loop) until the Stop condition has completed.
 *
 * Precondition: The I2C module must have been initialized & a transfer started.
 *
 * Parameters:  i2c_port    - I2C_MODULE
 * Returns:     None.
 * Remarks:     This is a blocking routine that waits for the Stop sequence to
 *              complete.
END DESCRIPTION **************************************************************/

void StopTransfer( I2C_MODULE i2c_port )
{
I2C_STATUS  i2c_status;

/* Send the STOP sequence */
    I2CStop(i2c_port);

/* Wait for the STOP sequence to complete */
    do
    {
        i2c_status = I2CGetStatus(i2c_port);

    } while ( !(i2c_status & I2C_STOP) );
}

/*******************************************************************************
 * Function:    I2C_RESULT ReceiveOneByte( I2C_MODULE i2c_port,
 *                                          BYTE *data, BOOL ack );
 * Summary:     This transmits one byte to the EEPROM.
 * Description: This transmits one byte to the EEPROM, and reports errors for
 *              any bus collisions.
 * Precondition: The transfer must have been previously started.
 *
 * Parameters:  i2c_port    - I2C_MODULE
 *              data        - Data byte to transmit
 *              ack         - TRUE - Send ACK
 *                          - FALSE - SEND NO ACK
 *
 * Returns:     TRUE    - Data was sent successfully
 *              FALSE   - A bus collision occurred
 *
 * Remarks:     This is a blocking routine that waits for the transmission to
 *              complete.
END DESCRIPTION **************************************************************/
 I2C_RESULT ReceiveOneByte( I2C_MODULE i2c_port, BYTE *data, BOOL ack )
 {
 I2C_RESULT i2c_result = I2C_SUCCESS;
    if(I2CReceiverEnable(i2c_port, TRUE) == I2C_RECEIVE_OVERFLOW)
    {
        printf("Error: I2C Receive Overflow\n");
        i2c_result =  I2C_RECEIVE_OVERFLOW;
    }
    else
    {
        while(!I2CReceivedDataIsAvailable(i2c_port));
/* The "ack" parameter determines if the EEPROM read is acknowledged */
        I2CAcknowledgeByte(i2c_port, ack);
        while(!I2CAcknowledgeHasCompleted(i2c_port));
/* Read the received data byte */
        *data = I2CGetByte(i2c_port);
    }
    return i2c_result;
 } /* End of ReceiveOneByte */

/* End of EEPROM_i2c.c */

