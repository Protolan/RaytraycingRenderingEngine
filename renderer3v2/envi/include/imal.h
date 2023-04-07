/* FILE NAME    imal.h
 * PURPOSE      IMAL library internal definitions.
 * 
 * SPEC         Vladimir Volevich                     1993-06-07
 * CODING       Vladimir Volevich                     1993-08-02
 *
 * NOTES        None
 * 
 * LAST UPDATED V. L. Volevich                        1993.08.11
 *
 * imal.h,v
 * Revision 1.1  1993/12/23  18:26:36  cvs
 * Initial revision
 *
 * 
 */

#ifndef _IMAL_H_
#define _IMAL_H_

/*------------------------- CONSTANTS -------------------------*/

                        /* Error codes */

#define MEM_OK                      0  /* Result is correct */
#define MEM_ERR_NOT_INIT            1  /* IMAL is not initialized yet */
#define MEM_ERR_INIT                2  /* IMAL is initialized */
#define MEM_ERR_NOT_EMPTY           3  /* There are not released */
                                       /* blocks                 */
#define MEM_ERR_BLOCK_DAMAGE        4  /* Allocated block damage */
#define MEM_ERR_BUFFER_DAMAGE       5  /* Locked buffer violation */
#define MEM_ERR_FATAL               6  /* IMAL internal structures */
                                       /* are corrupted            */
#define MEM_ERR_NOT_FOUND           7  /* Pointer is not a pointer to */
                                       /* allocated or locked block   */
#define MEM_ERR_RELEASED            8  /* Pointer is a pointer to */
                                       /* released block          */
#define MEM_WARN_CLASS_NO_CHECK     9  /* Pointer is a pointer to   */
                                       /* block of "no_check" class */
#define MEM_ERR_NOT_MEMORY         10  /* Not enough memory to */
                                       /* allocate block       */
#define MEM_ERR_BAD_SIZE           11  /* Invalid size of requested */
                                       /* block (0)                 */
#define MEM_ERR_NULL_PTR           12  /* Pointer passed to IMAL   */
                                       /* function is NULL         */
#define MEM_ERR_RTL_DAMAGE         13  /* Buffer overflow in C RTL */
                                       /* overlayed function       */
#define MEM_WARN_RTL_NOSIZE        14  /* Unknown buffer size in   */
                                       /* C RTL overlayed function */
#define MEM_ERR_IO                 15  /* File write error */
#define MEM_ERR_MODE_BAD           16  /* Wrong Imal mode */
#define MEM_WARN_MODE_OVER         17  /* Mode stack overflow */
#define MEM_ERR_CLASS_NOT_CREATED  18  /* Class is not created yet */
#define MEM_ERR_CLASS_NOT_RELEASED 19  /* Not all  blocks  of  closed */
                                       /* class are released          */
#define MEM_ERR_CLASS_PREDEFINED   20  /* Illegal operation with */
                                       /* predefined class       */

        /* Values returned by user's error processing functions */

#define MEM_PROG_STOP      0  /* Stop the program. */
#define MEM_PROG_CONTINUE  1  /* Continue execution */

                        /* IMAL modes */

#define MEM_MODE_NODEBUG  0
#define MEM_MODE_NOTRACE  0
#define MEM_MODE_DEBUG    0x01
#define MEM_MODE_TRACE    0x02
#define MEM_MODE_WARNING  0x04
#define MEM_MODE_CONTINUE 0x08
#define MEM_MODE_MODIFY   0x10

   /*  Values of parameter mode of mem_block_size/mem_class_size */

#define MEM_INFO_CURRENT  0   /* Info about current blocks. */
#define MEM_INFO_MAX      1   /* Info about maximum blocks. */


                /* Class identifiers */

#define MEM_NOT_CLASS     (-1)  /* The value which cannot coincide */
                                /*  with any class identifier.     */

#define MEM_CLASS_DEFAULT   0   /* Identifier of the class "default" */

