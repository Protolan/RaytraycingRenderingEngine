/*
 * MODULE NAME  IIFL.C
 * PURPOSE      immediate access to IIF structure.
 *
 * SPEC         Z. Blaszczyk
 *              Z. Butor                              1991.07.08
 * DESIGN       Z. Blaszczyk                          1991.08 02
 * DES. INSP.
 * CODING       Z. Blaszczyk                          1991.08.10
 * CODE INSP.
 * TEST         Z. Blaszczyk                          1991.08.20
 * LAST UPDATED
 *
 */

static char *sccsid =
"@(#)/usr2/ENVI_REPOSITORY/REPOSI/ENVI/portab/iifl/iifl.c,v 1.1.1.1 1993/09/20 06:09:58 cvs Exp ";

/* CONDITIONAL COMPILATION SWITCHES */

#define TEST  0  /* testing   switch */

#define DEFINE_GLOBALS

#include "integra.h"

/* COMPANY INCLUDES */

#include "iifl.h"
#include "iosl.h"
#include "ievl.h"

/* PROGRAM INCLUDES */

#include "iifl_own.h"

/* LOCAL TYPES DEFINITIONS */

typedef struct msg_txt
    {
    int err_no;                    /* an error code                   */
    char *msg;                     /* a pointer to error message text */
    }ERR_TXT;

/* CONSTANTS */

#define MAX_NAME_LEN        255    /* maximum length of component name     */
#define MAX_ERR_MSG_LEN     255    /* maximum length of error message text */


#define  DOS_LINES_AMOUNT    10    /* default size of buffered image area  */
                                   /* (in scan-lines) for MS-DOS           */
#define  UNIX_LINES_AMOUNT  100    /* default size of buffered image area  */
                                   /* (in scan-lines) for UNIX             */
#define  ENV_LINES      "lines:"   /* a string that should be assigned to  */
                                   /* IIFL_CFG environment variable        */

#define ENV_DEF             ':'
#define ENV_SEP             ','
#define ENV_DEF_FIELD       ":"
#define ENV_SEP_FIELD       ","
#define MAX_ENV_STRING_LEN  80


/* MACROS */

/* STATICS */

static BYTE new_files_format = IIF_FORMAT_IIFF;
static char err_msg_buff[MAX_ERR_MSG_LEN];
static int  os_name_locale_initialized = 0; /* flag; 1 if locale were initial.*/
static char IIF_locale_str_buf[MAX_ENV_STRING_LEN];
static ERR_TXT errtab[] = {
 {IIF_OK, "No error" },
 {IIFERR_BAD_DISK, "Disk error" },
 {IIFERR_DISK_FULL, "Disk full" },
 {IIFERR_EOF, "Unexpected end of file encountered!" },
 {IIFERR_NO_MEM, "Memory allocation error" },
 {IIFERR_BAD_PTR, "Improper IIF pointer used" },
 {IIFERR_BAD_FILE, "Not an image file" },
 {IIFERR_NO_FILE, "File not accessible!" },
 {IIFERR_OLD_VER, "Older image versions are not acceptable" },
 {IIFERR_BAD_FORMAT, "Unknown file format" },
 {IIFERR_BAD_COMPS, "Components don't match" },
 {IIFERR_BAD_PARMS, "Improper parameter(s) value(s) used" },
 {IIFERR_NOT_INIT, "Attempt to write to not initialized file" },
 {IIFERR_FILL_NDEF, "Fill components not defined" },
 {IIFERR_BAD_ORG, "Bad image origin or size" },
 {IIFERR_BAD_SIZE, "Bad pixel origin in image line" },
 {IIFERR_BAD_MODE, "Function not allowed in current file access mode" },
 {IIFERR_NOT_A_VAR, "Variable not found" },
 {IIFERR_RES_VAR, "Reserved variable name used" },
 {IIFERR_VAR_TOO_BIG, "Size of variable name or value exceeds allowed limit" },
 {IIFWARN_NO_SWAP, "Cannot control byte swapping in current file access mode" },
 {IIFWARN_OVERWRTN, "An existing variable has been overwritten" },
 {IIFWARN_INITIALIZED, "File has been already initialized" },
 {IIFWARN_STRING_DATA, "Variable has assigned string data" },
 {IIFWARN_BINARY_DATA, "Variable has assigned binary data" },
 {0, NULL}};

/* INTERFACE FUNCTIONS PROTOTYPES

IIF    *iif_open(char*, char*);
int     iif_close(IIF*);
int     iif_init_file(IIF*, int, int, char*, char*);
int     iif_def_components(IIF*, int, char*);
int     iif_def_fill(IIF*, VPTR);
int     iif_get_resolution(IIF*, int*, int*);
int     iif_get_components(IIF*, char**, char**);
int     iif_get_size(IIF*, BYTE**, unsigned short*);
int     iif_get_format(IIF*);
int     iif_set_format(int);
int     iif_enable_swap(IIF*);
int     iif_disable_swap(IIF*);
int     iif_get_swap(IIF*, BYTE*);
int     iif_error(IIF*);
int     iif_clrerror(IIF*);
char   *iif_strerror(IIF*);
int     iif_set_buffer(IIF*, int);
int     iif_setlocale(char*);
int     fl_check_iifstr_ptr(IIFL*);
short   fl_comps_amount(char*);
VOID    fl_get_comp_name(char*, char*, int);
int     fl_first_access(IIFL*, int);
VOID    fl_free_VLIST(VLIST*);
long    fl_get_comp_offset(IIFL*, int);
*/

/* LOCAL FUNCTIONS PROTOTYPES */

#ifndef NO_PROTOTYPES

static  IIFL  *alloc_IIF_struct(VOID);
static  VOID   free_IIF_struct(IIFL*);
static  int    set_lengths(IIFL*, char*);
static  int    alloc_lines_buffer(IIFL*);
static  char  *find_messg_err(int);
static  char  *get_file_name(char*);
static  BOOL   check_names(IIFL*, char*);
static  BYTE   get_comp_size(char*, int);
static  int    get_comp_pos(char*, char*);
static  int    get_num_of_lines(VOID);
static  int    check_allowed_types(char*);
static  int    env_atol(char *, long *);
static  int    env_atoi(char *, int *);
static  char  *ui_env_get_from(char *, char *);

#else

static  IIFL  *alloc_IIF_struct();
static  VOID   free_IIF_struct();
static  int    set_lengths();
static  int    alloc_lines_buffer();
static  char  *find_messg_err();
static  char  *get_file_name();
static  BOOL   check_names();
static  BYTE   get_comp_size();
static  int    get_comp_pos();
static  int    get_num_of_lines();
static  int    check_allowed_types();
static  int    env_atol();
static  int    env_atoi();
static  char  *ui_env_get_from();

#endif

/*************************** CODE ******************************/

/*==================== INTERFACE FUNCTIONS =============================*/

/* --------------------------------------------------------------------------
 * NAME         iif_open
 * PURPOSE      opens an image file for reading, writting or updating
 *              procces.
 * ARGUMENTS    name    a pointer to buffer with full file name.
 *              mode    shows if file is open for reading, updating or
 *                      a new file is created.
 * RETURNS      a pointer to opened IIF file.
 * EXTERNS      None
 --------------------------------------------------------------------------*/
