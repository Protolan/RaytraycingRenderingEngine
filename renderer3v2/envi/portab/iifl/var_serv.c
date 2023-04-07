/*
 * MODULE NAME  VAR_SERV.C
 * PURPOSE      servicing user-defined variables in image files.
 * 
 * SPEC         Z. Blaszczyk
 *              Z. Butor                              1991.07.
 * DESIGN       Z. Blaszczyk                          1991.08 02
 * DES. INSP.
 * CODING       Z. Blaszczyk                          1991.08.
 * CODE INSP.
 * TEST         Z. Blaszczyk                          1991.08.
 * LAST UPDATED
 *
 */

static char *sccsid =
"@(#)$Header: /home1/repository/envi/REPOSITORY/envi/portab/iifl/var_serv.c,v 1.3 1999/03/12 10:11:53 envi Exp $ ";

/* CONDITIONAL COMPILATION SWITCHES */

#define TEST  0  /* testing   switch */

#include "integra.h"

/* COMPANY INCLUDES */

#include "iifl.h"

/* PROGRAM INCLUDES */

#include "iifl_own.h"

/* CONSTANTS */

/* MACROS    */
 
/* STATICS   */

/* INTERFACE FUNCTIONS PROTOTYPES

int     iif_put_var(IIF*, char*, char*);
char   *iif_get_var(IIF*, char*);
char   *iif_get_first_var(IIF*);
char   *iif_get_next_var(IIF*);
int     iif_copy_vars(IIF*, IIF*);
int     iif_put_data(IIF*, char*, BYTE*, unsigned int);
BYTE   *iif_get_data(IIF*, char*, unsigned short int*);
*/

/* LOCAL FUNCTIONS PROTOTYPES */

#ifndef NO_PROTOTYPES

static int    put_var(IIFL*, char*, char*, int, unsigned int, int, BOOL*);

#else

static int    put_var();
static VLIST *find_var_name();

#endif

/*************************** CODE ******************************/

/*==================== PUBLIC FUNCTIONS =============================*/

/* --------------------------------------------------------------------------
 * NAME         iif_put_var
 * PURPOSE      store string pointed by value in variable identified
 *              by name.
 * ARGUMENTS    ifp          a pointer to opened IIF file
 *              name         variabe name
 *              value        value of variable
 * RETURNS      Error code
 * EXTERNS      None
 --------------------------------------------------------------------------*/
int iif_put_var(iflp, name, value)
IIF  *iflp;                                  /* a pointer to IIF structure */
char *name, *value;                          /* a pointers to strings with */
                                             /* variable name and value    */
  {
  BOOL overwritten;
  int nam_len, val_len, err;
  IIFL  *ifp;                               /* a pointer to IIFL structure */

  ifp = (IIFL *)iflp;

  if (fl_check_iifstr_ptr(ifp) != IIF_OK)
    {
    IIF_errno = IIFERR_BAD_PTR;
    return IIFERR_BAD_PTR;
    }

  if (name == NULL || value == NULL)
    {
    ifp->error = IIFERR_BAD_PARMS;
    return IIFERR_BAD_PARMS;
    }

  nam_len = strlen(name) + 1;
  val_len = strlen(value) + 1;
  if (nam_len > NAM_SIZE_LIMIT || val_len > VAR_SIZE_LIMIT)
    {
    ifp->error = IIFERR_VAR_TOO_BIG;
    return IIFERR_VAR_TOO_BIG;
    }

  if (ifp->attrib == 'r')
    {
    ifp->error = IIFERR_BAD_MODE;
    return IIFERR_BAD_MODE;
    }

  if (ifp->initialized == 0)
    {
    ifp->error = IIFERR_NOT_INIT;
    return IIFERR_NOT_INIT;
    }

  if (fl_check_registered_names(name) == TRUE)
    {
    ifp->error = IIFERR_RES_VAR;
    return IIFERR_RES_VAR;
    }

  if ((err = put_var(ifp, name, value, nam_len, (unsigned int)val_len,
      STRING_VAR, &overwritten)) != IIF_OK)
    {
    ifp->error = err;
    return err;
    }

  if (overwritten == FALSE)
    return IIF_OK;
  else
    return IIFWARN_OVERWRTN;
  }

/* --------------------------------------------------------------------------
 * NAME         iif_get_var
 * PURPOSE      returns pointer to a string assigned to variable named
 *              'name'.
 * ARGUMENTS    ifp          a pointer to opened IIF file
 *              name         variabe name
 * RETURNS      a pointer to buffer with "value" string
 * EXTERNS      None
 --------------------------------------------------------------------------*/
