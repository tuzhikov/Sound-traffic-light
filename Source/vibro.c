/*----------------------------------------------------------------------------*/
//
// $Title:          vibro.c
// $Description:    Mini mp3 Interface.
//
// $Rev:            0.0.1
//
// $Author:         ATA
// $Created:        23.06.16
// $Last Modified:
//
// $Copyright:
//
// $Notes:
//
/*----------------------------------------------------------------------------*/
#include "vibro.h"
#include "Pins.h"
#include "Inputs.h"
#include "System.h"
#include "Delay.h"
#include "TypeData.h"
//
typedef struct{
    uint8 timeOn;
    uint8 timeOff;
    uint8 timeDelay;
    uint8 timeCount;
    uint8 globalCount;
    bool enabled;
}TypeVibroDate;
//
static TypeVibroDate vibroData = {0};
static TypeStep step = ONE;
/*----------------------------------------------------------------------------*/
/**
*/
static void enableModuleVibro(const bool portOn)
{
    if(portOn){
        FIOSET |= BUZER_MASK;
    }else{
        FIOCLR |= BUZER_MASK;
    }
}
/**
*
*/
void setParametrVibro(const bool enabled,
                              const uint8 timeOn,
                              const uint8 timeOff,
                              const uint8 timeDelay,
                              const uint8 timeCount,
                              const uint8 globalCount)
{
    vibroData.timeOn = timeOn;
    vibroData.timeOff = timeOff;
    vibroData.timeDelay = timeDelay;
    vibroData.timeCount = timeCount;
    vibroData.globalCount = globalCount;
    vibroData.enabled = enabled;
    step = ONE;
}
/**
*/
void clearParametrVibro(void)
{
    vibroData.timeOn = 0;
    vibroData.timeOff = 0;
    vibroData.timeDelay = 0;
    vibroData.timeCount = 0;
    vibroData.globalCount = 0;
    vibroData.enabled = false;
    step = ONE;
}
/**
*
*/
void machineVibro(void)
{
    const uint8 numberTimer = 1;
    //static TypeStep step = ONE;
    static uint8 countCycle = 0;
    static uint8 countGlobalCycle = 0;

    if(vibroData.enabled){
        if(step == ONE)
        {
            enableModuleVibro(true);
            if(ms10TimeDelay(vibroData.timeOn,numberTimer)){
                if(++countCycle >= vibroData.timeCount){
                    step = THREE;
                }else{
                    step = TWO;
                }
            }
        }
        else if(step == TWO)
        {
            enableModuleVibro(false);
            if(ms10TimeDelay(vibroData.timeOff,numberTimer)){
                step = ONE;
            }
        }
        else if(step == THREE)
        {
            enableModuleVibro(false);
            countCycle = 0;
            if(ms10TimeDelay(vibroData.timeDelay,numberTimer)){
                if(vibroData.globalCount!=0){
                    if(++countGlobalCycle>=vibroData.globalCount){
                        countGlobalCycle = 0;
                        vibroData.enabled = false;
                    }
                }else{
                    step = ONE;
                }
            }
        }
    }else{
        step = ONE;
        countCycle=0;
        enableModuleVibro(false);
    }
}