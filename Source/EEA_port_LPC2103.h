//---------------------------------------------------------------------------
//
// $Title:          EEA_port_LPC2103.h
// $Description:    Определение платформенно-независимых типов
//                      для платформы ARM LPC2103
// $Rev:            1.0.0
//
// $Author:         EEA
// $Created:        9.10.11
// $Last Modified:  9.10.11
//
// $Copyright:      EEA
//
// $Notes:
//
//---------------------------------------------------------------------------

#ifndef __EEA_PORT_LPC2103_H
#define __EEA_PORT_LPC2103_H

/*******************************************************
* Basic Types
*******************************************************/

/* Integer data types */

typedef signed char      int8;
typedef signed short     int16;
typedef signed long      int32;
typedef signed long long int64;

typedef unsigned char       uint8;
typedef unsigned short      uint16;
typedef unsigned long       uint32;
typedef unsigned long long  uint64;

// For Compatibility
typedef unsigned char  BYTE;
typedef unsigned short WORD;
typedef unsigned long  DWORD;
typedef unsigned int   BOOL;

#define byte  uint8
#define word  uint16
#define dword uint32
#define lword uint64

typedef uint32  uint_opt;
typedef int32   int_opt;

typedef uint32  uint16_opt;
typedef int32   int16_opt;

typedef uint8   uint_min;
typedef int8    int_min;

typedef uint64  uint_max;
typedef int64   int_max;

/* Fractional data types */
typedef signed short    frac16;
typedef signed long     frac32;

typedef unsigned short  ufrac16;
typedef unsigned long   ufrac32;

/* Complex data types */
typedef struct
{
    frac16    real;
    frac16    imag;
} cfrac16;

typedef struct
{
    frac32 real;
    frac32 imag;
} cfrac32;

typedef struct
{
    int8  real;
    int8  imag;
} cint8;

typedef struct
{
    int16 real;
    int16 imag;
} cint16;

typedef struct
{
    int32 real;
    int32 imag;
} cint32;

/* Useful definitions */
#ifndef COMPILER_HAS_BOOL
typedef int16 bool;
#endif


#define NULL ((void*)0)


/*******************************************************
* Constants
*******************************************************/

/* Function Result Values */
#define PASS 0
#define FAIL -1

#define MAX_16 ((int16)0x7FFF)
#define MIN_16 ((int16)0x8000)

#define MAX_32 ((int32)0x7FFFFFFF)
#define MIN_32 ((int32)0x80000000)

#ifndef COMPILER_HAS_BOOL
#define true  1
#define false 0
#endif

/*******************************************************
* Implementation Limits
*******************************************************/
typedef uint16 clockid_t;
typedef int32  time_t;
typedef uint16 ssize_t;

typedef clockid_t timer_t;


#endif
