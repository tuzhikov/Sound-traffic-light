//---------------------------------------------------------------------------
//
// $Title:          DemoMode.c
// $Description:    Demo Mode Functions and Data.
//
// $Rev:            0.0.7
//
// $Author:         EEA
//
// $Created:        20.10.11
// $Last Modified:  15.8.14
//
// $Copyright:      EEA
//
// $Notes:
//
//---------------------------------------------------------------------------
#include "DemoMode.h"

#include "EEA_Port.h"
#include "Pins.h"

#include "DataTable.h"
#include "Config.h"
#include "Shedule.h"
#include "RTC.h"
#include "FastLoop.h"
#include "Digits.h"
#include "Sound.h"
#include "DateTime.h"
#include "LEDs.h"

//------------------------------------------------
// DEMO AUTOMATA
//------------------------------------------------

// Demo Mode
uint32 DemoCounter   = 0;
uint32 Demo_sec      = 0;
int32  Demo_DigR     = -1;
int32  Demo_DigG     = -1;
TDemoState DemoState     = (TDemoState)0;
TDemoState DemoStatePrev = DEMO_END;

const uint8 DemoBrightTable[] = {100, 80, 60, 40, 20};
const uint8 DemoVolumeTable[] = {100, 50, 10, 5, 1};

uint8 Demo_SampleNumBackup;
uint8 Demo_ModeBackup;

//---------------------------------------
static int32 ManStepCounter = -1;  // sec
//uint8 GManPhase = 0;
static int32 SoundRepeatCounter = -1;  // sec
static int32 secCounter = -1;   // sec
static int32 ManStepPeriod     = MAN_STEP_PERIOD;

//------------------------------------------------
/*
void DemoAutomata(void)
{
    switch (DemoState)
    {
        case DEMO_YELLOW1:
        case DEMO_YELLOW2:
        {
            if (DemoState != DemoStatePrev)
            {
                DemoStatePrev = DemoState;

                secCounter   = 0;
                Demo_sec     = 0;
                Demo_DigR    = -1;
                Demo_DigG    = -1;

                DigMask      = 0;
                
                RedManVar    = 0;
                GreenManVar  = 0;
                //SYellowVar   = SYellow;
            }

            secCounter++;
            if (secCounter>=1000)
            {
                secCounter = 0;
    
                Demo_sec++;
                if (Demo_sec >= 5)
                {
                    DemoState++;
                } 
            }        
            break;
        };

        case DEMO_REDBACK:
        {
            if (DemoState != DemoStatePrev)
            {
                DemoStatePrev = DemoState;

                secCounter   = 0;
                Demo_sec     = 0;
                Demo_DigR    = 99;
                Demo_DigG    = -1;

                DigMask = CalcNumberMask(Demo_DigR, RED_MAX_COUNT);

                RedManVar   = RMAN_MASK;
                GreenManVar = 0;
                //SYellowVar   = 0;
            }

            secCounter++;
            if (secCounter>=1000)
            {
                secCounter = 0;
    
                Demo_DigR -= 11;
                
                if (Demo_DigR>=0)
                {
                    DigMask = CalcNumberMask(Demo_DigR, RED_MAX_COUNT);
                }
                else
                {
                    DemoState++;
                }
            }        
            break;
        };

        case DEMO_RED:
        {
            if (DemoState != DemoStatePrev)
            {
                DemoStatePrev = DemoState;

                secCounter   = 0;
                Demo_sec     = 0;
                Demo_DigR    = -1;
                Demo_DigG    = -1;

                RedManVar   = RMAN_MASK;
                GreenManVar = 0;
                //SYellowVar   = 0;
            }

            secCounter++;
            if (secCounter>=1000)
            {
                secCounter = 0;
    
                Demo_sec++;
                if (Demo_sec >= 5)
                {
                    DemoState++;
                } 
            }        

            break;
        };

        case DEMO_GREENBACK:
        {
            if (DemoState != DemoStatePrev)
            {
                DemoStatePrev = DemoState;

                secCounter   = 0;
                Demo_sec     = 0;
                Demo_DigR    = -1;
                Demo_DigG    = 99;

                DigMask = CalcNumberMask(Demo_DigG, GREEN_MAX_COUNT);

                RedManVar   = 0;
                GreenManVar = GMAN0_MASK|GMAN1_MASK;
                GManPhase = 0;
                //SYellowVar   = 0;
            }

            secCounter++;
            if (secCounter>=1000)
            {
                secCounter = 0;
    
                Demo_DigG -= 11;
                
                if (Demo_DigG>=0)
                {
                    DigMask = CalcNumberMask(Demo_DigG, GREEN_MAX_COUNT);
                }
                else
                {
                    //SDig = 0;
                    DemoState++;
                    break;
                }
            }        
            break;
        };

        default:
        case DEMO_END:
        {
            DemoState = (TDemoState)0;
            break;
        };
    };
    
    if (DemoState >= DEMO_END)
        DemoState = (TDemoState)0;
}
*/
/*
void DemoAutomata(void)
{
    switch (DemoState)
    {
        default:
        case DEMO_END:
        {
            DemoState = (TDemoState)0;
            break;
        };
    };
    
    if (DemoState >= DEMO_END)
        DemoState = (TDemoState)0;
}
*/

