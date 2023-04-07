/* FILE NAME    iosl.h
 * PURPOSE      Header file for Integra Operating System Library
 *
 * SPEC         J. Kalinowski                         1990.06.06
 * UPDATED      J. Skrzymowski                        1990.08.03
 * UPDATED      S. Andreev                            1993.10.29
 *                      (added IOSL_EXT)
 * UPDATED      S. Andreev                            1994.11.30
 *                      (added OS_SIGBUS, os_getpid(), os_getppid())
 * UPDATED      R. Kujawski, S. Andreev               1995.01.11
 *                      (added new IOSL_EXT: os_getenv(), ...)
 * UPDATED      S. Andreev                            1995.02.28
 *                      (added os_localtime())
 * UPDATED      V. Rochtchine                         1995.09.06
 *                      (added disk space & file attrs functions)
 * UPDATED      E. Denisov                            1996.11.28
 *                      (added multiply timers)
 * UPDATED      E. Denisov                            1997.07.20
 *                      (added memory querying functions)
 *
 * NOTES        Do not modify this file without permission of INTEGRA
 *
 * iosl.h,v
 * Revision 1.5  1995/03/02  07:39:53  ENVI
 * ----------------------------------------------------------------------
 * Committing in portab/integra/include
 *
 * Modified Files:
 *      iosl.h
 * ----------------------------------------------------------------------
 * Updating 95.03.02 by Eym (request from Erk)
 *
 * Revision 1.3  1994/10/03  10:37:52  cvs
 *
 *
 * Revision 1.2  94/07/04  13:59:56  cvs
 *
 *
 * Revision 1.1.1.1  93/09/20  06:10:02  cvs
 * Imported under CVS.
 *
 *    Rev 1.1   03 Aug 1990 12:52:40   FJS
 * Version 1.10 released
 *
 *    Rev 1.0   29 Jun 1990 12:47:02   M(JK)
 * Initial revision.
 *
 */

#ifndef _IOSL_H_
#define _IOSL_H_

/* file attributes */
#define OS_DATTR_FILE    1 /* ordinary file */
#define OS_DATTR_SUBDIR  2 /* subdirectory  */
#define OS_DATTR_SYMLINK  3 /* symbolic link */

/* file modes */     /* by _vr */
/* Own: */
#define OS_ATTR_USR_R    0x100
#define OS_ATTR_USR_W    0x080
#define OS_ATTR_USR_X    0x040
/* Group: */
#define OS_ATTR_GRP_R    0x020
#define OS_ATTR_GRP_W    0x010
#define OS_ATTR_GRP_X    0x008
/* Other: */
#define OS_ATTR_OTR_R    0x004
#define OS_ATTR_OTR_W    0x002
#define OS_ATTR_OTR_X    0x001

/* environment variables maximal length */
#define OS_ENV_VAR_LENGTH 1024

/* signals */
typedef VOID (* OS_SIG_FUNC) PROT((IN int));

#define OS_SIGABRT      SIGABRT
#define OS_SIGFPE       SIGFPE
#define OS_SIGILL       SIGILL
#define OS_SIGINT       SIGINT
#define OS_SIGSEGV      SIGSEGV
#define OS_SIGTERM      SIGTERM
#ifdef SIGBUS
#define OS_SIGBUS       SIGBUS
#else
#define OS_SIGBUS       -17
#endif

#define OS_SIG_DFL      SIG_DFL
#define OS_SIG_IGN      SIG_IGN
#define OS_SIG_ERR      SIG_ERR

