/* ************************************************************************** */
/*

  Author: Derek Chadwick

  File Name: exbc_main.c

  Description: Controller for a magnetic resistance exercise bike using 
               a PIC32MX270F256B microcontroller. The PIC32 runs at 40MHz
               using the internal Fast Resistor Capacitor clock.
               PC communications is via serial UART1 and an FTDI232 USB
               to serial adapter board.
  
  Date: 22/08/2018
  
  NOTE: plib is DEPRECATED for the XC compiler and will not be supported in future,
   plib will only work with XC v1.40 or earlier, download and install XC32 v1.40 and 
   plib peripheral library from microchip website.
   Set preprocessor directive _SUPPRESS_PLIB_WARNING
   EXTRA NOTE: Harmony framework is a piece of shit bloatware, do not use. 
  
 */
/* ************************************************************************** */

// DEVCFG3
// USERID = No Setting
#pragma config PMDL1WAY = ON            // Peripheral Module Disable Configuration (Allow only one reconfiguration)
#pragma config IOL1WAY = ON             // Peripheral Pin Select Configuration (Allow only one reconfiguration)
#pragma config FUSBIDIO = ON            // USB USID Selection (Controlled by the USB Module)
#pragma config FVBUSONIO = ON           // USB VBUS ON Selection (Controlled by USB Module)

// DEVCFG2
#pragma config FPLLIDIV = DIV_2         // PLL Input Divider (2x Divider)
#pragma config FPLLMUL = MUL_20         // PLL Multiplier (20x Multiplier)
#pragma config UPLLIDIV = DIV_1         // USB PLL Input Divider (2x Divider)
#pragma config UPLLEN = OFF             // USB PLL Enable (Disabled and Bypassed)
#pragma config FPLLODIV = DIV_2         // System PLL Output Clock Divider (PLL Divide by 2)

// DEVCFG1
#pragma config FNOSC = FRCPLL           // Oscillator Selection Bits (Fast RC Osc with PLL)
#pragma config FSOSCEN = OFF            // Secondary Oscillator Enable (Disabled)
#pragma config IESO = OFF               // Internal/External Switch Over (Disabled)
#pragma config POSCMOD = OFF            // Primary Oscillator Configuration (Primary osc disabled)
#pragma config OSCIOFNC = OFF           // CLKO Output Signal Active on the OSCO Pin (Disabled)
#pragma config FPBDIV = DIV_1           // Peripheral Clock Divisor (Default Pb_Clk is Sys_Clk/8)
#pragma config FCKSM = CSDCMD           // Clock Switching and Monitor Selection (Clock Switch Disable, FSCM Disabled)
#pragma config WDTPS = PS1048576        // Watchdog Timer Postscaler (1:1048576)
#pragma config WINDIS = OFF             // Watchdog Timer Window Enable (Watchdog Timer is in Non-Window Mode)
#pragma config FWDTEN = OFF             // Watchdog Timer Enable (WDT Enabled)
#pragma config FWDTWINSZ = WINSZ_25     // Watchdog Timer Window Size (Window Size is 25%)

// DEVCFG0
#pragma config JTAGEN = OFF              // JTAG Enable (JTAG Port Enabled)
#pragma config ICESEL = ICS_PGx1        // ICE/ICD Comm Channel Select (Communicate on PGEC1/PGED1)
#pragma config PWP = OFF                // Program Flash Write Protect (Disable)
#pragma config BWP = OFF                // Boot Flash Write Protect bit (Protection Disabled)
#pragma config CP = OFF                 // Code Protect (Protection Disabled)

#include <xc.h>
#include <plib.h>
//#include <sys/attribs.h> 
//#include <stdlib.h>

#include "uart.h"
//#include "tft.h"
#include "util.h"
//#include "graphics.h"
#include "pulse.h"
#include "i2c.h"

// Configuration Bit settings
// SYSCLK = 40 MHz (8MHz Crystal/ FPLLIDIV * FPLLMUL / FPLLODIV)
// PBCLK = 40 MHz
// Primary Osc w/PLL (XT+,HS+,EC+PLL)
// NOTE: ALWAYS SET THE PBCLK FREQUENCY WHEN USING THE TIMERS.
// Assumes the SYSCLK and PBCLK frequencies are the same 
//<<<NOTE:::DO NOT ASSUME, ALWAYS SET FPBDIV !!!!!!!!!!!!!!!!!>>>


