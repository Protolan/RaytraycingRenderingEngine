/*
 * MODULE NAME  IMAL.C
 * PURPOSE      IMAL   library   functions  which   operate  with
 *              allocated  blocks;   memory  query  /   statistic
 *              functions;  IMAL   initialization  /  termination
 *              functions; IMAL basic internal data structures.
 *              Module imal.c uses functions of locked.c, class.c
 *              and mode.c modules.
 *
 * SPEC         V. L. Volevich                        1993.01.12
 * DESIGN       V. L. Volevich                        1993.06.03
 * DES. INSP.   Z. Blaszczyk                          1993.06.28
 * CODING       V. L. Volevich                        1993.07.24
 * CODE INSP.   Z. Blaszczyk                          1993.09.23
 * TEST         V. L. Volevich                        1993.08.10
 *
 * LAST UPDATED V. L. Volevich                        1993.10.07
 *              Enk                                   1998.06.05
 *              Ose  mem_realloc_fun()  ebug01nb.Ose   '00.02.02
 *              Eed  mem_realloc_fun()  ebug36nn.Eed   '00.02.10
 *
 */

static char *sccsid =
"@(#)$Header: /home1/repository/envi/REPOSITORY/envi/portab/imal/imal.c,v 1.3 2000/02/10 07:09:50 envi Exp $";
/*
"@(#)$Header: /home1/repository/envi/REPOSITORY/envi/portab/imal/imal.c,v 1.3 2000/02/10 07:09:50 envi Exp $";
*/

/* COMPANY INCLUDES */

#define IMAL_NO_C_RTL_MACRO  /* Turn off macros for C RTL functions */

#include "integra.h"
#include "iosl.h"
#include "imal.h"


/* PROJECT INCLUDE: */

#include "imal_loc.h"

/* MACROS */

#define MODE_SEMIDEBUG \
   (imal_mode_modify_fl == TRUE)

/* CONSTANTS */

#define MAX_IMAL_NAME_LEN 100    /* Maximum number of symbols in  */
                                 /* block/class name which can be */
                                 /* printed to the trace file.    */

#define N_BLOCK_CLASS 16  /* Number of positions for a class name */
#define N_BLOCK_NAME  30  /* Number of positions for a block name */


#define CHECK_BAD_PREV_PTR    0x01   /* Values used in allocated */
#define CHECK_BAD_PREV_BLOCK  0x02   /* block checking procedure */
#define CHECK_BAD_NEXT_PTR    0x04
#define CHECK_BAD_NEXT_BLOCK  0x08
#define CHECK_BAD_PREFIX      0x10
#define CHECK_BAD_POSTFIX     0x20
#define CHECK_BAD_SIZE        0x40


/*-------------------------------------------------------------/
                   Internal data structures.

IMAL  internal  dynamical  structures includes two  way  list  of
allocated  blocks and some auxiliary variables.
Allocated blocks of the same class  follow  immediately  by  each
other  in  the  list  (about class descriptors see class.c). Each
allocated block  contains  the  identifier  of  the corresponding
block  class  descriptor, each class descriptor has reference  to
the first allocated block of this class.
Block list is  empty  before the first mem_alloc/mem_calloc call.
Block list can be modified by functions mem_alloc  / mem_calloc /
mem_free. Memory  statistic  functions  get information from this
list.
IMAL functions walk down  this  list by means of mem_block_next()
function.

Example of block/class structure:


                  +----<----------------<-----------------+
                  V                                       |
   +-----+    +---------+    +---------+    +---------+   |
   |block|--->|block1 of|--->|block2 of|--->|block3 of|   |
   |head |<---|class1   |<---|class1   |<---|class1   |   |
   +-----+    +---------+    +---------+    +---------+   |
     |  ^                                      |  ^       |
     |  |                                      V  |       ^
     |  |     +---------+    +---------+    +---------+   |
     |  +--<--|block2 of|--->|block1 of|--->|block1 of|   |
     +------->|class4   |<---|class4   |<---|class2   |   |
              +---------+    +---------+    +---------+   |
                               ^              ^           |
                  +----->------+              |           |
                  |                           |           |
             +-------+     +-------+     +-------+    +-------+
             |class4 |     |class3 |     |class2 |    |class1 |
             +-------+     +-------+     +-------+    +-------+
                                |
                                +---> NULL

/-------------------------------------------------------------*/

/****************************** DATA *************************************/

static BOOL imal_init_fl = FALSE;

  /* Is IMAL initialized by function mem_init ? */

static BOOL imal_mode_debug_fl = FALSE;
static BOOL imal_mode_trace_fl = FALSE;
static BOOL imal_mode_modify_fl = FALSE;

  /* IMAL mode flags. Set by imal_mode_set() from mode.c */


static DOUBLE_PTR block_head = { &block_head, &block_head };

  /*
     Header of two  way list of allocated blocks.
     Blocks of the same class follow immediately by each other.
  */

static SIZE_T max_blocks_size = 0;
static SIZE_T max_csize = 0;
static SIZE_T total_blocks_size = 0;
static SIZE_T total_csize = 0;

  /* Info about maximum memory during run time of the program. */

static SIZE_T imal_blocks_number = 0;

  /* Current number of allocated blocks. */

static BOOL imal_tag_check_fl = TRUE;

  /*
    This flag manage the prefix/postfix  tag  checking  in IMAL query
    functions.   When   the  flag  is  FALSE  IMAL  query   functions
    mem_block_size, mem_query_block, mem_block,  mem_block_next  does
    not  check  the  prefix/postfix  area  of  ptr  passed  to  them.
    Functions give info  about block even  if it is  corrupted.  This
    feature   allows   to   IMAL  trace  functions   (mem_print_list,
    mem_print_class)  to  give  full  info  about  corrupted  blocks.
    Usually this flag is TRUE.  IMAL  functions can set it to  FALSE,
    call query functions and then set it to TRUE again.
  */


static VPTR imal_damage_ptr = NULL;
static SIZE_T imal_damage_offset = 0;

  /*
     Pointer to corrupted block or NULL. Corrupted data offset.
     These variable can be set  by  functions  mem_check_block,
     imal_check_block_ptr. Access via imal_get_damage_info().
  */

static char trace_file_name[MAX_PATHNAME_LENGTH + 1] = "memtrace.out";
static FILE *trace_file_ptr = NULL;
static BOOL trace_continue_fl = FALSE;

  /*
     Specify   current   IMAL   trace   file.   NULL   value   of
     trace_file_ptr means  "trace  file  is  closed  now".  Empty
     string in trace_file_name means that trace file is stderr.
     If flag trace_continue_fl is TRUE  then  trace  file will be
     opened  in  "a" mode i.e. it  will  be appended. If flag  is
     FALSE then a new trace file will be opened.
  */

/* STATIC PROTOTYPES */

static int print_name(INOUT FILE *, IN char *, IN int);
static int imal_check_block_ptr(IN VPTR, IN int);
static VOID trace_out(IN int, IN char *, IN char *,
                            IN SIZE_T, IN SIZE_T,
                            IN char *, IN int,
                            IN VPTR, IN VPTR, IN int);

/* Disable "cast truncates constant value" warning */
#pragma warning(disable : 4310)

/****************************** CODE *************************************/


                        /* Library functions */

/* --------------------------------------------------------------------
 * NAME         imal_version
 * PURPOSE      Get version data
 * ARGUMENTS    None.
 * RETURNS      Pointer to version data
 * EXTERNS      None.
 * NOTES        It is forbidden to use this function in applications
 ---------------------------------------------------------------------*/

char *imal_version()
  {
  return sccsid;
  } /* end of 'imal_version' function */


/* --------------------------------------------------------------------
 * NAME         mem_init
 * PURPOSE      Initialize INAL library.
 * ARGUMENTS    IN int  (*error_fun)() - Error processing functions.
 *              IN VOID (*abort_fun)() -
 *              IN char *trace_file_name_par - Name of the trace file
 *                         or NULL (use default name / IMAL_CFG).
 * RETURNS      None.
 * EXTERNS      None.
 *
 * NOTE         Program can be stopped in this function (If environment
 *              variable IMAL_CFG has wrong value).
 *              It is possible to initialize IMAL again  after the
 *              function mem_close() call. But it is not allowed to call
 *              mem_init() twice without mem_close() call.
 ---------------------------------------------------------------------*/

int mem_init(error_fun, abort_fun, trace_file_name_par)
IN MEM_TYPE_ERROR_FUN error_fun;
IN MEM_TYPE_ABORT_FUN abort_fun;
IN char *trace_file_name_par;
  {
  int len;

  if (imal_init_fl == TRUE)     /* IMAL is still initialized */
    {
    imal_err(MEM_ERR_INIT);
    return MEM_ERR_INIT;
    }

  imal_init_fl = TRUE;

  /* If second initialization then release old blocks */
  while (imal_blocks_number > 0 &&
         mem_free(block_head.next) == MEM_OK);
  imal_blocks_number = 0;

  /* Init error processing, mode stack,    */
  /* parse IMAL_CFG  environment variable. */

  len = sizeof(trace_file_name) / sizeof(char);
  imal_init(error_fun, abort_fun, trace_file_name, len);
  if (trace_file_name_par != NULL)
    {
    strncpy(trace_file_name, trace_file_name_par, len);
    trace_file_name[len - 1] = '\0';
    }

  imal_err(MEM_OK);
  return MEM_OK;
  } /* end of mem_init function */

/* --------------------------------------------------------------------
 * NAME         mem_close
 * PURPOSE      Terminate IMAL. Check the correctness of dynamical
 *              memory.
 * ARGUMENTS    None
 * RETURNS      IMAL error code:
 *              MEM_OK, MEM_ERR_NOT_INIT, MEM_ERR_NOT_EMPTY,
 *              MEM_ERR_BLOCK_DAMAGE, MEM_ERR_BUFFER_DAMAGE,
 *              MEM_ERR_FATAL.
 * EXTERNS      None
 * NOTE         None
 ---------------------------------------------------------------------*/

