//---------------------------------------------------------------------------
//
// $Title:          Timers.h
// $Description:    Timers Constants and Function Prototypes
//                  
// $Rev:            0.0.1
//
// $Author:         EEA
// $Created:        9.10.11
// $Last Modified:  9.10.11
//
// $Copyright:      EEA
//
// $Notes:          
//
//---------------------------------------------------------------------------

#ifndef __TIMERS_H
#define __TIMERS_H

#include "EEA_Port.h"

void Timers_Init(void);

void Timer2_Start(void);
void Timer2_Stop(void);
void Timer2_Reset(void);
void Timer2_SetValue(uint16 Value);

#endif