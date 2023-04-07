/*
 * MODULE NAME  MODE.C
 * PURPOSE      Functions which  are  used  by other IMAL modules
 *              such  as  error  processing, general checking  in
 *              debug  mode.  Module also  contains  some  user's
 *              functions:  IMAL mode  functions,  inquire  about
 *              last IMAL call result.
 *
 * SPEC         V. L. Volevich                        1993.01.12
 * DESIGN       V. L. Volevich                        1993.06.03
 * DES. INSP.   Z. Blaszczyk                          1993.06.28
 * CODING       V. L. Volevich                        1993.07.25
 * CODE INSP.   Z. Blaszczyk                          1993.07.??
 * TEST         V. L. Volevich                        1993.08.10
 *
 * LAST UPDATED V. L. Volevich                        1993.08.11
 *
 */

#define  DEFINE_GLOBALS   /* imal global variables are defined here */

/* COMPANY INCLUDES */

#define IMAL_NO_C_RTL_MACRO  /* Turn off macros for C RTL functions */

#include "integra.h"
#include "imal.h"


/* PROJECT INCLUDE: */

#include "imal_loc.h"


/* TYPEDEFS */

typedef struct
  {
  int code;
  char *msg;
  } ERROR_NAME;

typedef struct
  {
  char *name;
  BOOL *flag;
  } MODE_FLAG_REF;

/* CONSTANTS */

#define MODE_STACK_SIZE    1000         /* Size of the mode stack     */
#define IMAL_MESSAGE_SIGN  "*** IMAL: " /* Begin of the error message */

#define N_HEX    20   /* Number of bad block symbols which */
                      /* will be printed in heximal form   */
#define N_ASCII  20   /* Number of bad block symbols which */
                      /* will be printed in ascii form     */
#define N_NAME   20   /* Number of symbols in block/class  */
                      /* name which will be printed        */


/****************************** DATA **********************************/

/* Error message table */
static ERROR_NAME err_tab[] =
  {
  { MEM_OK,
    "O.K." },
  { MEM_ERR_NOT_INIT,
    "IMAL is not initialized yet" },
  { MEM_ERR_INIT,
    "mem_init() called when IMAL is initialized" },
  { MEM_ERR_NOT_EMPTY,
    "There are not released blocks" },
  { MEM_ERR_BLOCK_DAMAGE,
    "Allocated block damage" },
  { MEM_ERR_BUFFER_DAMAGE,
    "Locked buffer violation" },
  { MEM_ERR_FATAL,
    "IMAL internal structures are corrupted" },
  { MEM_ERR_NOT_FOUND,
    "Pointer is not allocated/locked block ptr" },
  { MEM_ERR_RELEASED,
    "Block has been released yet" },
  { MEM_WARN_CLASS_NO_CHECK,
    "Illegal operation with block of 'NO_CHECK' class" },
  { MEM_ERR_NOT_MEMORY,
    "Not enough memory to allocate block" },
  { MEM_ERR_BAD_SIZE,
    "Invalid size of requested block (0)" },
  { MEM_ERR_NULL_PTR,
    "Pointer passed to IMAL function is NULL" },
  { MEM_ERR_RTL_DAMAGE,
    "Buffer overflow in C RTL overloaded function" },
  { MEM_WARN_RTL_NOSIZE,
    "Unknown buffer size in C RTL overloaded function" },
  { MEM_ERR_IO,
    "File write failure" },
  { MEM_ERR_MODE_BAD,
    "Wrong IMAL mode" },
  { MEM_WARN_MODE_OVER,
    "Mode stack overflow" },
  { MEM_ERR_CLASS_NOT_CREATED,
    "Wrong class identifier" },
  { MEM_ERR_CLASS_NOT_RELEASED,
    "Not all blocks of closed class are released" },
  { MEM_ERR_CLASS_PREDEFINED,
    "Illegal operation with predefined class" },
  { 0, NULL }     /* table terminator */
  };


static int mem_errno = MEM_ERR_NOT_INIT;

/*
Result of the  last IMAL function  call. List of  IMAL  functions
which set this variable  is  into mem_error() function codes (see
below).
*/


static int mode_stack[MODE_STACK_SIZE] = { 0 };

/*
Stack which contains IMAL modes.  Each  mode value is the sum  of
some of the IMAL mode codes MEM_MODE_* #defined in imal.h
mode_stack[0] contains  the  mode  value  defined  by environment
variable IMAL_CFG. This value assigned by function imal_init().
*/

