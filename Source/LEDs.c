//---------------------------------------------------------------------------
//
// $Title:          LEDs.c
// $Description:    Functions for working with LEDs.
//                  
// $Rev:            0.0.4
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
#include <NXP/iolpc2103.h>
#include "LEDs.h"

#include "Pins.h"


//-------------------------------------------------------------
void LED1_On(void)                                         
{                                                          
    #if (LED1_INVERSION == 0)
        FIOCLR = LED1_MASK;                   
    #elif (LED1_INVERSION == 1)
        FIOSET = LED1_MASK;                   
    #else
        #error Wrong LED1_INVERSION Definition
    #endif                                                 
}                                                          
                                                           
void LED1_Off(void)                                        
{                                                          
    #if (LED1_INVERSION == 0)
        FIOSET = LED1_MASK;                   
    #elif (LED1_INVERSION == 1)
        FIOCLR = LED1_MASK;                   
    #else
        #error Wrong LED1_INVERSION Definition
    #endif                                                 
}                                                          
                                                           
void LED1_Toggle(void)                                     
{                                                          
    if (FIOPIN & LED1_MASK)
        FIOCLR = LED1_MASK;
    else
        FIOSET = LED1_MASK;
}                                                          
                                                           
                                                           
TLED_State LED1_GetState(void)                             
{                                                          
    if ( !!(FIOPIN & LED1_MASK) == !!LED1_INVERSION ) 
        return LED_ON;                                     
    else                                                   
        return LED_OFF;                                    
}

//-------------------------------------------------------------
void LED2_On(void)                                         
{                                                          
    #if (LED2_INVERSION == 0)
        FIOCLR = LED2_MASK;                   
    #elif (LED2_INVERSION == 1)
        FIOSET = LED2_MASK;                   
    #else
        #error Wrong LED2_INVERSION Definition
    #endif                                                 
}                                                          
                                                           
void LED2_Off(void)                                        
{                                                          
    #if (LED2_INVERSION == 0)
        FIOSET = LED2_MASK;                   
    #elif (LED2_INVERSION == 1)
        FIOCLR = LED2_MASK;                   
    #else
        #error Wrong LED2_INVERSION Definition
    #endif                                                 
}                                                          
                                                           
void LED2_Toggle(void)                                     
{                                                          
    if (FIOPIN & LED2_MASK)
        FIOCLR = LED2_MASK;
    else
        FIOSET = LED2_MASK;
}                                                          
                                                           
                                                           
TLED_State LED2_GetState(void)                             
{                                                          
    if ( !!(FIOPIN & LED2_MASK) == !!LED2_INVERSION ) 
        return LED_ON;                                     
    else                                                   
        return LED_OFF;                                    
}

//-------------------------------------------------------------
void RMan_On(void)                                         
{                                                          
    #if (RMAN_INVERSION == 0)
        FIOCLR = RMAN_MASK;                   
    #elif (RMAN_INVERSION == 1)
        FIOSET = RMAN_MASK;                   
    #else
        #error Wrong RMAN_INVERSION Definition
    #endif                                                 
}                                                          
                                                           
void RMan_Off(void)                                        
{                                                          
    #if (RMAN_INVERSION == 0)
        FIOSET = RMAN_MASK;                   
    #elif (RMAN_INVERSION == 1)
        FIOCLR = RMAN_MASK;                   
    #else
        #error Wrong RMAN_INVERSION Definition
    #endif                                                 
}                                                          
                                                           
void RMan_Toggle(void)                                     
{                                                          
    if (FIOPIN & RMAN_MASK)
        FIOCLR = RMAN_MASK;
    else
        FIOSET = RMAN_MASK;
}                                                          
                                                           
//-------------------------------------------------------------
void GMan_On(int State)                                         
{                                                          
    #if (GMAN_INVERSION == 0)
        if (State & 1)
            FIOCLR = GMAN0_MASK|GMAN2_MASK;
        else
            FIOCLR = GMAN0_MASK|GMAN1_MASK;
    #elif (GMAN_INVERSION == 1)
        if (State & 1)
            FIOSET = GMAN0_MASK|GMAN2_MASK;
        else
            FIOSET = GMAN0_MASK|GMAN1_MASK;
    #else
        #error Wrong GMAN_INVERSION Definition
    #endif                                                 
}                                                          
                                                           
void GMan_Off(void)                                        
{                                                          
    #if (GMAN_INVERSION == 0)
        FIOSET = GMAN_ALL_MASK;                   
    #elif (GMAN_INVERSION == 1)
        FIOCLR = GMAN_ALL_MASK;                   
    #else
        #error Wrong GMAN_INVERSION Definition
    #endif                                                 
}                                                          
                                                           
//-------------------------------------------------------------

void ClearAllLights(void)
{
    FIOCLR = 
    #if (GMAN_INVERSION == 0)
            GMAN_ALL_MASK   |
    #endif                                                 
    #if (RMAN_INVERSION == 0)
            RMAN_MASK       |
    #endif                                                 
    #if (SEG_INVERSION  == 0)
            SH_MASK         |
            SL_MASK         |
    #endif                                                 
            0;


    FIOSET = 
    #if (GMAN_INVERSION == 1)
            GMAN_ALL_MASK   |
    #endif                                                 
    #if (RMAN_INVERSION == 1)
            RMAN_MASK       |
    #endif                                                 
    #if (SEG_INVERSION  == 1)
            SH_MASK         |
            SL_MASK         |
    #endif                                                 
            0;
}

void SetLights(uint32 RedManVar, uint32 GreenManVar, uint32 DigitsVar)
{
    FIOSET = 
    #if (GMAN_INVERSION == 0)
            GreenManVar |
    #endif                                                 
    #if (RMAN_INVERSION == 0)
            RedManVar   |
    #endif                                                 
    #if (SEG_INVERSION  == 0)
            DigitsVar   |
    #endif                                                 
            0;

    FIOCLR = 
    #if (GMAN_INVERSION == 1)
            GreenManVar |
    #endif                                                 
    #if (RMAN_INVERSION == 1)
            RedManVar   |
    #endif                                                 
    #if (SEG_INVERSION  == 1)
            DigitsVar   |
    #endif                                                 
            0;
}

void PowerControl(uint32 bGreen)
{
    if (bGreen)
    {
        if ( !(FIODIR & PWR_CONTR_MASK) )
            FIODIR |= PWR_CONTR_MASK;
    }
    else
    {
        if (FIODIR & PWR_CONTR_MASK)
            FIODIR &= ~PWR_CONTR_MASK;
    }
}
