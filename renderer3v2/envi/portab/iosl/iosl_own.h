/*
 * FILE NAME            iosl_own.h
 * PURPOSE              Own include file for IOSL extension
 *                      for MSC/WATCOM/SCO/HP-UX platforms
 *
 * SPEC                 Esa             '93.10
 * DESIGN               Esa             '94.02
 * DES. VV              Ezb
 * CODING               Esa             '94.02
 * CODE VV              Ezb
 * TEST                 Esa             '94.02
 * UPDATED              Esa             '94.04.27
 *      (Added IRIX supporting)
 * UPDATED              Esa             '94.07.11
 *      (Added rest platforms supporting)
 * UPDATED              Esa             '95.01.11
 *      (Added LINUX supporting)
 * UPDATED              Esa             '95.01.24
 *      (According Erk's remarks)
 * UPDATED              Esa             '95.04.14
 *      (Added NT supporting)
 * UPDATED              _vr             '95.09.06
 *      (Added WINDOWS support for os_mem_avail
 *       2 disk space functions)
 *
 * SWITCHES             NONE
 * NOTES                SUN, IBM, TITAN were not tested, so
 *                      problem may occur with using IOSL on
 *                      these platforms.
 *
 */

#ifndef _IOSL_OWN_H_
#define _IOSL_OWN_H_

/*** Defines ***/
#define UNIX_SEP            '/'
#define DOS_SEP             '\\'
#define DEF_CHAR            '@'
#define COPY_BUFFER_SIZE    4096    /* Buffer Size for os_copy() */

/* Structure for Error Processing */
typedef struct error_text
  {
  int   error_num;  /* Error Number */
  char *error_txt;  /* Error Text */
  } OS_ERR_TEXT;

/*** Own Variables ***/
extern int iosl_error;

/*** Own Functions Prototypes ***/
extern int iosl_error_code(void);

/*--------------------------- WINDOWS NT -------------------------------*/
#if (OS_MARK == 'I')

VOID iosl_gettime PROT((long *, long *));

/* Redefine system structure to include there additional field */
typedef struct NT_FIND_DATA {
    DWORD dwFileAttributes; 
    FILETIME ftCreationTime; 
    FILETIME ftLastAccessTime; 
    FILETIME ftLastWriteTime; 
    DWORD    nFileSizeHigh; 
    DWORD    nFileSizeLow; 
    DWORD    dwReserved0; 
    DWORD    dwReserved1; 
    TCHAR    cFileName[ MAX_PATH ]; 
    TCHAR    cAlternateFileName[ 14 ];
    HANDLE   hdir;                       /* <--- this field added */
} NT_FIND_DATA; 

/*** System Includes ***/
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <io.h>
#include <time.h>
#include <direct.h>
#include <process.h>
#include <signal.h>
#include <dos.h>
#include <errno.h>
#include <sys\types.h>
#include <sys\stat.h>
#include <sys\utime.h>
#include <sys\timeb.h>

#endif /* (OS_MARK == 'I') */

/*----------------------------- HP-UX ----------------------------------*/
#if (OS_MARK == 'H')                            /* If HP-UX             */
#include <dirent.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <time.h>
#include <utime.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/vfs.h>
#include <fcntl.h>

/*** Dir Mask for HP-UX mkdir() ***/
#define OS_MKDIR_MASK  (S_IRUSR | S_IWUSR | S_IXUSR | \
                        S_IRGRP | S_IWGRP | S_IXGRP | \
                        S_IROTH | S_IWOTH | S_IXOTH)
#endif                                          /* End of HP-UX switch  */

/*----------------------------- LINUX ----------------------------------*/
#if (OS_MARK == 'L')                            /* If LINUX             */
#include <dirent.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <time.h>
#include <utime.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/sysinfo.h>
#include <sys/wait.h>
#include <sys/vfs.h>
#include <fcntl.h>

/*** Dir Mask for LINUX mkdir() ***/
#define OS_MKDIR_MASK  (S_IRUSR | S_IWUSR | S_IXUSR | \
                        S_IRGRP | S_IWGRP | S_IXGRP | \
                        S_IROTH | S_IWOTH | S_IXOTH)
#endif                                          /* End of LINUX switch  */

/*--------------------------- SCO UNIX ---------------------------------*/
#if (OS_MARK == 'O')                            /* If SCO UNIX          */
#include <dirent.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/param.h>
#include <sys/sysmacros.h>
#include <sys/page.h>
#include <sys/stat.h>
#include <sys/timeb.h>
#include <sys/event.h>
#include <sys/wait.h>
#include <mouse.h>
#include <time.h>
#include <utime.h>
#include <fcntl.h>

/*** Dir Mask for SCO mkdir() ***/
#define OS_MKDIR_MASK  (S_IEXEC | S_IREAD | S_IWRITE | \
                        S_IXGRP | S_IRGRP | S_IWGRP  | \
                        S_IXOTH | S_IROTH | S_IWOTH)
#endif                                          /* End of SCO switch    */

/*----------------------------- IRIX (+ rest) --------------------------*/
#if (OS_MARK == 'U' || \
     OS_MARK == 'N' || OS_MARK == 'R' || OS_MARK == 'T')
                                                /* If IRIX (+ rest)     */
#include <dirent.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <time.h>
#include <utime.h>

#if (OS_MARK == 'R')        /* IBM */
#include <sys/statfs.h>
#endif

#if (OS_MARK == 'U')
#include <sys/immu.h>
#include <sys/swap.h>
#include <sys/sysmp.h>
#include <sys/sysmips.h>
#include <sys/statfs.h>
#endif

#define ptok(x) ((x)<<12)

/*** Dir Mask for IRIX mkdir() ***/
#define OS_MKDIR_MASK  (S_IRUSR | S_IWUSR | S_IXUSR | \
                        S_IRGRP | S_IWGRP | S_IXGRP | \
                        S_IROTH | S_IWOTH | S_IXOTH)

#endif                      /* End of IRIX (+ rest) switch   */

#endif

/*** End of IOSL_OWN.H file ***/
