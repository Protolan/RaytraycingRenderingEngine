/* FILE NAME    int_env.h
 * PURPOSE      Integra Environment Include file WINDOWS NT
 *              and WATCOM C v 10.0 compiler
 * 
 * SPEC         NRC                                   1990.03.22
 * UPDATED      NRC                                   1990.03.22
 * UPDATED      SJK                                   1990.06.12
 * UPDATED      SJK                                   1990.11.01
 * UPDATED      ESA                                   1993.09.30
 * UPDATED      ESA                                   1994.02.21
 *              (added OS_AVAILABLE_NAME)
 * UPDATED      ESA                                   1995.04.15
 *              (adapted for WINDOWS NT from DOSX environment)
 * 
 * LAST UPDATED (Eed) E. Denisov         ppip04di.End  '95.09.04
 *              ---- Updated for Coding PNCL 4.00 ----
 * LAST UPDATED Eed   OS_MARK removed                  '00.01.24
 * LAST UPDATED Eed   LIB_TEMPLATE_INSTANCE() added    '00.01.25
 * LAST UPDATED Eed   INT64                            '01.01.05
 *
 * Copyright (C) INTEGRA, Inc. 1988 - 1995  All Rights Reserved.
 *
 * NOTES        Only supervisor can modify this file after consultations.
 *              However it must be assured, that kind of modification
 *              will not influence old programs behaviour, i.e.
 *              modifications must be limited only to additions.
 *
 * This header should include:
 *
 *    - #include directives for all compiler headers, which
 *      define standard C functions and macros allowed in
 *      Integra programs (see COD_PNCL for details)
 *
 *    - definitions of following types & constants:
 *
 *      VOID                   - 'void' type (void or empty)
 *      VPTR                   - void pointer definition
 *                               (void *) or (char *)
 *      REAL                   - definition of 'real' type
 *                               can be float or double
 *                               It ought to be #defined, so programmer
 *                               can #undefine it, and then redefine
 *                               according to needs
 *                               (but only float or double)
 *                               Obsolete now!
 *      BYTE                   - one byte type (0-255),
 *                               defined in integra.h now
 *      WORD                   - one word type (2 bytes)
 *      DWORD                  - double word (4 bytes) type
 *      BOOL                   - boolean type
 *
 *      MIN_SHORT, MAX_SHORT   - short type range
 *      MIN_INT, MAX_INT       - int type range
 *      MIN_LONG, MAX_LONG     - long type range
 *      MAX_USHORT, MAX_UINT,
 *      MAX_ULONG              - maximum unsigned values
 *
 *      MAX_FLOAT, MAX_DOUBLE  - maximum floating point type values
 *      MIN_FLOAT, MIN_DOUBLE  - minimum positive floating point values
 *      EPS_FLOAT, EPS_DOUBLE  - min x > 0 : 1.0 + x != 1.0
 *
 *      PROT                   - macro to define prototypes
 *
 *      IN                     - prefix for input only parameters
 *                               (can be 'const' or empty)
 *      INOUT                  - prefix for input/output parameters
 *                               (empty for the time being)
 *      OUT                    - prefix for output only parameters
 *                               (empty for the time being)
 *
 *      PATH_SEPARATOR         - path separator character ('\\' or '/')
 *      PATH_SEPARATOR_STR     - path separator string ("\\" or "/")
 *      MAX_PATHNAME_LENGTH    - max length of full pathname
 *      MAX_FILENAME_LENGTH    - max length of file name
 *      MAX_BASENAME_LENGTH    - max length of base file name
 *      MAX_SUFFIX_LENGTH      - max length of file extension (suffix)
 * 
 *      ALIGN                  - value of alignment (integer > 0)
 *      NO_64KB_LIMIT          - presence of 64KB limit or no
 *                               (defined as 0 or 1)
 *      MAX_ARRAY_SIZE         - max size of array 
 *                               (also max argument of malloc function)
 * 
 *      OS_NAME                - string with OS name
 *      OS_AVAILABLE_NAME      - available characters for file name
 * 
 */

#ifndef _INT_ENV_H_
#define _INT_ENV_H_