static long mode_stack_ind = 0;

/*
Current   mode    level.   If   mode_stack_ind   is   less   than
MODE_STACK_SIZE    then    current    mode   value   stored    in
mode_stack[mode_stack_ind]   else    current   mode   value    is
mode_stack[0].
*/

static int continue_blk_fl = FALSE;

/* Stop checking in CONTINUE mode after error */

static int imal_default_error_fun PROT((IN int, IN char*,
      IN char*, IN int, IN VPTR, IN SIZE_T));

static MEM_TYPE_ERROR_FUN imal_error_fun = imal_default_error_fun;
static MEM_TYPE_ABORT_FUN imal_abort_fun;

/*
Pointers to the  imal  error processing functions. Such functions
are either default or user's ( set by imal_init() ).
These functions can be called by imal_error_proc().
*/


static BOOL imal_mode_debug_fl = FALSE;
static BOOL imal_mode_trace_fl = FALSE;
static BOOL imal_mode_warning_fl = FALSE;
static BOOL imal_mode_continue_fl = FALSE;
static BOOL imal_mode_modify_fl = FALSE;

/* IMAL mode flags for mode.c (copies in imal.c and rtl.c are kept) */

static MODE_FLAG_REF envi_mode_tab[] =
  {
    { "debug",    &imal_mode_debug_fl },
    { "trace",    &imal_mode_trace_fl },
    { "warning",  &imal_mode_warning_fl },
    { "continue", &imal_mode_continue_fl },
    { "modify",   &imal_mode_modify_fl },
    { NULL, NULL }  /* Terminator */
  };

  /* Table for mode flags initialization by environment variable */


/* STATIC PROTOTYPES */

static int Ustrncmp PROT((IN char *, IN char *, IN SIZE_T));
static int imal_default_error_fun PROT((IN int, IN char*,
      IN char*, IN int, IN VPTR, IN SIZE_T));
static int set_mode PROT((IN int));

/****************************** CODE **********************************/


                        /* Library functions */


/* --------------------------------------------------------------------
 * NAME         mem_error
 * PURPOSE      Return current IMAL error code value.
 * ARGUMENTS    None
 * RETURNS      Error code.
 * EXTERNS      None
 *
 * NOTE         Following functions set error code:
 *              mem_alloc, mem_calloc, mem_class_create,
 *              mem_print_list, mem_print_class,
 *              mem_init, mem_close.
 *
 *              The functions set mem_errno via imal_err().
 ---------------------------------------------------------------------*/

int mem_error()
  {
  return mem_errno;
  } /* end of mem_error function */


/* --------------------------------------------------------------------
 * NAME         mem_strerr
 * PURPOSE      String representation of IMAL error codes.
 * ARGUMENTS    IN int err - Error code.
 * RETURNS      Error message.
 * EXTERNS      None.
 * NOTE         If code is wrong function returns corresponding message.
 ---------------------------------------------------------------------*/

char *mem_strerr(err)
IN int err;
  {
  int i;       /* Loop variable */

  for (i = 0; err_tab[i].msg != NULL; i++)
    {
    if (err_tab[i].code == err)
      return err_tab[i].msg;
    }
  return "Unknown error code";

  } /* end of mem_strerr function */


/* --------------------------------------------------------------------
 * NAME         mem_mode_open
 * PURPOSE      Set IMAL mode.
 * ARGUMENTS    IN int mode - Combination of MEM_MODE_* values.
 * RETURNS      MEM_OK or MEM_ERR_MODE_BAD or MEM_WARN_MODE_OVER
 * EXTERNS      None
 * NOTE         None
 ---------------------------------------------------------------------*/

int mem_mode_open(mode)
IN int mode;
  {
  mode_stack_ind++;
  if (mode_stack_ind < MODE_STACK_SIZE)
    {
    mode_stack[mode_stack_ind] = mode;
    }
  if (set_mode(mode) == FAILURE)   /* mode is uncorrect */
    {
    mem_mode_close();
    return MEM_ERR_MODE_BAD;
    }

  return  mode_stack_ind < MODE_STACK_SIZE ?
          MEM_OK : MEM_WARN_MODE_OVER;
  } /* end of mem_mode_open function */

/* --------------------------------------------------------------------
 * NAME         mem_mode_close
 * PURPOSE      Restore last IMAL mode.
 * ARGUMENTS    None
 * RETURNS      MEM_OK or MEM_WARN_MODE_OVER
 * EXTERNS      None
 * NOTE         None
 ---------------------------------------------------------------------*/

