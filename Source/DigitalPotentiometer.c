/*----------------------------------------------------------------------------*/
//
// $Title:          DigitalPotentiometer.c
// $Description:    DIGITAL POTENTIOMETER AD5242BR.
//
// $Rev:            0.0.1
//
// $Author:         ATA
// $Created:        22.06.16
// $Last Modified:
//
// $Copyright:
//
// $Notes:
//
/*----------------------------------------------------------------------------*/
#include "DigitalPotentiometer.h"
#include "I2C.h"
#include "System.h"
#include "Pins.h"
#include <intrinsics.h>

#define AD5242_MAX_POSITION 0xFF

typedef struct
{
    uint8 control; // Control Register
    uint8 data;    // data byte potentiometr
}TINFO_AD5242;

bool validAD5242;

/*----------------------------------------------------------------------------*/
/*Prototypes*/
static void enabledSHDN(bool portOn);
/*----------------------------------------------------------------------------*/
/**
* contpol pin SHDN Active LOW
* Port to HI if no use
*/
static void enabledSHDN(bool portOn)
{
    if (!portOn){
        if ( !(FIODIR & SHDN_CONTR_MASK) )
            FIODIR |= SHDN_CONTR_MASK;
    }else{
        if (FIODIR & SHDN_CONTR_MASK)
            FIODIR &= ~SHDN_CONTR_MASK;
    }
}
/**/
void initAD5242BR(void)
{
    validAD5242 = false;
    enabledSHDN(false);
}
/*
Read from
Return Result
*/
/*bool getTimeAD5242BR(const uint8 contr, uint8 *position)
{
    TI2C_Result res;
    TINFO_AD5242 infoAD5242={0};
    validAD5242 = false;
    infoAD5242.control = contr;

    //I2C_SetBaudRate(DS1338_BAUDRATE);
    //__disable_interrupt();
    res = I2C_ReadData(AD5242_DEVICE_ADD,1,0x00,sizeof(TINFO_AD5242),(uint8*)&infoAD5242);
    //__enable_interrupt();
    if (res == I2C_RES_OK){
        *position = infoAD5242.data;
        validAD5242 = true;
    }
    return validAD5242;
}*/
bool getTimeAD5242BR(uint8 *position)
{
    TI2C_Result res;
    uint8 data = 0;
    validAD5242 = false;
    //I2C_SetBaudRate(DS1338_BAUDRATE);
    //__disable_interrupt();
    res = I2C_ReadData(AD5242_DEVICE_ADD,1,0x00,sizeof(data),(uint8*)&data);
    //__enable_interrupt();
    if (res == I2C_RES_OK){
        *position = data;
        validAD5242 = true;
    }
    return validAD5242;
}
/*
Write to RTC if OK
Return Result
*/
bool setTimeAD5242BR(const uint8 contr, const uint8 position)
{
    TI2C_Result res;
    TINFO_AD5242 infoAD5242={0};
    validAD5242 = false;
    //I2C_SetBaudRate(DS1338_BAUDRATE);
    infoAD5242.control = contr;
    infoAD5242.data = position;
    //__disable_interrupt();
    res = I2C_WriteData(AD5242_DEVICE_ADD, 1, 0x00, sizeof(TINFO_AD5242), (uint8*)&infoAD5242);
    //__enable_interrupt();
    if (res == I2C_RES_OK)
        validAD5242 = true;
    return validAD5242;
}