/*
   Filename: tft.c
   
   Description: Functions to control an OPEN-SMART 2.4" TFT LCD screen.
                Ported to PIC32 C code from the Aurduino C++ code 
                provided by the manufacturer. 


   Author: Derek Chadwick
   
   Date: 27/08/2018

*/



#include "tft.h"
#include "uart.h"

char feedbackBuf[6];
int tft_ready;
int tft_timeout;

void tft_set_status(int status)
{
   tft_ready = status;
}

int tft_get_status()
{
   return(tft_ready);
}

/**************************************/
// Send baud rate to LCD, UART2 will have to be reconfigured 
// after sending.
void TFT_configure(int speed)
{
   char temp[5];
   
   temp[0] = 0x7e;
   temp[1] = 3;   
   temp[2] = WRITE_READ_BAUD;
   temp[3] = speed;
   temp[4] = 0xef;
   
   // Send baud rate to LCD, default is 9600 bps.
   // 0 = 9600
   // 1 = 19200
   // 2 = 38400
   // 3 = 57600
   // 4 = 115200
   
   Serial_Transmit_U2(temp, 5);
   
   return;
}


/**********************************************/
void TFT_sendCommand(char cmd[], int length)
{
  char dat[60]; // Maximum command message length for tft.
  int i;
  
  if (length > 60)
     length = 60;  //TODO: do some proper error checking here.
  
  dat[0] = 0x7e;
  dat[1] = length+1;
  for(i = 0; i < length; i++)
  {
    dat[i+2] = cmd[i];
  }
  dat[length+2] = 0xef;
  Serial_Transmit_U2(dat, length+3);
  tft_ready = 0;
  tft_timeout = 0;
  while ((tft_ready != 1) && (tft_timeout < 10000))
  {                       // Wait till tft is ready before continuing.
     delay_ms(2);         // UART2 ISR will set tft_ready to 1 when tft responds.
     tft_timeout++;
  }
  
  return;
}



//----------------------------------------
void TFT_test()
{
   char temp[1];
	temp[0] = TEST_CMD;
	TFT_sendCommand(temp,1);
	//while(TFT_checkFeedback()!= FEEDBACK_OK);
}

void TFT_reset()
{
	char temp[1];
	temp[0] = RESET;
	TFT_sendCommand(temp,1);
	//while(TFT_checkFeedback()!= FEEDBACK_OK);
}

void TFT_setCursor(int x, int y)
{
	char temp[5];
	temp[0] = SET_READ_CURSOR;
	temp[1] = x>>8;
	temp[2] = x;
	temp[3] = y>>8;
	temp[4] = y;
	TFT_sendCommand(temp,5);
	//while(TFT_checkFeedback()!= FEEDBACK_OK);
}

void TFT_readCursor(int *x, int *y)
{
	char temp[1];
	temp[0] = SET_READ_CURSOR;
	TFT_sendCommand(temp,1);
   
   //TODO: do something with this.
   
	//while(TFT_checkFeedback()!= SET_READ_CURSOR);
	//x = ((int)feedbackBuf[1]<<8) + (int)feedbackBuf[2];
	//y = ((int)feedbackBuf[3]<<8) + (int)feedbackBuf[4];
	//while(TFT_checkFeedback()!= FEEDBACK_OK);
}

void TFT_setTextColor(int color)
{
	char temp[3];
	temp[0] = SET_TEXTCOLOR;
	temp[1] = color>>8;
	temp[2] = color;
	TFT_sendCommand(temp,3);
	//while(TFT_checkFeedback()!= FEEDBACK_OK);
}

void TFT_setTextSize(int size) 
{
   char temp[2];
	temp[0] = SET_TEXTSIZE;
	temp[1] = size;
	TFT_sendCommand(temp,2);
	//while(TFT_checkFeedback()!= FEEDBACK_OK);
}

void TFT_setRotation(int rota) //rota = 0, 1, 2, 3
{
	char temp[2];
	temp[0] = SET_ROTATION;
	temp[1] = rota;
	TFT_sendCommand(temp,2);
	//while(TFT_checkFeedback()!= FEEDBACK_OK);
}