int mem_mode_close()
  {
  if (mode_stack_ind == 0)
    return MEM_WARN_MODE_OVER;
  mode_stack_ind--;
  if (mode_stack_ind < MODE_STACK_SIZE)
    {
    set_mode(mode_stack[mode_stack_ind]);
    return MEM_OK;
    }
  else
    {
    set_mode(mode_stack[0]);
    return MEM_WARN_MODE_OVER;
    }
  } /* end of mem_mode_close function */

/* --------------------------------------------------------------------
 * NAME         mem_mode_current
 * PURPOSE      Return current IMAL mode.
 * ARGUMENTS    None
 * RETURNS      Mode value.
 * EXTERNS      None
 * NOTE         None
 ---------------------------------------------------------------------*/

int mem_mode_current()
  {
  int mode;    /* IMAL mode */

  mode = 0;
  if (imal_mode_debug_fl == TRUE)
    mode |= MEM_MODE_DEBUG;
  if (imal_mode_trace_fl == TRUE)
    mode |= MEM_MODE_TRACE;
  if (imal_mode_warning_fl == TRUE)
    mode |= MEM_MODE_WARNING;
  if (imal_mode_continue_fl == TRUE)
    mode |= MEM_MODE_CONTINUE;
  if (imal_mode_modify_fl == TRUE)
    mode |= MEM_MODE_MODIFY;
  return mode;
  } /* end of mem_mode_current function */

/* --------------------------------------------------------------------
 * NAME         mem_mode_level
 * PURPOSE      Return  current   IMAL   mode  level,  i.e.  number  of
 *              mem_mode_open  successful  calls  minus number of
 *              mem_mode_close successful calls.
 * ARGUMENTS    None
 * RETURNS      Mode level.
 * EXTERNS      None
 * NOTE         None
 ---------------------------------------------------------------------*/

int mem_mode_level()
  {
  return mode_stack_ind < MODE_STACK_SIZE ?
         (int)mode_stack_ind : -1;
  } /* end of mem_mode_level function */


                       /* Interface functions */


/* --------------------------------------------------------------------
 * NAME         imal_error_proc
 * PURPOSE      IMAL Error/warning processing function.
 * ARGUMENTS    The same as for imal_default_error_fun(), see above.
 * RETURNS      None
 * EXTERNS      None
 * NOTE         Program can be stopped in this function.
 ---------------------------------------------------------------------*/

VOID imal_error_proc(err, fun, file, line, ptr, offset)
IN int err;
IN char *fun;
IN char *file;
IN int line;
IN VPTR ptr;
IN SIZE_T offset;
  {
  if (imal_error_fun != NULL)
    {
    if ((*imal_error_fun)(err, fun, file, line, ptr, offset) ==
        MEM_PROG_CONTINUE || imal_mode_continue_fl == TRUE)
      {
      if (imal_mode_continue_fl == TRUE)
        {
        /* CONTINUE mode - Stop debug/semidebug checkings */
        set_mode(mem_mode_current() &
                 ~(MEM_MODE_DEBUG | MEM_MODE_MODIFY));
        continue_blk_fl = TRUE;
        }
      return;
      }
    }
  if (imal_abort_fun != NULL)
    (*imal_abort_fun)();
  exit(1);
  } /* end of imal_error_proc function */

/* --------------------------------------------------------------------
 * NAME         imal_debug
 * PURPOSE      General IMAL checking.
 * ARGUMENTS    IN char *file - Source file name.
 *              IN int line   - Source line number.
 * RETURNS      IMAL error code - result of checking.
 * EXTERNS      None
 * NOTE         Program can be stopped in this function.
 ---------------------------------------------------------------------*/

int imal_debug(file, line)
IN char *file;
IN int line;
  {
  int err;               /* IMAL error code                        */
  VPTR damage_ptr;       /* Damaged block / internal data pointer  */
  SIZE_T damage_offset;  /* Damaged area offset in allocated block */

  damage_ptr = NULL;
  damage_offset = 0;

  if (imal_mode_debug_fl == FALSE)
    return MEM_OK;

  if ((err = mem_check_block(NULL)) != MEM_OK)
    {
    imal_get_damage_info(&damage_ptr, &damage_offset);
    }

  if (err == MEM_OK)
    {
    if ((err = mem_check_locked(NULL)) != MEM_OK)
      {
      imal_get_locked_damage_info(&damage_ptr);
      damage_offset = 0;
      }
    }

  if (err != MEM_OK)
    {
    imal_error_proc(err, "", file, line, damage_ptr, damage_offset);
    /* Program can be stopped in imal_error_proc() ! */
    }

  return err;
  } /* end of imal_debug function */