IIF *iif_open(name, mode)
char *name;                     /* a pointer to full file name with a path */
char  *mode;                    /* a mode in which file should be opened   */
  {
  FILE *fp;                     /* a file pointer                          */
  int  res;                     /* an result code                          */
  IIFL  *ifp;                   /* a pointer to created IIFL structure     */

  if (name == NULL || mode == NULL)
    {
    IIF_errno = IIFERR_BAD_PARMS;
    return NULL;
    }

  if (*mode == 'R' || *mode == 'r')
    {
    if ((fp = os_fopen(name, "rb")) == NULL)
      {
      IIF_errno = IIFERR_NO_FILE;
      return NULL;
      }
    }
  else if (*mode == 'U' || *mode == 'u')
    {
    if ((fp = os_fopen(name, "r+b")) == NULL)
      {
      IIF_errno = IIFERR_NO_FILE;
      return NULL;
      }
    }
  else if (*mode == 'W' || *mode == 'w')
    {
    if ((fp = os_fopen(name, "wb")) == NULL)
      {
      IIF_errno = IIFERR_BAD_DISK;
      return NULL;
      }
    }
  else          /* unknown, not allowed mode */
    {
    IIF_errno = IIFERR_BAD_PARMS;
    return NULL;
    }

  if ((ifp = alloc_IIF_struct()) == NULL)
    {
    os_fclose(fp);
    IIF_errno = IIFERR_NO_MEM;
    return NULL;
    }

  ifp->file_handle = fp;
  ifp->attrib = (char)tolower((int)(*mode));

  if (*mode != 'W' && *mode != 'w')
    {
    if ((res = fl_check_file_format(ifp)) == IIFERR_BAD_FORMAT)
      {
      IIF_errno = res;
      os_fclose(ifp->file_handle);
      free_IIF_struct(ifp);
      return NULL;
      }
    else
      {
      ifp->format = (BYTE)res;
      }

    if (((res = fl_read_header(ifp))     != IIF_OK) ||
        ((res = fl_init_var_access(ifp)) != IIF_OK))
      {
      IIF_errno = res;
      os_fclose(ifp->file_handle);
      free_IIF_struct(ifp);
      return NULL;
      }

    ifp->initialized = 1;

    if (fl_is_correct_ver(ifp) == FALSE)
      {
      IIF_errno = IIFERR_OLD_VER;
      os_fclose(ifp->file_handle);
      free_IIF_struct(ifp);
      return NULL;
      }

    if (((ifp->types = fl_get_types(ifp)) == NULL) ||
        ((ifp->float_ver = fl_get_float_ver(ifp)) == BAD_FP_FORMAT) ||
        (iif_def_components((IIF *)ifp, 0, ifp->cmps) != IIF_OK))
      {
      IIF_errno = IIFERR_NO_MEM;
      os_fclose(ifp->file_handle);
      free_IIF_struct(ifp);
      return NULL;
      }
    }
  else                         /* created a new file */
    {
    if ((ifp->filename = get_file_name(name)) == NULL)
      {
      free_IIF_struct(ifp);
      IIF_errno = IIFERR_NO_MEM;
      return NULL;
      }
    ifp->initialized = 0;
    ifp->format = new_files_format;
    ifp->swap = TRUE;        /* new files always in UNIX mode! */
    }
  return (IIF *)ifp;
  }

/*--------------------------------------------------------------------------
 * NAME         iif_close
 * PURPOSE      closes previously opened image file.
 * ARGUMENTS    ifp    a pointer to IIF file.
 * RETURNS      an error code
 * EXTERNS      None
 --------------------------------------------------------------------------*/

int  iif_close(iflp)
IIF *iflp;                      /* a pointer to IIF file */
  {
  int res;                      /* a result code         */
  IIFL  *ifp;                               /* a pointer to IIFL structure */

  ifp = (IIFL *)iflp;

  if (fl_check_iifstr_ptr(ifp) != IIF_OK)
    return IIFERR_BAD_PTR;

  if (ifp->initialized == 1)
    {
    if (ifp->affected == TRUE)
      {
      if ((res = fl_write_lines(ifp, ifp->first_line, ifp->valid_lines)) !=
           IIF_OK )
        {
        ifp->error = res;
        return res;
        }
      }

    if (ifp->attrib != 'r')
      if ((res = fl_finish_var_access(ifp)) != IIF_OK)
        {
        ifp->error = res;
        return res;
        }
    }

  os_fclose(ifp->file_handle);
  (ifp->marker)[0] = '\0';
  free_IIF_struct(ifp);
  return IIF_OK;
  }

/*-------------------------------------------------------------------------
 * NAME         iif_init_file
 * PURPOSE      Set basic parameters to new created image file.
 * ARGUMENTS    ifp   a pointer to IIF image file
 *              xres  a number of columns in image.
 *              yres  a number of rows in image.
 *              cmps  a pointer to a buffer with component names separated
 *                    with commas.
 *              types a pointer to buffer with components types.
 * RETURNS      an error code.
 * EXTERNS      None
 --------------------------------------------------------------------------*/
int  iif_init_file(iflp, xres, yres, cmps, types)
IIF   *iflp;                     /* a pointer to IIF image file           */
int    xres;                     /* a number of columns in image          */
int    yres;                     /* a number of rows in image             */
char  *cmps;                     /* a pointer to a buffer with component  */
                                 /* names separated with commas           */
char  *types;                    /* a pointer to buffer with components   */
                                 /* types                                 */
  {
  int cmps_len, types_len;       /* lengths of cmps and types strings     */
  int res;                       /* a result code                         */
  int n;                         /* an iteration counter                  */
  long buff_size;                /* a lines buffer size                   */
  long line_size;                /* size of one image line in bytes       */ 
  IIFL  *ifp;                    /* a pointer to IIFL structure           */

  ifp = (IIFL *)iflp;

  if (fl_check_iifstr_ptr(ifp) != IIF_OK)
    {
    IIF_errno = IIFERR_BAD_PTR;
    return IIFERR_BAD_PTR;
    }

  if (xres <= 0 || yres <= 0 || cmps == NULL || types == NULL)
    {
    ifp->error = IIFERR_BAD_PARMS;
    return IIFERR_BAD_PARMS;
    }

  if (check_allowed_types(types) != IIF_OK)
    {
    ifp->error = IIFERR_BAD_COMPS;
    return IIFERR_BAD_COMPS;
    }

  if (ifp->initialized == 1)
    {
    ifp->error = IIFWARN_INITIALIZED;
    return IIFWARN_INITIALIZED;
    }

  cmps_len = strlen(cmps) + 1;
  if (cmps_len > COMPS_LEN_LIMIT)
    {
    ifp->error = IIFERR_BAD_COMPS;
    return IIFERR_BAD_COMPS;
    }

  if ((ifp->cmps = (char *)malloc(cmps_len)) == NULL)
    {
    ifp->error = IIFERR_NO_MEM;
    return IIFERR_NO_MEM;
    }
  types_len = strlen(types) + 1;
  if ((ifp->types = (char *)malloc(types_len)) == NULL)
    {
    free(ifp->cmps);
    ifp->error = IIFERR_NO_MEM;
    return IIFERR_NO_MEM;
    }

  ifp->xres = xres;
  ifp->yres = yres;
  strcpy(ifp->cmps, cmps);
  strcpy(ifp->types, types);
  ifp->cmps_number = fl_comps_amount(cmps);

  if (((res = set_lengths(ifp, types)) != IIF_OK) ||
      ((res = fl_write_header(ifp)) != IIF_OK)       ||
      ((res = iif_def_components((IIF *)ifp, 0, cmps)) != IIF_OK))
    {
    free(ifp->cmps);
    free(ifp->types);
    ifp->error = res;
    return res;
    }

                 /* calculate image size buffer */
  line_size = 0;
  for (n = 0; n < (int)(ifp->cmps_number); n++)
    {
    line_size += (long)(ifp->xres * (*(ifp->lcmps + n)));
    }
  buff_size = line_size * ifp->num_of_lines;
#if NO_64KB_LIMIT == 0
  if (buff_size >= MAX_ARRAY_SIZE)
    {
    ifp->num_of_lines = (int)(MAX_ARRAY_SIZE / (int)line_size);
    ifp->num_of_lines -= 1;
    buff_size = line_size * ifp->num_of_lines;
    }
#endif
  ifp->buffer_size = buff_size;

  ifp->initialized = 1;
  return IIF_OK;
  }

/*--------------------------------------------------------------------------
 * NAME         iif_def_components
 * PURPOSE      Creates a new subset of components for image file which will
 *              be obligatory for all subsequent accesses to image buffer.
 * ARGUMENTS    ifp   a pointer to IIF image file
 *              mode  shows order of components buffers on user's side:
 *                    0 - data buffers are in the same order and offsets as
 *                        in the image file;
 *                    1 - data buffers are in the same order as shown in
 *                         'cmps' buffer, without any offsets between them.
 *              cmps  a pointer to a buffer with component names separated
 *                    with commas.
 * RETURNS      an error code.
 * EXTERNS      None
 --------------------------------------------------------------------------*/
