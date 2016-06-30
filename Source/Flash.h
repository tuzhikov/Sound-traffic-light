//---------------------------------------------------------------------------
//
// $Title:          Flash.h
// $Description:    Flash Memory Interface.
//
// $Rev:            0.0.4
//
// $Author:         EEA
// $Created:        17.10.11
// $Last Modified:  2.3.12
//
// $Copyright:      EEA
//
// $Notes:
//
//---------------------------------------------------------------------------
#ifndef __FLASH_H
#define __FLASH_H

#include "EEA_port.h"

#define FLASH_MODE_16K  16
#define FLASH_MODE_64K  64

typedef struct
{
    uint8  FlashVersionL;
    uint8  FlashVersionH;
    uint16 FlashSize;
} TPublicFlashInfo;

extern bool bFlashValid;
extern TPublicFlashInfo PublicFlashInfo;
extern uint16 FlashMode;

bool Flash_LoadDayPlan(uint8 Num);
bool Flash_SaveDayPlan(uint8 Num);

bool Flash_LoadSettings(void);
bool Flash_SaveSettings(void);

bool Flash_LoadState(void);
bool Flash_SaveState(void);

bool FormatFlash(void);

bool Flash_LoadAll(void);
bool Flash_SaveAll(void);

void DetectFlashType(void);

bool Flash_TestRead(void);

#endif