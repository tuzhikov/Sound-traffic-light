//---------------------------------------------------------------------------
//
// $Title:          Digits.c
// $Description:    Functions for Calc LCD Segment Masks
//                  
// $Rev:            0.0.4
//
// $Author:         EEA
// $Created:        9.10.11
// $Last Modified:  15.8.14
//
// $Copyright:      EEA
//
// $Notes:          
//
//---------------------------------------------------------------------------
#include <NXP/iolpc2103.h>

#include "FastLoop.h"

#include "EEA_port.h"
#include "Pins.h"
#include "Config.h"

#include "Sound.h"


/*
void SetHiDigit (int d)
{
  if (d<0) d=0;
  if (d>9) d=9;
  FIOCLR=SH;
    switch (d)
  {
    case 0:
    {
      FIOSET=SHA|SHB|SHC|SHD|SHE|SHF;
    }
    break;
    
    case 1:
    {
      FIOSET=SHB|SHC;
    }
    break;
    
    case 2:
    {
      FIOSET=SHA|SHB|SHD|SHE|SHG;
    }
    break;
    
    case 3:
    {
      FIOSET=SHA|SHB|SHC|SHD|SHG;
    }
    break;
    
    case 4:
    {
      FIOSET=SHB|SHC|SHF|SHG;
    }
    break;
    
    case 5:
    {
      FIOSET=SHA|SHC|SHD|SHF|SHG;
    }
    break;
    
    case 6:
    {
      FIOSET=SHA|SHC|SHD|SHE|SHF|SHG;
    }
    break;
    
    case 7:
    {
      FIOSET=SHA|SHB|SHC;
    }
    break;
    
    case 8:
    {
      FIOSET=SHA|SHB|SHC|SHD|SHE|SHF|SHG;
    }
    break;
    
    case 9:
    {
      FIOSET=SHA|SHB|SHC|SHD|SHF|SHG;
    }
    break;
  }
}

void SetLoDigit (int d)
{
  if (d<0) d=0;
  if (d>9) d=9;
  FIOCLR=SL;
    switch (d)
  {
    case 0:
    {
      FIOSET=SLA|SLB|SLC|SLD|SLE|SLF;
    }
    break;
    
    case 1:
    {
      FIOSET=SLB|SLC;
    }
    break;
    
    case 2:
    {
      FIOSET=SLA|SLB|SLD|SLE|SLG;
    }
    break;
    
    case 3:
    {
      FIOSET=SLA|SLB|SLC|SLD|SLG;
    }
    break;
    
    case 4:
    {
      FIOSET=SLB|SLC|SLF|SLG;
    }
    break;
    
    
    case 5:
    {
      FIOSET=SLA|SLC|SLD|SLF|SLG;
    }
    break;
    
    case 6:
    {
      FIOSET=SLA|SLC|SLD|SLE|SLF|SLG;
    }
    break;
    
    case 7:
    {
      FIOSET=SLA|SLB|SLC;
    }
    break;
    
    
    case 8:
    {
      FIOSET=SLA|SLB|SLC|SLD|SLE|SLF|SLG;
    }
    break;
    
    case 9:
    {
      FIOSET=SLA|SLB|SLC|SLD|SLF|SLG;
    }
    break;
    
  }
}


void SetDigit(int d)
{
  int hi,lo;
  if (d<0) d=0;
  if (d>99) d=99;

  hi=0;

  while (d>=10)
  {
    d-=10;
    hi++;
  }

  lo=d;
  
  SetLoDigit(lo);
  SetHiDigit(hi);
}
*/

unsigned int CalcHiDigitMask(int d)
{
    unsigned int res;
    
    if (d<0) d=0;
    if (d>9) d=9;
  
    switch (d)
    {
        case 0:
        {
            res = SHA_MASK|SHB_MASK|SHC_MASK|SHD_MASK|SHE_MASK|SHF_MASK;
            break;
        }
        
        case 1:
        {
            res = SHB_MASK|SHC_MASK;
            break;
        }
        
        case 2:
        {
            res = SHA_MASK|SHB_MASK|SHD_MASK|SHE_MASK|SHG_MASK;
            break;
        }
        
        case 3:
        {
            res = SHA_MASK|SHB_MASK|SHC_MASK|SHD_MASK|SHG_MASK;
            break;
        }
        
        case 4:
        {
            res = SHB_MASK|SHC_MASK|SHF_MASK|SHG_MASK;
            break;
        }
        
        case 5:
        {
            res = SHA_MASK|SHC_MASK|SHD_MASK|SHF_MASK|SHG_MASK;
            break;
        }
        
        case 6:
        {
            res = SHA_MASK|SHC_MASK|SHD_MASK|SHE_MASK|SHF_MASK|SHG_MASK;
            break;
        }
        
        case 7:
        {
            res = SHA_MASK|SHB_MASK|SHC_MASK;
            break;
        }
        
        case 8:
        {
            res = SHA_MASK|SHB_MASK|SHC_MASK|SHD_MASK|SHE_MASK|SHF_MASK|SHG_MASK;
            break;
        }
        
        case 9:
        {
            res = SHA_MASK|SHB_MASK|SHC_MASK|SHD_MASK|SHF_MASK|SHG_MASK;
            break;
        }
    }
  
    return res;
}

unsigned int CalcLoDigitMask(int d)
{
    unsigned int res;
    
    if (d<0) d=0;
    if (d>9) d=9;
  
    switch (d)
    {
        case 0:
        {
            res = SLA_MASK|SLB_MASK|SLC_MASK|SLD_MASK|SLE_MASK|SLF_MASK;
            break;
        }
        
        case 1:
        {
            res = SLB_MASK|SLC_MASK;
            break;
        }
        
        case 2:
        {
            res = SLA_MASK|SLB_MASK|SLD_MASK|SLE_MASK|SLG_MASK;
            break;
        }
        
        case 3:
        {
            res = SLA_MASK|SLB_MASK|SLC_MASK|SLD_MASK|SLG_MASK;
            break;
        }
        
        case 4:
        {
            res = SLB_MASK|SLC_MASK|SLF_MASK|SLG_MASK;
            break;
        }
        
        case 5:
        {
            res = SLA_MASK|SLC_MASK|SLD_MASK|SLF_MASK|SLG_MASK;
            break;
        }
        
        case 6:
        {
            res = SLA_MASK|SLC_MASK|SLD_MASK|SLE_MASK|SLF_MASK|SLG_MASK;
            break;
        }
        
        case 7:
        {
            res = SLA_MASK|SLB_MASK|SLC_MASK;
            break;
        }
        
        case 8:
        {
            res = SLA_MASK|SLB_MASK|SLC_MASK|SLD_MASK|SLE_MASK|SLF_MASK|SLG_MASK;
            break;
        }
        
        case 9:
        {
            res = SLA_MASK|SLB_MASK|SLC_MASK|SLD_MASK|SLF_MASK|SLG_MASK;
            break;
        }
    }
  
    return res;
}

unsigned int CalcNumberMask(int d, int Limit)
{
    int hi,lo;

    unsigned int res = 0;

    if (d<0) d=0;
    if (d>199)   d=199;
    if (d>Limit) d=Limit;

    if (d>99)
    {
      d-=100;
      res = S1_MASK;
    }
    
    hi=0;

    while (d>=10)
    {
      d-=10;
      hi++;
    }

    lo=d;
    
    res |= CalcLoDigitMask(lo);
    res |= CalcHiDigitMask(hi);

    return res;
}