int mem_close()
  {
  int class_id;
  int last_class_id;
  SIZE_T blk_num, dummy;

  /* Check that IMAL is initialized */
  if (imal_init_fl == FALSE)
    return MEM_ERR_NOT_INIT;

  /* Close trace file (trace_out() reopen it automatically) */
  if (trace_file_ptr != NULL)
    {
    if (trace_file_name[0])
      os_fclose(trace_file_ptr);
    trace_file_ptr = NULL;
    }

  /* Check allocated blocks */
  if (mem_check_block(NULL) != MEM_OK)
    return MEM_ERR_BLOCK_DAMAGE;

  /* Check and unlock locked buffers */
  if (mem_static_release(NULL) != MEM_OK)
    return MEM_ERR_BUFFER_DAMAGE;

  /* Check that all blocks are released */
  if (block_head.next != &block_head ||
      mem_class_size(MEM_CLASS_NO_CHECK, MEM_INFO_CURRENT,
                     &blk_num, &dummy, &dummy) != MEM_OK ||
      blk_num != 0)
    return MEM_ERR_NOT_EMPTY;

  /* Everything O.K. - close IMAL */

  /* Close class descriptors */
  last_class_id = MEM_NOT_CLASS;
  while ((class_id = imal_class_next(last_class_id)) != MEM_NOT_CLASS)
    {
    if (mem_class_close(class_id) != MEM_OK)
      last_class_id = class_id;         /* Pass predefined class */
    }

  /* Mark that IMAL is closed */
  imal_err(MEM_ERR_NOT_INIT);
  imal_init_fl = FALSE;

  return MEM_OK;
  } /* end of mem_close function */


/* --------------------------------------------------------------------
 * NAME         mem_alloc_fun
 * PURPOSE      Allocate a block of memory with imal control part.
 * ARGUMENTS    IN int  class_id;           - Block class ident.
 *              IN char *blk_name;          - Block name
 *              IN SIZE_T blk_size;         - Block size in bytes
 *              IN char *filename;          - Source file name
 *              IN int fileline;            - Source line
 *
 * RETURNS      Pointer to allocated block or NULL on failure.
 *              IMAL error code will be set.
 *
 * EXTERNS      None
 * NOTE         None
 ---------------------------------------------------------------------*/

VPTR mem_alloc_fun(class_id, blk_name, blk_size, filename, fileline)
IN int  class_id;
IN char *blk_name;
IN SIZE_T blk_size;
IN char *filename;
IN int fileline;
  {
  VPTR p;              /* Allocated block pointer        */
  DOUBLE_PTR *list;    /* Block's list fields pointer    */
  CLASS_INFO *pclass;  /* Block's class descriptor       */
  int name_size;       /* Length of the block's name     */
  SIZE_T size_all;     /* size of block allocated by malloc */
  int err;             /* IMAL error code                */

  err = MEM_OK;   /* Set default error code (O.K.) */

  /* Check that IMAL is initialized */
  if (imal_init_fl == FALSE)
    {
    imal_err(MEM_ERR_NOT_INIT);
    return NULL;
    }

  /* Check block's size */
  if (blk_size == 0)
    imal_err(err = MEM_ERR_BAD_SIZE);

  /* Check block's name */
  if (blk_name == NULL)
    imal_err(err = MEM_ERR_NULL_PTR);

  /* Get class descriptor / check class_id */
  if ((pclass = imal_class_id_to_ptr(class_id)) == NULL)
    imal_err(err = imal_class_check_id(class_id));

  if (err != MEM_OK)
    {
    if (imal_mode_trace_fl == TRUE)
      {
      trace_out('A', blk_name, mem_query_class(class_id),
                blk_size, 1, filename, fileline,
                NULL, NULL, err);
      }
    return NULL;
    }

  /* Make debug checking if necessary */
  if (imal_mode_debug_fl == TRUE)
    {
    err = imal_debug(filename, fileline);
    /* !!! Program can be stopped in imal_debug() */
    /*     If damage has been found but program not stopped then  */
    /*     error flag will be set and execution will be continued */
    }

  /* Allocate block. Initialize it. Set statistic variables */
  if (class_id == MEM_CLASS_NO_CHECK)
    {
    /* Allocate block of NO_CHECK class (without control part) */

    /* Allocate block */
    size_all = IMAL_BLOCK_NOCHECK_SIZE(blk_size);
    if ((p = malloc(size_all)) == NULL)
      {
      imal_err(MEM_ERR_NOT_MEMORY);
      if (imal_mode_trace_fl == TRUE)
        {
        trace_out('A', blk_name, mem_query_class(class_id),
                  blk_size, 1, filename, fileline,
                  NULL, NULL, MEM_ERR_NOT_MEMORY);
        }
      return NULL;
      }

    /* Set prefix */
    p = IMAL_BLOCK_NOCHECK_USER(p);
    IMAL_BLOCK_SET_PREFIX(p, IMAL_TAG_NO_CHECK);
    if (++(pclass->block_num) > pclass->max_block_num)
      pclass->max_block_num = pclass->block_num;
    if ((total_csize += IMAL_TAG_SIZE) > max_csize)
      max_csize = total_csize;
    }   /* end if (NO_CHECK) */
  else
    {
    /* Allocate an ordinary block (block with control part) */

    /* pclass - ptr to the class descriptor of the allocated block */
    if (pclass->block != NULL)
      {
      list = IMAL_BLOCK_PTR(pclass->block);
      }
    else {
      list = block_head.next;
      }
    /* list points to the list ptr of the first block of            */
    /* the class class_id or to the first block in the block's list */
    /* (if there are no blocks of class_id class).                  */

    if (MODE_SEMIDEBUG)
      {
      /* Check list where the new block will be inserted */
      if (list != &block_head &&
          mem_check_block(IMAL_BLOCK_LIST_TO_USER(list)) != MEM_OK)
        {
        imal_err(MEM_ERR_FATAL);
        if (imal_mode_trace_fl == TRUE)
          {
          trace_out('A', blk_name, mem_query_class(class_id),
                    blk_size, 1, filename, fileline,
                    NULL, NULL, MEM_ERR_FATAL);
          }
        imal_error_proc(MEM_ERR_FATAL, "", filename, fileline, NULL, 0);
        /* !!! Program can be stopped in imal_error_proc() */
        return NULL;
        } /* end if (list checking) */
      } /* end if SEMIDEBUG */

    /* Allocate and initialize block */
    if ((name_size = (int)strlen(blk_name)) > MAX_IMAL_NAME_LEN)
      name_size = MAX_IMAL_NAME_LEN;
    size_all = IMAL_BLOCK_SIZE(blk_size, name_size);
    if ((p = malloc(size_all)) == NULL)
      {
      imal_err(MEM_ERR_NOT_MEMORY);
      if (imal_mode_trace_fl == TRUE)
        {
        trace_out('A', blk_name, mem_query_class(class_id),
                  blk_size, 1, filename, fileline,
                  NULL, NULL, MEM_ERR_NOT_MEMORY);
        }
      return NULL;
      }

    /* Set block's name */
    if (blk_name[name_size] == '\0')
      strcpy(IMAL_BLOCK_SYS_TO_NAME(p), blk_name);
    else
      {
      memcpy(IMAL_BLOCK_SYS_TO_NAME(p), blk_name, name_size);
      IMAL_BLOCK_SYS_TO_NAME(p)[name_size] = '\0';
      }

    /* Initialize control block */
    p = IMAL_BLOCK_SYS_TO_USER(p, name_size);
    IMAL_BLOCK_CLASS_ID(p) = class_id;
    IMAL_BLOCK_NAME_LEN(p) = name_size;
    IMAL_BLOCK_USER_LEN(p) = blk_size;
    IMAL_BLOCK_SET_PREFIX(p, IMAL_TAG_PREFIX);
    IMAL_BLOCK_SET_POSTFIX(p, IMAL_TAG_POSTFIX);

    /* Insert the new block before list */
    IMAL_BLOCK_PTR(p)->prev = list->prev;
    IMAL_BLOCK_PTR(p)->next = list;
    list->prev->next = IMAL_BLOCK_PTR(p);
    list->prev = IMAL_BLOCK_PTR(p);

    /* Insert block to the list of its class */
    pclass->block = p;

    /* Correct statistic variables */
    imal_blocks_number++;
    if ((total_blocks_size += blk_size) > max_blocks_size)
      max_blocks_size = total_blocks_size;
    if ((total_csize += size_all - blk_size) > max_csize)
      max_csize = total_csize;

    /* Correct class statistic variables */
    if (++(pclass->block_num) > pclass->max_block_num)
      pclass->max_block_num = pclass->block_num;
    if ((pclass->block_csize += size_all - blk_size) >
        pclass->max_block_csize)
      pclass->max_block_csize = pclass->block_csize;
    if ((pclass->block_size += blk_size) > pclass->max_block_size)
      pclass->max_block_size = pclass->block_size;
    } /* end else (class is not NO_CHECK) */

  /* Trace successful ALLOC operation */
  if (imal_mode_trace_fl == TRUE)
    {
    trace_out('A', blk_name, mem_query_class(class_id),
              blk_size, 1, filename, fileline,
              p, NULL, MEM_OK);
    }
  imal_err(err);
  return p;
  } /* end of mem_alloc_fun function */


/* --------------------------------------------------------------------
 * NAME         mem_calloc_fun
 * PURPOSE      Allocate a block of memory with imal control part
 *              and clear it.
 * ARGUMENTS    IN int  class_id;           - Block class ident
 *              IN char *blk_name;          - Block name
 *              IN SIZE_T item_size         - Item size in bytes
 *              IN SIZE_T item_num          - Number of items in block
 *              IN char *filename;          - Source file name
 *              IN int fileline;            - Source line
 *
 * RETURNS      Pointer to allocated block or NULL on failure.
 *              IMAL error code will be set.
 *
 * EXTERNS      None
 * NOTE         None
 ---------------------------------------------------------------------*/

VPTR mem_calloc_fun(class_id, blk_name, item_size, item_num,
                 filename, fileline)
