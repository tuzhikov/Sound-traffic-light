//---------------------------------------------------------------------------
//
// $Title:          LightSensor.c
// $Description:    Light Sensor Processing Functions.
//
// $Rev:            0.0.11
//
// $Author:         EEA
//
// $Created:        24.4.12
// $Last Modified:  5.12.13
//
// $Copyright:      EEA
//
// $Notes:
//
//---------------------------------------------------------------------------
//#include <NXP/iolpc2103.h>
#include "LightSensor.h"
#include "Pins.h"
#include "ADC.h"
#include "DataTable.h"

#define SENSOR_SKIP_NUMBER    1600
#define SENSOR_MEASURE_NUMBER 16

#define SENSOR_CORRUPT_FLAG 0x0001
#define SENSOR_LEARN_FLAG   0x0002

#define FORCED_LEARN_TIME   (5*60)
#define FORCED_MEASURE_TIME (5*60)
#define NO_MEASURE_TIME     (5)

//#define FORCED_LEARN_TIME   (5*60)
//#define FORCED_MEASURE_TIME (10)
//#define NO_MEASURE_TIME     (5)

//typedef struct
//{
//    int16  SensorValue;
//    uint8  SensorInvalid;
//    uint8  TotalCnt;
//    uint32 TotalSum;
//    uint32 LastCheckGenDay;     // (year<<16 + month<<8 + day)<<1 + am_pm_bit
//    uint8  StartHour;
//    uint8  StartMin;
//    uint8  StartSec;
//    uint8  LastHour;
//    uint8  LastMin;
//    uint8  LastSec;
//} TSensorInfo;
TSensorInfo SensorInfo;

bool bSensorDisableLights;
bool bSensorDataReady;

static bool bMeasure;
static bool bMeasureFinished;

static uint32 LocalValue;
static uint32 LocalSum;
static uint16 LocalCnt;

uint32 LearnCount;
uint32 ForcedMeasureCount;

TDateTime CurTime;



uint32 CalcTimeDelta(uint8 h1, uint8 m1, uint8 s1, uint8 h2, uint8 m2, uint8 s2);



void SensorPreInitMeasure(void)
{
    int i;
    volatile int j;
    
    uint32 StartDeltaT;     // (CurTime - StartTime) sec
    uint32 LastDeltaT;      // (CurTime - LastTime) sec

    CurTime = Status.CurDateTime;

    LocalSum = 0;

    LastDeltaT = CalcTimeDelta(Status.CurDateTime.hour, Status.CurDateTime.min, Status.CurDateTime.sec,
                               SensorInfo.LastHour,     SensorInfo.LastMin,     SensorInfo.LastSec
                              );

    ForcedMeasureCount = LastDeltaT;
    
    if (LastDeltaT > NO_MEASURE_TIME)
    {
        StartDeltaT = CalcTimeDelta(Status.CurDateTime.hour, Status.CurDateTime.min, Status.CurDateTime.sec,
                                    SensorInfo.StartHour,    SensorInfo.StartMin,    SensorInfo.StartSec
                                   );
        LearnCount = StartDeltaT;

        if (ForcedMeasureCount > FORCED_LEARN_TIME)
        {
            SensorInfo.TotalSum = 0;
            SensorInfo.TotalCnt = 0;

            LocalSum   = 0;
            LocalCnt   = 0;
            LocalValue = 0;

            SensorInfo.SensorInvalid |= SENSOR_LEARN_FLAG;

            LearnCount = 0;
        }

        for (i=0; i<SENSOR_MEASURE_NUMBER; i++)
        {
            LocalSum += ADC_GetResult(ADC_LIGHT_SENS_CHANNEL);
            LocalCnt++;

            // Delay
            for (j=0;j<300;j++);
        }

        LocalValue = LocalSum/LocalCnt;
        LocalSum = 0;
        LocalCnt = 0;

        // LastTime = CurTime;
        SensorInfo.LastHour = Status.CurDateTime.hour;
        SensorInfo.LastMin  = Status.CurDateTime.min;
        SensorInfo.LastSec  = Status.CurDateTime.sec;
        
        ForcedMeasureCount = 0;
        
        if ( (LearnCount > ((uint32)Config2.SensorFilterTime*60)) && SensorInfo.TotalCnt )
        {
            SensorInfo.SensorValue = SensorInfo.TotalSum/SensorInfo.TotalCnt;

            SensorInfo.TotalSum = 0;
            SensorInfo.TotalCnt = 0;

            SensorInfo.SensorInvalid &= ~SENSOR_LEARN_FLAG;
        }

        if (!SensorInfo.TotalCnt)
        {
            // StartTime = CurTime;
            SensorInfo.StartHour = Status.CurDateTime.hour;
            SensorInfo.StartMin  = Status.CurDateTime.min;
            SensorInfo.StartSec  = Status.CurDateTime.sec;

            LearnCount = 0;
        }

        SensorInfo.TotalSum += LocalValue;
        SensorInfo.TotalCnt++;
    }
}

