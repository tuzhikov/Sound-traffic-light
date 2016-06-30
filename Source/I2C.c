/*----------------------------------------------------------------------------*/
//
// $Title:          I2C.c
// $Description:    I2C Functions and Data.
//
// $Rev:            0.0.5
//
// $Author:         EEA
// $Created:        17.10.11
// $Last Modified:  5.12.13
//
// $Copyright:      EEA
//
// $Notes:
//
/*----------------------------------------------------------------------------*/
#include <NXP/iolpc2103.h>
#include "I2C.h"
#include "Config.h"

#define I2C_BAUDRATE 400000UL

#define I2C_TIMEOUT  30000

#define I2C_AA_FLAG     0x04
#define I2C_SI_FLAG     0x08
#define I2C_STO_FLAG    0x10
#define I2C_STA_FLAG    0x20
#define I2C_I2EN_FLAG   0x40
/*----------------------------------------------------------------------------*/
typedef enum
{
    I2C_BUS_ERROR       = 0x00,
    I2C_START           = 0x08,
    I2C_REP_START       = 0x10,
    I2C_MT_SLA_ACK      = 0x18,
    I2C_MT_SLA_NACK     = 0x20,
    I2C_MT_DATA_ACK     = 0x28,
    I2C_MT_DATA_NACK    = 0x30,
    I2C_MT_ARBITR_LOSS  = 0x38, //?
    I2C_MR_SLA_ACK      = 0x40,
    I2C_MR_SLA_NACK     = 0x48,
    I2C_MR_DATA_ACK     = 0x50, //?
    I2C_MR_DATA_NACK    = 0x58  //?
} TI2C_States;
/*----------------------------------------------------------------------------*/
TI2C_Result I2C_ReadData(uint8 DevAddr, uint8 AddrSize, uint16 StartAddr, uint16 Size, uint8* pData)
{
    int i;
    TI2C_States st;
/*
    I2C_Start();

    I2C_Write(DevAddr|I2C_WRITE_CMD);

    if (AddrSize==2)
        I2C_Write(StartAddr>>8);

    I2C_Write(StartAddr&0xFF);

    I2C_Start();

    I2C_Write(DevAddr|I2C_READ_CMD);

    for (i=0; i<Size-1; i++)
    {
        I2C_Read(pData+i, 1);
    }

    I2C_Read(pData+i, 0);

    I2C_Stop();

    return I2C_RES_OK;
*/
/**/
    //I2C_Stop();
    st = (TI2C_States)I2C_Start();
    if (st != I2C_START)
    {
        I2C_Stop();
        return I2C_RES_BUS_ERROR;
    }


    st = (TI2C_States)I2C_Write(DevAddr|I2C_WRITE_CMD);
    switch (st)
    {
        case I2C_MT_SLA_ACK:
            break;
        case I2C_MT_SLA_NACK:
        I2C_Stop();
            return I2C_RES_NO_DEVICE;
        default:
        I2C_Stop();
            return I2C_RES_BUS_ERROR;
    }

    if (AddrSize == 2)
    {
        st = (TI2C_States)I2C_Write(StartAddr>>8);
        switch (st)
        {
            case I2C_MT_DATA_ACK:
                break;
            case I2C_MT_DATA_NACK:
        I2C_Stop();
                return I2C_RES_DEVICE_BUSY;
            default:
        I2C_Stop();
                return I2C_RES_BUS_ERROR;
        }
    }

    st = (TI2C_States)I2C_Write(StartAddr&0xFF);
    switch (st)
    {
        case I2C_MT_DATA_ACK:
            break;
        case I2C_MT_DATA_NACK:
        I2C_Stop();
            return I2C_RES_DEVICE_BUSY;
        default:
        I2C_Stop();
            return I2C_RES_BUS_ERROR;
    }

    st = (TI2C_States)I2C_Start();
    if (st != I2C_REP_START)
    {
        I2C_Stop();
        return I2C_RES_BUS_ERROR;
    }

    st = (TI2C_States)I2C_Write(DevAddr|I2C_READ_CMD);
    switch (st)
    {
        case I2C_MR_SLA_ACK:
            break;
        case I2C_MR_SLA_NACK:
            I2C_Stop();
            return I2C_RES_NO_DEVICE;
        default:
            I2C_Stop();
            return I2C_RES_BUS_ERROR;
    }

    Size--;
    i=0;
    do
    {
        I2C_Read(pData+i,Size-i);
        i++;
    }while (i<=Size);

    I2C_Stop();

    return I2C_RES_OK;
/**/
}
/*----------------------------------------------------------------------------*/
TI2C_Result I2C_WriteData(uint8 DevAddr, uint8 AddrSize, uint16 StartAddr, uint16 Size, uint8* pData)
{
    int i;
    TI2C_States st;
/*
    I2C_Start();

    I2C_Write(DevAddr|I2C_WRITE_CMD);

    if (AddrSize==2)
        I2C_Write(StartAddr>>8);

    I2C_Write(StartAddr&0xFF);

    for (i=0; i<Size; i++)
    {
        I2C_Write(pData[i]);
    }

    I2C_Stop();

    return I2C_RES_OK;
*/
/**/
    //I2C_Stop();
    st = (TI2C_States)I2C_Start();
    if (st != I2C_START)
    {
        I2C_Stop();
        return I2C_RES_BUS_ERROR;
    }

    st = (TI2C_States)I2C_Write(DevAddr|I2C_WRITE_CMD);
    switch (st)
    {
        case I2C_MT_SLA_ACK:
            break;
        case I2C_MT_SLA_NACK:
        I2C_Stop();
            return I2C_RES_NO_DEVICE;
        default:
        I2C_Stop();
            return I2C_RES_BUS_ERROR;
    }

    if (AddrSize == 2)
    {
        st = (TI2C_States)I2C_Write(StartAddr>>8);
        switch (st)
        {
            case I2C_MT_DATA_ACK:
                break;
            case I2C_MT_DATA_NACK:
        I2C_Stop();
                return I2C_RES_DEVICE_BUSY;
            default:
        I2C_Stop();
                return I2C_RES_BUS_ERROR;
        }
    }

    st = (TI2C_States)I2C_Write(StartAddr&0xFF);
    switch (st)
    {
        case I2C_MT_DATA_ACK:
            break;
        case I2C_MT_DATA_NACK:
        I2C_Stop();
            return I2C_RES_DEVICE_BUSY;
        default:
        I2C_Stop();
            return I2C_RES_BUS_ERROR;
    }

    i=0;
    do
    {
        st = (TI2C_States)I2C_Write(pData[i++]);
        if ( st != I2C_MT_DATA_ACK )
        {
            I2C_Stop();
            return I2C_RES_DEVICE_BUSY;
        }
    } while (i<Size);

    I2C_Stop();

    return I2C_RES_OK;
/**/
}
///*inline*/ uint8 I2C_Wait(void)
//{
//    I2C0CONCLR = I2C_SI_FLAG;
//    while( ! (I2C0CONSET & I2C_SI_FLAG) );
//    return I2C0STAT;
//}
/*----------------------------------------------------------------------------*/
void I2C_Init(void)
{
    I2C0CONCLR = 0x6C;
    I2C0CONSET = I2C_I2EN_FLAG|I2C_STO_FLAG;
    //I2C0STAT;
    //I2C0DAT;
    I2C0SCLH   = (PCCLK/(I2C_BAUDRATE*2));
    I2C0SCLL   = (PCCLK/I2C_BAUDRATE) - (PCCLK/(I2C_BAUDRATE*2));
}
/*----------------------------------------------------------------------------*/
uint8 I2C_Start(void)
{
    uint32 TimeLeft = I2C_TIMEOUT;

    I2C0CONSET = I2C_STA_FLAG;
    I2C0CONCLR = I2C_SI_FLAG;
    while( ! (I2C0CONSET & I2C_SI_FLAG) && TimeLeft )
    {
        // Wait
        TimeLeft--;
    }

    I2C0CONCLR = I2C_STA_FLAG;
    return I2C0STAT;
}
/*----------------------------------------------------------------------------*/
uint8 I2C_Stop(void)
{
    uint32 TimeLeft = I2C_TIMEOUT;

    I2C0CONSET = I2C_STO_FLAG;
//    I2C0CONCLR = I2C_SI_FLAG;
    while( ! (I2C0CONSET & I2C_SI_FLAG) && TimeLeft )
    {
        // Wait
        TimeLeft--;
    }

    return I2C0STAT;
}
/*----------------------------------------------------------------------------*/
uint8 I2C_Read(uint8 *pData8, uint8 Ack)
{
    uint32 TimeLeft = I2C_TIMEOUT;

    if (Ack)
        I2C0CONSET = I2C_AA_FLAG;
    //else
    //    I2C0CONCLR = I2C_AA_FLAG;

    I2C0CONCLR = I2C_SI_FLAG;
    while( ! (I2C0CONSET & I2C_SI_FLAG) && TimeLeft )
    {
        // Wait
        TimeLeft--;
    }

    I2C0CONCLR = I2C_AA_FLAG;

    *pData8 = I2C0DAT;

    return I2C0STAT;
}
/*----------------------------------------------------------------------------*/
uint8 I2C_Write(uint8 Data8)
{
    uint32 TimeLeft = I2C_TIMEOUT;

    I2C0DAT = Data8;
    I2C0CONCLR = I2C_SI_FLAG/*|I2C_STA_FLAG*/;
    while( ! (I2C0CONSET & I2C_SI_FLAG) && TimeLeft )
    {
        // Wait
        TimeLeft--;
    }

    return I2C0STAT;
}