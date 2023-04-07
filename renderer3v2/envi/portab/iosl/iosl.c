/*
 * FILE NAME            iosl.c
 * PURPOSE              Unified module for IOSL
 *                      for all UNIX, DOS, DOSX, WINDOWS platforms
 *                      (IRIX, HP-UX, SCO, SUN, IBM, TITAN, MSC, WATCOM C)
 *
 * SPEC                 J. Kalinowski               1990.06.06
 *                      S. Andreev (IOSL_EXT)       1994.02.11
 * DESIGN               J. Skrzymowski              1990.06.06
 *                      S. Andreev (IOSL_EXT)       1994.02.28
 * DESIGN INSP.         Z. Blaszczyk (IOSL_EXT)     1994.03
 * CODING               J. Skrzymowski              1990.06.07
 *                      S. Andreev (IOSL_EXT)       1994.03
 * CODE INSP.           Z. Blaszczyk (IOSL_EXT)     1994.03
 * TEST                 J. Skrzymowski              1990.06.08
 *                      Z. Blaszczyk (IOSL_EXT)     1994.03
 *
 * UPDATED      RAW, CPR                            1990.07.31
 * UPDATED      S. Andreev (Esa)                    1993.10.13
 *              (corrected line:  O_BINARY added)
 * UPDATED      S. Andreev (Esa)                    1994.01.24
 *              (added error codes, IN/OUT/INOUT macros)
 * UPDATED      Esa                                 1994.03.01
 *              Corrected os_touch, added iosl_error processing
 * UPDATED      S. Andreev (Esa)                    1994.07.07
 *              United sources for IRIX, HP-UX, SCO into single file
 *              Corrected os_fstat() - processing special files
 * UPDATED      Esa                                 1994.07.11
 *              United sources for all UNIX platform
 * UPDATED      S. Andreev (Esa)                    1995.04.14
 *              (added WINDOWS NT support)
 * UPDATED      S. Andreev (Esa)                    1995.07.05
 *              (corrected os_opendir() for NT)
 *              os_dirattrib() for NT/Windows'95
 *              corrected
 * UPDATED      Esa                                 1995.11.20
 *              Corrected os_time() for R8000 (64-bit long)
 * UPDATED      Esa                                 1996.02.02
 *              Corrected os_time() due to problems under IRIX 4.xx
 * UPDATED      V. Rochtchine (Evr)                 1996.07.22
 *              (File system accessing functions corrected
 *              to use iosl_convert_separator())
 * UPDATED      E. Denisov (Eed)                    1996.08.14
 *              (Correct processing of 'a:\'
 *              in case of empty diskette)
 * UPDATED      S.Grimaylo (Esg)                    1997.11.15
 *              (Corrected os_touch() for Windows NT)
 *
 * UPDATED      Eed                                 2001.02.01
 *              Merge of iosl.dos and iosl.unx files
 *
 */

#ifdef TIPE
#undef TIPE
#endif

#if (OS_MARK == 'I')
#include <windows.h>
#endif

#include "integra.h"
#include "iosl.h"
#include "iosl_own.h"

static char *sccsid = "IOSL 2.0";

/*
 * NAME         iosl_version
 * PURPOSE      Get version data
 * ARGUMENTS    None.
 * RETURNS      Pointer to version data
 * EXTERNS      None.
 * NOTES        It is forbidden to use this function in applications
 */

char *iosl_version()
  {
  iosl_error = OS_OK;
  return sccsid;
  } /* end of 'iosl_version' function */

#if (OS_MARK == 'I')
/* Win32 */

/* CONSTANTS */
#define MATCH_ANY_FILE  "*.*"
#define FIRST_ENTRY     0
#define FIND_T          struct find_t

#ifndef S_ISCHR
#define S_ISCHR(m)    (((m) & S_IFMT) == S_IFCHR)
#endif

#ifndef S_ISDIR
#define S_ISDIR(m)    (((m) & S_IFMT) == S_IFDIR)
#endif

#ifndef S_ISREG
#define S_ISREG(m)    (((m) & S_IFMT) == S_IFREG)
#endif

/*************************** CODE ******************************/

/* GLOBAL VARIABLES */
extern char iosl_work_buffer[];
extern VOID  iosl_convert_separator PROT((OUT char *, IN char *));
static void MakeSearchName PROT((char *, char *));

/*
 * NAME         os_fopen
 * PURPOSE      Open a stream
 * ARGUMENTS    filename    - path name of the file to open
 *              mode        - required access mode
 * RETURNS      Pointer to new FILE structure on success
 *              NULL on failure.
 * EXTERNS      None
 * NOTES        ANSI fopen() compatible
 */

