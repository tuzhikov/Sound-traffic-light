/*----------------------------------------------------------------------------*/
//
// $Title:          Delay.c
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
#include "Delay.h"
#include "TypeData.h"

volatile  uint8 System10ms;
volatile  uint8 System100ms;
volatile  uint8 System1sec;

/*----------------------------------------------------------------------------*/
void InstallDelay(void)
{
    System10ms = 0;
    System100ms= 0;
    System1sec = 0;
}
/*----------------------------------------------------------------------------*/
uint8 GetTimerTik(uint8 timers)
{
    switch(timers)
    {
        case tTimer10ms:
            return(System10ms);
	case tTimer100ms:
            return(System100ms);
	case tTimer1sec:
            return(System1sec);
        default:
            return 0;
    }
}
/*----------------------------------------------------------------------------*/
bool DelayOut(uint8 nStartTime,uint8 nCurrentTime,uint8 Delay)
{
    uint8 MaxLimit = sizeof(MaxLimit)*0xFF;

    if((nStartTime+Delay) > MaxLimit){
        if((nCurrentTime >= (nStartTime+Delay-MaxLimit-1))&&(nCurrentTime < nStartTime))
            return true;
        else
            return false;
    }else{
        if((nCurrentTime >= (nStartTime+Delay))||(nCurrentTime < nStartTime))
            return true;
        else
            return false;
    }
}
/*----------------------------------------------------------------------------*/
// function colling module fastloop
void TimerCooling(void)
{
    static volatile uint8 ms1Timer  = 0;
    static volatile uint8 ms10Timer = 0;
    static volatile uint8 ms100Timer= 0;

    if(++ms1Timer<10)
        return;
    else
        ms1Timer    = 0;
    // 10ms
    System10ms++;
    if(++ms10Timer<10)
        return;
    else
        ms10Timer = 0;
    // 100ms
    System100ms++;
    if(++ms100Timer<10)
        return;
    else
        ms100Timer = 0;
    // Sec
    System1sec++;
}
/*----------------------------------------------------------------------------*/
/**
* time delay step 10 ms. Total delay time  = (10*val)ms
* value 1: 0...255
* value 2: 0...MAX_TIMER
* return: true - time is up, false - delay
*/
bool ms10TimeDelay(const uint8 valTime,const uint8 curTimer)
{
    static TypeStep step[MAX_TIMER] = {ONE};
    static uint8 timeDelay[MAX_TIMER];

    // check max timer
    if(curTimer>=MAX_TIMER){
        return false;
    }

    switch(step[curTimer])
    {
        case ONE:
            timeDelay[curTimer] = GetTimerTik(tTimer10ms);
            step[curTimer] = TWO;
        break;
        case TWO:
            if(DelayOut(timeDelay[curTimer],GetTimerTik(tTimer10ms),valTime)){
                step[curTimer] = ONE;
                return true;
            }
        break;
        default:
            step[curTimer] = ONE;
        return false;
    }
    return false;
}