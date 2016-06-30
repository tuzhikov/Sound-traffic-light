//---------------------------------------------------------------------------
//
// $Title:          System.c
// $Description:    General System Functions.
//
// $Rev:            0.0.5
//
// $Author:         EEA
//
// $Created:        9.10.11
// $Last Modified:  5.12.13
//
// $Copyright:      EEA
//
// $Notes:
//
//---------------------------------------------------------------------------

#include "System.h"

#include "Pins.h"
#include "Config.h"

#include "Timers.h"
#include "UART.h"
#include "I2C.h"
#include "ADC.h"
#include "VIC.h"
#include "RTC.h"
#include "DPS.h"
#include "MiniMP3.h"

//------------------------------
// Function Prototypes
//------------------------------
void SysInit(void);
void Pins_Init(void);
//------------------------------

void System_Init(void)
{
    SysInit();

    VicInit();

    Pins_Init();

    Timers_Init();

    UART_Init();

    I2C_Init();

    ADC_Init();

    RTC_Init();

    initDPS();
}

void SysInit(void)
{
//    // Disable PLL
//    PLLCON = 0;
//    // Write Feed
//    PLLFEED = 0xAA;
//    PLLFEED = 0x55;
//    // Set periphery divider = cclk
//    APBDIV_bit.APBDIV  = 1;

    //Disabel Memory Accelerator Module
    MAMCR_bit.MODECTRL = 0;
    //Set fetch cycles
#if FCCLK < 20000000
    MAMTIM_bit.CYCLES = 1;
#elif FCCLK < 40000000
    MAMTIM_bit.CYCLES = 2;
#elif FCCLK < 60000000
    MAMTIM_bit.CYCLES = 3;
#else
    MAMTIM_bit.CYCLES = 4;
#endif
    //Here MAM can be enabled
    MAMCR_bit.MODECTRL = 2;

    //Disable PLL
    PLLCON = 0;
    // Write Feed
    PLLFEED = 0xAA;
    PLLFEED = 0x55;
    //Set MSEL and PSEL
    PLLCFG = PLL_M_VAL | (PLL_P_VAL<<5);
    // Write Feed
    PLLFEED = 0xAA;
    PLLFEED = 0x55;
    // Enable PLL, disconnected
    PLLCON = 1;
    // Write Feed
    PLLFEED = 0xAA;
    PLLFEED = 0x55;
    //Set periferial clock
    APBDIV = APBDIV_VAL;

    //Wait PLL Lock
    while(!PLLSTAT_bit.PLOCK);

    // connect PLL
    PLLCON = 3;
    // Write Feed
    PLLFEED = 0xAA;
    PLLFEED = 0x55;
}

