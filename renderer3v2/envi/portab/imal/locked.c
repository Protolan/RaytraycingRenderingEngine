/*
 * MODULE NAME  LOCKED.C
 * PURPOSE      Module contains data and functions which support
 *              locked (read-only) buffers in IMAL library.
 *
 * SPEC         V. L. Volevich                        1993.01.12
 * DESIGN       V. L. Volevich                        1993.06.03
 * DES. INSP.   Z. Blaszczyk                          1993.06.28
 * CODING       V. L. Volevich                        1993.08.02
 * CODE INSP.   Z. Blaszczyk                          1993.08.??
 * TEST         V. L. Volevich                        1993.08.03
 *
 * LAST UPDATED V. L. Volevich                        1993.08.11
 *
 */

/* COMPANY INCLUDES */

#define IMAL_NO_C_RTL_MACRO  /* Turn off macros for C RTL functions */

#include "integra.h"
#include "imal.h"


/* PROJECT INCLUDE: */

#include "imal_loc.h"

/* TYPEDEFS */

typedef struct locked_buf_ref   /* Locked buffer reference block. */
  {
  short tag;    /* This tag is the same for  any structure of */
                /* this type (IMAL_TAG_LOCKED).  Tag allow to */
                /* differ such struct from other data.        */
  struct locked_buf_ref *next;  /* Address of the next item   */
                                /* or NULL                    */
  VPTR buf;                     /* Locked buffer address      */
  SIZE_T length;                /* Length of locked buffer    */
  long check_sum;        /* Check sum of the buffer contains  */
  } LOCKED_BUF_REF;


typedef union long_char   /* Type used for check sum calculation */
  {
  long val;
  char pval[sizeof(long)];
  } LONG_CHAR;

/*-------------------------------------------------------------/
                   Internal data structures.

Internal data of module "locked"  consist  of  locked buffer list
and auxiliary variables. Each  item  of locked buffer list placed
in its own item allocated by malloc(). Item contains reference to
the lockd buffer and its control sum.

+------+      +-----+      +-----+              +-----+
|locked|----->|item1|----->|item2|-- .....  --->|itemN|--->NULL
| head |      +-----+      +-----+              +-----+
+------+

/-------------------------------------------------------------*/

/****************************** DATA *************************************/

static LOCKED_BUF_REF *locked_buf_head = NULL;

/* Header of the one way list of references to locked buffers. */

static int check_errno;
static VPTR check_damage_ptr;

/* Result of the locked buffer list checking */


static LOCKED_BUF_REF **imal_check_locked_ptr(IN VPTR);
static long check_sum(IN VPTR, IN SIZE_T);

/* Disable "cast truncates constant value" warning */
#pragma warning(disable : 4310)

/****************************** CODE *************************************/


                        /* Library functions */

/* --------------------------------------------------------------------
 * NAME         mem_static_lock
 * PURPOSE      Mark variable/array as 'read-only' (locked).
 * ARGUMENTS    IN VPTR buf - Pointer to the buffer.
 *              IN SIZE_T length - Length of the buffer.
 *
 * RETURNS      Error code:
 *              MEM_OK - O.K.
 *              MEM_ERR_NULL_PTR - buf is NULL.
 *              MEM_ERR_BAD_SIZE - length is 0.
 *              MEM_ERR_NOT_MEMORY - Not enough memory for internal data
 * EXTERNS      None
 * NOTE         None
 ---------------------------------------------------------------------*/

int mem_static_lock(buf, length)
IN VPTR buf;
IN SIZE_T length;
  {
  LOCKED_BUF_REF *locked_ref;

  if (buf == NULL)
    return MEM_ERR_NULL_PTR;
  if (length == 0)
    return MEM_ERR_BAD_SIZE;

  /* Allocate a locked reference block */
  if ((locked_ref =
         (LOCKED_BUF_REF*)malloc(sizeof(LOCKED_BUF_REF))) == NULL)
    {
    /* Not enough memory for the internal data */
    return MEM_ERR_NOT_MEMORY;
    }
  imal_add_csize(sizeof(LOCKED_BUF_REF));

  /* Initialize locked reference block and insert it to the list */
  locked_ref->tag = IMAL_TAG_LOCKED;
  locked_ref->next = locked_buf_head;
  locked_ref->buf = buf;
  locked_ref->length = length;
  locked_ref->check_sum = check_sum(buf, length);
  locked_buf_head = locked_ref;
  return MEM_OK;
  } /* end of mem_static_lock function */