IN int  class_id;
IN char *blk_name;
IN SIZE_T item_size;
IN SIZE_T item_num;
IN char *filename;
IN int fileline;
  {
  VPTR p;              /* Allocated block pointer        */
  DOUBLE_PTR *list;    /* Block's list fields pointer    */
  CLASS_INFO *pclass;  /* Block's class descriptor       */
  int name_size;       /* Length of the block's name     */
  SIZE_T size_all;     /* size of block allocated by malloc */
  SIZE_T blk_size;     /* Size of block                  */
  int err;             /* IMAL error code                */

  err = MEM_OK;   /* Set default error code (O.K.) */

  /* Check that IMAL is initialized */
  if (imal_init_fl == FALSE)
    {
    imal_err(MEM_ERR_NOT_INIT);
    return NULL;
    }

  /* Check block's size */
  blk_size = item_size * item_num;
  if (blk_size == 0)
    imal_err(err = MEM_ERR_BAD_SIZE);

  /* Check block's name */
  if (blk_name == NULL)
    imal_err(err = MEM_ERR_NULL_PTR);

  /* Get class descriptor / check class_id */
  if ((pclass = imal_class_id_to_ptr(class_id)) == NULL)
    imal_err(err = imal_class_check_id(class_id));

  if (err != MEM_OK)
    {
    if (imal_mode_trace_fl == TRUE)
      {
      trace_out('C', blk_name, mem_query_class(class_id),
                item_size, item_num, filename, fileline,
                NULL, NULL, err);
      }
    return NULL;
    }

  /* Make debug checking if necessary */
  if (imal_mode_debug_fl == TRUE)
    {
    err = imal_debug(filename, fileline);
    /* !!! Program can be stopped in imal_debug() */
    /*     If damage has been found but program not stopped then  */
    /*     error flag will be set and execution will be continued */
    }

  /* Allocate block. Initialize it. Set statistic variables */
  if (class_id == MEM_CLASS_NO_CHECK)
    {
    /* Allocate block of NO_CHECK class (without control part) */
    /* Allocate block */
    size_all = IMAL_BLOCK_NOCHECK_SIZE(blk_size);
    if ((p = calloc(size_all, 1)) == NULL)
      {
      imal_err(MEM_ERR_NOT_MEMORY);
      if (imal_mode_trace_fl == TRUE)
        {
        trace_out('C', blk_name, mem_query_class(class_id),
                  item_size, item_num, filename, fileline,
                  NULL, NULL, MEM_ERR_NOT_MEMORY);
        }
      return NULL;
      }
    /* Set prefix */
    p = IMAL_BLOCK_NOCHECK_USER(p);
    IMAL_BLOCK_SET_PREFIX(p, IMAL_TAG_NO_CHECK);
    if (++(pclass->block_num) > pclass->max_block_num)
      pclass->max_block_num = pclass->block_num;
    if ((total_csize += IMAL_TAG_SIZE) > max_csize)
      max_csize = total_csize;
    }   /* end if (NO_CHECK) */
  else
    {
    /* Allocate an ordinary block (block with control part) */

    /* pclass - ptr to the class descriptor of the allocated block */
    if (pclass->block != NULL)
      {
      list = IMAL_BLOCK_PTR(pclass->block);
      }
    else {
      list = block_head.next;
      }
    /* list points to the list ptr of the first block of          */
    /* the class class_id or to the first block in the block's list */
    /* (if there are no blocks of class_id class).                  */

    if (MODE_SEMIDEBUG)
      {
      /* Check list where the new block will be inserted */
      if (list != &block_head &&
          mem_check_block(IMAL_BLOCK_LIST_TO_USER(list)) != MEM_OK)
        {
        imal_err(MEM_ERR_FATAL);
        if (imal_mode_trace_fl == TRUE)
          {
          trace_out('C', blk_name, mem_query_class(class_id),
                    item_size, item_num, filename, fileline,
                    NULL, NULL, MEM_ERR_FATAL);
          }
        imal_error_proc(MEM_ERR_FATAL, "", filename, fileline, NULL, 0);
        /* !!! Program can be stopped in imal_error_proc() */
        return NULL;
        } /* end if (list checking) */
      } /* end if SEMIDEBUG */

    /* Allocate and initialize block */
    if ((name_size = (int)strlen(blk_name)) > MAX_IMAL_NAME_LEN)
      name_size = MAX_IMAL_NAME_LEN;
    size_all = IMAL_BLOCK_SIZE(blk_size, name_size);
    if ((p = calloc(1, size_all)) == NULL)
      {
      imal_err(MEM_ERR_NOT_MEMORY);
      if (imal_mode_trace_fl == TRUE)
        {
        trace_out('C', blk_name, mem_query_class(class_id),
                  item_size, item_num, filename, fileline,
                  NULL, NULL, MEM_ERR_NOT_MEMORY);
        }
      return NULL;
      }

    /* Set block's name */
    if (blk_name[name_size] == '\0')
      strcpy(IMAL_BLOCK_SYS_TO_NAME(p), blk_name);
    else
      {
      memcpy(IMAL_BLOCK_SYS_TO_NAME(p), blk_name, name_size);
      IMAL_BLOCK_SYS_TO_NAME(p)[name_size] = '\0';
      }

    /* Initialize control block */
    p = IMAL_BLOCK_SYS_TO_USER(p, name_size);
    IMAL_BLOCK_CLASS_ID(p) = class_id;
    IMAL_BLOCK_NAME_LEN(p) = name_size;
    IMAL_BLOCK_USER_LEN(p) = blk_size;
    IMAL_BLOCK_SET_PREFIX(p, IMAL_TAG_PREFIX);
    IMAL_BLOCK_SET_POSTFIX(p, IMAL_TAG_POSTFIX);

    /* Insert the new block before list */
    IMAL_BLOCK_PTR(p)->prev = list->prev;
    IMAL_BLOCK_PTR(p)->next = list;
    list->prev->next = IMAL_BLOCK_PTR(p);
    list->prev = IMAL_BLOCK_PTR(p);

    /* Insert block to the list of its class */
    pclass->block = p;

    /* Correct statistic variables */
    imal_blocks_number++;
    if ((total_blocks_size += blk_size) > max_blocks_size)
      max_blocks_size = total_blocks_size;
    if ((total_csize += size_all - blk_size) > max_csize)
      max_csize = total_csize;

    /* Correct class statistic variables */
    if (++(pclass->block_num) > pclass->max_block_num)
      pclass->max_block_num = pclass->block_num;
    if ((pclass->block_csize += size_all - blk_size) >
        pclass->max_block_csize)
      pclass->max_block_csize = pclass->block_csize;
    if ((pclass->block_size += blk_size) > pclass->max_block_size)
      pclass->max_block_size = pclass->block_size;
    } /* end else (class is not NO_CHECK) */

  /* Trace successful CALLOC operation */
  if (imal_mode_trace_fl == TRUE)
    {
    trace_out('C', blk_name, mem_query_class(class_id),
              item_size, item_num, filename, fileline,
              p, NULL, MEM_OK);
    }
  imal_err(err);
  return p;
  } /* end of mem_calloc_fun function */


/* --------------------------------------------------------------------
 * NAME         mem_free_fun
 * PURPOSE      Release block pointed by ptr. Block must be allocated
 *              before by mem_alloc_fun() / mem_calloc_fun().
 * ARGUMENTS    IN VPTR ptr - Allocated block pointer.
 *              IN char *filename;          - Source file name
 *              IN int fileline;            - Source line
 *
 * RETURNS      IMAL error code:
 *              MEM_OK, MEM_ERR_NOT_INIT, MEM_ERR_NOT_FOUND,
 *              MEM_ERR_RELEASED, MEM_ERR_FATAL,
 *              MEM_ERR_BLOCK_DAMAGE, MEM_ERR_BUFFER_DAMAGE.
 *
 * EXTERNS      None
 *              imal_mode_modify_fl.
 * NOTE         None
 ---------------------------------------------------------------------*/

int mem_free_fun(ptr, filename, fileline)
IN VPTR ptr;
IN char *filename;
IN int fileline;
  {
  CLASS_INFO *pclass;  /* Block's class descriptor pointer         */
  int err;             /* IMAL error code                          */
  int debug_err;       /* IMAL error code: result of debug checking*/
  char *p;             /* Pointer used for user's data modification*/
  int class_id;        /* Block's class identifier                 */
  char *name;          /* Block's name                             */

  /* Check that IMAL is initialized */
  if (imal_init_fl == FALSE)
    {
    return MEM_ERR_NOT_INIT;
    }

  /* Check released block, put result to the err */

  if (ptr == NULL)
    {
    err = MEM_ERR_NULL_PTR;
    imal_damage_ptr = NULL;
    }
  else if (MODE_SEMIDEBUG)
    {
    /* Deep checking mode */
    err = mem_check_block(ptr);
    }
  else  /* quick checking mode */
    {
    if (IMAL_BLOCK_EQUAL_PREFIX(ptr, IMAL_TAG_NO_CHECK))
      {
      /* Block without control part */
      err = MEM_WARN_CLASS_NO_CHECK;
      }
    else if (IMAL_BLOCK_EQUAL_PREFIX(ptr, IMAL_TAG_PREFIX) &&
             IMAL_BLOCK_EQUAL_POSTFIX(ptr, IMAL_TAG_POSTFIX))
      {
      /* Block seems like correct */
      err = MEM_OK;
      }
    else
      {
      /* Block is corrupted - find exact reason */
      err = mem_check_block(ptr);
      }
    } /* end else (brief checking mode) */

  /* Make debug checking if necessary */

  debug_err = MEM_OK;
  if (imal_mode_debug_fl == TRUE &&
      (err == MEM_OK || err == MEM_WARN_CLASS_NO_CHECK))
    {
    debug_err = imal_debug(filename, fileline);
    /* !!! Program can be stopped in imal_debug() */
    /*     If damage has been found but program not stopped then    */
    /*     execution will be continued, error code will be returned */
    }

  /* Release block of class "NO_CHECK" (without control part) */

  if (err == MEM_WARN_CLASS_NO_CHECK)   /* class is NO_CHECK */
    {
    /* Modify control part (to avoid further block usage) */
    IMAL_BLOCK_SET_PREFIX(ptr, IMAL_TAG_RELEASED);
    if (imal_mode_modify_fl == TRUE)
      *(char*)ptr = (char)(~(*(char*)ptr));
    if (imal_mode_trace_fl == TRUE)
      {
      trace_out('F', "NO_NAME", mem_query_class(MEM_CLASS_NO_CHECK),
                0, 0, filename, fileline, ptr, NULL, MEM_OK);
      }
    imal_class_id_to_ptr(MEM_CLASS_NO_CHECK)->block_num--;
    total_csize -= IMAL_TAG_SIZE;
    free(IMAL_BLOCK_NOCHECK_BEGIN(ptr));
    return debug_err;
    } /* end if (NO_CHECK) */

  /* If block is corrupted */

  if (err != MEM_OK)
    {
    /* Perform error processing procedure and return error code */

    block_corrupted:
    if (imal_mode_trace_fl == TRUE)
      {
      /* Allow to get info from bad blocks */
      imal_tag_check_fl = FALSE;  /* Allow to get info from bad block */
      name = mem_query_block(ptr, &class_id);
      imal_tag_check_fl = TRUE;   /* Restore default value */
      trace_out('F', name, mem_query_class(class_id),
                0, 0, filename, fileline, ptr, NULL, err);
      }
    imal_error_proc(err, "mem_free", filename, fileline,
                    imal_damage_ptr, imal_damage_offset);
    /* !!! Program can be stopped in imal_error_proc() */
    return err;
    } /* end if (bad block ... ) */

  /* Get class info */
  if ((pclass = imal_class_id_to_ptr(IMAL_BLOCK_CLASS_ID(ptr))) == NULL)
    {
    err = MEM_ERR_FATAL;
    goto block_corrupted;       /* See above */
    }

  /* Remove block from class list and block list */
  if (pclass->block == ptr)
    {           /* ptr is first block of its class */
    if (pclass->block_num <= 1)
      {         /* there is only one block of the class */
      pclass->block = NULL;
      }
    else
      {
      pclass->block =
        IMAL_BLOCK_LIST_TO_USER(IMAL_BLOCK_PTR(ptr)->next);
      }
    }
  IMAL_BLOCK_PTR(ptr)->prev->next = IMAL_BLOCK_PTR(ptr)->next;
  IMAL_BLOCK_PTR(ptr)->next->prev = IMAL_BLOCK_PTR(ptr)->prev;

  /* correct statistic variables */
  imal_blocks_number--;
  total_blocks_size -= IMAL_BLOCK_USER_LEN(ptr);
  total_csize -= IMAL_BLOCK_CONTROL_SIZE(ptr);
  pclass->block_num--;
  pclass->block_size -= IMAL_BLOCK_USER_LEN(ptr);
  pclass->block_csize -= IMAL_BLOCK_CONTROL_SIZE(ptr);

  /* Trace successful FREE operation */
  if (imal_mode_trace_fl == TRUE)
    {
    name = mem_query_block(ptr, &class_id);
    trace_out('F', name, mem_query_class(class_id),
              IMAL_BLOCK_USER_LEN(ptr), 1, filename, fileline,
              ptr, NULL, MEM_OK);
    }

  /* Modify control part (to avoid further block usage) */
  IMAL_BLOCK_SET_PREFIX(ptr, IMAL_TAG_RELEASED);
  if (imal_mode_modify_fl == TRUE)
    {
    for (p = ((char*)ptr) + IMAL_BLOCK_USER_LEN(ptr);
         --p >= (char*)ptr;
         *p = (char)~(*p));
    }

  free(IMAL_BLOCK_BEGIN(ptr));
  return debug_err;
  } /* end of mem_free_fun function */