#define SYSCLK 40000000L
#define BLINK_DELAY 1000 // LED toggle delay in ms
#define FPB 40000000
#define BAUDRATE 9600
#define BRGVAL ((FPB / BAUDRATE) /16) - 1
#define U_ENABLE 0x8000		// enable the UART
#define U_TXRX	 0x1400		// enable transmission and reception
#define CORE_TICKS 20000000 // 10 M ticks (one second)
#define CORE_TICKS_10SEC 200000000 // 100 M ticks (ten seconds)

#define VOLTS_PER_COUNT (3.3/1024)
#define set_PR2(seconds) (seconds * SYSCLK / 256) // Timer 2 period = (20000000/256 * secs))

static char uart1_input_buffer[256];
static char uart2_input_buffer[256];
static unsigned int user_command_recvd;
static unsigned int user_command_length;
static volatile unsigned  int tft_msg_recvd;
static volatile unsigned  int tft_msg_length;
static unsigned int rpm_counter;
static char user_command_buffer[256];
static char user_msg_buffer[256];
static char tft_command_buffer[60]; // Max LCD command length is 60 bytes.
static volatile unsigned int pulse_sample;
static volatile unsigned int pulse_sample_buffer[500]; // 500 samples per second.
static volatile unsigned int pulse_sample_index;
static volatile unsigned int pulse_sample_buffer_full;
static volatile unsigned int bpm; // pulse rate.
static volatile unsigned int bpm_interval; // 15 second send interval.
static volatile unsigned int get_bpm;
static unsigned int prox_range;
static unsigned int get_prox_range;
static unsigned int rpm;
static unsigned int rpm_detect;
static unsigned int rpm_interval;
static unsigned int rpm_detect_count;
static unsigned int rpm_last_detect_secs;
static unsigned int rpm_last_count;
static unsigned int average_rpm;
static unsigned int get_rpm;
static unsigned int user_age;
static unsigned int user_weight;
static unsigned int user_gender; // 0 == MALE, 1 == FEMALE.
static unsigned int increase_resistance;
static unsigned int decrease_resistance;
static unsigned int resistance_level;
static unsigned int session_seconds;
static unsigned int session_minutes;
static unsigned int session_state; // 0 = no session, 1 = session started, 2 = session paused, 3 = session end.
static unsigned int blink_count;
static unsigned int millisecond_counter; // Timing the rpm/cadence.
static float speed;
static float average_speed;
static float distance;
static float kilojoules;
static unsigned int debug;

// Function declarations.
int adc_read(char analog_pin);
void configure_adc();
void configure_io();
void configure_interrupts();
void initialise_vars();
void process_user_command();
float calculate_kilojoules();
void set_user_age(char *msg);
void set_user_weight(char *msg);
void set_user_gender(char *msg);
void send_kilojoules();


