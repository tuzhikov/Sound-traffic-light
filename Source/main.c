//---------------------------------------------------------------------------
//
// $Title:          main.c
// $Description:    APP Level Functionality.
//
// $Rev:            0.0.16
//
// $Author:         EEA
//
// $Created:        9.10.11
// $Last Modified:  15.8.14
//
// $Copyright:      EEA
//
// $Notes:
//
//---------------------------------------------------------------------------
//#include "EEA_port.h"
#include <intrinsics.h>

#include "System.h"
#include "Sound.h"
#include "Inputs.h"
#include "DataTable.h"
#include "Shedule.h"
#include "DateTime.h"
#include "RTC.h"
#include "CommProtocol.h"
#include "Flash.h"
#include "LightAutomata.h"
#include "LEDs.h"
#include "Test.h"
#include "WDT.h"
#include "DPS.h"
#include "LightSensor.h"
#include "MiniMP3.h"
#include "UART.h"
//DEBUG+
#include "Pins.h"
//#include "FastLoop.h"
#include "Digits.h"
//DEBUG-

// TODO: Сделать полноценный контроль входных данных и выдачу кодов ошибок

//void DEBUG_FillState(void);

int main(void)
{
    TDateTime DT;
    bool bOK;

    __disable_interrupt();

    volatile int i;
    for(i=0;i<30000;i++);

    System_Init();

    InitAutomatas();
    InitData();
    InitLightSensor();

    DetectFlashType();
//    FormatFlash();
//    Flash_SaveState();
//    Flash_SaveAll();
    Flash_LoadAll();

    //DEBUG_FillState();

//    if ( Status.Errors & ERROR_FLASH_NOT_VALID )
//    {
//        FormatFlash();
//    }

    if ( Status.Errors & (ERROR_FLASH_NOT_VALID|ERROR_FLASH_UNCOMPATIBLE_VERSION|ERROR_FLASH_SETTINGS_CORRUPTED) )
    {
//DEBUG+
            //ClearAllLights();
            //SetLights(0, GMAN0_MASK|GMAN1_MASK, CalcNumberMask(Status.Errors+FlashMode, DIGITS_MAX_COUNT));
//DEBUG-

        //InitAutomatas();
        InitData();
        InitLightSensor();
        FormatFlash();
        Flash_SaveState();
        Flash_SaveAll();

//DEBUG+
            //while(1);
//DEBUG-

    }

    InitSound();
    DateTime_Init();

    if ( RTC_GetTime(&DT) )
    {
        Set_DateTime(&DT);
        Get_DateTime(&Status.CurDateTime);
    }
    else
    {
        DT.sec   = 0;
        DT.min   = 0;
        DT.hour  = 0;
        DT.day   = 1;
        DT.month = 1;
        DT.year  = 2012;

        RTC_SetTime(&DT);
        Set_DateTime(&DT);
        Get_DateTime(&Status.CurDateTime);

        Status.Errors |= ERROR_RTC_READ;
    }

    SensorPreInitMeasure();

    FindDayPlan();
    ProcessShedule();

    // Check Test Pin
    if ( IsTestModeIn() )
    {
        // Test Mode
        Status.CurMode = TEST_MODE;
        InitTest();
        GRoundUpValue = DEF_ROUND_UP_TIME;
        RRoundUpValue = DEF_ROUND_UP_TIME;
        Flash_SaveState();
    }
    else
    {
        // Work Mode
        Status.CurMode = WORK_MODE;
    }

    if(Is_WDT_Reset())
    {
        Status.Errors |= ERROR_WDT_RESET;
    }

    WDT_Init();
    __enable_interrupt();

    while (1)
    {
        CheckMasterCommand();
        machineMP3();
        //sec
        if (bNewSec)
        {
            if (Status.CurMode == WORK_MODE)
            {
                bOK = RTC_GetTime(&DT);
                if (bOK)
                {
                    Set_DateTime(&DT);
                    LED1_Toggle();
                    Status.Errors &= ~ERROR_RTC_READ;
                }
                else
                {
                    DateTime_AddSec();
                    Status.Errors |= ERROR_RTC_READ;
                }

                if ( Get_DateTime(&Status.CurDateTime) )
                {
                    //if ( IsNewDay() )
                    {
                        FindDayPlan();
                    }

                    //if ( IsNewMinute() )
                    {
                        ProcessShedule();
                    }

                    CheckLightSensor(&Status.CurDateTime);
                }

                SensorSlowLoopControl();
            }
            else
            {
                bOK = RTC_GetTime(&DT);
                if (bOK)
                {
                    Set_DateTime(&DT);
                    LED1_Toggle();
                }
            }

            bNewSec = false;
        }
    }
}