void SensorFastLoopControl(bool bOff)
{
    if (bOff)
    {
        if (!bMeasureFinished)
        {
            bSensorDisableLights = true;
            bMeasure = true;
        }
       
        if (bMeasure)
        {
            LocalCnt++;

            if (LocalCnt > SENSOR_SKIP_NUMBER)
            {           
                LocalSum += ADC_GetResult(ADC_LIGHT_SENS_CHANNEL);
                //LocalCnt++;
               
                if (LocalCnt >= (SENSOR_MEASURE_NUMBER+SENSOR_SKIP_NUMBER))
                {
                    // MeasureFinished
                    bSensorDataReady = true;
                    bMeasureFinished = true;
                    bMeasure         = false;
                    bSensorDisableLights = false;
                }
            }
        }
    }
    else
    {
        if (bMeasureFinished)
        {
            bMeasureFinished = false;
        }
    }
}

void SensorSlowLoopControl(void)
{
    if (bSensorDataReady)
    {
        LocalValue = LocalSum/(LocalCnt-SENSOR_SKIP_NUMBER);
        LocalSum = 0;
        LocalCnt = 0;

        // LastTime = CurTime;
        SensorInfo.LastHour = Status.CurDateTime.hour;
        SensorInfo.LastMin  = Status.CurDateTime.min;
        SensorInfo.LastSec  = Status.CurDateTime.sec;
        
        ForcedMeasureCount = 0;

        if (!SensorInfo.TotalCnt)
        {
            // StartTime = CurTime;
            SensorInfo.StartHour = Status.CurDateTime.hour;
            SensorInfo.StartMin  = Status.CurDateTime.min;
            SensorInfo.StartSec  = Status.CurDateTime.sec;

            LearnCount = 0;
        }

        SensorInfo.TotalSum += LocalValue;
        SensorInfo.TotalCnt++;

        bSensorDataReady = false;
    }

    ForcedMeasureCount++;
    if ( (ForcedMeasureCount > FORCED_MEASURE_TIME) && !bMeasure )
    {
        //SensorInfo.TotalSum = 0;
        //SensorInfo.TotalCnt = 0;
        //
        //SensorInfo.SensorInvalid |= SENSOR_LEARN_FLAG;

        // Start Measure
        bSensorDisableLights = true;
        bMeasureFinished     = false;
    }
    
    LearnCount++;
    if ( (LearnCount > ((uint32)Config2.SensorFilterTime*60)) )
    //if ( (LearnCount > ((uint32)Config2.SensorFilterTime*6)) )
    //if ( (LearnCount > ((uint32)Config2.SensorFilterTime)) )
    {
        if (SensorInfo.TotalCnt)
        {
            SensorInfo.SensorValue = SensorInfo.TotalSum/SensorInfo.TotalCnt;

            SensorInfo.TotalSum = 0;
            SensorInfo.TotalCnt = 0;

            SensorInfo.SensorInvalid &= ~SENSOR_LEARN_FLAG;
        }
    }

    // Update Sensor Status
    Status.SensorValue = GetLightSensorPercent(SensorInfo.SensorValue);
    Status.SensorFlags = SensorInfo.SensorInvalid;
}

void InitLightSensor(void)
{
    bMeasure             = false;
    bMeasureFinished     = true;
    bSensorDisableLights = false;
    bSensorDataReady     = false;

    LocalSum = 0;
    LocalCnt = 0;

    SensorInfo.SensorValue      = 0;
    SensorInfo.SensorInvalid    = SENSOR_LEARN_FLAG;
    SensorInfo.TotalCnt         = 0;
    SensorInfo.TotalSum         = 0;
    SensorInfo.LastCheckGenDay  = 0;     // (year<<16 + month<<8 + day)<<1 + am_pm_bit
    SensorInfo.StartHour        = 0;
    SensorInfo.StartMin         = 0;
    SensorInfo.StartSec         = 0;
    SensorInfo.LastHour         = 0;
    SensorInfo.LastMin          = 0;
    SensorInfo.LastSec          = 0;
}

void ResetLightSensor(void)
{
    int i;
    uint8 *ptr = (uint8*)&SensorInfo;

    for (i=0; i<sizeof(SensorInfo); i++)
        ptr[i] = 0;

    SensorInfo.SensorInvalid = SENSOR_LEARN_FLAG;
}

