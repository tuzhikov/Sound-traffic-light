//---------------------------------------------------------------------------
//
// $Title:          BCD.c
// $Description:    BCD Arithmetic Realization
//                  
// $Rev:            0.0.2
//
// $Author:         EEA
// $Created:        26.2.8
// $Last Modified:  3.8.12
//
// $Copyright:      Electrotex
//
// $Notes:          
//
//---------------------------------------------------------------------------

#include "BCD.h"

const uint8  BCD_10N  [16] = {0,   10,   20,   30,   40,   50,   60,   70,   80,   90,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF };
const uint16 BCD_100N [16] = {0,  100,  200,  300,  400,  500,  600,  700,  800,  900, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF };
const uint16 BCD_1000N[16] = {0, 1000, 2000, 3000, 4000, 5000, 6000, 7000, 8000, 9000, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF };

int8 BCD_to_Byte(uint8 N)
{
    int8  Res;
    uint8 tmp;

    tmp = (N>>4)&0x0F;
    if (tmp>9) return -1;
    Res = BCD_10N[tmp]; 

    tmp = N&0x0F;
    if (tmp>9) return -1;
    Res += tmp; 

    return Res;
}

int8 Byte_to_BCD(uint8 N)
{
    uint32 i;
    int8   Res;
    
    if (N > 99) return 99;

    for (i=1; i<11; i++)
    {
        if (N < BCD_10N[i])
        {
            i--;
            break;
        }
    }

    Res  = (i<<4);
    N   -= BCD_10N[i];

    Res += N;

    return Res;
}

int16 BCD_to_Word(uint16 N)
{
    int16 Res;
    uint8 tmp;

    tmp = (N>>12)&0x000F;
    if (tmp>9) return -1;
    Res = BCD_1000N[tmp]; 

    tmp = (N>>8)&0x000F;
    if (tmp>9) return -1;
    Res += BCD_100N[tmp]; 

    tmp = (N>>4)&0x000F;
    if (tmp>9) return -1;
    Res += BCD_10N[tmp]; 

    tmp = N&0x000F;
    if (tmp>9) return -1;
    Res += tmp; 

    return Res;
//    return ((N&0x000F) + BCD_10N[(N>>4)&0x000F] + BCD_100N[(N>>8)&0x000F] + BCD_1000N[(N>>12)&0x000F]);
}

int16 Word_to_BCD(uint16 N)
{
    uint32 i;
    int16  Res;
    
    if (N > 9999) return 9999;

    // 1000
    for (i=1; i<11; i++)
    {
        if (N < BCD_1000N[i])
        {
            i--;
            break;
        }
    }
    Res  = (i<<12);
    N   -= BCD_1000N[i];

    // 100
    for (i=1; i<11; i++)
    {
        if (N < BCD_100N[i])
        {
            i--;
            break;
        }
    }
    Res += (i<<8);
    N   -= BCD_100N[i];

    // 10
    for (i=1; i<11; i++)
    {
        if (N < BCD_10N[i])
        {
            i--;
            break;
        }
    }
    Res += (i<<4);
    N   -= BCD_10N[i];

    Res += N;

    return Res;
}
