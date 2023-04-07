// FILE NAME  integra.hpp
// PURPOSE    INTEGRA Company header for C++ files
//
// SPEC       Pnd                               00.11.02
// DESIGN     Pnd                               00.11.02
// DES. VV
// CODING     Pnd                               00.11.02
// CODE VV
// UPDATED    Esa                               01.01.05
//              cross-platform defines, OS_MARK
// UPDATED    Piv, Esa, Eed                     02.07.18
//              Memory leakage detection
//
// Copyright (C) INTEGRA, Inc., 2000-2001. All Rights Reserved.


// The wrap flag is intentionally the same as for old "integra.h"
#ifndef _INTEGRA_H_
#define _INTEGRA_H_

#include <rep_version.hpp>

// Define OS_MARK
#ifndef OS_MARK

#if defined(_WIN32)          // Windows
#define OS_MARK  'I'
#elif defined(__sgi)        // IRIX
#define OS_MARK  'U'
#elif defined(__linux)      // Linux
#define OS_MARK  'L'
#endif

#endif


// Set DEBUG switches
#if defined(DEBUG) || defined (_DEBUG)

// Define DEBUG, _DEBUG, undef NDEBUG
#ifndef DEBUG
#define DEBUG
#endif
#ifndef _DEBUG
#define _DEBUG
#endif
#ifdef NDEBUG
#undef NDEBUG
#endif

#else

// Undef DEBUG, _DEBUG, define NDEBUG, QT_NO_DEBUG
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

#endif

#ifdef INTEGRA_CATIA_DEBUG
#undef NDEBUG
#endif

// Undef macros if any
#ifdef INLINE
#undef INLINE
#endif

#ifdef VIRTUAL
#undef VIRTUAL
#endif

#ifdef STATIC
#undef STATIC
#endif

#ifdef BOOL
#undef BOOL
#endif


// System includes
#include <ctype.h>
#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifndef _WIN32
#include <stdint.h>
#endif


// C code insertions
#define START_C_DECLS extern "C" {
#define END_C_DECLS   }

// Namespace
#define INTEGRA_NAMESPACE_START namespace Integra {
#define INTEGRA_NAMESPACE_END }
INTEGRA_NAMESPACE_START
INTEGRA_NAMESPACE_END

// INLINE macro
#ifdef _WIN32
#define INLINE __forceinline
#else
#define INLINE inline
#endif


// Reserve VIRTUAL, STATIC macros
#define VIRTUAL
#define STATIC
// Only to resolve external symbol in DLLs, not linking lib, where
// the function is defined.
#define FVIRTUAL virtual


// Prefixes for function parameters
#define IN
#define INOUT
#define OUT

// Function prototypes
#define PROT(x) x

// VOID type
#ifndef VOID
#define VOID void
typedef char CHAR;
typedef short SHORT;
typedef long LONG;
#endif


// VPTR type
typedef void *VPTR;

// Unsigned types
typedef unsigned char      BYTE;   // 8 bit
typedef unsigned short     WORD;   // 16 bits

// BYTE type may assume values from 0 to 255
#define  MAX_BYTE   255
#define  MIN_SHORT  (-32767)       // minimum signed short value
#define  MAX_SHORT  32767          // maximum signed short value
#define  MIN_INT    (-2147483647)  // minimum value of an int
#define  MAX_INT    2147483647     // maximum signed int value

// Maximum values for unsigned integer types
#define  MAX_USHORT 0xffff         // 65535U
#define  MAX_UINT   0xffffffff     // 4294967295U

// Maximal and minimal values for INT64 type
#define  MIN_INT64  0x8000000000000000
#define  MAX_INT64  0x7fffffffffffffff

// Maximal value for UINT64 type
#define  MAX_UINT64 0xffffffffffffffff

// Windows
#ifdef _WIN32
  typedef unsigned long       DWORD;  // 32 bits
  typedef __int64             INT64;  // 64 bits
  typedef unsigned __int64   UINT64;  // 64 bits
  #ifdef _WIN64
    typedef unsigned __int64    SIZE_T;
    const SIZE_T MAX_SIZE_T = MAX_INT64;  // 8 EB (intentional 1/2 of max)
  #else
    #if _MSC_VER >= 1300
      typedef __w64 unsigned long SIZE_T;
    #else
      typedef unsigned long       SIZE_T;
    #endif
    const SIZE_T MAX_SIZE_T = MAX_INT;    // 2 GB (intentional 1/2 of max)
  #endif
#endif

