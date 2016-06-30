/*----------------------------------------------------------------------------*/
//
// $Title:          I2C.h
// $Description:    I2C Module Interface.
//
// $Rev:            0.0.3
//
// $Author:         EEA
// $Created:        17.10.11
// $Last Modified:  30.11.11
//
// $Copyright:      EEA
//
// $Notes:
//
/*----------------------------------------------------------------------------*/
#ifndef __I2C_H
#define __I2C_H

#include "EEA_port.h"

#define I2C_WRITE_CMD   0x00  ///<I2C write command
#define I2C_READ_CMD    0x01  ///<I2C read command

typedef enum
{
    I2C_RES_OK,
    I2C_RES_NO_DEVICE,
    I2C_RES_DEVICE_BUSY,
    I2C_RES_BUS_ERROR
} TI2C_Result;
/*----------------------------------------------------------------------------*/
void I2C_Init(void);
//TI2C_Result I2C_ReadData(uint8 DevAddr, uint8 StartAddr, uint16 Size, uint8* pData);
//TI2C_Result I2C_WriteData(uint8 DevAddr, uint8 StartAddr, uint16 Size, uint8* pData);
TI2C_Result I2C_ReadData(uint8 DevAddr, uint8 AddrSize, uint16 StartAddr, uint16 Size, uint8* pData);
TI2C_Result I2C_WriteData(uint8 DevAddr, uint8 AddrSize, uint16 StartAddr, uint16 Size, uint8* pData);
uint8 I2C_Start(void);
uint8 I2C_Stop(void);
uint8 I2C_Read(uint8 *pData8, uint8 Ack);
uint8 I2C_Write(uint8 Data8);

#endif