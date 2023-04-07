/*
 * MODULE NAME  CLASS.C
 * PURPOSE      Support of the class descriptops of the allocated
 *              blocks in IMAL library.
 *
 * SPEC         V. L. Volevich                        1993.01.12
 * DESIGN       V. L. Volevich                        1993.06.03
 * DES. INSP.   Z. Blaszczyk                          1993.06.28
 * CODING       V. L. Volevich                        1993.07.20
 * CODE INSP.   Z. Blaszczyk                          1993.07.??
 * TEST         V. L. Volevich                        1993.07.21
 *
 * LAST UPDATED V. L. Volevich                        1993.08.11
 *
 */

/* COMPANY INCLUDES: */

#define IMAL_NO_C_RTL_MACRO  /* Turn off macros for C RTL functions */

#include "integra.h"
#include "imal.h"


/* PROJECT INCLUDE: */

#include "imal_loc.h"


/* TYPEDEFS */

typedef struct class_dsc        /* Class descriptor */
  {
  short tag;  /* This field has  the value IMAL_TAG_CLASS */
              /* for any  used  class  descriptor. It has */
              /* the  value  IMAL_TAG_CLASS_FREE for  any */
              /* free  class  descriptor.  Tag  allow  to */
              /* differ used  and free class  descriptors */
              /* and  to differ  class  descriptors  from */
              /* other data.                              */
  char *name;               /* Name of the class.           */
  int next;                 /* index of next busy/free dsc. */
  CLASS_INFO info;      /* info about class. This field is  */
                        /* accessible for "imal" module.    */
  } CLASS_DSC;


/* MACROS */

#define PREDEFINED_CLASS(class_id) \
  ((class_id) == MEM_CLASS_DEFAULT || (class_id) == MEM_CLASS_NO_CHECK)


/* CONSTANTS */

#define CLASS_TABLE_SIZE_1  20  /* Size of class table after */
                                /* the first extension.      */

#define CLASS_TABLE_DSIZE  100  /* Number of items which added to the */
                                /* class table during extensions.     */

/*-------------------------------------------------------------/

         Internal data structures of the CLASS module

Internal data of module "class" consist of class descriptor table
and auxiliary variables.
Class descriptor table is array of type CLASS_DSC which contains
descriptors of classes: used ( i.e. created  but  not closed) and
free class descriptors. List of used descriptors and list of free
descriptors are supported. At the beginning  the class descriptor
table is placed  in  a static  array.  If class descriptor  table
became full IMAL allocates a new table (greater then old  one) by
malloc(), copy data from old table to new, and release the old if
it also have been allocated.
Class names stored in buffers allocated by malloc().
Therefore practically there no  limits  for number of classes and
class name length.
The other advantage of such data structures is the fast access to
a class descriptor by class identifier.
Class descriptors can be modified  by  module  imal.c (access via
function imal_class_id_to_ptr).

This is scheme of class data:

        +-----+
        |free |------->-------+   +----->---+   +-->--+
        +-----+               |   |         |   |     |
+-----+        +-+--+---+--+--+---+---+--+--+---+-----+---------+
|class|------> |class1|class2|empty |class3|empty |empty |class4|
|ptr  |        +--+-+---+---+---------+---+---------------+-----+
+-----+        ^  | |   | | |         | | |               |  |
               |  | +->-+ | +---->----+ | +-------->------+  |
   +-----+     |  |       |             |                    |
   |used |-->--+  V       V             V                    V
   +-----+      +-+---+ +-+---+       +-+---+             +--+--+
                |name1| |name2|       |name3|             |name4|
                +-----+ +-----+       +-----+             +-----+

/-------------------------------------------------------------*/


/****************************** DATA **********************************/

static CLASS_DSC default_class_table[] =
  {
  {IMAL_TAG_CLASS, "DEFAULT",  MEM_CLASS_NO_CHECK, {NULL, 0, 0, 0, 0}},
  {IMAL_TAG_CLASS, "NO_CHECK", MEM_NOT_CLASS,      {NULL, 0, 0, 0, 0}}
  };

/*
Class  descriptor   table.   Two  default  classes  (DEFAULT  and
NO_CHECK) defined by static data initialization. MEM_NOT_CLASS is
the value which cannot coincide with any class identifier.
Field info of class descriptor  is  accessible  for other modules
via  function  imal_class_id_to_ptr.  Other fields accessible  in
this module only.
*/


static int class_used_list = MEM_CLASS_DEFAULT;
static int class_free_list = MEM_NOT_CLASS;

/*
Headers  of  used   descriptor   list  /  free  descriptor  list.
MEM_NOT_CLASS means end of list.
Free descriptors  list  is  useful  for  mem_class_create(); used
descriptors list used by imal memory statistic functions.
*/


static CLASS_DSC *class_table = default_class_table;
static int class_table_size = sizeof(default_class_table) / sizeof(CLASS_DSC);

