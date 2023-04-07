/*
 * MODULE NAME  RTL.C
 * PURPOSE      Module contains functions which replace C RTL
 *              function supported by IMAL library:
 *                strcpy, strcat, strncpy, strncat,
 *                memcpy, memset, fgets, fread, gets,
 *                realloc, strdup.
 *
 * SPEC         V. L. Volevich                        1993.01.12
 * DESIGN       V. L. Volevich                        1993.06.03
 * DES. INSP.   Z. Blaszczyk                          1993.06.28
 * CODING       V. L. Volevich                        1993.08.02
 * CODE INSP.   Z. Blaszczyk                          1993.08.??
 * TEST         V. L. Volevich                        1993.08.10
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


/* CONSTANTS */

/* Length of names used in macros which translate   */
/* malloc/free to the mem_alloc/mem_free.           */
/* Currently such name is <file_name>:<line_number> */

#define MAX_DEFAULT_NAME_LEN \
   (MAX_FILENAME_LENGTH + MAX_SUFFIX_LENGTH + 9)

/* MACROS */

#define MEM_BLOCK(ptr) \
  (IMAL_ALIGNED(ptr) && IMAL_BLOCK_EQUAL_PREFIX(ptr, IMAL_TAG_PREFIX))

/****************************** DATA *************************************/

static char imal_default_name_buf[MAX_DEFAULT_NAME_LEN];

/* Buffer used for preparing names in malloc/free macros */


static BOOL imal_mode_debug_fl = FALSE;
static BOOL imal_mode_warning_fl = FALSE;

/* IMAL mode flags. Set by imal_rtl_mode_set() called from mode.c */

/* Disable "cast truncates constant value" warning */
#pragma warning(disable : 4310)

/****************************** CODE *************************************/

/* --------------------------------------------------------------------
 * NAME         mem_strcpy
 * PURPOSE      Replace C RTL strcpy() function. Check destination size
 * ARGUMENTS    IN SIZE_T passed_dest_size - Size of dest or sizeof(VPTR)
 *              OUT char *dest - Parameters of strcpy().
 *              IN char *source
 *              IN char *file - Source file name.
 *              IN int line   - Source line.
 *
 * RETURNS      Value returned by strcpy()
 * EXTERNS      None
 * NOTE         If case of dest overflow if program will not be
 *              stopped in error processing procedure then no more than
 *              SIZE(dest) bytes will be copied.
 *              Value ptr passed to the error processing procedure is
 *              source.
 ---------------------------------------------------------------------*/

char *mem_strcpy(passed_dest_size, dest, source, file, line)
IN SIZE_T passed_dest_size;
OUT char *dest;
IN char *source;
IN char *file;
IN int line;
  {
  SIZE_T dest_size;           /* Size of the destination buffer */

  /* Check parameters */
  if (dest == NULL || source == NULL)
    {
    imal_error_proc(MEM_ERR_NULL_PTR, "strcpy", file, line, NULL, 0);
    /* !!! Program can be stopped in imal_error_proc() */
    imal_err(MEM_ERR_NULL_PTR);
    return NULL;
    }

  imal_err(MEM_OK);
  /* Make debug checking if necessary */
  if (imal_mode_debug_fl == TRUE)
    {
    imal_err(imal_debug(file, line));
    /* !!! Program can be stopped in imal_debug() */
    }

  /* Establish the size of dest buffer */
  if ((dest_size = passed_dest_size) == sizeof(VPTR))
    {
    /* dest is not static array */
    if (!MEM_BLOCK((VPTR)dest))
      {
      /* dest is neither static array nor allocated block */
      /* Its size cannot be established.                  */

      imal_err(MEM_WARN_RTL_NOSIZE);
      if (imal_mode_warning_fl == TRUE)
        {
        imal_error_proc(MEM_WARN_RTL_NOSIZE, "strcpy", file, line, (VPTR)source, 0);
        /* !!! Program can be stopped in imal_error_proc() */
        }

      return strcpy(dest, source);

      } /* end if */
    dest_size = IMAL_BLOCK_USER_LEN((VPTR)dest);
    } /* end if (dest_size == sizeof(VPTR)) */

  /* ASSERT: Size of dest is established and placed to the dest_size */
  /* Establish the size of source buffer and compare with dest size */

  if (strlen(source) + 1 > dest_size)
    {
    /* Buffer overflow */
    imal_error_proc(MEM_ERR_RTL_DAMAGE, "strcpy", file, line, (VPTR)source, 0);
    /* !!! Program can be stopped in imal_error_proc() */
    imal_err(MEM_ERR_RTL_DAMAGE);
    dest[dest_size - 1] = 0;
    return strncpy(dest, source, dest_size - 1);
    }

  return strcpy(dest, source);
  } /* end of mem_strcpy function */

