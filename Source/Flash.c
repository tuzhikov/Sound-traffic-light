//---------------------------------------------------------------------------
//
// $Title:          Flash.c
// $Description:    Flash Memory Functions and Data.
//                  
// $Rev:            0.0.11
//
// $Author:         EEA
// $Created:        6.11.11
// $Last Modified:  29.6.12
//
// $Copyright:      EEA
//
// $Notes:          
//
//---------------------------------------------------------------------------
#include "Flash.h"

#include "LightAutomata.h"
#include "I2C.h"
#include "CRC.h"
#include "DataTable.h"
#include "LightAutomata.h"

#define FLASH_BUFFER_SIZE 0x80

#define FLASH_DEVICE_ADDR 0xA0

#define FLASH_SIZE_16K    0x0800
#define FLASH_SIZE_64K    0x2000

#define DEF_FLASH_VERSION_H 2
#define DEF_FLASH_VERSION_L 0

#define DEF_FLASH_SIZE    FLASH_SIZE_16K

#define DEF_SLOT_SIZE       0x0080
#define DEF_DATA_SIZE       (MAX_DAY_PLAN_ITEMS*sizeof(TDayPlanItem))

#define DEF_STATE1_DISP     DEF_SLOT_SIZE
#define DEF_STATE2_DISP     (DEF_SLOT_SIZE + (DEF_SLOT_SIZE/2))
#define DEF_STATE_SIZE      (sizeof(TState))

#define DEF_SETTINGS_SLOT   0x02
#define DEF_DATA_SLOT       0x04
#define DEF_DATA_SLOTS_NUM  MAX_DAY_PLANS
#define DEF_BACKUP_SLOT     0x0F

#define FLASH_VERSION_LOWLIM    2
#define FLASH_VERSION_HIGHLIM   2

#define SETTINGS_CONFIG_DISP    0x00
#define SETTINGS_CONFIG2_DISP   0x08
#define SETTINGS_WEEK_DISP      0x18
#define SETTINGS_HOLIDAYS_DISP  0x20


#define MAX_DETECT_ATTEMPTS 10

typedef struct
{
    uint8  FlashVersionL;
    uint8  FlashVersionH;
    uint16 FlashSize;
    uint8  SlotSize;
    uint8  SlotDataSize;
    uint8  State1Disp;
    uint8  State2Disp;
    uint8  StateSize;
    uint8  Reserved_0;
    uint8  SettingsSlot;
    uint8  DataSlot;
    uint8  DataSlotsNum;
    uint8  BackupSlot;
} TFlashInfo;

int8 SlotNumInBackup;
bool bFlashValid;

const TFlashInfo DefaultFlashInfo = 
{
    DEF_FLASH_VERSION_L,    // FlashVersionL;
    DEF_FLASH_VERSION_H,    // FlashVersionH;
    DEF_FLASH_SIZE,         // FlashSize;
    DEF_SLOT_SIZE,          // SlotSize;
    DEF_DATA_SIZE,          // SlotDataSize;
    DEF_STATE1_DISP,        // State1Disp;
    DEF_STATE2_DISP,        // State2Disp;
    DEF_STATE_SIZE,         // StateSize;
    0,                      // Reserved_0;
    DEF_SETTINGS_SLOT,      // SettingsSlot;
    DEF_DATA_SLOT,          // DataSlot;
    DEF_DATA_SLOTS_NUM,     // DataSlotsNum;
    DEF_BACKUP_SLOT         // BackupSlot;
};


TFlashInfo FlashInfo;
TPublicFlashInfo PublicFlashInfo;

uint16 FlashMode = FLASH_MODE_16K;

uint8 FlashBuffer[FLASH_BUFFER_SIZE];

//------------------------------------------------
// Prototypes
void Flash_CheckBackupSlot(void);

bool ReadFlashInfo(void);
bool WriteFlashInfo(void);

bool GetFlashInfo(TPublicFlashInfo *pExtInfo);
//------------------------------------------------


void Flash_CheckBackupSlot(void)
{
    uint16 Addr;
    TI2C_Result res;

    SlotNumInBackup = -1;
    
    Addr = FlashInfo.BackupSlot*FlashInfo.SlotSize + FlashInfo.SlotDataSize + 2;
    
    if (FlashMode == FLASH_MODE_16K)
        res = I2C_ReadData(FLASH_DEVICE_ADDR|(Addr>>7)&0xFE, 1, Addr&0xFF, 2, FlashBuffer);
    else
        res = I2C_ReadData(FLASH_DEVICE_ADDR, 2, Addr, 2, FlashBuffer);

    if (res == I2C_RES_OK)
    {
        if (FlashBuffer[0]^FlashBuffer[1] == 0xFF)
        {
            SlotNumInBackup = FlashBuffer[0];
        }
    }
}