/* Pointer to the class descriptor table and table size */

/* Disable "cast truncates constant value" warning */
#pragma warning(disable : 4310)


/****************************** CODE **********************************/


                    /* Library functions */


/* --------------------------------------------------------------------------
 * NAME         mem_class_create
 * PURPOSE      Create a new class of name class_name.
 * ARGUMENTS    IN char *class_name - Name of class.
 * RETURNS      Class  identifier.  MEM_NOT_CLASS  on failure
 *              (Not enough memory or internal data corrupted).
 *              Error code will be set by function imal_err.
 * EXTERNS      None
 * NOTE         If mem_class_create(name)  called twice with  the
 *              same name it creates two  classes with  the  same
 *              names  but  different   identifiers.  Other  IMAL
 *              functions which use classes have parameter  class
 *              identifier (not  class  name).  Therefore call of
 *              functions  such  as mem_query_class(class_id)  in
 *              this case is not ambiguous.
 *              This feature  has  been  done because two classes
 *              with the same names are possible  in big projects
 *              which implemented by several developers.
 --------------------------------------------------------------------------*/

int mem_class_create(class_name)
IN char *class_name;
  {
  int i;               /* Loop variable                   */
  CLASS_DSC *pclass;   /* Pointer to the new class table  */
  int new_size;        /* Size of the new class table     */

  if (class_name == NULL)
    {
    imal_err(MEM_ERR_NULL_PTR);
    return MEM_NOT_CLASS;
    }
  if (class_free_list == MEM_NOT_CLASS)  /* class table is full */
    {
    /* Allocate a new table - longer than the old one */
    new_size = (class_table_size < CLASS_TABLE_SIZE_1 ?
      CLASS_TABLE_SIZE_1 : class_table_size + CLASS_TABLE_DSIZE);
    pclass = malloc(new_size * sizeof(CLASS_DSC));
    if (pclass == NULL)
      {
      imal_err(MEM_ERR_NOT_MEMORY);
      return MEM_NOT_CLASS;
      }

    /* Correct the total size of IMAL control structures */
    imal_add_csize(new_size * sizeof(CLASS_DSC));

    /* Copy old table to the new one */
    memcpy(pclass, class_table, class_table_size * sizeof(CLASS_DSC));

    /* Initialize new table; insert dsc. to the free dsc. list */
    for (i = class_table_size; i < new_size; i++)
      {
      pclass[i].tag = IMAL_TAG_CLASS_FREE;
      pclass[i].next = i + 1;
      }
    pclass[new_size - 1].next = MEM_NOT_CLASS;
    class_free_list = class_table_size;

    /* Release class_table if it has been allocated */
    if (class_table != default_class_table)
      {
      free(class_table);
      imal_add_csize(-class_table_size * sizeof(CLASS_DSC));
      }

    /* Set class data */
    class_table = pclass;
    class_table_size = new_size;
    }  /* end if (class table full) */

  /* ASSERT: Now class_free_list contains index of some */
  /*         empty descriptor. We use this descriptor.  */

  i = class_free_list;
  if ((class_table[i].name = malloc(strlen(class_name) + 1)) == NULL)
    {
    imal_err(MEM_ERR_NOT_MEMORY);
    return MEM_NOT_CLASS;
    }
  imal_add_csize(strlen(class_name) + 1);
  class_table[i].tag = IMAL_TAG_CLASS;
  strcpy(class_table[i].name, class_name);
  class_table[i].info.block = NULL;

  class_table[i].info.block_num = 0;
  class_table[i].info.max_block_num = 0;
  class_table[i].info.block_size = 0;
  class_table[i].info.max_block_size = 0;
  class_table[i].info.block_csize = 0;
  class_table[i].info.max_block_csize = 0;

  /* Remove descriptor from free list
     and insert it to the used list  */
  class_free_list = class_table[i].next;
  class_table[i].next = class_used_list;
  class_used_list = i;

  imal_err(MEM_OK);
  return i;
  } /* end of mem_class_create function */


/* --------------------------------------------------------------------------
 * NAME         mem_class_close
 * PURPOSE      Release class descriptor.
 * ARGUMENTS    IN int class_id - Class identifier.
 * RETURNS      Error code:
 *              MEM_OK,
 *              MEM_ERR_CLASS_NOT_CREATED,
 *              MEM_ERR_CLASS_NOT_RELEASED.
 *              MEM_ERR_CLASS_PREDEFINED.
 * EXTERNS      None
 --------------------------------------------------------------------------*/

