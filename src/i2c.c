
/*

  Author: Derek Chadwick

  File Name: i2c.c

  Description: I2C Master Functions.
  
  Date: 22/08/2018
  
  
 */


#include <xc.h>

#define Fsck 50000
#define BRG_VAL((PBCLK/2/Fsck)-2)  // I2CBRG = [1/(2*Fsck) - PGD]*Pblck - 2 (PGD = pulse gobbler delay)

void i2c_master_setup(void) {
  I2C1BRG = 0x00C2;                 // 100kHz on a 40MHz PBCLK according to the datasheet.
                                    // Fsck is the freq (100 kHz here), PGD = 104 ns
  I2C1CONbits.ON = 1;               // turn on the I2C1 module
}

// Start a transmission on the I2C bus
void i2c_master_start(void) 
{
    I2C1CONbits.SEN = 1;            // send the start bit
    while(I2C1CONbits.SEN) { ; }    // wait for the start bit to be sent
}

void i2c_master_restart(void) 
{     
    I2C1CONbits.RSEN = 1;           // send a restart 
    while(I2C1CONbits.RSEN) { ; }   // wait for the restart to clear
}

void i2c_master_send(unsigned char byte) 
{                                   // send a byte to slave
  I2C1TRN = byte;                   // if an address, bit 0 = 0 for write, 1 for read
  while(I2C1STATbits.TRSTAT) { ; }  // wait for the transmission to finish
  if(I2C1STATbits.ACKSTAT) {        // if this is high, slave has not acknowledged
    //NU32_WriteUART3("I2C2 Master: failed to receive ACK\r\n");
  }
}

unsigned char i2c_master_recv(void) 
{                                     // receive a byte from the slave
    I2C1CONbits.RCEN = 1;             // start receiving data
    while(!I2C1STATbits.RBF) { ; }    // wait to receive the data
    return I2C1RCV;                   // read and return the data
}

void i2c_master_ack(int val) 
{                                     // sends ACK = 0 (slave should send another byte)
                                      // or NACK = 1 (no more bytes requested from slave)
    I2C1CONbits.ACKDT = val;          // store ACK/NACK in ACKDT
    I2C1CONbits.ACKEN = 1;            // send ACKDT
    while(I2C1CONbits.ACKEN) { ; }    // wait for ACK/NACK to be sent
}

void i2c_master_stop(void) 
{                                     // send a STOP:
  I2C1CONbits.PEN = 1;                // comm is complete and master relinquishes bus
  while(I2C1CONbits.PEN) { ; }        // wait for STOP to complete
}