bool Flash_LoadDayPlan(uint8 Num)
{
    int  i;
    TI2C_Result res;
    uint16 CRC;
    const uint8 Size = FlashInfo.SlotDataSize;
    uint16 Addr;
    bool  bOK;
    uint8 *ptr;

    if ( (Num>=FlashInfo.DataSlotsNum) || (Num>=MAX_DAY_PLANS) )
        return false;

    bOK = false;

    Addr = (FlashInfo.DataSlot+Num)*FlashInfo.SlotSize;
    // Load Data to Buffer
    if (FlashMode == FLASH_MODE_16K)
        res = I2C_ReadData(FLASH_DEVICE_ADDR|(Addr>>7)&0xFE, 1, Addr&0xFF, Size+2, FlashBuffer);
    else    
        res = I2C_ReadData(FLASH_DEVICE_ADDR, 2, Addr, Size+2, FlashBuffer);

    CRC = CalcCRC16(FlashBuffer, Size);

    if ( (res == I2C_RES_OK) && (CRC == *(uint16*)&FlashBuffer[Size]) )
    {
        // if Data correct, put it to place
        bOK = true;
    }
    else if (SlotNumInBackup == (FlashInfo.DataSlot+Num))
    {
        // If Data Incorrect or Load Error
        
        Addr = FlashInfo.BackupSlot*FlashInfo.SlotSize;
        // Load Second Copy of Data to Buffer
        if (FlashMode == FLASH_MODE_16K)
            res = I2C_ReadData(FLASH_DEVICE_ADDR|(Addr>>7)&0xFE, 1, Addr&0xFF, Size+2, FlashBuffer);
        else
            res = I2C_ReadData(FLASH_DEVICE_ADDR, 2, Addr, Size+2, FlashBuffer);

        CRC = CalcCRC16(FlashBuffer, Size);

        if ( (res == I2C_RES_OK) && (CRC == *(uint16*)&FlashBuffer[Size]) )
        {
            // if Data correct, put it to place
            bOK = true;
            Addr = (FlashInfo.DataSlot+Num)*FlashInfo.SlotSize;
            
            if (FlashMode == FLASH_MODE_16K)
                I2C_WriteData(FLASH_DEVICE_ADDR|(Addr>>7)&0xFE, 1, Addr&0xFF, Size+2, FlashBuffer);
            else
                I2C_WriteData(FLASH_DEVICE_ADDR, 2, Addr, Size+2, FlashBuffer);
        }
    }

    if (bOK)
    {
        ptr = (uint8*)&DayPlans[Num];
        for (i=0; i<sizeof(DayPlans[Num].Items); i++)
        {
            ptr[i] = FlashBuffer[i];
        }

        DayPlans[Num].ActiveItems = MAX_DAY_PLAN_ITEMS;
        
        for (i=0; i<MAX_DAY_PLAN_ITEMS; i++)
        {
            if (DayPlans[Num].Items[i].hour == -1)
            {
                DayPlans[Num].ActiveItems = i;
                break;
            }
        }
    }
    
    return bOK;
}

bool Flash_SaveDayPlan(uint8 Num)
{
    int  i;
    TI2C_Result res;
    uint16 CRC;
    const uint8 Size = FlashInfo.SlotDataSize;
    uint16 Addr;
    bool  bOK;
    uint8 *ptr;

    bOK = false;

    if ( (Num>=FlashInfo.DataSlotsNum) || (Num>=MAX_DAY_PLANS) )
        return false;

    // Fill Buffer with Zeroes
    for (i=0; i<Size; i++)
        FlashBuffer[i] = 0;

    // Put Data to Buffer
    ptr = (uint8*)&DayPlans[Num];
    for (i=0; i<sizeof(DayPlans[Num].Items); i++)
    {
        FlashBuffer[i] = ptr[i];
    }

    // Put CRC to Buffer
    CRC = CalcCRC16(&FlashBuffer[0], Size);

    FlashBuffer[Size]   = CRC & 0xFF;
    FlashBuffer[Size+1] = CRC >> 8;

    FlashBuffer[Size+2] = (FlashInfo.DataSlot+Num);
    FlashBuffer[Size+3] = (FlashInfo.DataSlot+Num)^0xFF;

    // Save Backup Copy of Data
    Addr = FlashInfo.BackupSlot*FlashInfo.SlotSize;

    if (FlashMode == FLASH_MODE_16K)
        res = I2C_WriteData(FLASH_DEVICE_ADDR|(Addr>>7)&0xFE, 1, Addr&0xFF, Size+4, FlashBuffer);
    else    
        res = I2C_WriteData(FLASH_DEVICE_ADDR, 2, Addr, Size+4, FlashBuffer);

    if (res == I2C_RES_OK)
        bOK = true;

    // Save Work Copy of Data
    Addr = (FlashInfo.DataSlot+Num)*FlashInfo.SlotSize;
    
    if (FlashMode == FLASH_MODE_16K)
        res = I2C_WriteData(FLASH_DEVICE_ADDR|(Addr>>7)&0xFE, 1, Addr&0xFF, Size+2, FlashBuffer);
    else    
        res = I2C_WriteData(FLASH_DEVICE_ADDR, 2, Addr, Size+2, FlashBuffer);

    if (res == I2C_RES_OK)
        bOK = true;

    return bOK;
}