void CheckLightSensor(TDateTime *Today)
{
    uint16 SensPrc;
    
    uint32 GenDay;  // General Date

    GenDay = (((uint32)Today->year<<16)+((uint32)Today->month<<8)+(uint32)Today->day) << 1;
    
    if ( Today->hour == SENSOR_CHECK1_HOUR )
    {
        // Check1 Hour
        // GenDay+=0;
    }
    else if ( Today->hour == SENSOR_CHECK2_HOUR )
    {
        // Check2 Hour
        GenDay += 1;
    }
    else
    {
        return;
    }

    if ( (GenDay != SensorInfo.LastCheckGenDay) && !(SensorInfo.SensorInvalid & SENSOR_LEARN_FLAG) )
    {
        SensPrc = GetLightSensorPercent(SensorInfo.SensorValue);
        
        if ( (Today->hour>=DAY_START_HOUR) && (Today->hour<NIGHT_START_HOUR) )
        {
            // Day
            if (SensPrc < Config2.MinDayLight)
            {
                SensorInfo.SensorInvalid |= SENSOR_CORRUPT_FLAG;
            }
            else
            {
                SensorInfo.SensorInvalid &= ~SENSOR_CORRUPT_FLAG;
            }
        }
        else
        {
            // Night
            if (SensPrc > Config2.MaxNightLight)
            {
                SensorInfo.SensorInvalid |= SENSOR_CORRUPT_FLAG;
            }
            else
            {
                SensorInfo.SensorInvalid &= ~SENSOR_CORRUPT_FLAG;
            }
        }

        SensorInfo.LastCheckGenDay = GenDay;
    }

    return;
}

bool IsLightSensorChecked(TDateTime *Today)
{
    return false;
}

uint16 GetLightSensorPercent(uint16 Value)
{
    uint16 prc;
    
    prc = (LIGHT_SENS_PRC_SCALE * ((uint32)ADC_MAX_VALUE - (uint32)Value)) >> 16;

    return prc;
}

void ProcessLightSensor(void)
{
    SensorInfo.SensorValue += ADC_GetResult(ADC_LIGHT_SENS_CHANNEL);
    SensorInfo.SensorValue >>= 1;
}

void SetSensorBright(uint8 BrightRG, /*uint8 BrightY,*/ uint8 hour)
{
    uint16 SensPrc;
    uint16 prcRG;
    //uint16 prcY;
    
    SensPrc = GetLightSensorPercent(SensorInfo.SensorValue);
    //SensPrc = GetLightSensorPercent(LocalValue);

    if (SensPrc > Config2.MinDayLight)
    {
        prcRG = Config2.MaxSensorBrightRG;
        //prcY  = Config2.MaxSensorBrightY;
    }
    else if (SensPrc < Config2.MaxNightLight)
    {
        prcRG = Config2.MinSensorBrightRG;
        //prcY  = Config2.MinSensorBrightY;
    }
    else
    {
        prcRG = Config2.MinSensorBrightRG + (Config2.MaxSensorBrightRG-Config2.MinSensorBrightRG)*(SensPrc-Config2.MaxNightLight)/(Config2.MinDayLight-Config2.MaxNightLight);
        //prcY  = Config2.MinSensorBrightY  + (Config2.MaxSensorBrightY -Config2.MinSensorBrightY )*(SensPrc-Config2.MaxNightLight)/(Config2.MinDayLight-Config2.MaxNightLight);
    }

    SetBright_Prc(prcRG);
    //SetBrightRG_Prc(prcRG);
    //SetBrightY_Prc(prcY);
}

uint32 CalcTimeDelta(uint8 h1, uint8 m1, uint8 s1, uint8 h2, uint8 m2, uint8 s2)
{
    uint32 Delta;
    
    if (s1<s2)
    {
        s1 += 60;
        m1--;
    }

    Delta = s1-s2;

    if (m1<m2)
    {
        m1 += 60;
        h1--;
    }

    Delta += (uint32)(m1-m2)*60;

    if (h1<h2)
    {
        h1 += 24;
    }

    Delta += (uint32)(h1-h2)*3600;

    return Delta;
}


void Sensor_FillDebugInfo(uint8* ptr)
{
    *((uint32*)ptr) = LearnCount;
    ptr+=4;

    *((uint32*)ptr) = ForcedMeasureCount;
    ptr+=4;

    *((uint32*)ptr) = LocalValue;
    ptr+=4;

    *((uint32*)ptr) = LocalSum;
    ptr+=4;

    *((uint16*)ptr) = LocalCnt;
    ptr+=4;

    *((TSensorInfo*)ptr) = SensorInfo;
}
    