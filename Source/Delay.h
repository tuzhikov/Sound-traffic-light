/*----------------------------------------------------------------------------*/
//
// $Title:          Delay.h
// $Description:    DIGITAL POTENTIOMETER AD5242BR.
//
// $Rev:            0.0.1
//
// $Author:         ATA
// $Created:        24.06.16
// $Last Modified:
//
// $Copyright:
//
// $Notes:
//
/*----------------------------------------------------------------------------*/
#ifndef DELAY_H
#define DELAY_H

#include "EEA_port.h"

#define MAX_TIMER     5

enum TTimers {tTimer10ms,tTimer100ms,tTimer1sec};

void TimerCooling(void);
void InstallDelay(void);
uint8 GetTimerTik(uint8 timers);
bool DelayOut(uint8 nStartTime, uint8 nCurrentTime, uint8 Delay);
bool ms10TimeDelay(const uint8 valTime,const uint8 curTimer);

#endif