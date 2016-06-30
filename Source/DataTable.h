//---------------------------------------------------------------------------
//
// $Title:          DataTable.h
// $Description:    Configuration and Status Data Interface.
//
// $Rev:            0.0.17
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
#ifndef __DATATABLE_H
#define __DATATABLE_H

#include "EEA_port.h"
#include "DateTime.h"
#include "Config.h"
#include "LightSensor.h"

#define ERROR_FLASH_NOT_VALID               0x0001
#define ERROR_FLASH_STATE_CORRUPTED         0x0002
#define ERROR_FLASH_SETTINGS_CORRUPTED      0x0004
#define ERROR_FLASH_DAYPLANS_CORRUPTED      0x0008

#define ERROR_FLASH_INFO_WRITE              0x0010
#define ERROR_FLASH_SETTINGS_WRITE          0x0020
#define ERROR_FLASH_DAYPLAN_WRITE           0x0040

#define ERROR_FLASH_UNCOMPATIBLE_VERSION    0x0080

#define ERROR_RTC_READ                      0x0100

#define ERROR_WDT_RESET                     0x0200

#define LEARNING_NOW    0
#define LEARN_FINISHED  1

typedef struct
{
    uint8 SampleNum;        // 0-3
    uint8 TeachingPeriods;  // 1-2
    uint8 bSpeedUp;
    uint8 SpeedUpTime;      // sec
} TConfig;

typedef struct
{
    uint8 bDisableRedCount;
    uint8 bDisableGreenCount;
    uint8 bGOST;                // bool8
    uint8 MinDayLight;          // %
    uint8 MaxNightLight;        // %
    uint8 MaxSensorBrightRG;    // %
    uint8 MinSensorBrightRG;    // %
    uint8 MaxSensorBrightY;     // %
    uint8 MinSensorBrightY;     // %
    uint8 SensorFilterTime;     // min
    uint8 Reserved[6];
} TConfig2;

typedef struct
{
    uint8 GLearnState;
    uint8 GLearnBufLen;
    uint8 GLearnIndex;
    uint8 GLearnPeriod;

    uint8 RLearnState;
    uint8 RLearnBufLen;
    uint8 RLearnIndex;
    uint8 RLearnPeriod;

    uint8 GLearnBuf[LEARN_BUF_SIZE];
    uint8 RLearnBuf[LEARN_BUF_SIZE];
} TLearnData;

typedef struct
{
    //uint8 GLineState;
    //uint8 YLineState;
    //uint8 RLineState;

    //uint8 GState;
    //uint8 YState;
    //uint8 RState;

    int32 GStablePeriods;
    int32 GTime;            // msec
    int32 GTime_prev;       // msec
    int32 GTime_sec;        // sec
    int32 GTimeLeft;        // sec
    int16 GTime_sec_prev;   // sec
    int16 GTimeJitterCount; 
    int16 GRoundUpValue;    // msec 
   
    int32 RStablePeriods;
    int32 RTime;            // msec
    int32 RTime_prev;       // msec
    int32 RTime_sec;        // sec
    int32 RTimeLeft;        // sec
    int16 RTime_sec_prev;   // sec
    int16 RTimeJitterCount; 
    int16 RRoundUpValue;    // msec 

    int16 GmsecCounter;     // msec
    int16 RmsecCounter;     // msec

    int32 PrevStateEnd_FullSec; // sec

    uint16 PrevState;

    TSensorInfo SensorInfo;

    TLearnData LearnData;
} TState;

typedef struct
{
    TDateTime CurDateTime;

    int8   HolidayIndex;
    uint8  CurPlan;
    uint8  CurItem;
    uint8  CurMode;
    uint16 CurBright;  // %
    uint16 CurVolume;  // %
    
    uint8  RedState;
    uint8  YellowState;
    uint8  GreenState;
    uint8  MainState;
    int16  RCount;
    int16  GCount;

    uint16 Errors;

    int16  SensorValue;
    int8   SensorFlags;
    int8   bSensorMode;
    int8   Reserved[6];
} TStatus;

typedef struct
{
    int8  hour;
    int8  min;
    uint8 Bright;  // %
    uint8 Volume;  // %
    uint8 bSensor;   // bool8
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

typedef struct
{
    uint8 GreenState;
    uint8 RedCount;
    uint8 Bright;
    uint8 Modulo;
} TSlaveCommand;

typedef enum
{
    WORK_MODE,
    TEST_MODE,
    DEMO_MODE
} TMode;


typedef struct
{
    uint8 MaxDayPlans;
    uint8 MaxPlanItems;
    uint8 MaxHolidays;
    uint8 MaxWeekPlans;
    uint8 MaxSounds;
    uint8 MaxSpeedUpTime;
    
    uint8 VersionH;
    uint8 VersionL;

    uint8 ProtocolH;
    uint8 ProtocolL;

    uint8 LampColors;
    uint8 RedMaxCount;
    uint8 YellowMaxCount;
    uint8 GreenMaxCount;

    uint16 DeviceType;
} TSoftwareInfo;


typedef enum
{
    COLOR_RED,
    COLOR_YELLOW,
    COLOR_GREEN
} TColor;

extern TConfig  Config;
extern TConfig2 Config2;

extern TStatus Status;
//extern TDateTime NewDateTime;
extern const TSoftwareInfo SoftwareInfo;

extern TLearnData LearnData;

extern int8 WeekPlans[MAX_WEEK_PLANS];
extern THoliday Holidays[MAX_HOLIDAYS];
extern TDayPlan DayPlans[MAX_DAY_PLANS];

extern bool bNewSec;

extern TSlaveCommand SlaveCommand;

extern uint16 CurBrightR;  // real Val
extern uint16 CurBrightG;  // real Val
extern uint16 CurVolume;   // real Val

void InitData(void);

bool CheckConfig(TConfig *pCfg);
bool CheckConfig2(TConfig2 *pCfg);
bool CheckDayPlan(TDayPlan *pDayPlan, uint8 NumAct);
bool CheckWeek(int8 *pWeek);
bool CheckHolidays(THoliday *pHolidays, uint8 NumAct);

void SetBright_Prc(uint16 prc);
void SetVolume_Prc(uint16 prc);


#endif