/* Added by Pnd */
/* Disable "unreferenced formal parameter" warning */
#pragma warning(disable : 4100)
/* Disable "uses old-style declarator" warning */
#pragma warning(disable : 4131)
/* Disable "unreferenced inline function has been removed" warning */
#pragma warning(disable : 4514)
/* Disable "this call is not inlined" warning */
#pragma warning(disable : 4710)
/* Disable "function marked as __forceinline not inlined" warning */
#pragma warning(disable : 4714)

#undef  INLINE
#define INLINE __forceinline

/* Replacement for WINDOWS's RGB() macro */
#define WIN_RGB(r,g,b) ((COLORREF)(((BYTE)(r)|((WORD)((BYTE)(g))<<8))|(((DWORD)(BYTE)(b))<<16)))

/* System headers */

#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <ctype.h>
#include <string.h>
#include <math.h>
#include <signal.h>

START_C_DECLS

/***************************************************************************
 *   Definitions for function prototypes
 ***************************************************************************/

/* If none from [NO_]PROTOTYPES is defined, then define it */

#ifndef PROTOTYPES
#ifndef NO_PROTOTYPES

/*  Default value for the platform  */
#define PROTOTYPES

#endif
#endif

/* Now assure, that only one from [NO_]PROTOTYPES is defined */

#ifdef PROTOTYPES
#undef NO_PROTOTYPES
#endif

#ifdef NO_PROTOTYPES
#undef PROTOTYPES
#endif

/*  Prot macro definition (do not modify!)  */
#ifndef NO_PROTOTYPES
#define PROT(x) x
#else
#define PROT(x) ()
#endif

/*  Prefixes for function parameters (do not modify!)  */
#define IN
#define INOUT
#define OUT


/***************************************************************************
 *   Platform-dependent types
 ***************************************************************************/

/*  BOOL type
 *  May be defined as int or char
 *  It is FORBIDDEN to rely on sizeof(BOOL)!
 *  In WINDOWS BOOL is defined in system headers as int!
 */
#ifdef  BOOL
#undef  BOOL
#endif
typedef int  BOOL;

/*  REAL type is now obsolete  */
#ifdef  REAL
#undef  REAL
#endif
#define REAL double


/*  VOID / VPTR type definitions  */

/*  VOID may be defined by WINDOWS headers  */
#ifdef  VOID
#undef  VOID
#endif

#if YES     /* 'void' type IS supported */
#define VOID  void
typedef void *VPTR;
#else   /* 'void' type is NOT supported */
typedef int   VOID;
typedef char *VPTR;
#endif

/*  WORD / DWORD type definitions  */
/*  Must occupy 16 / 32 bits respectively  */
typedef unsigned short WORD;    /* 16 bits */
typedef unsigned long  DWORD;   /* 32 bits */

/* Handles to deal with pointers to functions */
typedef void *HANDLE;

/*  BYTE type is defined in integra.h  */

/* 64-bit int */
typedef __int64 INT64;
typedef unsigned __int64 UINT64;

#ifdef  _WIN64
typedef unsigned __int64 SIZE_T;
#elif _MSC_VER >= 1300
typedef __w64 unsigned long SIZE_T;
#else
typedef unsigned long SIZE_T;
#endif

// ISO C++ analogs
#define itoa _itoa
#define chdir _chdir
#define mkdir _mkdir
#define rmdir _rmdir
#define unlink _unlink
#define getcwd _getcwd
#define putenv _putenv
#define spawnvp _spawnvp
#define stricmp _stricmp
#define strnicmp _strnicmp

/***************************************************************************
 *   Ranges of integer / real types
 ***************************************************************************/

#define  MIN_SHORT  (-32767)   /* minimum signed short value */
#define  MAX_SHORT  32767      /* maximum signed short value */

#define  MIN_INT    (-2147483647)  /* minimum value of an int  */
#define  MAX_INT    2147483647     /* maximum signed int value */

#define  MIN_LONG   (-2147483647)  /* minimum signed long value */
#define  MAX_LONG   2147483647     /* maximum signed long value */

/*  Maximum values for unsigned integer types  */
#define  MAX_USHORT 0xffff         /* 65535U      */
#define  MAX_UINT   0xffffffff     /* 4294967295U */
#define  MAX_ULONG  0xffffffff     /* 4294967295U */

