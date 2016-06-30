//---------------------------------------------------------------------------
//
// $Title:          UART.c
// $Description:    UART Functions and Data.
//
// $Rev:            0.0.8
//
// $Author:         EEA
//
// $Created:        9.10.11
// $Last Modified:  10.2.12
//
// $Copyright:      EEA
//
// $Notes:
//
//---------------------------------------------------------------------------
#include <NXP/iolpc2103.h>
#include "UART.h"
#include "EEA_Port.h"
#include "Pins.h"
#include "DataTable.h"
#include "CRC.h"
#include "FastLoop.h"

#include "LEDs.h"


#define UART_RX_BUFFER_SIZE 128
#define UART_TX_BUFFER_SIZE 128

#define UART_CMD_BUFFER_SIZE 8

#define THRE_INT    0x02
#define RDA_INT     0x04
#define CTI_INT     0x0C
#define RLS_INT     0x06

#define IER_RBR     0x01
#define IER_THRE    0x02
#define IER_RLS     0x04


#define SLAVE_DATA_FRAME_SIZE 4

typedef enum
{
    RX_IDLE,
    RX_CMD,
    RX_NUM,
    RX_DATA,
    RX_END
} TRxState;

typedef enum
{
    TX_IDLE,
    TX_CMD,
    TX_ANSWER
} TTxState;

/*
typedef enum
{
    TX_IDLE,
    TX_START_A,
    TX_START_C,
    TX_INFO,
    TX_CMD,
    TX_NUM,
    TX_DATA,
    TX_CRC,
    TX_END
} TTxState;
*/

/*
115200    18   13  14   2.286
57600     36   13  14   2.286
38400     81    2   7   2.286
19200    125    2   3   0
9600     375    1   9   0
4800     625    1   3   0
2400    1250    1   3   0
1200    2500    1   3   0
600     5000    1   3   0
*/

uint8 RxBuf[UART_RX_BUFFER_SIZE];
uint8 TxBuf[UART_TX_BUFFER_SIZE];

uint8 CmdBuf[UART_CMD_BUFFER_SIZE];

bool UART_bRxDataReady;
bool UART_bSlaveCmdEnable;
uint32 UART_MasterConnectionTime;

uint32 UART_CharPause;
uint32 UART_AnswerDelay;

uint8 RxBufIndex;
uint8 TxBufIndex;
uint8 CmdBufIndex;

uint8 RxCounter;
uint8 TxCounter;
uint8 CmdCounter;

TTxState TxState;
TRxState RxState;

uint16 CRC;