/* --------------------------------------------------------------------
 * NAME         mem_realloc_fun
 * PURPOSE      Replace C RTL realloc() function (resize block)
 * ARGUMENTS    IN VPTR ptr - Ptr to allocated block or NULL.
 *              IN SIZE_T new_size - Size of a new block.
 *              IN char *file - Source file name.
 *              IN int line   - Source line.
 *
 * RETURNS      Block pointer or NULL. IMAL error code will be set.
 * EXTERNS      None
 * NOTE         Function cannot be used for blocks of NO_CHECK class.
 ---------------------------------------------------------------------*/

#define TRACE_OUT(a, b, c, d, e, f, g, h, i, j)\
	if (imal_mode_trace_fl)\
		{\
		trace_out(a, b, c, d, e, f, g, h, i, j);\
		}


VPTR mem_realloc_fun(ptr, new_size, file, line)
IN VPTR ptr;
IN SIZE_T new_size;
IN char *file;
IN int line;
  {
  int		class_id;
  char		*name;
  VPTR		new_ptr;
  SIZE_T	name_size, size, block_size, new_block_size;
  CLASS_INFO	*pclass;
  BOOL		save_trace;

  /* Check that IMAL is initialized */
  if (!imal_init_fl)
    {
    imal_err(MEM_ERR_NOT_INIT);
    return NULL;
    }

  imal_err(MEM_OK);


  /* special cases */

  /* alloc() functionality */
  if (ptr == NULL)
    {
    if (new_size == 0)
      return NULL;

    save_trace = imal_mode_trace_fl;
    imal_mode_trace_fl = FALSE;

    name = imal_default_name(file, line);
    new_ptr = mem_alloc_fun(MEM_CLASS_DEFAULT, name, new_size, file, line);

    imal_mode_trace_fl = save_trace;

    TRACE_OUT('R', name, mem_query_class(MEM_CLASS_DEFAULT),
    			0, new_size, file, line, ptr, new_ptr, mem_error())

    return new_ptr;
    }

  /* some debug here */
  if (imal_mode_debug_fl)
    {
    /* !!! Program can be stopped in imal_debug()
     *     If damage has been found but program not stopped then
     *     error flag will be set and execution will be continued
     */
    imal_err(imal_debug(file, line));
    }

  /* get block info */
  class_id = MEM_NOT_CLASS;
  name = mem_query_block(ptr, &class_id);
  if (name == NULL || class_id == MEM_NOT_CLASS)
    {
    int		error;
    VPTR	damage_ptr;
    SIZE_T	damage_offs;

    /* Uh-oh, something gone wrong */

    damage_ptr = NULL;
    damage_offs = 0;

    switch (mem_check_block(ptr))
      {
      case MEM_ERR_BLOCK_DAMAGE:
        error = MEM_ERR_BLOCK_DAMAGE;
        damage_ptr = imal_damage_ptr;
        damage_offs = imal_damage_offset;
        break;

      case MEM_WARN_CLASS_NO_CHECK:
        error = MEM_ERR_CLASS_PREDEFINED;
        break;

      default:
        error = MEM_ERR_NOT_FOUND;
        break;
      }

    imal_err(error);
    TRACE_OUT('R', name, mem_query_class(class_id),
    		0, new_size, file, line, ptr, NULL, mem_error())
    /* !!! Program can be stopped in imal_error_proc() */
    imal_error_proc(mem_error(), "mem_realloc", file, line,
    					damage_ptr, damage_offs);

    return NULL;
    }

  /* get current block size */
  size = IMAL_BLOCK_USER_LEN(ptr);
  if (size == new_size)
    {
    TRACE_OUT('R', name, mem_query_class(class_id),
    		size, new_size, file, line, ptr, ptr, mem_error())
    return ptr;
    }

  /* get class info here, though it will be needed much later */
  pclass = imal_class_id_to_ptr(class_id);
  if (pclass == NULL)
    {
    imal_err(imal_class_check_id(class_id));
    TRACE_OUT('R', name, mem_query_class(class_id),
    		size, new_size, file, line, ptr, ptr, mem_error())
    return NULL;
    }

  /* free() functionality */
  if (new_size == 0)
    {
    char	*name_copy;

    /* make a copy of block name so we can trace it. Yuk! */
    name_copy = malloc(strlen(name) + 1);
    if (name_copy != NULL)
      strcpy(name_copy, name);

    save_trace = imal_mode_trace_fl;
    imal_mode_trace_fl = FALSE;

    imal_err(mem_free_fun(ptr, file, line));

    imal_mode_trace_fl = save_trace;

    TRACE_OUT('R', name_copy, mem_query_class(class_id),
    		size, 0, file, line, ptr, NULL, mem_error())

    if (name_copy != NULL)
      free(name_copy);

    return NULL;
    }


  /* at last, now we can get some work done */

  name_size = strlen(name);
  new_block_size = IMAL_BLOCK_SIZE(new_size, name_size);
  new_ptr = realloc(IMAL_BLOCK_BEGIN(ptr), new_block_size);
  if (new_ptr == NULL)
    {
    imal_err(MEM_ERR_NOT_MEMORY);
    TRACE_OUT('R', name, mem_query_class(class_id),
    		size, new_size, file, line, ptr, new_ptr, mem_error())
    return NULL;
    }

  /* realloc() could move block to another memory area. */
  /* Then 'name' becomes invalid. Restore it.           */
  /* :Ose '00.02.02,   :Eed '00.02.10                   */
  name = IMAL_BLOCK_SYS_TO_NAME(new_ptr);

  /* adjust control info */

  new_ptr = IMAL_BLOCK_SYS_TO_USER(new_ptr, name_size);
  IMAL_BLOCK_USER_LEN(new_ptr) = new_size;
  IMAL_BLOCK_SET_POSTFIX(new_ptr, IMAL_TAG_POSTFIX);
  IMAL_BLOCK_PTR(new_ptr)->next->prev = IMAL_BLOCK_PTR(new_ptr);
  IMAL_BLOCK_PTR(new_ptr)->prev->next = IMAL_BLOCK_PTR(new_ptr);
  if (pclass->block == ptr)
    pclass->block = new_ptr;

  block_size = IMAL_BLOCK_SIZE(size, name_size);

  total_blocks_size -= size;
  total_blocks_size += new_size;
  if (total_blocks_size > max_blocks_size)
    max_blocks_size = total_blocks_size;

  total_csize -= block_size - size;
  total_csize += new_block_size - new_size;
  if (total_csize > max_csize)
    max_csize = total_csize;

  pclass->block_size -= size;
  pclass->block_size += new_size;
  if (pclass->block_size > pclass->max_block_size)
    pclass->max_block_size = pclass->block_size;

  pclass->block_csize -= block_size - size;
  pclass->block_csize += new_block_size - new_size;
  if (pclass->block_csize > pclass->max_block_csize)
      pclass->max_block_csize = pclass->block_csize;


  TRACE_OUT('R', name, mem_query_class(class_id),
  		size, new_size, file, line, ptr, new_ptr, mem_error())

  return new_ptr;
  }


#undef TRACE_OUT





/* --------------------------------------------------------------------
 * NAME         mem_check_block
 * PURPOSE      Check the items of allocated block that it
 *              are not corrupted (all blocks if ptr is NULL).
 * ARGUMENTS    IN VPTR ptr - Allocated block pointer or NULL.
 *
 * RETURNS      IMAL error code:
 *              MEM_OK, MEM_ERR_NOT_FOUND,
 *              MEM_ERR_RELEASED, MEM_WARN_CLASS_NO_CHECK,
 *              MEM_ERR_BLOCK_DAMAGE, MEM_ERR_FATAL.
 *              imal_damage_ptr/imal_damage_offset  will be set
 *              to corrupted data address or NULL.
 * EXTERNS      None
 * NOTE         None
 ---------------------------------------------------------------------*/

