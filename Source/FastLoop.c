//---------------------------------------------------------------------------
//
// $Title:          FastLoop.c
// $Description:    Fast Loop Function
//
// $Rev:            0.0.13
//
// $Author:         EEA
// $Created:        9.10.11
// $Last Modified:  5.12.13
//
// $Copyright:      EEA
//
// $Notes:
//
//---------------------------------------------------------------------------
#include "FastLoop.h"

#include "EEA_port.h"
#include "Config.h"

#include "Inputs.h"
#include "LightAutomata.h"
#include "LEDs.h"
#include "DataTable.h"
#include "ADC.h"
#include "Flash.h"

//#include <NXP/iolpc2103.h>
#include "Pins.h"
//#include "Sound.h"
#include "Digits.h"
#include "DemoMode.h"
#include "UART.h"
#include "CommProtocol.h"
#include "Test.h"
#include "WDT.h"
#include "LightSensor.h"
#include "Delay.h"
#include "MiniMP3.h"

typedef enum
{
    POWER_OK,
    POWER_LOW
} TPowerState;

static unsigned int msecCounter;
static unsigned int secCounter;
static unsigned int msCounter;

unsigned int SlaveControlCounter;

unsigned int RedManVar;
unsigned int GreenManVar;
unsigned int DigMask;
int GManPhase;

unsigned int CurR   = 0;
unsigned int CurG   = 0;
unsigned int CurDig = 0;

unsigned int pwmCounter = 0;
unsigned int pwmModulo  = PWM_MODULO;
unsigned int pwmValue   = PWM_MODULO;
unsigned int normBright; // Normalized Bright

uint16 PwrFB;
TPowerState PowerState = POWER_LOW;

//-----------------------------
//Prototypes
void CheckPower(void);
//-----------------------------

void FastLoop(void)
{
    uint32 PortData;

    //PortData = FIOPIN;
    PortData = GetInputsState();

    CheckInputLines(PortData);

    GLineAutomata();
    RLineAutomata();

    msecCounter++;
    if (msecCounter >= (FAST_LOOP_FREQ/1000))
    {
        msecCounter = 0;

        GreenAutomata();
        RedAutomata();

        // DEBUG+
        //Status.CurMode = DEMO_MODE;
        // DEBUG-

        switch (Status.CurMode)
        {
            case TEST_MODE:
            {
                TestAutomata();
                break;
            }
            case WORK_MODE:
            {
                WorkAutomata();
                break;
            }
            case DEMO_MODE:
            {
//                DemoAutomata();
                break;
            }
        };

        UpdateAutomataStatus();

        if (UART_CharPause)
        {
            UART_CharPause--;
        }

        if (UART_AnswerDelay)
        {
            UART_AnswerDelay--;
        }

        if (UART_MasterConnectionTime)
        {
            UART_MasterConnectionTime--;
            if (!UART_MasterConnectionTime)
            {
                UART_bSlaveCmdEnable = true;
            }
        }

    }

    ClearAllLights();
    PowerControl(GreenManVar);

    if (GreenManVar)
        normBright = CurBrightG;
    else
        normBright = CurBrightR;

    if (Config2.bDisableGreenCount)
    {
        DigMask = 0;
    }

    SensorFastLoopControl( !CurR && !CurG && !CurDig );

    CurR   = RedManVar;
    CurG   = GreenManVar;
    CurDig = DigMask;

    if (bSensorDisableLights)
    {
        CurR   = 0;
        CurG   = 0;
        CurDig = 0;
    }

    if ( (pwmCounter<normBright) && (PowerState==POWER_OK) )
    {
        SetLights(CurR, CurG, CurDig);
    }

    pwmCounter++;
    if (pwmCounter>=pwmModulo)
    {
        pwmCounter = 0;
    }

    SlaveControlCounter++;
    if (SlaveControlCounter >= (FAST_LOOP_FREQ/SLAVE_CONTROL_FREQ))
    {
        SlaveControlCounter = 0;
        switch (Status.CurMode)
        {
            case TEST_MODE:
            {
                Test_MakeSlaveCommand();
                break;
            }
            case WORK_MODE:
            {
                MakeSlaveCommand();
                break;
            }
            case DEMO_MODE:
            {
//                Demo_MakeSlaveCommand();
                break;
            }
        };
        //SendSlaveCommand();
    }
    // enter 1 sec
    secCounter++;
    if (secCounter >= FAST_LOOP_FREQ)
    {
        secCounter = 0;
        bNewSec = true;
    }
    // enter 1 ms
    msCounter++;
    if (msCounter >= FAST_LOOP_FREQ_MS)
    {
        msCounter = 0;
        checkButton();
        TimerCooling();
    }
    // Check Power
    CheckPower();

    WDT_Feed();
}
/*----------------------------------------------------------------------------*/
void CheckPower(void)
{
    PwrFB = ADC_GetResult(ADC_POWER_FB_CHANNEL);

    switch (PowerState)
    {
        case POWER_LOW:
        {
            if (PwrFB > POWER_GOOD_LEVEL)
                PowerState = POWER_OK;
            break;
        };
        case POWER_OK:
        {
            if (PwrFB < POWER_BAD_LEVEL)
            {
                PowerState = POWER_LOW;

                RState = 0;
                GState = 0;
                WorkAutomata();
                disabledMode(); // mp3
                Flash_SaveState();
            }
            break;
        };
        default:
        {
            if (PwrFB > POWER_GOOD_LEVEL)
                PowerState = POWER_OK;
            else
                PowerState = POWER_LOW;
            break;
        }
    }
}