/* --------------------------------------------------------------------
 * NAME         mem_strcat
 * PURPOSE      Replace C RTL strcat() function. Check destination size
 * ARGUMENTS    IN SIZE_T passed_dest_size - Size of dest or sizeof(VPTR)
 *              OUT char *dest - Parameters of strcat().
 *              IN char *source
 *              IN char *file - Source file name.
 *              IN int line   - Source line.
 *
 * RETURNS      Value returned by strcat()
 * EXTERNS      None
 * NOTE         If case of dest overflow if program will not be
 *              stopped in error processing procedure then no more than
 *              SIZE(dest) bytes will be copied.
 *              Value ptr passed to the error processing procedure is
 *              source.
 ---------------------------------------------------------------------*/

char *mem_strcat(passed_dest_size, dest, source, file, line)
IN SIZE_T passed_dest_size;
OUT char *dest;
IN char *source;
IN char *file;
IN int line;
  {
  SIZE_T dest_size;           /* Size of the destination buffer */

  /* Check parameters */
  if (dest == NULL || source == NULL)
    {
    imal_error_proc(MEM_ERR_NULL_PTR, "strcat", file, line, NULL, 0);
    /* !!! Program can be stopped in imal_error_proc() */
    imal_err(MEM_ERR_NULL_PTR);
    return NULL;
    }

  imal_err(MEM_OK);
  /* Make debug checking if necessary */
  if (imal_mode_debug_fl == TRUE)
    {
    imal_err(imal_debug(file, line));
    /* !!! Program can be stopped in imal_debug() */
    }

  /* Establish the size of dest buffer */
  if ((dest_size = passed_dest_size) == sizeof(VPTR))
    {
    /* dest is not static array */
    if (!MEM_BLOCK((VPTR)dest))
      {
      /* dest is neither static array nor allocated block */
      /* Its size cannot be established.                  */

      imal_err(MEM_WARN_RTL_NOSIZE);
      if (imal_mode_warning_fl == TRUE)
        {
        imal_error_proc(MEM_WARN_RTL_NOSIZE, "strcat", file, line, (VPTR)source, 0);
        /* !!! Program can be stopped in imal_error_proc() */
        }

      return strcat(dest, source);

      } /* end if */
    dest_size = IMAL_BLOCK_USER_LEN((VPTR)dest);
    } /* end if (dest_size == sizeof(VPTR)) */

  /* ASSERT: Size of dest is established and placed to the dest_size */
  /* Establish the size of source buffer and compare with dest size */

  if (strlen(dest) + strlen(source) + 1 > dest_size)
    {
    /* Buffer overflow */
    imal_error_proc(MEM_ERR_RTL_DAMAGE, "strcat", file, line, (VPTR)source, 0);
    /* !!! Program can be stopped in imal_error_proc() */
    imal_err(MEM_ERR_RTL_DAMAGE);
    dest[dest_size - 1] = 0;
    return strncat(dest, source, dest_size - 1 - strlen(dest));
    }

  return strcat(dest, source);
  } /* end of mem_strcat function */

/* --------------------------------------------------------------------
 * NAME         mem_strncpy
 * PURPOSE      Replace C RTL strncpy() function. Check destination size
 * ARGUMENTS    IN SIZE_T passed_dest_size - Size of dest or sizeof(VPTR)
 *              OUT char *dest - Parameters of strncpy().
 *              IN char *source
 *              IN SIZE_T len
 *              IN char *file - Source file name.
 *              IN int line   - Source line.
 *
 * RETURNS      Value returned by strncpy()
 * EXTERNS      None
 * NOTE         If case of dest overflow if program will not be
 *              stopped in error processing procedure then no more than
 *              SIZE(dest) bytes will be copied.
 ---------------------------------------------------------------------*/

