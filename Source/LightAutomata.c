//---------------------------------------------------------------------------
//
// $Title:          LightAutomata.c
// $Description:    Light Automatas Functions and Data.
//
// $Rev:            0.0.19
//
// $Author:         EEA
//
// $Created:        17.10.11
// $Last Modified:  15.8.14
//
// $Copyright:      EEA
//
// $Notes:
//
//---------------------------------------------------------------------------
//#include <NXP/iolpc2103.h>
#include "LightAutomata.h"
#include "Pins.h"
//#include "Timers.h"
//#include "System.h"
#include "Digits.h"
#include "Config.h"
#include "Sound.h"
#include "FastLoop.h"
#include "DemoMode.h"
#include "LightSensor.h"

#define LIGHT_AUTOMATA_CALC_FREQ    (1000UL)
#define TICK_MS(N) ((N)*(FAST_LOOP_FREQ/1000))

#define OFF_DEFAULT_TIME    500
#define OFF_LIMIT_SEC       3
#define OFF_LIMIT_TIME      1000

#define BLINK_ROUND_DOWN_TIME   250

// ++EXTERNAL
//extern unsigned int sndFinished;
//extern unsigned int sndCounter;
//extern unsigned int sndIndex;

//extern unsigned char *Sounds[];
//extern unsigned int  *SoundLengths[];
//int SoundNumber;

//extern unsigned char const *pSoundArray;
//extern unsigned int sndIndex;
//extern unsigned int sndLength;

//unsigned int CalcNumberMask(int d, int Limit);
// --EXTERNAL

typedef enum
{
    OFF_STATE,
    RED_STATE,
    GREEN_STATE
} TMainState;

uint32 NumG0, NumG1;
uint32 NumR0, NumR1;

uint32 NumG_Bad, NumG_Good;
uint32 NumR_Bad, NumR_Good;

uint16 GLineState;
uint16 RLineState;

uint16 GState;
uint16 RState;

uint32 GTi, GTp;
uint32 RTi, RTp;


int32 GStablePeriods;
int32 GTime;        // msec
int32 GTime_prev;   // msec
int32 GTime_sec ;        // sec
int32 GTimeLeft ;        // sec
int16 GTime_sec_prev;    // sec
int16 GTimeJitterCount;
int16 GRoundUpValue;     // msec

int32 RStablePeriods;
int32 RTime;        // msec
int32 RTime_prev;   // msec
int32 RTime_sec ;        // sec
int32 RTimeLeft ;        // sec
int16 RTime_sec_prev;    // sec
int16 RTimeJitterCount;
int16 RRoundUpValue;     // msec

//bool bJustPowerOn = true;

//volatile int32 GTimeDelta = 0;   // msec

static int32 ManStepCounter;      // msec
//uint8 GManPhase = 0;
static int32 SoundRepeatCounter;  // msec
static int32 GmsecCounter;        // msec
static int32 RmsecCounter;        // msec
static int32 ManStepPeriod;       // msec
static int32 SoundRepeatPeriod;

TMainState MainState;
TMainState PrevState;

int32 PrevStateEnd_FullSec;

//------------------------------------------------

void GTime_AddSec(void);
void RTime_AddSec(void);
void FillPrevStateEndTime(void);
uint8 FindPeriod(uint8 *pData);
uint8 FindPeriod2(uint8 *pData);

//------------------------------------------------

void CheckInputLines(uint32 PortData)
{
    if (PortData & GREEN_IN_MASK)
    {
        NumG0++;
        NumG1 = 0;
    }
    else
    {
        NumG1++;
        NumG0 = 0;
    }

    if (PortData & RED_IN_MASK)
    {
        NumR0++;
        NumR1 = 0;
    }
    else
    {
        NumR1++;
        NumR0 = 0;
    }
}

void GLineAutomata(void)
{
    switch (GLineState)
    {
        case 0:
        {
            if (NumG1 >= G1_SWITCH_NUM)
            {
                if ( (GTp <= TICK_MS(TP_MAX)) && (GTp >= TICK_MS(TP_MIN)) &&
                     (GTi <= TICK_MS(TI_MAX)) && (GTi >= TICK_MS(TI_MIN)) &&
                     ((GTi+GTp)<=TICK_MS(T_SUM_MAX)) && ((GTi+GTp)>=TICK_MS(T_SUM_MIN))
                   )
                {
                    NumG_Good++;
                    NumG_Bad=0;
                }
                else
                {
                    NumG_Good=0;
                    NumG_Bad++;
                }

                GTi = 0;
                GLineState = 1;
            }
            else
            {
                GTp++;
                if (GTp>TICK_MS(TP_LIMIT))
                {
                    NumG_Good = 0;
                    NumG_Bad  = 0;
                }
            }

            break;
        };
        case 1:
        {
            if (NumG0 >= G0_SWITCH_NUM)
            {
                GTp = 0;
                GLineState = 0;
            }
            else
            {
                GTi++;
                if (GTi>TICK_MS(TI_LIMIT))
                {
                    NumG_Good = 0;
                    NumG_Bad  = 0;
                }
            }

            break;
        };
        default:
        {
            GTi=0;
            GTp=0;
            GLineState = 0;
        }
    }
}


