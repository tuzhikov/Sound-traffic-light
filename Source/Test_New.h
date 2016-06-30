//---------------------------------------------------------------------------
//
// $Title:          Test.h
// $Description:    Test Mode Interface
//                  
// $Rev:            0.0.2
//
// $Author:         EEA
// $Created:        26.10.11
// $Last Modified:  6.11.11
//
// $Copyright:      EEA
//
// $Notes:          
//
//---------------------------------------------------------------------------
#ifndef __TEST_H
#define __TEST_H

#include "EEA_Port.h"

#define TEST_RED_TIME            5
#define TEST_GREEN_TIME          5
#define TEST_RED_START_NUM     121
#define TEST_RED_DEC_NUM        11
#define TEST_GREEN_START_NUM    99
#define TEST_GREEN_DEC_NUM      11

#define TEST_CLOCK_MODE_TIME    20

#define TEST_DEF_BRIGHT        100
#define TEST_DEF_VOLUME        100

typedef enum
{
    TESTRG_CALC_LOW_VOLT,
    TESTRG_CALC_HIGH_VOLT,
    TESTRG_CHECK_FLASH,
    TESTRG_CHECK_RTC,
    TESTRG_RED_PAUSE,

    TEST_RED,
    TEST_GREEN,

    TEST_RED_COUNT,
    TEST_GREEN_COUNT,

    TEST_TIME,

    TESTG,
    TESTR,

    TEST_END,
    TEST_DUMMY
} TTestState;

extern TTestState TestState;

void InitTest(void);

void TestAutomata(void);
void Test_MakeSlaveCommand(void);


#endif