char *mem_strncpy(passed_dest_size, dest, source, len, file, line)
IN SIZE_T passed_dest_size;
OUT char *dest;
IN char *source;
IN SIZE_T len;
IN char *file;
IN int line;
  {
  SIZE_T dest_size;           /* Size of the destination buffer */

  /* Check parameters */
  if (dest == NULL || source == NULL)
    {
    imal_error_proc(MEM_ERR_NULL_PTR, "strncpy", file, line, NULL, 0);
    /* !!! Program can be stopped in imal_error_proc() */
    imal_err(MEM_ERR_NULL_PTR);
    return NULL;
    }

  imal_err(MEM_OK);
  /* Make debug checking if necessary */
  if (imal_mode_debug_fl == TRUE)
    {
    imal_err(imal_debug(file, line));
    /* !!! Program can be stopped in imal_debug() */
    }

  /* Establish the size of dest buffer */
  if ((dest_size = passed_dest_size) == sizeof(VPTR))
    {
    /* dest is not static array */
    if (!MEM_BLOCK((VPTR)dest))
      {
      /* dest is neither static array nor allocated block */
      /* Its size cannot be established.                  */

      imal_err(MEM_WARN_RTL_NOSIZE);
      if (imal_mode_warning_fl == TRUE)
        {
        imal_error_proc(MEM_WARN_RTL_NOSIZE, "strncpy", file, line, NULL, 0);
        /* !!! Program can be stopped in imal_error_proc() */
        }

      return strncpy(dest, source, len);

      } /* end if */
    dest_size = IMAL_BLOCK_USER_LEN((VPTR)dest);
    } /* end if (dest_size == sizeof(VPTR)) */

  /* ASSERT: Size of dest is established and placed to the dest_size */
  /* Establish the size of source buffer and compare with dest size */

  if (len > dest_size)
    {
    /* Buffer overflow */
    imal_error_proc(MEM_ERR_RTL_DAMAGE, "strncpy", file, line, NULL, 0);
    /* !!! Program can be stopped in imal_error_proc() */
    imal_err(MEM_ERR_RTL_DAMAGE);
    return strncpy(dest, source, dest_size);
    }

  return strncpy(dest, source, len);
  } /* end of mem_strncpy function */

/* --------------------------------------------------------------------
 * NAME         mem_strncat
 * PURPOSE      Replace C RTL strncat() function. Check destination size
 * ARGUMENTS    IN SIZE_T passed_dest_size - Size of dest or sizeof(VPTR)
 *              OUT char *dest - Parameters of strncat().
 *              IN char *source
 *              IN SIZE_T len
 *              IN char *file - Source file name.
 *              IN int line   - Source line.
 *
 * RETURNS      Value returned by strncat()
 * EXTERNS      None
 * NOTE         If case of dest overflow if program will not be
 *              stopped in error processing procedure then no more than
 *              SIZE(dest) bytes will be copied.
 ---------------------------------------------------------------------*/

char *mem_strncat(passed_dest_size, dest, source, len, file, line)
IN SIZE_T passed_dest_size;
OUT char *dest;
IN char *source;
IN SIZE_T len;
IN char *file;
IN int line;
  {
  SIZE_T dest_size;           /* Size of the destination buffer */

  /* Check parameters */
  if (dest == NULL || source == NULL)
    {
    imal_error_proc(MEM_ERR_NULL_PTR, "strncat", file, line, NULL, 0);
    /* !!! Program can be stopped in imal_error_proc() */
    imal_err(MEM_ERR_NULL_PTR);
    return NULL;
    }

  imal_err(MEM_OK);
  /* Make debug checking if necessary */
  if (imal_mode_debug_fl == TRUE)
    {
    imal_err(imal_debug(file, line));
    /* !!! Program can be stopped in imal_debug() */
    }

  /* Establish the size of dest buffer */
  if ((dest_size = passed_dest_size) == sizeof(VPTR))
    {
    /* dest is not static array */
    if (!MEM_BLOCK((VPTR)dest))
      {
      /* dest is neither static array nor allocated block */
      /* Its size cannot be established.                  */

      imal_err(MEM_WARN_RTL_NOSIZE);
      if (imal_mode_warning_fl == TRUE)
        {
        imal_error_proc(MEM_WARN_RTL_NOSIZE, "strncat", file, line, NULL, 0);
        /* !!! Program can be stopped in imal_error_proc() */
        }

      return strncat(dest, source, len);

      } /* end if */
    dest_size = IMAL_BLOCK_USER_LEN((VPTR)dest);
    } /* end if (dest_size == sizeof(VPTR)) */

  /* ASSERT: Size of dest is established and placed to the dest_size */
  /* Establish the size of source buffer and compare with dest size */

  if (strlen(dest) + 1 + len > dest_size)
    {
    /* Buffer overflow */
    imal_error_proc(MEM_ERR_RTL_DAMAGE, "strncat", file, line, NULL, 0);
    /* !!! Program can be stopped in imal_error_proc() */
    imal_err(MEM_ERR_RTL_DAMAGE);
    if (strlen(dest) + 1 < dest_size)
      return strncat(dest, source, dest_size - strlen(dest) - 1);
    else
      return dest;
    }

  return strncat(dest, source, len);
  } /* end of mem_strncat function */