FILE *os_fopen(filename, mode)
IN char *filename;
IN char *mode;
  {
  FILE *os_stream;
  char open_mode[4];
  int i;

  /* check if mode is valid */
  if (mode[0] != 'r' && mode[0] != 'w' && mode[0] != 'a')
    {
    iosl_error = OS_ERR_OTHER;
    return NULL;
    }

  i = 1;
  if (mode[1] == '+')
    i = 2;

  if (mode[i] == '\0')
    {
    /* this is in case "r", "w", "a", "r+", "w+", "a+" */
    strcpy(open_mode, mode);
    open_mode[i++] = 't';
    open_mode[i]   = '\0';
    }
  else
    {
    /* check if letter b follows - if no, error */
    if (mode[i] != 'b')
      {
      iosl_error = OS_ERR_OTHER;
      return NULL;
      }
    i++;
    /* enable both "x+b" and "xb+" */
    if ((mode[i] != '\0') && (mode[i] != '+' || mode[i+1] != '\0'))
      {
      iosl_error = OS_ERR_OTHER;
      return NULL;
      }

    strcpy(open_mode, mode);
    }

  /* Convert path separators */
  iosl_convert_separator(iosl_work_buffer, filename);

  os_stream = fopen(iosl_work_buffer, open_mode);
  iosl_error = iosl_error_code();
  return os_stream;
  } /* end of 'os_fopen' function */


/*
 * NAME         os_fclose
 * PURPOSE      Close a stream
 * ARGUMENTS    fp  - pointer to a file
 * RETURNS      0 if file successfully closed
 *              EOF otherwise
 * EXTERNS      None.
 * NOTES        ANSI fclose() compatible
 */

int os_fclose(fp)
IN FILE *fp;
  {
  int result;

  result = fclose(fp);
  iosl_error = iosl_error_code();
  return result;
  } /* end of 'os_fclose' function */


/*
 * NAME         os_remove
 * PURPOSE      Delete a file
 * ARGUMENTS    filename    - path name of the file
 * RETURNS      0 if file is deleted
 *              -1 otherwise
 * EXTERNS      None
 * NOTES        ANSI unlink() compatible
 */

int os_remove(filename)
IN char *filename;
  {
  int result;

  /* Convert path separators */
  iosl_convert_separator(iosl_work_buffer, filename);

  result = unlink(iosl_work_buffer);
  iosl_error = iosl_error_code();
  return result;
  } /* end of 'os_remove' function */


/*
 * NAME         os_rename
 * PURPOSE      Change name of a file or directory
 * ARGUMENTS    oldname     - old name of a file or directory
 *              newname     - new name
 * RETURNS      0 if file is successfully renamed
 *              -1 otherwise
 * EXTERNS      None.
 * NOTES        This function can be used to move files between
 *              directories (but not between drives)
 *              Directories cannot be moved.
 *              ANSI rename() compatible
 */

int os_rename(oldname, newname)
IN char *oldname;
IN char *newname;
  {
  int result;

  /* Convert path separators */
  iosl_convert_separator(iosl_work_buffer, oldname);

  result = rename(iosl_work_buffer, newname);
  iosl_error = iosl_error_code();
  return result;
  } /* end of 'os_rename' function */


extern void os_own_seterrormode(void);
extern int os_own_getlasterror(void);

/*
 * NAME         os_fstat
 * PURPOSE      Get and store file information
 * ARGUMENTS    filename    - path name of a file
 *              info        - pointer to location receiving results
 * RETURNS      None.
 * EXTERNS      None.
 * NOTES        None.
 */

VOID os_fstat(filename, info)
IN    char *filename;
INOUT OS_FINFO *info;
  {
  struct stat f_info;
  struct tm *atime;

  /* Convert path separators */
  iosl_convert_separator(iosl_work_buffer, filename);

  f_info.st_size = 0L;

  os_own_seterrormode();

  if (stat(iosl_work_buffer, &f_info) == -1)
    {
    /* Process floppy disk absence */
    if (os_own_getlasterror())
      {
      info->access = 0;
      errno = ENOENT;
      iosl_error = iosl_error_code();
      return;
      }

    info->access = 0;
    iosl_error = iosl_error_code();
    return;
    }
  info->access = 1;

  if (S_ISREG(f_info.st_mode))
    {
    info->attrib = OS_DATTR_FILE;
    info->size   = (long)f_info.st_size;
    }
  else
    if (S_ISDIR(f_info.st_mode))
      {
      info->attrib = OS_DATTR_SUBDIR;
      info->size   = 0L;
      }
    else
      {
      info->attrib = OS_DATTR_FILE;
      info->size   = 0L;
      }

  info->atime = f_info.st_mtime;
  atime = localtime(&f_info.st_mtime);
  info->time.hour   = atime->tm_hour;
  info->time.minute = atime->tm_min;
  info->time.second = atime->tm_sec;
  info->time.year   = atime->tm_year;
  info->time.month  = atime->tm_mon;
  info->time.day    = atime->tm_mday;
  iosl_error = OS_OK;
  } /* end of 'os_fstat' function */

