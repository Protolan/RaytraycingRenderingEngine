/*
 * FILE NAME            fgets.c
 * PURPOSE              fgets() with dynamically allocated buffer.
 * AUTHOR               Eed - Eugene Denisov '07.06.07
 * COPYRIGHT            (C) INTEGRA, Inc., 2007
 */

#include <integra.h>
#include <imal.h>

static int mem_class = MEM_NOT_CLASS;
static const int CHUNK_LEN = 1024;

/* Analog of fgets() with dynamically allocated buffer.           */
/* Method replaces fgets() and reads whole line by chunks.        */
/* Parameter is input stream, equal to last parameter of fread(). */
/* Returned value can be NULL in cases when fread() would be      */
/* called, and additionaly when there is no more memory to        */
/* (re)allocate buffer.                                           */
/* Caller must free returned pointer with mem_free().             */
char *integra_fgets(FILE *stream)
  {
  char *buf, *p;
  int size = CHUNK_LEN;

  /* Create corresponding IMAL class, if not done yet */
  if (mem_class == MEM_NOT_CLASS)
    {
    mem_class = mem_class_create("Dynamic fgets()");
    if (mem_class == MEM_NOT_CLASS)
      return NULL;
    }

  /* Allocate intial buffer */
  buf = mem_alloc(mem_class, "string buffer", size);
  buf[0] = '\0';
  for ( ; ; )
    {
    /* Read next portion */
    p = fgets(buf + strlen(buf), CHUNK_LEN, stream);
    if (p == NULL || buf[strlen(buf) - 1] == '\n')
      break;

    /* String was read partially; extend buffer and read more */
    size += CHUNK_LEN;
    p = mem_realloc(buf, size);
    if (p == NULL)
      break;
    buf = p;
    }  /* for() */

  if (p == NULL && size == CHUNK_LEN)
    {
    /* The very first read attempt failed */
    mem_free(buf);
    return NULL;
    }

  return buf;
  }