/* --------------------------------------------------------------------
 * NAME         imal_err
 * PURPOSE      Set IMAL error code value.
 * ARGUMENTS    IN int err - Error code.
 * RETURNS      None.
 * EXTERNS      None.
 * NOTE         None.
 ---------------------------------------------------------------------*/

VOID imal_err(err)
IN int err;
  {
  mem_errno = err;
  } /* end of imal_err function */

/* --------------------------------------------------------------------
 * NAME         imal_init
 * PURPOSE      Initialize error processing functions/variable,
 *              mode stack; parse IMAL_CFG environment variable
 *              and set modes and trace file name.
 *
 * ARGUMENTS    IN int  (*error_fun)() - Error processing functions.
 *              IN VOID (*abort_fun)() -
 *              OUT char *file_name    - Trace file name buffer.
 *              IN int file_name_len   - Length of this buffer.
 * RETURNS      None.
 * EXTERNS      None.
 * NOTE         Program can be stopped in this function if IMAL_CFG
 *              is set but has wrong format.
 ---------------------------------------------------------------------*/

VOID imal_init(error_fun, abort_fun, file_name, file_name_len)
IN MEM_TYPE_ERROR_FUN error_fun;
IN MEM_TYPE_ABORT_FUN abort_fun;
OUT char *file_name;
IN int file_name_len;
  {
  int i;                /* Loop variable                      */
  int pos;              /* Position in IMAL_CFG envi variable */
  char *envi;           /* IMAL_CFG environment variable      */
  char *keyword;        /* Keyword matched with IMAL_CFG      */

  /* Set error processing functions/variable */

  imal_error_fun = 
    (error_fun == NULL ? imal_default_error_fun : error_fun);
  imal_abort_fun = abort_fun;
  mem_errno = MEM_OK;

  /* Initialize IMAL mode flags and stack */

  set_mode(0);  /* Set default modes values: all are FALSE */

  /*
     If environment variables IMAL_CFG is set then set modes
     according IMAL_CFG, else use default values.
  */

  if ((envi = getenv("IMAL_CFG")) != NULL)
    {
    pos = 0;
    while (envi[pos] != '\0')   /* Parse next item of IMAL_CFG */
      {
      /* Pass until letter */
      for ( ; envi[pos] != '\0' && !isalpha(envi[pos]); pos++);

      /* Compare envi with mode switch keywords */
      for (i = 0; (keyword = envi_mode_tab[i].name) != NULL; i++)
        {
        if (Ustrncmp(envi + pos, keyword, strlen(keyword)) == 0)
          break;
        }

      /* Set IMAL mode */
      if (keyword != NULL)
        {
        pos += (int)strlen(keyword);
        if (envi[pos] != ':' || envi[pos + 1] == '\0')
	  break;
        pos++;
        if (tolower(envi[pos]) == 'y')
          {
          *(envi_mode_tab[i].flag) = TRUE;
          }
        else
          {
          if (tolower(envi[pos]) == 'n')
            {
            *(envi_mode_tab[i].flag) = FALSE;
            }
          else break;   /* Bad IMAL_CFG */
          }
        pos++;
        continue;       /* Parse the next IMAL_CFG item */
        }

      /* Set trace file name */
      keyword = "file";
      if (file_name != NULL &&
	  Ustrncmp(envi + pos, keyword, strlen(keyword)) == 0)
        {
        pos += (int)strlen(keyword);
        if (envi[pos] != '\0')
          pos++;
        for (i = 0; i < file_name_len - 1; i++)
          {
          if (envi[pos] == '\0' || envi[pos] == ',' ||
              envi[pos] == ';'  || envi[pos] == ' ')
            {
            break;
            }
          *file_name++ = envi[pos++];
          }
        *file_name = '\0';
	file_name = NULL;  /* Turn off second 'file' option */
        continue;          /* Parse the next IMAL_CFG item */
        }

      /* All keywords are not match */
      break;
      } /* end while */

    if (envi[pos] != '\0')      /* IMAL_CFG format error */
      {
      fprintf(stderr,
        "%sWrong format of IMAL_CFG environment variable\n",
        IMAL_MESSAGE_SIGN);
      fprintf(stderr, "  IMAL_CFG = '%s'\n", envi);
      fprintf(stderr, "  wrong item:");
      for (i = 0; i <= pos; i++)
        fprintf(stderr, "-");
      fprintf(stderr, "^\n\n"); 
      exit(1);
      }
    } /* end if (IMAL_CFG is set) */

  /* ASSERT: local IMAL mode flags are set (default or IMAL_CFG) */
  /* Set mode stack and flags in imal.c and rtl.c */

  mode_stack_ind = 0;
  continue_blk_fl = FALSE;
  mode_stack[0] = mem_mode_current();
  set_mode(mode_stack[0]);

  } /* end of imal_init function */