/* --------------------------------------------------------------------
 * NAME         mem_memcpy
 * PURPOSE      Replace C RTL memcpy() function. Check destination size
 * ARGUMENTS    IN SIZE_T passed_dest_size - Size of dest or sizeof(VPTR)
 *              OUT VPTR dest - Parameters of memcpy().
 *              IN VPTR source
 *              IN SIZE_T len
 *              IN char *file - Source file name.
 *              IN int line   - Source line.
 *
 * RETURNS      Value returned by memcpy()
 * EXTERNS      None
 * NOTE         If case of dest overflow if program will not be
 *              stopped in error processing procedure then no more than
 *              SIZE(dest) bytes will be copied.
 ---------------------------------------------------------------------*/

VPTR mem_memcpy(passed_dest_size, dest, source, len, file, line)
IN SIZE_T passed_dest_size;
OUT VPTR dest;
IN VPTR source;
IN SIZE_T len;
IN char *file;
IN int line;
  {
  SIZE_T dest_size;           /* Size of the destination buffer */

  /* Check parameters */
  if (dest == NULL || source == NULL)
    {
    imal_error_proc(MEM_ERR_NULL_PTR, "memcpy", file, line, NULL, 0);
    /* !!! Program can be stopped in imal_error_proc() */
    imal_err(MEM_ERR_NULL_PTR);
    return NULL;
    }

  imal_err(MEM_OK);
  /* Make debug checking if necessary */
  if (imal_mode_debug_fl == TRUE)
    {
    imal_err(imal_debug(file, line));
    /* !!! Program can be stopped in imal_debug() */
    }

  /* Establish the size of dest buffer */
  if ((dest_size = passed_dest_size) == sizeof(VPTR))
    {
    /* dest is not static array */
    if (!MEM_BLOCK((VPTR)dest))
      {
      /* dest is neither static array nor allocated block */
      /* Its size cannot be established.                  */

      imal_err(MEM_WARN_RTL_NOSIZE);
      if (imal_mode_warning_fl == TRUE)
        {
        imal_error_proc(MEM_WARN_RTL_NOSIZE, "memcpy", file, line, NULL, 0);
        /* !!! Program can be stopped in imal_error_proc() */
        }

      return memcpy(dest, source, len);

      } /* end if */
    dest_size = IMAL_BLOCK_USER_LEN((VPTR)dest);
    } /* end if (dest_size == sizeof(VPTR)) */

  /* ASSERT: Size of dest is established and placed to the dest_size */
  /* Establish the size of source buffer and compare with dest size */

  if (len > dest_size)
    {
    /* Buffer overflow */
    imal_error_proc(MEM_ERR_RTL_DAMAGE, "memcpy", file, line, NULL, 0);
    /* !!! Program can be stopped in imal_error_proc() */
    imal_err(MEM_ERR_RTL_DAMAGE);
    return memcpy(dest, source, dest_size);
    }

  return memcpy(dest, source, len);
  } /* end of mem_memcpy function */

/* --------------------------------------------------------------------
 * NAME         mem_memset
 * PURPOSE      Replace C RTL memset() function. Check destination size
 * ARGUMENTS    IN SIZE_T passed_dest_size - Size of dest or sizeof(VPTR)
 *              OUT VPTR dest - Parameters of memset().
 *              IN int sym
 *              IN SIZE_T len
 *              IN char* file - Source file name.
 *              IN int line   - Source line.
 *
 * RETURNS      Value returned by memset()
 * EXTERNS      None
 * NOTE         If case of dest overflow if program will not be
 *              stopped in error processing procedure then no more than
 *              SIZE(dest) bytes will be copied.
 ---------------------------------------------------------------------*/

