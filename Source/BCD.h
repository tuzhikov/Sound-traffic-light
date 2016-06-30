//---------------------------------------------------------------------------
//
// $Title:          BCD.h
// $Description:    BCD Arithmetic Inteface
//                  
// $Rev:            0.0.1
//
// $Author:         EEA
// $Created:        26.2.8
// $Last Modified:  26.2.8
//
// $Copyright:      Electrotex
//
// $Notes:          
//
//---------------------------------------------------------------------------
#ifndef __BCD_H
#define __BCD_H

#include "EEA_port.h"

int8  BCD_to_Byte(uint8  N);
int8  Byte_to_BCD(uint8  N);

int16 BCD_to_Word(uint16 N);
int16 Word_to_BCD(uint16 N);

#endif
