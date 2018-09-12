/*
   Filename: pulse.c
   
   Description: Functions to read a pulse sensor, adapted from the
                Arduino code provided by Sparkfun. 


   Author: Derek Chadwick
   
   Date: 27/08/2018

*/

#ifndef _PULSE_H    /* Guard against multiple inclusion */
#define _PULSE_H




//void interruptSetup();
int pulse_ISR(unsigned int sample);



#endif

/* *****************************************************************************
 End of File
 */