void RLineAutomata(void)
{
    switch (RLineState)
    {
        case 0:
        {
            if (NumR1 >= R1_SWITCH_NUM)
            {
                if ( (RTp <= TICK_MS(TP_MAX)) && (RTp >= TICK_MS(TP_MIN)) &&
                     (RTi <= TICK_MS(TI_MAX)) && (RTi >= TICK_MS(TI_MIN)) &&
                     ((RTi+RTp)<=TICK_MS(T_SUM_MAX)) && ((RTi+RTp)>=TICK_MS(T_SUM_MIN))
                   )
                {
                    NumR_Good++;
                    NumR_Bad=0;
                }
                else
                {
                    NumR_Good=0;
                    NumR_Bad++;
                }

                RTi = 0;
                RLineState = 1;
            }
            else
            {
                RTp++;
                if (RTp>TICK_MS(TP_LIMIT))
                {
                    NumR_Good = 0;
                    NumR_Bad  = 0;
                }
            }

            break;
        };
        case 1:
        {
            if (NumR0 >= R0_SWITCH_NUM)
            {
                RTp = 0;
                RLineState = 0;
            }
            else
            {
                RTi++;
                if (RTi>TICK_MS(TI_LIMIT))
                {
                    NumR_Good = 0;
                    NumR_Bad  = 0;
                }
            }

            break;
        };
        default:
        {
            RTi=0;
            RTp=0;
            RLineState = 0;
            break;
        }
    }
}

void GreenAutomata(void)
{
    switch (GState)
    {
        case 0:
        {
            if (NumG_Good >= GOOD_G_PULSES_MAX)
            {
                GState = 1;
            }

            break;
        };
        case 1:
        {
            if ( (NumG_Bad >= BAD_G_PULSES_MAX) || (GTi>TICK_MS(TI_LIMIT)) || (GTp>TICK_MS(TP_LIMIT)) )
            {
                GState = 0;
            }

            break;
        };
        default:
        {
            GState = 0;
        };
    }
}


void RedAutomata(void)
{
    switch (RState)
    {
        case 0:
        {
            if (NumR_Good >= GOOD_R_PULSES_MAX)
            {
                RState = 1;
            }

            break;
        };
        case 1:
        {
            if ( (NumR_Bad >= BAD_R_PULSES_MAX) || (RTi>TICK_MS(TI_LIMIT)) || (RTp>TICK_MS(TP_LIMIT)) )
            {
                RState = 0;
            }

            break;
        };
        default:
        {
            RState = 0;
        };
    }
}

void WorkAutomata(void)
{
    switch (MainState)
    {
        case OFF_STATE:
        {
            if (RState)
            {
                SwitchToRed();
            }
            else if (GState)
            {
                SwitchToGreen();
            }
            break;
        };
        case RED_STATE:
        {
            RTime++;        // msec - Current Red Time

            RmsecCounter++;

            if (RmsecCounter>=LIGHT_AUTOMATA_CALC_FREQ)
            {
                RTime_AddSec();
                RmsecCounter = 0;
            }

            if (!RState)
            {
                FillPrevStateEndTime();

                if (GState)
                {
                    SwitchToGreen();
                }
                else
                {
                    SwitchToOff();
                }
            }

            break;
        };

        case GREEN_STATE:
        {
            GTime++;        // msec - Current Green Time

            GmsecCounter++;

            if (GmsecCounter>=LIGHT_AUTOMATA_CALC_FREQ)
            {
                GTime_AddSec();
                GmsecCounter = 0;
            }

            if ( (GTimeLeft>=0) && (GTimeLeft<=Config.SpeedUpTime) && Config.bSpeedUp)
            {
                ManStepPeriod     = MAN_STEP_PERIOD/2;
                SoundRepeatPeriod = SOUND_REPEAT_PERIOD/3;
            }
            else
            {
                ManStepPeriod     = MAN_STEP_PERIOD;
                SoundRepeatPeriod = SOUND_REPEAT_PERIOD;
            }

            ManStepCounter++;
            if (ManStepCounter >= ManStepPeriod)
            {
                GreenManVar ^= GMAN1_MASK|GMAN2_MASK;
                GManPhase = !GManPhase;
                ManStepCounter = 0;
            }

            if ( (GTimeLeft>GREEN_MAX_COUNT) && GManPhase )
            {
                DigMask = 0;
            }

            SoundRepeatCounter++;
            if (SoundRepeatCounter >= SoundRepeatPeriod)
            {
                StartSound();
                SoundRepeatCounter = 0;
            }

            if (RState || !GState)
            {
                FillPrevStateEndTime();

                if (RState)
                    SwitchToRed();
                else
                    SwitchToOff();
            }
            break;
        };
    }
}

