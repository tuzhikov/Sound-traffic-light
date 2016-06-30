//---------------------------------------------------------------------------
//
// $Title:          Inputs.c
// $Description:    Inputs Control Function.
//
// $Rev:            0.0.2
//
// $Author:         EEA
//
// $Created:        17.10.11
// $Last Modified:  27.10.11
//
// $Copyright:      EEA
//
// $Notes:
//
//---------------------------------------------------------------------------
#include <NXP/iolpc2103.h>
#include "Inputs.h"
#include "Pins.h"
#include "MiniMP3.h"

static bool buttonOK = false;

uint32 GetInputsState(void)
{
    return FIOPIN;
}

bool IsTestModeIn(void)
{
    if (FIOPIN & TEST_IN_MASK)
        return !TEST_IN_INVERSION;
    else
        return TEST_IN_INVERSION;
}

bool IsRedIn(void)
{
    if (FIOPIN & RED_IN_MASK)
        return true;
    else
        return false;
}

bool IsGreenIn(void)
{
    if (FIOPIN & GREEN_IN_MASK)
        return true;
    else
        return false;
}

bool isButtonIn(void)
{
    if (FIOPIN & BUTTON_PORT)
        return true;
    else
        return false;
}
/**
* call function fastloop 1 ms
*/
void checkButton(void)
{
    static TypeStep step = ONE;
    static uint32 timeDelay = 0;
    const uint32 TIME_DELAY = 100;

    if(step == ONE)
    {
      if(!buttonOK){
        if(isButtonIn()==true){
            if(timeDelay++>=TIME_DELAY){
                buttonOK = true;
                step = TWO;
            }
        }
      }
    }
    else if(step == TWO)
    {
        timeDelay = 0;
        if(isButtonIn()==false){
            step = ONE;
        }
    }
}
/**
*
*/
bool retStatusButton(void)
{
  bool retButton = buttonOK;
  return retButton;
}