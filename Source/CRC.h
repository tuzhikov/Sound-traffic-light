//---------------------------------------------------------------------------
//
// $Title:          CRC.h
// $Description:    CRC calculation Inteface
//                  
// $Rev:            0.0.1
//
// $Author:         EEA
// $Created:        30.5.8
// $Last Modified:  19.10.11
//
// $Copyright:      Electrotex
//
// $Notes:          
//
//---------------------------------------------------------------------------

#ifndef __CRC_H
#define __CRC_H

#include "EEA_port.h"

extern const uint8 CRC_TableHi[];
extern const uint8 CRC_TableLo[];

uint16 CalcCRC16(uint8 *pPkt, uint16 size);

#endif