VPTR mem_memset(passed_dest_size, dest, sym, len, file, line)
IN SIZE_T passed_dest_size;
OUT VPTR dest;
IN int sym;
IN SIZE_T len;
IN char* file;
IN int line;
  {
  SIZE_T dest_size;           /* Size of the destination buffer */

  /* Check parameters */
  if (dest == NULL)
    {
    imal_error_proc(MEM_ERR_NULL_PTR, "memset", file, line, NULL, 0);
    /* !!! Program can be stopped in imal_error_proc() */
    imal_err(MEM_ERR_NULL_PTR);
    return NULL;
    }

  imal_err(MEM_OK);
  /* Make debug checking if necessary */
  if (imal_mode_debug_fl == TRUE)
    {
    imal_err(imal_debug(file, line));
    /* !!! Program can be stopped in imal_debug() */
    }

  /* Establish the size of dest buffer */
  if ((dest_size = passed_dest_size) == sizeof(VPTR))
    {
    /* dest is not static array */
    if (!MEM_BLOCK((VPTR)dest))
      {
      /* dest is neither static array nor allocated block */
      /* Its size cannot be established.                  */

      imal_err(MEM_WARN_RTL_NOSIZE);
      if (imal_mode_warning_fl == TRUE)
        {
        imal_error_proc(MEM_WARN_RTL_NOSIZE, "memset", file, line, NULL, 0);
        /* !!! Program can be stopped in imal_error_proc() */
        }

      return memset(dest, sym, len);

      } /* end if */
    dest_size = IMAL_BLOCK_USER_LEN((VPTR)dest);
    } /* end if (dest_size == sizeof(VPTR)) */

  /* ASSERT: Size of dest is established and placed to the dest_size */
  /* Establish the size of source buffer and compare with dest size */

  if (len > dest_size)
    {
    /* Buffer overflow */
    imal_error_proc(MEM_ERR_RTL_DAMAGE, "memset", file, line, NULL, 0);
    /* !!! Program can be stopped in imal_error_proc() */
    imal_err(MEM_ERR_RTL_DAMAGE);
    return memset(dest, sym, dest_size);
    }

  return memset(dest, sym, len);
  } /* end of mem_memset function */

/* --------------------------------------------------------------------
 * NAME         mem_fread
 * PURPOSE      Replace C RTL fread() function. Check destination size
 * ARGUMENTS    IN SIZE_T passed_dest_size - Size of dest or sizeof(VPTR)
 *              OUT VPTR dest - Parameters of fread().
 *              IN SIZE_T item_size
 *              IN SIZE_T item_num
 *              INOUT FILE* stream
 *              IN char *file - Source file name.
 *              IN int line   - Source line.
 *
 * RETURNS      Value returned by fread()
 * EXTERNS      None
 * NOTE         If case of dest overflow if program will not be
 *              stopped in error processing procedure then no more than
 *              SIZE(dest) bytes will be copied.
 ---------------------------------------------------------------------*/

SIZE_T mem_fread(passed_dest_size, dest, item_size, item_num, stream, file, line)
IN SIZE_T passed_dest_size;
OUT VPTR dest;
IN SIZE_T item_size;
IN SIZE_T item_num;
INOUT FILE* stream;
IN char *file;
IN int line;
  {
  SIZE_T dest_size;           /* Size of the destination buffer */

  /* Check parameters */
  if (dest == NULL || stream == NULL)
    {
    imal_error_proc(MEM_ERR_NULL_PTR, "fread", file, line, NULL, 0);
    /* !!! Program can be stopped in imal_error_proc() */
    imal_err(MEM_ERR_NULL_PTR);
    return (SIZE_T)-1;
    }
  if (item_size == 0 || item_num == 0)
    {
    imal_error_proc(MEM_ERR_BAD_SIZE, "fread", file, line, NULL, 0);
    /* !!! Program can be stopped in imal_error_proc() */
    imal_err(MEM_ERR_BAD_SIZE);
    return (SIZE_T)-1;
    }

  imal_err(MEM_OK);
  /* Make debug checking if necessary */
  if (imal_mode_debug_fl == TRUE)
    {
    imal_err(imal_debug(file, line));
    /* !!! Program can be stopped in imal_debug() */
    }

  /* Establish the size of dest buffer */
  if ((dest_size = passed_dest_size) == sizeof(VPTR))
    {
    /* dest is not static array */
    if (!MEM_BLOCK((VPTR)dest))
      {
      /* dest is neither static array nor allocated block */
      /* Its size cannot be established.                  */

      imal_err(MEM_WARN_RTL_NOSIZE);
      if (imal_mode_warning_fl == TRUE)
        {
        imal_error_proc(MEM_WARN_RTL_NOSIZE, "fread", file, line, NULL, 0);
        /* !!! Program can be stopped in imal_error_proc() */
        }

      return fread(dest, item_size, item_num, stream);

      } /* end if */
    dest_size = IMAL_BLOCK_USER_LEN((VPTR)dest);
    } /* end if (dest_size == sizeof(VPTR)) */

  /* ASSERT: Size of dest is established and placed to the dest_size */
  /* Establish the size of source buffer and compare with dest size */

  if (item_size * item_num > dest_size)
    {
    /* Buffer overflow */
    imal_error_proc(MEM_ERR_RTL_DAMAGE, "fread", file, line, NULL, 0);
    /* !!! Program can be stopped in imal_error_proc() */
    imal_err(MEM_ERR_RTL_DAMAGE);
    if (dest_size < item_size) return 0;
    return fread(dest, item_size, dest_size / item_size, stream);
    }

  return fread(dest, item_size, item_num, stream);
  } /* end of mem_fread function */