char *iif_get_var(iflp, name)
IIF  *iflp;                                  /* a pointer to IIF structure */
char *name;                                  /* a pointer to string   with */
                                             /* variable name              */
  {
  VLIST *tmp_ptr;
  IIFL  *ifp;                               /* a pointer to IIFL structure */

  ifp = (IIFL *)iflp;

  if (fl_check_iifstr_ptr(ifp) != IIF_OK)
    {
    IIF_errno = IIFERR_BAD_PTR;
    return NULL;
    }

  if (name == NULL)
    {
    ifp->error = IIFERR_BAD_PARMS;
    return NULL;
    }

  if (ifp->initialized == 0)
    {
    ifp->error = IIFERR_NOT_INIT;
    return  NULL;
    }

  if ((tmp_ptr = find_var_name(ifp->syst_var, name)) == NULL &&
      (tmp_ptr = find_var_name(ifp->first_var, name)) == NULL)
    {
    ifp->error = IIFERR_NOT_A_VAR;
    return NULL;
    }
  else
   {
   if (tmp_ptr->type == (BYTE)STRING_VAR)
     {
     ifp->curr_var = tmp_ptr;
     return tmp_ptr->value;
     }
   else
     {
     ifp->error = IIFWARN_BINARY_DATA;
     return NULL;
     }
   }
  }

/* --------------------------------------------------------------------------
 * NAME         iif_get_first_var
 * PURPOSE      returns pointer to a string assigned to variable name
 *              which is first of defined variables in given file.
 * ARGUMENTS    ifp          a pointer to opened IIF file
 * RETURNS      a pointer to buffer with "name" string
 * EXTERNS      None
 --------------------------------------------------------------------------*/
char *iif_get_first_var(iflp)
IIF  *iflp;                                  /* a pointer to IIF structure */
  {
  IIFL  *ifp;                               /* a pointer to IIFL structure */

  ifp = (IIFL *)iflp;
  if (fl_check_iifstr_ptr(ifp) != IIF_OK)
    {
    IIF_errno = IIFERR_BAD_PTR;
    return NULL;
    }

  if (ifp->initialized == 0)
    {
    ifp->error = IIFERR_NOT_INIT;
    return  NULL;
    }

  if (ifp->first_var == NULL)
    return NULL;
  else
    {
    ifp->curr_var = ifp->first_var;
    return ifp->first_var->name;
    }
  }

/*--------------------------------------------------------------------------
 * NAME         iif_get_next_var
 * PURPOSE      returns pointer to a string assigned to variable name
 *              which is next of previously accessed variable in given
 *              file.
 * ARGUMENTS    ifp          a pointer to opened IIF file
 * RETURNS      a pointer to buffer with "name" string
 * EXTERNS      None
 --------------------------------------------------------------------------*/
char *iif_get_next_var(iflp)
IIF  *iflp;                                  /* a pointer to IIF structure */
  {
  IIFL  *ifp;                               /* a pointer to IIFL structure */

  ifp = (IIFL *)iflp;
  if (fl_check_iifstr_ptr(ifp) != IIF_OK)
    {
    IIF_errno = IIFERR_BAD_PTR;
    return NULL;
    }

  if (ifp->initialized == 0)
    {
    ifp->error = IIFERR_NOT_INIT;
    return  NULL;
    }

  if (ifp->curr_var == NULL)
    {
    if (ifp->first_var == NULL)
      return NULL;
    else
      {
      ifp->curr_var = ifp->first_var;
      return ifp->first_var->name;
      }
    }
  else
    {
    if (ifp->curr_var->next == NULL)
      {
      ifp->curr_var = NULL;
      return NULL;
      }
    else
      {
      ifp->curr_var = ifp->curr_var->next;
      return ifp->curr_var->name;
      }
    }
  }

/* --------------------------------------------------------------------------
 * NAME         iif_copy_vars
 * PURPOSE      copies all user-defined variables and their values from
 *              one file to another.
 * ARGUMENTS    dest         a pointer to opened IIF file
 *              src          a pointer to source IIF file
 * RETURNS      an error code.
 * EXTERNS      None
 --------------------------------------------------------------------------*/