void TFT_setBacklight(int bightness) 
{
	char temp[2];
	temp[0] = SET_BACKLIGHT;
	temp[1] = bightness;
	TFT_sendCommand(temp,2);
	//while(TFT_checkFeedback()!= FEEDBACK_OK);
}


void TFT_println()
{
   char temp[1];
	temp[0] = PRINTLN;
	TFT_sendCommand(temp,1);
	//while(TFT_checkFeedback()!= FEEDBACK_OK);
}

void TFT_print_string(const char str[])
{
	char temp[61];//can not be more than 61bytes a time
	int i = 0;
	temp[0] = PRINT_CHAR_ARRAY;
	while(str[i] != '\0')
	{
		temp[i+1] = str[i];
		i++;
	}
	TFT_sendCommand(temp,i+1);
	//while(TFT_checkFeedback()!= FEEDBACK_OK);
}

void TFT_print_char(char c)
{
  	char temp[4];
	temp[0] = PRINT_INT_8;
	temp[1] = SIGNED;
	temp[2] = 0;//format, char have no format
	temp[3] = c;
	TFT_sendCommand(temp,4);
	//while(TFT_checkFeedback()!= FEEDBACK_OK);
}

//base: 
//#define DEC 10
//#define HEX 16
//#define OCT 8
//#define BIN 2
void TFT_print_int8(int b, int base)
{
  	char temp[4];
	temp[0] = PRINT_INT_8;
	temp[1] = UNSIGNED;
	temp[2] = base;//
	temp[3] = b;
	TFT_sendCommand(temp,4);
	//while(TFT_checkFeedback()!= FEEDBACK_OK);
}

void TFT_print_int16(int n)
{
  	char temp[5];
	temp[0] = PRINT_INT_16;
	temp[1] = SIGNED;
	temp[2] = 0;//format, char have no format
	temp[3] = n>>8;
	temp[4] = n;
	TFT_sendCommand(temp,5);
	//while(TFT_checkFeedback()!= FEEDBACK_OK);
}

void TFT_print_int16_base(int n, int base)
{
	char temp[5];
	temp[0] = PRINT_INT_16;
	temp[1] = UNSIGNED;
	temp[2] = base;//
	temp[3] = n>>8;
	temp[4] = n;

	TFT_sendCommand(temp,5);
	//while(TFT_checkFeedback()!= FEEDBACK_OK);
}

void TFT_print_int32(int n)
{
  	char temp[7];
	temp[0] = PRINT_INT_32;
	temp[1] = SIGNED;
	temp[2] = 0;//format, char have no format
	temp[3] = n>>24;
	n <<= 8;
	temp[4] = n>>24;
	n <<= 8;
	temp[5] = n>>24;
	n <<= 8;
	temp[6] = n>>24;
	TFT_sendCommand(temp,7);
	//while(TFT_checkFeedback()!= FEEDBACK_OK);
}

void TFT_print_int32_base(int n, int base)
{
  	char temp[7];
	temp[0] = PRINT_INT_32;
	temp[1] = UNSIGNED;
	temp[2] = base;//
	temp[3] = n>>24;
	temp[4] = n>>16;
	temp[5] = n>>8;
	temp[6] = n;
	TFT_sendCommand(temp,7);
	//while(TFT_checkFeedback()!= FEEDBACK_OK);
}

void TFT_println_string(const char str[])
{
	TFT_print_string(str);
	TFT_println();
}

void TFT_println_char(char c)
{
	TFT_print_char(c);
	TFT_println();
}

void TFT_println_int8(int b, int base)
{
	TFT_print_int8(b, base);
	TFT_println(); 
}

void TFT_println_int16(int n)
{
	TFT_print_int16(n);
	TFT_println();	
}

void TFT_println_int16_base(int n, int base)
{
	TFT_print_int16_base(n,base);
	TFT_println();
}

void TFT_println_int32(int n)
{
	TFT_print_int32(n);
	TFT_println();	

}