int  iif_def_components(iflp, mode, cmps)
IIF   *iflp;          /* a pointer to IIF image file                      */
int    mode;          /* shows order of components buffers on user's side */

char  *cmps;          /* a pointer to a buffer with component             */
                      /* names separated with commas                      */
  {
  unsigned short amount;         /* amount of components in 'cmps' buffer */
  int  n, num;                                 /* component order numbers */
  char buff[MAX_NAME_LEN];                 /* a buffer for component name */
  IIFL  *ifp;                              /* a pointer to IIFL structure */

  ifp = (IIFL *)iflp;

  if (fl_check_iifstr_ptr(ifp) != IIF_OK)
    {
    IIF_errno = IIFERR_BAD_PTR;
    return IIFERR_BAD_PTR;
    }

  if (mode != 0 && mode != 1)
    {
    ifp->error = IIFERR_BAD_PARMS;
    return IIFERR_BAD_PARMS;
    }

  if (cmps == NULL)
    {
    ifp->error = IIFERR_BAD_PARMS;
    return IIFERR_BAD_PARMS;
    }

  if (check_names(ifp, cmps) == FALSE)
    {
    ifp->error = IIFERR_BAD_COMPS;
    return IIFERR_BAD_COMPS;
    }

  amount = fl_comps_amount(cmps);

  if (ifp->def_cmps_types == NULL)   /* a first call for given file */
    {
    if (((ifp->def_cmps_sizes = (BYTE*) malloc(amount * sizeof(BYTE))) ==
                                                                       NULL) ||
        ((ifp->def_cmps_types = (char*) malloc(amount * sizeof(char))) ==
                                                                       NULL) ||
        ((ifp->def_cmps_position = (int*)  malloc(amount * sizeof(int)))  ==
                                                                       NULL) ||
        ((ifp->def_cmps_offsets  = (long*) malloc(amount * sizeof(long))) ==
                                                                       NULL))
      {
      if (ifp->def_cmps_sizes != NULL)
        {
        free(ifp->def_cmps_sizes);
        ifp->def_cmps_sizes = NULL;
        }
      if (ifp->def_cmps_types != NULL)
        {
        free(ifp->def_cmps_types);
        ifp->def_cmps_types = NULL;
        }
      if (ifp->def_cmps_position != NULL)
        {
        free(ifp->def_cmps_position);
        ifp->def_cmps_position = NULL;
        }
      ifp->error = IIFERR_NO_MEM;
      return IIFERR_NO_MEM;
      }
   }

  for (n = 0; n < (int)amount; n++)
    {
    fl_get_comp_name(buff, cmps, n);
    num = get_comp_pos(ifp->cmps, buff);
    *(ifp->def_cmps_position + n) = (mode == 0) ? num : n;
    *(ifp->def_cmps_types + n) = *(ifp->types + num);
    *(ifp->def_cmps_sizes + n) = get_comp_size(ifp->types, num);
    *(ifp->def_cmps_offsets +  n) = fl_get_comp_offset(ifp, num);
    }

  ifp->def_cmps_amount = amount;
  return IIF_OK;
  }

/*--------------------------------------------------------------------------
 * NAME         iif_def_fill
 * PURPOSE      defines fill values for all components for given IIF file.
 * ARGUMENTS    ifp          a pointer to IIF file.
 *              cmps_val     a pointer to table with pointers to fill
 *                           values.
 * RETURNS      an error code.
 * EXTERNS      None
 --------------------------------------------------------------------------*/
int iif_def_fill(iflp, cmps_val)
IIF   *iflp;           /* a pointer to IIF file                           */
VPTR   cmps_val;       /* a pointer to table with pointers to fill values */
  {
  IIFL  *ifp;          /* a pointer to IIFL structure                     */

  ifp = (IIFL *)iflp;
  if (fl_check_iifstr_ptr(ifp) != IIF_OK)
    {
    IIF_errno = IIFERR_BAD_PTR;
    return IIFERR_BAD_PTR;
    }

  if (ifp->attrib == 'r')
    {
    ifp->error = IIFERR_BAD_MODE;
    return IIFERR_BAD_MODE;
    }

  if (cmps_val == NULL)
    {
    ifp->error = IIFERR_BAD_PARMS;
    return IIFERR_BAD_PARMS;
    }

  ifp->fill_val = cmps_val;
  return IIF_OK;
  }

/*--------------------------------------------------------------------------
 * NAME         iif_get_components
 * PURPOSE      function allocates two buffers for component names and types
 *              fills these buffers with proper values and sets pointers to
 *              addresses of  buffers.
 * ARGUMENTS    iflp         a pointer to IIF file.
 *              cmps         a pointer to character pointer which will be set
 *                           to an address of allocated buffer for comps names
 *              types        a pointer to character pointer which will be set
 *                           to an address of allocated buffer for comps types
 * RETURNS      an error code.
 * EXTERNS      None
 --------------------------------------------------------------------------*/
int iif_get_components(iflp, cmps, types)
IIF   *iflp;      /* a pointer to IIF file */
char **cmps;      /* a pointer to character pointer which will be set  */
                  /* to an address of allocated buffer for comps names */
char **types;     /* a pointer to character pointer which will be set  */
                  /* to an address of allocated buffer for comps types */
  {
  IIFL  *ifp;     /* a pointer to IIFL structure                       */

  ifp = (IIFL *)iflp;
  if (fl_check_iifstr_ptr(ifp) != IIF_OK)
    {
    IIF_errno = IIFERR_BAD_PTR;
    return IIFERR_BAD_PTR;
    }

  if (ifp->initialized == 0)
    {
    ifp->error = IIFERR_NOT_INIT;
    return IIFERR_NOT_INIT;
    }

  if (cmps == NULL || types == NULL)
    {
    ifp->error = IIFERR_BAD_PARMS;
    return IIFERR_BAD_PARMS;
    }

  if (((*cmps  = (char*)malloc(strlen(ifp->cmps)  + 1)) == NULL) ||
      ((*types = (char*)malloc(strlen(ifp->types) + 1)) == NULL))
    {
    if (*cmps != NULL)
      {
      free(*cmps);
      *cmps = NULL;
      }
    ifp->error = IIFERR_NO_MEM;
    return IIFERR_NO_MEM;
    }

  strcpy(*cmps, ifp->cmps);
  strcpy(*types, ifp->types);
  return IIF_OK;
  }

/*--------------------------------------------------------------------------
 * NAME         iif_get_resolution
 * PURPOSE      function retrieves amount of columns and rows of image from
 *              IIF structure fields.
 * ARGUMENTS    ifp          a pointer to IIF file.
 *              x            a pointer to variable that will be set to amount
 *                           of columns.
 *              y            a pointer to variable that will be set to amount
 *                           of rows.
 * RETURNS      an error code.
 * EXTERNS      None
 --------------------------------------------------------------------------*/
int iif_get_resolution(iflp, x, y)
IIF  *iflp;            /* a pointer to IIF file.                           */
int  *x;               /* a pointer to variable that will be set to amount */
                       /* of columns.                                      */
int  *y;               /* a pointer to variable that will be set to amount */
                       /* of rows.                                         */
  {
  IIFL  *ifp;         /* a pointer to IIFL structure                       */

  ifp = (IIFL *)iflp;
  if (fl_check_iifstr_ptr(ifp) != IIF_OK)
    {
    IIF_errno = IIFERR_BAD_PTR;
    return IIFERR_BAD_PTR;
    }

  if (x == NULL || y == NULL)
    {
    ifp->error = IIFERR_BAD_PARMS;
    return IIFERR_BAD_PARMS;
    }

  if (ifp->initialized == 0)
    {
    ifp->error = IIFERR_NOT_INIT;
    return IIFERR_NOT_INIT;
    }

  *x = ifp->xres;
  *y = ifp->yres;
  return IIF_OK;
  }