int main(void) 
{
   
   SYSTEMConfigPerformance(SYSCLK);
   
   user_command_recvd = 0;
   user_command_length = 0;
   tft_msg_recvd = 0;
   tft_msg_length = 0;
   pulse_sample_index = 0;
   pulse_sample_buffer_full = 0;
   bpm_interval = 0;
   get_bpm = 0;
   bpm = 0;
   prox_range = 0;
   get_prox_range = 0;
   rpm = 0;
   rpm_detect = 0;
   rpm_interval = 0;
   rpm_detect_count = 0;
   rpm_last_detect_secs = 0;
   rpm_last_count = 0;
   get_rpm = 0;
   increase_resistance = 0;
   decrease_resistance = 0;
   resistance_level = 1;
   session_seconds = 0;
   session_minutes = 0;
   session_state = 0;
   blink_count = 0;
   millisecond_counter = 0;
   speed = 0.0;
   average_speed = 0.0;
   distance = 0.0;
   debug = 0;
   xzero(uart1_input_buffer, 256); // CLEAR THE BUFFERS!!!
   xzero(uart2_input_buffer, 256);
   xzero(user_command_buffer, 256);
   xzero(user_msg_buffer, 256);
   
   configure_io();
   
   UART1_Configure(38400); // PC Communications.
   //UART2_Configure(9600);  // OPEN-SMART 2.4" TFT Display.
   
   i2c_master_setup();
   
   VL6180X_init();
   //VL6180X_clearResetFlag();
           
   configure_adc();
   
   configure_interrupts();

   //TFT_configure(0);
   //TFT_reset();
   //TFT_test();
   
   delay_ms(250);  // Wait for things to settle down or weird stuff happens to the interrupts.
   
   while(1)
   {
      //delay_ms(250);
      
      if (PORTAbits.RA4 == 0) // Test/reset button pressed.
      {
         // Test both serial ports are working.
         // Serial_Transmit_U1("Obey me and hit the button.\r\n\r\n> ");
         // test_lcd();
         // Serial_Transmit_U1("TFT test finished.\r\n\r\n> ");
         // TODO: test proximity sensor.
         // TODO: test pulse sensor.
         // TODO: test rpm sensor.
         // TODO: test h-bridge driver and motor.
         // TODO: test PC comms.
         // TODO: test motor control, invert motor control pins RB14 and RB15
         // TODO: reset session variables.
         
         switch(session_state)
         {
            case 0: session_state = 1; break; // Start Session. 
            case 1: session_state = 0; break; // End Session.
            case 2: break;
            case 3: break; // STOP: clear all the variables.
            default: session_state = 0;
         }
         
         get_rpm = 1;
         get_prox_range = 1;
         delay_ms(500);

      }

      if (PORTAbits.RA2 == 0) // Increase resistance push button pressed.
      {
         increase_resistance = 1;
         get_prox_range = 1;
      }
      
      if (PORTAbits.RA3 == 0) // Decrease resistance push button pressed.
      {
         decrease_resistance = 1;
         get_prox_range = 1;
      }

      if (get_bpm == 1)
      {
         sprintf(user_msg_buffer, "\r\nU9:BPM = %3u bpm.\r\n> ", bpm);
         Serial_Transmit_U1(user_msg_buffer);
         xzero(user_msg_buffer, 256);
         get_bpm = 0;     
      }

      if (get_rpm == 1)
      {
         
         session_minutes = session_seconds / 60;
         if (session_minutes > 0)
         {
            average_rpm = rpm_detect_count / session_minutes;
         }
         else
         {
            average_rpm = 0;
         }
         sprintf(user_msg_buffer, "\r\nU8:RPM = %3u : Average RPM = %3u : Detect = %5u : Minutes = %3u.\r\n> ", rpm, average_rpm, rpm_detect_count, session_minutes);
         Serial_Transmit_U1(user_msg_buffer);
         xzero(user_msg_buffer, 256);
        
         get_rpm = 0;     
      }
      
      if (get_prox_range == 1)
      {
         VL6180X_startSingleRangeMeasurement();
         delay_ms(25);
         if (VL6180X_isRangeResultReady() == 1)
         {
            //int valid = 0;
            prox_range = VL6180X_getRangeResult();
            //valid = VL6180X_dataValidation();
            //if (valid > 1)
            //{
            //   sprintf(user_msg_buffer, "\r\nUA:RANGE ERROR = %u\r\n> ", valid);
            //}
            //else
            //{
               sprintf(user_msg_buffer, "\r\nUA:RANGE = %3u mm.\r\n> ", prox_range);
            //}
            Serial_Transmit_U1(user_msg_buffer);
            xzero(user_msg_buffer, 256); 
            VL6180X_clearInterruptFlag(0x07); // ( CLEAR_RANGE_INT | CLEAR_ALS_INT | CLEAR_ERR_INT )
         } 
            
         get_prox_range = 0;
      }
   
      if (increase_resistance == 1) // PC command received, increase resistance.
      {
         if (prox_range > 8)
         {
            LATBbits.LATB14 = 0;
            LATBbits.LATB15 = 1;
            delay_ms(1000);
            LATBbits.LATB14 = 0;
            LATBbits.LATB15 = 0;
         }
         increase_resistance = 0;   
      }
      
      if (decrease_resistance == 1) // PC command received, decrease resistance.
      {
         if (prox_range < 24)
         {
            LATBbits.LATB14 = 1;
            LATBbits.LATB15 = 0;         
            delay_ms(1000);
            LATBbits.LATB14 = 0;
            LATBbits.LATB15 = 0;
         }
         decrease_resistance = 0;
      }
      
      // delay by BLINK_DELAY ms
      //delay_ms(25);
      //LATBINV = 0x0010; // Blink LED
   }
  
  return(0);
}