/* --------------------------------------------------------------------
 * NAME         mem_static_release
 * PURPOSE      Check that  locked  (  marked  as  'read-only'  )
 *              buffer buf have not been modified  and unlock it.
 *              If buf is NULL then check and unmark all buffers.
 *
 * ARGUMENTS    IN VPTR buf - Pointer to the locked buffer or NULL.
 *
 * RETURNS      IMAL error code:
 *              MEM_OK                - O.K.
 *              MEM_ERR_BUFFER_DAMAGE - Buffer has been modified.
 *              MEM_ERR_NOT_FOUND     - buf is not locked.
 *              MEM_ERR_FATAL         - Locked buffer list is corrupted.
 *              Variable check_damage_ptr is set to the address
 *              of corrupted/damaged data.
 *
 * EXTERNS      None
 * NOTE         None
 ---------------------------------------------------------------------*/

int  mem_static_release(buf)
IN VPTR buf;
  {
  LOCKED_BUF_REF *p, **pp;     /* Auxiliary pointers */
  int err;                     /* Checking result    */

  check_damage_ptr = NULL;  /* No damage found */

  /* CASE 'single buffer' */

  if (buf != NULL)
    {
    pp = imal_check_locked_ptr(buf);
    if (check_errno == MEM_ERR_BUFFER_DAMAGE)
      check_damage_ptr = buf;
    if (pp != NULL)     /* Release locked reference block */
      {                 /* and remove it from locked list */
      p = *pp;
      *pp = p->next;
      free(p);
      imal_add_csize(0 - sizeof(LOCKED_BUF_REF));
      }
    return check_errno;
    }

  /* CASE 'release all buffers' */

  /* Check buffers */
  err = mem_check_locked(NULL);
  if (err != MEM_OK && err != MEM_ERR_BUFFER_DAMAGE)
    {
    return err;   /* Locked reference list is corrupted */
    }

  /* Release all buffers */
  while (locked_buf_head != NULL)
    {
    p = locked_buf_head;
    locked_buf_head = locked_buf_head->next;
    free(p);
    imal_add_csize(0 - sizeof(LOCKED_BUF_REF));
    } /* end while */

  return err;
  } /* end of mem_static_release function */


/* --------------------------------------------------------------------
 * NAME         mem_check_locked
 * PURPOSE      Check locked buffer (all buffers if buf is NULL).
 *
 * ARGUMENTS    IN VPTR buf - Pointer to the locked buffer or NULL.
 *
 * RETURNS      IMAL error code:
 *              MEM_OK                - O.K.
 *              MEM_ERR_BUFFER_DAMAGE - Buffer has been modified.
 *              MEM_ERR_NOT_FOUND     - buf is not locked.
 *              MEM_ERR_FATAL         - Locked buffer list is corrupted.
 *              Variable check_damage_ptr is set to the address
 *              of corrupted/damaged data.
 *
 * EXTERNS      None
 * NOTE         None
 ---------------------------------------------------------------------*/

int  mem_check_locked(buf)
IN VPTR buf;
  {
  LOCKED_BUF_REF **pp; /* Pointer to the next reference */
                       /* of the locked buffers list    */
  int err;             /* Checking result               */

  check_damage_ptr = NULL;  /* No damage found */

  /* CASE 'single buffer checking' */

  if (buf != NULL)
    {
    imal_check_locked_ptr(buf);
    if (check_errno == MEM_ERR_BUFFER_DAMAGE)
      check_damage_ptr = buf;
    return check_errno;
    }

  /* CASE 'all buffers checking' */

  err = MEM_OK;
  for (pp = &locked_buf_head; *pp != NULL; pp = &((*pp)->next))
    {
    /* Check current/next list item that it not corrupted */
    if ((*pp)->tag != IMAL_TAG_LOCKED)
      {
      check_damage_ptr = (VPTR)(*pp);
      return MEM_ERR_FATAL;
      }

    /* Check that current buffer has not been modified */
    if (err == MEM_OK)
      {
      if (check_sum((*pp)->buf, (*pp)->length) != (*pp)->check_sum)
        {
        check_damage_ptr = (*pp)->buf;
        err = MEM_ERR_BUFFER_DAMAGE;
        }
      }
    } /* end for */

  return err;
  } /* end of mem_check_locked function */


                        /* Interface functions */

