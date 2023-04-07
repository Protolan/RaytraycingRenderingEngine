/* FILE NAME    iosl_ex1.c
 * PURPOSE      OS-dependent functions for ENVI variables handling
 * 
 * SPEC         EZB                                  '94.11
 * DESIGN       EZB                                  '94.11
 * DES. VV
 * CODING       EZB, Erk                             '95.01
 * CODE VV.     Erk                                  '95.01
 * TEST         Erk                                  '95.01
 * UPDATED      Esa                                  '95.01.16
 *              (Added IOSL error processing)
 * LAST UPDATED Esa                                  '96.02.05
 *              (Corrected os_getpath())
 * 
 * SWITCHES     None.
 * NOTES        None.
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

static char name_buffer[MAX_PATHNAME_LENGTH]; /* path/name/ext buff */


/* ---------------------------------------------------------------------
 * NAME         os_getenv
 * PURPOSE      get environment variable value
 * ARGUMENTS    name - a name of the variable
 * RETURNS      A  value of this variable, 
 *              or NULL -  if  function cannot determine value of 
 *              the  variable, i.e. variable is not
 *              defined upon local environment.
 * EXTERNS      None
   -------------------------------------------------------------------*/
char *os_getenv(name)
IN char *name;
  {
  iosl_error = OS_OK;
  return getenv(name);
  }

/* ---------------------------------------------------------------------
 * NAME         os_putenv
 * PURPOSE      add variable to local environment
 * ARGUMENTS    variable -   a variable in format name=value;
 * RETURNS      SUCCESS - if passed, or FAILURE - otherwise
 * EXTERNS      None
 * NOTES
 *              Function  adds environment variable 'variable'  to 
 *              local environment string table. If there was already
 *              such a variable  defined in environment table -  
 *              its old value is rewritten with new one.
 *
 *              Notice, that variable string is not copied and
 *              it is application duty to provide valid storage
 *              for variable value.
 *
 *              There  are   some platforms on  which  adding  new 
 *              variables is not implemented at all. In that  case 
 *              function returns FAILURE.
 ---------------------------------------------------------------------*/

int os_putenv(variable)
IN char *variable;
  {
  int err;                     /* error code */

  err = putenv(variable);
  if (err != 0)
    {
    iosl_error = OS_ERR_OTHER;
    return FAILURE;
    }
  iosl_error = OS_OK;
  return SUCCESS;
  }


/* ---------------------------------------------------------------------
 * NAME         os_getpath
 * PURPOSE      extract path from full path
 * ARGUMENTS    full_path       a full path to a file
 * RETURNS      pathname or NULL if incorrect input
 * EXTERNS      name_buffer - buffer for resultant string (common for 
 *                            all name conversion functions).
 * NOTES
 *              On    platforms   where   logical    drives    are 
 *              distinguished (e.g. MS-DOS, DOS-EXTENDER, WINDOWS) 
 *              -  all characters describing logical drive  should 
 ---------------------------------------------------------------------*/
char *os_getpath(full_path)
IN char *full_path;
  {
  char *p;   /* ptr to end of path name */

  if (full_path == NULL)
    {
    iosl_error = OS_ERR_OTHER;
    return NULL;
    }

  strcpy(name_buffer, full_path);
  p = strrchr(name_buffer, PATH_SEPARATOR);
  if (p != NULL)
#if (OS_MARK == 'I')
    if (p - 1 >= name_buffer && *(p - 1) == ':')
      *(p + 1) = '\0';
    else
#endif
      if (p == name_buffer)
        *(p + 1) = '\0';
      else
        *p = '\0';
  else
    name_buffer[0] = '\0';

  iosl_error = OS_OK;
  return name_buffer;
  }


/* ---------------------------------------------------------------------
 * NAME         os_getname
 * PURPOSE      extract file name from full path
 * ARGUMENTS    full_path       a full path to a file
 * RETURNS      base name or NULL if incorrect input
 * EXTERNS      name_buffer - buffer for resultant string (common for 
 *                            all name conversion functions).
 ---------------------------------------------------------------------*/

char *os_getname(full_path)
const char *full_path;
  {
  char *name; /* ptr to base name */
  char *p;    /* ptr to end of base name */

  if (full_path == NULL)
    {
    iosl_error = OS_ERR_OTHER;
    return NULL;
    }

  strcpy(name_buffer, full_path);

  name = strrchr(name_buffer, PATH_SEPARATOR);
  if (name == NULL)
    {
    name = name_buffer;
    }
  else
    {
    name++; /* skip path separator character */
    }

  if (strlen(name) > 0)
    {
    /* cut the name extension off */
    p = strrchr(name, '.');
    if (p != NULL)
      {
      *p = '\0';
      }
    }
  
  /* return this part of name_buffer that contains base name */
  iosl_error = OS_OK;
  return name;
  }


