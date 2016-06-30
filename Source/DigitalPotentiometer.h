/*----------------------------------------------------------------------------*/
//
// $Title:          DigitalPotentiometer.h
// $Description:    DIGITAL POTENTIOMETER AD5242BR. Interface.
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
#ifndef __DIGITAL_POTENTIOMETER_H
#define __DIGITAL_POTENTIOMETER_H

#include "EEA_port.h"
#include "DateTime.h"

#define AD5242_DEVICE_ADD    0x58  // AD0 and AD1 to low level
#define AD5242_SELECT_RDAC2  0x80
#define AD5242_MIDSCALE_RST  0x40
#define AD5242_SELECT_SHDN   0x20
#define AD5242_SELECT_OUT1   0x10
#define AD5242_SELECT_OUT2   0x08
#define AD5242_CONTROL_RDAC1 0x00//AD5242_MIDSCALE_RST|AD5242_SELECT_SHDN|0x07
#define AD5242_CONTROL_RDAC2 (0x80|0x20)//AD5242_SELECT_RDAC2//|AD5242_MIDSCALE_RST|AD5242_SELECT_SHDN|0x07

void initAD5242BR(void);
bool getTimeAD5242BR(uint8 *position);
bool setTimeAD5242BR(const uint8 contr, const uint8 position);

#endif