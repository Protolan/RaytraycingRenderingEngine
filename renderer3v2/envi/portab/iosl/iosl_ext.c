/*
 * FILE NAME            iosl_ext.c
 * PURPOSE              Main modules of IOSL extension
 *                      for MSC/WATCOM/SCO/HP-UX/IRIX platforms
 *
 * SPEC                 Esa             '93.10
 * DESIGN               Esa             '94.02
 * DES. VV              Ezb
 * CODING               Esa             '94.02
 * CODE VV              Ezb
 * TEST                 Esa             '94.02
 * UPDATED              Esa             '94.03.09
 *          (Correction for HP filelength limitation)
 * UPDATED              Esa             '94.04.25
 *          (Small changes for more readable code style)
 * UPDATED              Esa             '94.04.27
 *          (Added IRIX support)
 * UPDATED              Esa             '94.07.11
 *          (Added support for rest platforms)
 * UPDATED              Esa             '94.10.20
 *          (Added support for MS WINDOWS 3.1)
 * UPDATED              Esa             '94.11.30
 *          (Corrected processing os_trap_signal(), os_system(),
 *           added os_getpid(), os_getppid())
 * UPDATED              Esa             '95.01.11
 *          (Added support for LINUX)
 * UPDATED              Esa             '95.01.17
 *          (Added os_copy())
 * UPDATED              Esa             '95.01.24
 *          (According Erk's remarks)
 * UPDATED              Esa             '95.02.28
 *          (Added os_localtime())
 * UPDATED              Esa             '95.04.14
 *          (Added WINDOWS NT support)
 *
 * UPDATED              _vr             '95.08.29
 *          (Added WINDOWS support for os_mem_avail(),
 *           2 disk space functions, and file attribute functions)
 * UPDATED               Esa            '96.02.22
 *          (Corrected os_copy(): keeping file attributes)
 * UPDATED               Evr            '96.07.22
 *          (iosl_convert_separator() made global and corrected
 *           to suppress multiple path separators)
 *
 * UPDATED                Eed           '96.08.14
 *          (os_correct_name() for correct processing
 *           of '/./' and '/../' under DOS,WIN,NT)
 * UPDATED                Eed           '96.11.27
 *          (multiply timers added)
 *
 * UPDATED                Eed           '01.02.01
 *          os_free_space() and os_total_space()
 *          are now NOT wrappers for os_statfs() under Win32
 *          (problem with GetDiskFreeSpace() on large disks)
 *
 * NOTES                  Was not tested for SUN, IBM, TITAN platforms
 *                        Comments for IRIX except os_mem_avail() are
 *                        for SUN, IBM, TITAN also.
 *
 */

#ifdef TIPE
#undef TIPE
#endif

#if (OS_MARK == 'I')
#include <windows.h>
#endif

/*** INTEGRA Includes ***/
#include "integra.h"
#include "iosl.h"

/*** Own Includes ***/
#include "iosl_own.h"
#include "iosl_env.h"

static char *args = NULL;
static char **arg_ptrs = NULL;

/*** Global Module Variables ***/
/*---------------------- MSC, WATCOM C, SCO UNIX -----------------------*/
#if (OS_MARK == 'O')
                                                            /* If SCO   */
static struct timeb OS_mtime_start;

#endif                                                /* End of switch  */

/*------------------------ HP-UX, IRIX and rest ------------------------*/
#if (OS_MARK == 'H' || OS_MARK == 'U' || \
     OS_MARK == 'N' || OS_MARK == 'R' || \
     OS_MARK == 'T' || OS_MARK == 'L')
                                                /* Otherwise: HPUX/IRIX */

static struct timeval OS_mtime_start;
static struct timezone OS_tmp_timezone;

#endif                                                /* End of switch  */

/*---------------------------- WINDOWS NT ------------------------------*/
#if (OS_MARK == 'I')

static long mtime_start_sec, mtime_start_msec;

#endif                                                /* End of switch  */

static BOOL OS_timer_started = FALSE;             /* Ind. of timer(OFF) */

/* Multiply timers */
typedef struct OS_OWN_TIMER
  {
#if (OS_MARK == 'O')
  struct timeb start;
#endif
#if (OS_MARK == 'H' || OS_MARK == 'U' || \
     OS_MARK == 'N' || OS_MARK == 'R' || \
     OS_MARK == 'T' || OS_MARK == 'L')
  struct timeval  start;
  struct timezone timezone;
#endif
#if (OS_MARK == 'I')
  long start_sec, start_msec;
#endif
  BOOL started;
  } OS_OWN_TIMER;

static char fname_chars[] = OS_AVAILABLE_NAME;    /* Acceptable chars   */
char iosl_work_buffer[MAX_PATHNAME_LENGTH + 1];   /* Common work buffer */
int iosl_error;                                   /* IOSL error code    */

/*** Error Table ***/
/* _oh: */
static char no_such_err[] = "The system has no such error";

static OS_ERR_TEXT errtab[] =
  {
  {OS_OK,         "No errors"},
#ifdef EPERM
  {EPERM,                NULL},
#else
  {NO_SUCH_ERRNO, no_such_err},
#endif
#ifdef ENOENT
  {ENOENT,               NULL},
#else
  {NO_SUCH_ERRNO, no_such_err},
#endif
#ifdef ESRCH
  {ESRCH,                NULL},
#else
  {NO_SUCH_ERRNO, no_such_err},
#endif
#ifdef EINTR
  {EINTR,                NULL},
#else
  {NO_SUCH_ERRNO, no_such_err},
#endif
#ifdef EIO
  {EIO,                  NULL},
#else
  {NO_SUCH_ERRNO, no_such_err},
#endif
#ifdef ENXIO
  {ENXIO,                NULL},
#else
  {NO_SUCH_ERRNO, no_such_err},
#endif
#ifdef E2BIG
  {E2BIG,                NULL},
#else
  {NO_SUCH_ERRNO, no_such_err},
#endif
#ifdef ENOEXEC
  {ENOEXEC,              NULL},
#else
  {NO_SUCH_ERRNO, no_such_err},
#endif
#ifdef EBADF
  {EBADF,                NULL},
#else
  {NO_SUCH_ERRNO, no_such_err},
#endif
#ifdef ECHILD
  {ECHILD,               NULL},
#else
  {NO_SUCH_ERRNO, no_such_err},
#endif
#ifdef EAGAIN
  {EAGAIN,               NULL},
#else
  {NO_SUCH_ERRNO, no_such_err},
#endif
#ifdef ENOMEM
  {ENOMEM,               NULL},
#else
  {NO_SUCH_ERRNO, no_such_err},
#endif
#ifdef EACCES
  {EACCES,               NULL},
#else
  {NO_SUCH_ERRNO, no_such_err},
#endif
#ifdef EFAULT
  {EFAULT,               NULL},
#else
  {NO_SUCH_ERRNO, no_such_err},
#endif
#ifdef ENOTBLK
  {ENOTBLK,              NULL},
#else
  {NO_SUCH_ERRNO, no_such_err},
#endif
#ifdef EBUSY
  {EBUSY,                NULL},
#else
  {NO_SUCH_ERRNO, no_such_err},
#endif
#ifdef EEXIST
  {EEXIST,               NULL},
#else
  {NO_SUCH_ERRNO, no_such_err},
#endif
#ifdef EXDEV
  {EXDEV,                NULL},
#else
  {NO_SUCH_ERRNO, no_such_err},
#endif
#ifdef ENODEV
  {ENODEV,               NULL},
#else
  {NO_SUCH_ERRNO, no_such_err},
#endif
#ifdef ENOTDIR
  {ENOTDIR,              NULL},
#else
  {NO_SUCH_ERRNO, no_such_err},
#endif
#ifdef EISDIR
  {EISDIR,               NULL},
#else
  {NO_SUCH_ERRNO, no_such_err},
#endif
#ifdef EINVAL
  {EINVAL,               NULL},
#else
  {NO_SUCH_ERRNO, no_such_err},
#endif
#ifdef ENFILE
  {ENFILE,               NULL},
#else
  {NO_SUCH_ERRNO, no_such_err},
#endif
#ifdef EMFILE
  {EMFILE,               NULL},
#else
  {NO_SUCH_ERRNO, no_such_err},
#endif
#ifdef ENOTTY
  {ENOTTY,               NULL},
#else
  {NO_SUCH_ERRNO, no_such_err},
#endif
#ifdef ETXTBSY
  {ETXTBSY,              NULL},
#else
  {NO_SUCH_ERRNO, no_such_err},
#endif
#ifdef EFBIG
  {EFBIG,                NULL},
#else
  {NO_SUCH_ERRNO, no_such_err},
#endif
#ifdef ENOSPC
  {ENOSPC,               NULL},
#else
  {NO_SUCH_ERRNO, no_such_err},
#endif
#ifdef ESPIPE
  {ESPIPE,               NULL},
#else
  {NO_SUCH_ERRNO, no_such_err},
#endif
#ifdef EROFS
  {EROFS,                NULL},
#else
  {NO_SUCH_ERRNO, no_such_err},
#endif
#ifdef EMLINK
  {EMLINK,               NULL},
#else
  {NO_SUCH_ERRNO, no_such_err},
#endif
#ifdef EPIPE
  {EPIPE,                NULL},
#else
  {NO_SUCH_ERRNO, no_such_err},
#endif
#ifdef EDOM
  {EDOM,                 NULL},
#else
  {NO_SUCH_ERRNO, no_such_err},
#endif
#ifdef ERANGE
  {ERANGE,               NULL},
#else
  {NO_SUCH_ERRNO, no_such_err},
#endif
#ifdef EDEADLK
  {EDEADLK,              NULL},
#else
  {NO_SUCH_ERRNO, no_such_err},
#endif
#ifdef ENAMETOOLONG
  {ENAMETOOLONG,         NULL},
#else
  {NO_SUCH_ERRNO, no_such_err},
#endif
#ifdef ENOLCK
  {ENOLCK,               NULL},
#else
  {NO_SUCH_ERRNO, no_such_err},
#endif
#ifdef ENOSYS
  {ENOSYS,               NULL},
#else
  {NO_SUCH_ERRNO, no_such_err},
#endif
#ifdef ENOTEMPTY
  {ENOTEMPTY,            NULL},
#else
  {NO_SUCH_ERRNO, no_such_err},
#endif
#ifdef ELOOP
  {ELOOP,                NULL},
#else
  {NO_SUCH_ERRNO, no_such_err},
#endif
#ifdef EWOULDBLOCK
  {EWOULDBLOCK,          NULL},
#else
  {NO_SUCH_ERRNO, no_such_err},
#endif
#ifdef ENOMSG
  {ENOMSG,               NULL},
#else
  {NO_SUCH_ERRNO, no_such_err},
#endif
#ifdef EIDRM
  {EIDRM,                NULL},
#else
  {NO_SUCH_ERRNO, no_such_err},
#endif
#ifdef ENOSTR
  {ENOSTR,               NULL},
#else
  {NO_SUCH_ERRNO, no_such_err},
#endif
#ifdef ENODATA
  {ENODATA,              NULL},
#else
  {NO_SUCH_ERRNO, no_such_err},
#endif
#ifdef ETIME
  {ETIME,                NULL},
#else
  {NO_SUCH_ERRNO, no_such_err},
#endif
#ifdef ENOSR
  {ENOSR,                NULL},
#else
  {NO_SUCH_ERRNO, no_such_err},
#endif
#ifdef ENONET
  {ENONET,               NULL},
#else
  {NO_SUCH_ERRNO, no_such_err},
#endif
#ifdef EREMOTE
  {EREMOTE,              NULL},
#else
  {NO_SUCH_ERRNO, no_such_err},
#endif
#ifdef ENOLINK
  {ENOLINK,              NULL},
#else
  {NO_SUCH_ERRNO, no_such_err},
#endif
#ifdef ECOMM
  {ECOMM,                NULL},
#else
  {NO_SUCH_ERRNO, no_such_err},
#endif
#ifdef EPROTO
  {EPROTO,               NULL},
#else
  {NO_SUCH_ERRNO, no_such_err},
#endif
#ifdef EOVERFLOW
  {EOVERFLOW,            NULL},
#else
  {NO_SUCH_ERRNO, no_such_err},
#endif
#ifdef ENOTUNIQ
  {ENOTUNIQ,             NULL},
#else
  {NO_SUCH_ERRNO, no_such_err},
#endif
#ifdef EBADFD
  {EBADFD,               NULL},
#else
  {NO_SUCH_ERRNO, no_such_err},
#endif
#ifdef EREMCHG
  {EREMCHG,              NULL},
#else
  {NO_SUCH_ERRNO, no_such_err},
#endif
#ifdef ESTRPIPE
  {ESTRPIPE,             NULL},
#else
  {NO_SUCH_ERRNO, no_such_err},
#endif
#ifdef EUSERS
  {EUSERS,               NULL},
#else
  {NO_SUCH_ERRNO, no_such_err},
#endif
#ifdef EDESTADDRREQ
  {EDESTADDRREQ,         NULL},
#else
  {NO_SUCH_ERRNO, no_such_err},
#endif
#ifdef ENETDOWN
  {ENETDOWN,             NULL},
#else
  {NO_SUCH_ERRNO, no_such_err},
#endif
#ifdef ENETUNREACH
  {ENETUNREACH,          NULL},
#else
  {NO_SUCH_ERRNO, no_such_err},
#endif
#ifdef ENETRESET
  {ENETRESET,            NULL},
#else
  {NO_SUCH_ERRNO, no_such_err},
#endif
#ifdef ECONNABORTED
  {ECONNABORTED,         NULL},
#else
  {NO_SUCH_ERRNO, no_such_err},
#endif
#ifdef ECONNRESET
  {ECONNRESET,           NULL},
#else
  {NO_SUCH_ERRNO, no_such_err},
#endif
#ifdef ENOBUFS
  {ENOBUFS,              NULL},
#else
  {NO_SUCH_ERRNO, no_such_err},
#endif
#ifdef ETIMEDOUT
  {ETIMEDOUT,            NULL},
#else
  {NO_SUCH_ERRNO, no_such_err},
#endif
#ifdef ECONNREFUSED
  {ECONNREFUSED,         NULL},
#else
  {NO_SUCH_ERRNO, no_such_err},
#endif
#ifdef EHOSTDOWN
  {EHOSTDOWN,            NULL},
#else
  {NO_SUCH_ERRNO, no_such_err},
#endif
#ifdef EHOSTUNREACH
  {EHOSTUNREACH,         NULL},
#else
  {NO_SUCH_ERRNO, no_such_err},
#endif
#ifdef ESTALE
  {ESTALE,               NULL},
#else
  {NO_SUCH_ERRNO, no_such_err},
#endif
#ifdef EISNAM
  {EISNAM,               NULL},
#else
  {NO_SUCH_ERRNO, no_such_err},
#endif
#ifdef EREMOTEIO
  {EREMOTEIO,            NULL},
#else
  {NO_SUCH_ERRNO, no_such_err},
#endif
#ifdef EDQUOT
  {EDQUOT,               NULL},
#else
  {NO_SUCH_ERRNO, no_such_err},
#endif
  {OS_ERR_NO_MORE_ENTRIES, "No more entries"},
  {OS_ERR_TIME_NOT_VALID,   "Not valid time"},
  {OS_ERR_OTHER, "The error is not recognized by current IOSL version"},
  };