/* --------------------------------------------------------------------
 * NAME         mem_fgets
 * PURPOSE      Replace C RTL fgets() function. Check destination size
 * ARGUMENTS    IN SIZE_T passed_dest_size - Size of dest or sizeof(VPTR)
 *              OUT char *dest - Parameters of fgets().
 *              IN int len
 *              INOUT FILE* stream
 *              IN char *file - Source file name.
 *              IN int line   - Source line.
 *
 * RETURNS      Value returned by fgets()
 * EXTERNS      None
 * NOTE         If case of dest overflow if program will not be
 *              stopped in error processing procedure then no more than
 *              SIZE(dest) bytes will be copied.
 ---------------------------------------------------------------------*/

char *mem_fgets(passed_dest_size, dest, len, stream, file, line)
IN SIZE_T passed_dest_size;
OUT char *dest;
IN int len;
INOUT FILE* stream;
IN char *file;
IN int line;
  {
  int dest_size;           /* Size of the destination buffer */

  /* Check parameters */
  if (dest == NULL || stream == NULL)
    {
    imal_error_proc(MEM_ERR_NULL_PTR, "fgets", file, line, NULL, 0);
    /* !!! Program can be stopped in imal_error_proc() */
    imal_err(MEM_ERR_NULL_PTR);
    return NULL;
    }
  if (len == 0)
    {
    imal_error_proc(MEM_ERR_BAD_SIZE, "fgets", file, line, NULL, 0);
    /* !!! Program can be stopped in imal_error_proc() */
    imal_err(MEM_ERR_BAD_SIZE);
    return NULL;
    }

  imal_err(MEM_OK);
  /* Make debug checking if necessary */
  if (imal_mode_debug_fl == TRUE)
    {
    imal_err(imal_debug(file, line));
    /* !!! Program can be stopped in imal_debug() */
    }

  /* Establish the size of dest buffer */
  if ((dest_size = (int)passed_dest_size) == sizeof(VPTR))
    {
    /* dest is not static array */
    if (!MEM_BLOCK((VPTR)dest))
      {
      /* dest is neither static array nor allocated block */
      /* Its size cannot be established.                  */

      imal_err(MEM_WARN_RTL_NOSIZE);
      if (imal_mode_warning_fl == TRUE)
        {
        imal_error_proc(MEM_WARN_RTL_NOSIZE, "fgets", file, line, NULL, 0);
        /* !!! Program can be stopped in imal_error_proc() */
        }

      return fgets(dest, len, stream);

      } /* end if */
    dest_size = (int)IMAL_BLOCK_USER_LEN((VPTR)dest);
    } /* end if (dest_size == sizeof(VPTR)) */

  /* ASSERT: Size of dest is established and placed to the dest_size */
  /* Establish the size of source buffer and compare with dest size */

  if (len > dest_size)
    {
    /* Buffer overflow */
    imal_error_proc(MEM_ERR_RTL_DAMAGE, "fgets", file, line, NULL, 0);
    /* !!! Program can be stopped in imal_error_proc() */
    imal_err(MEM_ERR_RTL_DAMAGE);
    return fgets(dest, dest_size, stream);
    }

  return fgets(dest, len, stream);
  } /* end of mem_fgets function */

/* --------------------------------------------------------------------
 * NAME         mem_gets
 * PURPOSE      Replace C RTL gets() function. Check destination size
 * ARGUMENTS    IN SIZE_T passed_dest_size - Size of dest or sizeof(VPTR)
 *              OUT char *dest - Parameter of gets().
 *              IN char *file - Source file name.
 *              IN int line   - Source line.
 *
 * RETURNS      Value returned by gets()
 * EXTERNS      None
 * NOTE         Even if dest size can be established mem_gets cannot
 *              prevent buffer overflow.
 ---------------------------------------------------------------------*/