/* IOSL errors */
#define  OS_OK                       0
/* Begin of error codes that have their system prototypes */
#define  OS_ERR_EPERM                1
#define  OS_ERR_ENOENT               2
#define  OS_ERR_ESRCH                3
#define  OS_ERR_EINTR                4
#define  OS_ERR_EIO                  5
#define  OS_ERR_ENXIO                6
#define  OS_ERR_E2BIG                7
#define  OS_ERR_ENOEXEC              8
#define  OS_ERR_EBADF                9
#define  OS_ERR_ECHILD               10
#define  OS_ERR_EAGAIN               11
#define  OS_ERR_ENOMEM               12
#define  OS_ERR_EACCES               13
#define  OS_ERR_EFAULT               14
#define  OS_ERR_ENOTBLK              15
#define  OS_ERR_EBUSY                16
#define  OS_ERR_EEXIST               17
#define  OS_ERR_EXDEV                18
#define  OS_ERR_ENODEV               19
#define  OS_ERR_ENOTDIR              20
#define  OS_ERR_EISDIR               21
#define  OS_ERR_EINVAL               22
#define  OS_ERR_ENFILE               23
#define  OS_ERR_EMFILE               24
#define  OS_ERR_ENOTTY               25
#define  OS_ERR_ETXTBSY              26
#define  OS_ERR_EFBIG                27
#define  OS_ERR_ENOSPC               28
#define  OS_ERR_ESPIPE               29
#define  OS_ERR_EROFS                30
#define  OS_ERR_EMLINK               31
#define  OS_ERR_EPIPE                32
#define  OS_ERR_EDOM                 33
#define  OS_ERR_ERANGE               34
#define  OS_ERR_EDEADLK              35
#define  OS_ERR_ENAMETOOLONG         36
#define  OS_ERR_ENOLCK               37
#define  OS_ERR_ENOSYS               38
#define  OS_ERR_ENOTEMPTY            39
#define  OS_ERR_ELOOP                40
#define  OS_ERR_EWOULDBLOCK          41
#define  OS_ERR_ENOMSG               42
#define  OS_ERR_EIDRM                43
#define  OS_ERR_ENOSTR               44
#define  OS_ERR_ENODATA              45
#define  OS_ERR_ETIME                46
#define  OS_ERR_ENOSR                47
#define  OS_ERR_ENONET               48
#define  OS_ERR_EREMOTE              49
#define  OS_ERR_ENOLINK              50
#define  OS_ERR_ECOMM                51
#define  OS_ERR_EPROTO               52
#define  OS_ERR_EOVERFLOW            53
#define  OS_ERR_ENOTUNIQ             54
#define  OS_ERR_EBADFD               55
#define  OS_ERR_EREMCHG              56
#define  OS_ERR_ESTRPIPE             57
#define  OS_ERR_EUSERS               58
#define  OS_ERR_EDESTADDRREQ         59
#define  OS_ERR_ENETDOWN             60
#define  OS_ERR_ENETUNREACH          61
#define  OS_ERR_ENETRESET            62
#define  OS_ERR_ECONNABORTED         63
#define  OS_ERR_ECONNRESET           64
#define  OS_ERR_ENOBUFS              65
#define  OS_ERR_ETIMEDOUT            66
#define  OS_ERR_ECONNREFUSED         67
#define  OS_ERR_EHOSTDOWN            68
#define  OS_ERR_EHOSTUNREACH         69
#define  OS_ERR_ESTALE               70
#define  OS_ERR_EISNAM               71
#define  OS_ERR_EREMOTEIO            72
#define  OS_ERR_EDQUOT               73
/* End of error codes that have their system prototypes */
/* Begin of obsolete error codes */
#define  OS_ERR_NOT_OWNER            OS_ERR_EPERM
#define  OS_ERR_NO_SUCH_FILE_OR_DIR  OS_ERR_ENOENT
#define  OS_ERR_ARG_LIST_TOO_LONG    OS_ERR_E2BIG
#define  OS_ERR_NOT_ENOUGH_MEMORY    OS_ERR_ENOMEM
#define  OS_ERR_PERMISSION_DENIED    OS_ERR_EACCES
#define  OS_ERR_FILE_EXISTS          OS_ERR_EEXIST
#define  OS_ERR_NOT_DIRECTORY        OS_ERR_ENOTDIR
#define  OS_ERR_IS_DIRECTORY         OS_ERR_EISDIR
#define  OS_ERR_TOO_MANY_OPEN_FILES  OS_ERR_EMFILE
#define  OS_ERR_NOT_ENOUGH_SPACE     OS_ERR_ENOSPC
#define  OS_ERR_FILENAME_TOO_LONG    OS_ERR_ENAMETOOLONG
/* End of obsolete error codes */
/* Begin of IOSL-specific error codes (except of OS_OK) */
#define  OS_ERR_NO_MORE_ENTRIES      74
#define  OS_ERR_TIME_NOT_VALID       75
#define  OS_ERR_OTHER                76
/* End of IOSL-specific error codes (except of OS_OK) */
/* Error code absence indicator in a system */
#define NO_SUCH_ERRNO (-41)

/* date & time type */
typedef struct OS_TIME
  {
  int hour;      /* 0 - 23 */
  int minute;    /* 0 - 59 */
  int second;    /* 0 - 59 */
  int year;      /* years since 1900 */
  int month;     /* 0 - 11 */
  int day;       /* 1 - 31 */
  } OS_TIME;

/* for time functions */
typedef long OS_TIME_T;

/* for os_fstat */
typedef struct OS_FINFO
  {
  int       access;   /* 0 - not present, 1 - present     */
  int       attrib;   /* file/subdir                      */
  long      size;     /* file size in bytes (0 if subdir) */
  OS_TIME   time;     /* date & time of last file update  */
  OS_TIME_T atime;    /* time of last file update in secs */
  } OS_FINFO;

 typedef struct OS_STATFS_TAG
          {
          long block_size;    /* minimal block size for I/O operations */
          long total_blocks;  /* size of filesystem in blocks */
          long free_blocks;   /* free blocks in filesystem */
          } OS_STATFS;

