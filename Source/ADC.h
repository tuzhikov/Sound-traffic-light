//---------------------------------------------------------------------------
//
// $Title:          ADC.h
// $Description:    ADC Inteface
//                  
// $Rev:            0.0.1
//
// $Author:         EEA
// $Created:        19.10.11
// $Last Modified:  19.10.11
//
// $Copyright:      EEA
//
// $Notes:          
//
//---------------------------------------------------------------------------
#ifndef __ADC_H
#define __ADC_H

#include "EEA_port.h"

void ADC_Init(void);
void ADC_StartMeasure(void);
uint16 ADC_GetResult(uint8 ch);

#endif