//---------------------------------------------------------------------------
//
// $Title:          DateTime.h
// $Description:    Date and Time Functions Interface
//                  
// $Rev:            0.0.8
//
// $Author:         EEA
// $Created:        17.10.7
// $Last Modified:  25.10.11
//
// $Copyright:      Electrotex
//
// $Notes:          
//
//---------------------------------------------------------------------------
#ifndef __DATETIME_H
#define __DATETIME_H

#include "EEA_port.h"

typedef struct
{
    uint8  sec;     // 0-59
    uint8  min;     // 0-59
    uint8  hour;    // 0-23
    uint8  weekday; // 0-6  // 0 - Sunday
    uint8  day;     // 1-31
    uint8  month;   // 1-12 // 1 - Jan
    uint16 year;    // any positive
} TDateTime;

typedef enum
{
    SUNDAY,
    MONDAY,
    TUESDAY,
    WEDNESDAY,
    THURSDAY,
    FRIDAY,
    SATURDAY
} TWeekDayName;

typedef enum
{
    JANUARY = 1,
    FEBRUARY,
    MARCH,
    APRIL,
    MAY,
    JUNE,
    JULY,
    AUGUST,
    SEPTEMBER,
    OCTOBER,
    NOVEMBER,
    DECEMBER
} TMonthName;

extern TDateTime DateTime;

void DateTime_Init(void);

void DateTime_AddSec(void);

void Set_DateTime(TDateTime *pDateTime);
bool Get_DateTime(TDateTime *pDateTime);
void Copy_DateTime(TDateTime *pFromDT, TDateTime *pToDT);

bool IsDateTimeOK(TDateTime *pDT);

#endif