void SwitchToGreen(void)
{
    TDateTime DT;
    int32 NowFullSec;
    int32 CurLearn_sec;
    int   i;

    if ( Get_DateTime(&DT) )
    {
        NowFullSec = ((DT.hour*60UL) + DT.min)*60UL + DT.sec;

        if (NowFullSec < PrevStateEnd_FullSec)
        {
            NowFullSec += 24*60*60UL; // + 1 day
        }
    }
    else
    {
        NowFullSec = -1;
    }

    if ( ((NowFullSec-PrevStateEnd_FullSec) >= OFF_LIMIT_SEC) || (PrevState != GREEN_STATE) )
    {
        // New Light
        //if ( ((GTime-GTime_prev) <= TIME_MAX_DELTA) && ((GTime-GTime_prev) >= -TIME_MAX_DELTA) )
        //{
        //    GStablePeriods++;
        //}
        //else
        //{
        //    GStablePeriods = 0;
        //}

        //GRoundUpValue = ROUND_UP_TIME1;
        // Rounding Algorithm
        if ( GmsecCounter >= (1000-GRoundUpValue) )
        {
            GTime_sec++;
        }

        //if (GmsecCounter <= ROUND_DOWN_TIME)
        //{
        //    GTime -= GmsecCounter;
        //}

        //--------------------
        // Smart1 Learn Algorithm
        /*
        // Put Data To Buffer
        for (i=1; i<LEARN_BUF_SIZE; i++)
            LearnData.GLearnBuf[i-1] = LearnData.GLearnBuf[i];

        LearnData.GLearnBuf[LEARN_BUF_SIZE-1] = GTime_sec;

        if (LearnData.GLearnState == LEARNING_NOW)
        {
            LearnData.GLearnPeriod = FindPeriod2(&LearnData.GLearnBuf[0]);
            if (LearnData.GLearnPeriod)
            {
                LearnData.GLearnState = LEARN_FINISHED;
                LearnData.GLearnIndex = LEARN_BUF_SIZE - LearnData.GLearnPeriod;
            }
        }
        else
        {
            CurLearn_sec = LearnData.GLearnBuf[LearnData.GLearnIndex-1];

            if ( ((GTime_sec-CurLearn_sec) <= (TIME_MAX_DELTA/1000)) && ((GTime_sec-CurLearn_sec) >= -(TIME_MAX_DELTA/1000)) )
            {
                GStablePeriods++;
            }
            else
            {
                LearnData.GLearnPeriod = FindPeriod2(&LearnData.GLearnBuf[0]);
                if (LearnData.GLearnPeriod)
                {
                    LearnData.GLearnState = LEARN_FINISHED;
                    LearnData.GLearnIndex = LEARN_BUF_SIZE - LearnData.GLearnPeriod;
                }
                else
                {
                    GStablePeriods = 0;
                    LearnData.GLearnState = LEARNING_NOW;
                }
            }
        }
        */

        //--------------------
        // Smart2 Learn Algorithm
        // Put Data To Buffer
        for (i=1; i<LEARN_BUF_SIZE; i++)
            LearnData.GLearnBuf[i-1] = LearnData.GLearnBuf[i];

        LearnData.GLearnBuf[LEARN_BUF_SIZE-1] = GTime_sec;

        // Find Period
        LearnData.GLearnPeriod = FindPeriod2(&LearnData.GLearnBuf[0]);
        if (LearnData.GLearnPeriod)
        {
            LearnData.GLearnState = LEARN_FINISHED;
            LearnData.GLearnIndex = LEARN_BUF_SIZE - LearnData.GLearnPeriod;
        }
        else
        {
            LearnData.GLearnState = LEARNING_NOW;
        }

        //--------------------
        // Vulgar Learn Algorithm
        /*
        if (LearnData.GLearnState == LEARNING_NOW)
        {
            LearnData.GLearnBuf[LearnData.GLearnIndex++] = GTime_sec;
            if (LearnData.GLearnIndex >= LEARN_BUF_SIZE)
            {
                LearnData.GLearnPeriod = FindPeriod(&LearnData.GLearnBuf[0]);
                if (LearnData.GLearnPeriod)
                {
                    LearnData.GLearnState = LEARN_FINISHED;

                    for (i=0; i<LearnData.GLearnPeriod; i++)
                    {
                        LearnData.GLearnBuf[LearnData.GLearnPeriod-1-i] = LearnData.GLearnBuf[LEARN_BUF_SIZE-1-i];
                        LearnData.GLearnIndex = 0;
                    }
                }
                else
                {
                    for (i=1; i<LEARN_BUF_SIZE; i++)
                    {
                        LearnData.GLearnBuf[i-1] = LearnData.GLearnBuf[i];
                        LearnData.GLearnIndex = LEARN_BUF_SIZE-1;
                    }
                }
            }
        }
        else
        {
            CurLearn_sec = LearnData.GLearnBuf[LearnData.GLearnIndex];

            if ( ((GTime_sec-CurLearn_sec) <= (TIME_MAX_DELTA/1000)) && ((GTime_sec-CurLearn_sec) >= -(TIME_MAX_DELTA/1000)) )
            {
                GStablePeriods++;
                LearnData.GLearnBuf[LearnData.GLearnIndex++] = GTime_sec;
                //LearnData.GLearnIndex++;
                if (LearnData.GLearnIndex >= LearnData.GLearnPeriod)
                    LearnData.GLearnIndex = 0;
            }
            else
            {
                GStablePeriods = 0;
                LearnData.GLearnState = LEARNING_NOW;
                LearnData.GLearnIndex  = 0;
                LearnData.GLearnPeriod = 0;
                LearnData.GLearnBuf[LearnData.GLearnIndex++] = GTime_sec;
            }
        }
        */

        // Jitter Count
        GRoundUpValue = ROUND_UP_TIME1;
        if ( (GTime_sec == (GTime_sec_prev+TIME_JITTER_DELTA_SEC)) || (GTime_sec == (GTime_sec_prev-TIME_JITTER_DELTA_SEC)) )
        {
            GTimeJitterCount += TIME_JITTER_ADD_VALUE;

            if (GTimeJitterCount >= TIME_JITTER_COUNTER_LIMIT)
            {
                GTimeJitterCount = 0;
                if (GRoundUpValue != ROUND_UP_TIME1)
                    GRoundUpValue = ROUND_UP_TIME1;
                else
                    GRoundUpValue = ROUND_UP_TIME2;
            }
        }
        else
        {
            if (GTimeJitterCount >= TIME_JITTER_SUB_VALUE)
            {
                GTimeJitterCount -= TIME_JITTER_SUB_VALUE;
            }
            else
            {
                GTimeJitterCount = 0;
            }
        }

        GTime_sec_prev  = GTime_sec;
        GTime_prev = GTime;

        if (LearnData.GLearnState != LEARNING_NOW)
        //if (GStablePeriods >= Config.TeachingPeriods)
        {
            //GTimeLeft = GTime_sec;
            GTimeLeft = LearnData.GLearnBuf[LearnData.GLearnIndex];
            DigMask   = CalcNumberMask(GTimeLeft, GREEN_MAX_COUNT);
        }
        else
        {
            GTimeLeft = -1;
            DigMask = 0;
        }

        // Init Counters
        GTime = 0;
        GTime_sec = 0;
        GmsecCounter = 0;
    }
    else
    {
        // Old Light

        // Algorithm 1
        //GTime += 1000 - GmsecCounter;
        //GTime_AddSec();
        //GmsecCounter = 0;

        // Algorithm 2
        //GTime      += 500;
        //GmsecCounter += 500;
        //if (GmsecCounter >= 1000)
        //{
        //    GTime_AddSec();
        //    GmsecCounter = 0;
        //}

        // Algorithm 3
        if (GmsecCounter < BLINK_ROUND_DOWN_TIME)
        {
            GTime -= GmsecCounter;
            GmsecCounter = 0;

            if (GTimeLeft>=0)
                DigMask = CalcNumberMask(GTimeLeft, GREEN_MAX_COUNT);
            else
                DigMask = 0;
        }
        else
        {
            GTime += 1000 - GmsecCounter;
            GTime_AddSec();
            GmsecCounter = 0;
        }
    }

    ManStepCounter = 0;
    SoundRepeatCounter = 0;

    // Red Off
    RedManVar = 0;

    // Green On
    GreenManVar = GMAN0_MASK|GMAN1_MASK;
    GManPhase = 0;

    //Start Sound
    StartSound();

    MainState = GREEN_STATE;
    PrevState = GREEN_STATE;
}