void __ISR(_CORE_TIMER_VECTOR, IPL6SOFT) CoreTimerISR(void) {
   IFS0bits.CTIF = 0;                // clear CT int flag IFS0<0>, same as IFS0CLR=0x0001
   LATBINV = 0x0020;
   
   session_seconds++;
  
   _CP0_SET_COUNT(0);                // set core timer counter to 0
   _CP0_SET_COMPARE(CORE_TICKS);     // must set CP0_COMPARE again after interrupt
}

// Timer2 Interrupt Service Routine
void __ISR(8, IPL4SOFT) Timer2IntHandler(void) {
   // READ ADC INPUT FROM PULSE SENSOR MODULE ON PIN RA0/AN0 and calculate BPM.
   
   pulse_sample = adc_read(0);
   pulse_sample_buffer[pulse_sample_index] = pulse_sample;
   pulse_sample_index++;
   bpm = pulse_ISR(pulse_sample);
   
   if (pulse_sample_index >= 500) // 2 millisecond sample rate.
   {
      pulse_sample_index = 0;
      bpm_interval++;
      if (bpm_interval > 15) // Send BPM to LCD every 15 seconds.
      {
         
         sprintf(user_msg_buffer, "\r\nBPM: %3u\r\n>", bpm);
         Serial_Transmit_U1(user_msg_buffer);
         xzero(user_msg_buffer, 256);
         sprintf(user_msg_buffer, "\r\nAN0: %4u (%5.3f volts) \r\n> ", pulse_sample, pulse_sample * VOLTS_PER_COUNT);
         xzero(user_msg_buffer, 256);
         //TFT_print_string(tft_command_buffer);
         //cannot do this here, if the tft is currently executing a command it will white screen
         //and have to be reset.
         bpm_interval = 0;
      }
   }
   
   millisecond_counter += 2;
   
   IFS0CLR = 0x0200;       // clear timer 2 int flag, IFS0<9>
} // END Timer2 ISR

void __ISR(_UART_1_VECTOR, IPL1SOFT) IntUart1Handler(void) {
   char in_ch;
   if (IFS1bits.U1RXIF) 
   {       // check if interrupt generated by a RX event
      in_ch = U1RXREG;          // TODO: check command message length and wrap buffer if necessary.
      U1TXREG = in_ch;           // Echo character back to sending terminal.
      uart1_input_buffer[user_command_length] = in_ch;
      if (in_ch == '\r') //((in_ch == '\r') || (in_ch == '\n'))
      {
         //user_command_recvd = 1;
         process_user_command();
         user_command_length = 0;
         xzero(uart1_input_buffer, 256);   
      }
      else
      {
         user_command_length++;
      }
      
      IFS1bits.U1RXIF = 0;       // Clear the RX interrupt flag.
   } 
   else if(IFS1bits.U1TXIF) { // if it is a TX interrupt 
   } 
   else if(IFS1bits.U1EIF) {  // if it is an error interrupt. check U3STA for reason
   }   
}

void __ISR(_UART_2_VECTOR, IPL2SOFT) IntUart2Handler(void) {
   char in_ch;
   if (IFS1bits.U2RXIF) // Check if interrupt generated by a RX event.
   {       
      in_ch = U2RXREG;
      //uart2_input_buffer[tft_msg_length] = in_ch;
      //if (in_ch == (char)0xEF)  // TFT end of message id.
      //{
      //   tft_set_status(1);
      //   tft_msg_length = 0;
      //   xzero(uart2_input_buffer, 256);
      //}
      
      //tft_msg_length++;
     
      //U2TXREG = in_ch;           
      
      IFS1bits.U2RXIF = 0;    // clear the RX interrupt flag
   } 
   else if(IFS1bits.U2TXIF) { // if it is a TX interrupt 
   } 
   else if(IFS1bits.U2EIF) {  // if it is an error interrupt. check U3STA for reason
   } 
}