// for MP3
static bool mp3Read = false;
/*----------------------------------------------------------------------------*/
//Prototypes
void UART_ResetRX(void);
void UART0_Handler(void);
/*----------------------------------------------------------------------------*/
void UART_Init(void)
{
    UART_bRxDataReady    = false;
    UART_bSlaveCmdEnable = true;

    UART_CharPause    = 0;
    UART_AnswerDelay  = 0;

    RxBufIndex  = 0;
    TxBufIndex  = 0;

    CmdBufIndex = 0;
    CmdCounter  = 0;

    TxCounter   = 0;
    TxState     = TX_IDLE;
    RxState     = RX_IDLE;

    U0LCR = 0x83;

    U0DLM = 0x01;
    U0DLL = 0x77;

    U0FDR = (9<<4) + 1; // 9600

    U0FCR = 0x00;
    U0LCR = 0x03;

    // Assign to IRQ
    VICIntSelect_bit.UART0 = 0;
    // Set interrupt slots
    VICVectAddr4 = (unsigned int)UART0_Handler;
    VICVectCntl4_bit.NUMBER = VIC_UART0;
    VICVectCntl4_bit.ENABLED = 1;
    // Timer 0 interrupt enable
    VICIntEnable_bit.UART0 = 1;
    U0IER = IER_RBR; // Enable RX Interrupt
}
/*----------------------------------------------------------------------------*/
void UART0_Handler(void)
{
    uint32 status;
    //TSlaveCommand *pSC;
    uint8  ch;

    status = U0IIR;

    if ( (RxState != RX_IDLE) && (RxState != RX_END) && !UART_CharPause )
    {
//        UART_ResetRX();
    }

    switch (status & 0x0F)
    {
        case THRE_INT:
        {
            switch (TxState)
            {
                case TX_CMD:
                {
                    if (CmdCounter)
                    {
                        U0THR = CmdBuf[CmdBufIndex++];
                        CmdCounter--;
                    }
                    //else if (TxCounter)
                    //{
                    //    TxState = TX_ANSWER;
                    //    U0THR = TxBuf[TxBufIndex++];
                    //    TxCounter--;
                    //}
                    else
                    {

                        TxState = TX_IDLE;
                        U0IER &= ~IER_THRE;
                    }

                    break;
                };
                case TX_ANSWER:
                {
                    //LED2_Toggle();

                    if (TxCounter)
                    {
                        U0THR = TxBuf[TxBufIndex++];
                        TxCounter--;
                    }
                    else
                    {
                        // Answer Send Finished
                        UART_ResetRX();
                        //if (CmdCounter)
                        //{
                        //    TxState = TX_CMD;
                        //    U0THR = CmdBuf[CmdBufIndex++];
                        //    CmdCounter--;
                        //}
                        //else
                        {
                            TxState = TX_IDLE;
                            U0IER &= ~IER_THRE;
                        }
                    }
                    break;
                };
                default:
                {
                    TxState = TX_IDLE;
                    U0IER &= ~IER_THRE;
                    break;
                };
            };

            break;
        }

        case RDA_INT:
        case CTI_INT:
        {
            if ( (RxState != RX_IDLE) && (RxState != RX_END) && !UART_CharPause )
            {
                UART_ResetRX();
            }

            while (U0LSR & 0x01)
            {
                ch = U0RBR;

                switch (RxState)
                {
                    case RX_IDLE:
                    {
                      if((ch == UART_SR_CHAR)||(ch == UART_MP3_CHAR))
                        {
                            // device ch
                            if(ch == UART_MP3_CHAR)
                                mp3Read = true;
                            else
                                mp3Read = false;

                            RxState = RX_CMD;
                            RxBufIndex = 0;

                            UART_bSlaveCmdEnable = false;
                            UART_MasterConnectionTime = UART_MASTER_CONNECTION_TIMEOUT;

                            UART_CharPause = UART_CHAR_PAUSE;
                            TxState = TX_ANSWER;
                                // AbortTx
                                U0IER &= ~IER_THRE;
                                TxState = TX_IDLE;
                            CmdCounter = 0;
                        }
                        break;
                    };

                    case RX_CMD:
                    {
                        RxBuf[RxBufIndex++] = ch;
                        RxState = RX_NUM;

                        UART_CharPause = UART_CHAR_PAUSE;
                        //UART_MasterConnectionTime = UART_MASTER_CONNECTION_TIMEOUT;
//        LED2_Toggle();
                        break;
                    };

                    case RX_NUM:
                    {
                        RxBuf[RxBufIndex++] = ch;

                        if(mp3Read)
                            RxCounter = ch+1; // нужно прин€ть дл€ MP3
                        else
                            RxCounter = ch+2; // нужнол прин€ть дл€ светофора

                        if ((RxBufIndex+RxCounter) > UART_RX_BUFFER_SIZE)
                        {
                            UART_ResetRX();
//             LED2_Toggle();
                        }
                        else
                        {
                            RxState = RX_DATA;
                        }

                        UART_CharPause = UART_CHAR_PAUSE;
                        //UART_MasterConnectionTime = UART_MASTER_CONNECTION_TIMEOUT;
                        break;
                    };

                    case RX_DATA:
                    {
                        RxBuf[RxBufIndex++] = ch;
                        RxCounter--;

                        if (!RxCounter)
                        {
                            if((!CalcCRC16(RxBuf, RxBufIndex))||(mp3Read))
                            {
                                    LED1_Toggle();
                                    UART_bRxDataReady = true;
                                    UART_AnswerDelay  = UART_MASTER_ANSWER_DELAY;
                                    // AbortTx
                                     U0IER &= ~IER_THRE;
                                     TxState = TX_IDLE;
                                     RxState = RX_END;
                            }
                            else
                            {
                                UART_ResetRX();
                            }
                        }

                        UART_CharPause = UART_CHAR_PAUSE;
                        //UART_MasterConnectionTime = UART_MASTER_CONNECTION_TIMEOUT;
                        break;
                    };

                    case RX_END:
                    {
                        // Do Nothing
                        break;
                    };

                    default:
                    {
                        RxState = RX_IDLE;
                        break;
                    };
                };
            };
            break;
        };

        case RLS_INT:
        {
            U0LSR;
 //            LED2_Toggle();
            break;
        };

        default:
            break;
    }

    VICVectAddr = 0;        // Clear Interrupt in VIC
}
/*----------------------------------------------------------------------------*/
void UART_ResetRX(void)
{
    RxState    = RX_IDLE;
    RxBufIndex = 0;
    UART_bRxDataReady = false;
}
/*----------------------------------------------------------------------------*/
uint8* UART_GetSendBuf(void)
{
    return TxBuf;
}
/*----------------------------------------------------------------------------*/
uint8* UART_GetCmdBuf(void)
{
    return CmdBuf;
}
/*----------------------------------------------------------------------------*/
void UART_SendSlaveCmd(uint16 Size)
{
    CmdBufIndex = 0;
    CmdCounter  = Size;

    if (CmdCounter)
    {
        if ( UART_bSlaveCmdEnable && (TxState == TX_IDLE) )
        {
            TxState = TX_CMD;
            U0THR   = CmdBuf[CmdBufIndex++];
            CmdCounter--;
        }
        U0IER |= IER_THRE;
    }
}
/*----------------------------------------------------------------------------*/
void UART_SendAnswer(uint16 Size)
{

    TxBufIndex = 0;
    TxCounter  = Size;

    if (TxCounter){
        TxState = TX_ANSWER;
        U0THR   = TxBuf[TxBufIndex++];
        TxCounter--;
        U0IER |= IER_THRE;
    }
}
/*----------------------------------------------------------------------------*/
void sendBuffUART0(void *pBuff, const uint16 size)
{
    uint8 *const pTemp = (uint8*)pBuff;
    uint8 *const pBufUart = UART_GetSendBuf();
    const uint16 leng = size>UART_TX_BUFFER_SIZE?UART_TX_BUFFER_SIZE:size;

    for(uint16 index = 0;index<leng;index++)
    {
        pBufUart[index] = pTemp[index];
    }
    UART_SendAnswer(leng);
}
/*----------------------------------------------------------------------------*/
uint8* UART_Receive(uint16 *pSize)
{
    if (UART_bRxDataReady && !UART_AnswerDelay && !mp3Read)
    {
        *pSize = RxBufIndex-2;
        UART_bRxDataReady = false;

        return RxBuf;
    }
    else
    {
        *pSize = 0;
        return NULL;
    }
}
/*----------------------------------------------------------------------------*/
uint8* UART_ReceiveMp3(uint16 *pSize)
{
    if(UART_bRxDataReady && !UART_AnswerDelay && mp3Read)
    {
        *pSize = RxBufIndex-2;
        UART_bRxDataReady = false;

        return RxBuf;
    }
    else
    {
        *pSize = 0;
        return NULL;
    }
}