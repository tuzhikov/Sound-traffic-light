//---------------------------------------------------------------------------
//
// $Title:          WDT.h
// $Description:    WatchDog Interface
//
// $Rev:            0.0.2
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
#ifndef __WDT_H
#define __WDT_H

#include "EEA_Port.h"

//#define WDT_ENABLE

bool Is_WDT_Reset(void);
void WDT_Init(void);
void WDT_Feed(void);

#endif
