//---------------------------------------------------------------------------
//
// $Title:          Pins_LPC2103.h
// $Description:    Pins and Signals Definition for ARM LPC2103
//                  
// $Rev:            0.0.3
//
// $Author:         EEA
// $Created:        9.10.11
// $Last Modified:  20.10.11
//
// $Copyright:      EEA
//
// $Notes:          
//
//---------------------------------------------------------------------------
#ifndef __PINS_LPC2103_H
#define __PINS_LPC2103_H

// Pin Description Constants
#define FIO_DIR_INPUT       0
#define FIO_DIR_OUTPUT      1

#define FIO_NOT_MASKED      0
#define FIO_MASKED          1

#define PINSEL_GPIO         0        
#define PINSEL_FUNC0        0        
#define PINSEL_FUNC1        1        
#define PINSEL_FUNC2        2        
#define PINSEL_FUNC3        3        

#define FIO_PIN_ZERO        0
#define FIO_PIN_ONE         1

// ------------ Port 0 Description ------------
// Pin 00 - TXD0
#define PIN_00_DIR      FIO_DIR_INPUT
#define PIN_00_MASK     FIO_MASKED
#define PIN_00_SEL      PINSEL_FUNC1
#define PIN_00_PIN      FIO_PIN_ZERO

// Pin 01 - RXD0
#define PIN_01_DIR      FIO_DIR_INPUT
#define PIN_01_MASK     FIO_MASKED
#define PIN_01_SEL      PINSEL_FUNC1
#define PIN_01_PIN      FIO_PIN_ZERO

// Pin 02 - SCL0
#define PIN_02_DIR      FIO_DIR_INPUT
#define PIN_02_MASK     FIO_MASKED
#define PIN_02_SEL      PINSEL_FUNC1
#define PIN_02_PIN      FIO_PIN_ZERO

// Pin 03 - SDA0
#define PIN_03_DIR      FIO_DIR_INPUT
#define PIN_03_MASK     FIO_MASKED
#define PIN_03_SEL      PINSEL_FUNC1
#define PIN_03_PIN      FIO_PIN_ZERO

// Pin 04 - SHB
#define PIN_04_DIR      FIO_DIR_OUTPUT
#define PIN_04_MASK     FIO_NOT_MASKED
#define PIN_04_SEL      PINSEL_GPIO
#define PIN_04_PIN      FIO_PIN_ZERO

// Pin 05 - SHA
#define PIN_05_DIR      FIO_DIR_OUTPUT
#define PIN_05_MASK     FIO_NOT_MASKED
#define PIN_05_SEL      PINSEL_GPIO   
#define PIN_05_PIN      FIO_PIN_ZERO  

// Pin 06 - RMAN
#define PIN_06_DIR      FIO_DIR_OUTPUT
#define PIN_06_MASK     FIO_NOT_MASKED
#define PIN_06_SEL      PINSEL_GPIO   
#define PIN_06_PIN      FIO_PIN_ONE

// Pin 07 - SOUND_OUT
#define PIN_07_DIR      FIO_DIR_INPUT
#define PIN_07_MASK     FIO_MASKED
#define PIN_07_SEL      PINSEL_FUNC2
#define PIN_07_PIN      FIO_PIN_ZERO

// Pin 08 - GMAN2
#define PIN_08_DIR      FIO_DIR_OUTPUT
#define PIN_08_MASK     FIO_NOT_MASKED
#define PIN_08_SEL      PINSEL_GPIO   
#define PIN_08_PIN      FIO_PIN_ZERO  

// Pin 09 - GMAN1
#define PIN_09_DIR      FIO_DIR_OUTPUT
#define PIN_09_MASK     FIO_NOT_MASKED
#define PIN_09_SEL      PINSEL_GPIO   
#define PIN_09_PIN      FIO_PIN_ZERO  

// Pin 10 - FB_SOUND
#define PIN_10_DIR      FIO_DIR_INPUT
#define PIN_10_MASK     FIO_MASKED
#define PIN_10_SEL      PINSEL_FUNC3
#define PIN_10_PIN      FIO_PIN_ZERO

// Pin 11 - FB_BRIGHT
#define PIN_11_DIR      FIO_DIR_INPUT
#define PIN_11_MASK     FIO_MASKED
#define PIN_11_SEL      PINSEL_FUNC3
#define PIN_11_PIN      FIO_PIN_ZERO

// Pin 12 - GMAN0
#define PIN_12_DIR      FIO_DIR_OUTPUT
#define PIN_12_MASK     FIO_NOT_MASKED
#define PIN_12_SEL      PINSEL_GPIO   
#define PIN_12_PIN      FIO_PIN_ZERO  
             
