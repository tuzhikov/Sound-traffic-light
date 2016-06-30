//---------------------------------------------------------------------------
//
// $Title:          ADC.c
// $Description:    ADC Inteface
//                  
// $Rev:            0.0.2
//
// $Author:         EEA
// $Created:        19.10.11
// $Last Modified:  5.11.13
//
// $Copyright:      EEA
//
// $Notes:          
//
//---------------------------------------------------------------------------
#include <NXP/iolpc2103.h>

#include "ADC.h"

#include "Pins.h"
#include "Config.h"

void ADC_Init(void)
{
    AD0CR = (1<<ADC_POWER_FB_CHANNEL)                   |
            (1<<ADC_LIGHT_SENS_CHANNEL)                 |
            ( (PCCLK/ADC_FREQ-1) << ADCR_CLKDIV_DISP )  |
            ADCR_BURST_MASK                             |
            (ADCR_CLKS_10BITS << ADCR_CLKS_DISP)        |
            ADCR_PDN_MASK;
}

void ADC_StartMeasure(void)
{

}                                                                                                                            

uint16 ADC_GetResult(uint8 ch)
{
    uint32 *ptr32 = (uint32*)&ADDR0;
    uint16 res;
    
    res = (ptr32[ch]>>6) & 0x03FF;
    return res;
}



