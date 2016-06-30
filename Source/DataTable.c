//---------------------------------------------------------------------------
//
// $Title:          DataTable.c
// $Description:    Configuration and Status Data Definition.
//
// $Rev:            0.0.12
//
// $Author:         EEA
//
// $Created:        17.10.11
// $Last Modified:  5.12.13
//
// $Copyright:      EEA
//
// $Notes:
//
//---------------------------------------------------------------------------
#include "DataTable.h"

/*
typedef struct
{
    uint8 SampleNum;        // 0-3
    uint8 TeachingPeriods;  // 1-2
    uint8 bSpeedUp;
    uint8 SpeedUpTime;      // sec
} TConfig;

typedef struct
{
    TDateTime CurDateTime;

    int8  HolidayIndex;
    uint8 CurPlan;
    uint8 CurItem;
    uint8 CurBright;
    uint8 CurVolume;
    uint8 CurMode;
    
    uint8 RedState;
    uint8 YellowState;
    uint8 GreenState;
    int8  RCount;
    int8  GCount;
    uint8 MainState;

    uint8 Errors;
} TStatus;

typedef struct
{
    int8 hour;
    int8 min;
    int8 Bright;
    int8 Volume;
} TDayPlanItem;

typedef struct
{
    TDayPlanItem Items[MAX_DAY_PLAN_ITEMS];
    uint8 ActiveItems;
} TDayPlan;

typedef struct
{
    int8 day;
    int8 month;
    int8 plan;
    int8 Reserved;
} THoliday;
*/

const TSoftwareInfo SoftwareInfo = 
{
    MAX_DAY_PLANS     , //uint8 MaxDayPlans;
    MAX_DAY_PLAN_ITEMS, //uint8 MaxPlanItems;
    MAX_HOLIDAYS      , //uint8 MaxHolidays;
    MAX_WEEK_PLANS    , //uint8 MaxWeekPlans;
    MAX_SOUND_NUMBER  , //uint8 MaxSounds;
    MAX_SPEEDUP_TIME  , //uint8 MaxSpeedUpTime;

    SOFTWARE_VERSION_H, //uint8 VersionH;
    SOFTWARE_VERSION_L, //uint8 VersionL;

    PROTOCOL_VERSION_H, //uint8 ProtocolH;
    PROTOCOL_VERSION_L, //uint8 ProtocolL;

    (1<<COLOR_RED),     //uint8 LampColors;
    RED_MAX_COUNT,      //int8 RedMaxCount;
    -1,                 //int8 YellowMaxCount;
    GREEN_MAX_COUNT,    //int8 GreenMaxCount;

    DEVICE_TYPE         //uint16 DeviceType;
};


bool bNewSec;

TConfig Config;
TConfig2 Config2;

TStatus Status;
//TDateTime NewDateTime;

TLearnData LearnData;

TSlaveCommand SlaveCommand;

int8 WeekPlans[MAX_WEEK_PLANS];
THoliday Holidays[MAX_HOLIDAYS];
TDayPlan DayPlans[MAX_DAY_PLANS];
uint8 ActiveHolidays;

uint16 CurBrightR;  // real Val
uint16 CurBrightG;  // real Val
uint16 CurVolume;   // real Val