int mem_check_block(passed_ptr)
IN VPTR passed_ptr;
  {
  VPTR ptr;            /* Pointer to the checked blocks      */
  int checking_result; /* Block checking result (combination */
                       /* of CHECK_BAD_* values).            */
  DOUBLE_PTR *list;    /* Block's list fields pointer        */
  char *name;          /* Block's name                       */
  SIZE_T block_cnt;    /* Counter of the allocated blocks    */
  BOOL single_fl;      /* checking single block / all blocks */

  if (passed_ptr != NULL)  /* Single block checking */
    {
    single_fl = TRUE;
    list = IMAL_BLOCK_PTR(passed_ptr);
    }
  else  /* Check all allocated blocks */
    {
    single_fl = FALSE;
    list = block_head.next;
    }
  block_cnt = 0;

  while (list != &block_head)
    {
    ptr = IMAL_BLOCK_LIST_TO_USER(list);

    /* Set default values of imal_damage_ptr/imal_damage_offset */
    imal_damage_ptr = NULL;

    /* Check prefix / postfix / list pointers                 */
    /* Put result of checking to the variable checking_result */

    checking_result = 0;

    if (!IMAL_ALIGNED(ptr))
      return single_fl == TRUE ? MEM_ERR_NOT_FOUND : MEM_ERR_FATAL;

    /* Check block's prefix */
    if (!IMAL_BLOCK_EQUAL_PREFIX(ptr, IMAL_TAG_PREFIX))
      {
      /* Check is block of NO_CHECK class */
      if (IMAL_BLOCK_EQUAL_PREFIX(ptr, IMAL_TAG_NO_CHECK))
        {
        return MEM_WARN_CLASS_NO_CHECK;
        }
      /* Check is block released */
      if (IMAL_BLOCK_EQUAL_PREFIX(ptr, IMAL_TAG_RELEASED))
        {
        return MEM_ERR_RELEASED;
        }
      checking_result |= CHECK_BAD_PREFIX;
      }
    else /* Prefix OK. Check block's postfix */
      {
      if ((VPTR)(IMAL_BLOCK_POSTFIX_ADR(ptr)) <= ptr)
        {
        checking_result |= CHECK_BAD_SIZE;  /* Block's size is creasy */
        }
      else  /* Block size is seems like correct - check postfix */
        {
        if (!IMAL_BLOCK_EQUAL_POSTFIX(ptr, IMAL_TAG_POSTFIX))
          {
          checking_result |= CHECK_BAD_POSTFIX;
          }
        }
      }

    /* Check previous block pointer and previous block */
    if (single_fl)
      {
      if (list->prev->next != list)
        checking_result |= CHECK_BAD_PREV_BLOCK;
      }
    /* Check next block pointer and next block */
    if (list->next->prev != list)
      checking_result |= CHECK_BAD_NEXT_BLOCK;

    /* checking_result contains the result of block checking */
    /* checking_result == 0 means O.K.                       */

    /* If something corrupted then find exast reason */
    if (checking_result != 0)
      {
      return imal_check_block_ptr(ptr, checking_result);
      /* NOTE: imal_check_block_ptr() set the variables */
      /*       imal_damage_ptr and imal_damage_offset   */
      }

    /* Check class identifier */
    if (imal_class_id_to_ptr(IMAL_BLOCK_CLASS_ID(ptr)) == NULL)
      {
      return MEM_ERR_FATAL;  /* Class id. or class data corrupted */
      }

    /* Check block's name length */
    name = IMAL_BLOCK_NAME(ptr);
    if (name >= (char*)ptr ||
       ((char*)IMAL_BLOCK_INFO(ptr)) - name > ALIGN + MAX_IMAL_NAME_LEN)
      {
      /* Creasy name length */
      imal_damage_ptr = ptr;
      imal_damage_offset = ((char*) &IMAL_BLOCK_NAME_LEN(ptr)) -
                           ((char*) ptr);
      return MEM_ERR_BLOCK_DAMAGE;
      }

    /* Check block's name */
    if ((int)strlen(name) != IMAL_BLOCK_NAME_LEN(ptr))
      {
      /* Name length differ from value in control part */
      imal_damage_ptr = ptr;
      imal_damage_offset = name + strlen(name) - ((char*) ptr);
      return MEM_ERR_BLOCK_DAMAGE;
      }

    if (single_fl)  /* Single block checking */
      return MEM_OK;

    if (++block_cnt > imal_blocks_number)
      return MEM_ERR_FATAL;      /* Wrong number of blocks in list */

    list = list->next;
    } /* End while */

  if (block_cnt != imal_blocks_number)
    return MEM_ERR_FATAL;      /* Wrong number of blocks in list */
  return MEM_OK;
  } /* end of mem_check_block function */

/* --------------------------------------------------------------------
 * NAME         mem_block
 * PURPOSE      Is ptr a pointer to allocated block or not?
 * ARGUMENTS    IN VPTR ptr - Some pointer
 * RETURNS      MEM_OK - ptr is allocated block pointer.
 *              MEM_WARN_CLASS_NO_CHECK - ptr is  pointer  to the
 *              block of class MEM_CLASS_NO_CHECK.
 *              MEM_ERR_NOT_FOUND -  ptr  is  not allocated block
 *              pointer.
 * EXTERNS      None
 * NOTE         If allocated  block  is  corrupted  result can be
 *              MEM_ERR_NOT_FOUND.    Exact    reason   can    be
 *              established  by  means  of  mem_check_block  this
 *              case.
 *              This function  is  used  by  functions  of module
 *              "rtl" (via  mem_block_size). Therefore it  should
 *              be fast.
 *              if  flag  imal_tag_check_fl  is   set   to  FALSE
 *              function   does   not   check   allocated   block
 *              prefix/postfix.
 ---------------------------------------------------------------------*/

int mem_block(ptr)
IN VPTR ptr;   /* Some pointer */
  {

  if (ptr == NULL || !IMAL_ALIGNED(ptr))
    return MEM_ERR_NOT_FOUND;
  if (imal_tag_check_fl == TRUE)
    {
    if (!IMAL_BLOCK_EQUAL_PREFIX(ptr, IMAL_TAG_PREFIX))
      {
      return IMAL_BLOCK_EQUAL_PREFIX(ptr, IMAL_TAG_NO_CHECK) ?
             MEM_WARN_CLASS_NO_CHECK : MEM_ERR_NOT_FOUND;
      }
    }
  return MEM_OK;
  } /* end of mem_block function */


/* --------------------------------------------------------------------
 * NAME         mem_empty
 * PURPOSE      Check that block list is empty.
 * ARGUMENTS    None
 * RETURNS      MEM_OK / MEM_ERR_NOT_EMPTY.
 * EXTERNS      None
 * NOTE         None
 ---------------------------------------------------------------------*/

int mem_empty()
  {
  return (block_head.next == &block_head) ? MEM_OK : MEM_ERR_NOT_EMPTY;
  } /* end of mem_empty function */


/* --------------------------------------------------------------------
 * NAME         mem_print_list
 * PURPOSE      Print map of allocated memory to stream.
 * ARGUMENTS    INOUT FILE *stream - Opened stream
 * RETURNS      MEM_OK / MEM_ERR_IO / MEM_ERR_NULL_PTR
 *              Result of block  list checking will be set by the
 *              function  imal_err().  User has  access  to  this
 *              value via mem_error().
 *              MEM_OK - list is O.K.
 *              MEM_ERR_BLOCK_DAMAGE - there are damaged blocks.
 *              MEM_ERR_FATAL - block list is corrupted.
 * EXTERNS      None
 * NOTE         Stream should be a pointer to the file opened for
 *              writing by os_fopen function (os_fopen(file_name,
 *              "w")).
 ---------------------------------------------------------------------*/

int mem_print_list(stream)
INOUT FILE *stream;          /* Opened stream */
  {
  VPTR pblock;        /* Current block                                */
  int err;            /* IMAL error code                              */
  SIZE_T block_cnt;   /* Counter of allocated blocks            */
  int class_id;       /* Class identifier of the current block        */
  char *block_name;   /* Name of the current block                    */
  char *class_name;   /* Name of the current block's class            */
  SIZE_T size;        /* Size of the current block                    */
  SIZE_T csize;       /* Size of the current block's control part     */
  SIZE_T no_check_num; /* number of blocks of NO_CHECK class   */
  char *err_sign;     /* Error message prefix                         */
  char *err_list;     /* Error message about block's list damage      */
  char *err_unknown;  /* Used instead names of damaged blocks/classes */

  imal_err(MEM_OK);  /* No memory errors found */
  if (stream == NULL)
    return MEM_ERR_NULL_PTR;
  err_sign = "??? ";
  err_list = "Allocated block's list corrupted: ";
  err_unknown = "UNKNOWN";
  no_check_num = imal_class_id_to_ptr(MEM_CLASS_NO_CHECK)->block_num;
  if (fprintf(stream,
        "IMAL: There are %Id ordinary allocated blocks now\n",
        imal_blocks_number) <= 0 ||
      (no_check_num > 0 &&
       fprintf(stream,
         "      There are %Id allocated blocks of NO_CHECK class now\n",
         no_check_num) <= 0))
    {
    imal_err(MEM_ERR_IO);
    return MEM_ERR_IO;
    }
  /* FOR all allocated blocks */
  pblock = NULL;
  for (block_cnt = 0; block_cnt < imal_blocks_number; block_cnt++)
    {
    /* Get next block's ptr */
    imal_tag_check_fl = FALSE;  /* Allow to get info from bad block */
    err = mem_block_next(&pblock, MEM_NOT_CLASS);
    imal_tag_check_fl = TRUE;   /* Restore default value */

    /* Check that not end of list */
    if (pblock == NULL && err == MEM_OK)
      {
      if (fprintf(stream, "%s%sUnexpected end of block's list\n",
                  err_sign,
                  err_list) <= 0)
        {
        imal_err(MEM_ERR_IO);
        return MEM_ERR_IO;
        }
      break;
      }

    /* Check the current block */
    if (pblock != NULL)
      err = mem_check_block(pblock);

    /* Set error code and print info about corrupted block */
    if (err != MEM_OK)
      {
      imal_err(err);
      /* Print message if THIS (not previous/next) block is corrupted */
      if (imal_damage_ptr == NULL || imal_damage_ptr == pblock)
        {
        if (fprintf(stream, "%sBad block. Error name:%s\n",
                    err_sign,
                    mem_strerr(err)) <= 0)
          {
          imal_err(MEM_ERR_IO);
          return MEM_ERR_IO;
          }
        if (imal_damage_ptr != NULL)
          {
          imal_print_damaged_block(stream,
                                   imal_damage_ptr,
                                   imal_damage_offset);
          } /* end if */
        } /* end if (print message) */
      } /* end if (err != MEM_OK) */

    /* Get info about block */
    imal_tag_check_fl = FALSE;  /* Allow to get info from bad block */
    block_name = mem_query_block(pblock, &class_id);
    if (block_name != NULL)
      {
      class_name = mem_query_class(class_id);
      if (class_name == NULL)
        class_name = err_unknown;
      }
    else
      {
      block_name = err_unknown;
      class_name = err_unknown;
      }
    mem_block_size(pblock, MEM_INFO_CURRENT, &size, &csize);
    imal_tag_check_fl = TRUE;   /* Restore default value */

    /* Print info about block */
    if (fprintf(stream, "CL:") <= 0 ||
        print_name(stream, class_name, N_BLOCK_CLASS) == FAILURE ||
        fprintf(stream, " NAM:") <= 0 ||
        print_name(stream, block_name, N_BLOCK_NAME) == FAILURE ||
        fprintf(stream, " SIZ:%6Id  ADR:%p\n", size, pblock) <= 0)
      {
      imal_err(MEM_ERR_IO);
      return MEM_ERR_IO;
      }
    if (pblock == NULL) break;  /* Unexpected end of list */
    } /* end for */

  /* Check that end of list */
  if (pblock != NULL)
    mem_block_next(&pblock, MEM_NOT_CLASS);
  if (pblock != NULL)
    {
    imal_err(MEM_ERR_FATAL);
    if (fprintf(stream, "%s%sEnd of block's list expected\n",
                err_sign,
                err_list) <= 0)
      {
      imal_err(MEM_ERR_IO);
      return MEM_ERR_IO;
      }
    }

  /* Print total info */

  if (fprintf(stream,
      "  Current: Total size: %Id  Control structures: %Id\n",
      total_blocks_size, total_csize) <= 0 ||
      fprintf(stream,
      "  Maximum: Total size: %Id  Control structures: %Id\n",
      max_blocks_size, max_csize) <= 0)
    {
    imal_err(MEM_ERR_IO);
    return MEM_ERR_IO;
    }
  return MEM_OK;
  } /* end of mem_print_list function */

