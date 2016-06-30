//---------------------------------------------------------------------------
//
// $Title:          RTC.h
// $Description:    RTC General Interface.
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
#ifndef __RTC_H
#define __RTC_H

#include "DS1338.h"

#define RTC_Init()      DS1338_Init()
#define RTC_GetTime(N)  DS1338_GetTime(N)
#define RTC_SetTime(N)  DS1338_SetTime(N)

#endif