bool Flash_LoadSettings(void)
{
    int  i;
    TI2C_Result res;
    uint16 CRC;
    const uint8 Size = FlashInfo.SlotDataSize;
    uint16 Addr;
    bool  bOK;
    uint8 *ptr;


    bOK = false;

    Addr = FlashInfo.SettingsSlot*FlashInfo.SlotSize;
    // Load Data to Buffer
    if (FlashMode == FLASH_MODE_16K)
        res = I2C_ReadData(FLASH_DEVICE_ADDR|(Addr>>7)&0xFE, 1, Addr&0xFF, Size+2, FlashBuffer);
    else    
        res = I2C_ReadData(FLASH_DEVICE_ADDR, 2, Addr, Size+2, FlashBuffer);

    CRC = CalcCRC16(FlashBuffer, Size);

    if ( (res == I2C_RES_OK) && (CRC == *(uint16*)&FlashBuffer[Size]) )
    {
        // if Data correct, put it to place
        bOK = true;
    }
    else if (SlotNumInBackup == FlashInfo.SettingsSlot)
    {
        // If Data Incorrect or Load Error
        
        Addr = FlashInfo.BackupSlot*FlashInfo.SlotSize;
        // Load Second Copy of Data to Buffer
        if (FlashMode == FLASH_MODE_16K)
            res = I2C_ReadData(FLASH_DEVICE_ADDR|(Addr>>7)&0xFE, 1, Addr&0xFF, Size+2, FlashBuffer);
        else
            res = I2C_ReadData(FLASH_DEVICE_ADDR, 2, Addr, Size+2, FlashBuffer);

        CRC = CalcCRC16(FlashBuffer, Size);

        if ( (res == I2C_RES_OK) && (CRC == *(uint16*)&FlashBuffer[Size]) )
        {
            // if Data correct, put it to place
            bOK = true;
            Addr = FlashInfo.SettingsSlot*FlashInfo.SlotSize;

            if (FlashMode == FLASH_MODE_16K)
                I2C_WriteData(FLASH_DEVICE_ADDR|(Addr>>7)&0xFE, 1, Addr&0xFF, Size+2, FlashBuffer);
            else
                I2C_WriteData(FLASH_DEVICE_ADDR, 2, Addr, Size+2, FlashBuffer);
        }
    }

    if (bOK)
    {
        ptr = (uint8*)&Config;
        for (i=0; i<sizeof(TConfig); i++)
        {
            ptr[i] = FlashBuffer[i+SETTINGS_CONFIG_DISP];
        }

        ptr = (uint8*)&Config2;
        for (i=0; i<sizeof(TConfig2); i++)
        {
            ptr[i] = FlashBuffer[i+SETTINGS_CONFIG2_DISP];
        }

        for (i=0; i<MAX_WEEK_PLANS; i++)
        {
            WeekPlans[i] = FlashBuffer[i+SETTINGS_WEEK_DISP];
        }

        ptr = (uint8*)&Holidays;
        for (i=0; i<sizeof(Holidays); i++)
        {
            ptr[i] = FlashBuffer[i+SETTINGS_HOLIDAYS_DISP];
        }
    }
    
    return bOK;
}

