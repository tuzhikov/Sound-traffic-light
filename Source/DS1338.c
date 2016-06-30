//---------------------------------------------------------------------------
//
// $Title:          DS1338.c
// $Description:    RTC DS1338 Functions and Data.
//                  
// $Rev:            0.0.5
//
// $Author:         EEA
// $Created:        17.10.11
// $Last Modified:  17.2.12
//
// $Copyright:      EEA
//
// $Notes:          
//
//---------------------------------------------------------------------------

#include "DS1338.h"
#include "I2C.h"
#include "BCD.h"
#include "System.h"

#include <intrinsics.h>

#define DS1338_DEVICE_ADDR  0xD0
#define DS1338_BAUDRATE     400000UL

#define DS1338_CONTROL_REG  0x00

#define DS1338_RS_BITMASK   0x03
#define DS1338_SWE_BIT      0x10
#define DS1338_OSF_BIT      0x20
#define DS1338_OUT_BIT      0x80

#define DS1338_H12_24_BIT   0x40

typedef struct
{
    uint8 sec_BCD;      // 00-59  [bit 7 - flag ClockHalt: (1-Halt, 0-Run)]
    uint8 min_BCD;      // 00-59
    uint8 hour_BCD;     // 00-23
    uint8 weekday;      // 1-7 !
    uint8 day_BCD;      // 1-31
    uint8 month_BCD;    // 1-12
    uint8 year_BCD;     // 00-99
    uint8 Control;      // Control Register
} TRTC_Info;

TDateTime RTC_Time;
bool RTC_TimeValid;

TRTC_Info RTC_Info;

// Prototypes
bool Convert_DS1338_to_User(TRTC_Info *pRTC_Info, TDateTime *pRTC_Time);
bool Convert_User_to_DS1338(TRTC_Info *pRTC_Info, TDateTime *pRTC_Time);


void DS1338_Init(void)
{
    RTC_TimeValid = false;
}

bool DS1338_GetTime(TDateTime *pDateTime)
{
    // Read from RTC
    // Convert
    // Check
    // Copy Time if OK
    // Return Result
    
    bool bOK = false;
    TI2C_Result res;

    //I2C_SetBaudRate(DS1338_BAUDRATE);
    
    //__disable_interrupt();
    res = I2C_ReadData(DS1338_DEVICE_ADDR, 1, 0x00, sizeof(TRTC_Info), (uint8*)&RTC_Info);
    //__enable_interrupt();

    if (res == I2C_RES_OK)
    {
        bOK = Convert_DS1338_to_User(&RTC_Info, &RTC_Time);
        if (bOK)
        {
            *pDateTime = RTC_Time;
            /*
            pDateTime->sec     = RTC_Time.sec;
            pDateTime->min     = RTC_Time.min;
            pDateTime->hour    = RTC_Time.hour;
            pDateTime->weekday = RTC_Time.weekday;
            pDateTime->day     = RTC_Time.day;
            pDateTime->month   = RTC_Time.month;
            pDateTime->year    = RTC_Time.year;
            */
        }
    }

    RTC_TimeValid = bOK;
    return bOK;
}

bool DS1338_SetTime(TDateTime *pDateTime)
{
    // Check
    // Convert if Ok
    // Write to RTC if OK
    // Return Result

    bool bOK = false;
    TI2C_Result res;

    //I2C_SetBaudRate(DS1338_BAUDRATE);

    bOK = Convert_User_to_DS1338(&RTC_Info, pDateTime);
    if (!bOK) return false;

    //__disable_interrupt();
    res = I2C_WriteData(DS1338_DEVICE_ADDR, 1, 0x00, sizeof(TRTC_Info), (uint8*)&RTC_Info);
    //__enable_interrupt();

    if (res != I2C_RES_OK)
        bOK = false;
    
    RTC_TimeValid = bOK;
    return bOK;
}

bool Convert_DS1338_to_User(TRTC_Info *pRTC_Info, TDateTime *pRTC_Time)
{
    bool bOK;
    int8 tmp;
        
    bOK = true;

    if (pRTC_Info->Control & DS1338_OSF_BIT) bOK = false;

    tmp = BCD_to_Byte(pRTC_Info->sec_BCD & 0x7F);
    if ( (tmp<0) || (tmp>59) ) bOK = false;
    else pRTC_Time->sec = tmp;
    
    tmp = BCD_to_Byte(pRTC_Info->min_BCD & 0x7F);
    if ( (tmp<0) || (tmp>59) ) bOK = false;
    else pRTC_Time->min = tmp;
    
    if (pRTC_Info->hour_BCD & DS1338_H12_24_BIT)
    {
        // 12 Hour Format
        tmp = BCD_to_Byte(pRTC_Info->hour_BCD & 0x1F);
        if ( (tmp<1) || (tmp>12) ) bOK = false;
        else
        {
            if (pRTC_Info->hour_BCD & 0x20) tmp += 12;
            if (tmp == 24) tmp = 0;
            pRTC_Time->hour = tmp;
        }
    }
    else
    {
        // 24 Hour Format
        tmp = BCD_to_Byte(pRTC_Info->hour_BCD & 0x3F);
        if ( (tmp<0) || (tmp>23) ) bOK = false;
        else pRTC_Time->hour = tmp;
    }

    tmp = BCD_to_Byte(pRTC_Info->day_BCD & 0x3F);
    if ( (tmp<1) || (tmp>31) ) bOK = false;
    else pRTC_Time->day = tmp;
    
    tmp = BCD_to_Byte(pRTC_Info->month_BCD & 0x1F);
    if ( (tmp<1) || (tmp>12) ) bOK = false;
    else pRTC_Time->month = tmp;
    
    tmp = BCD_to_Byte(pRTC_Info->year_BCD);
    if ( tmp<0 ) bOK = false;
    else pRTC_Time->year = 2000 + tmp;
    
    return bOK;
}

bool Convert_User_to_DS1338(TRTC_Info *pRTC_Info, TDateTime *pRTC_Time)
{
    bool res;
    
    res = IsDateTimeOK(pRTC_Time);

    if (res)
    {
        pRTC_Info->sec_BCD    = Byte_to_BCD(pRTC_Time->sec);
        pRTC_Info->min_BCD    = Byte_to_BCD(pRTC_Time->min);
        pRTC_Info->hour_BCD   = Byte_to_BCD(pRTC_Time->hour);
        pRTC_Info->weekday    = Byte_to_BCD(pRTC_Time->weekday)+1;
        pRTC_Info->day_BCD    = Byte_to_BCD(pRTC_Time->day);
        pRTC_Info->month_BCD  = Byte_to_BCD(pRTC_Time->month);
        pRTC_Info->year_BCD   = Byte_to_BCD(pRTC_Time->year-2000);
        pRTC_Info->Control    = DS1338_CONTROL_REG;
    }

    return res;
}



