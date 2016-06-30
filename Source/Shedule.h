//---------------------------------------------------------------------------
//
// $Title:          Shedule.h
// $Description:    Day and Week Shedule Functions Interface.
//
// $Rev:            0.0.2
//
// $Author:         EEA
//
// $Created:        17.10.11
// $Last Modified:  19.10.11
//
// $Copyright:      EEA
//
// $Notes:
//
//---------------------------------------------------------------------------
#ifndef __SHEDULE_H
#define __SHEDULE_H

#include "EEA_port.h"

void FindDayPlan(void);
void ProcessShedule(void);

bool IsNewDay(void);
bool IsNewMinute(void);


#endif