bool Flash_SaveSettings(void)
{
    int  i;
    TI2C_Result res;
    uint16 CRC;
    const uint8 Size = FlashInfo.SlotDataSize;
    uint16 Addr;
    bool  bOK;
    uint8 *ptr;


    bOK = false;

    // Fill Buffer with Zeroes
    for (i=0; i<Size; i++)
        FlashBuffer[i] = 0;

    // Put Data to Buffer
    ptr = (uint8*)&Config;
    for (i=0; i<sizeof(TConfig); i++)
    {
        FlashBuffer[i+SETTINGS_CONFIG_DISP] = ptr[i];
    }

    ptr = (uint8*)&Config2;
    for (i=0; i<sizeof(TConfig2); i++)
    {
        FlashBuffer[i+SETTINGS_CONFIG2_DISP] = ptr[i];
    }

    for (i=0; i<MAX_WEEK_PLANS; i++)
    {
        FlashBuffer[i+SETTINGS_WEEK_DISP] = WeekPlans[i];
    }

    ptr = (uint8*)&Holidays;
    for (i=0; i<sizeof(Holidays); i++)
    {
        FlashBuffer[i+SETTINGS_HOLIDAYS_DISP] = ptr[i];
    }
        
    // Put CRC to Buffer
    CRC = CalcCRC16(&FlashBuffer[0], Size);

    FlashBuffer[Size]   = CRC & 0xFF;
    FlashBuffer[Size+1] = CRC >> 8;

    FlashBuffer[Size+2] = FlashInfo.SettingsSlot;
    FlashBuffer[Size+3] = FlashInfo.SettingsSlot^0xFF;

    // Save Backup Copy of Data
    Addr = FlashInfo.BackupSlot*FlashInfo.SlotSize;

    if (FlashMode == FLASH_MODE_16K)
        res = I2C_WriteData(FLASH_DEVICE_ADDR|(Addr>>7)&0xFE, 1, Addr&0xFF, Size+4, FlashBuffer);
    else
        res = I2C_WriteData(FLASH_DEVICE_ADDR, 2, Addr, Size+4, FlashBuffer);

    if (res == I2C_RES_OK)
        bOK = true;

    // Save Work Copy of Data
    Addr = FlashInfo.SettingsSlot*FlashInfo.SlotSize;
    
    if (FlashMode == FLASH_MODE_16K)
        res = I2C_WriteData(FLASH_DEVICE_ADDR|(Addr>>7)&0xFE, 1, Addr&0xFF, Size+2, FlashBuffer);
    else
        res = I2C_WriteData(FLASH_DEVICE_ADDR, 2, Addr, Size+2, FlashBuffer);

    if (res == I2C_RES_OK)
        bOK = true;

    return bOK;
}

bool Flash_LoadState(void)
{
    TI2C_Result res;
    uint16 CRC;
    const uint8 Size = sizeof(TState);

    // Load Data to Buffer
    if (FlashMode == FLASH_MODE_16K)
        res = I2C_ReadData(FLASH_DEVICE_ADDR, 1, FlashInfo.State1Disp, Size+2, FlashBuffer);
    else
        res = I2C_ReadData(FLASH_DEVICE_ADDR, 2, FlashInfo.State1Disp, Size+2, FlashBuffer);

    CRC = CalcCRC16(FlashBuffer, Size);

    if ( (res == I2C_RES_OK) && (CRC == *(uint16*)&FlashBuffer[Size]) )
    {
        // if Data correct, put it to place
        ActivateState((TState*)&FlashBuffer);
        return true;
    }
/*
    // If Data Incorrect or Load Error
    
    // Load Second Copy of Data to Buffer
    if (FlashMode == FLASH_MODE_16K)
        res = I2C_ReadData(FLASH_DEVICE_ADDR, 1, FlashInfo.State2Disp, Size+2, FlashBuffer);
    else
        res = I2C_ReadData(FLASH_DEVICE_ADDR, 2, FlashInfo.State2Disp, Size+2, FlashBuffer);

    CRC = CalcCRC16(FlashBuffer, Size);

    if ( (res == I2C_RES_OK) && (CRC == *(uint16*)&FlashBuffer[Size]) )
    {
        // if Data correct, put it to place
        ActivateState((TState*)&FlashBuffer);
        return true;
    }
*/
    return false;
}