int mem_class_close(class_id)
IN int class_id;
  {
  int *pnext;   /* Pointer to the 'next' field of class descriptors */

  /* Predefined class cannot be released */
  if (PREDEFINED_CLASS(class_id))
    {
    return MEM_ERR_CLASS_PREDEFINED;
    }

  if (imal_class_id_to_ptr(class_id) == NULL)
    {
    return MEM_ERR_CLASS_NOT_CREATED;
    }   /* class_id is not class descriptor index */

  /* Check that there are no allocated blocks of this class */
  if (class_table[class_id].info.block != NULL)
    {
    return MEM_ERR_CLASS_NOT_RELEASED;
    }

  /* Remove descriptor from used list and insert it to the free list */
  for (pnext = &class_used_list;
       *pnext != class_id;
       pnext = &(class_table[*pnext].next))
    {
    if (*pnext < 0 || *pnext >= class_table_size)
      return MEM_ERR_FATAL;       /* Class table corrupted */
    }
  *pnext = class_table[class_id].next;
  class_table[class_id].next = class_free_list;
  class_free_list = class_id;

  /* 'close' class descriptor */
  class_table[class_id].tag = IMAL_TAG_CLASS_FREE;
  imal_add_csize(0 - (strlen(class_table[class_id].name) + 1));
  free(class_table[class_id].name);

  /* If all classes are released release class table */
  if (PREDEFINED_CLASS(class_used_list))
    {
    if (class_table != default_class_table)
      {
      free(class_table);
      imal_add_csize(-class_table_size * sizeof(CLASS_DSC));
      }
    class_free_list = MEM_NOT_CLASS;
    class_table = default_class_table;
    class_table_size = sizeof(default_class_table) / sizeof(CLASS_DSC);
    }

  return MEM_OK;
  } /* end of mem_class_close function */


/* --------------------------------------------------------------------------
 * NAME         mem_query_class
 * PURPOSE      Returns name of the class.
 * ARGUMENTS    IN int class_id - Class identifier.
 * RETURNS      Name or NULL if class_id is not a class descriptor.
 * EXTERNS      None
 --------------------------------------------------------------------------*/


char *mem_query_class(class_id)
IN int class_id;
  {
  if (imal_class_id_to_ptr(class_id) == NULL)
    {
    return NULL;
    }   /* class_id is not class descriptor index */

  return class_table[class_id].name;
  } /* end of mem_duery_class function */


                     /* Interface functions */


/* --------------------------------------------------------------------------
 * NAME         imal_class_id_to_ptr
 * PURPOSE      Check class descriptor of identifier class_id and
 *              return its info address.
 * ARGUMENTS    IN int class_id - Class identifier.
 * RETURNS      Class descriptor info pointer or NULL
 *              (if  class_id is not class identifier or
 *              class descriptor have been corrupted).
 * EXTERNS      None
 --------------------------------------------------------------------------*/

CLASS_INFO *imal_class_id_to_ptr(class_id)
IN int class_id;
  {
  CLASS_DSC *pclass;   /* Class descriptor pointer */

  if (class_id < 0 || class_id >= class_table_size)
    {
    return NULL;        /* No such class */
    }
  pclass = class_table + class_id;
  return pclass->tag == (short)IMAL_TAG_CLASS ? &(pclass->info) : NULL;
  /*
     tag is not IMAL_TAG_CLASS means that class descriptor is
     free or corrupted.
  */
  } /* end of imal_class_id_to_ptr function */


/* --------------------------------------------------------------------------
 * NAME         imal_class_check_id
 * PURPOSE      Check class descriptor of identifier class_id.
 * ARGUMENTS    IN int class_id - Class identifier.
 * RETURNS      MEM_OK
 *              MEM_ERR_CLASS_NOT_CREATED
 *              (class_id is not class identifier)
 *              MEM_ERR_FATAL
 *              (class descriptor have been corrupted)
 * EXTERNS      None
 --------------------------------------------------------------------------*/

int imal_class_check_id(class_id)
IN int class_id;
  {
  if (class_id < 0 || class_id >= class_table_size ||
      class_table[class_id].tag == IMAL_TAG_CLASS_FREE)
    {
    return MEM_ERR_CLASS_NOT_CREATED;
    }
  if (class_table[class_id].tag == IMAL_TAG_CLASS)
    {
    return MEM_OK;
    }
  return MEM_ERR_FATAL;
  } /* end of imal_class_check_id function */


/* --------------------------------------------------------------------------
 * NAME         imal_class_next
 * PURPOSE      Give first/next used class identifier.
 * ARGUMENTS    IN int class_id - Class identifier or MEM_NOT_CLASS.
 * RETURNS      First descriptor if class_id is  MEM_NOT_CLASS.
 *              Next descriptor if class_id is not MEM_NOT_CLASS.
 *              MEM_NOT_CLASS if there are no more descriptors.
 * EXTERNS      None
 --------------------------------------------------------------------------*/

int imal_class_next(class_id)
IN int class_id;
  {
  if (class_id == MEM_NOT_CLASS)
    return class_used_list;
  if (imal_class_id_to_ptr(class_id) == NULL)
    return MEM_NOT_CLASS;       /* class_id is not class dsc. */
  return class_table[class_id].next;
  } /* end of imal_class_next function */


/*** End of file CLASS.C ***/