/* --------------------------------------------------------------------
 * NAME         mem_print_class
 * PURPOSE      Print  class  information  to   the   stream.  If
 *              class_id is  MEM_NOT_CLASS  print  info about all
 *              classes.
 * ARGUMENTS    INOUT FILE *stream - Opened stream
 *              IN int class_id - Class ident or MEM_NOT_CLASS
 * RETURNS      MEM_OK / MEM_ERR_IO / MEM_ERR_NULL_PTR
 *              Result of class  block  list checking will be set
 *              by the  function  imal_err().  User has access to
 *              this value via mem_error().
 *              MEM_OK - list is O.K.
 *              MEM_ERR_BLOCK_DAMAGE - there are damaged blocks.
 *              MEM_ERR_FATAL - block list is corrupted.
 *              MEM_ERR_CLASS_PREDEFINED - class "NO_CHECK".
 * EXTERNS      None
 * NOTE         Stream should be a pointer to the file opened for
 *              writing by os_fopen function (os_fopen(file_name,
 *              "w")).
 ---------------------------------------------------------------------*/

int mem_print_class(stream, class_id)
INOUT FILE *stream;    /* Opened stream */
IN int class_id;       /* Class ident or MEM_NOT_CLASS */
  {
  int err;             /* IMAL error code                      */
  int output_err;      /* Output result                        */
  int cur_class_id;    /* Class identifier                     */
  SIZE_T block_num;    /* Number of blocks of class_id class   */
  SIZE_T cur_cl_size;  /* Size of blocks of class_id class     */
  SIZE_T cur_cl_csize; /* Total control part size for class_id */
  SIZE_T max_cl_size;  /* Size of greatest block for class_id  */
  SIZE_T max_cl_csize; /* Its control part size                */
  SIZE_T max_num;      /* Max. number of blocks for class_id   */
  char *class_name;    /* Name of the class class_id           */
  char *err_sign;      /* Error message prefix                 */

  if (stream == NULL)
    {
    imal_err(MEM_OK); /* No memory errors found */
    return MEM_ERR_NULL_PTR;
    }

  /* CASE 'info about all classes' */

  if (class_id == MEM_NOT_CLASS)
    {
    output_err = MEM_OK;
    err = MEM_OK;
    cur_class_id = MEM_NOT_CLASS;
    while ((cur_class_id = imal_class_next(cur_class_id)) !=
            MEM_NOT_CLASS)
      {
      output_err = mem_print_class(stream, cur_class_id);
      if (err == MEM_OK)
        err = mem_error();
      if (output_err != MEM_OK)
        break;
      }
    imal_err(err);
    return output_err;
    }

  /* CASE 'info about single class' */

  err_sign = "??? Error: ";
  class_name = mem_query_class(class_id);
  if (fprintf(stream, "Cl: %s  Id: %d\n",
              (class_name != NULL ? class_name : "UNKNOWN"),
              class_id) <= 0)
    {
    imal_err(MEM_ERR_IO);
    return MEM_ERR_IO;
    }

  /* Get class info */
  if ((err = mem_class_size(class_id, MEM_INFO_CURRENT,
               &block_num, &cur_cl_size, &cur_cl_csize)) != MEM_OK ||
      (err = mem_class_size(class_id, MEM_INFO_MAX,
               &max_num, &max_cl_size, &max_cl_csize)) != MEM_OK)
    {
    imal_err(err);
    if (fprintf(stream, "%s%s\n\n", err_sign, mem_strerr(err)) <= 0)
      {
      imal_err(MEM_ERR_IO);
      return MEM_ERR_IO;
      }
    return MEM_OK;
    }

  /* If block's list is OK print class info */
  imal_err(MEM_OK);
  if (fprintf(stream,
      "Cur info: Blocks: %6Id  Mem: %8IdB  Ctrl parts: %7IdB\n",
      block_num, cur_cl_size, cur_cl_csize) <= 0 ||
      fprintf(stream,
      "Max info: Blocks: %6Id  Mem: %8IdB  Ctrl parts: %7IdB\n",
      max_num, max_cl_size, max_cl_csize) <= 0)
    {
    imal_err(MEM_ERR_IO);
    return MEM_ERR_IO;
    }
  return MEM_OK;
  } /* end of mem_print_class function */


/* --------------------------------------------------------------------
 * NAME         mem_block_size
 * PURPOSE      Return  block  size / size of the  all  blocks  /
 *              maximum block size.
 * ARGUMENTS    IN VPTR ptr - Block pointer or NULL
 *              IN int mode - MEM_INFO_CURRENT or MEM_INFO_MAX
 *              OUT SIZE_T *blk_size - Return: block size
 *              OUT SIZE_T *cblk_size - Return: control block size
 *
 * RETURNS      MEM_OK  or  MEM_ERR_NOT_FOUND  if   ptr   is  not
 *              allocated block pointer (or block is  corrupted).
 *              In the  case  of  MEM_NOT_FOUND  exact reason via
 *              mem_check_block().
 *              MEM_ERR_NULL_PTR if blk_size or cblk_size is NULL
 *              MEM_ERR_MODE_BAD if mode parameter is wrong
 *
 * EXTERNS      None
 * NOTE         This function  is  used  by  functions  of module
 *              "rtl". Therefore it should be fast.
 *              This  function  cannot  be  used  for  the  class
 *              MEM_CLASS_NO_CHECK.      Result      will      be
 *              MEM_ERR_NOT_FOUND.
 *              if  flag  imal_tag_check_fl  is   set   to  FALSE
 *              function   does   not   check   allocated   block
 *              prefix/postfix.
 ---------------------------------------------------------------------*/

int mem_block_size(ptr, mode, blk_size, cblk_size)
IN VPTR ptr;            /* Block pointer or NULL      */
IN int mode;            /* MEM_INFO_CURRENT or MEM_INFO_MAX */
OUT SIZE_T *blk_size;   /* Return: block size         */
OUT SIZE_T *cblk_size;  /* Return: control block size */
/*
  MEM_INFO_CURRENT means "give info about *ptr".
  MEM_INFO_MAX means  "give  info  about  maximum  block during run
  time of the program".
*/
  {

  if (blk_size == NULL || cblk_size == NULL)
    return MEM_ERR_NULL_PTR;

  /* CASE Single block size */

  if (mode == MEM_INFO_CURRENT)
    {
    if (ptr != NULL)
      {
      if (mem_block(ptr) != MEM_OK)   /* ptr is wrong or block corrupted */
        {
        *blk_size = 0;
        *cblk_size = 0;
        return MEM_ERR_NOT_FOUND;
        }
      *blk_size = IMAL_BLOCK_USER_LEN(ptr);
      *cblk_size = IMAL_BLOCK_CONTROL_SIZE(ptr);
      return MEM_OK;
      }

    /* CASE  Total size of the all allocated blocks */

    *blk_size = total_blocks_size;
    *cblk_size = total_csize;
    return MEM_OK;
    } /* end of IF MODE_CURRENT */

  /* CASE Maximum size */

  if (mode == MEM_INFO_MAX)
    {
    *blk_size = max_blocks_size;
    *cblk_size = max_csize;
    return MEM_OK;
    }

  /* Wrong mode parameter */
  return MEM_ERR_MODE_BAD;
  } /* end of mem_block_size function */


/* --------------------------------------------------------------------
 * NAME         mem_class_size
 * PURPOSE      Return block(s) size.
 * ARGUMENTS    IN int class_id - Class identifier
 *              IN int mode - MEM_INFO_CURRENT or MEM_INFO_MAX
 *              OUT SIZE_T *blk_num - blocks number in class
 *              OUT SIZE_T *blk_size - total blocks' size
 *              OUT SIZE_T *blk_size - total control part size
 *
 * RETURNS      MEM_OK / MEM_ERR_CLASS_NOT_CREATED / MEM_ERR_FATAL /
 *              MEM_ERR_BLOCK_DAMAGE / MEM_ERR_NULL_PTR.
 *              MEM_ERR_MODE_BAD if mode parameter is wrong
 *
 * EXTERNS      None
 * NOTE         For the  class MEM_CLASS_NO_CHECK blk_size/cblk_size
 *              will be set to 0.
 ---------------------------------------------------------------------*/