char *mem_gets(passed_dest_size, dest, file, line)
IN SIZE_T passed_dest_size;
OUT char *dest;
IN char *file;
IN int line;
  {
  SIZE_T dest_size;           /* Size of the destination buffer */
  char *gets_res;             /* Result of gets() call         */

  /* Check parameters */
  if (dest == NULL)
    {
    imal_error_proc(MEM_ERR_NULL_PTR, "gets", file, line, NULL, 0);
    /* !!! Program can be stopped in imal_error_proc() */
    imal_err(MEM_ERR_NULL_PTR);
    return NULL;
    }

  imal_err(MEM_OK);
  /* Make debug checking if necessary */
  if (imal_mode_debug_fl == TRUE)
    {
    imal_err(imal_debug(file, line));
    /* !!! Program can be stopped in imal_debug() */
    }

  /* Establish the size of dest buffer */
  if ((dest_size = passed_dest_size) == sizeof(VPTR))
    {
    /* dest is not static array */
    if (!MEM_BLOCK((VPTR)dest))
      {
      /* dest is neither static array nor allocated block */
      /* Its size cannot be established.                  */

      imal_err(MEM_WARN_RTL_NOSIZE);
      if (imal_mode_warning_fl == TRUE)
        {
        imal_error_proc(MEM_WARN_RTL_NOSIZE, "gets", file, line, NULL, 0);
        /* !!! Program can be stopped in imal_error_proc() */
        }

      return gets_s(dest, 4096);

      } /* end if */
    dest_size = IMAL_BLOCK_USER_LEN((VPTR)dest);
    } /* end if (dest_size == sizeof(VPTR)) */

  /* ASSERT: Size of dest is established and placed to the dest_size */
  /* Establish the size of source buffer and compare with dest size */

  gets_res = gets_s(dest, 4096);
  if (strlen(dest) + 1 > dest_size)
    {
    /* Buffer overflow */
    imal_error_proc(MEM_ERR_RTL_DAMAGE, "gets", file, line, NULL, 0);
    /* !!! Program can be stopped in imal_error_proc() */
    imal_err(MEM_ERR_RTL_DAMAGE);
    return gets_res;
    }

  return gets_res;
  } /* end of mem_gets function */


/* --------------------------------------------------------------------
 * NAME         mem_strdup_fun
 * PURPOSE      Replace C RTL strdup() function. Check destination size
 * ARGUMENTS    IN char *str - Parameter of strdup().
 *              IN char *file - Source file name.
 *              IN int line   - Source line.
 *
 * RETURNS      Ptr to the new string or NULL. Error code will be set.
 * EXTERNS      None
 * NOTE         None
 ---------------------------------------------------------------------*/

char *mem_strdup_fun(str, file, line)
IN char *str;
IN char *file;
IN int line;
  {
  VPTR ptr;     /* Allocated block pointer */

  if (str == NULL)
    {
    imal_err(MEM_ERR_NULL_PTR);
    return NULL;
    }
  if ((ptr = mem_alloc_fun(MEM_CLASS_DEFAULT,
                        imal_default_name(file, line),
                        strlen(str) + 1, file, line)) == NULL)
    {
    /* IMAL error code is set by mem_alloc_fun */
    return NULL;
    }
  return strcpy((char*)ptr, str);
  } /* end of mem_strdup_fun function */


/* --------------------------------------------------------------------
 * NAME         RTL_malloc
 * PURPOSE      Allow to call the ordinary malloc from IMAL.
 * ARGUMENTS    IN SIZE_T size - Block size.
 * RETURNS      Ptr to the new allocated block.
 *              NULL on failure.
 * EXTERNS      None
 * NOTE         None
 ---------------------------------------------------------------------*/

VPTR RTL_malloc(size)
IN SIZE_T size;
  {
  if (size == 0)
    return NULL;
  return malloc(size);
  } /* end of RTL_malloc function */

/* --------------------------------------------------------------------
 * NAME         RTL_calloc
 * PURPOSE      Allow to call the ordinary calloc from IMAL.
 * ARGUMENTS    IN SIZE_T items - Items number.
                IN SIZE_T size - Item size.
 * RETURNS      Ptr to the new allocated block.
 *              NULL on failure.
 * EXTERNS      None
 * NOTE         None
 ---------------------------------------------------------------------*/

VPTR RTL_calloc(items, size)
IN SIZE_T items;
IN SIZE_T size;
  {
  if (items == 0 || size == 0)
    return NULL;
  return calloc(items, size);
  } /* end of RTL_calloc function */

