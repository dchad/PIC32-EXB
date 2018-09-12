/*
   Filename: graphics.c
   
   Description: Graphics functions for the OPEN-SMART TFT LCD screen.
                Test functions ported to PIC32 C code from the Arduino C++ code
                provided by the manufacturer. 


   Author: Derek Chadwick
   
   Date: 27/08/2018

*/

#include <stdlib.h>
#include "tft.h"
#include "util.h"
#include "graphics.h"

#define TFT_WIDTH 320
#define TFT_HEIGHT 240

/* WINDOW FUNCTIONS */
void lcd_draw_main_frame()
{
   
}

void lcd_draw_setup_frame()
{
   
}

/* TEXT FUNCTIONS */
void lcd_write_bpm()
{
   
}

void lcd_write_rpm()
{
   
}

void lcd_write_distance()
{
   
}

void lcd_write_speed()
{
   
}

void lcd_write_burn()
{
   
}

/* IMAGE FUNCTIONS */
void lcd_draw_splash_image()
{
   TFT_bmpDraw("minions");

   return;   
}

void lcd_draw_pulse_line()
{
   
}

/* TEST FUNCTIONS */

void test_lcd() 
{
   TFT_reset();
	//TFT_setBacklight(100);//0~255
	//delay_ms(1000);
	testText();
	//delay_ms(1000);
   //testRects(GREEN);
	//delay_ms(1000);
 	//testFastLines(RED, BLUE);
	//delay_ms(1000);
	//testFilledRects(YELLOW, MAGENTA);
	//delay_ms(1000);
	//testFilledCircles(10, MAGENTA);
	//delay_ms(1000);
	//testCircles(10, WHITE);
	//delay_ms(1000);
	//testTriangles();
	//delay_ms(1000);
	//testFilledTriangles();
	//delay_ms(1000);
	//testRoundRects();
	//delay_ms(1000);
	//testFilledRoundRects();
   //delay_ms(1000);
   //testLines(CYAN);
 	//delay_ms(1000);
   //lcd_draw_splash_image();
   
   return;
}


void testLines(unsigned int color) {
  unsigned long start, t;
  int           x1, y1, x2, y2,
                w = 320,
                h = 240;

  TFT_fillScreen(BLACK);
  

  x1 = y1 = 0;
  y2    = h - 1;
  
  for(x2=0; x2<w; x2+=6) TFT_drawLine(x1, y1, x2, y2, color);
  x2    = w - 1;
  for(y2=0; y2<h; y2+=6) TFT_drawLine(x1, y1, x2, y2, color);


  TFT_fillScreen(BLACK);

  x1    = w - 1;
  y1    = 0;
  y2    = h - 1;

  for(x2=0; x2<w; x2+=6) TFT_drawLine(x1, y1, x2, y2, color);
  x2    = 0;
  for(y2=0; y2<h; y2+=6) TFT_drawLine(x1, y1, x2, y2, color);


  TFT_fillScreen(BLACK);

  x1    = 0;
  y1    = h - 1;
  y2    = 0;
  for(x2=0; x2<w; x2+=6) TFT_drawLine(x1, y1, x2, y2, color);
  x2    = w - 1;
  for(y2=0; y2<h; y2+=6) TFT_drawLine(x1, y1, x2, y2, color);


  TFT_fillScreen(BLACK);

  x1    = w - 1;
  y1    = h - 1;
  y2    = 0;

  for(x2=0; x2<w; x2+=6) TFT_drawLine(x1, y1, x2, y2, color);
  x2    = 0;
  for(y2=0; y2<h; y2+=6) TFT_drawLine(x1, y1, x2, y2, color);

}

void testFastLines(unsigned int color1, unsigned int color2) {

  int           x, y, w = 320, h = 240;
  TFT_fillScreen(BLACK);
  for(y=0; y<h; y+=5) TFT_drawFastHLine(0, y, w, color1);
  for(x=0; x<w; x+=5) TFT_drawFastVLine(x, 0, h, color2);

}

