/* FILE NAME    imal_loc.h
 * PURPOSE      IMAL library internal definitions.
 * 
 * SPEC         Vladimir Volevich                     1993-06-07
 * CODING       Vladimir Volevich                     1993-08-02
 *
 * NOTES        None
 *
 * LAST UPDATED V. L. Volevich                        1993.08.11
 *
 */


#ifndef _IMAL_LOC_H_
#define _IMAL_LOC_H_


/*--------------------- TYPE DEFINITIONS ----------------------*/

                       /* Module imal */

typedef struct double_ptr    /* pointers of two way list item */
  {
  struct double_ptr *prev;
  struct double_ptr *next;
  } DOUBLE_PTR;

typedef struct block_control_info  /* allocated block control part */
  {
  DOUBLE_PTR ptr;              /* ptr to prev/next control structure */
  int class_id;                /* ident. of block's class descriptor */
  int name_size;               /* length of user's block name */
  SIZE_T block_size;         /* size of user's block */
  } BLOCK_CONTROL_INFO;


                       /* Module class */

typedef struct class_info     /* Information about class */
  {
  BLOCK_CONTROL_INFO *block;  /* Ptr to the sublist of blocks */
                              /* of this class or NULL.       */
  SIZE_T block_num;           /* number of blocks of this class */
  SIZE_T max_block_num;       /* maximum   number   of   blocks */
                              /* during run time of the program.*/
  SIZE_T block_size;          /* Size of user's parts of blocks */
  SIZE_T max_block_size;      /* Maximum size of user's parts   */
                              /* during run time of the program.*/
  SIZE_T block_csize;         /* Size of blocks control parts   */
  SIZE_T max_block_csize;     /* Maximum memory occupied by c.parts*/
                              /* during run time of the program.   */
  } CLASS_INFO;


/*------------------------- TAG VALUES ------------------------*/

/*
Each byte of tag  value differ from 0,1,-1. Any two bytes  of tag
differ  from  each other. Tag size in  bytes  is  max(ALIGN , 2).
ALIGN is defined into int_env.h.
*/

#define IMAL_TAG_SIZE (ALIGN >= 2 ? ALIGN : 2) /* Size of prefix item */

#define IMAL_TAG_PREFIX_SIZE   2  /* Size of prefix tag  */
#define IMAL_TAG_POSTFIX_SIZE  2  /* Size of postfix tag */


#define IMAL_TAG_PREFIX     0xd9b76ce5L
          /* Allocated block prefix tag */

#define IMAL_TAG_POSTFIX    0x73685bf4L
          /* Allocated block postfix tag */

#define IMAL_TAG_NO_CHECK   0x6a23f9b7L
          /* Prefix of block of class "no check" */

#define IMAL_TAG_RELEASED   0x6becadf1L
          /* Released block tag */

#define IMAL_TAG_LOCKED     ((short) 0xb71c)   /* 0xfda9b71cL */
          /* Locked buffer reference tag */

#define IMAL_TAG_CLASS      ((short) 0x591e)   /* 0xc476591eL */
          /* Class descriptor tag */

#define IMAL_TAG_CLASS_FREE ((short) 0x981c)   /* 0xfe5a981cL */
          /* Free class descriptor tag */


/*-------------- MACROS FOR ALLOCATED BLOCK ITEMS -------------*/

/*
Allocated block consist of the following items:

sign   item                explanations

N  BLOCK NAME          Name  of  allocated  block - parameter  of
                       mem_alloc() function. Name  is  terminated
                       by  zero  byte.  Name  can  has  arbitrary
                       length stored  in  the  field name_size of
                       the BLOCK INFO item.

A  Align space         The  empty  space which  is  necessary  to
                       align the  next  item.  Possible length is
                       between 0 and ALIGN-1.

I  BLOCK INFO          This    is    struct    BLOCK_CONTROL_INFO
                       described above. It  contains  information
                       about  the  block and  references  to  the
                       previous/next block.  This item should  be
                       aligned.

T1 PREFIX TAG          Length of  this item is ALIGN. It contains
                       the  value IMAL_TAG_PREFIX.  It  used  for
                       allocated block damage checking.
                       This item is aligned,  therefore  the next
                       item is also aligned.

U  USER'S DATA BLOCK   This item  should be aligned. Size of this
                       item is given by mem_alloc() parameter and
                       stored in  the  field  block_size of BLOCK
                       INFO item.

T2 POSTFIX TAG         Length of  this item is 2. It contains
                       the  value  IMAL_TAG_POSTFIX. It  used for
                       allocated block damage checking.


Aligned means aligned  for objects of  any type i.e.  start  from
address such as ALIGN*n.  Constant  ALIGN is defined in int_env.h
file. It depend on processor.

Blocks of  default  class  MEM_CLASS_NO_CHECK has more simple form:

  NO_CHECK tag - max(ALIGN,2) bytes, value is IMAL_TAG_NOCHECK.
  USER'S DATA BLOCK - aligned.
*/


