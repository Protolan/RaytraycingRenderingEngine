/* FILE NAME    integra.h
 *
 * PURPOSE      Header file for INTEGRA's standard constant definitions.
 *              This file is environment independent - all environment
 *              specific constants, functions are defined in INT_ENV.H
 *              This header (INT_ENV.H) is included at the beginning of
 *              this file.
 *
 *              Who                       PRF name      When
 *
 * SPEC         INTEGRA
 * DESIGN       INTEGRA
 * DES. VV      INTEGRA
 * CODING       K. Klimaszewski                        1988.12.??
 *              Takashi Hyodo                          1988.12.??
 * CODE VV      Nobuhiro Watanabe                      1988.12.??
 * TEST         K. Klimaszewski                        1988.12.??
 *              Takashi Hyodo                          1988.12.??
 * UODATED      J. Kalinowski                          1990.06.10
 * UPDATED      Nrc (add TIPE include)                 1990.07.??
 * UPDATED      Sjk (add _P definition)                1992.02.25
 * UPDATED      Esa (MAX_IDENTIFIER_LENGTH)            1995.01.27
 * UPDATED      End (PNCL 4.0)                         1995.04.03
 * UPDATED      Esa (START_C_DECLS/END_C_DECLS)        1998.03.06
 * UPDATED      Esa (Assert/DEBUG/_DEBUG)              2001.01.05
 * UPDATES      Pnd (OS_MARK, TIPE auto definition)    2001.01.31
 * UPDATES      Piv Esa Eed: Memory leakage detection  2002.07.18
 *
 *
 * Copyright (C) INTEGRA, Inc. 1988 - 2001  All Rights Reserved.
 *
 *
 * NOTE         Do not modify this file without permission of INTEGRA.
 *
 */


#ifndef _INTEGRA_H_
#define _INTEGRA_H_

#include "../../sdk/rep_version.hpp"

/*
 * Added by Pnd '01.01.31
 * This is necessary for painless compilation of ENVI stuff
 * under VC++ (likely should be spread over particular int_env.h)
 */

#ifndef OS_MARK

#if defined(_WIN32)          // Windows
#define OS_MARK  'I'
#elif defined(__sgi)        // IRIX
#define OS_MARK  'U'
#elif defined(__linux)      // Linux
#define OS_MARK  'L'
#endif

#if !defined(TIPE) && !defined(NO_TIPE)
#define TIPE
#endif

#endif


/**************************************************************
 *               MACRO DEFINITIONS AND TYPEDEFS
 *************************************************************/

/* C declaration */
#ifdef __cplusplus

#define START_C_DECLS  extern "C" {
#define END_C_DECLS      }

#ifdef VIRTUAL
#undef VIRTUAL
#endif
#define VIRTUAL

#ifdef STATIC
#undef STATIC
#endif
#define STATIC

#ifdef INLINE
#undef INLINE
#endif
#define INLINE inline

/* Only to resolve external symbol in DLLs, not linking lib, where
   the function is defined. 
*/
#define FVIRTUAL virtual



#define INTEGRA_NAMESPACE_START namespace Integra {
#define INTEGRA_NAMESPACE_END }
INTEGRA_NAMESPACE_START
INTEGRA_NAMESPACE_END
using namespace Integra;

#else

#define START_C_DECLS
#define END_C_DECLS

#endif /* __cplusplus */

/*  Unsigned types  */
typedef unsigned short USHORT;
typedef unsigned long  ULONG;
typedef unsigned char  BYTE;

/*  BYTE type may assume values from 0 to 255  */
#define MAX_BYTE 255

/*  Type BOOL is defined as char or int in int_env.h  */
/*  Here are constants of this type  */

#define TRUE    1
#define FALSE   0

#define YES     TRUE
#define NO      FALSE

#define ON      TRUE
#define OFF     FALSE

/*  Type OKAY assumes SUCCESS or FAILURE values  */

typedef int OKAY;

#define SUCCESS   0
#define FAILURE (-1)



#define MAX_IDENTIFIER_LENGTH   128


/**************************************************************
 *                     COMPANY INCLUDES
 *************************************************************/

/*  Placed here to provide access to all definitions above  */

/*  Note:  globals.h is obsolete but retained
 *  to provide compatibility with old codes
 */

#include "int_env.h"


/*  Macros to deal with 2D/3D vectors and color components  */

#define X 0    /* X, Y, Z components */
#define Y 1
#define Z 2

#define XY  2  /* 2D vectors */
#define XYZ 3  /* 3D vectors */

#define R 0    /* R, G, B components */
#define G 1
#define B 2

#ifdef RGB
#undef RGB
#endif
#define RGB 3  /* RGB vectors */


/* C declaration for integra.h itself */
START_C_DECLS

/* The following includes for Integra Make only but not for PLUG */
#ifdef OS_MARK

#include "globals.h"

#ifdef TIPE
#include "itpl.h"
#endif

#ifdef NO_TIPE
#include "ikbl.h"
#include "iosl.h"
#include "ipdl.h"
#include "ietl.h"
#endif

#endif /* OS_MARK */

/**************************************************************
 *                      DEBUG MACROS
 *************************************************************/


/*  Debug levels (LOW, MIDDLE, HIGH)  */
#define DL 0x000F      /* decimal:  15   */
#define DM 0x00FF      /* decimal:  255  */
#define DH 0xFFFF      /* decimal: 65535 */


/*  DB, DBM macros  */
#if defined(DEBUG) || defined (_DEBUG)

#ifndef DEBUG
#define DEBUG
#endif
#ifndef _DEBUG
#define _DEBUG
#endif
#ifdef NDEBUG
#undef NDEBUG
#endif

#define DB(expr) (expr);

/*  Define DB_MASK as the name of a variable containing the actual mask  */
#define DBM(mask, expr) \
  { \
  if ((mask) & DB_MASK) \
    (expr); \
  }

#else  /* DEBUG or _DEBUG is not defined */

#ifdef DEBUG
#undef DEBUG
#endif
#ifdef _DEBUG
#undef _DEBUG
#endif
#ifndef NDEBUG
#define NDEBUG
#endif
#ifndef QT_NO_DEBUG
#define QT_NO_DEBUG
#endif

/*  Empty statements  */
#define  DB(expr)         ;
#define  DBM(mask, expr)  ;

#endif  /* DEBUG */

// Assert macro
#ifdef ASSERT
#undef ASSERT
#endif
#define ASSERT(expr) Assert(expr);  // obsolete

#if _MSC_VER >= 1400
#if defined(DEBUG) || defined(INT_ASSERT)
void __cdecl int_assert(const char *_Message,
                        const char *_File,
                        unsigned _Line);
#define Assert(_Expression) (void)( (!!(_Expression)) || \
  (int_assert(#_Expression, __FILE__, __LINE__), 0) )
#else  // DEBUG || INT_ASSERT
#define Assert(_Expression) ((void)0)
#endif  // DEBUG || INT_ASSERT
#else  // _MSC_VER
#include <assert.h>
#define Assert(expr) assert(expr)
#endif  // _MSC_VER

END_C_DECLS	/* For C++ compatibility */


/* Memory leakage detection */
#if (OS_MARK == 'I')

#if defined(MEMORY_LEAK_DEBUG) && defined(DEBUG)

/* prevent imal redefinitions */
#define IMAL_NO_C_RTL_MACRO
#undef RTL_MEMORY_TO_IMAL
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW

#endif /* MEMORY_LEAK_DEBUG */

#endif /* OS_MARK */

#endif  /* _INTEGRA_H_ */

/*  END of 'integra.h' FILE  */