void SwitchToRed(void)
{
    TDateTime DT;
    int32 NowFullSec;
    int32 CurLearn_sec;
    int   i;

    StopSound();

    DigMask = 0;

    if ( Get_DateTime(&DT) )
    {
        NowFullSec = ((DT.hour*60UL) + DT.min)*60UL + DT.sec;

        if (NowFullSec < PrevStateEnd_FullSec)
        {
            NowFullSec += 24*60*60UL; // + 1 day
        }
    }
    else
    {
        NowFullSec = -1;
    }

    if ( ((NowFullSec-PrevStateEnd_FullSec) >= OFF_LIMIT_SEC) || (PrevState != RED_STATE) )
    {
        // New Light
        //if ( ((RTime-RTime_prev) <= TIME_MAX_DELTA) && ((RTime-RTime_prev) >= -TIME_MAX_DELTA) )
        //{
        //    RStablePeriods++;
        //}
        //else
        //{
        //    RStablePeriods = 0;
        //}

        // Rounding Algorithm
        if ( RmsecCounter >= (1000-RRoundUpValue) )
        {
            RTime_sec++;
        }

        //if (RmsecCounter <= ROUND_DOWN_TIME)
        //{
        //    RTime -= RmsecCounter;
        //}

        //--------------------
        // Smart1 Learn Algorithm
        /*
        // Put Data To Buffer
        for (i=1; i<LEARN_BUF_SIZE; i++)
            LearnData.RLearnBuf[i-1] = LearnData.RLearnBuf[i];

        LearnData.RLearnBuf[LEARN_BUF_SIZE-1] = RTime_sec;

        if (LearnData.RLearnState == LEARNING_NOW)
        {
            LearnData.RLearnPeriod = FindPeriod2(&LearnData.RLearnBuf[0]);
            if (LearnData.RLearnPeriod)
            {
                LearnData.RLearnState = LEARN_FINISHED;
                LearnData.RLearnIndex = LEARN_BUF_SIZE - LearnData.RLearnPeriod;
            }
        }
        else
        {
            CurLearn_sec = LearnData.RLearnBuf[LearnData.RLearnIndex-1];

            if ( ((RTime_sec-CurLearn_sec) <= (TIME_MAX_DELTA/1000)) && ((RTime_sec-CurLearn_sec) >= -(TIME_MAX_DELTA/1000)) )
            {
                RStablePeriods++;
            }
            else
            {
                LearnData.RLearnPeriod = FindPeriod2(&LearnData.RLearnBuf[0]);
                if (LearnData.RLearnPeriod)
                {
                    LearnData.RLearnState = LEARN_FINISHED;
                    LearnData.RLearnIndex = LEARN_BUF_SIZE - LearnData.RLearnPeriod;
                }
                else
                {
                    RStablePeriods = 0;
                    LearnData.RLearnState = LEARNING_NOW;
                }
            }
        }
        */

        //--------------------
        // Smart2 Learn Algorithm
        // Put Data To Buffer
        for (i=1; i<LEARN_BUF_SIZE; i++)
            LearnData.RLearnBuf[i-1] = LearnData.RLearnBuf[i];

        LearnData.RLearnBuf[LEARN_BUF_SIZE-1] = RTime_sec;

        // Find Period
        LearnData.RLearnPeriod = FindPeriod2(&LearnData.RLearnBuf[0]);
        if (LearnData.RLearnPeriod)
        {
            LearnData.RLearnState = LEARN_FINISHED;
            LearnData.RLearnIndex = LEARN_BUF_SIZE - LearnData.RLearnPeriod;
        }
        else
        {
            LearnData.RLearnState = LEARNING_NOW;
        }

        //--------------------
        // Vulgar Learn Algorithm
        /*
        if (LearnData.RLearnState == LEARNING_NOW)
        {
            LearnData.RLearnBuf[LearnData.RLearnIndex++] = RTime_sec;
            if (LearnData.RLearnIndex >= LEARN_BUF_SIZE)
            {
                LearnData.RLearnPeriod = FindPeriod(&LearnData.RLearnBuf[0]);
                if (LearnData.RLearnPeriod)
                {
                    LearnData.RLearnState = LEARN_FINISHED;

                    for (i=0; i<LearnData.RLearnPeriod; i++)
                    {
                        LearnData.RLearnBuf[LearnData.RLearnPeriod-1-i] = LearnData.RLearnBuf[LEARN_BUF_SIZE-1-i];
                        LearnData.RLearnIndex = 0;
                    }
                }
                else
                {
                    for (i=1; i<LEARN_BUF_SIZE; i++)
                    {
                        LearnData.RLearnBuf[i-1] = LearnData.RLearnBuf[i];
                        LearnData.RLearnIndex = LEARN_BUF_SIZE-1;
                    }
                }
            }
        }
        else
        {
            CurLearn_sec = LearnData.RLearnBuf[LearnData.RLearnIndex];

            if ( ((RTime_sec-CurLearn_sec) <= (TIME_MAX_DELTA/1000)) && ((RTime_sec-CurLearn_sec) >= -(TIME_MAX_DELTA/1000)) )
            {
                RStablePeriods++;
                LearnData.RLearnBuf[LearnData.RLearnIndex++] = RTime_sec;
                //LearnData.RLearnIndex++;
                if (LearnData.RLearnIndex >= LearnData.RLearnPeriod)
                    LearnData.RLearnIndex = 0;
            }
            else
            {
                RStablePeriods = 0;
                LearnData.RLearnState = LEARNING_NOW;
                LearnData.RLearnIndex  = 0;
                LearnData.RLearnPeriod = 0;
                LearnData.RLearnBuf[LearnData.RLearnIndex++] = RTime_sec;
            }
        }
        */

        // Jitter Count
        RRoundUpValue = ROUND_UP_TIME1;
        if ( (RTime_sec == (RTime_sec_prev+TIME_JITTER_DELTA_SEC)) || (RTime_sec == (RTime_sec_prev-TIME_JITTER_DELTA_SEC)) )
        {
            RTimeJitterCount += TIME_JITTER_ADD_VALUE;

            if (RTimeJitterCount >= TIME_JITTER_COUNTER_LIMIT)
            {
                RTimeJitterCount = 0;
                if (RRoundUpValue != ROUND_UP_TIME1)
                    RRoundUpValue = ROUND_UP_TIME1;
                else
                    RRoundUpValue = ROUND_UP_TIME2;
            }
        }
        else
        {
            if (RTimeJitterCount >= TIME_JITTER_SUB_VALUE)
            {
                RTimeJitterCount -= TIME_JITTER_SUB_VALUE;
            }
            else
            {
                RTimeJitterCount = 0;
            }
        }

        RTime_sec_prev  = RTime_sec;
        RTime_prev = RTime;

        if (LearnData.RLearnState != LEARNING_NOW)
        //if (RStablePeriods >= Config.TeachingPeriods)
        {
            //RTimeLeft = RTime_sec;
            RTimeLeft = LearnData.RLearnBuf[LearnData.RLearnIndex];
    //        DigMask = CalcNumberMask(RTimeLeft, RED_MAX_COUNT);
        }
        else
        {
            RTimeLeft = -1;
            DigMask = 0;
        }

        // Init Counters
        RTime = 0;
        RTime_sec = 0;
        RmsecCounter = 0;
    }
    else
    {
        // Old Light

        // Algorithm 1
        //RTime += 1000 - RmsecCounter;
        //RTime_AddSec();
        //RmsecCounter = 0;

        // Algorithm 2
        //RTime      += 500;
        //RmsecCounter += 500;
        //if (RmsecCounter >= 1000)
        //{
        //    RTime_AddSec();
        //    RmsecCounter = 0;
        //}

        // Algorithm 3
        if (RmsecCounter < BLINK_ROUND_DOWN_TIME)
        {
            RTime -= RmsecCounter;
            RmsecCounter = 0;

            if (RTimeLeft>=0)
                DigMask = CalcNumberMask(RTimeLeft, RED_MAX_COUNT);
            else
                DigMask = 0;
        }
        else
        {
            RTime += 1000 - RmsecCounter;
            RTime_AddSec();
            RmsecCounter = 0;
        }
    }

    // Red On
    RedManVar   = RMAN_MASK;
    // All Green Off
    GreenManVar = 0;
    //DigMask         = 0;

    MainState = RED_STATE;
    PrevState = RED_STATE;
}

