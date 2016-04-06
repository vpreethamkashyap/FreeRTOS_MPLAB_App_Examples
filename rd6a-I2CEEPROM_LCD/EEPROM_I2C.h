/*
** EEPROM I2C Library
** Richard Wall
** October 3, 2011
** January 31, 2014 - Uses XC32 peripheral library functions
** September 30, 2014 - updated to chipKIT Pro MX7
*/

#ifndef __I2CLIB
    #define __I2CLIB

    #define I2C_CLOCK_FREQ      357000
    #define EEPROM_I2C_BUS      I2C2
    #define EEPROM_ADDRESS      0x50    /* 0b1010000 Serial EEPROM address */
    #define MAX_PAGES           64		/* 64 byte maximum page size. */
#endif

/* Function Prototypes */
void initI2C( I2C_MODULE i2c_port);

I2C_RESULT I2CWrtiteEEPROM(I2C_MODULE i2c_port, BYTE DeviceAddrress,
                            int mem_addr, BYTE *data, int len);
I2C_RESULT I2CReadEEPROM(I2C_MODULE i2c_port, BYTE DeviceAddrress,
                            int mem_addr, BYTE *data, int len);
int I2CReadByte(I2C_MODULE i2c_port, BYTE DeviceAddrress, BYTE *data);
int I2CWriteByte(I2C_MODULE i2c_port, BYTE DeviceAddrress, BYTE data);
BOOL StartTransfer( I2C_MODULE i2c_port, BOOL restart );
void StopTransfer( I2C_MODULE i2c_port );
BOOL TransmitOneByte( I2C_MODULE i2c_port, BYTE data );
I2C_RESULT ReceiveOneByte( I2C_MODULE i2c_port, BYTE *data, BOOL ack );

/* End of EEPROM_I2C_lib.h */
