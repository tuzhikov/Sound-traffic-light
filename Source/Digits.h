//---------------------------------------------------------------------------
//
// $Title:          Digits.h
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

#ifndef __DIGITS_H
#define __DIGITS_H

unsigned int CalcNumberMask(int d, int Limit);

unsigned int CalcHiDigitMask(int d);
unsigned int CalcLoDigitMask(int d);

#endif