int mem_class_size(class_id, mode, blk_num, blk_size, cblk_size)
IN int class_id;        /* Class identifier */
IN int mode;            /* MEM_INFO_CURRENT or MEM_INFO_MAX */
OUT SIZE_T *blk_num;    /* blocks number in class */
OUT SIZE_T *blk_size;   /* total blocks' size     */
OUT SIZE_T *cblk_size;  /* total control part size*/
/*
  MEM_INFO_CURRENT means "give current info".
  MEM_INFO_MAX means "give info about maximum block of this class /
    maximum blocks number during run time of the program".
*/
  {
  CLASS_INFO *pclass;  /* Class descriptor pointer */

  if (blk_num == NULL || blk_size == NULL || cblk_size == NULL)
    return MEM_ERR_NULL_PTR;

  *blk_num = 0;
  *blk_size = 0;
  *cblk_size = 0;
  if ((pclass = imal_class_id_to_ptr(class_id)) == NULL)
    return imal_class_check_id(class_id);

  /* CASE Get info about current blocks */

  if (mode == MEM_INFO_CURRENT)
    {
    *blk_num = pclass->block_num;
    *blk_size = pclass->block_size;
    *cblk_size = pclass->block_csize;
    return MEM_OK;
    }

  /* CASE Get info about maximum memory occupied by the class */

  if (mode == MEM_INFO_MAX)
    {
    *blk_num = pclass->max_block_num;
    *blk_size = pclass->max_block_size;
    *cblk_size = pclass->max_block_csize;
    return MEM_OK;
    }

  /* Wrong mode parameter */
  return MEM_ERR_MODE_BAD;
  } /* end of mem_class_size function */


/* --------------------------------------------------------------------
 * NAME         mem_query_block
 * PURPOSE      Get name and class descriptor of the block.
 * ARGUMENTS    IN VPTR pt - allocated block pointer
 *              OUT int *p_class_id - class identifier
 * RETURNS      Name  or  NULL  (if  ptr is not  allocated  block
 *              pointer).
 * EXTERNS      None
 * NOTE         For the class MEM_CLASS_NO_CHECK result will be
 *              NULL; *p_class_id will be MEM_CLASS_NO_CHECK.
 *              if  flag  imal_tag_check_fl  is   set   to  FALSE
 *              function   does   not   check   allocated   block
 *              prefix/postfix.
 ---------------------------------------------------------------------*/

char *mem_query_block(ptr, p_class_id)
IN VPTR ptr;            /* allocated block pointer */
OUT int *p_class_id;    /* class identifier */
  {
  char *name;  /* Name of the block */

  switch (mem_block(ptr))       /* Result of ptr checking */
    {
    case MEM_OK:
      if (p_class_id != NULL)
        *p_class_id = IMAL_BLOCK_CLASS_ID(ptr);
      name = IMAL_BLOCK_NAME(ptr);
      if (name >= (char*)ptr || name <
         ((char*)IMAL_BLOCK_INFO(ptr)) - ALIGN - MAX_IMAL_NAME_LEN)
        {
        return NULL;
        }
      return name;

    case MEM_WARN_CLASS_NO_CHECK:
      if (p_class_id != NULL)
        *p_class_id = MEM_CLASS_NO_CHECK;
      return NULL;

    default:            /* Bad ptr or block corrupted */
      if (p_class_id != NULL)
        *p_class_id = MEM_NOT_CLASS;
      return NULL;
    }
  } /* end of mem_query_block function */


/* --------------------------------------------------------------------
 * NAME         mem_block_next
 * PURPOSE      Get address  of  the first/next allocated block /
 *              block of the specified class.
 * ARGUMENTS    INOUT VPTR *pptr - Pointer to variable which contained
 *                                 allocated block pointer or NULL.
 *              IN int class_id - Class identifier or MEM_NOT_CLASS.
 * RETURNS      MEM_OK / MEM_ERR_CLASS_NOT_CREATED /
 *              MEM_ERR_NOT_FOUND / MEM_ERR_BLOCK_DAMAGE.
 * EXTERNS      None
 * NOTE         Function checks  only  current  block ptr but not
 *              the next one i.e. value returned in *pptr. This
 *              value can be wrong if current block is corrupted.
 *              But it is guaranteed that *pptr points to some data.
 *              Function   does   not   check   allocated   block
 *              prefix/postfix  if  flag   imal_tag_check_fl   is
 *              FALSE.
 ---------------------------------------------------------------------*/

int mem_block_next(pptr, class_id)
INOUT VPTR *pptr;  /* pointer to variable which contained */
                   /* allocated block pointer or NULL     */
IN int class_id;   /* class identifier or MEM_NOT_CLASS   */
  {
  VPTR ptr;            /* Allocated block pointer     */
  DOUBLE_PTR *list;    /* Block's list fields pointer */
  CLASS_INFO *pclass;  /* Class descriptor pointer    */

  if ((ptr = *pptr) != NULL)    /* Next (not first) block is required */
    {
    if (mem_block(ptr) != MEM_OK)       /* Current block ptr is bad */
      {
      *pptr = NULL;
      return mem_check_block(ptr);     /* Find what is wrong with ptr */
      }
    }
  /* ptr is the current block pointer or NULL */

  if (class_id == MEM_NOT_CLASS)        /* Next in block's list */
    {
    list = ((ptr == NULL) ? block_head.next : IMAL_BLOCK_PTR(ptr)->next);
    *pptr = ((list == &block_head) ? NULL : IMAL_BLOCK_LIST_TO_USER(list));
    }
  else  /* class_id is class identifier */
    {
    if ((pclass = imal_class_id_to_ptr(class_id)) == NULL)
      {
      *pptr = NULL;
      return imal_class_check_id(class_id);  /* Bad class identifier */
      }
    if (ptr == NULL)    /* Return first block of class class_id */
      {
      *pptr = pclass->block;
      }
    else                /* Return next block of class class_id */
      {
      if ((list = IMAL_BLOCK_PTR(ptr)->next) == &block_head)
        {
        *pptr = NULL;     /* End of block's list */
        return MEM_OK;
        }
      ptr = IMAL_BLOCK_LIST_TO_USER(list);
      /* Check the next block before reading class ident from it */
      if (mem_block(ptr) != MEM_OK)
        {
        *pptr = NULL;     /* Next block pointer is bad */
        return MEM_ERR_BLOCK_DAMAGE;
        }
      /* ptr is the next block pointer */
      *pptr = ((IMAL_BLOCK_CLASS_ID(ptr) == class_id) ? ptr : NULL);
      return MEM_OK;
      } /* End of else (ptr not NULL) */
    } /* End of else (class_id is identifier) */

  return MEM_OK;
  } /* end of mem_block_next function */


                        /* Interface functions */

/* --------------------------------------------------------------------
 * NAME         imal_mode_set
 * PURPOSE      Set IMAL mode flags in 'rtl' module.
 * ARGUMENTS    IN int debug_mode  - Debug mode value
 *              IN int trace_mode  - Warning mode value
 *              IN int modify_mode - Modify mode value
 * RETURNS      None
 * EXTERNS      None
 * NOTE         None
 ---------------------------------------------------------------------*/

VOID imal_mode_set(debug_mode, trace_mode, modify_mode)
IN int debug_mode;   /* Debug mode value    */
IN int trace_mode;   /* Warning mode value  */
IN int modify_mode;  /* Modify mode value   */
  {
  imal_mode_debug_fl = (BOOL)(debug_mode == (int)TRUE ? TRUE : FALSE);
  imal_mode_trace_fl = (BOOL)(trace_mode == (int)TRUE ? TRUE : FALSE);
  imal_mode_modify_fl = (BOOL)(modify_mode == (int)TRUE ? TRUE : FALSE);
  } /* end of imal_mode_set function */


/* --------------------------------------------------------------------
 * NAME         imal_add_csize
 * PURPOSE      Correct the total size of IMAL control structures.
 * ARGUMENTS    IN SIZE_T dcsize - Difference between the new and the
 *                              old size of IMAL control structures
 *                              (in bytes).
 * RETURNS      None
 * EXTERNS      None
 * NOTE         None
 ---------------------------------------------------------------------*/

VOID imal_add_csize(dcsize)
IN SIZE_T dcsize;
  {
  if ((total_csize += dcsize) > max_csize)
    max_csize = total_csize;
  } /* end of imal_add_csize function */


/* --------------------------------------------------------------------
 * NAME         imal_get_damage_info
 * PURPOSE      Get info about damaged block which has been set by
 *              mem_check_block().
 * ARGUMENTS    OUT VPTR* damage_ptr    - Corrupted block address.
 *              OUT SIZE_T* damage_offset - Offset of corrupted data
 *                                        from damage_ptr.
 * RETURNS      None.
 * EXTERNS      None.
 * NOTE         None.
 ---------------------------------------------------------------------*/

VOID imal_get_damage_info(damage_ptr, damage_offset)
OUT VPTR* damage_ptr;
OUT SIZE_T* damage_offset;
  {
  *damage_ptr = imal_damage_ptr;
  *damage_offset = imal_damage_offset;
  } /* end of imal_get_damage_info function */


/* --------------------------------------------------------------------
 * NAME         imal_get_corrupted_info
 * PURPOSE      Get info (name/class/size) from damaged block.
 * ARGUMENTS    IN VPTR ptr - Block pointer
                OUT char **name    - Block name
 *              OUT SIZE_T *size - Block size
 *              OUT int *class_id  - Class identifier
 * RETURNS      None.
 * EXTERNS      None.
 * NOTE         None.
 ---------------------------------------------------------------------*/

VOID imal_get_corrupted_info(ptr, name, size, class_id)
IN VPTR ptr;
OUT char **name;
OUT SIZE_T *size;
OUT int *class_id;
  {
  SIZE_T vsize, vcsize;

  imal_tag_check_fl = FALSE;  /* Allow to get info from bad block */
  *name = mem_query_block(ptr, class_id);
  if (mem_block_size(ptr, MEM_INFO_CURRENT, &vsize, &vcsize) != MEM_OK)
    vsize = 0;
  imal_tag_check_fl = TRUE;   /* Restore default value */
  *size = vsize;
  }


                       /* Auxiliary functions */


/* --------------------------------------------------------------------
 * NAME         trace_out
 * PURPOSE      Print trace info about mem_alloc/mem_calloc/mem_free
 *              to the trace file.
 *
 * ARGUMENTS    IN char type - 'A' or 'C' or 'F' - What function?
 *              IN char *block_name
 *              IN char *class_name
 *              IN SIZE_T item_size - Block's size (item size for calloc)
 *              IN SIZE_T item_number - This parameter for calloc only.
 *              IN char *file - Source file name.
 *              IN int line -   Source line number.
 *              IN VPTR ptr -   Block address.
 *              IN int err -    Error code.
 *
 * RETURNS      None
 * EXTERNS      None
 * NOTE         Trace records will be processed by memtrace program
 *              (sources in memtrace.c). If trace format will be changed
 *              memtrace.c should be corrected also.
 ---------------------------------------------------------------------*/