void InitData(void)
{
    int i;
    int j;

    // Init Config
    Config.SampleNum        = 0;        // 0-3
    Config.TeachingPeriods  = TEACHING_PERIODS;        // 1-2
    Config.bSpeedUp         = true;     // bool          
    Config.SpeedUpTime      = MAN_SPEEDUP_TIME;        // sec
    
    Config2.bDisableRedCount    = 0;
    Config2.bDisableGreenCount  = 0;
    
    Config2.bGOST               = 1;
    Config2.MinDayLight         = 50;    // %
    Config2.MaxNightLight       = 10;    // %
    Config2.MinSensorBrightRG   = 50;    // %
    Config2.MaxSensorBrightRG   = 100;   // %
    Config2.MinSensorBrightY    = 50;    // %
    Config2.MaxSensorBrightY    = 100;   // %
    Config2.SensorFilterTime    = MIN_SENSOR_FILTER_TIME;     // min
    
    //Config2.bGOST               = 0;
    //Config2.MinDayLight         = 80;    // %
    //Config2.MaxNightLight       = 40;    // %
    //Config2.MinSensorBright     = 2;    // %
    //Config2.MaxSensorBright     = 100;   // %
    //Config2.SensorFilterTime    = MIN_SENSOR_FILTER_TIME;     // min

    // Clear All Day Plans
    for (i=0; i<MAX_DAY_PLANS; i++)
    {
        DayPlans[i].ActiveItems = 0;
    
        for (j=0; j<MAX_DAY_PLAN_ITEMS; j++)
        {                               
            DayPlans[i].Items[j].hour     = -1;
            DayPlans[i].Items[j].min      = -1;
            DayPlans[i].Items[j].Bright   = (uint8)-1;
            DayPlans[i].Items[j].Volume   = (uint8)-1;
            DayPlans[i].Items[j].bSensor  = 0;
        }
    }

    // Set Default Day Plans
    i=0;

    DayPlans[i].ActiveItems     = 4;
    j=0;
//    //DayPlans[i].Items[j].hour   = 6;
//    DayPlans[i].Items[j].hour   = 0;
//    DayPlans[i].Items[j].min    = 0;
//    DayPlans[i].Items[j].Bright = DEFAULT_BRIGHT_PRC;
//    DayPlans[i].Items[j].Volume = DEFAULT_VOLUME_PRC;
//    j++;
    DayPlans[i].Items[j].hour     = 6;
    DayPlans[i].Items[j].min      = 0;
    DayPlans[i].Items[j].Bright   = 30;
    DayPlans[i].Items[j].Volume   = 0;
    DayPlans[i].Items[j].bSensor  = 0;
    j++;
    DayPlans[i].Items[j].hour     = 7;
    DayPlans[i].Items[j].min      = 0;
    DayPlans[i].Items[j].Bright   = 30;
    DayPlans[i].Items[j].Volume   = 10;
    DayPlans[i].Items[j].bSensor  = 0;
    j++;
    DayPlans[i].Items[j].hour     = 20;
    DayPlans[i].Items[j].min      = 0;
    DayPlans[i].Items[j].Bright   = 30;
    DayPlans[i].Items[j].Volume   = 0;
    DayPlans[i].Items[j].bSensor  = 0;
    j++;
    DayPlans[i].Items[j].hour     = 22;
    DayPlans[i].Items[j].min      = 0;
    DayPlans[i].Items[j].Bright   = 15;
    DayPlans[i].Items[j].Volume   = 0;
    DayPlans[i].Items[j].bSensor  = 0;
    j++;

    //i++;
    //DayPlans[i].ActiveItems     = 2;
    //j=0;
    //DayPlans[i].Items[j].hour   = 6;
    //DayPlans[i].Items[j].min    = 0;
    //DayPlans[i].Items[j].Bright = 100;
    //DayPlans[i].Items[j].Volume = 100;
    //j++;
    //DayPlans[i].Items[j].hour   = 22;
    //DayPlans[i].Items[j].min    = 0;
    //DayPlans[i].Items[j].Bright = 30;
    //DayPlans[i].Items[j].Volume = 30;
    //j++;

    //i++;
    //DayPlans[i].ActiveItems     = 2;
    //j=0;
    //DayPlans[i].Items[j].hour   = 6;
    //DayPlans[i].Items[j].min    = 0;
    //DayPlans[i].Items[j].Bright = 100;
    //DayPlans[i].Items[j].Volume = 100;
    //j++;
    //DayPlans[i].Items[j].hour   = 22;
    //DayPlans[i].Items[j].min    = 0;
    //DayPlans[i].Items[j].Bright = 30;
    //DayPlans[i].Items[j].Volume = 30;
    //j++;

    //ActiveDayPlans = i+1;
    
    for (i=0; i<MAX_WEEK_PLANS; i++)
        WeekPlans[i] = 0;    

    //// Set Default Week Plans
    //WeekPlans[0] = 1;   // Âñ
    //WeekPlans[1] = 0;   // Ïí
    //WeekPlans[2] = 0;   // Âò
    //WeekPlans[3] = 0;   // Ñð
    //WeekPlans[4] = 0;   // ×ò
    //WeekPlans[5] = 0;   // Ïò
    //WeekPlans[6] = 1;   // Ñá

    //WeekPlans[7] = 0;   // Default

    // Clear All Holidays
    for (i=0; i<MAX_HOLIDAYS; i++)
    {
        Holidays[i].day   = -1;
        Holidays[i].month = -1;
        Holidays[i].plan  = 0;
    }    

    // Set Default Holidays
    i=0;
    Holidays[i].day   = 1;
    Holidays[i].month = 1;
    //Holidays[i].plan  = 2;

    i++;
    Holidays[i].day   = 7;
    Holidays[i].month = 1;
    //Holidays[i].plan  = 2;

    i++;
    Holidays[i].day   = 23;
    Holidays[i].month = 2;
    //Holidays[i].plan  = 2;

    i++;
    Holidays[i].day   = 8;
    Holidays[i].month = 3;
    //Holidays[i].plan  = 2;

    i++;
    Holidays[i].day   = 1;
    Holidays[i].month = 5;
    //Holidays[i].plan  = 2;

    i++;
    Holidays[i].day   = 9;
    Holidays[i].month = 5;
    //Holidays[i].plan  = 2;

    i++;
    Holidays[i].day   = 7;
    Holidays[i].month = 11;
    //Holidays[i].plan  = 2;

    i++;
    ActiveHolidays = i;

    bNewSec = false;

    Status.CurDateTime.sec      = 0;    
    Status.CurDateTime.min      = 0;    
    Status.CurDateTime.hour     = 0;   
    Status.CurDateTime.weekday  = 0;
    Status.CurDateTime.day      = 1;    
    Status.CurDateTime.month    = 1;  
    Status.CurDateTime.year     = 2012;   

    Status.HolidayIndex = -1;
    Status.CurPlan      = 0;
    Status.CurItem      = 0;
    
    SetBright_Prc(DEFAULT_BRIGHT_PRC);
    SetVolume_Prc(DEFAULT_VOLUME_PRC);
    
    Status.RedState     = 0;
    Status.YellowState  = 0;
    Status.GreenState   = 0;
    Status.RCount       = -1;
    Status.GCount       = -1;
    Status.MainState    = 0;

    Status.Errors       = 0;
    Status.SensorValue  = -1;
    Status.bSensorMode  = 0;

    //Status.Reserved[0]  = 0;
    //Status.Reserved[1]  = 0;
    //Status.Reserved[2]  = 0;
}