/*
* NAME       imal_print_damaged_block
* PURPOSE    Print info about corrupted block to the stream.
* ARGUMENTS  INOUT FILE *stream - Stream opened by os_fopen()
*            IN VPTR ptr    - Allocated block / locked buffer.
*            IN SIZE_T offset - Corrupted area offset.
* RETURN     SUCCESS / FAILURE.
* EXTERNS    None.
* NOTE       None.
*/

int imal_print_damaged_block(stream, ptr, offset)
INOUT FILE *stream;
IN VPTR ptr;
IN SIZE_T offset;
  {
  int i;       /* Loop variable      */
  char c;      /* Auxiliary variable */
  char *name;
  SIZE_T size;
  int class_id;

  if (ptr != NULL)
    {
    if (offset != 0)
      {
      if (fprintf(stream,
                  "  Block address: %p  damage offset: %Id\n",
                  ptr, offset) <= 0)
        return FAILURE;
      }
    else
      {
      if (fprintf(stream,
                  "  Damaged buffer address: %p\n", ptr) <= 0)
        return FAILURE;
      }
    if (fprintf(stream, "  Data: ") <= 0)
      return FAILURE;
    for (i = 0; i < N_HEX; i++)
      {
      if (fprintf(stream,
                  "%02x ", ((unsigned char*)ptr)[offset + i]) <= 0)
        return FAILURE;
      }
    if (fprintf(stream, "\n  Ascii:") <= 0)
      return FAILURE;
    for (i = 0; i < N_ASCII; i++)
      {
      c = ((char*)ptr)[offset + i];
      if (fprintf(stream, " %c ", (isprint(c) ? c : '.')) <= 0)
        return FAILURE;
      }
    if (fprintf(stream, "\n") <= 0)
      return FAILURE;
    if (offset != 0)
      {
      imal_get_corrupted_info(ptr, &name, &size, &class_id);
      if (name != NULL)
        {
        if (fprintf(stream, "  Name:") <= 0)
          return FAILURE;
        for (i = 0; i < N_NAME && name[i] != '\0'; i++)
          if (fprintf(stream,
                      "%c", (isprint(name[i]) ? name[i] : '.')) <= 0)
            return FAILURE;
        }
      if ((name = mem_query_class(class_id)) != NULL)
        {
        if (fprintf(stream, "  Class:") <= 0)
          return FAILURE;
        for (i = 0; i < N_NAME && name[i] != '\0'; i++)
          if (fprintf(stream, "%c", (isprint(name[i]) ? name[i] : '.')) <= 0)
            return FAILURE;
        }
      if (size != 0)
        if (fprintf(stream, "  Size:%Id", size) <= 0)
          return FAILURE;
      if (fprintf(stream, "\n") <= 0)
        return FAILURE;
      }
    } /* End if */
  return SUCCESS;
  } /* end of imal_print_damaged_block function */


                       /* Auxiliary functions */

/*
* NAME       Ustrncmp
* PURPOSE    Compare two strings but no more than n symbols.Ignore case.
* ARGUMENTS  IN char *s1, *s2 - strings.
*            IN SIZE_T n - Maximum number of symbols will be compared.
* RETURN     0 - Equal, 1 - not.
* EXTERNS      None
* NOTE         None
*/

static int Ustrncmp(s1, s2, n)
IN char *s1, *s2;
IN SIZE_T n;
  {
  SIZE_T nsym;

  nsym = n;
  while (*s1 != '\0' && *s2 != '\0' && nsym > 0)
    {
    if (tolower(*s1) != tolower(*s2))
      {
      return 1;
      }
    s1++;
    s2++;
    nsym--;
    }
  return (nsym == 0 || *s1 == *s2) ? 0 : 1;
  } /* end of Ustrncmp function */


