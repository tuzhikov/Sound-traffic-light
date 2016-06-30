/*----------------------------------------------------------------------------*/
//
// $Title:          CommProtocol.c
// $Description:    Communication Protocol Interface
//
// $Rev:            0.0.10
//
// $Author:         EEA
// $Created:        9.10.11
// $Last Modified:  5.12.13
//
// $Copyright:      EEA
//
// $Notes:
//
/*----------------------------------------------------------------------------*/
#include "CommProtocol.h"

#include "EEA_Port.h"
#include "UART.h"
#include "DataTable.h"
#include "CRC.h"
#include "RTC.h"
#include "DateTime.h"
#include "Shedule.h"
#include "Flash.h"
#include "LEDs.h"
#include "LightAutomata.h"

uint16 SizeIn;
uint16 SizeOut;
uint8 *pDataIn;
uint8 *pDataOut;

typedef enum
{
    CMD_READ_STATE       = 0x00,
    CMD_SET_TIME         = 0x01,
    CMD_READ_SETTINGS    = 0x02,
    CMD_WRITE_SETTINGS   = 0x03,
    CMD_READ_DAY_PLAN    = 0x04,
    CMD_WRITE_DAY_PLAN   = 0x05,
    CMD_READ_WEEK        = 0x06,
    CMD_WRITE_WEEK       = 0x07,
    CMD_READ_HOLIDAYS    = 0x08,
    CMD_WRITE_HOLIDAYS   = 0x09,
    CMD_READ_INFO        = 0x0C,
    CMD_RESET_TO_DEFAULT = 0x0D,
    CMD_READ_FLASH_INFO  = 0x0E,
    CMD_FORMAT_FLASH     = 0x0F,
    CMD_READ_SETTINGS2   = 0x12,
    CMD_WRITE_SETTINGS2  = 0x13
} TMasterCommand;

typedef enum
{
    NO_ERRORS,
    ERR_UNKNOWN_CMD,
    ERR_WRONG_DATA_SIZE,
    ERR_WRONG_DATA_VALUE,
    ERR_FLASH_SAVE_FAILED
}TCommErrorCode;

const uint8 ResetCodeWord[]  = "RESET";
const uint8 FormatCodeWord[] = "FORMAT";

bool ProcessMasterCommand(uint8 *pDataIn, uint16 SizeIn, uint8 *pDataOut, uint16 *SizeOut);
uint8 SetErrorPacket(uint8 *pBuf, TMasterCommand cmd, uint8 ErrorCode);

void CheckMasterCommand(void)
{
    uint16 CRC=0;

    pDataIn = UART_Receive(&SizeIn);

    if (pDataIn != NULL)
    {
        pDataOut = UART_GetSendBuf();

        pDataOut[0] = UART_SA_CHAR;

        if(ProcessMasterCommand(pDataIn, SizeIn, pDataOut+1, &SizeOut) )
        {


            CRC = CalcCRC16(&pDataOut[1], SizeOut);
            SizeOut++;
            pDataOut[SizeOut++] = CRC & 0xFF;
            pDataOut[SizeOut++] = CRC >> 8;

            UART_SendAnswer(SizeOut);
        }
    }
}

void SendSlaveCommand(void)
{
    uint8 *pBuf;
    uint8 *pCmd;
    int i;
    uint16 CRC=0;

    pBuf = UART_GetCmdBuf();
    pCmd = (uint8*)&SlaveCommand;

    pBuf[0] = UART_SC_CHAR;

    for (i=0; i<sizeof(TSlaveCommand); i++)
    {
        pBuf[i+1] = pCmd[i];
    }

    CRC = CalcCRC16(&pBuf[1], i);

    i++;
    pBuf[i++] = CRC & 0xFF;
    pBuf[i++] = CRC >> 8;

    UART_SendSlaveCmd(i);
}