/*
 * Macros  which  provide access to the allocated  blocks items.
 * Access  to  allocated blocks items only via these macros !!!
*/

/* Is it possible to get the items of allocated block control */
/* structure from address ptr ?                               */
#define IMAL_ALIGNED(ptr) \
  (((SIZE_T)(ptr)) % (ALIGN < sizeof(long) ? ALIGN : sizeof(long)) == 0)

/*
   NOTE: We use above min(ALIGN,sizeof(long)) instead ALIGN because:
         1) For SCO UNIX constant ALIGN is 8 but allocated blocks
            addresses are N*4.
         2) Align sizeof(long) is enough for control part items
            having types long/VPTR.
*/

#define BLOCK_CONTROL_INFO_SIZE \
  (((sizeof(BLOCK_CONTROL_INFO) + ALIGN - 1) / ALIGN) * ALIGN)

/* Block info structure (user_block) */
#define IMAL_BLOCK_INFO(uptr)                \
  ((BLOCK_CONTROL_INFO *) (((char*)(uptr)) - \
    (IMAL_TAG_SIZE + BLOCK_CONTROL_INFO_SIZE)))

/* List ptr (user_block) */
#define IMAL_BLOCK_PTR(uptr) \
  (&(IMAL_BLOCK_INFO(uptr)->ptr))

/* Block name length (user_block) */
#define IMAL_BLOCK_NAME_LEN(uptr) \
  (IMAL_BLOCK_INFO(uptr)->name_size)

/* User's block length (user_block) */
#define IMAL_BLOCK_USER_LEN(uptr) \
  (IMAL_BLOCK_INFO(uptr)->block_size)

/* User's block class identifier (user_block) */
#define IMAL_BLOCK_CLASS_ID(uptr) \
  (IMAL_BLOCK_INFO(uptr)->class_id)

/* Block name (user_block) */
#define IMAL_BLOCK_NAME(uptr)                   \
  ((char *) (((char*)(IMAL_BLOCK_INFO(uptr))) - \
    ((IMAL_BLOCK_NAME_LEN(uptr) + ALIGN) / ALIGN) * ALIGN))

/* Whole block (user_block) */
#define IMAL_BLOCK_BEGIN(uptr)   \
  ((VPTR) IMAL_BLOCK_NAME(uptr))

/* Give user's part address by block list pointer */
#define IMAL_BLOCK_LIST_TO_USER(ptr) \
  ((VPTR) (((char*)(ptr)) +          \
    (BLOCK_CONTROL_INFO_SIZE + IMAL_TAG_SIZE)))

/* Give user's part address by whole block address. */
#define IMAL_BLOCK_SYS_TO_USER(ptr, name_len) \
  ((VPTR) (((char*)(ptr)) + ((((name_len) + ALIGN) / ALIGN) * ALIGN + \
    (BLOCK_CONTROL_INFO_SIZE + IMAL_TAG_SIZE))))

#define IMAL_BLOCK_SYS_TO_NAME(ptr)  ((char*)(ptr))

/* block size (user's part + control part). Passed to the malloc() */
#define IMAL_BLOCK_SIZE(user_size, name_size)    \
  ((SIZE_T)((((name_size) + ALIGN) / ALIGN) * ALIGN +         \
   (BLOCK_CONTROL_INFO_SIZE + IMAL_TAG_SIZE + IMAL_TAG_POSTFIX_SIZE) + \
   (SIZE_T)(user_size)))