/* --------------------------------------------------------------------
 * NAME         RTL_free
 * PURPOSE      Allow to call the ordinary free() from IMAL.
 * ARGUMENTS    IN VPTR ptr - Pointer to the block allocated by
 *                malloc() / calloc() RTL_malloc() / RTL_calloc()
 * RETURNS      MEM_OK if ptr is not NULL
 *              MEM_ERR_NULL_PTR if ptr is NULL
 * EXTERNS      None
 * NOTE         None
 ---------------------------------------------------------------------*/

int RTL_free(ptr)
IN VPTR ptr;
  {
  if (ptr == NULL)
    return MEM_ERR_NULL_PTR;
  free(ptr);
  return MEM_OK;
  } /* end of RTL_free function */

/* --------------------------------------------------------------------
 * NAME         RTL_realloc
 * PURPOSE      Allow to resize block by ordinary realloc.
 * ARGUMENTS    IN VPTR ptr - Pointer to the block allocated by
 *                malloc() / calloc() RTL_malloc() / RTL_calloc()
 *                or NULL.
 *              IN SIZE_T size - New block size.
 * RETURNS      Ptr to the new allocated block.
 *              NULL on failure or if size is equal to 0.
 * EXTERNS      None
 * NOTE         None
 ---------------------------------------------------------------------*/

VPTR RTL_realloc(ptr, size)
IN VPTR ptr;
IN SIZE_T size;
  {
  if (ptr != NULL)
    {
    if (size != 0)
      return realloc(ptr, size);
    free(ptr);
    }
  else
    {
    if (size != 0)
      return malloc(size);
    }
  return NULL;
  } /* end of RTL_realloc function */

/* --------------------------------------------------------------------
 * NAME         RTL_strdup
 * PURPOSE      Allow to call the ordinary free() from IMAL.
 * ARGUMENTS    IN char *str - String terminated by zero.
 * RETURNS      Copy of string str / NULL on failure.
 * EXTERNS      None
 * NOTE         None
 ---------------------------------------------------------------------*/

char *RTL_strdup(str)
IN char *str;
  {
  char *s;

  if (str == NULL)
    return NULL;
  if ((s = (char*) malloc(strlen(str) + 1)) == NULL)
    return NULL;
  strcpy(s, str);
  return s;
  } /* end of RTL_strdup function */

/* --------------------------------------------------------------------
 * NAME         imal_default_name
 * PURPOSE      Function imal_default_name makes  block name from
 *              source    file    name     and    line    number:
 *              "<source_file_name>:<line_number>"
 *
 * ARGUMENTS    IN char *file    - Source file name.
 *              IN unsigned int line - Source line.
 *
 * RETURNS      Ptr to the name.
 * EXTERNS      None
 * NOTE         This function is used in macros for C RTL functions.
 *              This function should be efficient. This is why I avoid
 *              library functions call in it.
 ---------------------------------------------------------------------*/

char *imal_default_name(file, line)
IN char *file;
IN unsigned int line;
  {
  char *s, *d;
  unsigned int n, nd;

  /* Copy file name to the buffer */
  s = (char*)file;
  d = imal_default_name_buf;
  while (*s)
    *d++ = *s++;
  *d++ = ':';

  /* Print line number to the buffer */
  n = line;
  for (nd = 1; n / nd >= 10; nd *= 10);
  while (nd > 0)
    {
    *d++ = (char)('0' + (int)(n / nd));
    n %= nd;
    nd /= 10;
    }
  *d = 0;
  return imal_default_name_buf;
  } /* end of imal_default_name function */


/* --------------------------------------------------------------------
 * NAME         imal_rtl_mode_set
 * PURPOSE      Set IMAL mode flags in 'rtl' module.
 * ARGUMENTS    IN int debug_mode   -  Debug mode value
 *              IN int warning_mode -  Warning mode value
 * RETURNS      None
 * EXTERNS      None
 * NOTE         None
 ---------------------------------------------------------------------*/

VOID imal_mode_rtl_set(debug_mode, warning_mode)
IN int debug_mode;      /* debug mode value */
IN int warning_mode;    /* warning mode value */
  {
  imal_mode_debug_fl = (BOOL)(debug_mode == (int)TRUE ? TRUE : FALSE);
  imal_mode_warning_fl = (BOOL)(warning_mode == (int)TRUE ? TRUE : FALSE);
  } /* end of imal_rtl_mode_set function */


/*** End of file RTL.C ***/