/* :_oh */

/*** Static functions prototypes ***/
VOID  iosl_convert_separator PROT((OUT char *, const char *));
static VOID  iosl_delay PROT((IN long));

/************************************************************************
 * NAME         os_copy()                                               *
 * PURPOSE      Copy file to other                                      *
 * ARGUMENTS    IN char *source                                         *
 *              IN char *destination                                    *
 * RETURNS      SUCCESS/FAILURE                                         *
 * EXTERNS      iosl_work_buffer, iosl_error                            *
 * NOTES        NONE                                                    *
 ************************************************************************/
#if (OS_MARK == 'I')
/*-------------------------------- NT ----------------------------------*/
int os_copy(source, destination)
IN char *source;
IN char *destination;
  {
  static char dst_file_name[MAX_PATHNAME_LENGTH + 1];
  char *src_file_name;
  int result;

  errno = 0;

  /*** Convert Path Separators ***/
  src_file_name = iosl_work_buffer;
  iosl_convert_separator(src_file_name, source);
  iosl_convert_separator(dst_file_name, destination);

  /*** Copy File ***/
  result = CopyFile(src_file_name, dst_file_name, FALSE);

  if (result != TRUE)
    {
    iosl_error = iosl_error_code();
    return FAILURE;
    }

  iosl_error = OS_OK;
  return SUCCESS;
  } /* End of os_copy() */
#else
/*------------------------------ NOT NT ________________________________*/
/************************************************************************
 * NAME         os_copy()                                               *
 * PURPOSE      Copy file to other                                      *
 * ARGUMENTS    IN char *source                                         *
 *              IN char *destination                                    *
 * RETURNS      SUCCESS/FAILURE                                         *
 * EXTERNS      iosl_work_buffer, iosl_error                            *
 * NOTES        NONE                                                    *
 ************************************************************************/
int os_copy(source, destination)
IN char *source;
IN char *destination;
  {
  static char buffer[COPY_BUFFER_SIZE];
  FILE *fpin, *fpout;
  int result;
  unsigned int attribute;
  OS_FINFO finfo;

  errno = 0;

  /*** Convert Path Separators ***/
  iosl_convert_separator(iosl_work_buffer, source);

  /*** Open Source File for Reading ***/
  fpin = os_fopen(iosl_work_buffer, "rb");
  if (fpin == NULL)        /* Error opening */
    {
    iosl_error = iosl_error_code();
    return FAILURE;
    }

  os_get_file_attr(iosl_work_buffer, &attribute);

  /*** Get Source File Info ***/
  os_fstat(iosl_work_buffer, &finfo);

  /*** Convert Path Separators ***/
  iosl_convert_separator(iosl_work_buffer, destination);

  /*** Create Destination File ***/
  fpout = os_fopen(iosl_work_buffer, "wb");
  if (fpout == NULL)       /* Error creation */
    {
    result = iosl_error_code();
    os_fclose(fpin);
    iosl_error = result;
    return FAILURE;
    }

  /*** Copy Source to Destination ***/
  while ((result = fread(buffer, 1, COPY_BUFFER_SIZE, fpin)) != 0)
    {
    if (errno != 0)        /* Error reading */
      {
      result = iosl_error_code();
      os_fclose(fpout);
      os_fclose(fpin);
      iosl_error = result;
      return FAILURE;
      }
    if (fwrite(buffer, 1, result, fpout) != result) /* Error writing */
      {
      result = iosl_error_code();
      os_fclose(fpout);
      os_fclose(fpin);
      iosl_error = result;
      return FAILURE;
      }
    }

  /*** Close Source/Destination Files ***/
  os_fclose(fpout);
  os_fclose(fpin);

  /*** Update Destination File Info ***/
  os_touch(iosl_work_buffer, &finfo.time);
  os_set_file_attr(iosl_work_buffer, &attribute);

  iosl_error = OS_OK;
  return SUCCESS;
  } /* End of os_copy() */

#endif

/************************************************************************
 * NAME         os_mkdir()                                              *
 * PURPOSE      Create directory                                        *
 * ARGUMENTS    IN char *dir_name (directory name)                      *
 * RETURNS      SUCCESS/FAILURE                                         *
 * EXTERNS      iosl_work_buffer, iosl_error                            *
 * NOTES        NONE                                                    *
 ************************************************************************/
#if (OS_MARK == 'I') /* Win32 */
int os_mkdir(dir_name)
IN char *dir_name;
  {
  int result;

  /*** Check dir_name for NULL pointer ***/
  if (dir_name == NULL)
    {
    iosl_error = OS_ERR_NO_SUCH_FILE_OR_DIR;
    return FAILURE;
    }

  /*** Convert Path Separators ***/
  iosl_convert_separator(iosl_work_buffer, dir_name);

  errno = 0;

  /*** Create Directory by mkdir() ***/
  if (mkdir(iosl_work_buffer) == 0)
    result = SUCCESS;
  else
    result = FAILURE;

  /*** Set Error Code ***/
  iosl_error = iosl_error_code();

  return result;
  } /* End of os_mkdir() */

#else /* Otherwise: UNIX */

int os_mkdir(dir_name)
IN char *dir_name;
  {
  int result;

  /*** Check dir_name for NULL pointer ***/
  if (dir_name == NULL)
    {
    iosl_error = OS_ERR_NO_SUCH_FILE_OR_DIR;
    return FAILURE;
    }

  /*** Convert Path Separators ***/
  iosl_convert_separator(iosl_work_buffer, dir_name);

  errno = 0;

  /*** Create Directory by mkdir() ***/
  if (mkdir(iosl_work_buffer, OS_MKDIR_MASK) == 0)
    result = SUCCESS;
  else
    result = FAILURE;

  /*** Set Error Code ***/
  iosl_error = iosl_error_code();

  return result;
  } /* End of os_mkdir() */

#endif                                          /* End of switch        */

/************************************************************************
 * NAME         os_chdir()                                              *
 * PURPOSE      Change Current Working Directory                        *
 * ARGUMENTS    IN char *dir_name (directory name)                      *
 * RETURNS      SUCCESS/FAILURE                                         *
 * EXTERNS      iosl_work_buffer, iosl_error                            *
 * NOTES        NONE                                                    *
 ************************************************************************/
int os_chdir(dir_name)
IN char *dir_name;
  {
  char *dir_name_ptr;
  int result;

  /*** Check dir_name for NULL pointer ***/
  if (dir_name == NULL)
    {
    iosl_error = OS_ERR_NO_SUCH_FILE_OR_DIR;
    return FAILURE;
    }

  /*** Convert Path Separators ***/
  iosl_convert_separator(iosl_work_buffer, dir_name);

  dir_name_ptr = iosl_work_buffer;

  /*** Change Current Working Directory by chdir() ***/
  errno = 0;
  if (chdir(dir_name_ptr) == 0)
    result = SUCCESS;
  else
    result = FAILURE;

  /*** Set Error Code ***/
  iosl_error = iosl_error_code();

  return result;
  } /* End of os_chdir() */

/************************************************************************
 * NAME         os_rmdir()                                              *
 * PURPOSE      Remove Directory                                        *
 * ARGUMENTS    IN char *dir_name (directory name)                      *
 * RETURNS      SUCCESS/FAILURE                                         *
 * EXTERNS      iosl_work_buffer, iosl_error                            *
 * NOTES        NONE                                                    *
 ************************************************************************/
#if (OS_MARK == 'I')/* Win32 */

int os_rmdir(dir_name)
IN char *dir_name;
  {
  int result;

  /*** Check dir_name for NULL pointer ***/
  if (dir_name == NULL)
    {
    iosl_error = OS_ERR_NO_SUCH_FILE_OR_DIR;
    return FAILURE;
    }

  /*** Convert Path Separators ***/
  iosl_convert_separator(iosl_work_buffer, dir_name);

  /*** Remove Directory by rmdir() ***/
  errno = 0;
  if (rmdir(iosl_work_buffer) == 0)
    result = SUCCESS;
  else
    result = FAILURE;

  /*** Set Error Code ***/
  if (result == SUCCESS)
    iosl_error = OS_OK;
  else
    iosl_error = OS_ERR_OTHER;

  return result;
  } /* End of os_rmdir() */

#else /* Otherwise: UNIX */

int os_rmdir(dir_name)
IN char *dir_name;
  {
  int result;

  /*** Check dir_name for NULL pointer ***/
  if (dir_name == NULL)
    {
    iosl_error = OS_ERR_NO_SUCH_FILE_OR_DIR;
    return FAILURE;
    }

  /*** Convert Path Separators ***/
  iosl_convert_separator(iosl_work_buffer, dir_name);

  /*** Remove Directory by rmdir() ***/
  errno = 0;
  if (rmdir(iosl_work_buffer) == 0)
    result = SUCCESS;
  else
    result = FAILURE;

  /*** Set Error Code ***/
  iosl_error = iosl_error_code();

  return result;
  } /* End of os_rmdir() */

#endif                                          /* End of switch        */

/************************************************************************
 * NAME         os_cwd()                                                *
 * PURPOSE      Return Current Working Directory                        *
 * ARGUMENTS    OUT char *dir_name (directory name)                     *
 * RETURNS      Pointer to buffer with CWD                              *
 * EXTERNS      iosl_error                                              *
 * NOTES        NONE                                                    *
 ************************************************************************/
char *os_cwd(dir_name)
OUT char *dir_name;
  {
  static char iosl_buffer[MAX_PATHNAME_LENGTH * 2 + 2];

  /*** Receive CWD by getcwd() ***/
  errno = 0;
  if (getcwd(iosl_buffer, MAX_PATHNAME_LENGTH) == NULL)
    {
    iosl_error = iosl_error_code();
    return NULL;
    }

#if (OS_MARK == 'I')
    {
    char *q;

    q = iosl_buffer;
    while (*q != '\0')
      {
      *q = (char)tolower((int)(*q));            /* Convert CWD to lower */
      q++;
      }
    }
#endif

  /*** Check if dir_name is not NULL ***/
  if (dir_name != NULL)
    {
    strcpy(dir_name, iosl_buffer);
    }

  /*** Set Error Code ***/
  iosl_error = OS_OK;

  return iosl_buffer;
  } /* End of os_cwd() */


#if (OS_MARK == 'I') /* Win32 */
/****************************************************************************
 * NAME      os_own_disk_space                                              *
 * PURPOSE   get free and total file system space using Win32 API           *
 * ARGUMENTS dir - path to directory                                        *
 *           free - free Kilobytes available to caller                      *
 *           total - total Kilobytes available to caller                    *
 * RETURNS  SUCESS/FAILURE                                                  *
 * EXTERNS  iosl_error                                                      *
 * NOTES    Eed                                              01.02.01       *
 *          function introduced due to problems with GetDiskFreeSpace()     *
 ****************************************************************************/
static OKAY os_own_disk_space(IN char *dir, OUT long *free, OUT long *total)
  {
  ULARGE_INTEGER free_to_caller, total_bytes;
  INT64 size64;

  if (!GetDiskFreeSpaceEx(dir, &free_to_caller, &total_bytes, NULL))
    {
    iosl_error = OS_ERR_NO_SUCH_FILE_OR_DIR;
    return FAILURE;
    }

  if (free != NULL)
    {
    size64 = free_to_caller.QuadPart;
    size64 /= 1024;
    *free = (long)size64;
    }

  if (total != NULL)
    {
    size64 = total_bytes.QuadPart;
    size64 /= 1024;
    *total = (long)size64;
    }

  return SUCCESS;
  } /* End of os_own_disk_space() */
#endif

/* _oh: */
/****************************************************************************
 * NAME   os_free_space                                                     *
 * PURPOSE  get free file system space                                      *
 * ARGUMENTS  dir - path to directory                                       *
 * RETURNS  free file system space in Kbytes or FAILURE if invalid path     *
 * EXTERNS  iosl_error                                                      *
 ****************************************************************************
 */
long os_free_space(dir)
IN char *dir;
  {
  long       free_sp; /* for Kbyte output transform */

#if (OS_MARK == 'I') /* Win32 */

  if (os_own_disk_space(dir, &free_sp, NULL) == FAILURE)
    return FAILURE;

#else /* UNIX */

  OS_STATFS     info;

  iosl_error = OS_OK;
  if (os_statfs(dir, &info) != SUCCESS)
    {
    iosl_error = os_errno();
    return FAILURE;
    }
  {
  long max_val, min_val;

  if (info.block_size > info.free_blocks)
    {
    max_val = info.block_size;
    min_val = info.free_blocks;
    }
  else
    {
    max_val = info.free_blocks;
    min_val = info.block_size;
    }
  if (max_val > 1024L)
    free_sp = (max_val / 1024L) * min_val +
              (max_val % 1024L) * min_val / 1024L;   /* to avoid carry */
  else
    free_sp = (max_val * min_val) / 1024L;
  }

#endif

  return free_sp;
  } /* End of os_free_space */