void TFT_println_int32_base(int n, int base)
{
	TFT_print_int32_base(n,base);
	TFT_println();

}

void TFT_fillScreen(int color)
{
	char temp[3];
	temp[0] = FILL_SREEN;
	temp[1] = color>>8;
	temp[2] = color;
	TFT_sendCommand(temp,3);
	//while(TFT_checkFeedback()!= FEEDBACK_OK);
}

void TFT_drawPixel(int x, int y, int color)
{
	char temp[7];
	temp[0] = DRAW_PIXEL;
	temp[1] = x>>8;
	temp[2] = x;
	temp[3] = y>>8;
	temp[4] = y;
	temp[5] = color>>8;
	temp[6] = color;
	TFT_sendCommand(temp,7);
	//while(TFT_checkFeedback()!= FEEDBACK_OK);
}
void TFT_drawFastHLine(int x0, int y0, int w, int color)
{
	char temp[9];
	temp[0] = DRAW_FASTHLINE;
	temp[1] = x0>>8;
	temp[2] = x0;
	temp[3] = y0>>8;
	temp[4] = y0;
	temp[5] = w>>8;
	temp[6] = w;
	temp[7] = color>>8;
	temp[8] = color;
	TFT_sendCommand(temp,9);
	//while(TFT_checkFeedback()!= FEEDBACK_OK);
}

void TFT_drawFastVLine(int x0, int y0, int h, int color)
{
	char temp[9];
	temp[0] = DRAW_FASTVLINE;
	temp[1] = x0>>8;
	temp[2] = x0;
	temp[3] = y0>>8;
	temp[4] = y0;
	temp[5] = h>>8;
	temp[6] = h;
	temp[7] = color>>8;
	temp[8] = color;
	TFT_sendCommand(temp,9);
	//while(TFT_checkFeedback()!= FEEDBACK_OK);
}

void TFT_drawLine(int x0, int y0, int x1, int y1, int color)
{
	char temp[11];
	temp[0] = DRAW_LINE;
	temp[1] = x0>>8;
	temp[2] = x0;
	temp[3] = y0>>8;
	temp[4] = y0;
	temp[5] = x1>>8;
	temp[6] = x1;
	temp[7] = y1>>8;
	temp[8] = y1;
	temp[9] = color>>8;
	temp[10] = color;
	TFT_sendCommand(temp,11);
	//while(TFT_checkFeedback()!= FEEDBACK_OK);
}

void TFT_drawRect(int x, int y, int w, int h,
        int color)
{
	char temp[11];
	temp[0] = DRAW_RECT;
	temp[1] = x>>8;
	temp[2] = x;
	temp[3] = y>>8;
	temp[4] = y;
	temp[5] = w>>8;
	temp[6] = w;
	temp[7] = h>>8;
	temp[8] = h;
	temp[9] = color>>8;
	temp[10] = color;
	TFT_sendCommand(temp,11);
	//while(TFT_checkFeedback()!= FEEDBACK_OK);
}

void TFT_fillRect(int x, int y, int w, int h, int color)
{
	char temp[11];
	temp[0] = FILL_RECT;
	temp[1] = x>>8;
	temp[2] = x;
	temp[3] = y>>8;
	temp[4] = y;
	temp[5] = w>>8;
	temp[6] = w;
	temp[7] = h>>8;
	temp[8] = h;
	temp[9] = color>>8;
	temp[10] = color;
	TFT_sendCommand(temp,11);
	//while(TFT_checkFeedback()!= FEEDBACK_OK);
}

void TFT_drawCircle(int x0, int y0, int r, int color)
{
	char temp[9];
	temp[0] = DRAW_CIRCLE;
	temp[1] = x0>>8;
	temp[2] = x0;
	temp[3] = y0>>8;
	temp[4] = y0;
	temp[5] = r>>8;
	temp[6] = r;
	temp[7] = color>>8;
	temp[8] = color;
	TFT_sendCommand(temp,9);
	//while(TFT_checkFeedback()!= FEEDBACK_OK);
}

