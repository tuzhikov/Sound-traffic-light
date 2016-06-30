//---------------------------------------------------------------------------
//
// $Title:          LightSensor.h
// $Description:    Light Sensor Interface.
//
// $Rev:            0.0.5
//
// $Author:         EEA
//
// $Created:        24.4.12
// $Last Modified:  5.12.13
//
// $Copyright:      EEA
//
// $Notes:
//
//---------------------------------------------------------------------------

#ifndef __LIGHTSENSOR_H
#define __LIGHTSENSOR_H

#include "EEA_port.h"
#include "DateTime.h"

typedef struct
{
    uint16 SensorValue;
    uint8  SensorInvalid;
    uint8  TotalCnt;
    uint32 TotalSum;
    uint32 LastCheckGenDay;     // (year<<16 + month<<8 + day)<<1 + am_pm_bit
    uint8  StartHour;
    uint8  StartMin;
    uint8  StartSec;
    uint8  LastHour;
    uint8  LastMin;
    uint8  LastSec;
} TSensorInfo;

extern TSensorInfo SensorInfo;

extern bool bSensorDisableLights;

void SensorPreInitMeasure(void);
void SensorFastLoopControl(bool bOff);
void SensorSlowLoopControl(void);

void InitLightSensor(void);
void ResetLightSensor(void);
void CheckLightSensor(TDateTime *Today);
void ProcessLightSensor(void);

bool IsLightSensorChecked(TDateTime *DT);

uint16 GetLightSensorPercent(uint16 Value);

void SetSensorBright(uint8 BrightRG, /*uint8 BrightY,*/ uint8 hour);

void Sensor_FillDebugInfo(uint8* ptr);

#endif