/*
 ***************************************************************************
 * NAME   os_total_space                                                   *
 * PURPOSE  gets file system capacity                                      *
 * ARGUMENTS  dir - path to directory                                      *
 * RETURNS  file system capacity in Kbytes or FAILURE if invalid path      *
 * EXTERNS  iosl_error                                                     *
 ***************************************************************************
 */
long os_total_space(dir)
IN char *dir;
  {
  long       total_sp; /* for Kbyte output transform */

#if (OS_MARK == 'I') /* Win32 */

  if (os_own_disk_space(dir, NULL, &total_sp) == FAILURE)
    return FAILURE;

#else /* UNIX */

  OS_STATFS      info;

  iosl_error = OS_OK;
  if (os_statfs(dir, &info) != SUCCESS)
    {
    iosl_error = os_errno();
    return FAILURE;
    }
  {
  long max_val, min_val;

  if (info.block_size > info.total_blocks)
    {
    max_val = info.block_size;
    min_val = info.total_blocks;
    }
  else
    {
    max_val = info.total_blocks;
    min_val = info.block_size;
    }
  if (max_val > 1024L)
    total_sp = (max_val / 1024L) * min_val +
               (max_val % 1024L) * min_val / 1024L;  /* to avoid carry */
  else
    total_sp = (max_val * min_val) / 1024L;
  }

#endif

  return total_sp;
  } /* End of os_total_space */
/* :_oh */

/* _vr: */
/* ---------------------------------------------------------------------
 * NAME         os_get_file_attr
 * PURPOSE        gets file access attributes
 * ARGUMENTS        name - filename with path;
 *                mode - variable to store combination of attributes
 * RETURNS        SUCCESS/FAILURE
 * EXTERNS        iosl_work_buffer, iosl_error
 ---------------------------------------------------------------------*/
/* -------------------------- All platforms ------------------------- */
int os_get_file_attr(name, mode)
IN char *name;
OUT unsigned int *mode;
  {
  struct stat file_stat;

  iosl_convert_separator(iosl_work_buffer, name);

  /* Get file info */
  if (stat(iosl_work_buffer, &file_stat) != 0)
    {
    iosl_error = iosl_error_code();
    return FAILURE;
    }

  *mode = (unsigned int)file_stat.st_mode;

  return SUCCESS;
  }

/* ---------------------------------------------------------------------
 * NAME         os_set_file_attr
 * PURPOSE        sets file access attributes
 * ARGUMENTS        name - filename with path;
 *                mode - combination of attributes in WORD format
 * RETURNS        SUCCESS/FAILURE
 * EXTERNS        iosl_work_buffer, iosl_error
 ---------------------------------------------------------------------*/
/* -------------------------- All platforms ------------------------- */
int os_set_file_attr(name ,mode)
IN char *name;
IN unsigned int *mode;
  {
  iosl_convert_separator(iosl_work_buffer, name);

  /* Change access permission */
  if (chmod(iosl_work_buffer, *mode) != 0)
    {
    iosl_error = iosl_error_code();
    return FAILURE;
    }

  return SUCCESS;
  }
/* :_vr */
/* _oh: */
/*
 ************************************************************************
 * NAME       os_statfs()                                               *
 * PURPOSE    Get filesystem info                                       *
 * ARGUMENTS  IN char *path       - path to any file within FS          *
 *            OUT OS_STATFS *info - pointer to the place for FS info    *
 * RETURNS    SUCCESS/FAILURE                                           *
 * EXTERNS    iosl_work_buffer, iosl_error                              *
 * NOTES      NONE                                                      *
 ************************************************************************
 */
OKAY os_statfs(path, info)
IN  char      *path;
OUT OS_STATFS *info;
  {
#if (OS_MARK == 'I') /* Win32 */

  DWORD sect_per_cluster;
  DWORD bytes_per_sector;
  DWORD free_clusters;
  DWORD total_clusters;

  iosl_error = OS_OK;
  /* If path is NULL, get current directory */
  if (path == NULL)
    {
    if (os_cwd(iosl_work_buffer) == NULL)
      {
      iosl_error = os_errno();
      return FAILURE;
      }
    }
  else
    {
    iosl_convert_separator(iosl_work_buffer, path);
      /* Check if user specified directory exists */
      {
      register int    i,j;
      OS_FINFO       finf;

      for(i = 0; TRUE; i++)
        {
        if (iosl_work_buffer[i] == '\0')
          {
          j = ((i > 0) ? i - 1 : i);
          break;
          }
        }
      if (j > 0)
        {
        while (iosl_work_buffer[j] == PATH_SEPARATOR && j > 0)
          --j;
        switch(iosl_work_buffer[j])
          {
          case ':' :
            iosl_work_buffer[j + 1] = PATH_SEPARATOR;
            iosl_work_buffer[j + 2] = '\0';
            break;
          default :
            iosl_work_buffer[j + 1] = '\0';
          } /* End of switch */
        }
      os_fstat(iosl_work_buffer, &finf);
      if (finf.access == 0)
        {
        iosl_error = os_errno();
        return FAILURE;
        }
      } /* End of directory exist checking */
    } /* End of if-else */
  /* Get root path */
  {
  char   *cc;

  cc = strchr(iosl_work_buffer, (int)':');
  /* If drive letter is not yet specified, get current directory */
  if (((cc == NULL) ? TRUE : (*cc != ':' || isalpha(*(cc - 1)) == 0)))
    if (os_cwd(iosl_work_buffer) == NULL)
      {
      iosl_error = os_errno();
      return FAILURE;
      }
    else
      {
      cc = strchr(iosl_work_buffer, (int)':');
      }
  *(cc + 1) = PATH_SEPARATOR;
  *(cc + 2) = '\0';
  }
  /* Get disk info */
  if ( GetDiskFreeSpace(iosl_work_buffer, &sect_per_cluster,
         &bytes_per_sector, &free_clusters, &total_clusters) == 0)
    {
    iosl_error = os_errno();
    return FAILURE;
    }
  /* Calculate */
  info->block_size = (long)bytes_per_sector * (long)sect_per_cluster;
  info->total_blocks = (long)total_clusters;
  info->free_blocks = (long)free_clusters;
  return SUCCESS;

#endif
/*-------------------------------- IRIS --------------------------------*/
#if (OS_MARK == 'U')

  struct statfs fsinfo;

  iosl_error = OS_OK;
  /* If path is NULL, get current directory */
  if (path == NULL)
    {
    if (os_cwd(iosl_work_buffer) == NULL)
      {
      iosl_error = os_errno();
      return FAILURE;
      }
    }
  else
    iosl_convert_separator(iosl_work_buffer, path);

  /* Get disk info */
  if (statfs(iosl_work_buffer, &fsinfo, sizeof(struct statfs), 0) != 0)
    {
    iosl_error = os_errno();
    return FAILURE;
    }
  /* Fill structure */
  info->block_size = (long)fsinfo.f_bsize;
  info->total_blocks = (long)fsinfo.f_blocks;
  info->free_blocks = (long)fsinfo.f_bfree;
  return SUCCESS;

#endif
/*------------------------ HP, LINUX, IBM ----------------------------*/
#if (OS_MARK == 'H' || OS_MARK == 'L' || OS_MARK == 'R')

  struct statfs fsinfo;

  iosl_error = OS_OK;
  /* If path is NULL, get current directory */
  if (path == NULL)
    {
    if (os_cwd(iosl_work_buffer) == NULL)
      {
      iosl_error = os_errno();
      return FAILURE;
      }
    }
  else
    iosl_convert_separator(iosl_work_buffer, path);

  /* Get disk info */
  if (statfs(iosl_work_buffer, &fsinfo) != 0)
    {
    iosl_error = os_errno();
    return FAILURE;
    }
  /* Fill structure */
  info->block_size = fsinfo.f_bsize;
  info->total_blocks = fsinfo.f_blocks;
  info->free_blocks = fsinfo.f_bavail;
  return SUCCESS;

#endif
/* -------- SUN, TITAN, SCO UNIX: Not yet implemented -------------*/
#if (OS_MARK == 'N' || OS_MARK == 'T' || OS_MARK == 'O')

#ifdef OS_ERR_ENOSYS
  iosl_error = OS_ERR_ENOSYS;
#else
  iosl_error = OS_ERR_OTHER;
#endif
  return FAILURE;

#endif /* Platform's switch */

  } /* end of os_statfs */

/*
 ************************************************************************
 * NAME       os_lstat()                                                *
 * PURPOSE    Get and store file information                            *
 * ARGUMENTS  IN char *filename  - path name of a file                  *
 *            OUT OS_FINFO *info - pointer to the place for file info   *
 * RETURNS    NONE                                                      *
 * EXTERNS    iosl_error                                                *
 * NOTES      When filename specifies symbolic link, the last one is    *
 *            statted.                                                  *
 ************************************************************************
 */
VOID os_lstat(filename, info)
IN char *filename;
OUT OS_FINFO *info;
  {
#if (OS_MARK == 'I') /* Win32 */

  os_fstat(filename, info);

#else /* UNIX */

  struct stat f_info;
  struct tm *atime;

  iosl_error = OS_OK;
  if (lstat(filename, &f_info) == -1)
    {
    iosl_error = os_errno();
    info->access = 0;
    return;
    }
  info->access = 1;
  if (S_ISLNK(f_info.st_mode))                /* Symbolic link */
    {
    info->attrib = OS_DATTR_SYMLINK;
    info->size = 0L;
    }
  else if (S_ISREG(f_info.st_mode))           /* Regular file */
    {
    info->attrib = OS_DATTR_FILE;
    info->size = (long)f_info.st_size;
    }
  else if (S_ISDIR(f_info.st_mode))           /* Directory */
    {
    info->attrib = OS_DATTR_SUBDIR;
    info->size = 0L;
    }
  else                                        /* Other special files */
    {
    info->attrib = OS_DATTR_FILE;
    info->size = 0L;
    }
  info->atime = f_info.st_mtime;
  atime = localtime(&f_info.st_mtime);
  info->time.hour   = atime->tm_hour;
  info->time.minute = atime->tm_min;
  info->time.second = atime->tm_sec;
  info->time.year   = atime->tm_year;
  info->time.month  = atime->tm_mon;
  info->time.day    = atime->tm_mday;

#endif
  } /* end of os_lstat */
/* :_oh */

/************************************************************************
 * NAME         os_system()                                             *
 * PURPOSE      Run OS system command                                   *
 * ARGUMENTS    IN char *command_string (OS command)                    *
 * RETURNS      SUCCESS/FAILURE                                         *
 * EXTERNS      iosl_error                                              *
 * NOTES        NONE                                                    *
 ************************************************************************/
/*--------------------------- SCO UNIX ---------------------------------*/
#if (OS_MARK == 'O')                            /* For SCO events queue */
int os_system(command_string)
IN char *command_string;
  {

  /*** Check command_string for NULL pointer ***/
  if (command_string == NULL)
    {
    iosl_error = OS_ERR_OTHER;
    return FAILURE;
    }

  /*** Suspend SCO events queue ***/
  ev_suspend();

  /*** Run OS Command by system() ***/
  errno = 0;
  system(command_string);

  /*** Resume SCO events queue ***/
  ev_resume();

  /*** Set Error Code ***/
  iosl_error = iosl_error_code();

  /*** Check Error Code ***/
  if (iosl_error == OS_OK)
    return SUCCESS;
  else
    return FAILURE;
  } /* End of os_system() */

#else /* All except SCO */

int os_system(command_string)
IN char *command_string;
  {

  /*** Check command_string for NULL pointer ***/
  if (command_string == NULL)
    {
    iosl_error = OS_ERR_OTHER;
    return FAILURE;
    }

  /*** Run OS Command by system() ***/
  errno = 0;
  system(command_string);

  /*** Set Error Code ***/
  iosl_error = iosl_error_code();

  /*** Check Error Code ***/
  if (iosl_error == OS_OK)
    return SUCCESS;
  else
    return FAILURE;
  } /* End of os_system() */

#endif                                          /* End of switch        */

/************************************************************************
 * NAME         os_run()                                                *
 * PURPOSE      Execute program                                         *
 * ARGUMENTS    IN char *prog_name (name of program)                    *
 *              IN char *argv[]    (argument list)                      *
 * RETURNS      SUCCESS/FAILURE                                         *
 * EXTERNS      iosl_work_buffer, iosl_error                            *
 * NOTES        argv[0] must be a name of program to run                *
 ************************************************************************/
#if (OS_MARK == 'I') /* Win32 */
int os_run(prog_name, argv)
IN char *prog_name;
IN char *argv[];
  {
  int result;
  int ia, num, len;

  /*** Check prog_name for NULL pointer ***/
  if (prog_name == NULL)
    {
    iosl_error = OS_ERR_OTHER;
    return FAILURE;
    }

  errno = 0;

  /*** Calculate arguments number ang them total length ***/
  for (ia = num = len = 0; argv[ia] != NULL; ia++)
    {
    if (argv[ia][0] == 0)
      continue;

    if (argv[ia][0] == '"' && argv[ia][strlen(argv[ia]) - 1] == '"')
      len += strlen(argv[ia]) + 1;
    else
      len += strlen(argv[ia]) + 4;
    num++;
    }

  /*** Check the arguments number ***/
  if (num == 0)
    {
    iosl_error = OS_ERR_OTHER;
    return FAILURE;
    }

  num++;

  /*** Memory allocation for args and arg_ptrs arrays ***/
  args = (char *)malloc(len * sizeof(char));
  arg_ptrs = (char **)malloc(num * sizeof(char *));
  if ((args == NULL) || (arg_ptrs == NULL))
    {
    free(args);
    free(arg_ptrs);
    iosl_error = OS_ERR_NOT_ENOUGH_MEMORY;
    return FAILURE;
    }

  /*** Fill args and arg_ptrs arrays ***/
  for (ia = num = len = 0; argv[ia] != NULL; ia++)
    {
    if (argv[ia][0] == 0)
      continue;

    arg_ptrs[num] = args + len;
    if (argv[ia][0] == '"' && argv[ia][strlen(argv[ia]) - 1] == '"')
      {
      sprintf(args + len, "%s", argv[ia]);
      len += strlen(argv[ia]) + 1;
      }
    else
      {
      if (argv[ia][strlen(argv[ia]) - 1] == '\\')
        sprintf(args + len, "\"%s \"", argv[ia]);
      else
        sprintf(args + len, "\"%s\"", argv[ia]);
      len += strlen(argv[ia]) + 4;
      }
    num++;
    }

  arg_ptrs[num] = NULL;

  /*** Execute Program by MSC/WATCOM spawnvp() ***/
  result = spawnvp(P_WAIT, prog_name, arg_ptrs);

  /*** Free temporary memory ***/
  free(args);
  free(arg_ptrs);
  args = NULL;
  arg_ptrs = NULL;

  /*** Set Error Code ***/
  iosl_error = iosl_error_code();

  /*** Check Error Code ***/
  if (iosl_error != OS_OK)
    return FAILURE;
  else
    return result;
  } /* End of os_run() */

