/*----------------------------------------------------------------------------*/
//
// $Title:          MiniMP3.h
// $Description:    Mini mp3 Interface.
//
// $Rev:            0.0.1
//
// $Author:         ATA
// $Created:        23.06.16
// $Last Modified:
//
// $Copyright:
//
// $Notes:
//
/*----------------------------------------------------------------------------*/
#ifndef __MINI_MP3_H
#define __MINI_MP3_H

#include "EEA_port.h"
// answer
typedef enum{
    ERROR = -1,
    OK,
    DELAY,
    NO_ANSWER
}TypeAnswer;
// step mashine
typedef enum{
    ONE,
    TWO,
    THREE,
    FOUR,
    FIVE,
    SIX,
    SEVEN
}TypeStep;

/*----------------------------------------------------------------------------*/
void machineMP3(void);
void disabledMode(void);

#endif