void __ISR(34, IPL3SOFT) ChangeNotificationHandler(void) {
   
   if (PORTBbits.RB13 == 0)
   {
      rpm = 60000 / millisecond_counter;
      millisecond_counter = 0;
      rpm_detect_count++;
      //distance(metres) = pedal revolutions * pedal to rear wheel gear ration (6.5) * rear wheel diameter(280mm) * PI / 1000;
      distance = rpm_detect_count * 5.717;
      average_speed = (distance / 1000.0) / ((float)session_seconds / 3600.0);  // km/h = distance metres / 1000 / session_seconds / 3600
      //LATBINV = 0x0010; // Blink LED on pin RB4. NOTE: for some reason this blinks RB5
      LATBbits.LATB4 = !LATBbits.LATB4; // DO NOT use != operator, it does not work.
   
      //if (session_state == 1)
      //{
      float kj = calculate_kilojoules();
         sprintf(user_msg_buffer, "\r\nU8:RPM = %3u rpm : DETECT = %u : DISTANCE = %3.2f : SPEED = %3.2f : KJ = %4.2f\r\n> ", rpm, rpm_detect_count, distance, average_speed, kj);
         Serial_Transmit_U1(user_msg_buffer);
         xzero(user_msg_buffer, 256);
      //}
   }

   IFS1bits.CNBIF = 0;    // Clear CN interrupt flag.    
}

// TODO: move to adc.c
int adc_read(char analog_pin)
{
   AD1CHS = analog_pin << 16;    // AD1CHS<16:19> controls which analog pin goes
   AD1CON1bits.SAMP = 1;         // Begin sampling
   while( AD1CON1bits.SAMP );    // wait until acquisition is done
   while( ! AD1CON1bits.DONE );  // wait until conversion done
   return(ADC1BUF0);             // result stored in ADC1BUF0
}

void configure_adc()
{
   AD1CON1CLR = 0x8000;  // disable ADC before configuration
   AD1CON1 = 0x00E0;     // internal counter ends sampling and starts conversion
   AD1CON2 = 0;          // AD1CON2<15:13> set voltage reference to pins AVSS/AD1CON3 = 0x0f01; 
                         // TAD = 4*TPB, acquisition time = 15*TAD
   AD1CON1SET = 0x8000;  // Enable ADC
   return;
}

void configure_interrupts()
{
   // Configure Interrupts, see datasheet: TABLE 7-1: INTERRUPT IRQ, VECTOR AND BIT LOCATION
   __builtin_disable_interrupts();   // step 2: disable interrupts at CPU
   
   // Enable multi vector interrupts
   //INTCONbits.MVEC = 0x1;
   INTEnableSystemMultiVectoredInt(); //NOTE: check the source code for this???
   
   // Configure the UART1 interrupt.
   IEC1bits.U1RXIE = 0;
   U1STAbits.URXISEL = 0x0; // RX interrupt when receive buffer not empty
   IFS1bits.U1RXIF = 0;     // clear the rx interrupt flag. For tx or error interrupts you would also need to clear the respective flags
   IPC8bits.U1IP = 1;       // interrupt priority
   IEC1bits.U1RXIE = 1;     // enable the RX interrupt

   // Configure the UART2 interrupt.
   IEC1bits.U2RXIE = 0;
   U2STAbits.URXISEL = 0x0; // RX interrupt when receive buffer not empty
   IFS1bits.U2RXIF = 0;     // clear the rx interrupt flag. For tx or error interrupts you would also need to clear the respective flags
   IPC9bits.U2IP = 2;       // interrupt priority
   IEC1bits.U2RXIE = 1;     // enable the RX interrupt
 
   // Configure timer2 interrupt.
   IEC0CLR = 0x0200;       // disable Timer2 int, IEC0<9>
   IFS0CLR = 0x0200;       // clear Timer2 int flag, IFS0<9>
   IPC2CLR = 0x001f;       // clear Timer2 priority/subpriority fields IPC2<4:0>
   IPC2SET = 0x0010;       // set Timer2 int priority = 4, IPC2<4:2>
   IPC2SET = 0x0000;       // set Timer2 int subpriority = 0, IPC2<1:0>
   IEC0SET = 0x0200;       // enable Timer2 int, IEC0<9>

   // Configure change notification interrupts.
   CNCONBbits.ON = 1;     // CN is enabled
   CNCONBbits.SIDL = 0;   // CPU Idle does not affect CN operation
   CNENBbits.CNIEB13 = 1; // Enable CN13/RB13 pin for interrupt detection
   //CNENBbits.CNIEB7 = 1;
   IEC1bits.CNBIE = 1;    // Enable CN interrupts
   IPC8bits.CNIP = 3;     // Interrupt priority to 3.
   IFS1bits.CNBIF = 0;    // Clear CN interrupt flag. 
 
   // Configure core timer interrupt.
   IPC0bits.CTIP = 6;                // interrupt priority
   IPC0bits.CTIS = 0;                // subp is 0, which is the default
   IFS0bits.CTIF = 0;                // clear CT interrupt flag
   IEC0bits.CTIE = 1;                // enable core timer interrupt
   _CP0_SET_COUNT(0);                // set core timer counter to 0
   _CP0_SET_COMPARE(CORE_TICKS);     // CP0_COMPARE register set to 20 M
  
   __builtin_enable_interrupts();    // step 7: CPU interrupts enabled
   
   return;
}

