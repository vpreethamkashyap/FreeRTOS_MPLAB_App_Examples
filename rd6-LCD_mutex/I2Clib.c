/* 
** 24LC254 EEPROM I2C Library
** Richard Wall
** October 13, 2011
**
** Notes:  	This file has been adapteded from the Microchip C32
**			Peripheral Library documentation.  Unlike the example file 
**			that appears in Chapter 14 of the C32 Peripheral Library 
**			Reference Manual, the programs contained in I2clib.c and 
**			I2Clib.h are written for multiple byte transfers. 
**
**			The BRG_VALUE used in the initI2C function is equal to the 
**			PB clock rate divided by the desired bit rate. The transfer 
**			rate that is set in I2Clib.h is limited to 400,000 bits per 
**			second. Truncation and rounding have no ill effects provided
**			the rounding process results in a bit rate less than 400000.
**
**			The I2CWrtiteEEPROM function has three distinctive write 
**			phases.  
**			Phase 1: Write the I2C device address and starting memory
**					 address. No STOP sequence is issued after header
**					 bytes are written to the I2C device.  The I2C 
**					 device must acknowledge each transfer.  If the 
**					 acknowledge is not receive by the master, this and
**					 all subsequent phases for the EEPROM write are
**					 aborted. The program returns the i2c_error status:
**					 FALSE indicates no error.
**			Phase 2: Write the I2C data.  This is limited to a maximum 
**					 of 64 bytes and all must be to address with in one 
**					 page boundary.  (Page boundaries are memory addresses
**					 modulo 64.) I2C device must acknowledge each transfer.  
**					 If the acknowledge is not receive by the master, this 
**					 and all subsequent phases for the EEPROM write are
**					 aborted. This sequence is terminated with a STOP 
**					 sequence. The program returns the i2c_error status:
**					 FALSE indicates no error.
**			Phase 3: Repeatedly write a single byte containing the device 
**					 address and checking the acknowledge status bit until 
**					 cleared (active low signal). It is handled by the
**					 wait_i2c_xfer function and returns the number of write-
**					 byte cycles before detecting an acknowledgment from
**					 the slave device. 
**			Note 1:	 The count value is consistently 64. This is most likely 
**					 related to the fact that entire page (64 bytes) is 
**					 written regardless of bytes in the transfer. Hence, writing 
**					 to an EEPROM is a very long process. 
**			Note 2:  This version of code does not observe the page writing 
**					 restrictions mentioned in Phase 2. 
**
**			The I2CReadEEPROM function is capable of reading all 32768
**			bytes of data with a single call to this function. This 
**			function is comprised of three phases.
**			Phase 1: Write the I2C device address and starting memory
**					 address. Unlike the I2CWrtiteEEPROM function a STOP 
**					 sequence is NOT issued after header bytes are written to 
**					 the I2C device. The I2C device must acknowledge each 
					 write of the header array.  If the acknowledge 
**					 is not receive by the master, this and all subsequent 
**					 phases for the EEPROM read are aborted. The program 
**					 returns the i2c_error status: FALSE indicates no error.
**			Phase 2: After each N-1 bytes are read into the receiving buffer,
**					 an ACK (acknowledgement) bit is sent to the slave device 
**					 by the master.
**			Phase 3: After the Nth byte read from the I2C device and 
**					 stored into the receiving buffer, an NAK ( not 
**					 acknowledge) bit is sent to the slave device by the master.
**
*/
 
#include <plib.h>
#include "Cerebot32mx7.h"
#include "I2Clib.h"

/* START FUNCTION DESCRIPTION ********************************************
i2c_wait		<I2Clib.c>
SYNTAX:        	void i2c_wait(unsigned int cnt);
KEYWORDS:		Delay, very short software delay
DESCRIPTION:	Implements a programmable delay using Nop's
Parameter 1:	Number of delay cycles
RETURN VALUE:  	None
NOTES:			None
END DESCRIPTION **********************************************************/
void i2c_wait(unsigned int cnt)
{
	while(--cnt)
	{
		Nop();
		Nop();
	}
}

/* START FUNCTION DESCRIPTION ********************************************
initI2C						<I2Clib.c>
SYNTAX:        	void initI2C(void);
KEYWORDS:		i2c, bit rate generator
DESCRIPTION:	Opens I2C channel 1 and sets the i2c bit rate based upon PBCLK
Parameter 1:	None
RETURN VALUE:  	None
NOTES:			BRG_VAL = PBCLK / desired bit rate. Must be less than
				400,000 or slowest I2C device
END DESCRIPTION **********************************************************/
void initI2C(void)
{
	OpenI2C1( I2C_EN, BRG_VAL );  //Enable channel
}

