//---------------------------------------------------------------------------
//
// $Title:          Test.c
// $Description:    Test Mode Functions and Data
//                  
// $Rev:            0.0.13
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
#include "Inputs.h"
#include "I2C.h"



#define SEG_LETTER_C_MASK (SHA_MASK|SHD_MASK|SHE_MASK|SHF_MASK)
#define SEG_LETTER_F_MASK (SHA_MASK|SHE_MASK|SHF_MASK|SHG_MASK)
#define SEG_MINUS_MASK    SLG_MASK

#define TEST_SHOW_LV_TIME   1000
#define TEST_SHOW_HV_TIME   1000
#define TEST_SHOW_LSV_TIME  1000
#define TEST_RED_PAUSE_TIME 1000
#define TEST_PAUSE_TIME     1000

#define TEST_MEASURE_LV_TIME 200
#define TEST_MEASURE_HV_TIME 200
#define TEST_MEASURE_LSV_TIME   50

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

typedef struct
{
    uint8 Num;
    uint8 Snd;
} TTestGData;

#if (GREEN_MAX_COUNT<100)
const TTestGData TestGreenArray[] = { {88, 0}, {77, 0}, {66, 1}, {55, 1}, {44, 2}, {33, 2}, {22, 3}, {11, 3} };
#else
const TTestGData TestGreenArray[] = { {188, 0}, {77, 0}, {66, 1}, {55, 1}, {44, 2}, {33, 2}, {22, 3}, {11, 3} };
#endif

#if (RED_MAX_COUNT<100)
const uint8 TestRedArray[] = { 88, 77, 66, 55, 44, 33, 22, 11 /*, SOFTWARE_VERSION_H, SOFTWARE_VERSION_L */};
#else
const uint8 TestRedArray[] = { 188, 77, 66, 55, 44, 33, 22, 11 /*, SOFTWARE_VERSION_H, SOFTWARE_VERSION_L */};
#endif

//---------------------------------------
static int32 ManStepCounter;  // sec
//uint8 GManPhase = 0;
static int32 SoundRepeatCounter;  // sec
static int32 secCounter;   // sec
static int32 ManStepPeriod;

//---------------------------------------
bool TestRG_ShowLowVolt(bool bInit);
bool TestRG_ShowHighVolt(bool bInit);
bool TestRG_CheckFlash(bool bInit);
bool TestRG_CheckRTC(bool bInit);
bool TestRG_ShowLightSensVolt(bool bInit);
//---------------------------------------

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

    ManStepCounter      = -1;  // sec
    SoundRepeatCounter  = -1;  // sec
    secCounter          = -1;   // sec
    ManStepPeriod       = MAN_STEP_PERIOD;
}