void configure_io()
{
   T1CON = 0x8000; // TMR1 on, prescaler 1:1
   PR1 = 0xffff; // Set timer 1 period to its max (2^16-1)
   T2CON = 0x8070; // Turn on 16-bit Timer2, set prescaler to 1:256 (frequency is Pbclk / 256)
   PR2 = set_PR2(0.002); // ADC read every 2 milliseconds (500 samples per second)

   ANSELA = 0; // TODO: set RA0 as ADC input from pulse sensor.
   ANSELAbits.ANSA0 = 1;   // set RA0 (AN0) to analog
   ANSELB = 0; // Set all of PortB as digital.
   
   TRISA = 0;  // Set all pins to outputs by default, (SEE DATASHEET SECTION 2.8 UNUSED I/O PINS). 
   TRISB = 0;  // TRISBbits.TRISB5 = 0; RB5 -> Output TRISBbits.TRISB3 = 1; RB3 -> Input
   
   TRISBbits.TRISB2 = 1;  // Make sure UART1 rx pin is configured as in INPUT!!!
   //TRISBbits.TRISB7 = 1;
   TRISBbits.TRISB11 = 1; // Make sure UART2 rx pin is configured as in INPUT!!!
   TRISBbits.TRISB13 = 1; // RPM sensor(reed switch).
   TRISAbits.TRISA0 = 1;  // ADC input from pulse sensor.
   TRISAbits.TRISA2 = 1;  // Increase resistance push button.
   TRISAbits.TRISA3 = 1;  // Decrease resistance push button.
   TRISAbits.TRISA4 = 1;  // Test/reset push button.
   
   LATA = 0; // Set all pin output values to 0, (SEE DATASHEET SECTION 2.8 UNUSED I/O PINS).
   LATB = 0;
   
   LATBbits.LATB4 = 1;  // Process loop LED on.
   LATBbits.LATB5 = 1;  // Power LED on.
   LATBbits.LATB14 = 0; // Motor control pins to 0 or 1 (Both same state turns motor off).
   LATBbits.LATB15 = 0; // RB15 high and RB14 low = motor on, increase resistance (CW).
                        // RB15 low and RB14 high = motor on, decrease resistance (CCW).
   
   return;
}

void process_user_command()
{
   // Serial_Transmit_U1("\r\nACK!\r\n\r\n> ");
   // TODO: process user commands. 
   // Message format: Ux:<data>\r\n
   // where Ux = { U1, U2, U3, U4, U5, U6, U7, U8, U9, UA, UB, UC ... }
   // is the command identifier, followed by a colon and optional data. 
   // All messages are ASCII encoded character strings terminated
   // with "\r\n" (0x0D, 0x0A).
   //
   // Example commands:
   // 1. START ("U1:\r\n")
   // 2. STOP ("U2:\r\n")
   // 3. PAUSE ("U3:\r\n")
   // 4. SET RESISTANCE ("U4: resistance \r\n") where resistance levels = { 1 ... 10 }
   // 5. SET TARGET ("U5: [ TIME | DIST | KJ ] target \r\n") where target is in minutes, kilometers or kilojoules = { 1 ... 10000 }
   // 6. RESET ("U6:\r\n")
   // 7. SEND TIME ("U7:\r\n")
   // 8. SEND RPM
   // 9. SEND BPM
   // 10. SEND PROXIMITY SENSOR RANGE
   // 11. SEND SPEED
   // 12. SEND DISTANCE
   // 13. SEND RESISTANCE LEVEL
   // 14. SET USER AGE ("UE:xxx\r\n"), xxx = years.
   // 15. SET USER WEIGHT ("UF:xxx\r\n"), xxx = kilograms
   // 16. SET USER GENDER ("UG:[ M | F ] \r\n" )
   // 17. INCREASE RESISTANCE ("UH:\r\n")
   // 18. DECREASE RESISTANCE ("UI:\r\n")
   // 19. CALCULATE KILOJOULES ("UJ:\r\n")
   // 20. SEND USER AGE ("UK:\r\n")
   // 21. SEND USER WEIGHT ("UL:\r\n")
   // 22. SEND USER GENDER ("UM:\r\n")
   // A user command always receives a response in the form of:
   // Ux:<data>\r\n    for data requests.
   // Ux:ACK\r\n       for commands with no data response.
   //
   
   switch(uart1_input_buffer[1])
   {
      case '0': break;
      case '1': break;
      case '2': break;
      case '3': break;
      case '4': break;
      case '5': break;
      case '6': break;
      case '7': break;
      case '8': get_rpm = 1; break;
      case '9': get_bpm = 1; break;
      case 'A': get_prox_range = 1; break;
      case 'B': break;
      case 'C': break;
      case 'D': break;
      case 'E': set_user_age(uart1_input_buffer); break;
      case 'F': set_user_weight(uart1_input_buffer); break;
      case 'G': set_user_gender(uart1_input_buffer); break;
      case 'H': increase_resistance = 1; Serial_Transmit_U1("\r\nACK!\r\n\r\n> "); break;
      case 'I': decrease_resistance = 1; Serial_Transmit_U1("\r\nACK!\r\n\r\n> "); break;
      case 'J': send_kilojoules(); break;
      case 'K': break;
      case 'L': break;
      case 'M': break;
      default: Serial_Transmit_U1("\r\nNACK!\r\n\r\n> ");
   }
   
   return;
}