void testFilledRects(unsigned int color1, unsigned int color2) {
  int           n, i, i2,
                cx = 320  / 2 - 1,
                cy = 240 / 2 - 1;

  TFT_fillScreen(BLACK);
  n = 240;
  for(i=n; i>0; i-=6) {
    i2    = i / 2;

    TFT_fillRect(cx-i2, cy-i2, i, i, color1);
    // Outlines are not included in timing results
    TFT_drawRect(cx-i2, cy-i2, i, i, color2);
  }
}

void testFilledCircles(int radius, unsigned int color) {
  int x, y, w = 320, h = 240, r2 = radius * 2;

  TFT_fillScreen(BLACK);
  for(x=radius; x<w; x+=r2) {
    for(y=radius; y<h; y+=r2) {
      TFT_fillCircle(x, y, radius, color);
    }
  }
}

void testCircles(int radius, unsigned int color) {
  int           x, y, r2 = radius * 2,
                w = 320  + radius,
                h = 240 + radius;

  // Screen is not cleared for this one -- this is
  // intentional and does not affect the reported time.
  for(x=0; x<w; x+=r2) {
    for(y=0; y<h; y+=r2) {
      TFT_drawCircle(x, y, radius, color);
    }
  }
}

void testText() {
	TFT_fillScreen(BLACK);
   TFT_setCursor(0, 0);
   TFT_setTextColor(YELLOW); 
	TFT_setTextSize(2);
   TFT_print_string("1234.56");
	TFT_println();
	TFT_print_string("1 + 1 = 2");
	TFT_println();
   TFT_setTextColor(RED);    
	TFT_setTextSize(3);
   unsigned int a = 0xABCD;
   TFT_print_int32_base(a,HEX);
   TFT_println();
   TFT_setTextColor(GREEN);
   TFT_print_string("OPEN-SMART TFT");
	TFT_println();
}

void testRects(unsigned int color) {
  int           n, i, i2,
                cx = 320 / 2,
                cy = 240 / 2;

  TFT_fillScreen(BLACK);
  n     = min(320, 240);
  for(i=2; i<n; i+=6) {
    i2 = i / 2;
    TFT_drawRect(cx-i2, cy-i2, i, i, color);
  }

}
void testTriangles() {

  int           n, i, cx = 320  / 2 - 1,
                      cy = 240 / 2 - 1;

  TFT_fillScreen(BLACK);
  n     = min(cx, cy);

  for(i=0; i<n; i+=5) {
    TFT_drawTriangle(
      cx    , cy - i, // peak
      cx - i, cy + i, // bottom left
      cx + i, cy + i, // bottom right
      TFT_color565(0, 0, i));
  }

}

void testFilledTriangles() {
  int           i, cx = 320  / 2 - 1,
                   cy = 240 / 2 - 1;

  TFT_fillScreen(BLACK);
  for(i=min(cx,cy); i>10; i-=5) {
    TFT_fillTriangle(cx, cy - i, cx - i, cy + i, cx + i, cy + i,
      TFT_color565(0, i, i));
    TFT_drawTriangle(cx, cy - i, cx - i, cy + i, cx + i, cy + i,
      TFT_color565(i, i, 0));
  }

}

void testRoundRects() {
  int           w, i, i2,
                cx = 320 / 2 - 1,
                cy = 240 / 2 - 1;

  TFT_fillScreen(BLACK);
  w     = 240;

  for(i=0; i<w; i+=6) {
    i2 = i / 2;
    TFT_drawRoundRect(cx-i2, cy-i2, i, i, i/8, TFT_color565(i, 0, 0));
  }

}

void testFilledRoundRects() {
  int           i, i2,
                cx = 320 / 2 - 1,
                cy = 240 / 2 - 1;

  TFT_fillScreen(BLACK);
  for(i=240; i>20; i-=6) {
    i2 = i / 2;
    TFT_fillRoundRect(cx-i2, cy-i2, i, i, i/8, TFT_color565(0, i, 0));
  }

}

