//---------------------------------------------------------------------------
//
// $Title:          DateTime.c
// $Description:    Date and Time Functions
//                  
// $Rev:            0.0.9
//
// $Author:         EEA
// $Created:        17.10.7
// $Last Modified:  17.10.11
//
// $Copyright:      EEA
//
// $Notes:          
//
//---------------------------------------------------------------------------

#include "DateTime.h"

TDateTime DateTime = {0, 0, 0, 5, 21, 10, 2011};

const uint8 MonthDisp[14] = { 0,  3,  3,  6,  1,  4,  6,  2,  5,  0,  3,  5,  1,  4};
const uint8 MonthDays[12] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};


bool  DateTimeValid = false;

uint16 IsLeapYear(uint16 y);
uint16 CalcWeekDay(uint16 y, uint16 m, uint16 d);

void DateTime_Init(void)
{
    DateTime.weekday = CalcWeekDay(DateTime.year, DateTime.month, DateTime.day);
    DateTimeValid = false;
}

void DateTime_AddSec(void)
{
    uint8 tmp;
    
    if (++DateTime.sec > 59)
    {
        DateTime.sec = 0;
        if (++DateTime.min > 59)
        {
            DateTime.min = 0;
            if (++DateTime.hour > 23)
            {
                DateTime.hour = 0;

                if (++DateTime.weekday > 6)
                {
                    DateTime.weekday = 0;
                };

                tmp = MonthDays[DateTime.month-1];
                if ( (DateTime.month==(uint8)FEBRUARY) && IsLeapYear(DateTime.year) )
                {
                   tmp++;
                }
                if (++DateTime.day > tmp)
                {
                    DateTime.day = 1;
                    if (++DateTime.month > DECEMBER)
                    {
                        DateTime.month = JANUARY;
                        DateTime.year++;
                    } // month
                } // day
            } // hour
        } // min
    } // sec
}

void Set_DateTime(TDateTime *pDateTime)
{
    DateTime = *pDateTime;
    
    //DateTime.sec     = pDateTime->sec;
    //DateTime.min     = pDateTime->min;
    //DateTime.hour    = pDateTime->hour;
    //DateTime.day     = pDateTime->day;
    //DateTime.month   = pDateTime->month;
    //DateTime.year    = pDateTime->year;

    DateTime.weekday = CalcWeekDay(DateTime.year, DateTime.month, DateTime.day);
    
    DateTimeValid = true;
}

bool Get_DateTime(TDateTime *pDateTime)
{
    if (DateTimeValid)
    {
        *pDateTime = DateTime;

        //pDateTime->sec     = DateTime.sec;
        //pDateTime->min     = DateTime.min;
        //pDateTime->hour    = DateTime.hour;
        //pDateTime->weekday = DateTime.weekday;
        //pDateTime->day     = DateTime.day;
        //pDateTime->month   = DateTime.month;
        //pDateTime->year    = DateTime.year;
    }
    return DateTimeValid;
}

void Copy_DateTime(TDateTime *pFromDT, TDateTime *pToDT)
{
    *pToDT = *pFromDT;
    
    //pToDT->sec     = pFromDT->sec;
    //pToDT->min     = pFromDT->min;
    //pToDT->hour    = pFromDT->hour;
    //pToDT->weekday = pFromDT->weekday;
    //pToDT->day     = pFromDT->day;
    //pToDT->month   = pFromDT->month;
    //pToDT->year    = pFromDT->year;
}


uint16 IsLeapYear(uint16 y)
{
    int16  dy;
    
    dy = y - 2000;

    if (dy<0) dy = -dy;

    while (dy >= 400) dy -= 400;
    if (dy == 0) return 1;

    while (dy >= 100) dy -= 100;
    if (dy == 0) return 0;

    if ( (dy&3) == 0 ) return 1;

    return 0;
}

uint16 CalcWeekDay(uint16 y, uint16 m, uint16 d)
{
    int16  deltaY, dy;
    uint16 y400, y100, y4;
    uint16 VisYears;
    uint8  Disp;
    
    deltaY = y - 2007;
    if ( (m==1) || (m==2) )
    {
        m+=12;
        deltaY--;
    }

    // Calc Number of 400years
    dy = deltaY;
    y400 = 0;
    while (dy > 393)
    {
        dy -= 400;
        y400++;
    }

    // Calc Number of 100years
    dy = deltaY;
    y100 = 0;
    while (dy > 93)
    {
        dy -= 100;
        y100++;
    }

    // Calc Number of 4years
    y4 = deltaY >> 2;
    if (deltaY&3) y4++;

    // Calc Number of Vis years
    VisYears = y4-y100+y400;

    Disp = deltaY + VisYears + MonthDisp[m-1] + d;

    Disp %= 7;

    // 0 - ‚á, 1 - ­, 2 - ‚â, ...
    return Disp;
}

bool IsDateTimeOK(TDateTime *pDT)
{
    if ( 
        (pDT->sec   > 59) ||
        (pDT->min   > 59) ||
        (pDT->hour  > 23) ||
        (pDT->day   <  1) ||
        (pDT->day   > 31) ||
        (pDT->month <  1) ||
        (pDT->month > 12)
       )
    {
        return false;
    }
    //else
    //{
    //    return true;
    //}

    return true;
}