// Pin 13 - SLC
#define PIN_13_DIR      FIO_DIR_OUTPUT
#define PIN_13_MASK     FIO_NOT_MASKED
#define PIN_13_SEL      PINSEL_GPIO   
#define PIN_13_PIN      FIO_PIN_ZERO  

// Pin 14 - BOOT_EN
#define PIN_14_DIR      FIO_DIR_INPUT
#define PIN_14_MASK     FIO_NOT_MASKED
#define PIN_14_SEL      PINSEL_GPIO
#define PIN_14_PIN      FIO_PIN_ONE
             
// Pin 15 - SLD
#define PIN_15_DIR      FIO_DIR_OUTPUT
#define PIN_15_MASK     FIO_NOT_MASKED
#define PIN_15_SEL      PINSEL_GPIO   
#define PIN_15_PIN      FIO_PIN_ZERO  

// Pin 16 - SLE
#define PIN_16_DIR      FIO_DIR_OUTPUT
#define PIN_16_MASK     FIO_NOT_MASKED
#define PIN_16_SEL      PINSEL_GPIO   
#define PIN_16_PIN      FIO_PIN_ZERO  
             
// Pin 17 - SLF
#define PIN_17_DIR      FIO_DIR_OUTPUT
#define PIN_17_MASK     FIO_NOT_MASKED
#define PIN_17_SEL      PINSEL_GPIO   
#define PIN_17_PIN      FIO_PIN_ZERO  
                                      
// Pin 18 - SLG                          
#define PIN_18_DIR      FIO_DIR_OUTPUT
#define PIN_18_MASK     FIO_NOT_MASKED
#define PIN_18_SEL      PINSEL_GPIO   
#define PIN_18_PIN      FIO_PIN_ZERO  
             
// Pin 19 - TEST_MODE 
#define PIN_19_DIR      FIO_DIR_INPUT 
#define PIN_19_MASK     FIO_NOT_MASKED
#define PIN_19_SEL      PINSEL_GPIO   
#define PIN_19_PIN      FIO_PIN_ONE   
                                      
// Pin 20 - RED_IN
#define PIN_20_DIR      FIO_DIR_INPUT 
#define PIN_20_MASK     FIO_NOT_MASKED
#define PIN_20_SEL      PINSEL_GPIO   
#define PIN_20_PIN      FIO_PIN_ONE   
                                      
// Pin 21 - GREEN_IN
#define PIN_21_DIR      FIO_DIR_INPUT 
#define PIN_21_MASK     FIO_NOT_MASKED
#define PIN_21_SEL      PINSEL_GPIO   
#define PIN_21_PIN      FIO_PIN_ONE   
                                      
// Pin 22 - LED1
#define PIN_22_DIR      FIO_DIR_OUTPUT
#define PIN_22_MASK     FIO_NOT_MASKED
#define PIN_22_SEL      PINSEL_GPIO   
#define PIN_22_PIN      FIO_PIN_ONE
             
// Pin 23 - LED2
#define PIN_23_DIR      FIO_DIR_OUTPUT
#define PIN_23_MASK     FIO_NOT_MASKED
#define PIN_23_SEL      PINSEL_GPIO   
#define PIN_23_PIN      FIO_PIN_ONE
                                      
// Pin 24 - FB_POWER
#define PIN_24_DIR      FIO_DIR_INPUT
#define PIN_24_MASK     FIO_MASKED   
#define PIN_24_SEL      PINSEL_FUNC3 
#define PIN_24_PIN      FIO_PIN_ZERO 
                                      
// Pin 25 - SLA
#define PIN_25_DIR      FIO_DIR_OUTPUT
#define PIN_25_MASK     FIO_NOT_MASKED
#define PIN_25_SEL      PINSEL_GPIO   
#define PIN_25_PIN      FIO_PIN_ZERO  
                                      
// Pin 26 - SLB
#define PIN_26_DIR      FIO_DIR_OUTPUT
#define PIN_26_MASK     FIO_NOT_MASKED
#define PIN_26_SEL      PINSEL_GPIO   
#define PIN_26_PIN      FIO_PIN_ZERO  
                                      
// Pin 27 - SHG
#define PIN_27_DIR      FIO_DIR_OUTPUT
#define PIN_27_MASK     FIO_NOT_MASKED
#define PIN_27_SEL      PINSEL_GPIO   
#define PIN_27_PIN      FIO_PIN_ZERO  
                                      
