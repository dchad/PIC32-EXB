/*
   Filename: util.c
   
   Description: Various utility functions. 


   Author: Derek Chadwick
   
   Date: 27/08/2018

*/


#include <xc.h>
#include <stdlib.h>

#define SYSCLK 40000000

char bcd10000 = 0;
char bcd1000 = 0;
char bcd100 = 0 ;
char bcdtens = 0 ;
char bcdunits = 0;
int average = 0;

/*
   Clear a buffer, use to avoid all the annoying compiler warnings when
   using stdlib functions with volatile buffers.
*/
void xzero(volatile char *buf, int len)
{
   int ii;
   
   for (ii = 0; ii < len; ii++)
   {
      buf[ii] = '\0';
   }
   return;
}

/*
   Copy a string segment specified by start and end indices. 
   Start and end values must be 0...strlen()-1, with start
   being less than the end value.
*/
int xstrcpy(char *out_buf, char *in_buf, int start, int end)
{
   int ii, len, result, ij;
   
   result = -1;
   len = strlen(in_buf);
   if ((start >= 0) && (end > start) && (end < len))
   {
      ij = 0;
      for (ii = start; ii <= end; ii++)
      {
         out_buf[ij] = in_buf[ii];
         ij++;
      }
      result = ij;
   }
   return(result);
}

void delay_ms(unsigned t)
{
   T1CON = 0x8000; // enable Timer1, source PBCLK, 1:1 prescaler
   while(t--)
   {
      TMR1 = 0;
      while(TMR1 < SYSCLK/1000); // wait 1ms
   }
   // disable Timer1
   T1CONCLR = 0x8000;
   
   return;
}


// Assumes the SYSCLK and PBCLK frequencies are the same
// Occupies Timer1
void delay_us(unsigned t)
{
   T1CON = 0x8000; // enable Timer1, source PBCLK, 1:1 prescaler
   // delay 100us per loop until less than 100us remain
   while(t >= 100)
   {
      t-=100;
      TMR1 = 0;
      while(TMR1 < SYSCLK/10000); // wait 100us
   }
   // delay 10us per loop until less than 10us remain
   while(t >= 10)
   {
      t-=10;
      TMR1 = 0;
      while(TMR1 < SYSCLK/100000); // wait 10us
   }
   // delay 1us per loop until finished
   while(t > 0)
   {
      t--;
      TMR1 = 0;
      while(TMR1 < SYSCLK/1000000); // wait 1us
   }
   // disable Timer1
   T1CONCLR = 0x8000;
   
   return;
}



void binary_to_ASCIIconvert(int n) 
{
   bcd10000 = 0x00;
   bcd1000 = 0x00;
   bcd100 = 0x00;
   bcdtens = 0x00;
   bcdunits = 0x00;
   while (n >= 10000) 
   {
      n = n-10000;
      bcd10000 = bcd10000 + 1;
   }
   while (n >= 1000) 
   {
      n = n-1000;
      bcd1000 = bcd1000 + 1;
   }
   while (n >= 100) 
   {
      n = n - 100;
      bcd100 = bcd100 + 1;
   }
   while (n >= 10) 
   {
      n = n - 10;
      bcdtens = bcdtens + 1;
   }

   //bcdunits = n;
   bcd10000 = bcd10000 + 0x30;
   bcd1000 = bcd1000 + 0x30;
   bcd100 = bcd100 + 0x30;
   bcdtens = bcdtens + 0x30;
   bcdunits = n + 0x30;
}

char *ltrim(char *s)
{
    while(isspace(*s)) s++;
    return s;
}

char *rtrim(char *s)
{
    char* back = s + strlen(s);
    while(isspace(*--back));
    *(back+1) = '\0';
    return s;
}

char *trim(char *s)
{
    return rtrim(ltrim(s));
}

void uppercase(char *s)
{
   while ( *s != '\0' )
   {
      *s = toupper((unsigned char) *s);
      ++s;
   }
   
   return;
}

int replacechar(char *str, char orig, char rep) 
{
    char *ix = str;
    int n = 0;
    while((ix = strchr(ix, orig)) != NULL) 
    {
        *ix++ = rep;
        n++;
    }
    return(n);
}

int string_to_int(char *str)
{
  int i_result;
  int i_sign;

  i_result = 0;
  i_sign = 1;
  while (('-' == (*str)) || ((*str) == '+'))
  {
      if (*str == '-')
        i_sign = i_sign * -1;
      str++;
  }
  while ((*str >= '0') && (*str <= '9'))
  {
      i_result = (i_result * 10) + ((*str) - '0');
      str++;
  }
  return (i_result * i_sign);
}