void SwitchToOff(void)
{
    StopSound();

    // Red Off
    RedManVar = 0;
    //FIOSET = SRedMan;

    // All Green Off
    GreenManVar = 0;
    DigMask     = 0;
    //FIOCLR = SGreenMan2|SGreenMan1|SGreenMan|SH|SL;

    MainState = OFF_STATE;
}

void UpdateAutomataStatus(void)
{
    Status.RedState     = RState;
    Status.YellowState  = 0;
    Status.GreenState   = GState;
    Status.RCount       = RTimeLeft;
    Status.GCount       = GTimeLeft;
    Status.MainState    = (uint8)MainState;
}

void InitAutomatas(void)
{
    int i;

    NumG0=0; NumG1=0;
    NumR0=0; NumR1=0;

    NumG_Bad=0; NumG_Good=0;
    NumR_Bad=0; NumR_Good=0;

    GLineState = 0;
    RLineState = 0;

    GState = 0;
    RState = 0;

    GTi=0; GTp=0;
    RTi=0; RTp=0;

    GTime     = 0;        // msec
    GTime_sec = 0;        // sec

    GStablePeriods  = 0;
    GTime_prev      = 0;  // msec
    GTimeLeft       = 0;  // sec

    GTime_sec_prev      = 0;
    GTimeJitterCount    = 0;
    GRoundUpValue       = DEF_ROUND_UP_TIME;

    RTime     = 0;        // msec
    RTime_sec = 0;        // sec

    RStablePeriods  = 0;
    RTime_prev      = 0;  // msec
    RTimeLeft       = 0;  // sec

    RTime_sec_prev      = 0;
    RTimeJitterCount    = 0;
    RRoundUpValue       = DEF_ROUND_UP_TIME;

    //GManPhase = 0;

    ManStepCounter      = -1;   // msec
    SoundRepeatCounter  = -1;   // msec
    GmsecCounter        = -1;   // msec
    RmsecCounter        = -1;   // msec

    ManStepPeriod       = MAN_STEP_PERIOD;

    MainState = OFF_STATE;
    PrevState = OFF_STATE;

    PrevStateEnd_FullSec = -1;

    RedManVar   = 0;
    GreenManVar = 0;
    DigMask     = 0;

//    bJustPowerOn = true;

    LearnData.GLearnState    = LEARNING_NOW;
    LearnData.GLearnBufLen   = 0;
    LearnData.GLearnIndex    = 0;
    LearnData.GLearnPeriod   = 0;

    LearnData.RLearnState    = LEARNING_NOW;
    LearnData.RLearnBufLen   = 0;
    LearnData.RLearnIndex    = 0;
    LearnData.RLearnPeriod   = 0;

    for (i=0; i<LEARN_BUF_SIZE; i++)
    {
        LearnData.GLearnBuf[i] = 0;
        LearnData.RLearnBuf[i] = 0;
    }
}