// Handles to deal with pointers to functions
typedef void *HANDLE;

// IRIX
#ifdef __sgi
typedef unsigned int       DWORD;  // 32 bits
typedef __int64_t          INT64;  // 64 bits
#endif

// Linux
#ifdef __linux
typedef unsigned int       DWORD;  // 32 bits
typedef int64_t            INT64;  // 64 bits
typedef uint64_t           UINT64; // 64 bits
typedef size_t             SIZE_T;
#ifdef __x86_64
const SIZE_T MAX_SIZE_T = MAX_INT64;  // 8 EB (intentional 1/2 of max)
#else
const SIZE_T MAX_SIZE_T = MAX_INT;    // 2 GB (intentional 1/2 of max)
#endif
#endif


// Type BOOL and its constants
#ifdef _WIN32
typedef int BOOL;
#else
typedef unsigned int BOOL;
#endif

#define TRUE    1
#define FALSE   0
#define YES     TRUE
#define NO      FALSE
#define ON      TRUE
#define OFF     FALSE


// Type OKAY assumes SUCCESS or FAILURE values
typedef int OKAY;
#define SUCCESS   0
#define FAILURE (-1)

// Assert macro
#ifdef ASSERT
#undef ASSERT
#endif
#define ASSERT(expr) Assert(expr);  // obsolete

#if _MSC_VER >= 1400
#if defined(DEBUG) || defined(INT_ASSERT)
START_C_DECLS
void __cdecl int_assert(const char *_Message,
                        const char *_File,
                        unsigned _Line);
END_C_DECLS
#define Assert(_Expression) (void)( (!!(_Expression)) || \
  (int_assert(#_Expression, __FILE__, __LINE__), 0) )
#else // DEBUG || INT_ASSERT
#define Assert(_Expression) ((void)0)
#endif  // DEBUG || INT_ASSERT
#else  // _MSC_VER
#include <assert.h>
#define Assert(expr) assert(expr)
#endif  // _MSC_VER

// Import / export
#define PLUG_EXPORT

// Compiler warning control
#ifdef _WIN32  // WIN32 specific
// Disable "unreferenced formal parameter" warning
#pragma warning(disable : 4100)
// Disable "declaration of variable hides previous local declaration" warning
#pragma warning(disable : 4456)
// Disable "declaration of variable hides function parameter" warning
#pragma warning(disable : 4457)
// Disable "declaration of variable hides class member" warning
#pragma warning(disable : 4458)
// Disable "declaration of variable hides global declaration" warning
#pragma warning(disable : 4459)
// Disable "unreferenced inline function has been removed" warning
#pragma warning(disable : 4514)
// Disable "this call is not inlined" warning
#pragma warning(disable : 4710)
// Disable "function marked as __forceinline not inlined" warning
#pragma warning(disable : 4714)
// Disable "non-portable use of class as an argument to a variadic function" warning
#pragma warning(disable : 4840)
#if _MSC_VER >= 1900
// Disable "an out-of-line definition of a member of a class template cannot have default arguments" warning
#pragma warning(disable : 5037)
#endif
#endif

// String and number functions
#ifdef _WIN32
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
#else  // not _WIN32
#define  _isnan    isnan
#define  _finite   finite
#define  stricmp   strcasecmp
#define  strnicmp  strncasecmp
#define _vsnprintf vsnprintf
#endif

// Other platform-dependent definitions
#ifdef _WIN32
#define OS_BYTE_ORDER        0          // low byte first
#define LIB_TEMPLATE_INSTANCE(Class, Type) template class Class<Type>;
#endif

#ifdef __sgi
#define OS_BYTE_ORDER        1          // high byte first
// Explicit class instantiation
#define LIB_TEMPLATE_INSTANCE(Class, Type) \
Class<Type> *Generate_##Class##_##Type()   \
  {                                        \
  return (Class<Type> *) new Class<Type>;  \
  }
#endif

#ifdef __linux
#define OS_BYTE_ORDER        0          // low byte first
#define LIB_TEMPLATE_INSTANCE(Class, Type) template class Class<Type>;
#endif

// Memory leakage detection
#if (OS_MARK == 'I')
#if defined(MEMORY_LEAK_DEBUG) && defined(DEBUG)

// Prevent imal redefinitions
#define IMAL_NO_C_RTL_MACRO
#undef RTL_MEMORY_TO_IMAL
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif // MEMORY_LEAK_DEBUG

#endif // OS_MARK

#endif //_KLXX_INTEGRA_HPP_
