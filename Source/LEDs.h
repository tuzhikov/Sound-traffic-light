//---------------------------------------------------------------------------
//
// $Title:          LEDs.h
// $Description:    Functions and types for working with LEDs.
//                  
// $Rev:            0.0.5
//
// $Author:         EEA
// $Created:        9.10.11
// $Last Modified:  3.11.11
//
// $Copyright:      EEA
//
// $Notes:          
//
//---------------------------------------------------------------------------
#ifndef __LEDS_H
#define __LEDS_H

#include "EEA_port.h"

typedef enum
{
    LED_OFF,
    LED_ON
} TLED_State;

void LED1_On(void);
void LED1_Off(void);
void LED1_Toggle(void);

TLED_State LED1_GetState(void);

void LED2_On(void);
void LED2_Off(void);
void LED2_Toggle(void);

TLED_State LED2_GetState(void);

void ClearAllLights(void);
void SetLights(uint32 RedManVar, uint32 GreenManVar, uint32 DigitsVar);

void PowerControl(uint32 bGreen);


#endif