#define MEM_CLASS_NO_CHECK  1   /* Identifier of the class "no check" */
                                /* It is class of blocks without      */
                                /* control part.                      */

/* Types of error processing functions */

typedef int (*MEM_TYPE_ERROR_FUN)
  (IN int, IN char*, IN char*, IN int, IN VPTR, IN SIZE_T);

typedef void (*MEM_TYPE_ABORT_FUN) (void);


/*----------------- MACROS FOR C RTL FUNCTIONS -----------------*/

#ifndef   __FILE__
#define   __FILE__   "Unknown"
#endif

#ifndef   __LINE__
#define   __LINE__   0
#endif


#ifdef   IMAL_TO_RTL_FUN
            /* "Empty' definitions of IMAL functions */
                /* module "mode" */

#define  mem_error()                             (MEM_OK)
#define  mem_strerr(err)                         ("")
#define  mem_mode_open(mode)                     (MEM_OK)
#define  mem_mode_close()                        (MEM_OK)
#define  mem_mode_current()                      (MEM_MODE_NODEBUG)
#define  mem_mode_level()                        (0)

                /* module "imal" */

#define  imal_version()                          ("")
#define  mem_init(err_fun, abort_fun, file)      (MEM_OK)
#define  mem_close()                             (MEM_OK)
#define  mem_alloc(id, bl_name, size)            malloc(size)
#define  mem_calloc(id, bl_name, items, size)    calloc(items, size)
#define  mem_free(ptr)                           (free(ptr), MEM_OK)
#define  mem_realloc(ptr, size)                  realloc(ptr, size)
#define  mem_empty()                             (MEM_OK)
#define  mem_print_list(file)                    (MEM_OK)
#define  mem_print_class(file, id)               (MEM_OK)
#define  mem_block(ptr)                          (MEM_OK)
#define  mem_check_block(ptr)                    (MEM_OK)
#define  mem_block_size(ptr,blk, mem,ctrl) \
                                        (*blk=0, *mem=0, *ctrl=0, MEM_OK)
#define  mem_class_size(id,mode,blk,mem,cltrl) \
                                        (*blk=0, *mem=0, *ctrl=0, MEM_OK)
#define  mem_query_bloc(ptr, id)                 (*id=MEM_CLASS_NO_CHECK, "")
#define  mem_block_next(ptr, mode)               (*ptr=NULL, MEM_OK)

                /* module "class" */

#define  mem_class_create(name)                  (MEM_CLASS_NO_CHECK)
#define  mem_class_close(id)                     (MEM_OK)
#define  mem_query_class(id)                     ("NO_CHECK")

                /* module "locked" */

#define  mem_static_lock(ptr,len)                (MEM_OK)
#define  mem_static_release(ptr)                 (MEM_OK)
#define  mem_check_locked(ptr)                   (MEM_OK)

                /* module "rtl" */
static int strdp_len;
static char *strdp_ptr;
#define  mem_strdup(ptr) \
     (strdp_len = strlen(ptr), strdp_ptr = malloc(strdp_len + 1), \
      strcpy(strdp_ptr,ptr), strdp_ptr)

/*   temporarily inactive: */

/*
#define  RTL_malloc(ptr)                         malloc(ptr)
#define  RTL_calloc(items,size)                  calloc(ptr,size)
#define  RTL_free(ptr)                           (free(ptr), 0)
#define  RTL_realloc(ptr,size)                   realloc(ptr,size)
*/

#else   /* IMAL functions and macros are active:  */

#ifndef IMAL_NO_C_RTL_MACRO     /* This MACRO is defined into IMAL */
                                /* source files. It turn off the   */
                                /* redefinition of C RTL functions */
                                /* It allows to redefine such      */
                                /* functions into imal.h           */