/*--------------------------------------------------------------------------
 * NAME         iif_get_size
 * PURPOSE      function retrieves amount of components per pixel and lengths
 *              of each component.
 * ARGUMENTS    ifp          a pointer to IIF file.
 *              n_bytes      a pointer to BYTE pointer that will be set to
 *                           address of buffer with component lengths.
 *              n_comps      a pointer to variable that will be set to amount
 *                           of components per pixel data.
 * RETURNS      an error code.
 * EXTERNS      None
 --------------------------------------------------------------------------*/
int iif_get_size(iflp, n_bytes, n_comps)
IIF    *iflp;          /* a pointer to IIF file.                           */
BYTE  **n_bytes;       /* a pointer to BYTE pointer that will be set to    */
                       /* address of buffer with component lengths.        */
unsigned short *n_comps; /* a pointer to variable that will be set to amount */
                       /* of components per pixel data.                    */
  {
  IIFL  *ifp;          /* a pointer to IIFL structure                      */

  ifp = (IIFL *)iflp;
  if (fl_check_iifstr_ptr(ifp) != IIF_OK)
    {
    IIF_errno = IIFERR_BAD_PTR;
    return IIFERR_BAD_PTR;
    }

  if (ifp->initialized == 0)
    {
    ifp->error = IIFERR_NOT_INIT;
    return IIFERR_NOT_INIT;
    }

  if (n_bytes == NULL || n_comps == NULL)
    {
    ifp->error = IIFERR_BAD_PARMS;
    return IIFERR_BAD_PARMS;
    }

  if ((*n_bytes = (BYTE *)malloc(ifp->cmps_number * sizeof(BYTE))) == NULL)
    {
    ifp->error = IIFERR_NO_MEM;
    return IIFERR_NO_MEM;
    }

  memcpy(*n_bytes, ifp->lcmps, ifp->cmps_number);
  *n_comps = ifp->cmps_number;
  return IIF_OK;
  }

/*--------------------------------------------------------------------------
 * NAME         iif_get_format
 * PURPOSE      function gets format of image file, i.e. IIF_FORMAT_IIFF,
 *              IIF_FORMAT_TIFF, IIF_FORMAT_GIF.
 * ARGUMENTS    ifp          a pointer to IIF file.
 * RETURNS      retrieved file format, or error code - if bad ifp used.
 * EXTERNS      None
 --------------------------------------------------------------------------*/
int iif_get_format(iflp)
IIF  *iflp;                                /* a pointer to opened IIF file */
  {
  IIFL  *ifp;                              /* a pointer to IIFL structure  */

  ifp = (IIFL *)iflp;
  if (fl_check_iifstr_ptr(ifp) != IIF_OK)
    {
    IIF_errno = IIFERR_BAD_PTR;
    return IIFERR_BAD_PTR;
    }

  return (int)(ifp->format);
  }

/*--------------------------------------------------------------------------
 * NAME         iif_set_format
 * PURPOSE      function sets one of allowed formats, i.e. IIF_FORMAT_IIFF,
 *              IIF_FORMAT_TIFF, IIF_FORMAT_GIF for all subsequent created
 *              files.
 * ARGUMENTS    format       required format.
 * RETURNS      error code.
 * EXTERNS      None
 --------------------------------------------------------------------------*/
int iif_set_format(format)
int   format;               /* an output format for all new created files */
  {
  if (format != IIF_FORMAT_IIFF &&
      format != IIF_FORMAT_TIFF &&
      format != IIF_FORMAT_GIF)
    {
    IIF_errno = IIFERR_BAD_FORMAT;
    return IIFERR_BAD_FORMAT;
    }

  new_files_format = (BYTE)format;
  return IIF_OK;
  }

/*--------------------------------------------------------------------------
 * NAME         iif_set_buffer
 * PURPOSE      function changes size of image buffer to 'lines' size.
 * ARGUMENTS    ifp          a pointer to IIF file.
 *              lines        required size of buffer in scan-lines.
 * RETURNS      size of newly set buffer (in lines). Function returns 0 if
 *              failed.
 * EXTERNS      None
 --------------------------------------------------------------------------*/
int iif_set_buffer(iflp, lines)
IIF  *iflp;               /* a pointer to IIF file.                         */
int   lines;              /* required size of buffer in scan-lines.         */
  {
  int old_num;            /* current buffer size in lines                   */
  int new_num;            /* new buffer size in lines                       */
  unsigned long bytes_per_line; /* size of one line in bytes                */
  unsigned long buf_size;      /* size of new buffer                        */
  BOOL increase;          /* TRUE if new buf size is greater than previous  */
  IIFL  *ifp;             /* a pointer to IIFL structure                    */
  int err;                      /* an error code                            */

  ifp = (IIFL *)iflp;

  if (fl_check_iifstr_ptr(ifp) != IIF_OK)
    {
    IIF_errno = IIFERR_BAD_PTR;
    return 0;
    }

  if (lines <= 0)
    {
    ifp->error = IIFERR_BAD_PARMS;
    return 0;
    }

  if (ifp->initialized == 0)
    {
    ifp->error = IIFERR_NOT_INIT;
    return 0;
    }

  if (ifp->buffer == NULL)  /* buffer was not allocated yet; simply         */
    {                       /* calculate new size of buffer and do nothing  */
    new_num = lines;
    bytes_per_line = (unsigned long)(ifp->buffer_size / ifp->num_of_lines);
    buf_size = new_num * bytes_per_line;
#if NO_64KB_LIMIT == 0
    if (buf_size >= MAX_ARRAY_SIZE)
      {
      new_num = (int)(MAX_ARRAY_SIZE / (int)bytes_per_line);
      new_num--;
      buf_size = new_num * bytes_per_line;
      }
#endif
    ifp->num_of_lines = new_num;
    ifp->buffer_size = buf_size;
    return new_num;
    }

  if (ifp->num_of_lines == lines)
    return lines;                    /* the same amount of lines in buffer */

  if (ifp->affected == TRUE)
    {
    if ((err = fl_write_lines(ifp, ifp->first_line, ifp->valid_lines)) !=
        IIF_OK)
      {
      ifp->error = err;
      return 0;
      }
    }

  old_num = ifp->num_of_lines;
  new_num = lines;
  bytes_per_line = (unsigned long)(ifp->buffer_size / old_num);
  increase = (BOOL)((new_num > old_num) ? TRUE : FALSE);
  buf_size = new_num * bytes_per_line;
#if NO_64KB_LIMIT == 0
  if (buf_size >= MAX_ARRAY_SIZE)
    {
    ifp->error = IIFERR_NO_MEM;
    return old_num;
    }
#endif
  free(ifp->buffer);

  if ((ifp->buffer = (VPTR)malloc(
#if NO_64KB_LIMIT == 0
                                  (unsigned int)
#endif
                                                buf_size)) == NULL)
    {
    ifp->error = IIFERR_NO_MEM;
    if (increase == FALSE)
      {
      return 0;
      }
    else          /* try to allocate a mid-size buffer */
      {
      new_num = (old_num + lines) / 2;
      buf_size = new_num * bytes_per_line;

      if ((ifp->buffer = (VPTR)malloc(
#if NO_64KB_LIMIT == 0
                                      (unsigned int)
#endif
                                                    buf_size)) == NULL)
        {         /* try to allocate a buffer of previous size */
        new_num = old_num;
        buf_size = new_num * bytes_per_line;

        if ((ifp->buffer = (VPTR)malloc(
#if NO_64KB_LIMIT == 0
                                        (unsigned int)
#endif
                                                      buf_size)) == NULL)
           return 0;
        }
      }
    }

  ifp->buffer_size = buf_size;
  ifp->num_of_lines = new_num;
  ifp->valid_lines = (ifp->first_line + new_num <= ifp->yres) ?
                      new_num : ifp->yres - ifp->first_line;

  if ((err = fl_read_lines(ifp, ifp->first_line, ifp->valid_lines)) != IIF_OK)
    {
    ifp->error = err;
    return 0;
    }

  return new_num;
  }

/* --------------------------------------------------------------------------
 * NAME         iif_enable_swap
 * PURPOSE      enables byte swapping for given file.
 * ARGUMENTS    ifp     a pointer to IIF file.
 * RETURNS      an error code
 * EXTERNS      None
 --------------------------------------------------------------------------*/
