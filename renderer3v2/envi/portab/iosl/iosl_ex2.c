/* FILE NAME    iosl_ex2.c
 * PURPOSE      OS-dependent function for IOSL
 *
 * DESIGN       Eed                                  '97.07
 * CODING       Eed                                  '97.07
 *
 */

#if (OS_MARK == 'I')
#include "windows.h"
#endif

/* ---------------------------------------------------------------------
 * NAME         os_own_seterrormode
 * PURPOSE      wrapper for SetErrorMode
   -------------------------------------------------------------------*/
void os_own_seterrormode(void)
  {
#if (OS_MARK == 'I')
  SetErrorMode(SEM_FAILCRITICALERRORS);
#endif
  }

/* ---------------------------------------------------------------------
 * NAME         os_own_getlasterror
 * PURPOSE      wrapper for GetLastError
   -------------------------------------------------------------------*/
int os_own_getlasterror(void)
  {
#if (OS_MARK == 'I')
  return GetLastError() == ERROR_NOT_READY;
#else
  return 0;
#endif
  }

/* end of file 'iosl_ex2.c' */