void TestAutomata(void)
{
    //TDateTime DT;
    
    if (RState && !GState)
    {
        if ( (TestState <= TESTR_START) || (TestState >= TESTR_END) )
            TestState = TESTR_START+1;
//        TestState = TEST_RED_COUNT;
        SetBright_Prc(TEST_DEF_BRIGHT);
    }
    else if (GState && !RState)
    {
        if ( (TestState <= TESTG_START) || (TestState >= TESTG_END) )
            TestState = TESTG_START+1;
//        TestState = TEST_GREEN_COUNT;
        SetBright_Prc(TEST_DEF_BRIGHT);
    }
    else if (GState && RState)
    {
        if ( (TestState <= TESTRG_START) || (TestState >= TESTRG_END) )
            TestState = TESTRG_START+1;

        SetBright_Prc(TEST_DEF_BRIGHT);
    }
    else
    {
        if ( (TestState <= TEST_START) || (TestState >= TEST_END) )
            TestState = TEST_START+1;

        SetBright_Prc(TEST_DEF_BRIGHT);
    }
    
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
            if ( TestRG_ShowHighVolt(TestState != TestStatePrev) )
            {
                if (IsTestModeIn())
                {
                    TestState++;
                }
                else
                {
                    TestRG_ShowHighVolt(true);
                }
            }
            
            break;
        };

        case TESTRG_CALC_LSENS_VOLT:
        {
            if ( TestRG_ShowLightSensVolt(TestState != TestStatePrev) )
            {
                if (IsTestModeIn())
                {
                    TestState++;
                }
                else
                {
                    TestRG_ShowLightSensVolt(true);
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

        case TESTR_COUNT:
        {
            if (TestState != TestStatePrev)
            {
                TestStatePrev = TestState;

                secCounter   = 0;
                TestCounter  = 0;
                Test_sec     = 0;

                Test_DigG    = -1;
                Test_DigR    = TestRedArray[TestCounter];
                
                RedManVar    = RMAN_MASK;
                GreenManVar  = 0;
                DigMask      = 0;
            }

            secCounter++;
            if (secCounter>=1000)
            {
                secCounter = 0;

                if (IsTestModeIn())                
                {
                    TestCounter++;

                    if (TestCounter < sizeof(TestRedArray))
                        Test_DigR    = TestRedArray[TestCounter];
                    else                
                    {
                        TestState++;
                        TestStatePrev = TEST_END;
                    }
                }
            }        
            break;
        };
        
        case TESTG_COUNT:
        {
            if (TestState != TestStatePrev)
            {
                TestStatePrev = TestState;

                secCounter   = 0;
                TestCounter  = 0;
                Test_sec     = 0;

                Test_DigG    = TestGreenArray[TestCounter].Num;
                Test_DigR    = -1;

                DigMask      = CalcNumberMask(Test_DigG, GREEN_MAX_COUNT);
                
                RedManVar    = 0;
                GreenManVar  = GMAN0_MASK|GMAN1_MASK;
                GManPhase    = 0;

                Config.SampleNum = TestGreenArray[TestCounter].Snd;

                StartSound();

                ManStepCounter     = 0;
                SoundRepeatCounter = 0;
            }

            secCounter++;
            if (secCounter>=1000)
            {
                secCounter = 0;

                if (IsTestModeIn())                
                {
                    TestCounter++;
                    if (TestCounter < (sizeof(TestGreenArray)/sizeof(TestGreenArray[0])))
                    {
                        Test_DigG    = TestGreenArray[TestCounter].Num;
                        DigMask      = CalcNumberMask(Test_DigG, GREEN_MAX_COUNT);
                        Config.SampleNum = TestGreenArray[TestCounter].Snd;
                    }
                    else                
                    {
                        TestState++;
                        TestStatePrev = TEST_END;
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

        case TESTRG_RED_PAUSE:
        {
            if (TestState != TestStatePrev)
            {
                TestStatePrev = TestState;

                secCounter  = 0;
                TestCounter = 0; 

                Test_DigR   = -1;
                Test_DigG   = -1;

                RedManVar   = RMAN_MASK;
                GreenManVar = 0;
                DigMask     = 0;
            }

            secCounter++;
            if (secCounter>=TEST_RED_PAUSE_TIME)
            {
                secCounter = 0;
                if (IsTestModeIn())                
                    TestState++;
            }        

            break;
        };
        
        case TESTR_PAUSE1:
        case TESTR_PAUSE2:
        case TESTR_PAUSE3:
        case TESTR_PAUSE4:
        {
            if (TestState != TestStatePrev)
            {
                TestStatePrev = TestState;

                secCounter  = 0;
                TestCounter = 0; 

                Test_DigR   = -1;
                Test_DigG   = -1;

                RedManVar   = 0;
                GreenManVar = 0;
                DigMask     = 0;
            }

            secCounter++;
            if (secCounter>=TEST_PAUSE_TIME)
            {
                secCounter = 0;
                if (IsTestModeIn())                
                    TestState++;
            }        

            break;
        };
        
        case TESTR_VERSION_H:
        {
            if (TestState != TestStatePrev)
            {
                TestStatePrev = TestState;

                secCounter  = 0;
                TestCounter = 0; 

                Test_DigR   = -1;
                Test_DigG   = SOFTWARE_VERSION_H;

                RedManVar   = 0;
                GreenManVar = GMAN0_MASK|GMAN1_MASK;
                DigMask     = CalcNumberMask(Test_DigG, GREEN_MAX_COUNT);
            }

            secCounter++;
            if (secCounter>=1000)
            {
                secCounter = 0;
                if (IsTestModeIn())                
                    TestState++;
            }        

            break;
        };
        
        case TESTR_VERSION_L:
        {
            if (TestState != TestStatePrev)
            {
                TestStatePrev = TestState;

                secCounter  = 0;
                TestCounter = 0; 

                Test_DigR   = -1;
                Test_DigG   = SOFTWARE_VERSION_L;

                RedManVar   = 0;
                GreenManVar = GMAN0_MASK|GMAN1_MASK;
                DigMask     = CalcNumberMask(Test_DigG, GREEN_MAX_COUNT);
            }

            secCounter++;
            if (secCounter>=1000)
            {
                secCounter = 0;
                if (IsTestModeIn())                
                    TestState++;
            }        

            break;
        };

/*
        case TEST_RED:
        {
            if (TestState != TestStatePrev)
            {
                TestStatePrev = TestState;

                secCounter   = 0;
                TestCounter  = 0;
                Test_sec     = 0;

                Test_DigG    = -1;
                
                Test_DigR    = TEST_RED_TIME;

                DigMask   = 0;
                
                RedManVar    = RMAN_MASK;
                GreenManVar  = 0;
            }

            secCounter++;
            if (secCounter>=1000)
            {
                secCounter = 0;

                if (Test_DigR>=0)
                    Test_DigR--;
                
                Test_sec++;
                if (Test_sec >= TEST_RED_TIME)
                {
                    TestState++;
                } 
            }        
            break;
        };
        
        
        case TEST_GREEN:
        {
            if (TestState != TestStatePrev)
            {
                TestStatePrev = TestState;

                secCounter   = 0;
                TestCounter  = 0;
                Test_sec     = 0;

                Test_DigG    = TEST_GREEN_TIME;
                Test_DigR    = -1;

                DigMask      = CalcNumberMask(Test_DigG, GREEN_MAX_COUNT);
                
                RedManVar    = 0;
                GreenManVar  = GMAN0_MASK|GMAN1_MASK;
                GManPhase    = 0;
            }

            secCounter++;
            if (secCounter>=1000)
            {
                secCounter = 0;

                if (Test_DigG>=0)
                    Test_DigG--;
                
                DigMask = CalcNumberMask(Test_DigG, GREEN_MAX_COUNT);
                
                Test_sec++;
                if (Test_sec >= TEST_GREEN_TIME)
                {
                    Config.SampleNum++;
                    if (Config.SampleNum >= MAX_SOUND_NUMBER)
                        Config.SampleNum = 0;

                    secCounter   = 0;
                    TestCounter  = 0;
                    Test_sec     = 0;

                    Test_DigG    = TEST_GREEN_TIME;
                    Test_DigR    = -1;

                    DigMask      = CalcNumberMask(Test_DigG, GREEN_MAX_COUNT);
                    
                    RedManVar    = 0;
                    GreenManVar  = GMAN0_MASK|GMAN1_MASK;
                    GManPhase    = 0;

                    TestState++;
                    break;
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
        
        
        case TEST_GREEN_COUNT:
        {
            if (TestState != TestStatePrev)
            {
                TestStatePrev = TestState;

                secCounter   = 0;
                TestCounter  = 0;
                Test_sec     = 0;

                Test_DigG    = TEST_GREEN_START_NUM;
                Test_DigR    = -1;

                DigMask      = CalcNumberMask(Test_DigG, GREEN_MAX_COUNT);
                
                RedManVar    = 0;
                GreenManVar  = GMAN0_MASK|GMAN1_MASK;
                GManPhase    = 0;
            }

            secCounter++;
            if (secCounter>=1000)
            {
                secCounter = 0;

                Test_DigG -= TEST_GREEN_DEC_NUM;
                if (Test_DigG>=0)
                {
                    DigMask = CalcNumberMask(Test_DigG, GREEN_MAX_COUNT);
                }
                else
                {                
                    Config.SampleNum++;
                    if (Config.SampleNum >= MAX_SOUND_NUMBER)
                        Config.SampleNum = 0;

                    TestState++;
                    break;
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

        case TEST_RED_COUNT:
        {
            if (TestState != TestStatePrev)
            {
                TestStatePrev = TestState;

                secCounter   = 0;
                TestCounter  = 0;
                Test_sec     = 0;

                Test_DigG    = -1;
                Test_DigR    = TEST_RED_START_NUM;

                DigMask      = 0;
                
                RedManVar    = RMAN_MASK;
                GreenManVar  = 0;
            }

            secCounter++;
            if (secCounter>=1000)
            {
                secCounter = 0;

                Test_DigR -= TEST_RED_DEC_NUM;
                if (Test_DigR<0)
                {                
                    secCounter   = 0;
                    TestCounter  = 0;
                    Test_sec     = 0;

                    Test_DigG    = -1;
                    Test_DigR    = TEST_RED_START_NUM;

                    DigMask      = 0;
                    
                    RedManVar    = RMAN_MASK;
                    GreenManVar  = 0;

                    TestState++;
                } 
            }

            break;
        };

        case TEST_TIME:
        {
            if (TestState != TestStatePrev)
            {
                TestStatePrev = TestState;

                secCounter   = 0;
                TestCounter  = 0;
                Test_sec     = 0;

                if (!RTC_GetTime(&DT))
                {
                    Test_DigG    = 99;
                    Test_DigR    = 199;
                }
                else
                {
                    Test_DigG    = DT.min;
                    Test_DigR    = DT.sec;
                }

                DigMask      = CalcNumberMask(Test_DigG, GREEN_MAX_COUNT);

                RedManVar    = 0;
                GreenManVar  = GMAN0_MASK|GMAN1_MASK;
                GManPhase    = -1;
            }

            secCounter++;
            if (secCounter>=1000)
            {
                secCounter = 0;
    
                if (!RTC_GetTime(&DT))
                {
                    Test_DigG    = 99;
                    Test_DigR    = 199;
                }
                else
                {
                    Test_DigG    = DT.min;
                    Test_DigR    = DT.sec;
                }                                                                             

                DigMask      = CalcNumberMask(Test_DigG, GREEN_MAX_COUNT);
                
                Test_sec++;
                if (Test_sec >= TEST_CLOCK_MODE_TIME)
                {
                    TestState++;
                }            
            }
            
            break;
        };

        case Test_FADE_BRIGHT:
        {
            if (TestState != TestStatePrev)
            {
                TestStatePrev = TestState;

                secCounter   = 0;
                TestCounter  = 0;
                Test_sec     = 0;

                Test_DigG    = -1;
                
                Test_DigR = 100;

                SetBright_Prc(Test_DigR);

                DigMask   = 0;
                
                RedManVar    = RMAN_MASK;
                GreenManVar  = 0;
            }
            
            secCounter++;
            if (secCounter>=1000)
            {
                secCounter = 0;
    
                Test_sec++;
                if (Test_sec >= Test_DEF_TIME)
                {
                    Test_sec = 0;
                    
                    TestCounter++;
                    if (TestCounter < sizeof(TestBrightTable))
                    {
                        Test_DigR = TestBrightTable[TestCounter];
                        SetBright_Prc(Test_DigR);
                    }
                    else
                    {
                        TestState++;
                    }
                } 
            }        
            break;
        };

*/        
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
    
    if (SlaveCommand.GreenState)
        SlaveCommand.Bright     = CurBrightG;
    else
        SlaveCommand.Bright     = CurBrightR;

    SlaveCommand.RedCount   = (uint8)Test_DigR;
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
        
        RedManVar    = 0;
        GreenManVar  = 0;
        GManPhase    = 0;

        Test_ShowLV_State = 0;
    }
    
    secCounter++;
    
    switch (Test_ShowLV_State)
    {
        case 0:
        {
            if (secCounter>=TEST_MEASURE_LV_TIME)
            {
                Voltage = ADC_GetResult(ADC_POWER_FB_CHANNEL);

                Test_DigG = (VOLTAGE_SCALE * (uint32)Voltage) >> 16;
                DigMask   = CalcNumberMask(Test_DigG, GREEN_MAX_COUNT);

                RedManVar    = 0;
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
                //DigMask      = 0;
                
                //RedManVar    = RMAN_MASK;
                //GreenManVar  = 0;
                //GManPhase    = 0;
                
                secCounter = 0;
                //Test_ShowLV_State = 2;
                return true;
            }

            break;
        };
                        
        //case 2:
        //{
        //    if (secCounter>=TEST_RED_PAUSE_TIME)
        //    {
        //        secCounter = 0;
        //        return true;
        //    }
        //
        //    break;
        //};

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

        Test_ShowHV_State = 0;
    }
    
    secCounter++;
    
    switch (Test_ShowHV_State)
    {
        case 0:
        {
            if (secCounter>=TEST_MEASURE_HV_TIME)
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
                //DigMask      = 0;
                
                //GreenManVar  = 0;
                //RedManVar    = RMAN_MASK;
                //GManPhase    = 0;
                
                secCounter = 0;
                //Test_ShowHV_State = 2;
                return true;
            }

            break;
        };
                        
        //case 2:
        //{
        //    if (secCounter>=TEST_RED_PAUSE_TIME)
        //    {
        //        secCounter = 0;
        //        return true;
        //    }
        //
        //    break;
        //};

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
//    TI2C_Result res;
    
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

        if ( Flash_TestRead() )
        {
            if (FlashMode == FLASH_MODE_16K)
                DigMask = SEG_LETTER_F_MASK+CalcLoDigitMask(1);    
            else            
                DigMask = SEG_LETTER_F_MASK+CalcLoDigitMask(2);
        }
        else
        {
            DigMask = SEG_LETTER_F_MASK+SEG_MINUS_MASK;
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
            DigMask = SEG_LETTER_C_MASK+CalcLoDigitMask(DT.sec%10);
        }
        else
        {
            DigMask = SEG_LETTER_C_MASK+SEG_MINUS_MASK;
        }
    }

    secCounter++;

    if (secCounter >= 1000)
    {
        Test_sec++;
        if (Test_sec >= TEST_CHECK_RTC_TIME_SEC)
        {
            Test_sec = 0;
            return true;
        }

        if ( RTC_GetTime(&DT) )
        {
        //if ( !(Status.Errors|ERROR_RTC_READ) )
        //{
            //Get_DateTime(&DT);
            DigMask = SEG_LETTER_C_MASK+CalcLoDigitMask(DT.sec%10);
        }
        else
        {
            DigMask = SEG_LETTER_C_MASK+SEG_MINUS_MASK;
        }
        
        secCounter = 0;
    }
    
    return false;
}

bool TestRG_ShowLightSensVolt(bool bInit)
{
    static int Test_ShowLSV_State;
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
        GreenManVar  = 0;
        GManPhase    = 0;

        Test_ShowLSV_State = 0;
    }
    
    secCounter++;
    
    switch (Test_ShowLSV_State)
    {
        case 0:
        {
            Voltage = ADC_GetResult(ADC_LIGHT_SENS_CHANNEL);
            
            if (secCounter>=TEST_MEASURE_LSV_TIME)
            {
                Voltage = ADC_GetResult(ADC_LIGHT_SENS_CHANNEL);

                //Test_DigG = Voltage;
                Test_DigG = (LIGHT_SENS_SCALE * (uint32)Voltage) >> 16;
                DigMask   = CalcNumberMask(Test_DigG, GREEN_MAX_COUNT);

                GreenManVar  = GMAN0_MASK|GMAN1_MASK;

                secCounter = 0;

                Test_ShowLSV_State = 1;
            }

            break;
        };

        case 1:
        {
            if (secCounter>=TEST_SHOW_LSV_TIME)
            {
                //DigMask      = 0;
                
                //GreenManVar  = 0;
                //RedManVar    = RMAN_MASK;
                //GManPhase    = 0;
                
                secCounter = 0;
                //Test_ShowHV_State = 2;
                return true;
            }

            break;
        };
                        
        //case 2:
        //{
        //    if (secCounter>=TEST_RED_PAUSE_TIME)
        //    {
        //        secCounter = 0;
        //        return true;
        //    }
        //
        //    break;
        //};

        default:
        {
            Test_ShowLSV_State = 0;
            break;
        }
    }

    return false;
}



