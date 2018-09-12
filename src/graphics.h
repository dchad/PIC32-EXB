/*
   Filename: graphics.c
   
   Description: Graphics functions for the OPEN-SMART TFT LCD screen.
                Test functions ported to PIC32 C code from the Arduino C++ code
                provided by the manufacturer. 


   Author: Derek Chadwick
   
   Date: 27/08/2018

*/

#ifndef _GRAPHICS_H    
#define _GRAPHICS_H

/* WINDOW FUNCTIONS */
/* TEXT FUNCTIONS */
/* IMAGE FUNCTIONS */


/* TEST FUNCTIONS */
void test_lcd();
void testLines(unsigned int color);
void testFastLines(unsigned int color1, unsigned int color2);
void testFilledRects(unsigned int color1, unsigned int color2);
void testFilledCircles(int radius, unsigned int color);
void testCircles(int radius, unsigned int color);
void testText();
void testRects(unsigned int color);
void testTriangles();
void testFilledTriangles();
void testRoundRects();
void testFilledRoundRects();

#endif /* _EXAMPLE_FILE_NAME_H */

/* *****************************************************************************
 End of File
 */