#else /* UNIX */

int os_run(prog_name, argv)
IN char *prog_name;
IN char *argv[];
  {
  int result;

  /*** Check prog_name for NULL pointer ***/
  if (prog_name == NULL)
    {
    iosl_error = OS_ERR_OTHER;
    return FAILURE;
    }

  errno = 0;

    {
    int i;

    /*** Prepare Command String for Running by system() ***/
    strcpy(iosl_work_buffer, prog_name);
    for (i = 1; argv[i] != NULL; i++)
      {
      strcat(iosl_work_buffer, " ");
      strcat(iosl_work_buffer, argv[i]);
      }

/*--------------------------- SCO UNIX ---------------------------------*/
#if (OS_MARK == 'O')                            /* For SCO events queue */
    ev_suspend();

#endif                                          /* End of SCO switch    */

    /*** Run EXE module by system() function ***/
    result = system(iosl_work_buffer);

/*--------------------------- SCO UNIX ---------------------------------*/
#if (OS_MARK == 'O')                            /* For SCO events queue */
    ev_resume();

#endif                                          /* End of SCO switch    */
    }

  /*** Set Error Code ***/
  iosl_error = iosl_error_code();

  /*** Check Error Code ***/
  if (iosl_error == OS_OK)
    return WEXITSTATUS(result);
  else
    return FAILURE;
  } /* End of os_run() */

#endif                                          /* End of switch        */

/************************************************************************
 * NAME         os_trap_signal()                                        *
 * PURPOSE      Trap signals which aborts the program                   *
 * ARGUMENTS    IN int sig         (signal to trap)                     *
 *              IN VOID (*func)()  (trap function)                      *
 * RETURNS      OS_SIG_ERR/previous value of indicated signal           *
 * EXTERNS      iosl_error                                              *
 * NOTES        NONE                                                    *
 ************************************************************************/
VOID (*os_trap_signal(sig, func)) PROT((int))
IN int sig;
IN VOID (*func) PROT((int));
  {
  VOID (*result) PROT((int));

#if (OS_MARK == 'I')
  /*** Check if SIGBUS come in (not supported) ***/
  if (sig == OS_SIGBUS)
    {
    iosl_error = OS_ERR_OTHER;
    return OS_SIG_ERR;
    }
#endif

  result = signal(sig, func);
  if (result == OS_SIG_ERR)
    iosl_error = OS_ERR_OTHER;
  else
    iosl_error = OS_OK;

  return result;
  } /* End of os_trap_signal() */

/************************************************************************
 * NAME         os_raise()                                              *
 * PURPOSE      Send signal to executing program                        *
 * ARGUMENTS    IN int sig      (signal to send)                        *
 * RETURNS      SUCCESS/FAILURE                                         *
 * EXTERNS      iosl_error                                              *
 * NOTES        NONE                                                    *
 ************************************************************************/
int os_raise(sig)
IN int sig;
  {
  int result;

  errno = 0;
#if (OS_MARK == 'N')
  result = kill(getpid(), sig);
#else
  result = raise(sig);
#endif
  if (result == 0)
    {
    iosl_error = OS_OK;
    return SUCCESS;
    }
  else
    {
    iosl_error = OS_ERR_OTHER;
    return FAILURE;
    }
  } /* End of os_raise() function */

/************************************************************************
 * NAME         os_getpid()                                             *
 * PURPOSE      Return procces ID of calling process                    *
 * ARGUMENTS    NONE                                                    *
 * RETURNS      Process ID                                              *
 * EXTERNS      NONE                                                    *
 * NOTES        NONE                                                    *
 ************************************************************************/
long os_getpid()
  {
  iosl_error = OS_OK;
  return (long)getpid();
  } /* End of os_getpid() function */


/************************************************************************
 * NAME         os_getppid()                                            *
 * PURPOSE      Return parent process ID of calling process             *
 * ARGUMENTS    NONE                                                    *
 * RETURNS      Process ID                                              *
 * EXTERNS      NONE                                                    *
 * NOTES        NONE                                                    *
 ************************************************************************/
long os_getppid()
  {
  iosl_error = OS_OK;
#if (OS_MARK == 'I') /* Win32 */
  return 1L;
#else
  return (long)getppid();
#endif
  } /* End of os_getppid() function */

/************************************************************************
 * NAME         os_localtime()                                          *
 * PURPOSE      Convert calendar time OS_TIME_T into OS_TIME struct     *
 * ARGUMENTS    OS_TIME_T *                                             *
 * RETURNS      Pointer to converted OS_TIME                            *
 * EXTERNS      iosl_error                                              *
 * NOTES        NONE                                                    *
 ************************************************************************/
OS_TIME *os_localtime(timer)
OS_TIME_T *timer;
  {
  static OS_TIME result;
  static time_t tmp_timer;
  struct tm *ansi_result;

  /*** Convert Time to tm Structure ***/
  tmp_timer = (time_t)(*timer);
  ansi_result = localtime(&tmp_timer);

  /*** Save Result till Next Call ***/
  result.hour   = ansi_result->tm_hour;
  result.minute = ansi_result->tm_min;
  result.second = ansi_result->tm_sec;
  result.year   = ansi_result->tm_year;
  result.month  = ansi_result->tm_mon;
  result.day    = ansi_result->tm_mday;

  /*** Set Error Code ***/
  iosl_error = OS_OK;

  /*** Return Result ***/
  return &result;
  } /* End of os_localtime() function */

/************************************************************************
 * NAME         os_mtime_reset()                                        *
 * PURPOSE      Reset timer (milliseconds)                              *
 * ARGUMENTS    NONE                                                    *
 * RETURNS      NONE                                                    *
 * EXTERNS      OS_mtime_start, OS_timer_started, iosl_error            *
 * NOTES        NONE                                                    *
 ************************************************************************/
/*---------------------- SCO UNIX -----------------------*/
#if (OS_MARK == 'O')
VOID os_mtime_reset()
  {
  /*** Set Timer Start Indicator ***/
  OS_timer_started = TRUE;

  /*** Receive Current Time ***/
  ftime(&OS_mtime_start);

  /*** Set Error Code ***/
  iosl_error = OS_OK;
  } /* End of os_mtime_reset() */

#endif                                          /* End of switch        */

/*--------------------------- WINDOWS NT -------------------------------*/
#if (OS_MARK == 'I')
VOID os_mtime_reset()
  {
  /*** Set Timer Start Indicator ***/
  OS_timer_started = TRUE;

  /*** Receive Current Time ***/
  iosl_gettime(&mtime_start_sec, &mtime_start_msec);

  /*** Set Error Code ***/
  iosl_error = OS_OK;
  } /* End of os_mtime_reset() */

#endif                                          /* End of switch        */

/*--------------------------- HP-UX, IRIX ------------------------------*/
#if (OS_MARK == 'H' || OS_MARK == 'U' || \
     OS_MARK == 'N' || OS_MARK == 'R' || \
     OS_MARK == 'T' || OS_MARK == 'L')
                                                /* Otherwise: HPUX/IRIX */
VOID os_mtime_reset()
  {
  /*** Set Timer Start Indicator ***/
  OS_timer_started = TRUE;

  /*** Receive Current Time ***/
  gettimeofday(&OS_mtime_start, &OS_tmp_timezone);

  /*** Set Error Code ***/
  iosl_error = OS_OK;
  } /* End of os_mtime_reset() */

#endif                                          /* End of switch        */

/************************************************************************
 * NAME         os_mtime()                                              *
 * PURPOSE      Get time in millisecons from last timer reset           *
 * ARGUMENTS    NONE                                                    *
 * RETURNS      Time in milliseconds from last timer reset              *
 * EXTERNS      OS_mtime_start, OS_timer_started, iosl_error            *
 * NOTES        NONE                                                    *
 ************************************************************************/
/*---------------------- SCO UNIX -----------------------*/
#if (OS_MARK == 'O')

long os_mtime()
  {
  long millisec;
  struct timeb OS_mtime_end;

  /*** Check if Timer Was Started ***/
  if (OS_timer_started == FALSE)
    return -1L;

  /*** Receive Current Time ***/
  ftime(&OS_mtime_end);

  /*** Calculate a Differences of Times in Milliseconds ***/
  millisec = ((long)OS_mtime_end.time - (long)OS_mtime_start.time) * 1000L +
             ((long)OS_mtime_end.millitm - (long)OS_mtime_start.millitm);

  /*** Set Error Code ***/
  iosl_error = OS_OK;

  return millisec;
  } /* End of os_mtime() */

#endif                                          /* End of switch        */

/*--------------------------- WINDOWS NT -------------------------------*/
#if (OS_MARK == 'I')
long os_mtime()
  {
  long millisec;
  long mtime_end_sec;
  long mtime_end_msec;

  /*** Check if Timer Was Started ***/
  if (OS_timer_started == FALSE)
    return -1L;

  /*** Receive Current Time ***/
  iosl_gettime(&mtime_end_sec, &mtime_end_msec);

  /*** Calculate a Differences of Times in Milliseconds ***/
  millisec = ((long)mtime_end_sec - (long)mtime_start_sec) * 1000L +
             ((long)mtime_end_msec - (long)mtime_start_msec);

  /*** Set Error Code ***/
  iosl_error = OS_OK;

  return millisec;
  } /* End of os_mtime() */

#endif                                          /* End of switch        */

/*-------------------------- HP-UX, IRIX -------------------------------*/
#if (OS_MARK == 'H' || OS_MARK == 'U' || \
     OS_MARK == 'N' || OS_MARK == 'R' || \
     OS_MARK == 'T' || OS_MARK == 'L')
                                                /* Otherwise: HPUX/IRIX */
long os_mtime()
  {
  long millisec;
  struct timeval OS_mtime_end;

  /*** Check if Timer Was Started ***/
  if (OS_timer_started == FALSE)
    return -1L;

  /*** Receive Current Time ***/
  gettimeofday(&OS_mtime_end, &OS_tmp_timezone);

  /*** Calculate a Differences of Times in Milliseconds ***/
  millisec = ((long)OS_mtime_end.tv_sec - (long)OS_mtime_start.tv_sec) * 1000L +
             ((long)OS_mtime_end.tv_usec - (long)OS_mtime_start.tv_usec) / 1000L;

  /*** Set Error Code ***/
  iosl_error = OS_OK;

  return millisec;
  } /* End of os_mtime() */

#endif                                          /* End of switch        */

/************************************************************************
 * NAME         os_mtime_wait()                                         *
 * PURPOSE      Wait a time in milliseconds                             *
 * ARGUMENTS    IN long millicec        Time in milliseconds to wait    *
 * RETURNS      0 or Time Passed Over                                   *
 * EXTERNS      OS_mtime_start, OS_timer_started, iosl_error            *
 * NOTES        NONE                                                    *
 ************************************************************************/
/*---------------------- SCO UNIX -----------------------*/
#if (OS_MARK == 'O')

long os_mtime_wait(millisec)
IN long millisec;
  {
  long millisec_beg;
  struct timeb OS_mtime_end;

  /*** Check if Timer Was Started ***/
  if (OS_timer_started == FALSE)
    return -1L;

  /*** Receive Current Time ***/
  ftime(&OS_mtime_end);

  /*** Calculate a Differences of Times in Milliseconds ***/
  millisec_beg = ((long)OS_mtime_end.time - (long)OS_mtime_start.time) * 1000L +
                 ((long)OS_mtime_end.millitm - (long)OS_mtime_start.millitm);

  /*** Set Error Code ***/
  iosl_error = OS_OK;

  /*** Check if Time is not Already Over ***/
  if (millisec_beg < millisec)
    {
    iosl_delay(millisec - millisec_beg);
    iosl_error = OS_OK;
    return 0L;
    }

  /*** Otherwise: Time is Already Over ***/
  return (millisec_beg - millisec);
  } /* End of os_mtime_wait() */

#endif                                          /* End of switch        */

/*--------------------------- WINDOWS NT -------------------------------*/
#if (OS_MARK == 'I')
long os_mtime_wait(millisec)
IN long millisec;
  {
  long millisec_beg;
  long mtime_end_sec;
  long mtime_end_msec;

  /*** Check if Timer Was Started ***/
  if (OS_timer_started == FALSE)
    return -1L;

  /*** Receive Current Time ***/
  iosl_gettime(&mtime_end_sec, &mtime_end_msec);

  /*** Calculate a Differences of Times in Milliseconds ***/
  millisec_beg = ((long)mtime_end_sec - (long)mtime_start_sec) * 1000L +
                 ((long)mtime_end_msec - (long)mtime_start_msec);

  /*** Set Error Code ***/
  iosl_error = OS_OK;

  /*** Check if Time is not Already Over ***/
  if (millisec_beg < millisec)
    {
    iosl_delay(millisec - millisec_beg);
    iosl_error = OS_OK;
    return 0L;
    }

  /*** Otherwise: Time is Already Over ***/
  return (millisec_beg - millisec);
  } /* End of os_mtime_wait() */

#endif                                          /* End of switch        */

/*-------------------------- HP-UX, IRIX -------------------------------*/
#if (OS_MARK == 'H' || OS_MARK == 'U' || \
     OS_MARK == 'N' || OS_MARK == 'R' || \
     OS_MARK == 'T' || OS_MARK == 'L')
                                                /* Otherwise: HPUX/IRIX */
