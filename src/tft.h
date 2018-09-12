/*
   Filename: tft.h
   
   Description: Functions to control an OPEN-SMART 2.4" TFT LCD screen.
                Ported to PIC32 C code from the Aurduino C++ code 
                provided by the manufacturer. 


   Author: Derek Chadwick
   
   Date: 27/08/2018

*/


#ifndef _OS_SERIALTFT_H__
#define _OS_SERIALTFT_H__


#define BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF

#define TEST_CMD 0X00
#define SET_READ_CURSOR 0X01
#define SET_TEXTCOLOR 0X02
#define SET_TEXTSIZE 0X03
#define SET_ROTATION 0X04
#define RESET 0X05
#define SET_BACKLIGHT 0X06
#define READ_TOUCH 0X07

#define PRINTLN 0x10
#define PRINT_CHAR_ARRAY 0X11
#define PRINT_INT_8 0X12
#define PRINT_INT_16 0X13
#define PRINT_INT_32 0X14


#define FILL_SREEN 0X20
#define DRAW_PIXEL 0X21
#define DRAW_FASTVLINE 0X22
#define DRAW_FASTHLINE 0X23
#define DRAW_LINE 0X24
#define DRAW_RECT 0X25
#define FILL_RECT 0X26
#define DRAW_CIRCLE 0X27
#define FILL_CIRCLE 0X28
#define DRAW_TRIANGLE 0X29
#define FILL_TRIANGLE 0X2A
#define DRAW_ROUNDRECT 0X2B
#define FILL_ROUNDRECT 0X2C

#define DRAW_BMP 0X30

#define WRITE_READ_BAUD 0X40
#define READ_VERSION 0X41
#define READ_DRIVER_ID 0X42
#define READ_RESOLUTION 0X43

////////////////////////////////////////////

#define FEEDBACK_OK 0x6F
#define FEEDBACK_E1 0x65
#define FEEDBACK_E2 0x66

#define SIGNED 1
#define UNSIGNED 0

#define DEC 0x0A
#define HEX 0x10
#define OCT 0x08
#define BIN 0x02

#define BAUD_9600 0 // Baud rates for the TFT, see user manual pg 19.
#define BAUD_19200 1
#define BAUD_38400 2
#define BAUD_57600 3
#define BAUD_115200 4


void tft_set_status(int status);
int tft_get_status();

void TFT_configure(int speed);
void TFT_sendCommand(char cmd[], int length);
void TFT_test();
void TFT_setCursor(int x, int y);
void TFT_readCursor(int *x, int *y);
void TFT_setTextColor(int color);
void TFT_setTextSize(int size) ;
void TFT_setRotation(int rota); //rota = 0, 1, 2, 3
void TFT_reset();
void TFT_setBacklight(int bightness);

void TFT_println();
void TFT_print_string(const char str[]);
void TFT_print_char(char c);
void TFT_print_int8(int b, int base);
void TFT_print_int16(int n);
void TFT_print_int16_base(int n, int base);
void TFT_print_int32(int n);
void TFT_print_int32_base(int n, int base);
void TFT_println_string(const char str[]);
void TFT_println_char(char c);
void TFT_println_int8(int b, int base);
void TFT_println_int16(int n);
void TFT_println_int16_base(int n, int base);
void TFT_println_int32(int n);
void TFT_println_int32_base(int n, int base);

void TFT_fillScreen(int color);
void TFT_drawPixel(int x, int y, int color);
void TFT_drawFastHLine(int x0, int y0, int w, int color);
void TFT_drawFastVLine(int x0, int y0, int h, int color);
void TFT_drawLine(int x0, int y0, int x1, int y1, int color);
void TFT_drawRect(int x, int y, int w, int h, int color);
void TFT_fillRect(int x, int y, int w, int h, int color);
void TFT_drawCircle(int x0, int y0, int r, int color);
void TFT_fillCircle(int x0, int y0, int r, int color);
void TFT_drawTriangle(int x0, int y0, int x1, int y1, int x2, int y2, int color);
void TFT_fillTriangle(int x0, int y0, int x1, int y1, int x2, int y2, int color);
void TFT_drawRoundRect(int x0, int y0, int w, int h, int r, int color);
void TFT_fillRoundRect(int x0, int y0, int w, int h, int r, int color);
void TFT_bmpDraw(char *filename);
int TFT_color565(int r, int g, int b);




	

#endif