int  iif_enable_swap(iflp)
IIF  *iflp;                                     /* a pointer to image file */
  {
  IIFL  *ifp;                               /* a pointer to IIFL structure */

  ifp = (IIFL *)iflp;
  if (fl_check_iifstr_ptr(ifp) != IIF_OK)
    {
    IIF_errno = IIFERR_BAD_PTR;
    return IIFERR_BAD_PTR;
    }

  if (ifp->attrib != 'r')
    {
    ifp->error = IIFWARN_NO_SWAP;
    return IIFWARN_NO_SWAP;
    }

  ifp->enable_swap = TRUE;
  return IIF_OK;
  }

/* --------------------------------------------------------------------------
 * NAME         iif_disable_swap
 * PURPOSE      disables byte swapping for given file.
 * ARGUMENTS    ifp     a pointer to IIF file.
 * RETURNS      an error code
 * EXTERNS      None
 --------------------------------------------------------------------------*/
int  iif_disable_swap(iflp)
IIF  *iflp;                                     /* a pointer to image file */
  {
  IIFL  *ifp;                               /* a pointer to IIFL structure */

  ifp = (IIFL *)iflp;
  if (fl_check_iifstr_ptr(ifp) != IIF_OK)
    {
    IIF_errno = IIFERR_BAD_PTR;
    return IIFERR_BAD_PTR;
    }

  if (ifp->attrib != 'r')
    {
    ifp->error = IIFWARN_NO_SWAP;
    return IIFWARN_NO_SWAP;
    }

  ifp->enable_swap = FALSE;
  return IIF_OK;
  }

/* --------------------------------------------------------------------------
 * NAME         iif_get_swap
 * PURPOSE      retrieves byte swapping flag for given file.
 * ARGUMENTS    ifp     a pointer to IIF file.
 *              swap    a pointer to variable which will be set to proper value
 * RETURNS      an error code
 * EXTERNS      None
--------------------------------------------------------------------------*/
int  iif_get_swap(iflp, swap)
IIF  *iflp;    /* a pointer to IIF file.                                  */
BYTE *swap;    /* a pointer to variable which will be set to proper value */
  {
  IIFL  *ifp;                              /* a pointer to IIFL structure */

  ifp = (IIFL *)iflp;
  if (fl_check_iifstr_ptr(ifp) != IIF_OK)
    {
    IIF_errno = IIFERR_BAD_PTR;
    return IIFERR_BAD_PTR;
    }

  if (ifp->initialized != 1)
    {
    ifp->error = IIFERR_NOT_INIT;
    return IIFERR_NOT_INIT;
    }

  if (swap == NULL)
    {
    ifp->error = IIFERR_BAD_PARMS;
    return IIFERR_BAD_PARMS;
    }

  *swap = (BYTE)((ifp->swap == TRUE) ? 1 : 0);
  return IIF_OK;
  }

/* --------------------------------------------------------------------------
 * NAME         iif_error
 * PURPOSE      retrieves error flag for given file, or IIF_errno value;
 * ARGUMENTS    ifp     a pointer to IIF file.
 * RETURNS      an error code
 * EXTERNS      None
---------------------------------------------------------------------------*/
int iif_error(iflp)
IIF  *iflp;                                       /* a pointer to IIF file */
  {
  IIFL  *ifp;                               /* a pointer to IIFL structure */

  ifp = (IIFL *)iflp;
  if (ifp == NULL)
    return IIF_errno;

  if (fl_check_iifstr_ptr(ifp) != IIF_OK)
    return IIFERR_BAD_PTR;

  return ifp->error;
  }

/* --------------------------------------------------------------------------
 * NAME         iif_clrerror
 * PURPOSE      clears error flag for given file, or IIF_errno value;
 * ARGUMENTS    iflp     a pointer to IIF file.
 * RETURNS      an error code
 * EXTERNS      None
--------------------------------------------------------------------------*/
int iif_clrerror(iflp)
IIF  *iflp;                                      /* a pointer to IIF file */
  {
  IIFL  *ifp;                               /* a pointer to IIFL structure */

  ifp = (IIFL *)iflp;
  if (ifp == NULL)
    {
    IIF_errno = IIF_OK;
    return IIF_OK;
    }

  if (fl_check_iifstr_ptr(ifp) != IIF_OK)
    return IIFERR_BAD_PTR;

  ifp->error = IIF_OK;
  return IIF_OK;
  }

/* --------------------------------------------------------------------------
 * NAME         iif_strerror
 * PURPOSE      returns a pointer to error message text according to error
 *              number that occured in given file, or to IIF_errno value;
 * ARGUMENTS    iflp     a pointer to IIF file.
 * RETURNS      a pointer to buffer with message text.
 * EXTERNS      None
--------------------------------------------------------------------------*/
char *iif_strerror(iflp)
IIF  *iflp;                                    /*  a pointer to IIF file */
  {
  int err;                                       /* "temporary" error flag */
  IIFL  *ifp;                               /* a pointer to IIFL structure */
  char * messg;                           /* a pointer  to returned string */

  ifp = (IIFL *)iflp;

  if (ifp == NULL)
    err = IIF_errno;     /* a call with NULL applies to IIF_errno variable */
  else
    {
    if (fl_check_iifstr_ptr(ifp) != IIF_OK)
      return NULL;
    err = ifp->error;
    }
  if ((messg = find_messg_err(err)) == NULL)    /* this should never happen */
    sprintf(err_msg_buff, "%s #%d ", "UNKNOWN Error: ", err);
  else
    strcpy(err_msg_buff, messg);
  return err_msg_buff;
  }

/* --------------------------------------------------------------------------
 * NAME         iif_setlocale
 * PURPOSE      sets new values to following IIFL locales:
 *              IIF_os_mark
 *              IIF_os_name
 *              IIF_os_byte_order
 *              IIF_os_fp_format
 *
 * ARGUMENTS    new_settings     a pointer to buffer with string that
 *                               describes new values for locale
 * RETURNS      an error code
 * EXTERNS      None
--------------------------------------------------------------------------*/
int iif_setlocale(new_settings)
char* new_settings;               /* a pointer to buffer with string that */
                                  /* describes new values for locale      */
  {
    int  loc_byte_order;                          /* byte order flag      */
    BYTE loc_os_mark;                             /* OS mark              */
    int  loc_fp_format;                           /* floating poin format */
    char loc_os_name[20];                         /* OS name              */
    char tmpbuf[MAX_ENV_STRING_LEN + 1];          /* a temporary buffer   */  
    char *v;                      /* a pointer to buffer with field value */

  if (new_settings == NULL)
    return IIFERR_BAD_PARMS;

  if ((v = ui_env_get_from(new_settings, OS_NAME_FIELD)) == NULL)
    return IIFERR_BAD_PARMS;
  else
    strcpy(loc_os_name, v);

  if ((v = ui_env_get_from(new_settings, OS_MARK_FIELD)) == NULL)
    return IIFERR_BAD_PARMS;
  else
    loc_os_mark = (BYTE)(*v);

  v = ui_env_get_from(new_settings, OS_BYTE_ORDER_FIELD);

  if (v == NULL || env_atoi(v, &loc_byte_order))
    return IIFERR_BAD_PARMS;

  v = ui_env_get_from(new_settings, OS_FP_FORMAT_FIELD);

  if (v == NULL || env_atoi(v, &loc_fp_format))
    return IIFERR_BAD_PARMS;

               /* set IIFL locale to retrieved values */
  strcpy(IIF_os_name, loc_os_name);
  IIF_os_fp_format = (BYTE)loc_fp_format;
  IIF_os_byte_order = (BYTE)loc_byte_order;
  IIF_os_mark = loc_os_mark;

  if (os_name_locale_initialized == 0)
      os_name_locale_initialized = 1;
  strcpy(tmpbuf, new_settings);
  strcpy(IIF_locale_str_buf, tmpbuf);
  return IIF_OK;
  }

