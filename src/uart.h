/*
   Filename: tft.h
   
   Description: Functions to control an OPEN-SMART 2.4" TFT LCD screen.
                Ported to PIC32 C code from the Aurduino C++ code 
                provided by the manufacturer. 


   Author: Derek Chadwick
   
   Date: 27/08/2018

*/
#ifndef _UART_H    /* Guard against multiple inclusion */
#define _UART_H


int Serial_Transmit_U1(const char *buffer);
unsigned int Serial_Receive_U1(char *buffer, unsigned int max_size);
int UART1_Configure( int baud);

int Serial_Transmit_U2(const char *buffer, unsigned int size);
unsigned int Serial_Receive_U2(char *buffer, unsigned int max_size);
int UART2_Configure( int baud);

#endif

/* *****************************************************************************
 End of File
 */
