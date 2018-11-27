/*
   Filename: util.h
   
   Description: Various utility functions. 


   Author: Derek Chadwick
   
   Date: 27/08/2018

*/


#ifndef _UTIL_H    /* Guard against multiple inclusion */
#define _UTIL_H

void xzero(volatile char *buf, int len);
int xstrcpy(char *out_buf, char *in_buf, int start, int end);
void delay_ms(unsigned t);
void delay_us(unsigned t);
void binary_to_ASCIIconvert(int n);
int string_to_int(char *str);

#endif /* _EXAMPLE_FILE_NAME_H */

/* *****************************************************************************
 End of File
 */
