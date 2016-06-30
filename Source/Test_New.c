//---------------------------------------------------------------------------
//
// $Title:          Test.c
// $Description:    Test Mode Functions and Data
//                  
// $Rev:            0.0.6
//
// $Author:         EEA
// $Created:        26.10.11
// $Last Modified:  15.8.14
//
// $Copyright:      EEA
//
// $Notes:          
//
//---------------------------------------------------------------------------
#include "Test.h"

#include "EEA_Port.h"

#include "Pins.h"
#include "DataTable.h"
#include "Config.h"
//#include "Shedule.h"
#include "RTC.h"
#include "FastLoop.h"
#include "Digits.h"
#include "Sound.h"
#include "DateTime.h"
#include "LEDs.h"
#include "LightAutomata.h"
#include "ADC.h"
#include "Flash.h"

#define SEG_LETTER_C_MASK (SHA_MASK|SHD_MASK|SHE_MASK|SHF_MASK)
#define SEG_LETTER_F_MASK (SHA_MASK|SHE_MASK|SHF_MASK|SHG_MASK)
#define SEG_MINUS_MASK    SHG_MASK

#define TEST_SHOW_LV_TIME   1000
#define TEST_SHOW_HV_TIME   1000
#define TEST_RED_PAUSE_TIME 500

#define TEST_CHECK_FLASH_TIME   1000
#define TEST_CHECK_RTC_TIME_SEC 3


//------------------------------------------------
// TEST AUTOMATA
//------------------------------------------------

// Test Mode
uint32 TestCounter;
uint32 Test_sec   ;
int32  Test_DigR  ;
int32  Test_DigG  ;
TTestState TestState;
TTestState TestStatePrev;

uint8 Test_SampleNumBackup;

uint16 Voltage;

bool bTest_StartRTCCheck;
bool bTest_RTCCheckOK;

bool bTest_StartFlashCheck;
bool bTest_FlashCheckOK;

//---------------------------------------
static int32 ManStepCounter = -1;  // sec
//uint8 GManPhase = 0;
static int32 SoundRepeatCounter = -1;  // sec
static int32 secCounter = -1;   // sec
static int32 ManStepPeriod     = MAN_STEP_PERIOD;


//---------------------------------------
bool TestRG_ShowLowVolt(bool bInit);
bool TestRG_ShowHighVolt(bool bInit);
bool TestRG_CheckFlash(bool bInit);
bool TestRG_CheckRTC(bool bInit);



void InitTest(void)
{
    TestState     = (TTestState)0;
    TestStatePrev = TEST_END;         

    TestCounter = 0; 
    Test_sec    = 0; 
    Test_DigR   = -1;
    Test_DigG   = -1;

    SetBright_Prc(TEST_DEF_BRIGHT);
    SetVolume_Prc(TEST_DEF_VOLUME);

    Config.SampleNum = 0;
}

void TestAutomata(void)
{
    switch (TestState)
    {
        case TESTRG_CALC_LOW_VOLT:
        {
            if ( TestRG_ShowLowVolt(TestState != TestStatePrev) )
            {
                if (IsTestModeIn())
                {
                    TestState++;
                }
                else
                {
                    TestRG_ShowLowVolt(true);
                }
            }
            
            break;
        };

        case TESTRG_CALC_HIGH_VOLT:
        {
            if ( TestRG_ShowLowVolt(TestState != TestStatePrev) )
            {
                if (IsTestModeIn())
                {
                    TestState++;
                }
                else
                {
                    TestRG_ShowLowVolt(true);
                }
            }
            
            break;
        };

        case TESTRG_CHECK_FLASH:
        {
            if ( TestRG_CheckFlash(TestState != TestStatePrev) )
            {
                if (IsTestModeIn())
                {
                    TestState++;
                }
                else
                {
                    TestRG_CheckFlash(true);
                }
            }
            
            break;
        };

        case TESTRG_CHECK_RTC:
        {
            if ( TestRG_CheckRTC(TestState != TestStatePrev) )
            {
                if (IsTestModeIn())
                {
                    TestState++;
                }
                else
                {
                    TestRG_CheckRTC(true);
                }
            }
            
            break;
        };

        case TESTRG_RED_PAUSE:
        {
            break;
        };

        case TESTR:
        {
            break;
        };

        case TESTG:
        {
            break;
        };

        default:
        case TEST_END:
        {
            TestState = (TTestState)0;
            break;
        };
    };
    
    if (TestState >= TEST_END)
        TestState = (TTestState)0;
}

void Test_MakeSlaveCommand(void)
{
    if (GreenManVar)
        SlaveCommand.GreenState = GManPhase+1;
    else
        SlaveCommand.GreenState = 0;
    
    SlaveCommand.RedCount   = Test_DigR;
    SlaveCommand.Bright     = CurBright;
    SlaveCommand.Modulo     = PWM_MODULO;
}


