//---------------------------------------------------------------------------
//
// $Title:          DemoMode.h
// $Description:    Demo Mode Interface.
//
// $Rev:            0.0.1
//
// $Author:         EEA
//
// $Created:        20.10.11
// $Last Modified:  20.10.11
//
// $Copyright:      EEA
//
// $Notes:
//
//---------------------------------------------------------------------------
#ifndef __DEMOMODE_H
#define __DEMOMODE_H

//------------------------------------------------
// DEMO AUTOMATA
//------------------------------------------------
#define DEMO_DEF_TIME            3
#define DEMO_COUNTBACK_START_NUM 7
#define DEMO_SPEEDUP_TIME        3

#define DEMO_DEF_BRIGHT        100
#define DEMO_TIME_BRIGHT       30

#define DEMO_FADE_TIME         2000
#define DEMO_FADE_NUM          4

typedef enum
{
    DEMO_TIME,
    DEMO_RED_BRIGHT,
    DEMO_GREEN_BRIGHT,
    DEMO_END,
    DEMO_GREENBACK_SND,
    DEMO_FADE_BRIGHT,
    DEMO_YELLOW1,
    DEMO_REDBACK,
    DEMO_YELLOW2,
    DEMO_RED,
    DEMO_GREENBACK,
    DEMO_DUMMY
} TDemoState;

extern TDemoState DemoState;

//-----------------------------

void DemoAutomata(void);
void EnterDemoMode(void);
void ExitDemoMode(void);

void Demo_MakeSlaveCommand(void);


#endif