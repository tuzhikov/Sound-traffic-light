//---------------------------------------------------------------------------
//
// $Title:          Sound.c
// $Description:    Sound Functions and Data.
//
// $Rev:            0.0.5
//
// $Author:         EEA
//
// $Created:        9.10.11
// $Last Modified:  23.11.11
//
// $Copyright:      EEA
//
// $Notes:
//
//---------------------------------------------------------------------------
#include "Sound.h"
#include "Samples\Samples.h"
#include "Config.h"
#include "DataTable.h"

bool sndFinished;
uint16 NextSoundByte;

unsigned char const *pSoundArray;
unsigned int sndIndex;
unsigned int sndLength;

const unsigned char *Sounds[MAX_SOUND_NUMBER] = {Sample24, Sample21, Sample22, Sample20};
const unsigned int  *SoundLengths[MAX_SOUND_NUMBER] = {&Sample24Size, &Sample21Size, &Sample22Size, &Sample20Size};

void InitSound(void)
{
    sndIndex = 0;
    
    pSoundArray = Sounds[Config.SampleNum];
    sndLength   = *(SoundLengths[Config.SampleNum]);

    sndFinished = true;
}

void SoundIntHandler(void)
{
    SoundPWM_SetValue(NextSoundByte);
    SoundPWM_Start();

    // Play Next Sound Sample
    if (!sndFinished)
    {
        // Set Match Value
        NextSoundByte = pSoundArray[sndIndex]; // Calc Volume
        NextSoundByte *= CurVolume;
        NextSoundByte /= SOUND_PWM_MAXVOLUME;
        
        sndIndex++;
        if (sndIndex >= sndLength)
        {
            sndIndex = 0;

            // SoundPWM Stop
            SoundPWM_Reset();

            NextSoundByte = 0;
            sndFinished = true;
        }
    }
    else
    {
        // SoundPWM Stop
        SoundPWM_Reset();
        NextSoundByte = 0;
    }
}

void StartSound(void)
{
    sndIndex = 0;

    pSoundArray = Sounds[Config.SampleNum];
    sndLength   = *(SoundLengths[Config.SampleNum]);

    sndFinished = false;

    // TMR2 Start
    Timer2_Start();
}

void StopSound(void)
{
    // TMR2 Stop
//    Timer2_Stop();
    Timer2_SetValue(0);
    sndFinished = true;
}