long os_mtime_wait(millisec)
IN long millisec;
  {
  long millisec_beg;
  struct timeval OS_mtime_end;

  /*** Check if Timer Was Started ***/
  if (OS_timer_started == FALSE)
    return -1L;

  /*** Receive Current Time ***/
  gettimeofday(&OS_mtime_end, &OS_tmp_timezone);

  /*** Calculate a Differences of Times in Milliseconds ***/
  millisec_beg = ((long)OS_mtime_end.tv_sec - (long)OS_mtime_start.tv_sec) * 1000L +
                 ((long)OS_mtime_end.tv_usec - (long)OS_mtime_start.tv_usec) / 1000L;

  /*** Set Error Code ***/
  iosl_error = OS_OK;

  /*** Check if Time is not Already Over ***/
  if (millisec_beg < millisec)
    {
    iosl_delay(millisec - millisec_beg);
    iosl_error = OS_OK;
    return 0L;
    }

  /*** Otherwise: Time is Already Over ***/
  return (millisec_beg - millisec);
  } /* End of os_mtime_wait() */

#endif                                          /* End of switch        */

/************************************************************************
 * NAME         os_timer_create()                                       *
 * PURPOSE      Create timer structure                                  *
 * ARGUMENTS    NONE                                                    *
 * RETURNS      Timer handler (if SUCCESS)                              *
 * NOTES        NONE                                                    *
 ************************************************************************/
OS_TIMER os_timer_create()
  {
  OS_OWN_TIMER *timer;

  timer = (OS_OWN_TIMER *)malloc(sizeof(OS_OWN_TIMER));
  if (timer == NULL)
    {
    iosl_error = OS_ERR_NOT_ENOUGH_MEMORY;
    return NULL;
    }

  timer->started = FALSE;
  iosl_error = OS_OK;
  return (OS_TIMER)timer;
  } /* End of os_timer_create() function */

/************************************************************************
 * NAME         os_timer_destroy()                                      *
 * PURPOSE      Release memory allocated for timer structure            *
 * ARGUMENTS    timer - timer handler                                   *
 * RETURNS      NONE                                                    *
 * NOTES        NONE                                                    *
 ************************************************************************/
VOID os_timer_destroy(timer)
IN OS_TIMER timer;
  {
  if (timer != NULL)
    free((OS_OWN_TIMER *)timer);
  return;
  } /* End of os_timer_destroy() function */

/************************************************************************
 * NAME         os_timer_reset()                                        *
 * PURPOSE      Reset timer (milliseconds)                              *
 * ARGUMENTS    timer - timer handler                                   *
 * RETURNS      NONE                                                    *
 * NOTES        NONE                                                    *
 ************************************************************************/
VOID os_timer_reset(timer)
IN OS_TIMER timer;
  {
  OS_OWN_TIMER *own_timer = (OS_OWN_TIMER *)timer;

  iosl_error = OS_OK;
  if (own_timer == NULL)
    return;

  /*** Receive current time ***/
#if (OS_MARK == 'O')
  ftime(&(own_timer->start));
#endif
#if (OS_MARK == 'I')
  iosl_gettime(&(own_timer->start_sec), &(own_timer->start_msec));
#endif
#if (OS_MARK == 'H' || OS_MARK == 'U' || OS_MARK == 'N' || \
     OS_MARK == 'R' || OS_MARK == 'T' || OS_MARK == 'L')
  gettimeofday(&(own_timer->start), &(own_timer->timezone));
#endif

  own_timer->started = TRUE;
  return;
  } /* End of os_timer_reset() function */

/************************************************************************
 * NAME         os_timer_get()                                          *
 * PURPOSE      Get time in millisecons from last timer reset           *
 * ARGUMENTS    timer - timer handler                                   *
 * RETURNS      Time in milliseconds from last timer reset              *
 * NOTES        NONE                                                    *
 ************************************************************************/
long os_timer_get(timer)
IN OS_TIMER timer;
  {
  long millisec;
  OS_OWN_TIMER *own_timer = (OS_OWN_TIMER *)timer;

  iosl_error = OS_OK;
  if (own_timer == NULL)
    return -1L;

  if (!own_timer->started)
    return -1L;

    {
#if (OS_MARK == 'O')
    struct timeb end;

    /*** Receive current time ***/
    ftime(&end);

    /*** Calculate a differences of times in milliseconds ***/
    millisec = ((long)end.time - (long)own_timer->start.time) * 1000L +
               ((long)end.millitm - (long)own_timer->start.millitm);
#endif
#if (OS_MARK == 'I')
    long end_sec;
    long end_msec;

    /*** Receive current time ***/
    iosl_gettime(&end_sec, &end_msec);

    /*** Calculate a differences of times in milliseconds ***/
    millisec = ((long)end_sec - (long)own_timer->start_sec) * 1000L +
               ((long)end_msec - (long)own_timer->start_msec);
#endif
#if (OS_MARK == 'H' || OS_MARK == 'U' || OS_MARK == 'N' || \
     OS_MARK == 'R' || OS_MARK == 'T' || OS_MARK == 'L')
    struct timeval end;

    /*** Receive current time ***/
    gettimeofday(&end, &(own_timer->timezone));

    /*** Calculate a differences of times in milliseconds ***/
    millisec = ((long)end.tv_sec - (long)own_timer->start.tv_sec) * 1000L +
               ((long)end.tv_usec - (long)own_timer->start.tv_usec) / 1000L;
#endif
    }

  return millisec;
  } /* End of os_timer_get() function */

/************************************************************************
 * NAME         os_timer_wait()                                         *
 * PURPOSE      Wait a time in milliseconds                             *
 * ARGUMENTS    timer - timer handler                                   *
 *              time - Time in milliseconds to wait                     *
 * RETURNS      0 or Time Passed Over                                   *
 * NOTES        NONE                                                    *
 ************************************************************************/
long os_timer_wait(timer, time)
IN OS_TIMER timer;
IN long time;
  {
  long time_beg;
  OS_OWN_TIMER *own_timer = (OS_OWN_TIMER *)timer;

  iosl_error = OS_OK;
  if (own_timer == NULL)
    return -1L;

  if (!own_timer->started)
    return -1L;

    {
#if (OS_MARK == 'O')
    struct timeb end;

    /*** Receive current time ***/
    ftime(&end);

    /*** Calculate a differences of times in milliseconds ***/
    time_beg = ((long)end.time - (long)own_timer->start.time) * 1000L +
               ((long)end.millitm - (long)own_timer->start.millitm);
#endif
#if (OS_MARK == 'I')
    long end_sec;
    long end_msec;

    /*** Receive current time ***/
    iosl_gettime(&end_sec, &end_msec);

    /*** Calculate a differences of times in milliseconds ***/
    time_beg = ((long)end_sec - (long)own_timer->start_sec) * 1000L +
               ((long)end_msec - (long)own_timer->start_msec);
#endif
#if (OS_MARK == 'H' || OS_MARK == 'U' || OS_MARK == 'N' || \
     OS_MARK == 'R' || OS_MARK == 'T' || OS_MARK == 'L')
    struct timeval end;

    /*** Receive current time ***/
    gettimeofday(&end, &(own_timer->timezone));

    /*** Calculate a differences of times in milliseconds ***/
    time_beg = ((long)end.tv_sec - (long)own_timer->start.tv_sec) * 1000L +
               ((long)end.tv_usec - (long)own_timer->start.tv_usec) / 1000L;
#endif
    }

  /*** Check if time is not already over ***/
  if (time_beg < time)
    {
    iosl_delay(time - time_beg);
    return 0L;
    }

  /*** Otherwise: time is already over ***/
  return (time_beg - time);

  } /* End of os_timer_wait() function */

/************************************************************************
 * NAME         os_mem_avail()                                          *
 * PURPOSE      Return RAM available (by IOSL_CFG)                      *
 * ARGUMENTS    NONE                                                    *
 * RETURNS      RAM available                                           *
 * EXTERNS      iosl_error                                              *
 * NOTES        NONE                                                    *
 ************************************************************************/
SIZE_T os_mem_avail()
  {
  long mem_value;

  iosl_env_envstr(IOSL_ENV);

  /*** Set Error Code ***/
  iosl_error = OS_OK;

  /*** Read IOSL_CFG MEM: field ***/
  mem_value = iosl_env_getl(IOSL_MEM_FIELD, IOSL_MEM_DEFAULT);

/*-------------------------- IRIX --------------------------------------*/
#if (OS_MARK == 'U')                            /* For IRIX only        */
  if (mem_value == 0)        /* IOSL_CFG is not set */
    {
    int rinfosz = sysmp(MP_SASZ, MPSA_RMINFO);  /* get memory info */
    struct rminfo r;

    /*** Just from example freevmem.c ***/
    sysmp(MP_SAGET, MPSA_RMINFO, (char *) &r, rinfosz);
    return (SIZE_T)((ptok(r.physmem) + 512.0) / 1024.0);
    }
#endif                                          /* End of IRIX switch   */

/*-------------------------- LINUX -------------------------------------*/
#if (OS_MARK == 'L')                            /* For LINUX only       */
  if (mem_value == 0)        /* IOSL_CFG is not set */
    {
    struct sysinfo sinfo;

    if (sysinfo(&sinfo) == 0);
      return (SIZE_T)(((double)sinfo.totalram * sinfo.mem_unit
                       + 512.0) / 1024.0);
    }
#endif                                          /* End of LINUX switch  */

/*-------------------------- WINDOWS NT --------------------------------*/
#if (OS_MARK == 'I')                            /* For WINDOWS NT only  */
  if (mem_value == 0)        /* IOSL_CFG is not set */
    {
    MEMORYSTATUSEX mem_stat;

    /*** Function from NT's SDK ***/
    mem_stat.dwLength = sizeof(mem_stat);
    GlobalMemoryStatusEx(&mem_stat);
    return (SIZE_T)((min(mem_stat.ullTotalPhys, mem_stat.ullTotalVirtual)
                     + 512.0) / 1024.0);
    }
#endif                                          /* End of NT's switch   */

  return (SIZE_T)mem_value;
  } /* End of os_mem_avail() */

/************************************************************************
 * NAME         os_mem_phys_free                                        *
 * PURPOSE      Return size of free RAM                                 *
 * ARGUMENTS    NONE                                                    *
 ************************************************************************/
SIZE_T os_mem_phys_free()
  {
  SIZE_T mem_value = 0;

  /*** Set Error Code ***/
  iosl_error = OS_OK;

/*-------------------------- IRIX --------------------------------------*/
#if (OS_MARK == 'U')                            /* For IRIX only        */
  {
  int rinfosz = sysmp(MP_SASZ, MPSA_RMINFO);  /* get memory info */
  struct rminfo r;

  /*** Just from example freevmem.c ***/
  sysmp(MP_SAGET, MPSA_RMINFO, (char *) &r, rinfosz);
  mem_value = (SIZE_T)((ptok(r.availrmem) + 512.0) / 1024.0);
  }
#endif                                          /* End of IRIX switch   */

/*-------------------------- LINUX -------------------------------------*/
#if (OS_MARK == 'L')                            /* For LINUX only       */
  {
  struct sysinfo sinfo;

    if (sysinfo(&sinfo) == 0);
      mem_value = (SIZE_T)(((double)sinfo.freeram * sinfo.mem_unit
                           + 512.0) / 1024.0);
  }
#endif                                          /* End of LINUX switch  */

/*-------------------------- WINDOWS NT --------------------------------*/
#if (OS_MARK == 'I')                            /* For WINDOWS NT only  */
  {
  MEMORYSTATUSEX mem_stat;

  /*** Function from NT's SDK ***/
  mem_stat.dwLength = sizeof(mem_stat);
  GlobalMemoryStatusEx(&mem_stat);
  mem_value = (SIZE_T)((min(mem_stat.ullAvailPhys, mem_stat.ullAvailVirtual)
                        + 512.0) / 1024.0);
  }
#endif                                          /* End of NT's switch   */

  if (mem_value == 0)
    /*** Impossible to detect: set error code ***/
    iosl_error = OS_ERR_ENOSYS;

  return mem_value;
  } /* End of os_mem_phys_free() */

/************************************************************************
 * NAME         os_mem_virt_avail                                       *
 * PURPOSE      Return size of virtual memory                           *
 * ARGUMENTS    NONE                                                    *
 * RETURNS      RAM available                                           *
 * EXTERNS      iosl_error                                              *
 * NOTES        NONE                                                    *
 ************************************************************************/
SIZE_T os_mem_virt_avail()
  {
  SIZE_T mem_value = 0;

  /*** Set Error Code ***/
  iosl_error = OS_OK;

/*-------------------------- IRIX --------------------------------------*/
#if (OS_MARK == 'U')                            /* For IRIX only        */
  {
  off_t swap_size;

  swapctl(SC_GETSWAPTOT, &swap_size); /* get swap info */
  mem_value = (SIZE_T)((swap_size + 1.0) / 2.0);
  }
#endif                                          /* End of IRIX switch   */

/*-------------------------- LINUX -------------------------------------*/
#if (OS_MARK == 'L')                            /* For LINUX only       */
  {
  struct sysinfo sinfo;

    if (sysinfo(&sinfo) == 0);
      mem_value = (SIZE_T)(((double)sinfo.totalswap * sinfo.mem_unit
                            + 512.0) / 1024.0);
  }
#endif                                          /* End of LINUX switch  */

/*-------------------------- WINDOWS NT --------------------------------*/
#if (OS_MARK == 'I')                            /* For WINDOWS NT only  */
  {
  MEMORYSTATUSEX mem_stat;

  /*** Function from NT's SDK ***/
  mem_stat.dwLength = sizeof(mem_stat);
  GlobalMemoryStatusEx(&mem_stat);
  mem_value = (SIZE_T)((min(mem_stat.ullTotalVirtual, mem_stat.ullTotalPageFile)
                        + 512.0) / 1024.0);
  }
#endif                                          /* End of NT's switch   */

  if (mem_value == 0)
    /*** Impossible to detect: set error code ***/
    iosl_error = OS_ERR_ENOSYS;

  return mem_value;
  } /* End of os_mem_virt_avail() */

/************************************************************************
 * NAME         os_temp_name()                                          *
 * PURPOSE      Return unique name of temporary file                    *
 * ARGUMENTS    NONE                                                    *
 * RETURNS      Name of TMP file                                        *
 * EXTERNS      iosl_error                                              *
 * NOTES        NONE                                                    *
 ************************************************************************/