bool Flash_SaveState(void)
{
    TI2C_Result res;
    uint16 CRC;
    const uint8 Size = sizeof(TState);
    bool bOK;

    bOK = false;

    // Put Data to Buffer
    FillState((TState*)&FlashBuffer);
    // Put CRC to Buffer
    CRC = CalcCRC16(&FlashBuffer[0], Size);

    FlashBuffer[Size]   = CRC & 0xFF;
    FlashBuffer[Size+1] = CRC >> 8;

    // Save Data from Buffer
    if (FlashMode == FLASH_MODE_16K)
        res = I2C_WriteData(FLASH_DEVICE_ADDR, 1, FlashInfo.State1Disp, Size+2, &FlashBuffer[0]);
    else
        res = I2C_WriteData(FLASH_DEVICE_ADDR, 2, FlashInfo.State1Disp, Size+2, &FlashBuffer[0]);
    
    if (res == I2C_RES_OK)
        bOK = true;
/*
    if (FlashMode == FLASH_MODE_16K)
        res = I2C_WriteData(FLASH_DEVICE_ADDR, 1, FlashInfo.State2Disp, Size+2, &FlashBuffer[0]);
    else
        res = I2C_WriteData(FLASH_DEVICE_ADDR, 2, FlashInfo.State2Disp, Size+2, &FlashBuffer[0]);

    if (res == I2C_RES_OK)
        bOK = true;
*/
    return bOK;
}

bool ReadFlashInfo(void)
{
/*
    TI2C_Result res;
    uint16 CRC;
    const uint8 Size = sizeof(TFlashInfo);
    uint8 *ptr;
    int i;
    // Load Data to Buffer

    if (FlashMode == FLASH_MODE_16K)
        res = I2C_ReadData(FLASH_DEVICE_ADDR, 1, 0x00, Size+2, FlashBuffer);
    else
        res = I2C_ReadData(FLASH_DEVICE_ADDR, 2, 0x00, Size+2, FlashBuffer);
 
    CRC = CalcCRC16(FlashBuffer, Size);

    if ( (res == I2C_RES_OK) && (CRC == *(uint16*)&FlashBuffer[Size]) )
    {
        // if Data correct, put it to place
        ptr = (uint8*)&FlashInfo;
        for (i=0; i<Size; i++)
        {
            ptr[i] = FlashBuffer[i];
        }

        PublicFlashInfo.FlashVersionL = FlashInfo.FlashVersionL;
        PublicFlashInfo.FlashVersionH = FlashInfo.FlashVersionH;
        PublicFlashInfo.FlashSize     = FlashInfo.FlashSize    ;

        return true;

    }

    return false;
*/

// DEBUG+
    uint8 *pInfo;
    uint8 *pDefInfo;
    int i;

    const uint8 Size = sizeof(TFlashInfo);

    if (FlashMode == FLASH_MODE_16K)
        I2C_ReadData(FLASH_DEVICE_ADDR, 1, 0x00, Size+2, FlashBuffer);
    else
        I2C_ReadData(FLASH_DEVICE_ADDR, 2, 0x00, Size+2, FlashBuffer);
    
    
    pInfo    = (uint8*)&FlashInfo;
    pDefInfo = (uint8*)&DefaultFlashInfo;

    for (i=0; i<sizeof(TFlashInfo); i++)
      {
        *pInfo++ = *pDefInfo++;
    }

    if (FlashMode == FLASH_MODE_16K)
        FlashInfo.FlashSize = FLASH_SIZE_16K;
    else
        FlashInfo.FlashSize = FLASH_SIZE_64K;
    
    PublicFlashInfo.FlashVersionL = FlashInfo.FlashVersionL;
    PublicFlashInfo.FlashVersionH = FlashInfo.FlashVersionH;
    PublicFlashInfo.FlashSize     = FlashInfo.FlashSize    ;

    return true;
// DEBUG-
}


bool WriteFlashInfo(void)
{
    TI2C_Result res;
    uint16 CRC;
    const uint8 Size = sizeof(TFlashInfo);
    uint8 *ptr;
    int i;

    // Put Data to Buffer
    ptr = (uint8*)&FlashInfo;
    for (i=0; i<Size; i++)
    {
        FlashBuffer[i] = ptr[i];
    }
    
    // Put CRC to Buffer
    CRC = CalcCRC16(&FlashBuffer[0], Size);

    FlashBuffer[Size]   = CRC & 0xFF;
    FlashBuffer[Size+1] = CRC >> 8;

    // Save Data from Buffer
    if (FlashMode == FLASH_MODE_16K)
        res = I2C_WriteData(FLASH_DEVICE_ADDR, 1, 0x00, Size+2, &FlashBuffer[0]);
    else
        res = I2C_WriteData(FLASH_DEVICE_ADDR, 2, 0x00, Size+2, &FlashBuffer[0]);

    if (res == I2C_RES_OK)
    {
        return true;
    }

    return false;
}