void TFT_fillCircle(int x0, int y0, int r, int color)
{
	char temp[9];
	temp[0] = FILL_CIRCLE;
	temp[1] = x0>>8;
	temp[2] = x0;
	temp[3] = y0>>8;
	temp[4] = y0;
	temp[5] = r>>8;
	temp[6] = r;
	temp[7] = color>>8;
	temp[8] = color;
	TFT_sendCommand(temp,9);
	//while(TFT_checkFeedback()!= FEEDBACK_OK);
}

void TFT_drawTriangle(int x0, int y0, int x1, int y1, int x2, int y2, int color)
{
	char temp[15];
	temp[0] = DRAW_TRIANGLE;
	temp[1] = x0>>8;
	temp[2] = x0;
	temp[3] = y0>>8;
	temp[4] = y0;
	temp[5] = x1>>8;
	temp[6] = x1;
	temp[7] = y1>>8;
	temp[8] = y1;
	temp[9] = x2>>8;
	temp[10] = x2;
	temp[11] = y2>>8;
	temp[12] = y2;
	temp[13] = color>>8;
	temp[14] = color;
	TFT_sendCommand(temp,15);
	//while(TFT_checkFeedback()!= FEEDBACK_OK);
}

void TFT_fillTriangle(int x0, int y0, int x1, int y1, int x2, int y2, int color)
{
	char temp[15];
	temp[0] = FILL_TRIANGLE;
	temp[1] = x0>>8;
	temp[2] = x0;
	temp[3] = y0>>8;
	temp[4] = y0;
	temp[5] = x1>>8;
	temp[6] = x1;
	temp[7] = y1>>8;
	temp[8] = y1;
	temp[9] = x2>>8;
	temp[10] = x2;
	temp[11] = y2>>8;
	temp[12] = y2;
	temp[13] = color>>8;
	temp[14] = color;
	TFT_sendCommand(temp,15);
	//while(TFT_checkFeedback()!= FEEDBACK_OK);
}

void TFT_drawRoundRect(int x0, int y0, int w, int h, int r, int color)
{
	char temp[13];
	temp[0] = DRAW_ROUNDRECT;
	temp[1] = x0>>8;
	temp[2] = x0;
	temp[3] = y0>>8;
	temp[4] = y0;
	temp[5] = w>>8;
	temp[6] = w;
	temp[7] = h>>8;
	temp[8] = h;
	temp[9] = r>>8;
	temp[10] = r;
	temp[11] = color>>8;
	temp[12] = color;
	TFT_sendCommand(temp,13);
	//while(TFT_checkFeedback()!= FEEDBACK_OK);
}

void TFT_fillRoundRect(int x0, int y0, int w, int h, int r, int color)
{
	char temp[13];
	temp[0] = FILL_ROUNDRECT;
	temp[1] = x0>>8;
	temp[2] = x0;
	temp[3] = y0>>8;
	temp[4] = y0;
	temp[5] = w>>8;
	temp[6] = w;
	temp[7] = h>>8;
	temp[8] = h;
	temp[9] = r>>8;
	temp[10] = r;
	temp[11] = color>>8;
	temp[12] = color;
	TFT_sendCommand(temp,13);
	
   return;
}

void TFT_bmpDraw(char *filename)
{
	char temp[9];//can not be more than 8 byte file name
	int i = 0;
	temp[0] = DRAW_BMP;
	while(*(filename+i)!='\0')
	{
		temp[i+1] = *(filename+i);
		i++;
	}
	TFT_sendCommand(temp,i+1);
	
   return;
}



int TFT_color565(int r, int g, int b)
{
	return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
}

/**************************/
//if someone touch, it will return 1. otherwise return 0;
int TFT_touch()
{
   int touchX;
   int touchY;
   
	//if(TFT_checkFeedback() == READ_TOUCH)
	//{
	    
	//	touchX = ((int)feedbackBuf[1]<<8) + (int)feedbackBuf[2];
	//	touchY = ((int)feedbackBuf[3]<<8) + (int)feedbackBuf[4];
	//	return 1;
	//}
	//else return 0;
   return(1);
}