#if (OS_MARK == 'I') /* Win32 */
char *os_temp_name()
  {
  static char iosl_buffer[MAX_PATHNAME_LENGTH * 2 + 2];
  char *q;

  /*** Set Error Code ***/
  iosl_error = OS_OK;

  /*** Get CWD ***/
  os_cwd(iosl_buffer);

  /*** If CWD is not root ***/
  if (iosl_buffer[strlen(iosl_buffer) - 1] != PATH_SEPARATOR)
    strcat(iosl_buffer, PATH_SEPARATOR_STR);

  /*** Receive a name of temporary file by tmpnam() ***/
  q = iosl_work_buffer;
  strcpy(q, tmpnam(NULL));

  /* Translate filename to lowercase */
  while (*q != '\0')
    {
    *q = (char)tolower((int)(*q));
    q++;
    }
  q = iosl_work_buffer;

  while (*q == '\\')
    q++;
 
  strcat(iosl_buffer, q);
  return iosl_buffer;
  } /* End of os_temp_name() */

#endif                                          /* End of switch        */

/*--------------------- HP-UX, SCO UNIX, IRIX --------------------------*/
#if (OS_MARK == 'O' || OS_MARK == 'H' || OS_MARK == 'U' || \
     OS_MARK == 'N' || OS_MARK == 'R' || OS_MARK == 'T' || OS_MARK == 'L')
                                                /* Otherwise: UNIX      */
char *os_temp_name()
  {
  static char iosl_buffer[MAX_PATHNAME_LENGTH * 2 + 2];
  char *tempnam_ret;

  /*** Set Error Code ***/
  iosl_error = OS_OK;

  /*** Get CWD ***/
  os_cwd(iosl_buffer);

  /*** Receive a name of temporary file by tempnam() ***/
  if((tempnam_ret = tempnam(iosl_buffer, "")) != NULL)
    {
    strcpy(iosl_work_buffer, tempnam_ret);
    free(tempnam_ret);
    }
  else
    return NULL;

  /*** Convert Path Separator ***/
  iosl_convert_separator(iosl_buffer, iosl_work_buffer);

  return iosl_buffer;
  } /* End of os_temp_name() */

#endif                                          /* End of switch        */

/************************************************************************
 * NAME         os_check_name()                                         *
 * PURPOSE      Check is file name OK                                   *
 * ARGUMENTS    IN char *file_name - File name                          *
 * RETURNS      Pointer to inproper character or NULL                   *
 * EXTERNS      iosl_error                                              *
 * NOTES        NONE                                                    *
 ************************************************************************/
#if (OS_MARK == 'I') /* Win32 */
char *os_check_name(file_name)
IN char *file_name;
  {
#if 0  /* Dummy function, obsolete approach, return NULL always */
  int i, j, start;
  char separator;
  int name_cnt, ext_cnt;
  BOOL count;
  int point_cnt;        /* Counter of extensions */

  iosl_error = OS_ERR_OTHER;
  point_cnt = 0;
  separator = PATH_SEPARATOR;

  /*** Check if DOS Drive is used ***/
  if ((file_name[1] == ':') && (isalpha(file_name[0]) != 0))
    start = 2;        /* Checking only after drive (like C:) */
  else
    start = 0;        /* Checking from the name start */

  /*** Check if [..] or [.\] is used in start of File Name ***/
  if (file_name[start] == '.')
    {
    if (file_name[start + 1] == '.')    /* Case like [..\TMP] */
      start = start + 2;
    else
      {
      if (file_name[start + 1] == '\0') /* Eed's correction for pure "." */
        {
        iosl_error = OS_OK;
        return NULL;
        }
      else
        if (file_name[start + 1] != separator)
          return &file_name[start + 1];  /* Wrong using of [.] in start */
        else
          start = start + 1;                 /* Case like [.\TMP] is OK */
      }
    }

  /*** Check File Name ***/
  count = TRUE; name_cnt = ext_cnt = 0;
  for (i = start; file_name[i] != '\0'; i++)
    {
    /*** If PATH name is too large ***/
    if (i > MAX_PATHNAME_LENGTH)
      return &file_name[MAX_PATHNAME_LENGTH];

    /*** If path separator ***/
    if (file_name[i] == separator)
      {
      if (file_name[i + 1] == '.')      /* Case like [\.TMP] is invalid */
        return &file_name[i + 1];
      point_cnt = 0;        /* Zero counting of extensions */
      count = TRUE;
      name_cnt = 0;         /* Counter of name length */
      if (file_name[i + 1] == separator)       /* Double path separator */
        return &file_name[i + 1];
      else
        continue;
      }

    /*** If extension ([.] sign) ***/
    if (file_name[i] == '.')                /* If [.] sign */
      {
      point_cnt++;
      if (point_cnt > 1)        /* If more than one extension */
        return &file_name[i];
      count = FALSE;
      ext_cnt = 0;
      if (file_name[i + 1] == '.')        /* Double points [..] */
        return &file_name[i + 1];
      else
        continue;
      }

    /*** Count extension or name length ***/
    if (count == TRUE)        /* Calculate name length */
      name_cnt++;
    else                /* Calculate extension length */
      ext_cnt++;

    /*** Check if name is too long ***/
    if (name_cnt > MAX_FILENAME_LENGTH) /* Length too long */
      return &file_name[i];

    /*** Check if extension is too long ***/
    if (ext_cnt > MAX_SUFFIX_LENGTH)        /* Length too long */
      return &file_name[i];

    /*** Check for ALPHA or NUMERIC chars ***/
    if (((file_name[i] >= 'A') && (file_name[i] <= 'Z')) ||
        ((file_name[i] >= 'a') && (file_name[i] <= 'z')) ||
        ((file_name[i] >= '0') && (file_name[i] <= '9')))
      continue;

    /*** Check for other suitable chars ***/
    for (j = 0; fname_chars[j] != '\0'; j++)
      if (file_name[i] == fname_chars[j])
        break;

    /*** If NO other suitable chars ***/
    if (fname_chars[j] == '\0')
      return &file_name[i];
    }

#endif /* End of '#if 0' */
  iosl_error = OS_OK;
  return NULL;
  } /* End of os_check_name */

#endif                                          /* End of switch        */

/*--------------------- HP-UX, SCO UNIX, IRIX --------------------------*/
#if (OS_MARK == 'O' || OS_MARK == 'H' || OS_MARK == 'U' || \
     OS_MARK == 'N' || OS_MARK == 'R' || OS_MARK == 'T' || OS_MARK == 'L')
                                                /* For old SCO/HP/IRIX  */
char *os_check_name(file_name)
IN char *file_name;
  {
  int i, j, start;
  char separator;
  int name_cnt, ext_cnt;
  BOOL count;
  int flen_cnt;

  flen_cnt = 0;
  iosl_error = OS_ERR_OTHER;
  separator = PATH_SEPARATOR;
  start = 0;        /* Checking from the name start */

  if ((file_name[start] == '.') && (file_name[start + 1] == '.'))
    start = start + 2;        /* Case like [../tmp] */

  /*** Check File Name ***/
  count = TRUE; name_cnt = ext_cnt = 0;
  for (i = start; file_name[i] != '\0'; i++)
    {
    /*** If PATH name is too large ***/
    if (i > MAX_PATHNAME_LENGTH)
      return &file_name[MAX_PATHNAME_LENGTH];

    /*** If path separator ***/
    if (file_name[i] == separator)
      {
      flen_cnt = 0;
      count = TRUE;
      name_cnt = 0;        /* Counter of name length */
      if (file_name[i + 1] == separator)  /* Double path separator */
        return &file_name[i + 1];
      else
        continue;
      }
      flen_cnt++;
      if (flen_cnt > MAX_FILENAME_LENGTH)
        return &file_name[i];

    /*** If extension ([.] sign) ***/
    if (file_name[i] == '.')              /* If [.] sign */
      {
      count = FALSE;
      ext_cnt = 0;
      if (file_name[i + 1] == '.')        /* Double points [..] */
        return &file_name[i + 1];
      else
        continue;
      }

    /*** Count extension or name length ***/
    if (count == TRUE)        /* Calculate name length */
      name_cnt++;
    else                /* Calculate extension length */
      ext_cnt++;

    /*** Check if name is too long ***/
    if (name_cnt > MAX_FILENAME_LENGTH)   /* Length too long */
      return &file_name[i];

    /*** Check if extension is too long ***/
    if (ext_cnt > MAX_SUFFIX_LENGTH)      /* Length too long */
      return &file_name[i];

    /*** Check for ALPHA or NUMERIC chars ***/
    if (((file_name[i] >= 'A') && (file_name[i] <= 'Z')) ||
        ((file_name[i] >= 'a') && (file_name[i] <= 'z')) ||
        ((file_name[i] >= '0') && (file_name[i] <= '9')))
      continue;

    /*** Check for other suitable chars ***/
    for (j = 0; fname_chars[j] != '\0'; j++)
      if (file_name[i] == fname_chars[j])
        break;

    /*** If NO other suitable chars ***/
    if (fname_chars[j] == '\0')
      return &file_name[i];
    }

  iosl_error = OS_OK;
  return NULL;
  } /* End of os_check_name */

#endif                                          /* End of switch        */

/************************************************************************
 * NAME         os_correct_name()                                       *
 * PURPOSE      Correct file name                                       *
 * ARGUMENTS    IN char *file_name; - file name to correct              *
 * RETURNS      Corrected file name                                     *
 * EXTERNS      iosl_error                                              *
 * NOTES        NONE                                                    *
 ************************************************************************/
#if (OS_MARK == 'I') /* Win32 */
char *os_correct_name(file_name)
const char *file_name;
  {
  static char corrected_fname[MAX_PATHNAME_LENGTH + 1];
  int i, j, start, ptr;
  char separator;
  int name_cnt, ext_cnt;
  BOOL count;
  int point_cnt;

  point_cnt = 0;

  /*** Convert Path Separator ***/
  iosl_convert_separator(corrected_fname, file_name);
# if 0  /* Dummy function, obsolete approach, return pointer to filename */
        /* with only converted path separators always */
  ptr = 0;

  /*** Cut Path Name if Too Large ***/
  if (strlen(corrected_fname) > MAX_PATHNAME_LENGTH)
    corrected_fname[MAX_PATHNAME_LENGTH] = '\0';

  separator = PATH_SEPARATOR;

  if ((((corrected_fname[0] >= 'A') && (corrected_fname[0] <= 'Z')) ||
       ((corrected_fname[0] >= 'a') && (corrected_fname[0] <= 'z'))) &&
        (corrected_fname[1] == ':'))            /* If DOS Drive is used */
    {
    start = 2;                                  /* Then let's use it    */
    ptr = ptr + 2;
    }
  else
    start = 0;

  /* Eed's correction for pure "." */
  if (corrected_fname[start] == '.' && corrected_fname[start + 1] == '\0')
    {
    start++;
    ptr++;
    }

  /*** Save cases like [../TMP] ***/
  if ((corrected_fname[start] == '.') &&
      (corrected_fname[start + 1] == '.') &&
      (corrected_fname[start + 2] == separator))
    {
    corrected_fname[ptr] = corrected_fname[ptr + 1] = '.';
    corrected_fname[ptr + 2] = separator;
    start = start + 3;
    ptr = ptr + 3;
    }

  /*** Save cases like [./TMP] **/
  if ((corrected_fname[start] == '.') &&
      (corrected_fname[start + 1] == separator))
    {
    corrected_fname[ptr] = '.';
    corrected_fname[ptr + 1] = separator;
    start = start + 2;
    ptr = ptr + 2;
    }

  /*** Convert Incorrect Characters ***/
  name_cnt = ext_cnt = 0;
  count = TRUE;
  for (i = start; corrected_fname[i] != '\0'; i++)
    {
    /*** Test /./ and /../ cases ***/
    if (corrected_fname[i]     == separator &&
        corrected_fname[i + 1] == '.'       &&
        corrected_fname[i + 2] == separator)
      {
      i += 1;
      ptr += 2;
      continue;
      }

    if (corrected_fname[i]     == separator &&
        corrected_fname[i + 1] == '.'       &&
        corrected_fname[i + 2] == '.'       &&
        corrected_fname[i + 3] == separator)
      {
      i += 2;
      ptr += 3;
      continue;
      }

    /*** If Path Separator ***/
    if (corrected_fname[i] == separator)
      {
      if (corrected_fname[i + 1] == '.')
        corrected_fname[i + 1] = DEF_CHAR;
      point_cnt = 0;
      if (corrected_fname[i + 1] == separator)  /* If double separators */
        corrected_fname[i + 1] = DEF_CHAR;      /* Let's correct that   */
      count = TRUE;        /* Start counting name length */
      name_cnt = 0;
      corrected_fname[ptr] = separator;
      ptr++;
      continue;
      }

    /*** If Extension [.] ***/
    if (corrected_fname[i] == '.')
      {
      point_cnt++;
      if (name_cnt == 0) /* extension w/o name */
        {                                       
        corrected_fname[i] = DEF_CHAR;
        i--;
        continue;
        }
      if (corrected_fname[i + 1] == '.')        /* If double [.]        */
        iosl_work_buffer[i + 1] = DEF_CHAR;     /* Let's correct that   */
      count = FALSE;        /* Start counting extension length */
      ext_cnt = 0;
      corrected_fname[ptr] = '.';
      ptr++;
      continue;
      }

    /*** Counting extension or name length ***/
    if (count == TRUE)        /* Calculate name length */
      {
      name_cnt++;

      /*** Check if name is too long ***/
      if (name_cnt > MAX_FILENAME_LENGTH) /* Name length too long */
        {
        while (corrected_fname[i + 1] != '\0')       /* Loop of cutting */
          {
          if (corrected_fname[i + 1] == separator)
            break;
          if (corrected_fname[i + 1] == '.')
            break;
          i++;
          }
        continue; /* Continue main cycle */
        }
      }
    else                /* Calculate extension length */
      {
      ext_cnt++;

      /*** Check if extension is too long ***/
      if (ext_cnt > MAX_SUFFIX_LENGTH) /* Extension length too long */
        {
        while (corrected_fname[i + 1] != '\0')       /* Loop of cutting */
          {
          if (corrected_fname[i + 1] == separator)
            break;
          if (corrected_fname[i + 1] == '.')
            break;
          i++;
          }
        continue; /* Continue main cycle */
        }
      }

    /*** Check for ALPHA or NUMERIC chars ***/
    if (((corrected_fname[i] >= 'A') && (corrected_fname[i] <= 'Z')) ||
        ((corrected_fname[i] >= 'a') && (corrected_fname[i] <= 'z')) ||
        ((corrected_fname[i] >= '0') && (corrected_fname[i] <= '9')))
      {
      corrected_fname[ptr] = corrected_fname[i];
      ptr++;
      continue;
      }

    /*** Check for suitable chars ***/
    for (j = 0; fname_chars[j] != '\0'; j++)
      if (corrected_fname[i] == fname_chars[j])
        break;

    /*** If NO other suitable chars ***/
    if (fname_chars[j] == '\0') /* Correct improper character */
      corrected_fname[ptr] = DEF_CHAR;
    else                        /* or stay proper character   */
      corrected_fname[ptr] = corrected_fname[i];

    /*** Increase Pointer ***/
    ptr++;
    }

  /*** Append wor buffer by '\0' ***/
  corrected_fname[ptr] = '\0';

#endif /* End of '#if 0' */

  /*** Return result ***/
  iosl_error = OS_OK;

  return corrected_fname;
  } /* End of os_correct_name() */