void Pins_Init(void)
{
    PINSEL0 =  (PIN_00_SEL    <<  0) +
               (PIN_01_SEL    <<  2) +
               (PIN_02_SEL    <<  4) +
               (PIN_03_SEL    <<  6) +
               (PIN_04_SEL    <<  8) +
               (PIN_05_SEL    << 10) +
               (PIN_06_SEL    << 12) +
               (PIN_07_SEL    << 14) +
               (PIN_08_SEL    << 16) +
               (PIN_09_SEL    << 18) +
               (PIN_10_SEL    << 20) +
               (PIN_11_SEL    << 22) +
               (PIN_12_SEL    << 24) +
               (PIN_13_SEL    << 26) +
               (PIN_14_SEL    << 28) +
               (PIN_15_SEL    << 30);

    PINSEL1 =  (PIN_16_SEL    <<  0) +
               (PIN_17_SEL    <<  2) +
               (PIN_18_SEL    <<  4) +
               (PIN_19_SEL    <<  6) +
               (PIN_20_SEL    <<  8) +
               (PIN_21_SEL    << 10) +
               (PIN_22_SEL    << 12) +
               (PIN_23_SEL    << 14) +
               (PIN_24_SEL    << 16) +
               (PIN_25_SEL    << 18) +
               (PIN_26_SEL    << 20) +
               (PIN_27_SEL    << 22) +
               (PIN_28_SEL    << 24) +
               (PIN_29_SEL    << 26) +
               (PIN_30_SEL    << 28) +
               (PIN_31_SEL    << 30);

    //--------------------------------
    // Enable fast IO
    SCS_bit.GPIO0M = 1;

    //--------------------------------
    FIODIR =   (PIN_00_DIR    <<  0) +
               (PIN_01_DIR    <<  1) +
               (PIN_02_DIR    <<  2) +
               (PIN_03_DIR    <<  3) +
               (PIN_04_DIR    <<  4) +
               (PIN_05_DIR    <<  5) +
               (PIN_06_DIR    <<  6) +
               (PIN_07_DIR    <<  7) +
               (PIN_08_DIR    <<  8) +
               (PIN_09_DIR    <<  9) +
               (PIN_10_DIR    << 10) +
               (PIN_11_DIR    << 11) +
               (PIN_12_DIR    << 12) +
               (PIN_13_DIR    << 13) +
               (PIN_14_DIR    << 14) +
               (PIN_15_DIR    << 15) +
               (PIN_16_DIR    << 16) +
               (PIN_17_DIR    << 17) +
               (PIN_18_DIR    << 18) +
               (PIN_19_DIR    << 19) +
               (PIN_20_DIR    << 20) +
               (PIN_21_DIR    << 21) +
               (PIN_22_DIR    << 22) +
               (PIN_23_DIR    << 23) +
               (PIN_24_DIR    << 24) +
               (PIN_25_DIR    << 25) +
               (PIN_26_DIR    << 26) +
               (PIN_27_DIR    << 27) +
               (PIN_28_DIR    << 28) +
               (PIN_29_DIR    << 29) +
               (PIN_30_DIR    << 30) +
               (PIN_31_DIR    << 31);

    FIOMASK =  (PIN_00_MASK   <<  0) +
               (PIN_01_MASK   <<  1) +
               (PIN_02_MASK   <<  2) +
               (PIN_03_MASK   <<  3) +
               (PIN_04_MASK   <<  4) +
               (PIN_05_MASK   <<  5) +
               (PIN_06_MASK   <<  6) +
               (PIN_07_MASK   <<  7) +
               (PIN_08_MASK   <<  8) +
               (PIN_09_MASK   <<  9) +
               (PIN_10_MASK   << 10) +
               (PIN_11_MASK   << 11) +
               (PIN_12_MASK   << 12) +
               (PIN_13_MASK   << 13) +
               (PIN_14_MASK   << 14) +
               (PIN_15_MASK   << 15) +
               (PIN_16_MASK   << 16) +
               (PIN_17_MASK   << 17) +
               (PIN_18_MASK   << 18) +
               (PIN_19_MASK   << 19) +
               (PIN_20_MASK   << 20) +
               (PIN_21_MASK   << 21) +
               (PIN_22_MASK   << 22) +
               (PIN_23_MASK   << 23) +
               (PIN_24_MASK   << 24) +
               (PIN_25_MASK   << 25) +
               (PIN_26_MASK   << 26) +
               (PIN_27_MASK   << 27) +
               (PIN_28_MASK   << 28) +
               (PIN_29_MASK   << 29) +
               (PIN_30_MASK   << 30) +
               (PIN_31_MASK   << 31);

    FIOPIN =   (PIN_00_PIN    <<  0) +
               (PIN_01_PIN    <<  1) +
               (PIN_02_PIN    <<  2) +
               (PIN_03_PIN    <<  3) +
               (PIN_04_PIN    <<  4) +
               (PIN_05_PIN    <<  5) +
               (PIN_06_PIN    <<  6) +
               (PIN_07_PIN    <<  7) +
               (PIN_08_PIN    <<  8) +
               (PIN_09_PIN    <<  9) +
               (PIN_10_PIN    << 10) +
               (PIN_11_PIN    << 11) +
               (PIN_12_PIN    << 12) +
               (PIN_13_PIN    << 13) +
               (PIN_14_PIN    << 14) +
               (PIN_15_PIN    << 15) +
               (PIN_16_PIN    << 16) +
               (PIN_17_PIN    << 17) +
               (PIN_18_PIN    << 18) +
               (PIN_19_PIN    << 19) +
               (PIN_20_PIN    << 20) +
               (PIN_21_PIN    << 21) +
               (PIN_22_PIN    << 22) +
               (PIN_23_PIN    << 23) +
               (PIN_24_PIN    << 24) +
               (PIN_25_PIN    << 25) +
               (PIN_26_PIN    << 26) +
               (PIN_27_PIN    << 27) +
               (PIN_28_PIN    << 28) +
               (PIN_29_PIN    << 29) +
               (PIN_30_PIN    << 30) +
               (PIN_31_PIN    << 31);

}

