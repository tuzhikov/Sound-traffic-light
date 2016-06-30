//---------------------------------------------------------------------------
//
// $Title:          Shedule.c
// $Description:    Day and Week Shedule Functions and Data.
//
// $Rev:            0.0.7
//
// $Author:         EEA
//
// $Created:        17.10.11
// $Last Modified:  22.1.14
//
// $Copyright:      EEA
//
// $Notes:
//
//---------------------------------------------------------------------------

#include "Shedule.h"
#include "DataTable.h"
#include "Config.h"
#include "LightSensor.h"

uint32 PrevDay    = 0xF0F0F0F0;
uint32 PrevMinute = 0xF0F0F0F0;

void FindDayPlan(void)
{
    int   i;
    uint8 CurPlan;
    int8  HolidayIndex;

    int8  weekday = Status.CurDateTime.weekday;
    int8  day     = Status.CurDateTime.day;
    int8  month   = Status.CurDateTime.month;

    for (i=0; i<MAX_HOLIDAYS; i++)
    {
        if ( (Holidays[i].day == day) && (Holidays[i].month == month) )
        {
            CurPlan      = Holidays[i].plan;
            HolidayIndex = i;
            break;
        }
    }

    if (i >= MAX_HOLIDAYS)
    {
        CurPlan      = WeekPlans[weekday];
        HolidayIndex = -1;
    }

    if (CurPlan >= MAX_DAY_PLANS)
        CurPlan = WeekPlans[7];

    Status.CurPlan      = CurPlan;
    Status.HolidayIndex = HolidayIndex;
}

void ProcessShedule(void)
{
    int    i;
    uint8  ActiveItems;
    uint8  CurItem;
    uint8  CurPlan;
    uint32 HM;
    uint32 TableHM;

    HM = Status.CurDateTime.min + ((uint32)Status.CurDateTime.hour<<8);

    CurPlan = Status.CurPlan;

    if (CurPlan < MAX_DAY_PLANS)
    {
        ActiveItems = DayPlans[CurPlan].ActiveItems;

        if (ActiveItems)
        {
            CurItem = ActiveItems-1;
            for (i=0; i<ActiveItems; i++)
            {
                TableHM = DayPlans[CurPlan].Items[i].min + ((uint32)DayPlans[CurPlan].Items[i].hour<<8);

                if (TableHM <= HM)
                {
                    CurItem = i;
                }
            }

            Status.CurItem = CurItem;


            if (DayPlans[CurPlan].Items[CurItem].bSensor && !SensorInfo.SensorInvalid)
            {
                SetSensorBright(DayPlans[CurPlan].Items[CurItem].Bright, Status.CurDateTime.hour);
                Status.bSensorMode = 1;
            }
            else
            {
                SetBright_Prc(DayPlans[CurPlan].Items[CurItem].Bright);
                Status.bSensorMode = 0;
            }

            SetVolume_Prc(DayPlans[CurPlan].Items[CurItem].Volume);

            return;

        }
    }

    Status.CurItem   = (uint8)-1;

    SetBright_Prc(DEFAULT_BRIGHT_PRC);
    SetVolume_Prc(DEFAULT_VOLUME_PRC);

    return;
}

bool IsNewDay(void)
{
    uint32 NewDay;

    NewDay = Status.CurDateTime.day + ((uint32)Status.CurDateTime.month<<8) + ((uint32)Status.CurDateTime.year<<16);

    if (PrevDay != NewDay)
    {
        PrevDay = NewDay;
        PrevMinute = 0xF0F0F0F0;
        return true;
    }
    else
    {
        return false;
    }
}

bool IsNewMinute(void)
{
    uint32 NewMinute;

    NewMinute = Status.CurDateTime.min + ((uint32)Status.CurDateTime.hour<<8);
    if (PrevMinute != NewMinute)
    {
        PrevMinute = NewMinute;
        return true;
    }
    else
    {
        return false;
    }
}