/* for directory maintenance */
typedef struct OS_DIR
  {
  VPTR fdata;
  int  entries;
  } OS_DIR;

/* for multiply timers */
typedef VPTR OS_TIMER;

/*** Standard IOSL functions prototypes ***/
extern char     *iosl_version    PROT((VOID));
extern FILE     *os_fopen        PROT((IN char *, IN char *));
extern int       os_fclose       PROT((IN FILE *));
extern int       os_remove       PROT((IN char *));
extern int       os_rename       PROT((IN char *, IN char *));
extern int       os_copy         PROT((IN char *, IN char *));
extern VOID      os_fstat        PROT((IN char *, OUT OS_FINFO *));
extern VOID      os_lstat        PROT((IN char *, OUT OS_FINFO *));
extern int       os_touch        PROT((IN char *, IN OS_TIME *));

extern OS_DIR   *os_opendir      PROT((IN char *));
extern OS_DIR   *os_readdir      PROT((IN OS_DIR *));
extern VOID      os_closedir     PROT((IN OS_DIR *));
extern char     *os_dirfilename  PROT((IN OS_DIR *));
extern int       os_dirattrib    PROT((IN OS_DIR *));

extern OS_TIME_T os_time         PROT((INOUT OS_TIME_T *));
extern double    os_difftime     PROT((IN OS_TIME_T, IN OS_TIME_T));
extern OS_TIME  *os_localtime    PROT((IN OS_TIME_T *));

/*** Extended IOSL functions prototypes ***/
extern int       os_mkdir        PROT((IN char *));
extern int       os_chdir        PROT((IN char *));
extern int       os_rmdir        PROT((IN char *));
extern char     *os_cwd          PROT((OUT char *));
extern int       os_system       PROT((IN char *));
extern int       os_run          PROT((IN char *, IN char **));
extern long      os_getpid       PROT((VOID));
extern long      os_getppid      PROT((VOID));
extern VOID    (*os_trap_signal  PROT((IN int, IN VOID(*)(IN int))))
                                       PROT((IN int));
extern int       os_raise        PROT((int));
extern VOID      os_mtime_reset  PROT((VOID));
extern long      os_mtime        PROT((VOID));
extern long      os_mtime_wait   PROT((IN long));
extern char     *os_temp_name    PROT((VOID));
extern VOID      os_assert       PROT((IN char *, IN int));
extern int       os_error        PROT((VOID));
extern int       os_errno        PROT((VOID));
extern char     *os_strerror     PROT((IN int));
extern char     *os_check_name   PROT((IN char *));
extern char     *os_correct_name PROT((const char *));
extern char     *os_getenv       PROT((IN char *));
extern int       os_putenv       PROT((IN char *));
extern char     *os_getpath      PROT((IN char *));
extern char     *os_getname      PROT((const char *));
extern char     *os_getext       PROT((IN char *));
extern int       os_path2name    PROT((IN char *, OUT char *,
                                       OUT char *, OUT char *));
extern int       os_cmppath      PROT((IN char *, IN char *));

/* Memory querying functions */
extern SIZE_T    os_mem_avail      PROT((VOID));
extern SIZE_T    os_mem_phys_free  PROT((VOID));
extern SIZE_T    os_mem_virt_avail PROT((VOID));

/* More Extended IOSL Functions */
extern long     os_free_space    PROT((IN char *));
extern long     os_total_space   PROT((IN char *));
extern int      os_get_file_attr PROT((IN char *, OUT unsigned int *));
extern int      os_set_file_attr PROT((IN char *, IN unsigned int *));
extern OKAY     os_statfs        PROT((IN char *, OUT OS_STATFS *));

/* IOSL multiply timer functions */
extern OS_TIMER  os_timer_create   PROT((VOID));
extern VOID      os_timer_destroy  PROT((IN OS_TIMER));
extern VOID      os_timer_reset    PROT((IN OS_TIMER));
extern long      os_timer_get      PROT((IN OS_TIMER));
extern long      os_timer_wait     PROT((IN OS_TIMER, IN long));

/* Analog of fgets() with dynamically allocated buffer */
extern char *integra_fgets(FILE *stream);

/* Moves the file pointer to a specified location. */
extern int os_fseek(FILE *stream, INT64 offset, int origin);
/* Gets the current position of a file pointer. */
extern INT64 os_ftell(FILE *stream);

#endif

/* END OF 'iosl.h' FILE */