// Maximal and minimal values for INT64 type
#define  MIN_INT64  0x8000000000000000
#define  MAX_INT64  0x7fffffffffffffff

// Maximal value for UINT64 type
#define  MAX_UINT64 0xffffffffffffffff

#ifdef _WIN64
#define MAX_SIZE_T MAX_INT64 // 8 EB (intentional 1/2 of max)
#else
#define MAX_SIZE_T MAX_INT   // 2 GB (intentional 1/2 of max)
#endif

/*  Maximum values for float and double types (rounded down)  */
#define  MAX_FLOAT  3.40e+38         /* maximum float value   */
#define  MAX_DOUBLE 1.79e+308        /* maximum double value  */

/*  Minimal positive values for float and double types (rounded up)  */
#define  MIN_FLOAT  1.18e-38       /* minimal positive float value   */
#define  MIN_DOUBLE 2.23e-308      /* minimal positive double value  */

/* minimum positive floating point number x such that 1.0 + x != 1.0 */
#define  EPS_FLOAT  1.192092896e-7                /* for float  */
#define  EPS_DOUBLE 2.22044604925031300e-016      /* for double */


/***************************************************************************
 *   Other platform-dependent definitions
 ***************************************************************************/

#define PATH_SEPARATOR       '\\'
#define PATH_SEPARATOR_STR   "\\"
#define MAX_PATHNAME_LENGTH  259
#define MAX_FILENAME_LENGTH  251
#define MAX_BASENAME_LENGTH  251
#define MAX_SUFFIX_LENGTH    250

#define ALIGN                4
#define NO_64KB_LIMIT        1
#define MAX_ARRAY_SIZE       0x7FFFFFFF

#define OS_NAME              "NT"       /* WINDOWS NT name   */
#define OS_BYTE_ORDER        0          /* low byte first */
#define OS_FP_FORMAT         0          /* IEEE */

#ifndef min
#define min(x, y) ((x) < (y) ? (x) : (y))
#define max(x, y) ((x) > (y) ? (x) : (y))
#endif

#define OS_AVAILABLE_NAME       {' ', '!', '#', '$', '%', '&', '\'', '(', ')', \
                                 '-', '@', '^', '_', '`', '{', '}', '~', \
                                 '\x7F', '\x86', '\x87', '\x8D', '\x8E', \
                                 '\x8F', '\x94', '\x98', '\x9B', '\x9C', \
                                 '\x9F', '\xA0', '\xA1', '\xA2', '\xA3', \
                                 '\xA4', '\xA5', '\xA6', '\xA7', '\xA8', \
                                 '\xA9', '\xAA', '\xAB', '\xAC', '\xAD', \
                                 '\xAE', '\xAF', '\xB0', '\xB1', '\xB2', \
                                 '\xB3', '\xB4', '\xB5', '\xB6', '\xB7', \
                                 '\xB8', '\xB9', '\xBA', '\xBB', '\xBC', \
                                 '\xBD', '\xBE', '\xBF', '\xC0', '\xC1', \
                                 '\xC2', '\xC3', '\xC4', '\xC5', '\xC6', \
                                 '\xC7', '\xC8', '\xC9', '\xCA', '\xCB', \
                                 '\xCC', '\xCD', '\xCE', '\xCF', '\xD0', \
                                 '\xD1', '\xD2', '\xD3', '\xD4', '\xD5', \
                                 '\xD6', '\xD7', '\xD8', '\xD9', '\xDA', \
                                 '\xDB', '\xDC', '\xDD', '\xDE', '\xDF', \
                                 '\xE0', '\xE1', '\xE2', '\xE3', '\xE4', \
                                 '\xE5', '\xE6', '\xE7', '\xE8', '\xE9', \
                                 '\xEA', '\xEB', '\xEC', '\xED', '\xEE', \
                                 '\xEF', '\xF0', '\xF1', '\xF2', '\xF3', \
                                 '\xF4', '\xF5', '\xF6', '\xF7', '\xF8', \
                                 '\xF9', '\xFA', '\xFB', '\xFC', '\xFD', \
                                 '\xFE', '\xFF', '\x00'}


/* Explicit class instantiation */
#define LIB_TEMPLATE_INSTANCE(Class, Type) template class Class<Type>;

END_C_DECLS

#endif


/* END OF 'int_env.h' FILE */