/* Control part size */
#define IMAL_BLOCK_CONTROL_SIZE(uptr) \
  (((IMAL_BLOCK_NAME_LEN(uptr) + ALIGN) / ALIGN) * ALIGN + \
   (BLOCK_CONTROL_INFO_SIZE + IMAL_TAG_SIZE + IMAL_TAG_POSTFIX_SIZE))


        /* Prefix/postfix operations/queries */

/* Prefix (user_block) */
#define IMAL_BLOCK_PREFIX_ADR(uptr) \
    (((char*)(uptr)) - IMAL_TAG_PREFIX_SIZE)

/* Postfix address(user_block) */
#define IMAL_BLOCK_POSTFIX_ADR(uptr) \
  (((char*)(uptr)) + IMAL_BLOCK_USER_LEN(uptr))

#define IMAL_BLOCK_SET_PREFIX(ptr, IMAL_TAG_PREFIX) \
  (IMAL_BLOCK_PREFIX_ADR(ptr)[0] = (char)IMAL_TAG_PREFIX, \
   IMAL_BLOCK_PREFIX_ADR(ptr)[1] = (char)(IMAL_TAG_PREFIX >> 8))

#define IMAL_BLOCK_EQUAL_PREFIX(ptr, IMAL_TAG_PREFIX) \
  (IMAL_BLOCK_PREFIX_ADR(ptr)[0] == (char)IMAL_TAG_PREFIX && \
   IMAL_BLOCK_PREFIX_ADR(ptr)[1] == (char)(IMAL_TAG_PREFIX >> 8))

#define IMAL_BLOCK_SET_POSTFIX(ptr, IMAL_TAG_POSTFIX) \
  (IMAL_BLOCK_POSTFIX_ADR(ptr)[0] = (char)IMAL_TAG_POSTFIX, \
   IMAL_BLOCK_POSTFIX_ADR(ptr)[1] = (char)(IMAL_TAG_POSTFIX >> 8))

#define IMAL_BLOCK_EQUAL_POSTFIX(ptr, IMAL_TAG_POSTFIX) \
  (IMAL_BLOCK_POSTFIX_ADR(ptr)[0] == (char)IMAL_TAG_POSTFIX && \
   IMAL_BLOCK_POSTFIX_ADR(ptr)[1] == (char)(IMAL_TAG_POSTFIX >> 8))


                /* "NO_CHECK" class macros */

#define IMAL_BLOCK_NOCHECK_SIZE(size)   \
  ((SIZE_T)(((SIZE_T)(size)) + IMAL_TAG_SIZE))
#define IMAL_BLOCK_NOCHECK_BEGIN(uptr)  \
  ((VPTR) (((char*)(uptr)) - IMAL_TAG_SIZE))
#define IMAL_BLOCK_NOCHECK_USER(ptr)    \
  ((VPTR) (((char*)(ptr)) + IMAL_TAG_SIZE))


/*------------------------ PROTOTYPES -------------------------*/

                        /* Module mode */

extern VOID imal_err(IN int);
extern VOID imal_error_proc(IN int, IN char*, IN char*, IN int, IN VPTR, IN SIZE_T);
extern int imal_debug(IN char*, IN int);
extern VOID imal_init(IN MEM_TYPE_ERROR_FUN, IN MEM_TYPE_ABORT_FUN, OUT char*, IN int);
extern int imal_print_damaged_block(INOUT FILE *, IN VPTR, IN SIZE_T);

                        /* Module imal */

extern VOID imal_get_damage_info(OUT VPTR*, OUT SIZE_T*);
extern VOID imal_mode_set(IN int, IN int, IN int);
extern VOID imal_get_corrupted_info(IN VPTR, OUT char**, OUT SIZE_T*, OUT int *);
extern VOID imal_add_csize(IN SIZE_T);

                        /* Module rtl */

extern VOID imal_mode_rtl_set(IN int, IN int);

                        /* Module locked */

extern VOID imal_get_locked_damage_info(OUT VPTR*);

                        /* Module class */

extern CLASS_INFO *imal_class_id_to_ptr(IN int);
extern int imal_class_next(IN int);
extern int imal_class_check_id(IN int);

#endif  /* _IMAL_LOC_H_ */

/* END OF imal_loc.h HEADER */