int iif_copy_vars(destination, source)
IIF  *destination;              /* a pointer to IIF destination file */
IIF  *source;                   /* a pointer to IIF source file      */
  {
  VLIST *tmp_ptr;
  BOOL overwritten;
  int err;
  IIFL *dest, *src;             /* pointers to IIFL structures       */

  src = (IIFL *)source;
  dest = (IIFL *)destination;
  if ((fl_check_iifstr_ptr(dest) != IIF_OK) ||
      (fl_check_iifstr_ptr(src) != IIF_OK))
    {
    IIF_errno = IIFERR_BAD_PTR;
    return IIFERR_BAD_PTR;
    }

  if (dest->initialized == 0)
    {
    dest->error = IIFERR_NOT_INIT;
    return IIFERR_NOT_INIT;
    }

  if (src->initialized == 0)
    {
    src->error = IIFERR_NOT_INIT;
    return IIFERR_NOT_INIT;
    }

  tmp_ptr = src->first_var;
  while (tmp_ptr != NULL)
    {
    if ((err = put_var(dest, tmp_ptr->name, tmp_ptr->value,
        (int)(tmp_ptr->nam_len), (unsigned int)(tmp_ptr->val_len),
        (int)(tmp_ptr->type), &overwritten)) != IIF_OK)
      {
      dest->error = err;
      return err;
      }
    tmp_ptr = tmp_ptr->next;
    }

  if (overwritten == FALSE)
    return IIF_OK;
  else
    return IIFWARN_OVERWRTN;
  }

/* --------------------------------------------------------------------------
 * NAME         iif_put_data
 * PURPOSE      store data area pointed by value in variable identified
 *              by name.
 * ARGUMENTS    ifp          a pointer to opened IIF file
 *              name         variabe name
 *              value        value of variable
 *              len          size of data area
 * RETURNS      Error code
 * EXTERNS      None
 --------------------------------------------------------------------------*/
int iif_put_data(iflp, name, value, len)
IIF  *iflp;                                  /* a pointer to IIF structure */
char *name;                     /* a pointer to strings with variable name */
BYTE *value;                    /* a pointer to data area                  */
unsigned int len;               /* variable area length                    */
  {
  BOOL overwritten;
  int nam_len, err;
  IIFL  *ifp;                               /* a pointer to IIFL structure */

  ifp = (IIFL *)iflp;

  if (fl_check_iifstr_ptr(ifp) != IIF_OK)
    {
    IIF_errno = IIFERR_BAD_PTR;
    return IIFERR_BAD_PTR;
    }

  if (name == NULL || value == NULL)
    {
    ifp->error = IIFERR_BAD_PARMS;
    return IIFERR_BAD_PARMS;
    }

  nam_len = strlen(name) + 1;
  if (nam_len > NAM_SIZE_LIMIT || len >= DATA_BLOCK_LIMIT)
    {
    ifp->error = IIFERR_VAR_TOO_BIG;
    return IIFERR_VAR_TOO_BIG;
    }

  if (ifp->attrib == 'r')
    {
    ifp->error = IIFERR_BAD_MODE;
    return IIFERR_BAD_MODE;
    }

  if (ifp->initialized == 0)
    {
    ifp->error = IIFERR_NOT_INIT;
    return IIFERR_NOT_INIT;
    }

  if (fl_check_registered_names(name) == TRUE)
    {
    ifp->error = IIFERR_RES_VAR;
    return IIFERR_RES_VAR;
    }

  if ((err = put_var(ifp, name, (char *)value, nam_len, len, DATA_VAR,
      &overwritten)) != IIF_OK)
    {
    ifp->error = err;
    return err;
    }
  return IIF_OK;
  }

/* --------------------------------------------------------------------------
 * NAME         iif_get_data
 * PURPOSE      returns pointer to a data area assigned to variable named
 *              'name'.
 * ARGUMENTS    ifp          a pointer to opened IIF file
 *              name         variabe name
 *              len          an address of variable to set to size of area
 * RETURNS      a pointer to buffer with "value" data area
 * EXTERNS      None
 --------------------------------------------------------------------------*/
BYTE *iif_get_data(iflp, name, len)
IIF  *iflp;              /* a pointer to IIF structure                     */
char *name;              /* a pointer to string   with  variable name      */
unsigned short int *len; /* a pointer to variable to set to data area size */
  {
  VLIST *tmp_ptr;
  IIFL  *ifp;                               /* a pointer to IIFL structure */

  ifp = (IIFL *)iflp;

  if (fl_check_iifstr_ptr(ifp) != IIF_OK)
    {
    IIF_errno = IIFERR_BAD_PTR;
    return NULL;
    }

  if (name == NULL || len == NULL)
    {
    ifp->error = IIFERR_BAD_PARMS;
    return NULL;
    }

  if (ifp->initialized == 0)
    {
    ifp->error = IIFERR_NOT_INIT;
    return  NULL;
    }

  if ((tmp_ptr = find_var_name(ifp->first_var, name)) == NULL)
    {
    ifp->error = IIFERR_NOT_A_VAR;
    return NULL;
    }
  else
   {
   if (tmp_ptr->type == (BYTE)DATA_VAR)
     {
     ifp->curr_var = tmp_ptr;
     *len = tmp_ptr->val_len;
     return (BYTE *)(tmp_ptr->value);
     }
   else
     {
     ifp->error = IIFWARN_STRING_DATA;
     return NULL;
     }
   }
  }