/* --------------------------------------------------------------------------
 * NAME         iif_getlocale
 * PURPOSE      gets string describing current IIFL locales:
 *              IIF_os_mark
 *              IIF_os_name
 *              IIF_os_byte_order
 *              IIF_os_fp_format
 *
 * ARGUMENTS    none
 * RETURNS      a pointer to valid string
 * EXTERNS      None
--------------------------------------------------------------------------*/
char *iif_getlocale()
  {
  char buffer[20];

  if (os_name_locale_initialized == 0)     /* in first action of IIFL, locale */
    {                                      /* IIF_os_name is initialized      */
    strcpy(IIF_os_name, OS_NAME);
    IIF_locale_str_buf[0] = '\0';
    os_name_locale_initialized = 1;
    }

  if (IIF_locale_str_buf[0] != '\0')  /* after valid call to iif_setlocale,  */
    return IIF_locale_str_buf;        /* this buffer already contains proper */
  else                                    /* string                              */
    {
    strcpy(IIF_locale_str_buf, OS_NAME_FIELD);
    strcat(IIF_locale_str_buf, ENV_DEF_FIELD);
    strcat(IIF_locale_str_buf, IIF_os_name);
    strcat(IIF_locale_str_buf, ENV_SEP_FIELD);
    strcat(IIF_locale_str_buf, OS_MARK_FIELD);
    strcat(IIF_locale_str_buf, ENV_DEF_FIELD);
    buffer[0] = IIF_os_mark;
    buffer[1] = '\0';
    strcat(IIF_locale_str_buf, buffer);
    strcat(IIF_locale_str_buf, ENV_SEP_FIELD);
    strcat(IIF_locale_str_buf, OS_BYTE_ORDER_FIELD);
    sprintf(buffer, ":%d,", (int)IIF_os_byte_order);
    strcat(IIF_locale_str_buf, buffer);
    strcat(IIF_locale_str_buf, OS_FP_FORMAT_FIELD);
    sprintf(buffer, ":%d", (int)IIF_os_fp_format);
    strcat(IIF_locale_str_buf, buffer);

    return IIF_locale_str_buf;
    }
  }

/* --------------------------------------------------------------------------
 * NAME         fl_check_iifstr_ptr
 * PURPOSE      checks if passed pointer points to IIFL structure
 * ARGUMENTS    ifp     a pointer to check.
 * RETURNS      IIF_OK if ifp is correct; IIFERR_BAD_PTR - otherwise.
 * EXTERNS      None
--------------------------------------------------------------------------*/
int fl_check_iifstr_ptr(ifp)
IIFL  *ifp;                                    /*  a pointer to check    */
  {
  register char *mrk;

  if (ifp == NULL)
    return IIFERR_BAD_PTR;

  mrk = ifp->marker;
  if (mrk[0] == 'I' && mrk[1] == 'F' && mrk[2] == 'S' && mrk[3] == '\0')
    return IIF_OK;
  else
    return IIFERR_BAD_PTR;
  }

/* --------------------------------------------------------------------------
 * NAME         fl_comps_amount
 * PURPOSE      returns amount of components in given buffer
 * ARGUMENTS    comps   a pointer to buffer with components names, separated
 *                      with commas, with NULL in the end.
 * RETURNS      amount of components.
 * EXTERNS      None
--------------------------------------------------------------------------*/
unsigned short fl_comps_amount(comps)
char  *comps;                /* a pointer to buffer with components names */
  {
  short counter;              /* local counter                             */

  counter = 0;
  while (*comps != '\0')
    {
    if (*comps ==',')
        counter++;
    comps++;
    }

  counter++;
  return counter;
  }

/*--------------------------------------------------------------------------
 * NAME         fl_get_comp_name
 * PURPOSE      fills up given buffer with component name of 'num' position
 *              in components buffer.
 * ARGUMENTS    buffer  a buffer for found component name.
 *              comps   a pointer to buffer with components names, separated
 *                      with commas, with NULL in the end.
 *              num     component name position in components buffer.
 * RETURNS      None
 * EXTERNS      None
--------------------------------------------------------------------------*/
VOID  fl_get_comp_name(buffer, comps, num)
char  *buffer;     /* a buffer for found component name.           */
char  *comps;      /* a pointer to buffer with components names    */
int    num;        /* component name position in components buffer */
  {
  int counter;     /* local counter */

  counter = 0;
  while (*comps != '\0')
    {
    if (counter == num)
      {
      while (*comps != ',' && *comps != '\0')
        *buffer++ = *comps++;
      *buffer = '\0';
      return;
      }
    else
      {
      if (*comps == ',')
        counter++;
      comps++;
      }
    }
  }

/*--------------------------------------------------------------------------
 * NAME         fl_first_access
 * PURPOSE      allocates lines buffer and fills it with image data before
 *              first access to image area.
 * ARGUMENTS    ifp     a pointer to IIF file.
 *              line    first line that should be placed in buffer
 * RETURNS      error code
 * EXTERNS      None
--------------------------------------------------------------------------*/
int  fl_first_access(ifp, line)
IIFL  *ifp;       /* a pointer to IIF file.                               */
int  line;        /* number of first line that should be placed in buffer */
  {
  int err;                                              /* an error code  */

  if (alloc_lines_buffer(ifp) != IIF_OK)
    return IIFERR_NO_MEM;

  if ((ifp->changed_lines = (BYTE*)calloc(ifp->yres, 1)) == NULL)
    return IIFERR_NO_MEM;

  ifp->first_line = line;
  ifp->valid_lines = (ifp->first_line + ifp->num_of_lines <= ifp->yres) ?
                      ifp->num_of_lines : ifp->yres - ifp->first_line;

  if (ifp->attrib != 'w')
    {
    if ((err = fl_read_lines(ifp, line, ifp->valid_lines)) != IIF_OK)
      return err;
    }
  return IIF_OK;
  }

/*--------------------------------------------------------------------------
 * NAME         fl_free_VLIST
 * PURPOSE      frees all list and buffers associated with it, starting from
 *              'list' element.
 * ARGUMENTS    list    a pointer to the beginning of the list.
 * RETURNS      None
 * EXTERNS      None
--------------------------------------------------------------------------*/
VOID  fl_free_VLIST(list)
VLIST  *list;                  /* a pointer to the beginning of the list. */
  {
  VLIST  *wptr;                /* "working" pointer                       */

  while (list != NULL)
    {
    free(list->name);
    free(list->value);
    wptr = list;
    list = list->next;
    free(wptr);
    }
  }

/*--------------------------------------------------------------------------
 * NAME         fl_get_comp_offset
 * PURPOSE      returns offset in a scan-line to the first element of
 *              component number 'num'.
 * ARGUMENTS    ifp            a pointer to IIF file.
 *              num            an order number of component in a scan-line.
 * RETURNS      offset from the beginning of a scan-line to the component.
 * EXTERNS      None
--------------------------------------------------------------------------*/
long  fl_get_comp_offset(ifp, num)
IIFL  *ifp;                /* a pointer to IIF file.                      */
int   num;                 /* an order number of component in a scan-line */
  {
  int xres;                /* x resolution of image                       */
  long offs;               /* offset from the beginning of a scan-line    */
  int   n;                 /* component order number                      */

  xres = ifp->xres;
  offs = 0;
  for (n = 0; n < num; n++)
    offs += (long)(*(ifp->lcmps + n) * xres);

  return offs;
  }

/*========================= LOCAL FUNCTIONS ==============================*/

/* --------------------------------------------------------------------------
 * NAME         alloc_IIF_struct
 * PURPOSE      allocates IIF structure and initializes part of it's fields.
 *              Also initializes IIF_os_name buffer when library starts working.
 * ARGUMENTS    None
 * RETURNS      a pointer to allocated structure.
 * EXTERNS      None
 --------------------------------------------------------------------------*/