/* --------------------------------------------------------------------
 * NAME         imal_default_error_fun
 * PURPOSE      Default IMAL error processing (print to stdout).
 * ARGUMENTS
 *       IN int   err    - Error/warning code (See table above).
 *                         Following codes are possible:
 *                         MEM_ERR_RTL_DAMAGE, MEM_WARN_RTL_NOSIZE,
 *                         MEM_ERR_BLOCK_DAMAGE, MEM_ERR_BUFFER_DAMAGE,
 *                         MEM_ERR_FATAL, MEM_ERR_NOT_FOUND,
 *                         MEM_ERR_RELEASED, MEM_ERR_NOT_MEMORY.
 *
 *       IN char *fun    - if  error  occurred  during  'overloaded'  RTL
 *                         function  -  name  of  that  function   (e.g.
 *                         "strcpy"). In other case empty string.
 *
 *       IN int line     - Source line number (of function during which
 *                         error was found).
 *       IN char *file   - Source file name (of function during which
 *                         error was found).
 *
 *       IN VPTR ptr     - Pointer to start of the damaged block or NULL
 *       IN SIZE_T offset- Offset from the 'ptr' to damaged data
 *                         (negative = control structure before block
 *                         is damaged).
 * RETURNS      None
 * EXTERNS      None
 * NOTE         None
 ---------------------------------------------------------------------*/

static int imal_default_error_fun(err, fun, file, line, ptr, offset)
IN int err;
IN char *fun;
IN char *file;
IN int line;
IN VPTR ptr;
IN SIZE_T offset;
  {

  /* Print error message */
  if (fun == NULL || fun[0] == '\0')
    printf("%sError:%s\n", IMAL_MESSAGE_SIGN, mem_strerr(err));
  else
    {
    printf("%s%s in fun. %s: %s\n",
           IMAL_MESSAGE_SIGN,
           (err == MEM_WARN_RTL_NOSIZE ? "Warning" : "Error"),
           fun, mem_strerr(err));
    }
  printf("  Source file '%s'  source line: %d\n", file, line);

  /* Print info about corrupted block */
  imal_print_damaged_block(stdout, ptr, offset);
  printf("\n");

  return err == MEM_WARN_RTL_NOSIZE ? MEM_PROG_CONTINUE : MEM_PROG_STOP;
  } /* end of imal_default_error_fun function */


/* --------------------------------------------------------------------
 * NAME         set_mode
 * PURPOSE      Set IMAL mode flags in mode.c imal.c rtl.c
 * ARGUMENTS    IN int mode_value - Combination of MEM_MODE_* values.
 * RETURNS      SUCCESS - Correct mode value.
 *              FAILURE - Wrong mode value.
 * EXTERNS      None
 * NOTE         Mode flags can be modified only by this function !!!
 ---------------------------------------------------------------------*/

static int set_mode(mode_value)
IN int mode_value;       /* IMAL mode */
  {
  int mode;

  mode = mode_value;
  if (continue_blk_fl == TRUE)
    mode &= ~(MEM_MODE_DEBUG | MEM_MODE_MODIFY);
  if (mode & (~(MEM_MODE_DEBUG |
                MEM_MODE_TRACE |
                MEM_MODE_WARNING |
                MEM_MODE_CONTINUE |
                MEM_MODE_MODIFY)))
    return FAILURE;

  imal_mode_debug_fl = (BOOL)((mode & MEM_MODE_DEBUG) ? TRUE : FALSE);
  imal_mode_trace_fl = (BOOL)((mode & MEM_MODE_TRACE) ? TRUE : FALSE);
  imal_mode_warning_fl = (BOOL)((mode & MEM_MODE_WARNING) ? TRUE : FALSE);
  imal_mode_continue_fl = (BOOL)((mode & MEM_MODE_CONTINUE) ? TRUE : FALSE);
  imal_mode_modify_fl = (BOOL)((mode & MEM_MODE_MODIFY) ? TRUE : FALSE);

  imal_mode_set(imal_mode_debug_fl,
                imal_mode_trace_fl,
                imal_mode_modify_fl);

  imal_mode_rtl_set(imal_mode_debug_fl,
                    imal_mode_warning_fl);

  return SUCCESS;
  } /* end of set_mode function */


/*** End of file MODE.C ***/
