//---------------------------------------------------------------------------
//
// $Title:          Sound.h
// $Description:    Sound Module Interface.
//
// $Rev:            0.0.3
//
// $Author:         EEA
//
// $Created:        14.10.11
// $Last Modified:  20.10.11
//
// $Copyright:      EEA
//
// $Notes:
//
//---------------------------------------------------------------------------
#ifndef __SOUND_H
#define __SOUND_H

#include "Timers.h"

#define SoundPWM_SetValue(N)    Timer2_SetValue(N)
#define SoundPWM_Start()        Timer2_Start()
#define SoundPWM_Stop()         Timer2_Stop()
#define SoundPWM_Reset()        Timer2_Reset()

void InitSound(void);
void SoundIntHandler(void);

void StartSound(void);
void StopSound(void);

#endif