#endif                                          /* End of switch        */

/*--------------------- HP-UX, SCO UNIX, IRIX --------------------------*/
#if (OS_MARK == 'O' || OS_MARK == 'H' || OS_MARK == 'U' || \
     OS_MARK == 'N' || OS_MARK == 'R' || OS_MARK == 'T' || OS_MARK == 'L')
                                                /* If UNIX              */
char *os_correct_name(file_name)
const char *file_name;
  {
  static char corrected_fname[MAX_PATHNAME_LENGTH + 1];
  int i, j, start, ptr;
  char separator;
  int name_cnt, ext_cnt;
  BOOL count;
  int flen_cnt;

  flen_cnt = 0;

  /*** Convert Path Separator ***/
  iosl_convert_separator(corrected_fname, file_name);
  ptr = 0;

  /*** Cut Path Name if Too Large ***/
  if (strlen(corrected_fname) > MAX_PATHNAME_LENGTH)
    corrected_fname[MAX_PATHNAME_LENGTH] = '\0';

  separator = PATH_SEPARATOR;

  if ((((corrected_fname[0] >= 'A') && (corrected_fname[0] <= 'Z')) ||
       ((corrected_fname[0] >= 'a') && (corrected_fname[0] <= 'z'))) &&
       (corrected_fname[1] == ':'))     /* If DOS Drive under UNIX */
    start = 2;                          /* Then let's omit it      */
  else
    start = 0;

  /*** Save cases like [../TMP] ***/
  if ((corrected_fname[start] == '.') &&
      (corrected_fname[start + 1] == '.') &&
      (corrected_fname[start + 2] == separator))
    {
    corrected_fname[ptr] = corrected_fname[ptr + 1] = '.';
    corrected_fname[ptr + 2] = separator;
    start = start + 3;
    ptr = ptr + 3;
    }

  /*** Save cases like [./TMP] **/
  if ((corrected_fname[start] == '.') &&
      (corrected_fname[start + 1] == separator))
    {
    corrected_fname[ptr] = '.';
    corrected_fname[ptr + 1] = separator;
    start = start + 2;
    ptr = ptr + 2;
    }

  /*** Convert Incorrect Characters ***/
  name_cnt = ext_cnt = 0;
  count = TRUE;
  for (i = start; corrected_fname[i] != '\0'; i++)
    {
    /*** If Path Separator ***/
    if (corrected_fname[i] == separator)
      {
      flen_cnt = 0;
      if (corrected_fname[i + 1] == separator)  /* If double separators */
        corrected_fname[i + 1] = DEF_CHAR;      /* Let's correct that   */
      count = TRUE;        /* Start counting name length */
      name_cnt = 0;
      corrected_fname[ptr] = separator;
      ptr++;
      continue;
      }

      flen_cnt++;
      if (flen_cnt > MAX_FILENAME_LENGTH)
        {
        while (corrected_fname[i + 1] != '\0')       /* Loop of cutting */
          {
          if (corrected_fname[i + 1] == separator)
            break;
          i++;
          }
        continue; /* Continue main cycle */
        }

    /*** If Extension [.] ***/
    if (corrected_fname[i] == '.')
      {
      if (corrected_fname[i + 1] == '.')          /* If double [.]      */
        iosl_work_buffer[i + 1] = DEF_CHAR;       /* Let's correct that */
      count = FALSE;        /* Start counting extension length */
      ext_cnt = 0;
      corrected_fname[ptr] = '.';
      ptr++;
      continue;
      }

    /*** Counting extension or name length ***/
    if (count == TRUE)        /* Calculate name length */
      {
      name_cnt++;

      /*** Check if name is too long ***/
      if (name_cnt > MAX_FILENAME_LENGTH) /* Name length too long */
        {
        while (corrected_fname[i + 1] != '\0')       /* Loop of cutting */
          {
          if (corrected_fname[i + 1] == separator)
            break;
          if (corrected_fname[i + 1] == '.')
            break;
          i++;
          }
        continue; /* Continue main cycle */
        }
      }
    else          /* Calculate extension length */
      {
      ext_cnt++;

      /*** Check if extension is too long ***/
      if (ext_cnt > MAX_SUFFIX_LENGTH) /* Extension length too long */
        {
        while (corrected_fname[i + 1] != '\0')       /* Loop of cutting */
          {
          if (corrected_fname[i + 1] == separator)
            break;
          if (corrected_fname[i + 1] == '.')
            break;
          i++;
          }
        continue; /* Continue main cycle */
        }
      }

    /*** Check for ALPHA or NUMERIC chars ***/
    if (((corrected_fname[i] >= 'A') && (corrected_fname[i] <= 'Z')) ||
        ((corrected_fname[i] >= 'a') && (corrected_fname[i] <= 'z')) ||
        ((corrected_fname[i] >= '0') && (corrected_fname[i] <= '9')))
      {
      corrected_fname[ptr] = corrected_fname[i];
      ptr++;
      continue;
      }

    /*** Check for suitable chars ***/
    for (j = 0; fname_chars[j] != '\0'; j++)
      if (corrected_fname[i] == fname_chars[j])
        break;

    /*** If NO other suitable chars ***/
    if (fname_chars[j] == '\0') /* Correct improper character */
      corrected_fname[ptr] = DEF_CHAR;
    else                        /* or stay proper character   */
      corrected_fname[ptr] = corrected_fname[i];

    /*** Increase Pointer ***/
    ptr++;
    }

  /*** Append wor buffer by '\0' ***/
  corrected_fname[ptr] = '\0';

  /*** Return result ***/
  iosl_error = OS_OK;

  return corrected_fname;
  } /* End of os_correct_name() */

#endif                                          /* End of switch        */

/* ******************************************************************** *
 * NAME        os_assert()
 * PURPOSE     Checks assertion and exits with error code if failed
 * ARGUMENTS   file_name - name of the file error occured in
 *             line_no   - line number where error is situated
 * RETURNS     NONE
 * EXTERNS     NONE
 * NOTES       NONE
 * ******************************************************************** */
VOID os_assert(file_name, line_no)
IN char *file_name;
IN int line_no;
  {
  printf("Assertion failed in file %s, line %i\n", file_name, line_no);
  exit(99);
  }

/************************************************************************
 * NAME   os_error()                                                    *
 * PURPOSE  Return Error Code of last IOSL function call                *
 * ARGUMENTS  NONE                                                      *
 * RETURNS  Error Code                                                  *
 * EXTERNS  iosl_error                                                  *
 * NOTES  NONE                                                          *
 ************************************************************************/
int os_error()
  {
  return iosl_error;
  } /* End of os_error() */

/* _oh: */
/*
 ************************************************************************
 * NAME      os_errno()                                                 *
 * PURPOSE   system error code recognizer                               *
 * ARGUMENTS NONE                                                       *
 * RETURNS   IOSL error code corresponding to current errno value       *
 * EXTERNS   NONE                                                       *
 * NOTES     NONE                                                       *
 ************************************************************************
 */
int os_errno()
  {
  int err_code;

#ifdef EPERM
  if (errno == EPERM)
    {
    err_code = OS_ERR_EPERM;
    }
#endif
#ifdef ENOENT
  else if (errno == ENOENT)
    {
    err_code = OS_ERR_ENOENT;
    }
#endif
#ifdef ESRCH
  else if (errno == ESRCH)
    {
    err_code = OS_ERR_ESRCH;
    }
#endif
#ifdef EINTR
  else if (errno == EINTR)
    {
    err_code = OS_ERR_EINTR;
    }
#endif
#ifdef EIO
  else if (errno == EIO)
    {
    err_code = OS_ERR_EIO;
    }
#endif
#ifdef ENXIO
  else if (errno == ENXIO)
    {
    err_code = OS_ERR_ENXIO;
    }
#endif
#ifdef E2BIG
  else if (errno == E2BIG)
    {
    err_code = OS_ERR_E2BIG;
    }
#endif
#ifdef ENOEXEC
  else if (errno == ENOEXEC)
    {
    err_code = OS_ERR_ENOEXEC;
    }
#endif
#ifdef EBADF
  else if (errno == EBADF)
    {
    err_code = OS_ERR_EBADF;
    }
#endif
#ifdef ECHILD
  else if (errno == ECHILD)
    {
    err_code = OS_ERR_ECHILD;
    }
#endif
#ifdef EAGAIN
  else if (errno == EAGAIN)
    {
    err_code = OS_ERR_EAGAIN;
    }
#endif
#ifdef ENOMEM
  else if (errno == ENOMEM)
    {
    err_code = OS_ERR_ENOMEM;
    }
#endif
#ifdef EACCES
  else if (errno == EACCES)
    {
    err_code = OS_ERR_EACCES;
    }
#endif
#ifdef EFAULT
  else if (errno == EFAULT)
    {
    err_code = OS_ERR_EFAULT;
    }
#endif
#ifdef ENOTBLK
  else if (errno == ENOTBLK)
    {
    err_code = OS_ERR_ENOTBLK;
    }
#endif
#ifdef EBUSY
  else if (errno == EBUSY)
    {
    err_code = OS_ERR_EBUSY;
    }
#endif
#ifdef EEXIST
  else if (errno == EEXIST)
    {
    err_code = OS_ERR_EEXIST;
    }
#endif
#ifdef EXDEV
  else if (errno == EXDEV)
    {
    err_code = OS_ERR_EXDEV;
    }
#endif
#ifdef ENODEV
  else if (errno == ENODEV)
    {
    err_code = OS_ERR_ENODEV;
    }
#endif
#ifdef ENOTDIR
  else if (errno == ENOTDIR)
    {
    err_code = OS_ERR_ENOTDIR;
    }
#endif
#ifdef EISDIR
  else if (errno == EISDIR)
    {
    err_code = OS_ERR_EISDIR;
    }
#endif
#ifdef EINVAL
  else if (errno == EINVAL)
    {
    err_code = OS_ERR_EINVAL;
    }
#endif
#ifdef ENFILE
  else if (errno == ENFILE)
    {
    err_code = OS_ERR_ENFILE;
    }
#endif
#ifdef EMFILE
  else if (errno == EMFILE)
    {
    err_code = OS_ERR_EMFILE;
    }
#endif
#ifdef ENOTTY
  else if (errno == ENOTTY)
    {
    err_code = OS_ERR_ENOTTY;
    }
#endif
#ifdef ETXTBSY
  else if (errno == ETXTBSY)
    {
    err_code = OS_ERR_ETXTBSY;
    }
#endif
#ifdef EFBIG
  else if (errno == EFBIG)
    {
    err_code = OS_ERR_EFBIG;
    }
#endif
#ifdef ENOSPC
  else if (errno == ENOSPC)
    {
    err_code = OS_ERR_ENOSPC;
    }
#endif
#ifdef ESPIPE
  else if (errno == ESPIPE)
    {
    err_code = OS_ERR_ESPIPE;
    }
#endif
#ifdef EROFS
  else if (errno == EROFS)
    {
    err_code = OS_ERR_EROFS;
    }
#endif
#ifdef EMLINK
  else if (errno == EMLINK)
    {
    err_code = OS_ERR_EMLINK;
    }
#endif
#ifdef EPIPE
  else if (errno == EPIPE)
    {
    err_code = OS_ERR_EPIPE;
    }
#endif
#ifdef EDOM
  else if (errno == EDOM)
    {
    err_code = OS_ERR_EDOM;
    }
#endif
#ifdef ERANGE
  else if (errno == ERANGE)
    {
    err_code = OS_ERR_ERANGE;
    }
#endif
#ifdef EDEADLK
  else if (errno == EDEADLK)
    {
    err_code = OS_ERR_EDEADLK;
    }
#endif
#ifdef ENAMETOOLONG
  else if (errno == ENAMETOOLONG)
    {
    err_code = OS_ERR_ENAMETOOLONG;
    }
#endif
#ifdef ENOLCK
  else if (errno == ENOLCK)
    {
    err_code = OS_ERR_ENOLCK;
    }
#endif
#ifdef ENOSYS
  else if (errno == ENOSYS)
    {
    err_code = OS_ERR_ENOSYS;
    }
#endif
#ifdef ENOTEMPTY
  else if (errno == ENOTEMPTY)
    {
    err_code = OS_ERR_ENOTEMPTY;
    }
#endif
#ifdef ELOOP
  else if (errno == ELOOP)
    {
    err_code = OS_ERR_ELOOP;
    }
#endif
#ifdef EWOULDBLOCK
  else if (errno == EWOULDBLOCK)
    {
    err_code = OS_ERR_EWOULDBLOCK;
    }
#endif
#ifdef ENOMSG
  else if (errno == ENOMSG)
    {
    err_code = OS_ERR_ENOMSG;
    }
#endif
#ifdef EIDRM
  else if (errno == EIDRM)
    {
    err_code = OS_ERR_EIDRM;
    }
#endif
#ifdef ENOSTR
  else if (errno == ENOSTR)
    {
    err_code = OS_ERR_ENOSTR;
    }
#endif
#ifdef ENODATA
  else if (errno == ENODATA)
    {
    err_code = OS_ERR_ENODATA;
    }
#endif
#ifdef ETIME
  else if (errno == ETIME)
    {
    err_code = OS_ERR_ETIME;
    }
#endif
#ifdef ENOSR
  else if (errno == ENOSR)
    {
    err_code = OS_ERR_ENOSR;
    }
#endif
#ifdef ENONET
  else if (errno == ENONET)
    {
    err_code = OS_ERR_ENONET;
    }
#endif
#ifdef EREMOTE
  else if (errno == EREMOTE)
    {
    err_code = OS_ERR_EREMOTE;
    }
#endif
#ifdef ENOLINK
  else if (errno == ENOLINK)
    {
    err_code = OS_ERR_ENOLINK;
    }
#endif
#ifdef ECOMM
  else if (errno == ECOMM)
    {
    err_code = OS_ERR_ECOMM;
    }
#endif
#ifdef EPROTO
  else if (errno == EPROTO)
    {
    err_code = OS_ERR_EPROTO;
    }
#endif
#ifdef EOVERFLOW
  else if (errno == EOVERFLOW)
    {
    err_code = OS_ERR_EOVERFLOW;
    }
#endif
#ifdef ENOTUNIQ
  else if (errno == ENOTUNIQ)
    {
    err_code = OS_ERR_ENOTUNIQ;
    }
#endif
#ifdef EBADFD
  else if (errno == EBADFD)
    {
    err_code = OS_ERR_EBADFD;
    }
#endif
#ifdef EREMCHG
  else if (errno == EREMCHG)
    {
    err_code = OS_ERR_EREMCHG;
    }
#endif
#ifdef ESTRPIPE
  else if (errno == ESTRPIPE)
    {
    err_code = OS_ERR_ESTRPIPE;
    }
#endif
#ifdef EUSERS
  else if (errno == EUSERS)
    {
    err_code = OS_ERR_EUSERS;
    }
#endif
#ifdef EDESTADDRREQ
  else if (errno == EDESTADDRREQ)
    {
    err_code = OS_ERR_EDESTADDRREQ;
    }
#endif
#ifdef ENETDOWN
  else if (errno == ENETDOWN)
    {
    err_code = OS_ERR_ENETDOWN;
    }
#endif
#ifdef ENETUNREACH
  else if (errno == ENETUNREACH)
    {
    err_code = OS_ERR_ENETUNREACH;
    }
#endif
#ifdef ENETRESET
  else if (errno == ENETRESET)
    {
    err_code = OS_ERR_ENETRESET;
    }
#endif
#ifdef ECONNABORTED
  else if (errno == ECONNABORTED)
    {
    err_code = OS_ERR_ECONNABORTED;
    }
#endif
#ifdef ECONNRESET
  else if (errno == ECONNRESET)
    {
    err_code = OS_ERR_ECONNRESET;
    }
#endif
#ifdef ENOBUFS
  else if (errno == ENOBUFS)
    {
    err_code = OS_ERR_ENOBUFS;
    }
#endif
#ifdef ETIMEDOUT
  else if (errno == ETIMEDOUT)
    {
    err_code = OS_ERR_ETIMEDOUT;
    }
#endif
#ifdef ECONNREFUSED
  else if (errno == ECONNREFUSED)
    {
    err_code = OS_ERR_ECONNREFUSED;
    }
#endif
#ifdef EHOSTDOWN
  else if (errno == EHOSTDOWN)
    {
    err_code = OS_ERR_EHOSTDOWN;
    }
#endif
#ifdef EHOSTUNREACH
  else if (errno == EHOSTUNREACH)
    {
    err_code = OS_ERR_EHOSTUNREACH;
    }
#endif
#ifdef ESTALE
  else if (errno == ESTALE)
    {
    err_code = OS_ERR_ESTALE;
    }
#endif
#ifdef EISNAM
  else if (errno == EISNAM)
    {
    err_code = OS_ERR_EISNAM;
    }
#endif
#ifdef EREMOTEIO
  else if (errno == EREMOTEIO)
    {
    err_code = OS_ERR_EREMOTEIO;
    }
#endif
#ifdef EDQUOT
  else if (errno == EDQUOT)
    {
    err_code = OS_ERR_EDQUOT;
    }
#endif
  else            /* by default */
    {
    err_code = OS_ERR_OTHER;
    } /* End of if-else construction */
  return err_code;
  } /* End of os_errno */

