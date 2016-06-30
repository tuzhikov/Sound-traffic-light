//---------------------------------------------------------------------------
//
// $Title:          VIC.h
// $Description:    VIC interface.
//
// $Rev:            0.0.2
//
// $Author:         EEA
//
// $Created:        9.10.11
// $Last Modified:  14.10.11
//
// $Copyright:      EEA
//
// $Notes:
//
//---------------------------------------------------------------------------

#ifndef __VIC_H
#define __VIC_H

void VicInit(void);

__irq __arm void irq_handler(void);
__fiq __arm void fiq_handler(void);
__arm void undef_handler(void);
__arm void prefetch_handler(void);
__arm void data_handler(void);

#endif

