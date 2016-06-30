//---------------------------------------------------------------------------
//
// $Title:          DS1338.h
// $Description:    DS1338 Interface.
//                  
// $Rev:            0.0.1
//
// $Author:         EEA
// $Created:        17.10.11
// $Last Modified:  17.10.11
//
// $Copyright:      EEA
//
// $Notes:          
//
//---------------------------------------------------------------------------
#ifndef __DS1338_H
#define __DS1338_H

#include "EEA_port.h"
#include "DateTime.h"

void DS1338_Init(void);
bool DS1338_GetTime(TDateTime *pDateTime);
bool DS1338_SetTime(TDateTime *pDateTime);

#endif