/* START FUNCTION DESCRIPTION ********************************************
I2CWrtiteEEPROM						<I2Clib.c>
SYNTAX:        	int I2CWrtiteEEPROM(int SlaveAddress, int mem_addr, 
									char *i2cData, int len);
KEYWORDS:		Write EEPROM, device address, memory address
DESCRIPTION:	Writes from 1 to 64 bytes of data to the EEPROM starting
				at the specified memory address (0x0000 to 0x7FFF).
Parameter 1:	EEPROM Device address
Parameter 2:	EEPROM memory starting address
Parameter 3:	Array of 8 bit data to be written to the EEPROM
Parameter 4:	Number of data bytes to write
RETURN VALUE:  	error_flag: False -> No error, write was successful
NOTES:			The maximum block size is 64 bytes.  All bytes must be 
				within a single page boundary (memory address modulo 64).
END DESCRIPTION **********************************************************/
int I2CWrtiteEEPROM(int SlaveAddress, int mem_addr, char *i2cData, int len)
{
unsigned char header[3];  // contains the device address and memory address
int Index;
int i2c_error;

	i2c_error = FALSE;	 	// Reset the error flag
	header[0] = (unsigned char) (SlaveAddress << 1);
	header[1] = (unsigned char) (mem_addr >> 8);
	header[2] = (unsigned char) (mem_addr);

	Index = 0;				// Send device and memory address
	StartI2C1();			// Send the Start Bit
	IdleI2C1();				// Wait to complete
	while((Index < 3)  && !i2c_error)
	{
		MasterWriteI2C1( header[Index++] );
		IdleI2C1();			// Wait to complete
		i2c_error |= I2C1STATbits.ACKSTAT;	// Check for acknowledgement
	}

	Index = 0;				// Write data to EEPROM
	while((Index < len)  && !i2c_error)
	{
		MasterWriteI2C1( i2cData[Index++] );
		IdleI2C1();			// Wait to complete
		i2c_error |= I2C1STATbits.ACKSTAT;	// Check each byte written
	}
	StopI2C1();				// Send the Stop condition
	IdleI2C1();				// Wait to complete

	if(!i2c_error) 			// if write was successful - complete write process
	{
		Index =  wait_i2c_xfer( SlaveAddress );
	}

	return i2c_error;
}

/* START FUNCTION DESCRIPTION ********************************************
I2CReadEEPROM						<I2Clib.c>
SYNTAX:        	int I2CReadEEPROM(int SlaveAddress, int mem_addr, 
									char *i2cData, int len);
KEYWORDS:		Read EEPROM, device address, memory address
DESCRIPTION:	Reads the number of specified bytes from the EEPROM starting
				at the specified memory address (0x0000 to 0x7FFF).
Parameter 1:	EEPROM Device address
Parameter 2:	EEPROM memory starting address
Parameter 3:	Array of 8 bit data to be read from the EEPROM
Parameter 4:	Number of data bytes to read
RETURN VALUE:  	error_flag: False -> No error, Write was successful
NOTES:			There is no restriction on the number of bytes that can 
				be read in a single call to this function.
END DESCRIPTION **********************************************************/
int I2CReadEEPROM(int SlaveAddress, int mem_addr, char *i2cData, int len)
{
unsigned char header[3];
int Index;
int i2c_error;

// Set up EEPROM header using write sequence
	i2c_error = FALSE;
	header[0] = (unsigned char) (SlaveAddress << 1);
	header[1] = (unsigned char) (mem_addr >> 8);
	header[2] = (unsigned char) (mem_addr);

// Write starting address - this must be a full START -STOP Sequence
// and will poll acknowledgment from slave device
	StartI2C1();			// Send the Start Bit
	IdleI2C1();				// Wait to complete
	Index = 0;				// Send device and memory address
	while((Index < 3)  && !i2c_error)
	{
		MasterWriteI2C1( header[Index++] );
		IdleI2C1();			// Wait to complete
		i2c_error |= I2C1STATbits.ACKSTAT;	
	}

// The read cycle continues only if successful in setting the 
// starting address
	if(!i2c_error)		
	{
		RestartI2C1();		// Implemented repeated start
		IdleI2C1();			// Wait to complete
		MasterWriteI2C1( (SlaveAddress << 1) | 1 ); // Send read command
		IdleI2C1();			// Wait to complete
		Index = 0;			
		while(Index < len-1) // Read all but the last byte
		{
			i2cData[Index++] = MasterReadI2C1();
			IdleI2C1();		// Wait to complete
			AckI2C1();		// Master ACK to slave
			IdleI2C1();		// Wait to complete
		}
		i2cData[Index] = MasterReadI2C1();	// Get last byte
		IdleI2C1();			// Wait to complete
		NotAckI2C1();		// Master sends NACK to slave
		IdleI2C1();			// Wait to complete
	}
	StopI2C1();				// Terminate read cycle
	IdleI2C1();				// Wait to complete
	return i2c_error;	
}

/* START FUNCTION DESCRIPTION ********************************************
wait_i2c_xfer						<I2Clib.c>
SYNTAX:        	int wait_i2c_xfer(int SlaveAddress);
KEYWORDS:		Write EEPROM, acknowledgement, write cycle complete
DESCRIPTION:	Continuously writes the device address to the slave 
				device until the ACK bit is received from the slave
				device. A continuous write cycle is used to test ACK 
				bit that indicates that write is complete
Parameter 1:	EEPROM Device address
RETURN VALUE:  	Number of cycles needed to complete transfer
NOTES:			This is a blocking function.  Timeout based upon core 
				timer or loop count should be implemented for critical 
				timed systems.  100 cycles are typically required
END DESCRIPTION **********************************************************/
int wait_i2c_xfer(int SlaveAddress)
{
int count;
int i2c_ack;

	count = 0;	// Track how many cycles are needed
	do
	{
		i2c_wait(100);
		StartI2C1();			// Send the Start Bit
		IdleI2C1();				// Wait to complete
		MasterWriteI2C1( SlaveAddress << 1 );
		IdleI2C1();				// Wait to complete
		i2c_ack = I2C1STATbits.ACKSTAT;	// Read ACK status
		StopI2C1();				// Send the Stop condition
		IdleI2C1();				// Wait to complete
		++count;
	} while(i2c_ack);			// Wait for ACK
	return count;	
} 

// end of i2clib.c