static IIFL *alloc_IIF_struct()
  {
  IIFL *ifp;                               /* a pointer to the structure */

  if (os_name_locale_initialized == 0)     /* in first action of IIFL, locale */
    {                                      /* IIF_os_name is initialized      */
    strcpy(IIF_os_name, OS_NAME);
    IIF_locale_str_buf[0] = '\0';
    os_name_locale_initialized = 1;
    }

  if ((ifp = (IIFL *)malloc(sizeof(IIFL))) == NULL)
    return NULL;

  ifp->buffer = NULL;
  ifp->cmps   = NULL;
  ifp->types  = NULL;
  ifp->lcmps  = NULL;
  ifp->fill_val = NULL;
  ifp->sub_cmps = NULL;
  ifp->def_cmps_sizes = NULL;
  ifp->def_cmps_types = NULL;
  ifp->def_cmps_position = NULL;
  ifp->def_cmps_offsets = NULL;
  ifp->syst_var = NULL;
  ifp->first_var = NULL;
  ifp->curr_var = NULL;
  ifp->filename = NULL;
  ifp->changed_lines = NULL;
  ifp->error = IIF_OK;
  ifp->affected = FALSE;
  ifp->xres = 0;
  ifp->yres = 0;
  ifp->first_line = 0;
  ifp->num_of_lines = get_num_of_lines();
  ifp->buffer_size = 0;
  ifp->valid_lines = 0;
  ifp->vars_offset = 0;
  ifp->enable_swap = TRUE;
  strcpy(ifp->marker, "IFS");
  fl_default_header_sizes(ifp);
  return ifp;
  }

 /*--------------------------------------------------------------------------
 * NAME         free_IIF_struct
 * PURPOSE      frees IIF structure and its buffers (if any).
 * ARGUMENTS    a pointer to structure to be freed.
 * RETURNS      None
 * EXTERNS      None
 --------------------------------------------------------------------------*/
static VOID free_IIF_struct(ifp)
IIFL *ifp;                                /* a pointer to the structure */
  {
  if (ifp->changed_lines != NULL)
    free(ifp->changed_lines);
  if (ifp->filename != NULL)
    free(ifp->filename);
  if (ifp->buffer != NULL)
    free(ifp->buffer);
  if (ifp->cmps   != NULL)
    free(ifp->cmps);
  if (ifp->types  != NULL)
    free(ifp->types);
  if (ifp->lcmps  != NULL)
    free(ifp->lcmps);
/*  WRONG !!! at least now...
  if (ifp->fill_val != NULL)
    free(ifp->fill_val);
*/
  if (ifp->sub_cmps != NULL)
    free(ifp->sub_cmps);
  if (ifp->def_cmps_sizes != NULL)
    free(ifp->def_cmps_sizes);
  if (ifp->def_cmps_types != NULL)
    free(ifp->def_cmps_types);
  if (ifp->def_cmps_position != NULL)
    free(ifp->def_cmps_position);
  if (ifp->def_cmps_offsets != NULL)
    free(ifp->def_cmps_offsets);
  if (ifp->syst_var != NULL)
    fl_free_VLIST(ifp->syst_var);
  if (ifp->first_var != NULL)
    fl_free_VLIST(ifp->first_var);
  free(ifp);
  }

/*--------------------------------------------------------------------------
 * NAME         set_lengths
 * PURPOSE      allocates buffer for component lengths and fills it with
 *              proper values.
 * ARGUMENTS    ifp       a pointer to IIF structure.
 *              types     a pointer to character buffer with component types
 * RETURNS      an error code
 * EXTERNS      None
 --------------------------------------------------------------------------*/
static int set_lengths(ifp, types)
IIFL   *ifp;                     /* a pointer to opened IIF file           */
char  *types;                    /* a pointer to buffer with 'comps' types */
  {
  unsigned short n;

  if ((ifp->lcmps = (BYTE *)malloc(ifp->cmps_number)) == NULL)
    return IIFERR_NO_MEM;

  for (n = 0; n < ifp->cmps_number; n++)
    (ifp->lcmps)[n] = get_comp_size(types, (int)n);

  return IIF_OK;
  }

/*--------------------------------------------------------------------------
 * NAME         alloc_lines_buffer
 * PURPOSE      allocates lines buffer in memory.
 * ARGUMENTS    ifp     a pointer to IIF file.
 * RETURNS      error code
 * EXTERNS      None
--------------------------------------------------------------------------*/
static int  alloc_lines_buffer(ifp)
IIFL  *ifp;                                     /* a pointer to IIF file. */
  {
  unsigned long bytes_per_line;               /* size of one line in bytes */
  unsigned long buf_size;                     /* size of  buffer           */
  unsigned short n;                           /* auxilliary counter        */

  bytes_per_line = 0;
  for (n = 0; n < ifp->cmps_number; n++)   /* calculate line size in bytes */
    bytes_per_line += (unsigned long)((ifp->lcmps)[n] * ifp->xres);

  buf_size = bytes_per_line * ifp->num_of_lines;

#if NO_64KB_LIMIT == 0
  if (buf_size >= MAX_ARRAY_SIZE)
    return IIFERR_NO_MEM;
#endif

  if ((ifp->buffer = (VPTR)malloc(
#if NO_64KB_LIMIT == 0
                                  (unsigned int)
#endif
                                                buf_size)) == NULL)
    return IIFERR_NO_MEM;

  ifp->buffer_size = buf_size;
  return IIF_OK;
  }

/*--------------------------------------------------------------------------
 * NAME         find_messg_err
 * PURPOSE      searches for given error code in local data  table and returns
 *              a pointer to text associated with it.
 * ARGUMENTS    error   an error code.
 * RETURNS      character pointer to error message text.
 * EXTERNS      None
--------------------------------------------------------------------------*/
static char  *find_messg_err(error)
int  error;                                          /* an error code     */
  {
  int n;                                             /* auxiliary counter */

  n = 0;
  while (errtab[n].msg != NULL)
    {
    if (errtab[n].err_no == error)
      return errtab[n].msg;
    else
      n++;
    }

  return NULL;                               /* this should never happen! */
  }

/*--------------------------------------------------------------------------
 * NAME         get_file_name
 * PURPOSE      extracts file name from given buffer.
 * ARGUMENTS    name    a pointer to buffer with full path to the file.
 * RETURNS      character pointer to retrieved file name.
 * EXTERNS      None
--------------------------------------------------------------------------*/
static char  *get_file_name(name)
char  *name;         /* a pointer to buffer with full path to the file.   */
  {
  char *name_buf;    /* a pointer to newly allocated buffer for file name */
  int len;           /* a total length of the string in 'name' buffer     */

  len = strlen(name);
  while (len > 0 && *(name + len) != PATH_SEPARATOR && *(name + len) != ':')
    len--;

  if ((name_buf = (char*)malloc(strlen(name + len) + 1)) == NULL)
    return NULL;

  strcpy(name_buf, name + len);
  return name_buf;
  }

/*--------------------------------------------------------------------------
 * NAME         check_names
 * PURPOSE      checks if component names in given buffer match with those
 *              defined in 'ifp; file.
 * ARGUMENTS    ifp      a pointer to IIF file.
 *              names    a pointer to buffer with names to check,separated
 *                       with commas.
 * RETURNS      TRUE if all names in buffer match with names defined in file,
 *              FALSE - if no.
 * EXTERNS      None
--------------------------------------------------------------------------*/
static BOOL  check_names(ifp, names)
IIFL  *ifp;                    /* a pointer to IIF file.                  */
char *names;                   /* a pointer to buffer with names to check */
  {
  unsigned short cmp_num;      /* components amount in 'names' buffer     */
  unsigned short i, k;         /* local counters                          */
  char name_buf[MAX_NAME_LEN]; /* a temporary buffer for checked name     */
  char patt_buf[MAX_NAME_LEN]; /* a temporary buffer for pattern name     */
  BOOL found;                  /* local flag; set to TRUE if names match  */

  cmp_num = fl_comps_amount(names);
  for (i = 0; i < cmp_num; i++)
    {
    fl_get_comp_name(name_buf, names, i);
    found = FALSE;
    for (k = 0; k < ifp->cmps_number; k++)
      {
      fl_get_comp_name(patt_buf, ifp->cmps, k);
      if (strcmp(patt_buf, name_buf) == 0)
        {
        found = TRUE;
        break;
        }
      }
    if (found == FALSE)
      return FALSE;
    }

  return TRUE;
  }

