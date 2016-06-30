//---------------------------------------------------------------------------
//
// $Title:          Test.h
// $Description:    Test Mode Interface
//                  
// $Rev:            0.0.9
//
// $Author:         EEA
// $Created:        26.10.11
// $Last Modified:  28.12.13
//
// $Copyright:      EEA
//
// $Notes:          
//
//---------------------------------------------------------------------------
#ifndef __TEST_H
#define __TEST_H

#define TEST_RED_TIME            3
#define TEST_GREEN_TIME          3
#define TEST_RED_START_NUM     110
#define TEST_RED_DEC_NUM        11
#define TEST_GREEN_START_NUM    99
#define TEST_GREEN_DEC_NUM      11

#define TEST_CLOCK_MODE_TIME    10
                                
#define TEST_DEF_BRIGHT          2
#define TEST_DEF_VOLUME         20

typedef enum
{
    TESTRG_START,
    TESTRG_RED_PAUSE,
    TESTRG_CALC_LOW_VOLT,
    TESTRG_CALC_HIGH_VOLT,
    TESTRG_CALC_LSENS_VOLT,
    TESTR_PAUSE4,
    TESTRG_CHECK_FLASH,
    TESTRG_CHECK_RTC,
    TESTR_PAUSE3,
    TESTR_VERSION_H,
    TESTR_PAUSE2,
    TESTR_VERSION_L,
    TESTRG_END,

    TESTR_START,
    TESTR_COUNT,
    //TESTR_PAUSE1,
    //TESTR_VERSION_H,
    //TESTR_PAUSE2,
    //TESTR_VERSION_L,
    //TESTR_PAUSE3,
    TESTR_END,

    TESTG_START,
    TESTG_COUNT,
    TESTG_END,
   
    TEST_START,
    TEST_RED_COUNT,
    TEST_GREEN_COUNT,
    TEST_RED,
    TEST_GREEN,
    TEST_TIME,
    TEST_END,
    TESTR_PAUSE1,
    TEST_CLOCK,
    TEST_DUMMY
} TTestState;

extern TTestState TestState;

void InitTest(void);

void TestAutomata(void);
void Test_MakeSlaveCommand(void);


#endif