void ActivateState(TState *pState)
{
    GStablePeriods      = pState->GStablePeriods;
    GTime               = pState->GTime     ;       // msec
    GTime_prev          = pState->GTime_prev;       // msec
    GTime_sec           = pState->GTime_sec ;       // sec
    GTimeLeft           = pState->GTimeLeft ;       // sec
    GTime_sec_prev      = pState->GTime_sec_prev  ; // sec
    GTimeJitterCount    = pState->GTimeJitterCount;
    GRoundUpValue       = pState->GRoundUpValue;

    RStablePeriods      = pState->RStablePeriods;
    RTime               = pState->RTime     ;       // msec
    RTime_prev          = pState->RTime_prev;       // msec
    RTime_sec           = pState->RTime_sec ;       // sec
    RTimeLeft           = pState->RTimeLeft ;       // sec
    RTime_sec_prev      = pState->RTime_sec_prev  ; // sec
    RTimeJitterCount    = pState->RTimeJitterCount;
    RRoundUpValue       = pState->RRoundUpValue;

    GmsecCounter    = pState->GmsecCounter;
    RmsecCounter    = pState->RmsecCounter;

    PrevStateEnd_FullSec = pState->PrevStateEnd_FullSec;

    PrevState  = pState->PrevState;

    SensorInfo = pState->SensorInfo;

    LearnData  = pState->LearnData;
}