/*
 ************************************************************************
 * NAME      os_strerror()                                              *
 * PURPOSE   Return error description text                              *
 * ARGUMENTS IN int os_error_code  - IOSL Error Code                    *
 * RETURNS   Pointer to error description                               *
 * EXTERNS   NONE                                                       *
 * NOTES     NONE                                                       *
 ************************************************************************
 */
char *os_strerror(os_error_code)
IN int os_error_code;
  {
  int              err_num,
               err_checked;  /* range checked error code */
  char            *err_txt;
  /* Check code's range: if not valid, return OS_ERR_OTHER description */
  err_checked = ((os_error_code < OS_OK ||
                  os_error_code > OS_ERR_OTHER) ?
    OS_ERR_OTHER : os_error_code);
  err_num = errtab[err_checked].error_num;
  err_txt = errtab[err_checked].error_txt;
  /*  if entries in errtab[] corresponding to errno equivalents
   *  are set to NULL, we take description from the system
   */
  return ((err_txt == NULL) ?
    strerror(err_num) : err_txt);
  } /* End of os_strerror() */
/* :_oh */

/*
 ************************************************************************
 * NAME      os_fseek()                                                 *
 * PURPOSE   Moves the file pointer to a specified location.            *
 * ARGUMENTS IN FILE *stream  - Pointer to FILE structure.              *
 *           IN INT64 offset  - Number of bytes from origin.            *
 *           IN int origin    - Initial position. This argument must be *
 *                              one of the following constants, defined *
 *                              in STDIO.H:                             * 
 *           SEEK_CUR - Current position of file pointer.               *
 *           SEEK_END - End of file.                                    *
 *           SEEK_SET - Beginning of file.                              *
 *                                                                      *
 * RETURNS   If successful, os_fseek returns 0. Otherwise, it returns a *
 *           nonzero value.                                             *
 * EXTERNS   NONE                                                       *
 * NOTES     NONE                                                       *
 ************************************************************************
 */
int os_fseek(FILE *stream, INT64 offset, int origin)
  {
#if (OS_MARK == 'I')
  return _fseeki64(stream, offset, origin);
#else
#error Not implemented
#endif
  }

/*
 ************************************************************************
 * NAME      os_ftell()                                                 *
 * PURPOSE   Gets the current position of a file pointer.               *
 * ARGUMENTS IN FILE *stream  - Pointer to FILE structure.              *
 * RETURNS   Current file position. The position is expressed as an     *
 *           offset relative to the beginning of the stream.            *
 * EXTERNS   NONE                                                       *
 * NOTES     NONE                                                       *
 ************************************************************************
 */
INT64 os_ftell(FILE *stream)
  {
#if (OS_MARK == 'I')
  return _ftelli64(stream);
#else
#error Not implemented
#endif
  }

/************************************************************************/
/*                         INTERNAL IOSL FUNCTIONS                      */
/************************************************************************/

/************************************************************************
 * NAME         iosl_convert_separator()                                *
 * PURPOSE        Convert UNIX2DOS or DOS2UNIX Path Separator           *
 * ARGUMENTS        OUT char *new_path_name, IN char *path_name         *
 * RETURNS        NONE                                                  *
 * EXTERNS        NONE                                                  *
 * NOTES        NONE                                                    *
 ************************************************************************/
#if (OS_MARK == 'I')  /* Win32 */
VOID iosl_convert_separator(new_path_name, path_name)
OUT char *new_path_name;
const char *path_name;
  {
  int i;

  /*** Convert UNIX to DOS or DOS to UNIX Separator ***/
  for (i = 0; path_name[i] != '\0'; i++)
    {
    if (path_name[i] == UNIX_SEP)
      new_path_name[i] = PATH_SEPARATOR;
    else
      new_path_name[i] = path_name[i];
    }

  new_path_name[i] = '\0';
  } /* End of iosl_convert_separator() */

#endif                                          /* End of switch        */

/*--------------------- HP-UX, SCO UNIX, IRIX --------------------------*/
#if (OS_MARK == 'O' || OS_MARK == 'H' || OS_MARK == 'U' || \
     OS_MARK == 'N' || OS_MARK == 'R' || OS_MARK == 'T' || OS_MARK == 'L')
                                                /* Otherwise: UNIX      */
VOID iosl_convert_separator(new_path_name, path_name)
OUT char *new_path_name;
const char *path_name;
  {
  int i;

  /*** Convert UNIX to DOS or DOS to UNIX Separator ***/
  for (i = 0; path_name[i] != '\0'; i++)
    if (path_name[i] == DOS_SEP)
      new_path_name[i] = PATH_SEPARATOR;
    else
      new_path_name[i] = path_name[i];

  new_path_name[i] = '\0';
  } /* End of iosl_convert_separator() */

#endif                                          /* End of switch        */

/************************************************************************
 * NAME         iosl_error_code                                         *
 * PURPOSE      Returns IOSL error code based on 'errno'                *
 * ARGUMENTS    NONE                                                    *
 * RETURNS      IOSL error code                                         *
 * EXTERNS      NONE                                                    *
 * NOTES        NONE                                                    *
 ************************************************************************/
int iosl_error_code()
  {
  int error_code;

  switch (errno)
    {
    case 0:
      error_code = OS_OK;
      break;
    case ENOENT:
      error_code = OS_ERR_NO_SUCH_FILE_OR_DIR;
      break;
    case E2BIG:
      error_code = OS_ERR_ARG_LIST_TOO_LONG;
      break;
    case ENOMEM:
      error_code = OS_ERR_NOT_ENOUGH_MEMORY;
      break;
    case EACCES:
      error_code = OS_ERR_PERMISSION_DENIED;
      break;
    case EEXIST:
      error_code = OS_ERR_FILE_EXISTS;
      break;
    case EMFILE:
      error_code = OS_ERR_TOO_MANY_OPEN_FILES;
      break;
    case ENOSPC:
      error_code = OS_ERR_NOT_ENOUGH_SPACE;
      break;

/*------------------------- MSC, HP-UX, SCO UNIX -----------------------*/
#if (OS_MARK != 'I') /* If not Win32 */
    case EPERM:
      error_code = OS_ERR_NOT_OWNER;
      break;
    case ENOTDIR:
      error_code = OS_ERR_NOT_DIRECTORY;
      break;
    case EISDIR:
      error_code = OS_ERR_IS_DIRECTORY;
      break;
    case ENAMETOOLONG:
      error_code = OS_ERR_FILENAME_TOO_LONG;
      break;

#endif                                          /* End of switch        */
    default:
      error_code = OS_ERR_OTHER;
      break;
    }

  return error_code;
  } /* End of iosl_error_code() */

/* _oh: */
/*
 ************************************************************************
 * NAME       iosl_delay()                                              *
 * PURPOSE    Delay in milliseconds                                     *
 * ARGUMENTS  IN long millisec  - Time to delay in millisec             *
 * RETURNS    NONE                                                      *
 * EXTERNS    NONE                                                      *
 * NOTES      NONE                                                      *
 ************************************************************************
 */
static VOID iosl_delay(millisec)
IN long millisec;
  {
/*----------------------------- IRIS, LINUX ----------------------------*/
#if (OS_MARK == 'U'|| OS_MARK == 'L')

  struct timespec tm, oldtm;

  tm.tv_sec = millisec / 1000L;                /* seconds */
  tm.tv_nsec = (millisec % 1000L) * 1000000L;  /* and nanoseconds */
  nanosleep(&tm, &oldtm);

#endif
/*----------------------------- HP -------------------------------------*/
#if (OS_MARK == 'H')

  struct timeval delay;

  delay.tv_sec = millisec / 1000L;               /* seconds */
  delay.tv_usec = (millisec % 1000L) * 1000L;  /* microseconds */
  select(0, (int *)0, (int *)0, (int *)0, &delay);

#endif
/*--------------------------------- Win_NT/Win_95 ----------------------*/
#if (OS_MARK == 'I')

  /* Use WINDOWS SDK Sleep() */
  Sleep(millisec);

#endif
/*-------------------------------- SUN, TITAN, IBM ---------------------*/
#if (OS_MARK == 'N' || OS_MARK == 'R' || OS_MARK == 'T')

  /*  Usage of "high resolution sleep" is not yet implemented */
  long sec, msec;
  struct timeval start_time, end_time;

  /* Receive Current Time */
  gettimeofday(&start_time, &OS_tmp_timezone);
  /* Calculate Time in Seconds to Sleep */
  sec = millisec / 1000L;
  /* Go to Sleep if Necessary */
  if (sec > 0L)
    sleep(sec);
  /* Loop of Delay */
  do
    {
    gettimeofday(&end_time, &OS_tmp_timezone);
    msec = ((long)end_time.tv_sec - (long)start_time.tv_sec) * 1000L +
           ((long)end_time.tv_usec - (long)start_time.tv_usec) / 1000L;
    } while (msec < millisec);

#endif
/*----------------------------------- SCO UNIX -------------------------*/
#if (OS_MARK == 'O')

  /* Usage of "high resolution sleep" is not yet implemented */
  long msec;
  struct timeb start_time, end_time;

  /* Receive Current Time */
  ftime(&start_time);
    {
    long sec;
    /* Calculate Time in Seconds to Sleep */
    sec = millisec / 1000L;
    /* Go to Sleep if Necessary */
    if (sec > 0L)
      sleep(sec);
    }
  /* Loop of Delay */
  do
    {
    ftime(&end_time);
    msec = ((long)end_time.time - (long)start_time.time) * 1000L +
           ((long)end_time.millitm - (long)start_time.millitm);
    } while (msec < millisec);

#endif
  } /* End of iosl_delay() */
/* :_oh */


/*** End of  IOSL_EXT.C file ***/
