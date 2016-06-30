//---------------------------------------------------------------------------
//
// $Title:          VIC.c
// $Description:    VIC Functions and Data.
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

#include <NXP/iolpc2103.h>
//#include <intrinsics.h>

#include "VIC.h"

#include "EEA_port.h"
#include "Sound.h"


/*************************************************************************
 * Function Name: VicInit
 * Parameters: none
 *
 * Return: none
 *
 * Description: Init VIC module
 *
 *************************************************************************/
void VicInit(void)
{
    // Assign all interrupt channels to IRQ
    VICIntSelect  =  0;
    // Disable all interrupts
    VICIntEnClear = 0xFFFFFFFF;
    // Clear all software interrupts
    VICSoftIntClear = 0xFFFFFFFF;
    // VIC registers can be accessed in User or privileged mode
    VICProtection = 0;
    // Clear interrupt
    VICVectAddr = 0;
    // Clear address of the Interrupt Service routine (ISR) for non-vectored IRQs.
    VICDefVectAddr = 0;
    // Clear address of the Interrupt Service routine (ISR) for vectored IRQs.
    VICVectAddr0  = VICVectAddr1  = VICVectAddr2  = VICVectAddr3  =\
    VICVectAddr4  = VICVectAddr5  = VICVectAddr6  = VICVectAddr7  =\
    VICVectAddr8  = VICVectAddr9  = VICVectAddr10 = VICVectAddr11 =\
    VICVectAddr12 = VICVectAddr13 = VICVectAddr14 = VICVectAddr15 = 0;
    // Disable all vectored IRQ slots
    VICVectCntl0  = VICVectCntl1  = VICVectCntl2  = VICVectCntl3  =\
    VICVectCntl4  = VICVectCntl5  = VICVectCntl6  = VICVectCntl7  =\
    VICVectCntl8  = VICVectCntl9  = VICVectCntl10 = VICVectCntl11 =\
    VICVectCntl12 = VICVectCntl13 = VICVectCntl14 = VICVectCntl15 = 0;
}

/*************************************************************************
 * Function Name: irq_handler
 * Parameters: none
 *
 * Return: none
 *
 * Description: IRQ handler
 *
 *************************************************************************/
__irq __arm void irq_handler (void)
{
    void (*interrupt_function)();
    unsigned int vector;

    vector = VICVectAddr;     // Get interrupt vector.
    interrupt_function = (void(*)())vector;
    if(interrupt_function != NULL)
    {
        interrupt_function();  // Call vectored interrupt function.
    }
    else
    {
        VICVectAddr = 0;      // Clear interrupt in VIC.
    }
}


/* These handlers trap instruction fiq, prefetch abort, undefined instruction abort
   and data fetch abort exceptions
*/
__fiq __arm void fiq_handler(void)
{
    T2IR_bit.MR3INT = 1; // Clear Interrupt Flag  
                                                    
    SoundIntHandler();   // Do FIQ Work
                                                     
    VICVectAddr = 0;     // Clear Interrupt in VIC
}


__arm void undef_handler(void)
{
  while(1);
}


__arm void prefetch_handler(void)
{
  while(1);
}


__arm void data_handler(void)
{
  while(1);
}