void FillState(TState *pState)
{
    pState->GStablePeriods      = GStablePeriods;
    pState->GTime               = GTime;
    pState->GTime_prev          = GTime_prev;
    pState->GTime_sec           = GTime_sec;
    pState->GTimeLeft           = GTimeLeft;
    pState->GTime_sec_prev      = GTime_sec_prev;
    pState->GTimeJitterCount    = GTimeJitterCount;
    pState->GRoundUpValue       = GRoundUpValue;

    pState->RStablePeriods      = RStablePeriods;
    pState->RTime               = RTime;
    pState->RTime_prev          = RTime_prev;
    pState->RTime_sec           = RTime_sec;
    pState->RTimeLeft           = RTimeLeft;
    pState->RTime_sec_prev      = RTime_sec_prev;
    pState->RTimeJitterCount    = RTimeJitterCount;
    pState->RRoundUpValue       = RRoundUpValue;

    pState->GmsecCounter    = GmsecCounter;
    pState->RmsecCounter    = RmsecCounter;
    pState->PrevStateEnd_FullSec = PrevStateEnd_FullSec;

    pState->PrevState       = PrevState;

    pState->SensorInfo      = SensorInfo;

    pState->LearnData = LearnData;
}

void MakeSlaveCommand(void)
{
    switch (MainState)
    {
        case RED_STATE:
        {
            SlaveCommand.GreenState = 0;
            SlaveCommand.RedCount   = (uint8)RTimeLeft;
            SlaveCommand.Bright     = CurBrightR;

// DEBUG+
            if ( ((RTimeLeft>RED_MAX_COUNT) && (RmsecCounter>=LIGHT_AUTOMATA_CALC_FREQ/2)) || (Config2.bDisableRedCount) )
            {
                SlaveCommand.RedCount   = (uint8)-1;
            }
// DEBUG-

            break;
        };

        case GREEN_STATE:
        {
            SlaveCommand.GreenState = GManPhase+1;
            SlaveCommand.RedCount   = (uint8)-1;
            SlaveCommand.Bright     = CurBrightG;
            break;
        };

        default:
        case OFF_STATE:
        {
            SlaveCommand.GreenState =  0;
            SlaveCommand.RedCount   = (uint8)-1;
            SlaveCommand.Bright     =  0;
            break;
        };
    };

    SlaveCommand.Modulo = PWM_MODULO;

// DEBUG MODE +
//    SlaveCommand.GreenState = GRoundUpValue/10;
//    SlaveCommand.RedCount   = GTimeJitterCount;
//    SlaveCommand.Bright     = RRoundUpValue/10;
//    SlaveCommand.Modulo     = RTimeJitterCount;
// DEBUG MODE -
}