/*
 * NAME         os_touch
 * PURPOSE      Set file modification time
 * ARGUMENTS    filename    - path name of the file
 *              ostime      - pointer to structure with time values
 * RETURNS      0 if modification time set
 *              -1 otherwise
 * EXTERNS      None.
 * NOTES        If second argument is NULL, modification time is current
 *              system time.
 *              Invalid time is rejected.
 *              No touch possible with date before Jan 1, 1980.
 *              Due to implementation of IRIX 'touch' command, files
 *              touched are displayed by 'ls -l' in format different 
 *              than usual, i.e. year is given instead of time 
 */

static int month_days[] = {31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

int os_touch(filename, ostime)
char *filename;
OS_TIME *ostime;
  {
  struct utimbuf settime;
  struct tm file_time;
  time_t ftime;

  /* check filename for NULL pointer */
  if (filename == NULL)
    {
    iosl_error = OS_ERR_NO_SUCH_FILE_OR_DIR;
    return FAILURE;
    }

  if (ostime != NULL)
    {
    /* check if time valid */
    if (ostime->year   < 80 || ostime->year   > 199 ||  /* 99->199  :Esg */
        ostime->month  <  0 || ostime->month  > 11  ||
        ostime->day    <  1 || ostime->day    > month_days[ostime->month] ||
        ostime->hour   <  0 || ostime->hour   > 23  ||
        ostime->minute <  0 || ostime->minute > 59  ||
        ostime->second <  0 || ostime->second > 59)
      {
      iosl_error = OS_ERR_TIME_NOT_VALID;
      return -1;
      }

    if (ostime->month == 1 && ostime->day == 29 && ostime->year % 4 != 0)
      {
      iosl_error = OS_ERR_TIME_NOT_VALID;
      return -1;
      }

    file_time.tm_sec = ostime->second;
    file_time.tm_min = ostime->minute;
    file_time.tm_hour = ostime->hour;
    file_time.tm_mday = ostime->day;
    file_time.tm_mon = ostime->month;
    file_time.tm_year = ostime->year;
    file_time.tm_isdst = -1; /* Standart or DAYLIGHT time   '97.11.15 :Esg */

    ftime = mktime(&file_time);
    settime.actime = ftime;
    settime.modtime = ftime;

    utime(filename, &settime);
    }
  else
    {
    utime(filename, NULL);
    }

  errno = 0;
  iosl_error = iosl_error_code();
  if (iosl_error == OS_OK)
    return SUCCESS;
  else
    return FAILURE;
  } /* end of 'os_touch' function */

/*
 * NAME         os_opendir
 * PURPOSE      Enable access to directory entries.
 * ARGUMENTS    path_name   - path to directory
 * RETURNS      Pointer to directory data structure if successful
 *              NULL on error
 * EXTERNS      None.
 * NOTES        None.
 */

OS_DIR *os_opendir(path_name)
IN char *path_name;
  {
  static char search_name[MAX_PATHNAME_LENGTH];
  OS_DIR *dir;
  HANDLE tmp;

  if ((path_name == NULL) || (path_name[0] == '\0'))
    {
    iosl_error = OS_ERR_NO_SUCH_FILE_OR_DIR;
    return NULL;
    }

  if ((dir = (OS_DIR *)malloc(sizeof(OS_DIR))) == NULL)
    {
    iosl_error = OS_ERR_NOT_ENOUGH_MEMORY;
    return (NULL);
    }
  if ((dir->fdata = (void *)malloc(sizeof(NT_FIND_DATA))) == NULL)
    {
    free((void *)dir);
    iosl_error = OS_ERR_NOT_ENOUGH_MEMORY;
    return (NULL);
    }
  MakeSearchName(path_name, search_name);
  if ((tmp = FindFirstFile(search_name, (WIN32_FIND_DATA *)(dir->fdata))) == (HANDLE)-1)
    {
    struct stat dummy;
    free((void *)dir->fdata);
    if (stat(path_name, &dummy) == -1)
      {
      iosl_error = iosl_error_code();
      free((void *)dir);
      return (NULL);
      }
    dir->fdata     = NULL;
    dir->entries = FIRST_ENTRY;
    }
  else
    {
    ((NT_FIND_DATA *)(dir->fdata))->hdir = tmp;
    dir->entries = FIRST_ENTRY;
    /* skip . entry; .. will be skipped on first call to os_readdir */
    if (*os_dirfilename(dir) == '.')
      {
      FindNextFile(((NT_FIND_DATA *)(dir->fdata))->hdir, (WIN32_FIND_DATA *)(dir->fdata));
      dir->entries++;
      }
    }
    
  iosl_error = OS_OK;
  return dir;
  } /* end of 'os_opendir' function */


/*
 * NAME         MakeSearchName
 * PURPOSE      Prepare search name for subsequent _dos_findfirst call
 * ARGUMENTS    name        - path to directory
 *              search_name -
 * RETURNS      None.
 * EXTERNS      None.
 * NOTES        None.
 */

static void MakeSearchName(name, search_name)
IN  char *name;
OUT char *search_name;
  {
  iosl_convert_separator(search_name, name);
  if ((name[1] == '\0' && name[0] == '\\') ||
      (name[1] == ':' && name[2] == '\\' && name[3] == '\0') ||
      (name[0] == '.' && name[1] == '\\' && name[2] == '\0') ||
      (name[1] == ':' && name[2] == '\0'))
    {
    strcat(search_name, MATCH_ANY_FILE);
    }
  else
    {
    strcat(search_name, "\\");
    strcat(search_name, MATCH_ANY_FILE);
    }
  } /* end of 'MakeSearchName' function */


/*
 * NAME         os_readdir
 * PURPOSE      Enable access to subsequent directory entry
 * ARGUMENTS    dir   - pointer to directory data structure
 * RETURNS      Pointer to directory data if next entry found
 *              NULL otherwise
 * EXTERNS      None.
 * NOTES        None.
 */

OS_DIR *os_readdir(dir)
INOUT OS_DIR *dir;
  {
  if (dir == NULL)
    {
    iosl_error = OS_ERR_NO_SUCH_FILE_OR_DIR;
    return NULL;
    }
  if (dir->fdata == NULL)
    {
    iosl_error = OS_ERR_NO_MORE_ENTRIES;
    return NULL;
    }

  if (dir->entries > FIRST_ENTRY)
    {
    if (FindNextFile(((NT_FIND_DATA *)(dir->fdata))->hdir,
                     (WIN32_FIND_DATA *)(dir->fdata)) != TRUE)
      {
      iosl_error = OS_ERR_NO_MORE_ENTRIES;
      return (NULL);
      }
    }
  dir->entries++;
  return dir;
  } /* end of 'os_readdir' function */


/*
 * NAME         os_closedir
 * PURPOSE      Finish access to directory
 * ARGUMENTS    dir   - pointer to directory data
 * RETURNS      None.
 * EXTERNS      None.
 * NOTES        None.
 */

VOID os_closedir(dir)
IN OS_DIR *dir;
  {
  if (dir != NULL)
    {
    if (dir->fdata != NULL)
      {
      FindClose(((NT_FIND_DATA *)(dir->fdata))->hdir);
      free((void *)dir->fdata);
      }
    dir->fdata = NULL;
    free((void *)dir);
    }
  } /* end of 'os_closedir' function */


/*
 * NAME         os_dirfilename
 * PURPOSE      Get file name of directory entry
 * ARGUMENTS    dir   - pointer to directory data
 * RETURNS      Pointer to file name
 *              NULL if dir is NULL
 * EXTERNS      None.
 * NOTES        None.
 */

char *os_dirfilename(dir)
IN OS_DIR *dir;
  {
  char *p, *q;

  if (dir == NULL || dir->fdata == NULL)
    {
    iosl_error = OS_ERR_NO_MORE_ENTRIES;
    return NULL;
    }
  iosl_error = OS_OK;
  p = (char *)(((WIN32_FIND_DATA *)dir->fdata)->cFileName);
  q = p;
  while (*q != '\0')
    {
    *q = (char)tolower((int)(*q));
    q++;
    }

  return p;
  } /* end of 'os_dirfilename' function */


/*
 * NAME         os_dirattrib
 * PURPOSE      Get attributes of directory entry
 * ARGUMENTS    dir   - pointer to directory data
 * RETURNS      Attributes mask
 *              -1 if dir is NULL
 * EXTERNS      None.
 * NOTES        None.
 */

int os_dirattrib(dir)
IN OS_DIR *dir;
  {
  if (dir == NULL || dir->fdata == NULL)
    {
    iosl_error = OS_ERR_NO_MORE_ENTRIES;
    return -1;
    }
  iosl_error = OS_OK;
  if (((WIN32_FIND_DATA *)dir->fdata)->dwFileAttributes & 0x010)
    return OS_DATTR_SUBDIR;
  else
    return OS_DATTR_FILE;
  } /* end of 'os_dirattrib' function */

/*
 * NAME         os_time
 * PURPOSE      Get current system time
 * ARGUMENTS    ostime  - pointer to storage for time
 * RETURNS      Current system time
 * EXTERNS      None.
 * NOTES        If ostime is NULL, time will not be stored
 */
OS_TIME_T os_time(ostime)
IN OS_TIME_T *ostime;
  {
  OS_TIME_T seconds;
  long mseconds;

  iosl_gettime((long *)&seconds, &mseconds);

  if (ostime != NULL)
    *ostime = seconds;
  return seconds;
  } /* end of 'os_time()' function */

/*
 * NAME         os_difftime
 * PURPOSE      Compute difference between two times
 * ARGUMENTS    time1
 *              time2
 * RETURNS      Difference time1 - time2 as double.
 * EXTERNS      None.
 * NOTES        None.
 */

double os_difftime(time1, time2)
IN OS_TIME_T time1;
IN OS_TIME_T time2;
  {
  iosl_error = OS_OK;
  return (double)(time1 - time2);
  } /* end of 'os_difftime' function */

/*
 * NAME         iosl_gettime()
 * PURPOSE      Return time in sec. and msec.
 * ARGUMENTS    seconds
 *              mseconds
 * RETURNS      NONE
 * EXTERNS      NONE
 * NOTES        NONE
 */

VOID iosl_gettime(seconds, mseconds)
OUT long *seconds;
OUT long *mseconds;
  {
  static long last_time = -1L;
  static WORD wDay = -1;
  time_t curtime;
  SYSTEMTIME systime;

  GetLocalTime(&systime);
  if ((last_time == -1L) || (wDay != systime.wDay))
    {
    time(&curtime);
    last_time = (long)(curtime - systime.wHour * 3600 -
                                 systime.wMinute * 60 -
                                 systime.wSecond);
    wDay = systime.wDay;
    }
  else
    {
    curtime = last_time + systime.wHour * 3600 +
                          systime.wMinute * 60 +
                          systime.wSecond;
    }

  *seconds = (long)curtime;
  *mseconds = systime.wMilliseconds;
  } /* end of os_time() function */

/* End of Win32 part */
#else
/* Otherwise UNIX */

/* TYPES */

#define	DirFileName(entry)	(entry->d_name)

static char fname[MAXNAMLEN];

typedef struct OS_DIRENT
  {
  DIR *dir;
  char dirname[MAXNAMLEN];
  struct dirent	*entry;
  } OS_DIRENT;


/*************************** CODE ******************************/

/*
 * NAME         os_fopen
 * PURPOSE      Open a stream
 * ARGUMENTS    filename    - path name of the file to open
 *              mode        - required access mode
 * RETURNS      Pointer to new FILE structure on success
 *              NULL on failure.
 * EXTERNS      None
 * NOTES        ANSI fopen() compatible
 */

FILE *os_fopen(filename, mode)
char *filename;
char *mode;
  {
  char open_mode[4];
  int i;
  FILE *stream;

  /* check filename for NULL pointer */
  if (filename == NULL)
    {
    iosl_error = OS_ERR_NO_SUCH_FILE_OR_DIR;
    return NULL;
    }

  /* check if mode is valid */
  if (mode[0] != 'r' && mode[0] != 'w' && mode[0] != 'a')
    {
    iosl_error = OS_ERR_OTHER;
    return NULL;
    }

  i = 1;
  if (mode[1] == '+')
    i = 2;

  if (mode[i] == '\0')
    {
    /* this is in case "r", "w", "a", "r+", "w+", "a+" */
    strcpy(open_mode, mode);
    }
  else
    {
    /* check if letter b follows - if no, error */
    if (mode[i] != 'b' || mode[i+1] != '\0')
      {
      iosl_error = OS_ERR_OTHER;
      return NULL;
      }

    strcpy(open_mode, mode);
    /* remove 'b' character - on UNIX is not needed */
    open_mode[i] = '\0';
    }

  stream = fopen(filename, open_mode);
  iosl_error = iosl_error_code();
  return stream;
  } /* end of 'os_fopen' function */


/*
 * NAME         os_fclose
 * PURPOSE      Close a stream
 * ARGUMENTS    fp  - pointer to a file
 * RETURNS      0 if file successfully closed
 *              EOF otherwise
 * EXTERNS      None.
 * NOTES        ANSI fclose() compatible
 */

int os_fclose(fp)
FILE *fp;
  {
  /* check fp for NULL pointer */
  if (fp == NULL)
    {
    iosl_error = OS_ERR_NO_SUCH_FILE_OR_DIR;
    return FAILURE;
    }

  iosl_error = OS_OK;
  return fclose(fp);
  } /* end of 'os_fclose' function */


/*
 * NAME         os_remove
 * PURPOSE      Delete a file
 * ARGUMENTS    filename    - path name of the file
 * RETURNS      0 if file is deleted
 *              -1 otherwise
 * EXTERNS      None
 * NOTES        ANSI unlink() compatible
 */

int os_remove(filename)
char *filename;
  {
  int result;

  /* check filename for NULL pointer */
  if (filename == NULL)
    {
    iosl_error = OS_ERR_NO_SUCH_FILE_OR_DIR;
    return FAILURE;
    }

  result = unlink(filename);
  iosl_error = iosl_error_code();
  return result;
  } /* end of 'os_remove' function */


/*
 * NAME         os_rename
 * PURPOSE      Change name of a file or directory
 * ARGUMENTS    oldname     - old name of a file or directory
 *              newname     - new name
 * RETURNS      0 if file is successfully renamed
 *              -1 otherwise
 * EXTERNS      None.
 * NOTES        This function can be used to move files between
 *              directories (but not between drives)
 *              Directories cannot be moved.
 *              ANSI rename() compatible
 */

int os_rename(oldname, newname)
char *oldname;
char *newname;
  {
  int result;

  /* check filenames for NULL pointer */
  if ((oldname == NULL) || (newname == NULL))
    {
    iosl_error = OS_ERR_NO_SUCH_FILE_OR_DIR;
    return FAILURE;
    }

  result = rename(oldname, newname);
  iosl_error = iosl_error_code();
  return result;
  } /* end of 'os_rename' function */


/*
 * NAME         os_fstat
 * PURPOSE      Get and store file information
 * ARGUMENTS    filename    - path name of a file
 *              info        - pointer to location receiving results
 * RETURNS      None.
 * EXTERNS      None.
 * NOTES        Added by Esa processing special files
 */

VOID os_fstat(filename, info)
char *filename;
OS_FINFO *info;
  {
  struct stat f_info;
  struct tm *atime;

  /* check filename for NULL pointer */
  if (filename == NULL)
    {
    iosl_error = OS_ERR_NO_SUCH_FILE_OR_DIR;
    return;
    }

  iosl_error = OS_OK;

  if (stat(filename, &f_info) == -1)
    {
    info->access = 0;
    iosl_error = iosl_error_code();
    return;
    }
  info->access = 1;

  if (S_ISREG(f_info.st_mode))
    {
    info->attrib = OS_DATTR_FILE;       /* Regular file */
    info->size   = (long)f_info.st_size;
    }
  else
    if (S_ISDIR(f_info.st_mode))        /* Directory */
      {
      info->attrib = OS_DATTR_SUBDIR;
      info->size   = 0L;
      }
    else                                /* Special files */
      if ((S_ISBLK(f_info.st_mode)) ||
          (S_ISCHR(f_info.st_mode)) ||
/*------------------------------ SCO UNIX ------------------------------*/
#if (OS_MARK == 'O')                            /* For SCO UNIX only    */
          (S_ISNAM(f_info.st_mode)) ||
#endif                                          /* End of SCO switch    */
/*------------------------ IRIX (SUN, IBM, TITAN) ----------------------*/
#if (OS_MARK == 'U')                            /* For IRIX/SUN/IBM/TIT */
          (S_ISLNK(f_info.st_mode)) ||
          (S_ISSOCK(f_info.st_mode)) ||
#endif                                          /* End of IRIX switch   */
          (S_ISFIFO(f_info.st_mode)))
        {
        info->attrib = OS_DATTR_FILE;
        info->size   = 0L;
        }

  info->atime = f_info.st_mtime;
  atime = localtime(&f_info.st_mtime);
  info->time.hour   = atime->tm_hour;
  info->time.minute = atime->tm_min;
  info->time.second = atime->tm_sec;
  info->time.year   = atime->tm_year;
  info->time.month  = atime->tm_mon;
  info->time.day    = atime->tm_mday;
  } /* end of 'os_fstat' function */


/*
 * NAME         os_time
 * PURPOSE      Get current system time
 * ARGUMENTS    ostime  - pointer to storage for time
 * RETURNS      Current system time
 * EXTERNS      None.
 * NOTES        If ostime is NULL, time will not be stored
 */

OS_TIME_T os_time(ostime)
OS_TIME_T *ostime;
  {
  static time_t tmp_time;

  iosl_error = OS_OK;

  if (ostime == NULL)
    return (OS_TIME_T)time(ostime);
  else
    {
    time(&tmp_time);
    *ostime = (OS_TIME_T)tmp_time;
    return (OS_TIME_T)tmp_time;
    }
  } /* end of 'os_time' function */


/*
 * NAME         os_difftime
 * PURPOSE      Compute difference between two times
 * ARGUMENTS    time1
 *              time2
 * RETURNS      Difference time1 - time2 as double.
 * EXTERNS      None.
 * NOTES        None.
 */

double os_difftime(time1, time2)
OS_TIME_T time1;
OS_TIME_T time2;
  {
  iosl_error = OS_OK;
  return (double)(time1 - time2);
  } /* end of 'os_difftime' function */


/*
 * NAME         os_touch
 * PURPOSE      Set file modification time
 * ARGUMENTS    filename    - path name of the file
 *              ostime      - pointer to structure with time values
 * RETURNS      0 if modification time set
 *              -1 otherwise
 * EXTERNS      None.
 * NOTES        If second argument is NULL, modification time is current
 *              system time.
 *              Invalid time is rejected.
 *              No touch possible with date before Jan 1, 1980.
 *              Due to implementation of IRIX 'touch' command, files
 *              touched are displayed by 'ls -l' in format different 
 *              than usual, i.e. year is given instead of time 
 */

static int month_days[] = {31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

int os_touch(filename, ostime)
char *filename;
OS_TIME *ostime;
  {
  struct utimbuf settime;
  struct tm file_time;
  time_t ftime;

  /* check filename for NULL pointer */
  if (filename == NULL)
    {
    iosl_error = OS_ERR_NO_SUCH_FILE_OR_DIR;
    return FAILURE;
    }

  if (ostime != NULL)
    {
    /* check if time valid */
    if (ostime->year   < 80 || ostime->year   > 99 ||
        ostime->month  <  0 || ostime->month  > 11  ||
        ostime->day    <  1 || ostime->day    > month_days[ostime->month] ||
        ostime->hour   <  0 || ostime->hour   > 23  ||
        ostime->minute <  0 || ostime->minute > 59  ||
        ostime->second <  0 || ostime->second > 59)
      {
      iosl_error = OS_ERR_TIME_NOT_VALID;
      return -1;
      }

    if (ostime->month == 1 && ostime->day == 29 && ostime->year % 4 != 0)
      {
      iosl_error = OS_ERR_TIME_NOT_VALID;
      return -1;
      }

    file_time.tm_sec = ostime->second;
    file_time.tm_min = ostime->minute;
    file_time.tm_hour = ostime->hour;
    file_time.tm_mday = ostime->day;
    file_time.tm_mon = ostime->month;
    file_time.tm_year = ostime->year;

    ftime = mktime(&file_time);
    settime.actime = ftime;
    settime.modtime = ftime;

    utime(filename, &settime);
    }
  else
    {
    utime(filename, NULL);
    }

  errno = 0;
  iosl_error = iosl_error_code();
  if (iosl_error == OS_OK)
    return SUCCESS;
  else
    return FAILURE;
  } /* end of 'os_touch' function */


/*
 * NAME         os_opendir
 * PURPOSE      Enable access to directory entries.
 * ARGUMENTS    path_name   - path to directory
 * RETURNS      Pointer to directory data structure if successful
 *              NULL on error
 * EXTERNS      None.
 * NOTES        None.
 */

OS_DIR *os_opendir(path_name)
char *path_name;
  {
  OS_DIR *dir;
  OS_DIRENT *os_dirent;

  /* check path_name for NULL pointer */
  if (path_name == NULL)
    {
    iosl_error = OS_ERR_NO_SUCH_FILE_OR_DIR;
    return NULL;
    }

  if ((dir = (OS_DIR *)malloc(sizeof(OS_DIR))) == NULL)
    {
    iosl_error = OS_ERR_NOT_ENOUGH_MEMORY;
    return (NULL);
    }

  if ((os_dirent = (OS_DIRENT *)malloc(sizeof(OS_DIRENT))) == NULL)
    {
    free((VPTR)dir);
    iosl_error = OS_ERR_NOT_ENOUGH_MEMORY;
    return (NULL);
    }

  if ((os_dirent->dir = opendir(path_name)) == NULL)
    {
    iosl_error = iosl_error_code();
    free((VPTR)os_dirent);
    free((VPTR)dir);
    return (NULL);
    }

  os_dirent->entry = NULL;
  strcpy(os_dirent->dirname, path_name);

  dir->fdata     = (VPTR)os_dirent;
  dir->entries   = 0;

  iosl_error = OS_OK;
  return dir;
  } /* end of 'os_opendir' function */


/*
 * NAME         os_readdir
 * PURPOSE      Enable access to subsequent directory entry
 * ARGUMENTS    dir   - pointer to directory data structure
 * RETURNS      Pointer to directory data if next entry found
 *              NULL otherwise
 * EXTERNS      None.
 * NOTES        None.
 */

OS_DIR *os_readdir(dir)
OS_DIR *dir;
  {
  OS_DIRENT *os_dirent;
  char *name;

  os_dirent = (OS_DIRENT *)dir->fdata;
  do
    {
    os_dirent->entry = readdir(os_dirent->dir);
    if (os_dirent->entry == NULL)
      {
      iosl_error = OS_ERR_NO_MORE_ENTRIES;
      return NULL;
      }

    name  = DirFileName(os_dirent->entry);
    }
  while ((name[0] == '.' && name[1] == '\0') ||
         (name[0] == '.' && name[1] == '.' && name[2] == '\0'));

  iosl_error = OS_OK;
  return dir;
  } /* end of 'os_readdir' function */


/*
 * NAME         os_closedir
 * PURPOSE      Finish access to directory
 * ARGUMENTS    dir   - pointer to directory data
 * RETURNS      None.
 * EXTERNS      None.
 * NOTES        None.
 */

VOID os_closedir(dir)
OS_DIR *dir;
  {
  if (dir != NULL)
    {
    closedir(((OS_DIRENT *)(dir->fdata))->dir);
    free((VPTR)dir->fdata);
    free((VPTR)dir);
    iosl_error = OS_OK;
    }
  else
    iosl_error = OS_ERR_NO_SUCH_FILE_OR_DIR;
  } /* end of 'os_closedir' function */


/*
 * NAME         os_dirfilename
 * PURPOSE      Get file name of directory entry
 * ARGUMENTS    dir   - pointer to directory data
 * RETURNS      Pointer to file name
 *              NULL if dir is NULL
 * EXTERNS      None.
 * NOTES        None.
 */

char *os_dirfilename(dir)
OS_DIR *dir;
  {
  if (dir == NULL)
    {
    iosl_error = OS_ERR_NO_SUCH_FILE_OR_DIR;
    return NULL;
    }

  if (((OS_DIRENT *)dir->fdata)->entry == NULL)
    {
    iosl_error = OS_ERR_NO_MORE_ENTRIES;
    return NULL;
    }

  iosl_error = OS_OK;
  return DirFileName(((OS_DIRENT *)dir->fdata)->entry);
  } /* end of 'os_dirfilename' function */


/*
 * NAME         os_dirattrib
 * PURPOSE      Get attributes of directory entry
 * ARGUMENTS    dir   - pointer to directory data
 * RETURNS      Attributes mask
 *              -1 if dir is NULL
 * EXTERNS      None.
 * NOTES        None.
 */

int os_dirattrib(dir)
OS_DIR *dir;
  {
  OS_FINFO info;
  char     *filename;

  if (dir == NULL)
    {
    iosl_error = OS_ERR_NO_SUCH_FILE_OR_DIR;
    return -1;
    }
  if (((OS_DIRENT *)dir->fdata)->entry == NULL)
    {
    iosl_error = OS_ERR_NO_MORE_ENTRIES;
    return -1;
    }

  filename = DirFileName(((OS_DIRENT *)dir->fdata)->entry);
  strcpy(fname, ((OS_DIRENT *)dir->fdata)->dirname);
  strcat(fname, "/");
  strcat(fname, filename);
  os_fstat(fname, &info);
  if (info.access == 0)
    {
    iosl_error = OS_ERR_OTHER;
    return -1;
    }

  iosl_error = OS_OK;
  return info.attrib;
  } /* end of 'os_dirattrib' function */

/* End of UNIX part */
#endif

/* End of IOSL.C file */