/* --------------------------------------------------------------------
 * NAME         imal_get_locked_damage_info
 * PURPOSE      Get info about damaged data which has been set by
 *              mem_check_locked() / mem_static_release().
 * ARGUMENTS    OUT VPTR* damage_ptr    - Corrupted buffer or internal
 *                                        data address.
 * RETURNS      None.
 * EXTERNS      None.
 * NOTE         None.
 ---------------------------------------------------------------------*/

VOID imal_get_locked_damage_info(damage_ptr)
OUT VPTR *damage_ptr;
  {
  *damage_ptr = check_damage_ptr;
  } /* end of imal_get_locked_damage_info function */


                       /* Auxiliary functions */

/* --------------------------------------------------------------------
 * NAME         imal_check_locked_ptr
 * PURPOSE      Check that  locked_ptr is a pointer to the locked
 *              buffer. Check that buffer is not damaged.
 *
 * ARGUMENTS    IN VPTR locked_ptr - Pointer to locked buffer.
 *
 * RETURNS      Pointer to  the  pointer  to  the  item in locked
 *              buffer references  list which contains  reference
 *              to the  locked_ptr.  NULL  if  locked_ptr  is not
 *              locked buffer or buffer list corrupted.  Variable
 *              check_errno   contains   result   code:   MEM_OK,
 *              MEM_ERR_BUFFER_DAMAGE,    MEM_ERR_NOT_FOUND    or
 *              MEM_ERR_FATAL.  In the  case  of  MEM_FATAL_ERROR
 *              check_damage_ptr   contains    the   address   of
 *              the corrupted internal data.
 *
 * EXTERNS      None
 * NOTE         Function returns pointer to  the  pointer because
 *              it gives the possibility to remove  the item from
 *              the locked buffer references list.
 ---------------------------------------------------------------------*/

static LOCKED_BUF_REF **imal_check_locked_ptr(locked_ptr)
IN VPTR locked_ptr;
  {
  LOCKED_BUF_REF **pp; /* Pointer to the next reference */
                       /* of the locked buffers list    */

  /* Seek locked_ptr in locked buffers list */
  for (pp = &locked_buf_head; *pp != NULL; pp = &((*pp)->next))
    {
    /* Check current/next list item that it not corrupted */
    if ((*pp)->tag != IMAL_TAG_LOCKED)
      {
      check_errno = MEM_ERR_FATAL;
      check_damage_ptr = (VPTR)(*pp);
      return NULL;
      }

    /* If we find locked_ptr ... */
    if ((*pp)->buf == locked_ptr)
      {
      check_errno =
        (check_sum((*pp)->buf, (*pp)->length) == (*pp)->check_sum ?
        MEM_OK : MEM_ERR_BUFFER_DAMAGE);
      return pp;
      }
    } /* end for */

  /* locked ptr has not been found in the list */
  check_errno = MEM_ERR_NOT_FOUND;
  return NULL;
  } /* end of imal_check_locked_ptr function */


/* --------------------------------------------------------------------
 * NAME         check_sum
 * PURPOSE      Calculate check sum of the buffer.
 *
 * ARGUMENTS    IN VPTR buf - Pointer to the buffer.
 *              IN SIZE_T len - Length of the buffer.
 *
 * RETURNS      Check sum of the buffer.
 *
 * EXTERNS      None
 * NOTE         The function  presented  below can be replaced by
 *              another one (for example CRC-32 check sum).
 *              This function has the advantage that it is fast.
 ---------------------------------------------------------------------*/

static long check_sum(buf, length)
IN VPTR buf;
IN SIZE_T length;
  {
  long *p;             /* pointer to the checked data            */
  LONG_CHAR beg_data;  /* check sum of the nonaligned left edje  */
  long sum;            /* check sum of the 'middle' part of buf  */
  LONG_CHAR end_data;  /* check sum of the nonaligned right edje */
  char *end_buf;       /* Auxiliary pointer                      */

  beg_data.val = 0;
  if (length <= sizeof(long))
    {
    memcpy(beg_data.pval, buf, length);
    return beg_data.val;
    }
  end_data.val = 0;
  end_buf = ((char*)buf) + length;
  memcpy(beg_data.pval, buf, sizeof(long));
  memcpy(end_data.pval, end_buf - sizeof(long), sizeof(long));
  p = (long*)(end_buf - ((SIZE_T)end_buf) % sizeof(long));
  for (sum = 0; (VPTR)(--p) >= buf; )
    {
    sum += *p;
    }
  return beg_data.val + sum + end_data.val;
  } /* end of check_sum function */


/*** End of file LOCKED.C ***/