static VOID trace_out(type, block_name, class_name,
              item_size, item_number, file, line, ptr, ptr1, err)
IN int type;
IN char *block_name;
IN char *class_name;
IN SIZE_T item_size, item_number;
IN char *file;
IN int line;
IN VPTR ptr, ptr1;
IN int err;             /* IMAL error code */
  {

  /* If trace file not opened then open it */

  if (trace_file_ptr == NULL && trace_file_name[0] != '\0')
    {
    if ((trace_file_ptr =
          os_fopen(trace_file_name,
                   (trace_continue_fl == TRUE ? "a" : "w"))) == NULL)
      {
      /* Trace file open failure */
      fprintf(stderr, "IMAL: Cannot open trace file '%s'\n",
                      trace_file_name);
      trace_file_name[0] = '\0';  /* Trace file will be stderr */
      }
    trace_continue_fl = TRUE;   /* Trace file will be appended     */
    }                           /* (not rewritten) in this program */

  /* Cannot open trace file - print to stderr */

  if (trace_file_ptr == NULL)
    trace_file_ptr = stderr;

  /* Consider NULL/empty values of string parameters */
  if (block_name == NULL)
    block_name = "NULL";
  if (*block_name == '\0')
    block_name = "EMPTY";
  if (class_name == NULL)
    class_name = "NULL";
  if (*class_name == '\0')
    class_name = "EMPTY";
  if (file == NULL)
    file = "NULL";

  /* Print record to the trace file */

  switch ((char)type)
    {
    case 'A':
    case 'C':
    case 'F':
    case 'R':
      break;
    default:
      fprintf(stderr, "??? IMAL: Bad trace_out parameter\n");
      return;
    }

  if (fprintf(trace_file_ptr, "%c ", type) < 2 ||
      print_name(trace_file_ptr, class_name, 0) == FAILURE ||
      fprintf(trace_file_ptr, " ") <= 0 ||
      print_name(trace_file_ptr, block_name, 0) == FAILURE ||
      fprintf(trace_file_ptr, " %Iu", item_size) < 2 ||
      (((char)type == 'C' || (char)type == 'R') &&
        fprintf(trace_file_ptr, " %Iu", item_number) < 2) ||
      fprintf(trace_file_ptr, " %s %d %p", file, line, ptr) <= 0 ||
      ((char)type == 'R' &&
        fprintf(trace_file_ptr, " %p", ptr1) <= 0) ||
      fprintf(trace_file_ptr, " %d\n", err) <= 0 ||
      fflush(trace_file_ptr) == EOF)
    {
    /* Write failure: Close file, new trace file is stderr */
    if (trace_file_name[0] != '\0')
      {
      fprintf(stderr, "IMAL: Write failure in file '%s'\n",
                      trace_file_name);
      os_fclose(trace_file_ptr);
      trace_file_name[0] = '\0';  /* Trace file will be stderr */
      }
    if (trace_file_ptr != stderr)
      {
      trace_file_ptr = stderr;
      /* Repeat current record to the stderr */
      trace_out(type, block_name, class_name,
                item_size, item_number, file, line, ptr, ptr1, err);
      }
    }
  } /* end of trace_out function */


/*
* NAME       print_name
* PURPOSE    Print block/class name which can be corrupted to the stream
* ARGUMENTS  INOUT FILE *stream - Stream opened by os_fopen()
*            IN char *name - Block/class name.
*            IN int npos - Minimum number of positions.
* RETURN     SUCCESS on success.
*            FAILURE on write failure.
*/

static int print_name (stream, name, npos)
INOUT FILE *stream;
IN char *name;
IN int npos;
  {
  int i;

  if (name == NULL) name = "NULL";
  for (i = 0; *name != '\0' && i < MAX_IMAL_NAME_LEN; i++)
    {
    if (fprintf(stream, "%c", (isprint(*name) ? *name : '.')) <= 0)
      {
      return FAILURE;
      }
    name++;
    }

  while (i++ < npos)
    if (fprintf(stream, " ") <= 0)
      return FAILURE;

  return SUCCESS;
  } /* end of print_name function */


/* --------------------------------------------------------------------
 * NAME         imal_check_block_ptr
 * PURPOSE      Find what is wrong with allocated block pointer
 *              (block corrupted or next/previous block corrupted or
 *              wrong pointer).
 *              Set imal_damage_* variables.
 *
 * ARGUMENTS    IN VPTR ptr - Allocated block pointer.
 *              IN int checking_result - Result of block's items
 *              checking. It is combination of the values:
 *              CHECK_BAD_PREV_PTR, CHECK_BAD_PREV_BLOCK
 *              CHECK_BAD_NEXT_PTR, CHECK_BAD_NEXT_BLOCK
 *              CHECK_BAD_PREFIX, CHECK_BAD_POSTFIX, CHECK_BAD_SIZE.
 *
 * RETURNS      MEM_OK or  error  code:  MEM_ERR_NOT_FOUND (not a
 *              pointer to allocated block), MEM_ERR_BLOCK_DAMAGE
 *              (block   damage   is   found),   MEM_ERR_RELEASED
 *              (pointer     to     the      released      block)
 *              MEM_WARN_CLASS_NO_CHECK (pointer to  the block of
 *              class MEM_CLASS_NO_CHECK) or MEM_ERR_FATAL (block
 *              list      corrupted).
 *              Either variables imal_damage_ptr/imal_damage_offset
 *              contains the address of the damaged data
 *              or imal_damage_ptr is NULL.
 *
 * EXTERNS      None
 * NOTE         None
 ---------------------------------------------------------------------*/

static int imal_check_block_ptr(ptr, checking_result)
IN VPTR ptr;
IN int checking_result;
  {
  DOUBLE_PTR *list;
  SIZE_T block_cnt;

  /* Analyse the value of checking_result */
  if (checking_result == 0)     /* Everything OK with block */
    return MEM_OK;

  /* Something is wrong - try to find exact reason */

  imal_damage_ptr = ptr;
  if ((checking_result &
       (CHECK_BAD_PREV_PTR | CHECK_BAD_PREV_BLOCK |
        CHECK_BAD_NEXT_PTR | CHECK_BAD_NEXT_BLOCK)) == 0)
    {
    /* Pointers O.K.; prefix/postfix corrupted */
    if (checking_result & CHECK_BAD_POSTFIX)
      {
      imal_damage_offset =
        ((char*) IMAL_BLOCK_POSTFIX_ADR(ptr)) - ((char*)ptr);
      if (IMAL_BLOCK_POSTFIX_ADR(ptr)[0] == (char)IMAL_TAG_POSTFIX)
        imal_damage_offset++;
      }
    if (checking_result & CHECK_BAD_SIZE)
      {
      imal_damage_offset =
        ((char*) &IMAL_BLOCK_USER_LEN(ptr)) - ((char*)ptr);
      }
    if (checking_result & CHECK_BAD_PREFIX)
      {
      imal_damage_offset =
        ((char*) IMAL_BLOCK_PREFIX_ADR(ptr)) - ((char*)ptr);
      if (IMAL_BLOCK_PREFIX_ADR(ptr)[0] == (char)IMAL_TAG_PREFIX)
        imal_damage_offset++;
      }
    return MEM_ERR_BLOCK_DAMAGE;
    }

  list = IMAL_BLOCK_PTR(ptr);
  if (checking_result == CHECK_BAD_PREV_BLOCK)
    {
    /* Previous block corrupted; This block is OK */
    imal_damage_ptr = IMAL_BLOCK_LIST_TO_USER(list->prev);
    imal_damage_offset = ((char*) &(list->prev->next)) -
                         ((char*) imal_damage_ptr);
    return MEM_ERR_BLOCK_DAMAGE;
    }

  if (checking_result == CHECK_BAD_NEXT_BLOCK)
    {
    /* Next block corrupted; This block is OK */
    imal_damage_ptr = IMAL_BLOCK_LIST_TO_USER(list->next);
    imal_damage_offset = ((char*) &(list->next->prev)) -
                         ((char*) imal_damage_ptr);
    return MEM_ERR_BLOCK_DAMAGE;
    }

  if ((checking_result & CHECK_BAD_PREFIX) == 0)
    {
    /* Prefix OK; At least one of list pointers is corrupted. */
    imal_damage_offset = ((char*) list) - ((char*)ptr);
    return MEM_ERR_BLOCK_DAMAGE;
    }

  /* ASSERT: Prefix and list pointers are corrupted both           */
  /*   There are two possibilities now:                            */
  /*     1) ptr is not allocated block pointer (MEM_ERR_NOT_FOUND) */
  /*     2) block *ptr is fully corrupted (MEM_ERR_BLOCK_DAMAGE)   */
  /*   If block list is O.K. then 1) take place.                   */
  /*   If not then probably 2).                                    */

  list = &block_head;
  /* Check allocated blocks list */
  for (block_cnt = imal_blocks_number; block_cnt > 0; block_cnt--)
    {
    if (list->next == IMAL_BLOCK_PTR(ptr))
      {
      /* We find checked block in allocated blocks list */
      imal_damage_offset = ((char*) IMAL_BLOCK_PREFIX_ADR(ptr)) -
                           ((char*) ptr);
      return MEM_ERR_BLOCK_DAMAGE;
      }
    if (!IMAL_ALIGNED(list->next) || list->next == NULL)
      {
      /* Current block corrupted */
      imal_damage_ptr = IMAL_BLOCK_LIST_TO_USER(list);
      imal_damage_offset = ((char*) &(list->next)) -
                           ((char*) imal_damage_ptr);
      return MEM_ERR_BLOCK_DAMAGE;
      }
    if (list->next->prev != list)
      {
      /* Probably next block corrupted */
      imal_damage_ptr = IMAL_BLOCK_LIST_TO_USER(list->next);
      imal_damage_offset = ((char*) &(list->next->prev)) -
                           ((char*) imal_damage_ptr);
      return MEM_ERR_BLOCK_DAMAGE;
      }
    list = list->next;
    }

  imal_damage_ptr = NULL;
  if (list->next == &block_head)  /* Block'c list is O.K. */
    {
    return MEM_ERR_NOT_FOUND;     /* ptr is not block pointer */
    }
  else
    {
    return MEM_ERR_FATAL;   /* Block'c list corrupted */
    }

  } /* end of imal_check_block_ptr function */


/*** End of file IMAL.C ***/