#ifdef malloc
#undef malloc
#endif
#ifdef calloc
#undef calloc
#endif
#ifdef free
#undef free
#endif
#ifdef realloc
#undef realloc
#endif
#ifdef strdup
#undef strdup
#endif
#ifdef strcpy
#undef strcpy
#endif
#ifdef strcat
#undef strcat
#endif
#ifdef memcpy
#undef memcpy
#endif
#ifdef fgets
#undef fgets
#endif
#ifdef fread
#undef fread
#endif
#ifdef strncpy
#undef strncpy
#endif
#ifdef strncat
#undef strncat
#endif
#ifdef memset
#undef memset
#endif
#ifdef gets
#undef gets
#endif


#ifdef RTL_MEMORY_TO_IMAL     /* This name #defined in user's  files  */
                              /* It turn on the translation of C RTL  */
                              /* functions malloc, calloc, free,      */
                              /* realloc to the IMAL functions mem_*  */

#define malloc(size) mem_alloc(MEM_CLASS_DEFAULT, \
          imal_default_name(__FILE__, __LINE__), (size))

#define calloc(size, num)  mem_calloc(MEM_CLASS_DEFAULT, \
          imal_default_name(__FILE__, __LINE__), (size), (num))

#define free(ptr)  mem_free(ptr)

#define realloc(ptr, size)  mem_realloc(ptr, size)

#define strdup(str)  mem_strdup(str)

#else  /* RTL_MEMORY_TO_IMAL */

#define malloc(size) \
 (printf("Usage of ordinary malloc() function\
 with IMAL is forbidden!\n"), (VPTR)NULL)

#define calloc(size, num) \
 (printf("Usage of ordinary calloc() function\
 with IMAL is forbidden!\n"), (VPTR)NULL)

#define free(ptr) \
 (printf("Usage of ordinary free() function\
 with IMAL is forbidden!\n"), (VPTR)NULL)

#define realloc(ptr, size) \
 (printf("Usage of ordinary realloc() function\
 with IMAL is forbidden!\n"), (VPTR)NULL)

#define strdup(ptr) \
 (printf("Usage of ordinary strdup() function\
 with IMAL is forbidden!\n"), (VPTR)NULL)

#endif  /* RTL_MEMORY_TO_IMAL */
#endif  /* #ifndef IMAL_NO_C_RTL_MACRO */

#ifdef RTL_STRING_TO_IMAL     /* This name #defined in user's  files  */
                              /* It turn on the translation of C RTL  */
                              /* string and memory copying functions  */
                              /* to the IMAL functions mem_*          */

#define strcpy(dest, source) \
  mem_strcpy(sizeof(dest), dest, source, __FILE__, __LINE__)

#define strcat(dest, source) \
  mem_strcat(sizeof(dest), dest, source, __FILE__, __LINE__)

#define memcpy(dest, source, len) \
  mem_memcpy(sizeof(dest), dest, source, len, __FILE__, __LINE__)

#define fgets(dest, len, stream) \
  mem_fgets(sizeof(dest), dest, len, stream, __FILE__, __LINE__)

#define fread(dest, size, n, stream) \
  mem_fread(sizeof(dest), dest, size, n, stream, __FILE__, __LINE__)

#define strncpy(dest, source, len) \
  mem_strncpy(sizeof(dest), dest, source, len, __FILE__, __LINE__)

#define strncat(dest, source, len) \
  mem_strncat(sizeof(dest), dest, source, len, __FILE__, __LINE__)

#define memset(dest, sym, len) \
  mem_memset(sizeof(dest), dest, sym, len, __FILE__, __LINE__)

#define gets(dest) \
  mem_gets(sizeof(dest), dest, __FILE__, __LINE__)

#endif  /* RTL_MEMORY_TO_IMAL */


#ifdef mem_alloc
#undef mem_alloc
#endif
#define mem_alloc(class, name, size) \
  mem_alloc_fun(class, name, size, __FILE__, __LINE__)

#ifdef mem_calloc
#undef mem_calloc
#endif
#define mem_calloc(class, name, size, num) \
  mem_calloc_fun(class, name, size, num, __FILE__, __LINE__)

#ifdef mem_free
#undef mem_free
#endif
#define mem_free(ptr) \
  mem_free_fun(ptr, __FILE__, __LINE__)

