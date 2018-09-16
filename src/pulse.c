/*
   Filename: pulse.c
   
   Description: Functions to read a pulse sensor, adapted from the
                Arduino code provided by Sparkfun. 


   Author: Derek Chadwick
   
   Date: 27/08/2018

*/



#include "pulse.h"

static volatile int rate[10];                    // array to hold last ten IBI values
static volatile unsigned long sampleCounter = 0;          // used to determine pulse timing
static volatile unsigned long lastBeatTime = 0;           // used to find IBI
static volatile int P =2048;                      // used to find peak in pulse wave, seeded
static volatile int T = 2048;                     // used to find trough in pulse wave, seeded
static volatile int thresh = 2048;                // used to find instant moment of heart beat, seeded
static volatile int amp = 410;                    // used to hold amplitude of pulse waveform, seeded
static volatile int firstBeat = 1;        // used to seed rate array so we startup with reasonable BPM
static volatile int secondBeat = 0;       // used to seed rate array so we startup with reasonable BPM

// these variables are volatile because they are used during the interrupt service routine!
static volatile int BPM;                   // used to hold the pulse rate
static volatile int Signal;                // holds the incoming raw data
static volatile int IBI = 600;             // holds the time between beats, must be seeded! 
static volatile int Pulse = 0;     // 1 when pulse wave is high, 0 when it's low
static volatile int QS = 0;        // becomes 1 when Arduino finds a beat.


// THIS IS THE TIMER (2 on Core, 3 on Photon) INTERRUPT SERVICE ROUTINE. 
// The Timer makes sure that we take a reading every 2 miliseconds
//ISR(TIMER2_COMPA_vect){                         // triggered when Timer2 counts to 124
int pulse_ISR(unsigned int sample) 
{
   int i;
   
   Signal = sample;                            // read the Pulse Sensor 
   sampleCounter += 2;                         // keep track of the time in mS with this variable
   int N = sampleCounter - lastBeatTime;       // monitor the time since the last beat to avoid noise

   //  find the peak and trough of the pulse wave
   if(Signal < thresh && N > (IBI/5)*3)
   {                                       // avoid dichrotic noise by waiting 3/5 of last IBI
      if (Signal < T){                     // T is the trough
         T = Signal;                        // keep track of lowest point in pulse wave 
      }
   }

   if(Signal > thresh && Signal > P)
   {          // thresh condition helps avoid noise
     P = Signal;                            // P is the peak
   }                                        // keep track of highest point in pulse wave

   //  NOW IT'S TIME TO LOOK FOR THE HEART BEAT
   // signal surges up in value every time there is a pulse
   if (N > 250)
   {                                   // avoid high frequency noise
     if ( (Signal > thresh) && (Pulse == 0) && (N > (IBI/5)*3) )
     {        
       Pulse = 1;                               // set the Pulse flag when we think there is a pulse
       //digitalWrite(blinkPin,HIGH);                // turn on blink LED
       IBI = sampleCounter - lastBeatTime;      // measure time between beats in mS
       lastBeatTime = sampleCounter;            // keep track of time for next pulse

       if(secondBeat)
       {                                  // if this is the second beat, if secondBeat == TRUE
         secondBeat = 0;                  // clear secondBeat flag
         for(i=0; i<=9; i++)
         {                                // seed the running total to get a realisitic BPM at startup
           rate[i] = IBI;                      
         }
       }

       if(firstBeat)
       {                                  // if it's the first time we found a beat, if firstBeat == TRUE
         firstBeat = 0;                   // clear firstBeat flag
         secondBeat = 1;                  // set the second beat flag
         return(0);                          // IBI value is unreliable so discard it
       }   


       // keep a running total of the last 10 IBI values
       int runningTotal = 0;                   // clear the runningTotal variable    

       for(i=0; i<=8; i++)
       {                                       // shift data in the rate array
         rate[i] = rate[i+1];                  // and drop the oldest IBI value 
         runningTotal += rate[i];              // add up the 9 oldest IBI values
       }

       rate[9] = IBI;                          // add the latest IBI to the rate array
       runningTotal += rate[9];                // add the latest IBI to runningTotal
       runningTotal /= 10;                     // average the last 10 IBI values 
       BPM = 60000/runningTotal;               // how many beats can fit into a minute? that's BPM!
       QS = 1;                                 // set Quantified Self flag 
                                               // QS FLAG IS NOT CLEARED INSIDE THIS ISR
     }                       
   }

   if (Signal < thresh && Pulse == 1)
   {                                        // when the values are going down, the beat is over
     //digitalWrite(blinkPin,LOW);          // turn off pin blink LED
     Pulse = 0;                             // reset the Pulse flag so we can do it again
     amp = P - T;                           // get amplitude of the pulse wave
     thresh = amp/2 + T;                    // set thresh at 50% of the amplitude
     P = thresh;                            // reset these for next time
     T = thresh;
   }

   if (N > 2500)
   {                                        // if 2.5 seconds go by without a beat
     thresh = 512;                          // set thresh default
     P = 512;                               // set P default
     T = 512;                               // set T default
     lastBeatTime = sampleCounter;          // bring the lastBeatTime up to date        
     firstBeat = 1;                         // set these to avoid noise
     secondBeat = 0;                        // when we get the heartbeat back
   }

   return(BPM);
   
}// end isr