void GTime_AddSec(void)
{
    GTime_sec++;  // Full Seconds of Current Green

    if (GTimeLeft>0)          // Zero Enabled
    //if (GTimeLeft>1)            // Zero Disabled
    {
        GTimeLeft--;
        //DigMask = CalcNumberMask(GTimeJitterCount, GREEN_MAX_COUNT);
        //DigMask = CalcNumberMask(GTimeJitterCount, GREEN_MAX_COUNT);
        DigMask = CalcNumberMask(GTimeLeft, GREEN_MAX_COUNT);
    }
    else    // Zero Enabled
    //else if (GTimeLeft==0)    // Zero Enabled
    //else                        // Zero Disabled
    {
        DigMask = 0;
    }
}

void RTime_AddSec(void)
{
    RTime_sec++;  // Full Seconds of Current Red

    if (RTimeLeft>0)     // Zero Enabled
    //if (RTimeLeft>0)        // Zero Disabled
    {
        RTimeLeft--;
        //DigMask = CalcNumberMask(RTimeLeft, RED_MAX_COUNT);
    }
    else
    {
        RTimeLeft = -1;
    }

    //else if (RTimeLeft==0)
    //{
    //    DigMask = 0;
    //}
}

void FillPrevStateEndTime(void)
{
    if ( Get_DateTime(&Status.CurDateTime) )
    {
        PrevStateEnd_FullSec = ((Status.CurDateTime.hour*60UL) + Status.CurDateTime.min)*60UL + Status.CurDateTime.sec;
    }
    else
    {
        PrevStateEnd_FullSec = -1;
    }
}

/*
void DEBUG_FillState(void)
{
    GStablePeriods      = 3;
    GTime               = 99999;
    GTime_prev          = 99999;
    GTime_sec           = 99;
    GTimeLeft           = 50;
    GTime_sec_prev      = 99;
    GTimeJitterCount    = 0;
    GRoundUpValue       = 750;

    RStablePeriods      = 3;
    RTime               = 99999;
    RTime_prev          = 99999;
    RTime_sec           = 99;
    RTimeLeft           = 50;
    RTime_sec_prev      = 99;
    RTimeJitterCount    = 0;
    RRoundUpValue       = 750;

    GmsecCounter        = 45000;
    RmsecCounter        = 45000;

    PrevStateEnd_FullSec = 1234567;
    PrevState           = 0;
}
*/

// For Vulgar Algorithm
uint8 FindPeriod(uint8 *pData)
{
    int i;
    uint8 Period;
    bool  bRes;
    int32 Delta;

    Period = LEARN_BUF_SIZE/2;

    while (Period)
    {
        bRes = true;
        for (i=Period; i<LEARN_BUF_SIZE; i++)                      // Vulgar Algorithm
        {
            Delta = (int32)pData[i] - (int32)pData[i-Period];

            if ( !((Delta <= (TIME_MAX_DELTA/1000)) && (Delta >= -(TIME_MAX_DELTA/1000))) )
            {
               bRes = false;
               break;
            }
        }

        if (bRes)
            break;

        Period--;
    }

    return Period;
}

// For Smart Algorithm
uint8 FindPeriod2(uint8 *pData)
{
    int i;
    uint8 Period;
    bool  bRes;
    int32 Delta;

    Period = LEARN_BUF_SIZE/2;

    while (Period)
    {
        bRes = true;
        for (i=LEARN_BUF_SIZE-1; i>LEARN_BUF_SIZE-1-Period; i--)   // Smart Algorithm
        {
            Delta = (int32)pData[i] - (int32)pData[i-Period];

            if ( !((Delta <= (TIME_MAX_DELTA/1000)) && (Delta >= -(TIME_MAX_DELTA/1000))) )
            {
               bRes = false;
               break;
            }
        }

        if (bRes)
            break;

        Period--;
    }

    return Period;
}