bool FormatFlash(void)
{
    int i;
    uint8 *pInfo;
    uint8 *pDefInfo;
    bool bOK;

    pInfo    = (uint8*)&FlashInfo;
    pDefInfo = (uint8*)&DefaultFlashInfo;

    for (i=0; i<sizeof(TFlashInfo); i++)
    {
        *pInfo++ = *pDefInfo++;
    }

    if (FlashMode == FLASH_MODE_16K)
        FlashInfo.FlashSize = FLASH_SIZE_16K;
    else
        FlashInfo.FlashSize = FLASH_SIZE_64K;
    
    bOK = WriteFlashInfo();
    if (!bOK)
    {
        Status.Errors |= ERROR_FLASH_INFO_WRITE;
    }

    return bOK;
}

bool Flash_LoadAll(void)
{
    bool bOK;
    bool res;
    int  i;

    
    bFlashValid = true;
    bOK = ReadFlashInfo();
    if (!bOK)
    {
        Status.Errors |= ERROR_FLASH_NOT_VALID;
        bFlashValid = false;
    }
    else if ((FlashInfo.FlashVersionH < FLASH_VERSION_LOWLIM) || (FlashInfo.FlashVersionH > FLASH_VERSION_HIGHLIM))
    {
        Status.Errors |= ERROR_FLASH_UNCOMPATIBLE_VERSION;
        bFlashValid = false;
    }

    if (!bFlashValid)
        return false;

    res = true;

    bOK = Flash_LoadState();
    if (!bOK)
    {
        Status.Errors |= ERROR_FLASH_STATE_CORRUPTED;
        res = false;
    }

    Flash_CheckBackupSlot();

    for (i=0; i<MAX_DAY_PLANS; i++)
    {
        bOK = Flash_LoadDayPlan(i);
        if (!bOK)
        {
            Status.Errors |= ERROR_FLASH_SETTINGS_CORRUPTED;
            res = false;
        }
    }

    bOK = Flash_LoadSettings();
    if (!bOK)
    {
        Status.Errors |= ERROR_FLASH_SETTINGS_CORRUPTED;
        res = false;
    }

    return res;
}

bool Flash_SaveAll(void)
{
    bool bOK;
    bool res;
    int  i;
    
    res = true;

    for (i=0; i<MAX_DAY_PLANS; i++)
    {
        bOK = Flash_SaveDayPlan(i);
        if (!bOK)
        {
            Status.Errors |= ERROR_FLASH_DAYPLAN_WRITE;
            res = false;
        }
    }

    bOK = Flash_SaveSettings();
    if (!bOK)
    {
        Status.Errors |= ERROR_FLASH_SETTINGS_WRITE;
        res = false;
    }

    return res;
}

bool GetFlashInfo(TPublicFlashInfo *pExtInfo)
{
    if (bFlashValid)
    {
        pExtInfo->FlashVersionL = FlashInfo.FlashVersionL;
        pExtInfo->FlashVersionH = FlashInfo.FlashVersionH;
        pExtInfo->FlashSize     = FlashInfo.FlashSize    ;

        return true;
    }

    return false;
}

void DetectFlashType(void)
{
    TI2C_Result res;
    int Attempts = MAX_DETECT_ATTEMPTS;

    FlashMode = FLASH_MODE_16K;

    while (Attempts)
    {
        FlashMode = FLASH_MODE_16K;
        res = I2C_ReadData(FLASH_DEVICE_ADDR+0x0E, 1, 0xFF, 1, FlashBuffer);
     
        if (res == I2C_RES_OK)
        {
            break;
        }

        I2C_Stop();

        FlashMode = FLASH_MODE_64K;
        res = I2C_ReadData(FLASH_DEVICE_ADDR, 2, 0xFF, 1, FlashBuffer);
     
        if (res == I2C_RES_OK)
        {
            break;
        }

        I2C_Stop();
    }
}

bool Flash_TestRead(void)
{
    TI2C_Result res;
    const uint8 Size = sizeof(TState);

    // Load Data to Buffer
    if (FlashMode == FLASH_MODE_16K)
        res = I2C_ReadData(FLASH_DEVICE_ADDR, 1, FlashInfo.State1Disp, Size+2, FlashBuffer);
    else
        res = I2C_ReadData(FLASH_DEVICE_ADDR, 2, FlashInfo.State1Disp, Size+2, FlashBuffer);

    if (res == I2C_RES_OK)
        return true;
    else
        return false;
}