void DemoAutomata(void)
{
    TDateTime DT;
    
    switch (DemoState)
    {
        case DEMO_RED_BRIGHT:
        {
            if (DemoState != DemoStatePrev)
            {
                DemoStatePrev = DemoState;

                secCounter   = 0;
                DemoCounter  = 0;
                Demo_sec     = 0;

                Demo_DigG    = -1;
                
                Demo_DigR = DemoBrightTable[DemoCounter];
                SetBright_Prc(Demo_DigR);

                DigMask   = 0;
                
                RedManVar    = RMAN_MASK;
                GreenManVar  = 0;
            }

            secCounter++;
            if (secCounter>=1000)
            {
                secCounter = 0;
    
                Demo_sec++;
                if (Demo_sec >= DEMO_DEF_TIME)
                {
                    Demo_sec = 0;
                    
                    DemoCounter++;
                    if (DemoCounter < sizeof(DemoBrightTable))
                    {
                        Demo_DigR = DemoBrightTable[DemoCounter];
                        SetBright_Prc(Demo_DigR);
                    }
                    else
                    {
                        DemoState++;
                        LED2_Toggle();
                    }
                } 
            }        
            break;
        };
        
        
        case DEMO_GREEN_BRIGHT:
        {
            if (DemoState != DemoStatePrev)
            {
                DemoStatePrev = DemoState;

                secCounter   = 0;
                DemoCounter  = 0;
                Demo_sec     = 0;

                Demo_DigR    = -1;
                
                Demo_DigG = DemoBrightTable[DemoCounter];

                SetBright_Prc(Demo_DigG);
                SetVolume_Prc(Demo_DigG);

                DigMask   = CalcNumberMask(Demo_DigG, GREEN_MAX_COUNT);
                
                RedManVar   = 0;
                GreenManVar = GMAN0_MASK|GMAN1_MASK;
                GManPhase = 0;

                ManStepCounter     = 0;
                SoundRepeatCounter = 0;
                ManStepPeriod      = MAN_STEP_PERIOD;

                Config.SampleNum = 0;

                StartSound();
            }

            secCounter++;
            if (secCounter>=1000)
            {
                secCounter = 0;
    
                Demo_sec++;
                if (Demo_sec >= DEMO_DEF_TIME)
                {
                    Demo_sec = 0;
                    
                    DemoCounter++;
                    if (DemoCounter < sizeof(DemoBrightTable))
                    {
                        Demo_DigG = DemoBrightTable[DemoCounter];

                        SetBright_Prc(Demo_DigG);
                        SetVolume_Prc(Demo_DigG);
                        
                        DigMask   = CalcNumberMask(Demo_DigG, GREEN_MAX_COUNT);
                    }
                    else
                    {
                        if (Config.SampleNum<3)
                        {
                            Config.SampleNum++;
            
                            secCounter   = 0;
                            DemoCounter  = 0;
                            Demo_sec     = 0;

                            Demo_DigR    = -1;
                            
                            Demo_DigG = DemoBrightTable[DemoCounter];
                        
                            SetBright_Prc(Demo_DigG);
                            SetVolume_Prc(Demo_DigG);

                            DigMask   = CalcNumberMask(Demo_DigG, GREEN_MAX_COUNT);
                            
                            RedManVar   = 0;
                            GreenManVar = GMAN0_MASK|GMAN1_MASK;
                            GManPhase   = 0;

                            ManStepCounter     = 0;
                            SoundRepeatCounter = 0;
                            ManStepPeriod      = MAN_STEP_PERIOD;

                            StartSound();
                        }
                        else
                        {
                            DemoState++;
                            LED2_Toggle();
                            break;
                        }
                    }
                } 
            }

            ManStepCounter++;
            if (ManStepCounter >= ManStepPeriod)
            {
                GreenManVar ^= GMAN1_MASK|GMAN2_MASK;
                GManPhase = !GManPhase;
                ManStepCounter = 0;
            }

            SoundRepeatCounter++;
            if (SoundRepeatCounter >= (ManStepPeriod*2))
            {
                StartSound();
                SoundRepeatCounter = 0;
            }

            break;
        };

        case DEMO_TIME:
        {
            if (DemoState != DemoStatePrev)
            {
                DemoStatePrev = DemoState;

                if (!RTC_GetTime(&DT))
                {
                    DemoState++;
                }
                else
                {
                    secCounter   = 0;
                    DemoCounter  = 0;
                    Demo_sec     = 0;

                    Demo_DigG    = DT.min;
                    Demo_DigR    = DT.hour;
                    DigMask      = CalcNumberMask(Demo_DigG, GREEN_MAX_COUNT);

                    RedManVar    = 0;
                    GreenManVar  = GMAN0_MASK|GMAN1_MASK;
                    GManPhase    = -1;
                }
            }

            secCounter++;
            if (secCounter>=1000)
            {
                secCounter = 0;
    
                Demo_sec++;
                if (Demo_sec >= DEMO_DEF_TIME*2)
                {
                    Demo_sec = 0;
                    DemoState++;
                    LED2_Toggle();
                }            
            }
            
            break;
        };
/*
        case DEMO_FADE_BRIGHT:
        {
            if (DemoState != DemoStatePrev)
            {
                DemoStatePrev = DemoState;

                secCounter   = 0;
                DemoCounter  = 0;
                Demo_sec     = 0;

                Demo_DigG    = -1;
                
                Demo_DigR = 100;
                SetBright_Prc(Demo_DigR);

                DigMask   = 0;
                
                RedManVar    = RMAN_MASK;
                GreenManVar  = 0;
            }
            
            secCounter++;
            if (secCounter>=1000)
            {
                secCounter = 0;
    
                Demo_sec++;
                if (Demo_sec >= DEMO_DEF_TIME)
                {
                    Demo_sec = 0;
                    
                    DemoCounter++;
                    if (DemoCounter < sizeof(DemoBrightTable))
                    {
                        Demo_DigR = DemoBrightTable[DemoCounter];
                        SetBright_Prc(Demo_DigR);
                    }
                    else
                    {
                        DemoState++;
                    }
                } 
            }        
            break;
        };

*/        
        default:
        case DEMO_END:
        {
            DemoState = (TDemoState)0;
            break;
        };
    };
    
    if (DemoState >= DEMO_END)
        DemoState = (TDemoState)0;
}

void EnterDemoMode(void)
{
    Demo_SampleNumBackup = Config.SampleNum;
    Demo_ModeBackup      = Status.CurMode;
    Status.CurMode       = DEMO_MODE;
}

void ExitDemoMode(void)
{
    TDateTime DT;
  
    Config.SampleNum = Demo_SampleNumBackup;
    Status.CurMode   = Demo_ModeBackup;

    if ( RTC_GetTime(&DT) )
    {
        Set_DateTime(&DT);
        FindDayPlan();
        ProcessShedule();
    }
}

void Demo_MakeSlaveCommand(void)
{
    if (GreenManVar)
        SlaveCommand.GreenState = GManPhase+1;
    else
        SlaveCommand.GreenState = 0;
    
    SlaveCommand.RedCount = (uint8)Demo_DigR;

    if (SlaveCommand.GreenState)
        SlaveCommand.Bright     = CurBrightR;
    else
        SlaveCommand.Bright     = CurBrightG;

    SlaveCommand.Modulo     = PWM_MODULO;
}