/* --------------------------------------------------------------------------
 * NAME         fl_check_registered_names
 * PURPOSE      checks if given string describes one of registered
 *              system variables name.
 * ARGUMENTS    name         a pointer to buffer with variabe name
 * RETURNS      TRUE - if name is one of system variables names;
 *              FALSE - if no.
 * EXTERNS      None
 --------------------------------------------------------------------------*/
BOOL  fl_check_registered_names(name)
char *name;                         /* a pointer  to string  with    */
                                    /* variable name                 */
  {
   if (strcmp(IIFVAR_FLOAT_VER, name) == 0 ||
       strcmp(IIFVAR_SYST_VER, name)  == 0 ||
       strcmp(IIFVAR_TYPES, name)     == 0 ||
       strcmp(IIFVAR_NEW_VER, name)   == 0 ||
       strcmp(IIFVAR_NEW_XRES, name)  == 0 ||
       strcmp(IIFVAR_NEW_YRES, name)  == 0)
     return TRUE;
   else
     return FALSE;
   }


/*==================== LOCAL FUNCTIONS ==============================*/

/* --------------------------------------------------------------------------
 * NAME         put_var
 * PURPOSE      store string or data area pointed by value in variable
 *              identified by name.
 * ARGUMENTS    ifp          a pointer to opened IIF file
 *              name         variabe name
 *              value        value of variable
 *              type         type of data assigned to variable 'name'
 *              overwritten  a pointer to boolean variable that signalizes
 *                           if variable was previously set.
 * RETURNS      an error code.
 * EXTERNS      None
 --------------------------------------------------------------------------*/
static int put_var(ifp, name, value, nam_len, val_len, type, overwritten)
IIFL   *ifp;                   /* a pointer to IIF structure               */
char   *name, *value;          /* a pointers to strings with               */
                               /* variable name and value                  */
int     nam_len;               /* variable name length (with \0 at the end */
unsigned int  val_len;         /* variable value length/data area size     */
int     type;                  /* shows what kind of data is to be stored  */
                               /* in variable 'name' : binary or string    */
BOOL   *overwritten;           /* a pointer to a flag; TRUE                */
                               /* if an existing variable value was        */
                               /* overwritten                              */
  {
  VLIST *tmp_ptr, *list;
  char *new_value, *new_name;

  new_value = NULL;
  new_name = NULL;
  if ((tmp_ptr = find_var_name(ifp->first_var, name)) == NULL)
    {
    *overwritten = FALSE;
    if((tmp_ptr = (VLIST *)malloc(sizeof(VLIST))) == NULL)
      {
      return IIFERR_NO_MEM;
      }
    if ((new_name = (char*)malloc(nam_len)) == NULL)
      {
      free(tmp_ptr);
      return IIFERR_NO_MEM;
      }
    }
  else
    *overwritten = TRUE;

  if ((new_value = (char*)malloc(val_len)) == NULL)
    {
    if (*overwritten == FALSE)
      {
      free(tmp_ptr);
      free(new_name);
      }
    return IIFERR_NO_MEM;
    }

  if (*overwritten == FALSE)
    {
    if (ifp->first_var == NULL)
      list = ifp->first_var = tmp_ptr;
    else  /* seek to the end of the list */
      {
      list = ifp->first_var;
      while (list->next != NULL)
        list = list->next;
      list->next = tmp_ptr;
      }

    strcpy(new_name, name);
    tmp_ptr->name = new_name;
    tmp_ptr->next = NULL;
    }
  else
    free(tmp_ptr->value);

  tmp_ptr->value = new_value;
  memcpy(new_value, value, val_len);
  tmp_ptr->type = (BYTE)type;
  tmp_ptr->val_len = (unsigned short int)val_len;
  tmp_ptr->nam_len = (BYTE)nam_len;

  if (*overwritten == TRUE)
    return IIFWARN_OVERWRTN;
  else
    return IIF_OK;
  }

/* --------------------------------------------------------------------------
 * NAME         find_var_name
 * PURPOSE      searches given variable name in variables list
 * ARGUMENTS    list         a pointer to variables list
 *              name         a pointer to buffer with variabe name
 * RETURNS      a pointer to element of the list that describes given
 *              variable.
 * EXTERNS      None
 --------------------------------------------------------------------------*/
VLIST *find_var_name(list, name)
VLIST *list;                        /* a pointer to vars list              */
char *name;                         /* a pointer  to string  with          */
                                    /* variable name                       */
  {
  while (list != NULL)
    {
    if (strcmp(list->name, name) == 0)
      return list;
    list = list->next;
    }
  return NULL;
  }

/*--------------------------- END OF MODULE -------------------------------*/
