//---------------------------------------------------------------------------
//
// $Title:          Config.h
// $Description:    General Configuration Data.
//
// $Rev:            0.0.18
//
// $Author:         EEA
//
// $Created:        9.10.11
// $Last Modified:  15.8.14
//
// $Copyright:      EEA
//
// $Notes:
//
//---------------------------------------------------------------------------
#ifndef __CONFIG_H
#define __CONFIG_H

//Oscilator Frequency[Hz]
#define FOSC    16000000UL
//System Clock[Hz]
#define FCCLK   64000000UL
//Periferial Clock[Hz]
#define PCCLK   FCCLK
//PLL MSEL
#define PLL_M_VAL 3
//PLL PSEL
#define PLL_P_VAL 1
//APB divider
#define APBDIV_VAL 1  //Same as System Clock

//-----------------------------------------------
// SOFTWARE_VERSION
// 1.x - ACC09
// 2.x - TrafficLight2
// 3.x - Light-Photo
// 4.x - TrafficLight3

#define SOFTWARE_VERSION_H  3
#define SOFTWARE_VERSION_L  3

#define PROTOCOL_VERSION_H  2
#define PROTOCOL_VERSION_L  1

// DEVICE_TYPE
// 0x10 - ACC09
// 0x20 - TrafficLight2
// 0x30 - Light-Photo
// 0x40 - TrafficLight3

#define DEVICE_TYPE     0x0030

//-----------------------------------------------

#define MAX_SOUND_NUMBER    4
#define MAX_DAY_PLANS       10
#define MAX_DAY_PLAN_ITEMS  24
#define MAX_HOLIDAYS        20
#define MAX_WEEK_PLANS      (7+1)

//-----------------------------------------------

#define MIN_TEACHING_PERIODS 1
#define MAX_TEACHING_PERIODS 2

#define MAX_SPEEDUP_TIME    10

#define DIGITS_MAX_COUNT    199
#define RED_MAX_COUNT       199
#define GREEN_MAX_COUNT     99

#define MIN_SENSOR_FILTER_TIME  5
#define MAX_SENSOR_FILTER_TIME  60

//-----------------------------------------------

#define SOUND_SAMPLES_FREQ  (20000UL)
#define SOUND_PWM_MAXVOLUME  256

#define FAST_LOOP_FREQ      (10000UL)
#define FAST_LOOP_FREQ_MS   (10UL)
#define SLAVE_CONTROL_FREQ  (30UL)

#define PWM_MODULO  100

#define TEACHING_PERIODS  1
#define LEARN_BUF_SIZE    12

#define MAN_SPEEDUP_TIME  4

//#define ROUND_UP_TIME   200
//#define ROUND_DOWN_TIME   0

#define ROUND_UP_TIME1   750
#define ROUND_UP_TIME2   250

#define DEF_ROUND_UP_TIME ROUND_UP_TIME1

//-----------------------------------------------
#define ADC_FREQ        (4000000UL)
#define ADC_MAX_VALUE   0x3FF

#define REF_VOLTAGE     3.3
#define POWER_DIV_RL    1.0
#define POWER_DIV_RH   10.0

#define POWER_BAD_VOLTAGE   12.0
#define POWER_GOOD_VOLTAGE  14.0

#define POWER_BAD_LEVEL  (int)((POWER_BAD_VOLTAGE*POWER_DIV_RL*ADC_MAX_VALUE)/((POWER_DIV_RL+POWER_DIV_RH)*REF_VOLTAGE))
#define POWER_GOOD_LEVEL (int)((POWER_GOOD_VOLTAGE*POWER_DIV_RL*ADC_MAX_VALUE)/((POWER_DIV_RL+POWER_DIV_RH)*REF_VOLTAGE))

#define VOLTAGE_SCALE           (int)(((POWER_DIV_RL+POWER_DIV_RH)*REF_VOLTAGE*0x10000)/(POWER_DIV_RL*ADC_MAX_VALUE))
#define LIGHT_SENS_SCALE        (int)((REF_VOLTAGE*10*0x10000)/(ADC_MAX_VALUE))
#define LIGHT_SENS_PRC_SCALE    (int)((100*0x10000)/(ADC_MAX_VALUE))

//-----------------------------------------------

#define MAN_STEP_PERIOD   500
#define SOUND_REPEAT_PERIOD (MAN_STEP_PERIOD*2)

//-----------------------------------------------

#define DEFAULT_BRIGHT_PRC  100
#define DEFAULT_VOLUME_PRC  100

#define GREEN_MAX_BRIGHT_PRC    70
#define RED_MAX_BRIGHT_PRC     100

#define GREEN_MAX_BRIGHT    (GREEN_MAX_BRIGHT_PRC*PWM_MODULO/100)
#define RED_MAX_BRIGHT      (RED_MAX_BRIGHT_PRC*PWM_MODULO/100)


#define DAY_START_HOUR     6
#define NIGHT_START_HOUR  18

//-----------------------------------------------
#define SENSOR_CHECK1_HOUR 1
#define SENSOR_CHECK2_HOUR 13

//-----------------------------------------------
#define WDT_TIMEOUT 100UL
//-----------------------------------------------

#endif