bool CheckConfig(TConfig* pCfg)
{
    if (pCfg->SampleNum >= MAX_SOUND_NUMBER)
        pCfg->SampleNum = 0;

    if (pCfg->TeachingPeriods > MAX_TEACHING_PERIODS)
        pCfg->TeachingPeriods = MAX_TEACHING_PERIODS;

    if (pCfg->TeachingPeriods < MIN_TEACHING_PERIODS)
        pCfg->TeachingPeriods = MIN_TEACHING_PERIODS;

    if (pCfg->SpeedUpTime > MAX_SPEEDUP_TIME)
        pCfg->SpeedUpTime = MAX_SPEEDUP_TIME;

    return true;
}

bool CheckConfig2(TConfig2* pCfg)
{
    if (Config2.MinDayLight < Config2.MaxNightLight)
        return false;

    if (Config2.MinSensorBrightRG > Config2.MaxSensorBrightRG)
        return false;
    
    //if (Config2.MinSensorBrightY > Config2.MaxSensorBrightY)
    //    return false;
    
    if (pCfg->SensorFilterTime < MIN_SENSOR_FILTER_TIME)
        pCfg->SensorFilterTime = MIN_SENSOR_FILTER_TIME;
    
    if (pCfg->SensorFilterTime > MAX_SENSOR_FILTER_TIME)
        pCfg->SensorFilterTime = MAX_SENSOR_FILTER_TIME;
    
    return true;
}

bool CheckDayPlan(TDayPlan *pDayPlan, uint8 NumAct)
{
    int i;
    int32 HM, prevHM;

    prevHM = -1;

    for (i=0; i<NumAct; i++)
    {
        if ( (pDayPlan->Items[i].hour > 23) || (pDayPlan->Items[i].min  > 59) )
        {
            return false;
        }

        HM = pDayPlan->Items[i].min + ((uint32)pDayPlan->Items[i].hour<<8);

        if (HM <= prevHM)
        {
            return false;
        }

        prevHM = HM;
        
        if (pDayPlan->Items[i].Volume > 100)
            pDayPlan->Items[i].Volume = 100;

        if (pDayPlan->Items[i].Bright > 100)
            pDayPlan->Items[i].Bright = 100;
    }

    return true;
}

bool CheckWeek(int8 *pWeek)
{
    if ( (pWeek[MAX_WEEK_PLANS-1] < MAX_DAY_PLANS) && (pWeek[MAX_WEEK_PLANS-1] >= 0) )
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool CheckHolidays(THoliday *pHolidays, uint8 NumAct)
{
    int i;

    for (i=0; i<NumAct; i++)
    {
        if ( (pHolidays[i].day   < 1)  ||
             (pHolidays[i].day   > 31) ||
             (pHolidays[i].month < 1)  ||
             (pHolidays[i].month > 12) ||
             (pHolidays[i].plan  >= MAX_DAY_PLANS)
           )
        {
            return false;
        }
    }

    return true;
}

void SetBright_Prc(uint16 prc)
{
    if (prc>100)
        prc=100;
    
    Status.CurBright = prc;
    CurBrightG = Status.CurBright*GREEN_MAX_BRIGHT/100;
    CurBrightR = Status.CurBright*RED_MAX_BRIGHT/100;
}

void SetVolume_Prc(uint16 prc)
{
    if (prc>100)
        prc=100;
    
    Status.CurVolume = prc;
    CurVolume = Status.CurVolume*SOUND_PWM_MAXVOLUME/100;
}