#ifdef mem_realloc
#undef mem_realloc
#endif
#define mem_realloc(ptr, size) \
  mem_realloc_fun(ptr, size, __FILE__, __LINE__)

#ifdef mem_strdup
#undef mem_strdup
#endif
#define mem_strdup(str) \
  mem_strdup_fun(str, __FILE__, __LINE__)



/*------------------------ PROTOTYPES -------------------------*/

                /* module "mode" */

extern int mem_error(void);
extern char *mem_strerr(IN int);
extern int mem_mode_open(IN int);
extern int mem_mode_close(void);
extern int mem_mode_current(void);
extern int mem_mode_level(void);

                /* module "imal" */

extern char *imal_version(void);
extern int mem_init(IN MEM_TYPE_ERROR_FUN, IN MEM_TYPE_ABORT_FUN, IN char *);
extern int mem_close(void);
extern VPTR mem_alloc_fun(IN int, IN char*, IN SIZE_T, IN char*, IN int);
extern VPTR mem_calloc_fun(IN int, IN char*, IN SIZE_T, IN SIZE_T, IN char*, IN int);
extern int mem_free_fun(IN VPTR, IN char*, IN int);
extern VPTR mem_realloc_fun(IN VPTR, IN SIZE_T, IN char*, IN int);
extern int mem_empty(void);
extern int mem_print_list(INOUT FILE*);
extern int mem_print_class(INOUT FILE*, IN int);
extern int mem_block(IN VPTR);
extern int mem_check_block(IN VPTR);
extern int mem_block_size(IN VPTR, IN int, OUT SIZE_T*, OUT SIZE_T*);
extern int mem_class_size(IN int, IN int, OUT SIZE_T*, OUT SIZE_T*, OUT SIZE_T*);
extern char *mem_query_block(IN VPTR, OUT int*);
extern int mem_block_next(INOUT VPTR*, IN int);

                /* module "class" */

extern int mem_class_create(IN char*);
extern int mem_class_close(IN int);
extern char *mem_query_class(IN int);

                /* module "locked" */

extern int mem_static_lock(IN VPTR, IN SIZE_T);
extern int mem_static_release(IN VPTR);
extern int mem_check_locked(IN VPTR);

                /* module "rtl" */

extern char *mem_strdup_fun(IN char*, IN char*, IN int);
extern char *mem_strcpy(IN SIZE_T, OUT char *, IN char *, IN char *, IN int);
extern char *mem_strcat(IN SIZE_T, OUT char *, IN char *, IN char *, IN int);
extern VPTR mem_memcpy(IN SIZE_T, OUT VPTR, IN VPTR, IN SIZE_T, IN char *, IN int);
extern char *mem_fgets(IN SIZE_T, OUT char *, IN int, INOUT FILE *, IN char *, IN int);
extern SIZE_T mem_fread(IN SIZE_T, OUT VPTR, IN SIZE_T, IN SIZE_T,
                        INOUT FILE *, IN char *, IN int);
extern char *mem_strncpy(IN SIZE_T, OUT char *, IN char *,
                               IN SIZE_T, IN char *, IN int);
extern char *mem_strncat(IN SIZE_T, OUT char *, IN char *,
                               IN SIZE_T, IN char *, IN int);
extern VPTR mem_memset(IN SIZE_T, OUT VPTR, IN int, IN SIZE_T,
                        IN char *, IN int);
extern char *mem_gets(IN SIZE_T, OUT char *, IN char *, IN int);

extern char *imal_default_name(IN char *, IN unsigned);

extern VPTR RTL_malloc(IN SIZE_T);
extern VPTR RTL_calloc(IN SIZE_T, IN SIZE_T);
extern int RTL_free(IN VPTR);
extern VPTR RTL_realloc(IN VPTR, IN SIZE_T);
extern char* RTL_strdup(IN char*);

#endif /* of mem_.. "empty" functions definitions */

#endif  /* _IMAL_H_ */

/* END OF imal.h HEADER */