/* ---------------------------------------------------------------------
 * NAME         os_getext
 * PURPOSE      extract file extension from full path
 * ARGUMENTS    full_path - a full path to a file
 * RETURNS      name extension or NULL if incorrect input
 * EXTERNS      name_buffer - buffer for resultant string (common for 
 *                            all name conversion functions).
 * NOTES
 *              As to file name extension on different OS, a following 
 *              convention is assumed:
 *
 *              FULL_NAME          BASE NAME      EXTENSION
 *
 *              "xxx.yyy.zzz" =>   "xxx.yyy"        ".zzz"
 *              "xxx..zzz"    =>   "xxx.",          ".zzz"
 *              "xxx."        =>   "xxx"            "."
 *              "xxx.."       =>   "xxx."           "."
 *              "xxx"         =>   "xxx"            ""
 *              "."           =>   ""               "."
 ---------------------------------------------------------------------*/

char *os_getext(full_path)
IN char *full_path;
  {
  char *ext;  /* ptr to file name extension */
  char *buff; /* temporay ptr inside name_buffer */

  if (full_path == NULL)
    {
    iosl_error = OS_ERR_OTHER;
    return NULL;
    }

  strcpy(name_buffer, full_path);
  buff = name_buffer;

  /* eliminate preceding path */
  ext = strrchr(buff, PATH_SEPARATOR);
  if (ext != NULL)
    buff = ext;

  /* find DOT - extension separator */
  ext = strrchr(buff, '.');
  if (ext == NULL)
    ext = &name_buffer[strlen(name_buffer)];

  iosl_error = OS_OK;
  return ext;
  }


/* ---------------------------------------------------------------------
 * NAME         os_path2name
 * PURPOSE      extract file name, extension and path from full path
 * ARGUMENTS    full_path - a full path to a file
 *              path      - a buffer for extracted path
 *              name      - a buffer for a single file name
 *              extension -  a buffer for extension
 * RETURNS      SUCCESS - if passed, or FAILURE - otherwise
 * EXTERNS      None
 * NOTES
 *              Function   retrieves   path,  filename   and   its 
 *              extension  (if  any) from full path  given  as  an 
 *              input.  If  any of 'path', 'name'  or  'extension' 
 *              parameters  is  NULL  - it is  not  evaluated  (is
 *               skipped).
 *
 *              It is up to the user that he should have sufficient
 *              enough  buffers  for  extracted  items.   It   is
 *              recommended     to    use     MAX_PATHNAME_LENGTH, 
 *              of these buffers.
 *
 *              As to file name extension on different OS, a following 
 *              convention is assumed:
 *
 *              FULL_NAME          BASE NAME      EXTENSION
 *
 *              "xxx.yyy.zzz" =>   "xxx.yyy"        ".zzz"
 *              "xxx..zzz"    =>   "xxx.",          ".zzz"
 *              "xxx."        =>   "xxx"            "."
 *              "xxx.."       =>   "xxx."           "."
 *              "xxx"         =>   "xxx"            ""
 *              "."           =>   ""               "."
 *
 *               On    platforms   where   logical    drives    are 
 *               distinguished (e.g. MS-DOS, DOS-EXTENDER, WINDOWS) 
 *               -  all characters describing logical drive  should 
 *               be placed at the beginning of 'path' buffer, also.
 ---------------------------------------------------------------------*/
int os_path2name(full_path, path, name, extension)
IN  char *full_path;
OUT char *path;
OUT char *name;
OUT char *extension;
  {
  char *p;    /* ptr to end of path or base name */
  char *buff; /* temporary pointer inside name_buffer */

  if (full_path == NULL)
    {
    iosl_error = OS_ERR_OTHER;
    return FAILURE;
    }

  strcpy(name_buffer, full_path);

  /* looking for components from end backward */

  /* find end cut off extension */
  buff = name_buffer;

  /* eliminate preceding path */
  p = strrchr(buff, PATH_SEPARATOR);
  if (p != NULL)
    buff = p;

  /* find DOT - extension separator */
  p = strrchr(buff, '.');
  if (p != NULL)
    {
    if (extension != NULL)
      strcpy(extension, p);
    *p = '\0';
    }
  else
    {
    if (extension != NULL)
      strcpy(extension, "");
    }

  /* path and base name */
  p = strrchr(name_buffer, PATH_SEPARATOR);
  if (p != NULL)
    {
    /* separate path and  base name */
    *p = '\0';

    if (path != NULL)
      strcpy(path, name_buffer);

    if (name != NULL)
      strcpy(name, p + 1);
    }
  else
    /* there is no path */
    {
    if (path != NULL)
      strcpy(path, "");

    if (name != NULL)
      strcpy(name, name_buffer);
    }

#ifdef DEBUG
  printf("os_path2name(%s, %s, %s, %s)\n", full_path, 
         (path == NULL ? "NULL" : path), 
         (name == NULL ? "NULL" : name), 
         (extension == NULL ? "NULL" : extension));
#endif

  iosl_error = OS_OK;

  return SUCCESS;
  }

/* ---------------------------------------------------------------------
 * NAME         os_cmppath
 * PURPOSE      Analog of strcmp - to compare 2 pathes (case sensitive or
 *              case insensitive depending on platform)
 * ARGUMENTS    2 pathes to be compared
 * RETURNS      result of stricmp() - for NT, or strcmp() - for UNIX
 ---------------------------------------------------------------------*/
int os_cmppath(IN char *path1, IN char *path2)
  {
#if (OS_MARK == 'I')
  return stricmp(path1, path2);
#else
  return strcmp(path1, path2);
#endif
  }

/* end of file 'iosl_ex1.c' */