bool TestRG_ShowLowVolt(bool bInit)
{
    static int Test_ShowLV_State;
    static uint16 Voltage;
    
    if (bInit)
    {
        TestStatePrev = TestState;
        secCounter   = 0;
        TestCounter  = 0;
        Test_sec     = 0;

        Test_DigG    = -1;
        Test_DigR    = -1;

        DigMask      = 0;
        
        GreenManVar  = 0;
        RedManVar    = 0;
        GManPhase    = 0;

        Show_LV_State = 0;
    }
    
    secCounter++;
    
    switch (Test_ShowLV_State)
    {
        case 0:
        {
            if (secCounter>=50)
            {
                Voltage = ADC_GetResult(ADC_POWER_FB_CHANNEL);

                Test_DigG = (VOLTAGE_SCALE * (uint32)Voltage) >> 16;
                DigMask   = CalcNumberMask(Test_DigG, GREEN_MAX_COUNT);

                GreenManVar  = GMAN0_MASK|GMAN1_MASK;
                GManPhase    = 0;

                secCounter = 0;

                Test_ShowLV_State = 1;
            }

            break;
        };

        case 1:
        {
            if (secCounter>=TEST_SHOW_LV_TIME)
            {
                DigMask      = 0;
                
                GreenManVar  = 0;
                RedManVar    = RMAN_MASK;
                GManPhase    = 0;
                
                secCounter = 0;
                Test_ShowLV_State = 2;
            }

            break;
        };
                        
        case 2:
        {
            if (secCounter>=TEST_RED_PAUSE_TIME)
            {
                secCounter = 0;
                return true;
            }

            break;
        };

        default:
        {
            Test_ShowLV_State = 0;
            break;
        }
    }

    return false;
}

bool TestRG_ShowHighVolt(bool bInit)
{
    static int Test_ShowHV_State;
    static uint16 Voltage;
    
    if (bInit)
    {
        TestStatePrev = TestState;
        secCounter   = 0;
        TestCounter  = 0;
        Test_sec     = 0;

        Test_DigG    = -1;
        Test_DigR    = -1;

        DigMask      = 0;
        
        RedManVar    = 0;
        GreenManVar  = GMAN0_MASK|GMAN1_MASK;
        GManPhase    = 0;

        Show_HV_State = 0;
    }
    
    secCounter++;
    
    switch (Test_ShowHV_State)
    {
        case 0:
        {
            if (secCounter>=50)
            {
                Voltage = ADC_GetResult(ADC_POWER_FB_CHANNEL);

                Test_DigG = (VOLTAGE_SCALE * (uint32)Voltage) >> 16;
                DigMask   = CalcNumberMask(Test_DigG, GREEN_MAX_COUNT);

                GreenManVar  = GMAN0_MASK|GMAN1_MASK;

                secCounter = 0;

                Test_ShowHV_State = 1;
            }

            break;
        };

        case 1:
        {
            if (secCounter>=TEST_SHOW_HV_TIME)
            {
                DigMask      = 0;
                
                GreenManVar  = 0;
                RedManVar    = RMAN_MASK;
                GManPhase    = 0;
                
                secCounter = 0;
                Test_ShowHV_State = 2;
            }

            break;
        };
                        
        case 2:
        {
            if (secCounter>=TEST_RED_PAUSE_TIME)
            {
                secCounter = 0;
                return true;
            }

            break;
        };

        default:
        {
            Test_ShowHV_State = 0;
            break;
        }
    }

    return false;
}

bool TestRG_CheckFlash(bool bInit)
{
    if (bInit)
    {
        TestStatePrev = TestState;
        secCounter   = 0;
        TestCounter  = 0;
        Test_sec     = 0;

        Test_DigG    = -1;
        Test_DigR    = -1;

        RedManVar    = 0;
        GreenManVar  = GMAN0_MASK|GMAN1_MASK;
        GManPhase    = 0;

        if ( Flash_LoadAll() )
        {
            DigMask = SEG_LETTER_F_MASK+SEG_DIGIT_1_MASK;
        }
        else
        {
            DigMask = SEG_LETTER_F_MASK+SEG_DIGIT_0_MASK;
        }
    }

    secCounter++;
    
    if (secCounter >= TEST_CHECK_FLASH_TIME)
    {
        secCounter = 0;
        return true;
    }
    
    return false;
}

bool TestRG_CheckRTC(bool bInit)
{
    TDateTime DT;
    
    if ( (TestState != TestStatePrev) || !secCounter )
    {
    }

    if (bInit)
    {
        TestStatePrev = TestState;
        secCounter   = 0;
        TestCounter  = 0;
        Test_sec     = 0;

        Test_DigG    = -1;
        Test_DigR    = -1;

        RedManVar    = 0;
        GreenManVar  = GMAN0_MASK|GMAN1_MASK;
        GManPhase    = 0;

        if ( RTC_GetTime(&DT) )
        {
            DigMask = SEG_LETTER_C_MASK+CalcLoDigitMask(DT.sec);
        }
        else
        {
            DigMask = SEG_LETTER_C_MASK+SEG_MINUS_MASK;
        }
    }

    secCounter++;

    if (secCounter >= 100)
    {
        Test_sec++;
        if (Test_sec >= TEST_CHECK_RTC_TIME_SEC*10)
        {
            Test_sec = 0;
            return true;
        }

        if ( RTC_GetTime(&DT) )
        {
            DigMask = SEG_LETTER_C_MASK+CalcLoDigitMask(DT.sec);
        }
        else
        {
            DigMask = SEG_LETTER_C_MASK+SEG_MINUS_MASK;
        }
        
        secCounter = 0;
    }
    
    return false;
}