void initialise_vars()
{
   return;
}

float calculate_kilojoules()
{
   float session_minutes;
   
   // MaleEE = (-55.0969 + 0.6309 x heart rate + 0.1988 x weight + 0.2017 x age) * session_minutes
   // FemaleEE (-20.4022 + 0.4472 x heart rate - 0.1263 x weight + 0.074 x age) * session_minutes
   
   // TEST:
   user_gender = 0;
   user_weight = 80;
   user_age = 50;
   bpm = 80;
   
   session_minutes = (float)session_seconds / 60.0;
   
   if (user_gender == 0) // MALE
   {
      kilojoules = ((-55.0969 + (0.6309 * bpm) + (0.1988 * user_weight) + (0.2017 * user_age)) * session_minutes);
   }
   else
   {
   
      kilojoules = ((-20.4022 + (0.4472 * bpm) - (0.1263 * user_weight) + (0.074 * user_age)) * session_minutes);
   }
   
   return(kilojoules);
}

void set_user_age(char *msg)
{
   int buf_idx, msg_idx, msg_len;
   char num_str[16];
   
   xzero(num_str, 16);
   msg_idx = 0;
   msg_len = strlen(msg);
   
   for (buf_idx = 3; buf_idx < msg_len; buf_idx++)
   {
      if (msg[buf_idx] < 32)   /* Look for 0x0A or 0x0D message delimiters. */
      {
         break;
      }
      else
      {
         num_str[msg_idx] = msg[buf_idx]; /* Add character to the reply message buffer. */
         msg_idx++;
      }
   }
   
   user_age = string_to_int(num_str);
   
   return;
}

void set_user_gender(char *msg)
{
   if (msg[3] == '0')
   {
      user_gender = 0;
   }
   else if (msg[3] == '1')
   {
      user_gender = 1;
   }
   else
   {
      user_gender = 2;
   }
   
   return;
}

void set_user_weight(char *msg)
{
   int buf_idx, msg_idx, msg_len;
   char num_str[16];
   
   xzero(num_str, 16);
   msg_idx = 0;
   msg_len = strlen(msg);
   
   for (buf_idx = 3; buf_idx < msg_len; buf_idx++)
   {
      if (msg[buf_idx] < 32)   /* Look for 0x0A or 0x0D message delimiters. */
      {
         break;
      }
      else
      {
         num_str[msg_idx] = msg[buf_idx]; /* Add character to the reply message buffer. */
         msg_idx++;
      }
   }
   
   user_weight = string_to_int(num_str);
   
   return;
}

void send_kilojoules()
{
   float kj = calculate_kilojoules();
   sprintf(user_msg_buffer, "UJ:%4.2f\r\n> ", kj);
   Serial_Transmit_U1(user_msg_buffer);
   xzero(user_msg_buffer, 256); 
   
   return;
}


/* *****************************************************************************
 End of File
 */

