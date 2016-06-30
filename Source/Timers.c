//---------------------------------------------------------------------------
//
// $Title:          Timers.c
// $Description:    Timers Functions Definition
//
// $Rev:            0.0.2
//
// $Author:         EEA
// $Created:        9.10.11
// $Last Modified:  20.10.11
//
// $Copyright:      EEA
//
// $Notes:
//
//---------------------------------------------------------------------------
#include <NXP/iolpc2103.h>
#include "Timers.h"

#include "Pins.h"
#include "Config.h"
#include "System.h"
#include "VIC.h"
#include "FastLoop.h"


#define SOUND_PWM_FREQ      (SOUND_SAMPLES_FREQ*SOUND_PWM_MAXVOLUME)

// Prototypes
void Timer0_Init(void);
void Timer2_Init(void);
void Timer0Handler(void);


void Timers_Init(void)
{
    Timer0_Init();
    Timer2_Init();
}

void Timer0_Init(void)
{
    // Init timer
    // Reset and stop timer0
    T0TCR = 2;
    // Set timer counters mode - clock by PCLK
    T0CTCR = 0;
    // Set timer prescaler
    T0PR  = 0;
    // Set timer period
    T0MR0 = PCCLK/FAST_LOOP_FREQ;
    // Set match action - interrupt by MACH0 enable, reset counter
    T0MCR = 3;
    // No external action
    T0EMR = 0;

    // Assign to IRQ
    VICIntSelect_bit.TIMER0 = 0;
    // Set interrupt slots
    VICVectAddr3 = (unsigned int) Timer0Handler;
    VICVectCntl3_bit.NUMBER = VIC_TIMER0;
    VICVectCntl3_bit.ENABLED = 1;
    // Timer 0 interrupt enable
    VICIntEnable_bit.TIMER0 = 1;
    // Enable timer0
    T0TCR = 1;
}


void Timer2_Init(void)
{
    // Init timer
    // Reset and stop timer
    T2TCR = 2;

    T2IR = 0x08;
    // Set timer counters mode - clock by PCLK
    T2CTCR = 0;

    T2TC = 0;
    T2PC = 0;

    // Set timer prescaler
    T2PR  = PCCLK/SOUND_PWM_FREQ-1;
//    T2PR  = 0;
    // Set PWM period
    T2MR3 = SOUND_PWM_MAXVOLUME-1;
    T2MR0 = 0;
    T2MR1 = 0;
    T2MR2 = 0;
    // Set match action
    //T2MCR = 0x0600;   // Int and Reset
    T2MCR = 0x0E00;     // Int, Stop and Reset
    // External action
    T2EMR = 0;

    T2PWMCON = 1<<SOUND_PWM_CHANNEL;   // Chanel for Sound PWM

    // Assign To FIQ
    VICIntSelect_bit.TIMER2 = 1;
    // Set interrupt slots
    //VICVectAddr2 = (unsigned int) Timer0Handler;
    //VICVectCntl2_bit.NUMBER = VIC_TIMER2;
    //VICVectCntl2_bit.ENABLED = 1;
    // Timer 2 interrupt enable
    VICIntEnable_bit.TIMER2 = 1;
  
    // Enable timer
    //T2TCR = 1;
}

void Timer2_Start(void)
{
    T2TCR = 1;
}

void Timer2_Stop(void)
{
    T2TCR = 0;
}

void Timer2_Reset(void)
{
    T2TCR = 2;
}

void Timer2_SetValue(uint16 Value)
{
//    unsigned int tmp32;
//    tmp32 = 2*(unsigned int)Value*(unsigned int)T2MR3 / SOUND_PWM_MAXVOLUME;
//    T2MR0 = tmp32;

//    T2MR0 = SOUND_PWM_MAXVOLUME/2;

    #if   (SOUND_PWM_CHANNEL == 0)
        T2MR0 = Value;
    #elif (SOUND_PWM_CHANNEL == 1)
        T2MR1 = Value;
    #elif (SOUND_PWM_CHANNEL == 2)
        T2MR2 = Value;
    #else
        #error Wrong SOUND_PWM_CHANNEL Value
    #endif
}

void Timer0Handler(void)
{
    T0IR_bit.MR0INT = 1;    // Clear Interrupt Flag

    FastLoop();             // Do Work

    VICVectAddr = 0;        // Clear Interrupt in VIC
}

