//---------------------------------------------------------------------------
//
// $Title:          Inputs.h
// $Description:    Inputs Interface.
//
// $Rev:            0.0.1
//
// $Author:         EEA
//
// $Created:        17.10.11
// $Last Modified:  17.10.11
//
// $Copyright:      EEA
//
// $Notes:
//
//---------------------------------------------------------------------------

#ifndef __INPUTS_H
#define __INPUTS_H

#include "EEA_port.h"

uint32 GetInputsState(void);

bool IsTestModeIn(void);
bool IsRedIn(void);
bool IsGreenIn(void);
bool isButtonIn(void);
void checkButton(void);
bool retStatusButton(void);

#endif