bool ProcessMasterCommand(uint8 *pDataIn, uint16 SizeIn, uint8 *pDataOut, uint16 *pSizeOut)
{
    TMasterCommand cmd;
    uint8 NumBytes;
    uint8 Num;
    uint8 NumAct;
    uint8 *ptr;
    bool res;
    int  i,j;

    cmd = (TMasterCommand)pDataIn[0];
    NumBytes = pDataIn[1];

    res = false;

    switch (cmd)
    {
        case CMD_READ_STATE:
        {
            // Check
            if (NumBytes)
            {
                // Send Error
                *pSizeOut = SetErrorPacket(pDataOut, cmd, ERR_WRONG_DATA_SIZE);
                res = true;
                break;
            }

            // Process
            // Make Answer
            pDataOut[0] = cmd;

            ptr = (uint8*)&Status;
            for (i=0; i<sizeof(TStatus); i++)
            {
                pDataOut[i+2] = ptr[i];
            }

            pDataOut[1] = i;
            *pSizeOut = i+2;

            res = true;
            break;
        };

        case CMD_SET_TIME:
        {
            // Check
            if (NumBytes != sizeof(TDateTime))
            {
                // Send Error
                *pSizeOut = SetErrorPacket(pDataOut, cmd, ERR_WRONG_DATA_SIZE);
                res = true;
                break;
            }

            TDateTime *pDT;
            pDT = (TDateTime*)&pDataIn[2];

            if ( !IsDateTimeOK(pDT) )
            {
                // Send Error
                *pSizeOut = SetErrorPacket(pDataOut, cmd, ERR_WRONG_DATA_VALUE);
                res = true;
                break;
            }

            // Process
            Set_DateTime(pDT);
            RTC_SetTime(pDT);
            FindDayPlan();
            ProcessShedule();

            // Make Answer
            pDataOut[0] = cmd;
            pDataOut[1] = 0;

            *pSizeOut = 2;

            res = true;
            break;
        };

        case CMD_READ_SETTINGS:
        {
            // Check
            if (NumBytes)
            {
                // Send Error
                *pSizeOut = SetErrorPacket(pDataOut, cmd, ERR_WRONG_DATA_SIZE);
                res = true;
                break;
            }

            // Process
            // Make Answer
            pDataOut[0] = cmd;

            ptr = (uint8*)&Config;
            for (i=0; i<sizeof(TConfig); i++)
            {
                pDataOut[i+2] = ptr[i];
            }

            pDataOut[1] = i;
            *pSizeOut = i+2;

            res = true;
            break;
        };

        case CMD_WRITE_SETTINGS:
        {
            // Check
            if (NumBytes != sizeof(TConfig))
            {
                // Send Error
                *pSizeOut = SetErrorPacket(pDataOut, cmd, ERR_WRONG_DATA_SIZE);
                res = true;
                break;
            }

            //// Process
            if ( CheckConfig(((TConfig*)&pDataIn[2])) )
            {
                // Copy Data
                ptr = (uint8*)&Config;
                for (i=0; i<sizeof(TConfig); i++)
                {
                    ptr[i] = pDataIn[i+2];
                }

                if (!Flash_SaveSettings())
                {
                    // Send Error
                    *pSizeOut = SetErrorPacket(pDataOut, cmd, ERR_FLASH_SAVE_FAILED);
                    res = true;
                    break;
                }
            }
            else
            {
                // Send Error
                *pSizeOut = SetErrorPacket(pDataOut, cmd, ERR_WRONG_DATA_VALUE);
                res = true;
                break;
            }

            // Make Answer
            pDataOut[0] = cmd;
            pDataOut[1] = 0;

            //if ( Flash_SaveSettings() )
            //    pDataOut[1] = 0;
            //else
            //    pDataOut[1] = 0xFF;

            *pSizeOut = 2;

            res = true;
            break;
        };

        case CMD_READ_SETTINGS2:
        {
            // Check
            if (NumBytes)
            {
                // Send Error
                *pSizeOut = SetErrorPacket(pDataOut, cmd, ERR_WRONG_DATA_SIZE);
                res = true;
                break;
            }

            // Process
            // Make Answer
            pDataOut[0] = cmd;

            ptr = (uint8*)&Config2;
            for (i=0; i<sizeof(TConfig2); i++)
            {
                pDataOut[i+2] = ptr[i];
            }

            pDataOut[1] = i;
            *pSizeOut = i+2;

            res = true;
            break;
        };

        case CMD_WRITE_SETTINGS2:
        {
            // Check
            if (NumBytes != sizeof(TConfig2))
            {
                // Send Error
                *pSizeOut = SetErrorPacket(pDataOut, cmd, ERR_WRONG_DATA_SIZE);
                res = true;
                break;
            }

            //// Process
            if ( CheckConfig2(((TConfig2*)&pDataIn[2])) )
            {
                // Copy Data
                ptr = (uint8*)&Config2;
                for (i=0; i<sizeof(TConfig2); i++)
                {
                    ptr[i] = pDataIn[i+2];
                }

                if (!Flash_SaveSettings())
                {
                    // Send Error
                    *pSizeOut = SetErrorPacket(pDataOut, cmd, ERR_FLASH_SAVE_FAILED);
                    res = true;
                    break;
                }
            }
            else
            {
                // Send Error
                *pSizeOut = SetErrorPacket(pDataOut, cmd, ERR_WRONG_DATA_VALUE);
                res = true;
                break;
            }

            // Make Answer
            pDataOut[0] = cmd;
            pDataOut[1] = 0;

            //if ( Flash_SaveSettings() )
            //    pDataOut[1] = 0;
            //else
            //    pDataOut[1] = 0xFF;

            *pSizeOut = 2;

            res = true;
            break;
        };

        case CMD_READ_DAY_PLAN:
        {
            // Check
            Num = pDataIn[2];

            if (NumBytes != 1)
            {
                // Send Error
                *pSizeOut = SetErrorPacket(pDataOut, cmd, ERR_WRONG_DATA_SIZE);
                res = true;
                break;
            }

            if (Num >= MAX_DAY_PLANS)
            {
                // Send Error
                *pSizeOut = SetErrorPacket(pDataOut, cmd, ERR_WRONG_DATA_VALUE);
                res = true;
                break;
            }

            // Process
            NumAct = DayPlans[Num].ActiveItems;
            //DEBUG+
            //if (NumAct>4)
            //    NumAct=0;
            //DEBUG-
            // Make Answer
            pDataOut[0] = cmd;
            pDataOut[2] = Num;
            pDataOut[3] = NumAct;

            *pSizeOut = 4;

            ptr = (uint8*)&DayPlans[Num].Items[0];

            for (i=0; i < (NumAct*sizeof(TDayPlanItem)); i++)
            {
                pDataOut[(*pSizeOut)++] = *ptr++;
            }

            pDataOut[1] = *pSizeOut-2;

            res = true;
            break;
        };

        case CMD_WRITE_DAY_PLAN:
        {
            // Check
            Num    = pDataIn[2];
            NumAct = pDataIn[3];

            //DEBUG+
            //if (NumAct>4)
            //    NumAct=0;
            //DEBUG-

            if (NumBytes != (sizeof(TDayPlanItem)*NumAct+2))
            {
                // Send Error
                *pSizeOut = SetErrorPacket(pDataOut, cmd, ERR_WRONG_DATA_SIZE);
                res = true;
                break;
            }

            if ( (Num >= MAX_DAY_PLANS) || (NumAct > MAX_DAY_PLAN_ITEMS) )
            {
                // Send Error
                *pSizeOut = SetErrorPacket(pDataOut, cmd, ERR_WRONG_DATA_VALUE);
                res = true;
                break;
            }

            // Process
            if ( CheckDayPlan( (TDayPlan*)&pDataIn[4], NumAct ) )
            {
                // Copy Data
                ptr = (uint8*)&DayPlans[Num].Items[0];
                for (i=0; i < (NumAct*sizeof(TDayPlanItem)); i++)
                {
                    *ptr++ = pDataIn[i+4];
                }

                for (; i < (MAX_DAY_PLAN_ITEMS*sizeof(TDayPlanItem)); i++)
                {
                    *ptr++ = (uint8)-1;
                }

                DayPlans[Num].ActiveItems = NumAct;

                if ( !Flash_SaveDayPlan(Num) )
                {
                    // Send Error
                    *pSizeOut = SetErrorPacket(pDataOut, cmd, ERR_FLASH_SAVE_FAILED);
                    res = true;
                    break;
                }
            }
            else
            {
                // Send Error
                *pSizeOut = SetErrorPacket(pDataOut, cmd, ERR_WRONG_DATA_VALUE);
                res = true;
                break;
            }

            // Make Answer
            pDataOut[0] = cmd;
            pDataOut[2] = Num;
            pDataOut[3] = DayPlans[Num].ActiveItems;

            *pSizeOut = 4;

            pDataOut[1] = *pSizeOut-2;

            res = true;
            break;
        };

        case CMD_READ_WEEK:
        {
            // Check
            if (NumBytes)
            {
                // Send Error
                *pSizeOut = SetErrorPacket(pDataOut, cmd, ERR_WRONG_DATA_SIZE);
                res = true;
                break;
            }

            // Process
            // Make Answer
            pDataOut[0] = cmd;

            ptr = (uint8*)&WeekPlans;
            for (i=0; i<sizeof(WeekPlans); i++)
            {
                pDataOut[i+2] = ptr[i];
            }

            pDataOut[1] = i;
            *pSizeOut = i+2;

            res = true;
            break;
        };

        case CMD_WRITE_WEEK:
        {
            // Check
            if (NumBytes != sizeof(WeekPlans))
            {
                // Send Error
                *pSizeOut = SetErrorPacket(pDataOut, cmd, ERR_WRONG_DATA_SIZE);
                res = true;
                break;
            }

            //// Process
            if ( CheckWeek(((int8*)&pDataIn[2])) )
            {
                // Copy Data
                ptr = (uint8*)&WeekPlans;
                for (i=0; i<sizeof(WeekPlans); i++)
                {
                    ptr[i] = pDataIn[i+2];
                }

                if (!Flash_SaveSettings())
                {
                    // Send Error
                    *pSizeOut = SetErrorPacket(pDataOut, cmd, ERR_FLASH_SAVE_FAILED);
                    res = true;
                    break;
                }
            }
            else
            {
                // Send Error
                *pSizeOut = SetErrorPacket(pDataOut, cmd, ERR_WRONG_DATA_VALUE);
                res = true;
                break;
            }
            // Make Answer
            pDataOut[0] = cmd;
            pDataOut[1] = 0;

            *pSizeOut = 2;

            res = true;
            break;
        };

        case CMD_READ_HOLIDAYS:
        {
            // Check
            if (NumBytes)
            {
                // Send Error
                *pSizeOut = SetErrorPacket(pDataOut, cmd, ERR_WRONG_DATA_SIZE);
                res = true;
                break;
            }

            // Process
            // Make Answer
            pDataOut[0] = cmd;

            NumAct = 0;

            *pSizeOut = 3;
            ptr = (uint8*)&Holidays;

            for (i=0; i<MAX_HOLIDAYS; i++)
            {
                if (Holidays[i].day == -1)
                {
                    break;
                }

                NumAct++;
                for (j=0; j<sizeof(THoliday); j++)
                {
                    pDataOut[(*pSizeOut)++] = *ptr++;
                }
            }

            pDataOut[2] = NumAct;
            pDataOut[1] = *pSizeOut-2;

            res = true;
            break;
        };

        case CMD_WRITE_HOLIDAYS:
        {
            // Check
            NumAct = pDataIn[2];

            if (NumBytes != (sizeof(THoliday)*NumAct+1))
            {
                // Send Error
                *pSizeOut = SetErrorPacket(pDataOut, cmd, ERR_WRONG_DATA_SIZE);
                res = true;
                break;
            }

            if (NumAct > MAX_HOLIDAYS)
            {
                // Send Error
                *pSizeOut = SetErrorPacket(pDataOut, cmd, ERR_WRONG_DATA_VALUE);
                res = true;
                break;
            }

            // Process
            if ( CheckHolidays( (THoliday*)&pDataIn[3], NumAct ) )
            {
                // Copy Data
                ptr = (uint8*)&Holidays;
                for (i=0; i < (NumAct*sizeof(THoliday)); i++)
                {
                    *ptr++ = pDataIn[i+3];
                }

                for (; i < (MAX_HOLIDAYS*sizeof(THoliday)); i++)
                {
                    *ptr++ = (uint8)-1;
                }

                if (!Flash_SaveSettings())
                {
                    // Send Error
                    *pSizeOut = SetErrorPacket(pDataOut, cmd, ERR_FLASH_SAVE_FAILED);
                    res = true;
                    break;
                }
            }
            else
            {
                // Send Error
                *pSizeOut = SetErrorPacket(pDataOut, cmd, ERR_WRONG_DATA_VALUE);
                res = true;
                break;
            }

            // Make Answer
            pDataOut[0] = cmd;
            pDataOut[2] = NumAct;

            *pSizeOut = 3;

            pDataOut[1] = *pSizeOut-2;

            res = true;
            break;
        };

        case CMD_READ_INFO:
        {
            // Check
            if (NumBytes)
            {
                // Send Error
                *pSizeOut = SetErrorPacket(pDataOut, cmd, ERR_WRONG_DATA_SIZE);
                res = true;
                break;
            }

            // Process
            // Make Answer
            pDataOut[0] = cmd;

            ptr = (uint8*)&SoftwareInfo;
            for (i=0; i<sizeof(TSoftwareInfo); i++)
            {
                pDataOut[i+2] = ptr[i];
            }

            pDataOut[1] = i;
            *pSizeOut = i+2;

            res = true;
            break;
        };

        case CMD_RESET_TO_DEFAULT:
        {
            // Check
            if (NumBytes != sizeof(ResetCodeWord))
            {
                // Send Error
                *pSizeOut = SetErrorPacket(pDataOut, cmd, ERR_WRONG_DATA_SIZE);
                res = true;
                break;
            }

            // Process
            for (i=0; i<sizeof(ResetCodeWord); i++)
            {
                if (pDataIn[i+2] != ResetCodeWord[i])
                {
                    // Send Error
                    *pSizeOut = SetErrorPacket(pDataOut, cmd, ERR_WRONG_DATA_VALUE);
                    res = true;
                    break;
                }
            }

            InitAutomatas();
            InitData();
            InitLightSensor();

            if ( !Flash_SaveAll() )
            {
                // Send Error
                *pSizeOut = SetErrorPacket(pDataOut, cmd, ERR_FLASH_SAVE_FAILED);
                res = true;
                break;
            }

            if ( !Flash_SaveState() )
            {
                // Send Error
                *pSizeOut = SetErrorPacket(pDataOut, cmd, ERR_FLASH_SAVE_FAILED);
                res = true;
                break;
            }

            // Make Answer
            pDataOut[0] = cmd;
            pDataOut[1] = 0;

            *pSizeOut = 2;

            res = true;
            break;
        };

        case CMD_READ_FLASH_INFO:
        {
            // Check
            if (NumBytes)
            {
                // Send Error
                *pSizeOut = SetErrorPacket(pDataOut, cmd, ERR_WRONG_DATA_SIZE);
                res = true;
                break;
            }

            // Process
            // Make Answer
            pDataOut[0] = cmd;

            ptr = (uint8*)&PublicFlashInfo;
            for (i=0; i<sizeof(TPublicFlashInfo); i++)
            {
                pDataOut[i+2] = ptr[i];
            }

            pDataOut[1] = i;
            *pSizeOut = i+2;

            res = true;
            break;
        };

        case CMD_FORMAT_FLASH:
        {
            // Check
            if (NumBytes != sizeof(FormatCodeWord))
            {
                // Send Error
                *pSizeOut = SetErrorPacket(pDataOut, cmd, ERR_WRONG_DATA_SIZE);
                res = true;
                break;
            }

            // Process
            for (i=0; i<sizeof(FormatCodeWord); i++)
            {
                if (pDataIn[i+2] != FormatCodeWord[i])
                {
                    // Send Error
                    *pSizeOut = SetErrorPacket(pDataOut, cmd, ERR_WRONG_DATA_VALUE);
                    res = true;
                    break;
                }
            }

            if ( !FormatFlash() )
            {
                // Send Error
                *pSizeOut = SetErrorPacket(pDataOut, cmd, ERR_FLASH_SAVE_FAILED);
                res = true;
                break;
            }

            if ( !Flash_SaveState() )
            {
                // Send Error
                *pSizeOut = SetErrorPacket(pDataOut, cmd, ERR_FLASH_SAVE_FAILED);
                res = true;
                break;
            }

            if ( !Flash_SaveAll() )
            {
                // Send Error
                *pSizeOut = SetErrorPacket(pDataOut, cmd, ERR_FLASH_SAVE_FAILED);
                res = true;
                break;
            }

            // Make Answer
            pDataOut[0] = cmd;
            pDataOut[1] = 0;

            *pSizeOut = 2;

            res = true;
            break;
        };

        default:
        {
            // Send Error
            *pSizeOut = SetErrorPacket(pDataOut, cmd, ERR_UNKNOWN_CMD);
            res = true;
            break;
        }
    }

    return res;
}

uint8 SetErrorPacket(uint8 *pBuf, TMasterCommand cmd, uint8 ErrorCode)
{
    pBuf[0] = cmd|0x80;
    pBuf[1] = 1;
    pBuf[2] = ErrorCode;

    return 3;
}