// Pin 28 - SHF
#define PIN_28_DIR      FIO_DIR_OUTPUT
#define PIN_28_MASK     FIO_NOT_MASKED
#define PIN_28_SEL      PINSEL_GPIO   
#define PIN_28_PIN      FIO_PIN_ZERO  
                                      
// Pin 29 - SHE
#define PIN_29_DIR      FIO_DIR_OUTPUT
#define PIN_29_MASK     FIO_NOT_MASKED
#define PIN_29_SEL      PINSEL_GPIO   
#define PIN_29_PIN      FIO_PIN_ZERO  
                                      
// Pin 30 - SHD
#define PIN_30_DIR      FIO_DIR_OUTPUT
#define PIN_30_MASK     FIO_NOT_MASKED
#define PIN_30_SEL      PINSEL_GPIO   
#define PIN_30_PIN      FIO_PIN_ZERO  

// Pin 31 - SHC
#define PIN_31_DIR      FIO_DIR_OUTPUT
#define PIN_31_MASK     FIO_NOT_MASKED
#define PIN_31_SEL      PINSEL_GPIO   
#define PIN_31_PIN      FIO_PIN_ZERO  


//-----------------------------------------------
#define LED1_INVERSION  1
#define LED2_INVERSION  1

#define SEG_INVERSION   0
#define GMAN_INVERSION  0

#define RMAN_INVERSION  1

#define SOUND_INVERSION 0
//-----------------------------------------------



//                          C840C840        
#define SHA_MASK          0x00000020UL
#define SHB_MASK          0x00000010UL
#define SHC_MASK          0x80000000UL
#define SHD_MASK          0x40000000UL
#define SHE_MASK          0x20000000UL
#define SHF_MASK          0x10000000UL
#define SHG_MASK          0x08000000UL
#define SH_MASK           (SHA_MASK|SHB_MASK|SHC_MASK|SHD_MASK|SHE_MASK|SHF_MASK|SHG_MASK)

#define SLA_MASK          0x02000000UL
#define SLB_MASK          0x04000000UL
#define SLC_MASK          0x00002000UL
#define SLD_MASK          0x00008000UL
#define SLE_MASK          0x00010000UL
#define SLF_MASK          0x00020000UL
#define SLG_MASK          0x00040000UL
#define SL_MASK           (SLA_MASK|SLB_MASK|SLC_MASK|SLD_MASK|SLE_MASK|SLF_MASK|SLG_MASK)

#define RMAN_MASK         0x00000040UL
#define GMAN0_MASK        0x00001000UL
#define GMAN1_MASK        0x00000100UL
#define GMAN2_MASK        0x00000200UL
#define GMAN_ALL_MASK     (GMAN0_MASK|GMAN1_MASK|GMAN2_MASK)


#define LED1_MASK         0x00400000UL
#define LED2_MASK         0x00800000UL
#define ALL_LEDS_MASK     (LED1_MASK|LED2_MASK)


#define SOUND_OUT_MASK    0x00000080UL


#define RED_IN_MASK       0x00100000UL
#define GREEN_IN_MASK     0x00200000UL
#define TEST_IN_MASK      0x00080000UL


#define DIRECT_MASK       (SH_MASK|SL_MASK|RMAN_MASK|GMAN_ALL_MASK|ALL_LEDS_MASK|SOUND_OUT_MASK)
#define OFF_MASK          (LED1_MASK|LED2_MASK|RMAN_MASK)


#define ADC_POWER_FB_CHANNEL   2
#define ADC_SOUND_FB_CHANNEL   3
#define ADC_BRIGHT_FB_CHANNEL  4

#define ADCR_CLKDIV_MASK 0x0000FF00
#define ADCR_CLKDIV_DISP 8
#define ADCR_BURST_MASK  0x00010000
#define ADCR_CLKS_MASK   0x000E0000
#define ADCR_CLKS_DISP   17
#define ADCR_PDN_MASK    0x00200000
#define ADCR_START_MASK  0x07000000
#define ADCR_START_DISP  24
#define ADCR_EDGE_MASK   0x08000000

#define ADCR_CLKS_10BITS    0
#define ADCR_CLKS_9BITS     1
#define ADCR_CLKS_8BITS     2
#define ADCR_CLKS_7BITS     3
#define ADCR_CLKS_6BITS     4
#define ADCR_CLKS_5BITS     5
#define ADCR_CLKS_4BITS     6
#define ADCR_CLKS_3BITS     7

#define SOUND_PWM_CHANNEL   0

#endif
