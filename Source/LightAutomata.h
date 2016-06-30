//---------------------------------------------------------------------------
//
// $Title:          LightAutomata.h
// $Description:    Light Automatas Interface.
//
// $Rev:            0.0.5
//
// $Author:         EEA
//
// $Created:        17.10.11
// $Last Modified:  30.1.12
//
// $Copyright:      EEA
//
// $Notes:
//
//---------------------------------------------------------------------------
#ifndef __LIGHTAUTOMATA_H
#define __LIGHTAUTOMATA_H

#include "EEA_port.h"
#include "DataTable.h"

#define G0_SWITCH_NUM       5
#define G1_SWITCH_NUM       5

#define R0_SWITCH_NUM       5
#define R1_SWITCH_NUM       5

#define GOOD_G_PULSES_MAX   3
#define GOOD_R_PULSES_MAX   3

#define BAD_G_PULSES_MAX    3
#define BAD_R_PULSES_MAX    3

#define TI_MIN     2
#define TP_MIN     2

#define TI_MAX     15
#define TP_MAX     15

#define T_SUM_MIN  18
#define T_SUM_MAX  22

#define TI_LIMIT   70
#define TP_LIMIT   70

#define TIME_MAX_DELTA    2000
#define TIME_JITTER_DELTA_SEC       1
#define TIME_JITTER_COUNTER_LIMIT  60
#define TIME_JITTER_ADD_VALUE      15
#define TIME_JITTER_SUB_VALUE       1

extern uint16 GState;
extern uint16 RState;

extern int16  GRoundUpValue;
extern int16  RRoundUpValue;

void InitAutomatas(void);

void CheckInputLines(uint32 PortData);

void GLineAutomata(void);
void RLineAutomata(void);
void GreenAutomata(void);
void RedAutomata(void);

void WorkAutomata(void);

void SwitchToGreen(void);
void SwitchToRed(void);
void SwitchToOff(void);

void UpdateAutomataStatus(void);

void ActivateState(TState *pState);
void FillState(TState *pState);

void MakeSlaveCommand(void);

#endif