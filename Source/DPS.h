/*----------------------------------------------------------------------------*/
//
// $Title:          DPS.h
// $Description:    DIGITAL POTENTIOMETER General Interface.
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
#ifndef __DPS_H
#define __DPS_H

#include "DigitalPotentiometer.h"


#define ADDRDAC1 AD5242_CONTROL_RDAC1
#define ADDRDAC2 AD5242_CONTROL_RDAC2

#define initDPS()        initAD5242BR()
#define getTimeDPS(N)    getTimeAD5242BR(N)
#define setTimeDPS(N,K)  setTimeAD5242BR(N,K)

#endif