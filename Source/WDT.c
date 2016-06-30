//---------------------------------------------------------------------------
//
// $Title:          WDT.c
// $Description:    WatchDog Functions Realisation
//                  
// $Rev:            0.0.3
//
// $Author:         EEA
// $Created:        4.11.11
// $Last Modified:  11.11.11
//
// $Copyright:      EEA
//
// $Notes:          
//
//---------------------------------------------------------------------------
#include <NXP/iolpc2103.h>
#include "WDT.h"
#include "Config.h"
#include "System.h"

bool Is_WDT_Reset(void)
{
//    return (WDMOD & WDTOF);
    return false;
}

void WDT_Init(void)
{
    #ifdef WDT_ENABLE
    WDTC  = WDT_TIMEOUT * (PCCLK / (4*1000));
    WDMOD = 0x03;
    WDT_Feed();
    #endif
}

void WDT_Feed(void)
{
    #ifdef WDT_ENABLE
    WDFEED = 0xAA;
    WDFEED = 0x55;
    #endif
}


