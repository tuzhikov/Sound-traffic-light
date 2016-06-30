//---------------------------------------------------------------------------
//
// $Title:          System.h
// $Description:    General System Functions.
//
// $Rev:            0.0.3
//
// $Author:         EEA
//
// $Created:        9.10.11
// $Last Modified:  28.11.11
//
// $Copyright:      EEA
//
// $Notes:
//
//---------------------------------------------------------------------------
#ifndef __SYSTEM_H
#define __SYSTEM_H

#include <NXP/iolpc2103.h>

__IO_REG32(PWRCTRL, 0xE0024040,__READ_WRITE);
__IO_REG32(CRP_REG, 0x000001FC,__WRITE);

#define WDEN    0x0001 
#define WDRESET 0x0002
#define WDTOF   0x0004
#define WDINT   0x0008

#define CRP_CODE1_MAGIC   0x12345678
#define CRP_CODE2_MAGIC   0x87654321

// Are You Sure To Use CRP3?
//----#define CRP_CODE3_MAGIC   0x43218765

void System_Init(void);

#endif