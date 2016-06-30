//---------------------------------------------------------------------------
//
// $Title:          UART.h
// $Description:    UART interface.
//
// $Rev:            0.0.7
//
// $Author:         EEA
//
// $Created:        14.10.11
// $Last Modified:  10.2.12
//
// $Copyright:      EEA
//
// $Notes:
//
//---------------------------------------------------------------------------

#ifndef __UART_H
#define __UART_H

#include "EEA_Port.h"

#define UART_MP3_CHAR   0x7E
#define UART_SR_CHAR    'R'
#define UART_SA_CHAR    '@'
#define UART_SC_CHAR    '#'

#define UART_MASTER_ANSWER_DELAY        50
#define UART_CHAR_PAUSE                 5
#define UART_MASTER_CONNECTION_TIMEOUT  2000

//extern bool UART_bRxDataReady;
extern bool UART_bSlaveCmdEnable;

extern uint32 UART_CharPause;
extern uint32 UART_AnswerDelay;
extern uint32 UART_MasterConnectionTime;

void UART_Init(void);

uint8* UART_GetSendBuf(void);
uint8* UART_GetCmdBuf(void);

void UART_SendAnswer(uint16 Size);
void UART_SendSlaveCmd(uint16 Size);

uint8* UART_Receive(uint16 *pSize);
uint8* UART_ReceiveMp3(uint16 *pSize);
void sendBuffUART0(void *pBuff, const uint16 size);

#endif