/*--------------------------------------------------------------------------
 * NAME         get_comp_size
 * PURPOSE      returns size (in bytes) of type for given component.
 * ARGUMENTS    ifp            a pointer to IIF file.
 *              comp_number    a number of component.
 * RETURNS      size of type of given component.
 * EXTERNS      None
--------------------------------------------------------------------------*/
static BYTE get_comp_size(types, num)
char *types;                 /* a pointer to buffer with components types */
int   num;                   /* a number of component                     */
  {
  char type;                                /* type of selected component */

  type = *(types + num);
  if (type == 'a')
    return sizeof(char);
  else if (type == 's')
    return sizeof(short int);
  else if (type == 'l')
    return sizeof(long);
  else if (type == 'b')
    return sizeof(BYTE);
  else if (type == 'f')
    return sizeof(float);
  else if (type == 'd')
    return sizeof(double);

#ifdef DEBUG
  printf(" MODULE: iifl.c FUNC.: get_comp_size; Unknown component type!\n");
#endif

  /* error ! */
  ASSERT(FALSE)
  return 0;
  }

/*--------------------------------------------------------------------------
 * NAME         get_comp_pos
 * PURPOSE      returns position of component 'name' in buffer 'comps'.
 * ARGUMENTS    comps          a pointer to buffer with component names.
 *              name           a name of this component
 * RETURNS      position of given component in buffer.
 * EXTERNS      None
--------------------------------------------------------------------------*/
static int  get_comp_pos(comps, name)
char *comps;                 /* a pointer to buffer with component names. */
char *name;                  /* a name of this component                  */
  {
  int nam_len;               /* length of name string                     */
  int counter;               /* local counter                             */

  nam_len = strlen(name);
  counter = 0;
  while (*comps != '\0')
    {
    if (strncmp(comps, name, nam_len) == 0)  /* check if this name match */
      return counter;
    else                                  /* seek to next component name  */
      {
      counter++;
      while (*comps != ',' && *comps != '\0')
        comps++;
      if (*comps != '\0')  /* a protection against going beyond the string */
        comps++;
      }
    }

#ifdef DEBUG
  printf(" MODULE: iifl.c FUNC.: get_comp_pos; Unknown component name!\n");
#endif

  /* error ! */
  ASSERT(FALSE)
  return 0;
  }

/*--------------------------------------------------------------------------
 * NAME         get_num_of_lines
 * PURPOSE      retrieves size of image buffer in lines set in environmental
 *              variable "IIFL_CFG". If such a variable does not exists, or
 *              has not form "lines:xx" (where xx is lines amount), function
 *              returns amount of lines depends on operetion system (MS-DOS,
 *              UNIX).
 * ARGUMENTS    None
 * RETURNS      size of image buffer in lines
 * EXTERNS      None
--------------------------------------------------------------------------*/
static int  get_num_of_lines()
  {
  char *val;        /* a pointer to "IIFL_CFG" environment variable value */
  int  lnum;                              /* a number of lines            */
  /* NOTE !
     Since library servicing environment is not ready at this time,
     there is no calls to proper functions that retrieve number of
     lines in buffer.
  */
  if ((val = ev_getenv("IIFL_CFG")) != NULL)
    {
    /*while (isspace((int)(*val)) != 0)   skip a white - space characters
      val++;  */
    if (strncmp(val, ENV_LINES, strlen(ENV_LINES)) == 0)
      {
      val += strlen(ENV_LINES);
      if ((lnum = atoi(val)) != 0)
        return lnum;
      }
    }

  if (IIF_os_mark == 'M')   /* MS-DOS */
    return DOS_LINES_AMOUNT;
  else                  /* UNIX   */
    return UNIX_LINES_AMOUNT;
  }

/*--------------------------------------------------------------------------
 * NAME         check_allowed_types
 * PURPOSE      check if all component types described in given buffer are
 *              one of allowed types, i.e.
 *              a - ASCII
 *              s - short int
 *              l - long
 *              b - byte
 *              f - float
 *              d - double
 *              Function returns immediately if any component don't match
 * ARGUMENTS    types     a pointer to buffer witch types to check
 * RETURNS      an error code
 * EXTERNS      None
--------------------------------------------------------------------------*/
static int check_allowed_types(types)
char  *types;     /* a character buffer with types of components to check */
  {
  int len;                                  /* a length of 'types' string */
  int n;                                    /* an iteration counter       */

  len = strlen(types);
  for (n = 0; n < len; n++)
    {
    if (types[n] != 'a' && types[n] != 'b' && types[n] != 'd' &&
        types[n] != 'f' && types[n] != 'l' && types[n] != 's')
      return IIFERR_BAD_COMPS;
    }
  return IIF_OK;
  }

/*--------------------------------------------------------------------------
 * NAME         env_atol
 * PURPOSE      converts string to long int
 * ARGUMENTS    b      a pointer to buffer with string to be converted
 *              val    a pointer to variable that keeps result
 * RETURNS      an error code ; 0 - OK
 * EXTERNS      None
--------------------------------------------------------------------------*/
static int env_atol(b, val)
char *b;
long *val;
  {
  long i, j;
  int sign;

  *val = 0;
  while (*b == ' ')
    b++;

  if (*b == '-' || *b == '+')
    {
    sign = *b == '-';
    b++;
    while (*b == ' ')
      b++;
    }
  else
    sign = 0;

  if (*b == '\0')
    return 1;

  i = 0;
  while (*b >= '0' && *b <= '9')
    {
    j = i * 10;
    if (j / 10 != i)
      return 2;
    i = j + (*b++ - '0');
    if (i < 0)
      return 2;
    }

  if (*b != '\0')
    return 1;

  if (sign)
    i = -i;

  *val = i;

  return 0;
  } /* end of 'int env_atol()' function */

/*--------------------------------------------------------------------------
 * NAME         env_atoi
 * PURPOSE      converts string to int
 * ARGUMENTS    b      a pointer to buffer with string to be converted
 *              val    a pointer to variable that keeps result
 * RETURNS      an error code ; 0 - OK
 * EXTERNS      None
--------------------------------------------------------------------------*/
static int env_atoi(b, val)
char *b;
int *val;
  {
  int  i;
  long l;

  i = env_atol(b, &l);
  if (i != 0)
    return i;

  i = (int)l;
  if (((long)i) != l)
    return 2;

  *val = i;
  return 0;
  } /* end of 'int env_atoi()' function */

/*--------------------------------------------------------------------------
 * NAME         ui_env_get_from
 * PURPOSE      extracts option from environment string
 * ARGUMENTS    env  - pointer to environment string
 *              name - name of option from given environment string
 * RETURNS      NULL of pointer to buffer with extracted string.
 * EXTERNS      None
--------------------------------------------------------------------------*/
static char *ui_env_get_from(env, name)
char *env;
char *name;
  {
  char *ee;  /* current position in 'env' string  */
  char *nn;  /* current position in 'name' string */
  static char field_buf[MAX_ENV_STRING_LEN + 1];
                                            /* a place for extracted string */

#ifdef DEBUG
  printf("GETTING \"%s\" FROM \"%s\"\n", name, env);
#endif

  ee = env;
  for (;;)
    {
    /* return NULL when not found */
    if (*ee == '\0')
      return NULL;

    /* compare starting from current position with name */ 
    for (nn = name; *nn != '\0'; nn++)
      if (toupper(*ee) != toupper(*nn))
        break;
      else
        ee++;

    if (*nn != '\0' || *ee != ENV_DEF)
      /* if not match */
      {
      /* skip to env of 'env' or to ENV_SEP character */
      while (*ee != ENV_SEP && *ee != '\0')
        ee++;

      if (*ee == ENV_SEP)
        /* skip ENV_SEP character */
        ee++;
      }
    else
      /* string match OK */
      {
      char *b;

      /* skip ENV_DEF character */
      ee++;

      /* copy characters starting from 'ee' to 'buf' */
      b = field_buf;
      while (*ee != ENV_SEP && *ee != '\0')
        *b++ = *ee++;

      /* mark end of string in 'buf' */
      *b = '\0';

      return field_buf;
      }
    }
  } /* end of 'char *ui_env_get_from()' function */


/*-------------------------- END OF MODULE -------------------------------*/
