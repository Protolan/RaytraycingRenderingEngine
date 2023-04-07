/*
 * MODULE NAME  FSERV.C
 * PURPOSE      access to image files in different formats.
 *
 * SPEC         Z. Blaszczyk
 *              Z. Butor                              1991.07.
 * DESIGN       Z. Blaszczyk                          1991.08 02
 * DES. INSP.
 * CODING       Z. Blaszczyk                          1991.08.
 * CODE INSP.
 * TEST         Z. Blaszczyk                          1991.08.
 * LAST UPDATED
 *              Eed  bugs correction at5706nb.Mvr     01.09.17
 *                   in fl_write_lines(),
 *                   fl_read_sys_vars()
 *
 */

/* CONDITIONAL COMPILATION SWITCHES */

#define TEST  0  /* testing   switch */

#include "integra.h"

/* COMPANY INCLUDES */

#include "iifl.h"

/* PROGRAM INCLUDES */

#include "iifl_own.h"

/* LOCAL TYPES DEFINITIONS */

/* CONSTANTS */
               /* OFFSETS AND LENGTHS OF IIFF HEADER FIELDS */
#define  OSFLAG_FIELD_OFFS        0  /* offset to 'osflag'  field in header */
#define  FMT_FIELD_OFFS           4  /* offset to 'fmt'     field in header */
#define  LPX_FIELD_OFFS           5  /* offset to 'lpx'     field in header */
#define  EXTFL_FIELD_OFFS         6  /* offset to 'extflag' field in header */
#define  ELPX_FIELD_OFFS         12  /* offset to 'lpx'     extended value  */
#define  ECMP_FIELD_OFFS         16  /* offset to           cmp line length */
#define  XSIZ_FIELD_OFFS         20  /* offset to 'xsiz'    field in header */
#define  YSIZ_FIELD_OFFS         24  /* offset to 'ysiz'    field in header */
#define  CMP_FIELD_OFFS          28  /* offset to 'cmp'     field in header */
#define  LCMP_FIELD_OFFS        142  /* offset to 'lcmp'    field in header */
#define  FILE_FIELD_OFFS        256  /* offset to 'file'    field in header */
#define  CMNT_FIELD_OFFS        512  /* offset to 'cmnt'    field in header */
#define  SWAP_FIELD_OFFS        768  /* offset to 'swap'    field in header */
#define  IMAGE_AREA_OFFS       1024  /* offset to image data, after header  */

#define  OSFLAG_FIELD_LEN         4  /* length of 'osflag'  field in header */
#define  FMT_FIELD_LEN            1  /* length of 'swap'    field in header */
#define  EXTFL_FIELD_LEN          2  /* length of 'extflag' field in header */
#define  LPX_FIELD_LEN            1  /* length of 'lpx'     field in header */
#define  ELPX_FIELD_LEN           4  /* offset to 'lpx'     extended value  */
#define  ECMP_FIELD_LEN           4  /* offset to           cmp line length */
#define  XSIZ_FIELD_LEN           4  /* length of 'xsiz'    field in header */
#define  YSIZ_FIELD_LEN           4  /* length of 'ysiz'    field in header */
#define  CMNT_FIELD_LEN         256  /* length of 'cmnt'    field in header */
#define  SWAP_FIELD_LEN           4  /* length of 'swap'    field in header */

                /* other constants */
#define  TIFF_MARKER_OFFS         0  /* offset to TIFF marker in a file     */
#define  TIFF_MARKER_LEN          4  /* length of TIFF marker in a file     */
#define  GIF_MARKER_OFFS          0  /* offset to  GIF marker in a file     */
#define  GIF_MARKER_LEN           3  /* length of  GIF marker in a file     */
#define  VARS_DESCR_LEN           4  /* length of set of bytes preceding    */
                                     /* 'user_variable=value' area          */

#define  IIFF_HEADER_SIZE      1024  /* a size of IIFF header               */

#define  SP                     ' '  /* a space character                   */
#define  MVM                     16  /* "More Vars Marker"; indicates that  */
                                     /* there is a variabes area beyond     */
                                     /* image area                          */
/* MACROS    */
 
/* STATICS   */

static BYTE dos_marker[OSFLAG_FIELD_LEN] = { 0xff, 0xff, 0xff, 0xf0};
static BYTE unix_marker[OSFLAG_FIELD_LEN] = { 0xFF, 0xFF, 0xFF, 0xFF};
static BYTE extflag[EXTFL_FIELD_LEN] = { 0, 1};

/* INTERFACE FUNCTIONS PROTOTYPES

int    fl_write_header(IIFL*);
int    fl_read_header(IIFL*);
VOID   fl_default_header_sizes(IIFL*);
VOID   fl_recalc_header_sizes(IIFL*, int, int);
int    fl_write_lines(IIFL*, int, int);
int    fl_read_lines(IIFL*, int, int);
int    fl_init_var_access(IIFL*);
int    fl_finish_var_access(IIFL*);
char  *fl_get_os_ver(IIFL*);
BYTE   fl_get_float_ver(IIFL*);
char  *fl_get_types(IIFL*);
BOOL   fl_is_correct_ver(IIFL*);
int    fl_check_file_format(IIFL*);
int    fl_read_sys_vars(IIFL*);
*/

/* LOCAL FUNCTIONS PROTOTYPES */

#ifndef NO_PROTOTYPES

static int   single_read_file(FILE*, INT64, char*, int);
static VOID  single_read(BYTE*, BYTE*, int);
static VOID  single_write(BYTE*, BYTE*, int);
static int   put_sys_var(IIFL*, char*, char*);
static int   ASCII_to_int(char*);
static VOID  int_to_ASCII(int, char*);
static VOID  int_to_ASCII_8(int, char*);
static char  check_o_s(VOID);
static BYTE  check_float_ver(VOID);
static int   put_sys_var(IIFL*, char*, char*);
static int   write_sys_vars(IIFL*);

#else

static int   single_read_file();
static VOID  single_read();
static VOID  single_write();
static int   put_sys_var();
static int   ASCII_to_int();
static VOID  int_to_ASCII();
static VOID  int_to_ASCII_8(int, char*);
static char  check_o_s();
static BYTE  check_float_ver();
static int   put_sys_var();
static int   write_sys_vars();

#endif

/****************************** CODE *************************************/

/*======================== INTERFACE FUNCTIONS ===========================*/

/* --------------------------------------------------------------------------
 * NAME         fl_write_header
 * PURPOSE      writes set of image parameters kept in IIF structure to file
 *              pointed by file_handle member of  the structure.
 * ARGUMENTS    ifp     a pointer to IIF  file.
 * RETURNS      an error code.
 * EXTERNS      None
 --------------------------------------------------------------------------*/
int  fl_write_header(ifp)
IIFL  *ifp;                                      /* a pointer to IIF file */
  {
  BYTE *hbuff;                                 /* pointer to header buffer */
  BYTE *tptr;                                   /* auxiliary pointer       */
  static char buf[1024];   /* a buffer for value - to - string conversions */
  int  err;                                     /* an error code           */
  int  swap;                                    /* swap field value        */
  int  wrtn;                                    /* amount of written items */
  unsigned short n;                             /* a loop counter          */

  if (ifp->format == IIF_FORMAT_TIFF ||
      ifp->format == IIF_FORMAT_GIF)
    return IIFERR_BAD_FORMAT;
  else      /*  for IIF_FORMAT_IIFF  */
    {
    fl_recalc_header_sizes(ifp, ifp->cmps_number, strlen(ifp->cmps));

    if ((hbuff = (BYTE *)calloc(1, ifp->iiff_header_size)) == NULL)
       return IIFERR_NO_MEM;

    single_write(hbuff + ifp->file_field_offs, (BYTE *)(ifp->filename),
                 strlen(ifp->filename) + 1);

    if (check_o_s() == 'M')
      tptr = dos_marker;
    else
      tptr = unix_marker;

    single_write(hbuff + OSFLAG_FIELD_OFFS, tptr, OSFLAG_FIELD_LEN);

    buf[0] = 1;
    single_write(hbuff + FMT_FIELD_OFFS, (BYTE *)buf, FMT_FIELD_LEN);

    buf[0] = (char)ifp->cmps_number;
    single_write(hbuff + LPX_FIELD_OFFS, (BYTE *)buf, LPX_FIELD_LEN);

    single_write(hbuff + EXTFL_FIELD_OFFS, extflag, EXTFL_FIELD_LEN);

    if (ifp->new_header_sizes)
      {
      int_to_ASCII(ifp->cmps_number, buf);
      single_write(hbuff + ELPX_FIELD_OFFS, (BYTE *)buf, ELPX_FIELD_LEN);
     
      int_to_ASCII((int)strlen(ifp->cmps), buf);
      single_write(hbuff + ECMP_FIELD_OFFS, (BYTE *)buf, ECMP_FIELD_LEN);
      }

    int_to_ASCII(ifp->xres, buf);
    single_write(hbuff + XSIZ_FIELD_OFFS, (BYTE *)buf, XSIZ_FIELD_LEN);

    int_to_ASCII(ifp->yres, buf);
    single_write(hbuff + YSIZ_FIELD_OFFS, (BYTE *)buf, YSIZ_FIELD_LEN);

    swap = ((ifp->swap == TRUE) ? 1 : 0);
    int_to_ASCII(swap, buf);
    single_write(hbuff + ifp->swap_field_offs, (BYTE *)buf, SWAP_FIELD_LEN);

    single_write(hbuff + CMP_FIELD_OFFS, (BYTE *)(ifp->cmps),
                 strlen(ifp->cmps) + 1);

    strncpy(buf, (char *)(ifp->lcmps), ifp->cmps_number);
    for (n = 0; n < ifp->cmps_number; n++)  /* convert from byte length to */
      buf[n] *= 8;                          /* bit length                  */

    single_write(hbuff + ifp->lcmp_field_offs, (BYTE *)buf, ifp->cmps_number);

    if (os_fseek(ifp->file_handle, 0, SEEK_SET) != 0)
      {
      free(hbuff);
      if (feof(ifp->file_handle) != 0)
        return IIFERR_EOF;
      else
        return IIFERR_BAD_DISK;
      }

    if ((wrtn = fwrite(hbuff, 1, ifp->iiff_header_size, ifp->file_handle)) !=
        ifp->iiff_header_size)
      {
      ifp->error = IIFERR_DISK_FULL;
      free(hbuff);
      return IIFERR_DISK_FULL;
      }
    free(hbuff);

    if ((err = put_sys_var(ifp, IIFVAR_SYST_VER, IIF_os_name)) != IIF_OK)
       return err;

    sprintf(buf, "%d", (int)check_float_ver());
    if ((err = put_sys_var(ifp, IIFVAR_FLOAT_VER, buf)) != IIF_OK)
      return err;

    if ((err = put_sys_var(ifp, IIFVAR_NEW_VER, "1")) != IIF_OK)
      return err;

    if ((err = put_sys_var(ifp, IIFVAR_TYPES, ifp->types)) != IIF_OK)
      return err;

    if (ifp->xres > 65535 || ifp->yres > 65535)
      {
      // Pls. We include xres and yres as 8-character strings in system
      // variables to avoid restriction in 65535 for image resolution.
      int_to_ASCII_8(ifp->xres, buf);
      if ((err = put_sys_var(ifp, IIFVAR_NEW_XRES, buf)) != IIF_OK)
        return err;

      int_to_ASCII_8(ifp->yres, buf);
      if ((err = put_sys_var(ifp, IIFVAR_NEW_YRES, buf)) != IIF_OK)
        return err;
      }

    if ((err = write_sys_vars(ifp)) != IIF_OK)
      return err;
    }
  return IIF_OK;
  }

/*--------------------------------------------------------------------------
 * NAME         fl_read_header
 * PURPOSE      reads set of image parameters kept from file pointed by
 *              file_handle member of  the structure , to other structure
 *              fields.
 * ARGUMENTS    ifp     a pointer to IIF  file.
 * RETURNS      an error code.
 * EXTERNS      None
 --------------------------------------------------------------------------*/
int  fl_read_header(ifp)
IIFL  *ifp;                                      /* a pointer to IIF file */
  {
  BYTE *hbuff;                              /* a pointer to header buffer */
  BYTE *buf;                           /* an auxiliary buffer for reading */
  BYTE ebuf[ELPX_FIELD_LEN + 1];
  int len;                             /* an auxiliary string length      */
  int  err;                            /* an error code                   */
  long buff_size;                      /* a size of lines buffer          */
  long line_size;                      /* size of one scan-line           */            
#if NO_64KB_LIMIT == 0
  int  max_lines;                      /* max number of lines in buffer   */ 
#endif
  unsigned short n;                    /* an iteration counter            */
  int  read;                           /* amount of items read into hbuff */
  int comps_length, comps_number;      /* extended components parameters  */
  char  *res_buf;
  VLIST *lst;

  if (ifp->format == IIF_FORMAT_TIFF ||
      ifp->format == IIF_FORMAT_GIF)
    return IIFERR_BAD_FORMAT;
  else      /*  for IIF_FORMAT_IIFF  */
    {
    comps_number = 0;
    comps_length = LCMP_FIELD_OFFS - CMP_FIELD_OFFS;
    /* get extended component parameters */
    if (single_read_file(ifp->file_handle, ELPX_FIELD_OFFS,
                         (char *)ebuf, ELPX_FIELD_LEN) != IIF_OK)
      return IIFERR_BAD_DISK;
    if (ebuf[0] != '\0' || ebuf[1] != '\0' ||
        ebuf[2] != '\0' || ebuf[3] != '\0')
      {
      comps_number = ASCII_to_int((char *)ebuf);
      if (single_read_file(ifp->file_handle, ECMP_FIELD_OFFS,
                           (char *)ebuf, ECMP_FIELD_LEN) != IIF_OK)
        return IIFERR_BAD_DISK;
      comps_length = ASCII_to_int((char *)ebuf);
      fl_recalc_header_sizes(ifp, comps_number, comps_length);
      }

    if ((buf = (BYTE *)calloc(1, ifp->iiff_header_size)) == NULL)
      return IIFERR_NO_MEM;

    if (os_fseek(ifp->file_handle, 0, SEEK_SET) != 0)
      if (feof(ifp->file_handle) != 0)
        {
        free(buf);
        return IIFERR_EOF;
        }
      else
        {
        free(buf);
        return IIFERR_BAD_DISK;
        }

    if ((hbuff = (BYTE *)malloc(ifp->iiff_header_size)) == NULL)
      {
      free(buf);
      return IIFERR_NO_MEM;
      }

    if ((read = fread(hbuff, 1, ifp->iiff_header_size, ifp->file_handle)) !=
        ifp->iiff_header_size)
      {
      free(hbuff);
      free(buf);
      return IIFERR_BAD_DISK;
      }

    if (comps_number > 0)
      ifp->cmps_number = comps_number;
    else
      {
      single_read(hbuff + LPX_FIELD_OFFS, buf, LPX_FIELD_LEN);
      ifp->cmps_number = (BYTE) buf[0];
      }

    single_read(hbuff + XSIZ_FIELD_OFFS, buf, XSIZ_FIELD_LEN);
    ifp->xres = ASCII_to_int((char *)buf);

    single_read(hbuff + YSIZ_FIELD_OFFS, buf, YSIZ_FIELD_LEN);
    ifp->yres = ASCII_to_int((char *)buf);

    single_read(hbuff + ifp->swap_field_offs, buf, SWAP_FIELD_LEN);
    ifp->swap = (BOOL)((ASCII_to_int((char *)buf) == 1) ? TRUE : FALSE);

    single_read(hbuff + CMP_FIELD_OFFS, buf, comps_length + 1);
    len = strlen((char *)buf);
    if ((ifp->cmps = (char*)malloc(len + 1)) == NULL)
      {
      free(hbuff);
      free(buf);
      return IIFERR_NO_MEM;
      }
    strcpy(ifp->cmps, (char *)buf);

    single_read(hbuff + ifp->lcmp_field_offs, buf, ifp->cmps_number);
    for (n = 0; n < ifp->cmps_number; n++)  /* convert from bit  length to */
      buf[n] /= 8;                          /* byte length                 */

    if ((ifp->lcmps = (BYTE*)malloc((unsigned int)(ifp->cmps_number))) == NULL)
      {
      free(hbuff);
      free(buf);
      return IIFERR_NO_MEM;
      }
    strncpy((char *)(ifp->lcmps), (char *)buf, (int)(ifp->cmps_number));

    free(hbuff);
    free(buf);

    if ((err = fl_read_sys_vars(ifp)) != IIF_OK)
      return err;
    
    // Pls. We try to get image resolution from system variables.
    // For old files (where resolution is lower than 65535) these fields
    // are absent. They are already loaded from header above.
    // We cannot use iif_get_var() because IIFL "initialized" field
    // have not set to TRUE. So we will use find_var_name().

    if ((lst = find_var_name(ifp->syst_var, IIFVAR_NEW_XRES)) != NULL)
      ifp->xres = ASCII_to_int(lst->value);
    if ((lst = find_var_name(ifp->syst_var, IIFVAR_NEW_YRES)) != NULL)
      ifp->yres = ASCII_to_int(lst->value);

    line_size = 0;
    for (n = 0; n < ifp->cmps_number; n++)
      {
      line_size += (long)(ifp->xres * (*(ifp->lcmps + n)));
      }
    buff_size = line_size * ifp->num_of_lines;
#if NO_64KB_LIMIT == 0
    if (buff_size >= MAX_ARRAY_SIZE)
      {
      max_lines = (int)(MAX_ARRAY_SIZE / (int)line_size);
      max_lines--;
      ifp->num_of_lines = max_lines;
      buff_size = max_lines * line_size;
      }
#endif
    ifp->buffer_size = buff_size;
    }
  return IIF_OK;
  }

/*--------------------------------------------------------------------------
 * NAME         fl_default_header_sizes
 * PURPOSE      Set default values of header offsets and lengths
 * ARGUMENTS    ifp          a pointer to IIF  file.
 * EXTERNS      None
 --------------------------------------------------------------------------*/
VOID fl_default_header_sizes(ifp)
IIFL  *ifp;                           /* a pointer to IIF file             */
  {
  ifp->new_header_sizes = FALSE;
  ifp->lcmp_field_offs = LCMP_FIELD_OFFS;
  ifp->file_field_offs = FILE_FIELD_OFFS;
  ifp->cmnt_field_offs = CMNT_FIELD_OFFS;
  ifp->swap_field_offs = SWAP_FIELD_OFFS;
  ifp->image_area_offs = IMAGE_AREA_OFFS;
  ifp->cmnt_field_len = CMNT_FIELD_LEN;
  ifp->iiff_header_size = IIFF_HEADER_SIZE;
  }

/*--------------------------------------------------------------------------
 * NAME         fl_recalc_header_sizes
 * PURPOSE      Recalculate header offsets and record lenghts according to
 *              the component parameters
 * ARGUMENTS    ifp          a pointer to IIF  file.
 * EXTERNS      None
 --------------------------------------------------------------------------*/
VOID fl_recalc_header_sizes(ifp, comps_number, comps_length)
IIFL  *ifp;                           /* a pointer to IIF file             */
int comps_number;                     /* a number of IIF components        */
int comps_length;                     /* a string length of IIF components */
  {
  if (comps_length + 1 > LCMP_FIELD_OFFS - CMP_FIELD_OFFS)
    {
    int n1 = (CMP_FIELD_OFFS + 2 * (comps_length > comps_number ?
                                    comps_length : comps_number) + 1) / 256;
    int n2 = (comps_number - 64) / 256;
    int l1 = CMNT_FIELD_OFFS - FILE_FIELD_OFFS;
    int l2 = IMAGE_AREA_OFFS - SWAP_FIELD_OFFS;

    ifp->new_header_sizes = TRUE;
    ifp->file_field_offs = (n1 + 1) * 256;
    ifp->lcmp_field_offs = (ifp->file_field_offs + CMP_FIELD_OFFS) / 2;
    ifp->cmnt_field_offs = ifp->file_field_offs + l1;
    if (comps_number > CMNT_FIELD_LEN - 64)
      ifp->cmnt_field_len = (n2 + 2) * 256;
    ifp->swap_field_offs = ifp->cmnt_field_offs + ifp->cmnt_field_len;
    ifp->iiff_header_size = ifp->image_area_offs = ifp->swap_field_offs + l2;
    }
  }

/*--------------------------------------------------------------------------
 * NAME         fl_read_lines
 * PURPOSE      reads 'number' lines of image from file pointed by
 *              file_handle member of  the structure , to internall buffer,
 *              beginning from line 'first'.
 * ARGUMENTS    ifp     a pointer to IIF  file.
 *              first   first line that should be placed in buffer
 *              number  amount of lines to read.
 * RETURNS      an error code.
 * EXTERNS      None
 --------------------------------------------------------------------------*/
int  fl_read_lines(ifp, first, number)
IIFL  *ifp;                  /* a pointer to IIF  file.                    */
int   first;                 /* first line that should be placed in buffer */
int   number;                /* amount of lines to read.                   */
  {
  unsigned long bytes_per_line; /* size of one line in bytes               */
  unsigned int read_items;      /* amount of lines successfully read into  */
                                /* buffer */
  INT64  beginning;          /* beginning of file area with data to read   */
  int    err;                /* error flag */

  if (ifp->format == IIF_FORMAT_TIFF ||
      ifp->format == IIF_FORMAT_GIF)
    return IIFERR_BAD_FORMAT;
  else      /*  for IIF_FORMAT_IIFF  */
    {
    bytes_per_line = (unsigned long)(ifp->buffer_size / ifp->num_of_lines);
    beginning = ifp->image_area_offs + (INT64)bytes_per_line * first;

    if (ifp->attrib == 'u')
      fflush(ifp->file_handle);
    if (os_fseek(ifp->file_handle, beginning, SEEK_SET) != 0)
      {
      if (feof(ifp->file_handle) != 0)
        err = IIFERR_EOF;
      else
        err = IIFERR_BAD_DISK;
      ifp->error = err;
#ifdef DEBUG
      printf("MOD.:fserv.c, FUNC.: read_lines. Seek to beginning of first line \
              failed!");
#endif
      return err;
      }

    if ((read_items = fread(ifp->buffer, (unsigned int)bytes_per_line, number,
                            ifp->file_handle)) != (unsigned int)number)
      {
      if (feof(ifp->file_handle) != 0)
        err = IIFERR_EOF;
      else
        err = IIFERR_BAD_DISK;
      ifp->error = err;
#ifdef DEBUG
printf("MOD.:fserv.c, FUNC.: read_lines. Reading lines to buffer failed!");
printf("%s%d%s%d\n", "Requires bytes: ", number, " read: ", read_items);
#endif
      return err;
      }
    }
  return IIF_OK;
  }

/*--------------------------------------------------------------------------
 * NAME         fl_write_lines
 * PURPOSE      writes 'number' lines of image to a file pointed by
 *              file_handle member of  the structure , from internall buffer,
 *              beginning from line 'first'.
 * ARGUMENTS    ifp     a pointer to IIF  file.
 *              first   first line that should be written to a file
 *              number  amount of lines to write.
 * RETURNS      an error code.
 * EXTERNS      None
 --------------------------------------------------------------------------*/
int  fl_write_lines(ifp, first, number)
IIFL  *ifp;                 /* a pointer to IIF  file.                     */
int   first;                /* first line that should be written to a file */
int   number;               /* amount of lines to write                    */
  {
  unsigned long bytes_per_line;  /* size of one line in bytes               */
  unsigned int write_items;      /* amount of lines successfully written   */
                                 /* from buffer */
  INT64  beginning;       /* beginning of file area with data to read      */
  int    line;            /* currently checked image line                  */
  int    last;            /* last image line to be checked                 */
  int    line_in_buf;     /* line number in buffer                         */
  int    to_write;        /* amount of lines to be written in one group    */
  BYTE  *bufptr;          /* an auxiliary pointer to image data to write   */

  if (ifp->format == IIF_FORMAT_TIFF ||
      ifp->format == IIF_FORMAT_GIF)
    return IIFERR_BAD_FORMAT;
  else      /*  for IIF_FORMAT_IIFF  */
    {
    bytes_per_line = (unsigned long)(ifp->buffer_size / ifp->num_of_lines);
    /*beginning = (long)(ifp->image_area_offs + bytes_per_line * first);*/
    last = first + number;
    to_write = 0;

    if (ifp->attrib == 'u')
      fflush(ifp->file_handle);
/**/
    for (line = first; line < last; )
      {
      while ((line < last) && (ifp->changed_lines[line] == LINE_NOT_CHANGED))
        line ++;        /* skip not changed lines */

      if (line >= last)
        break;

      beginning = ifp->image_area_offs + (INT64)bytes_per_line * line;
      line_in_buf = line - ifp->first_line;
      bufptr = (BYTE*)(ifp->buffer) + (unsigned)(bytes_per_line * line_in_buf);

      while ((line < last) && (ifp->changed_lines[line] == LINE_CHANGED))
        {
        ifp->changed_lines[line] = LINE_NOT_CHANGED;
        to_write++;
        line++;
        }

      if (to_write == 0)
        continue;

      if (os_fseek(ifp->file_handle, beginning, SEEK_SET) != 0)
        {
        ifp->error = IIFERR_DISK_FULL;
#ifdef DEBUG
printf("MOD.:fserv.c, FUNC.: write_lines. Seek to beginning of first line \
 failed!");
#endif
        return IIFERR_DISK_FULL;
        }

      if ((write_items = fwrite(bufptr, (unsigned int)bytes_per_line, to_write,
                               ifp->file_handle)) != (unsigned int)to_write)
        {
        ifp->error = IIFERR_DISK_FULL;
#ifdef DEBUG
printf("MOD.:fserv.c, FUNC.: write_lines. Buffer writting failed!");
printf("%s%d%s%d\n", "Required lines: ", number, " written: ", write_items);
#endif
        return IIFERR_DISK_FULL;
        }

      to_write = 0;
      } /* end  for every line  */
    }
  return IIF_OK;
  }

/*--------------------------------------------------------------------------
 * NAME         fl_init_var_access
 * PURPOSE      initializes access to user-defined variables area
 * ARGUMENTS    ifp     a pointer to IIF  file.
 * RETURNS      an error code.
 * EXTERNS      None
 --------------------------------------------------------------------------*/
int fl_init_var_access(ifp)
IIFL  *ifp;                         /* a pointer to IIF file */
  {
  INT64   image_size;               /* an image area size                    */
  unsigned int vars_in_header;      /* a size of vars area in header area    */
  INT64  vars_ptr;                  /* current pointer position in a file    */
  unsigned long bytes_per_line;     /* size of one line in bytes             */
  BOOL   done;                      /* flag; TRUE - if '\0''\0' marker found */
  BYTE type;                        /* type of data stored in 'value'        */
  BYTE nam_len;                     /* length of 'name' string including \0  */
  BYTE tmp;                         /* temporary value                       */
  unsigned short int val_len;       /* length of 'value' area                */
  VLIST *tmp_ptr, *list;            /* vorking pointers                      */
  BYTE  *var_ptr;                   /* a pointer to swapped value            */

  if (ifp->format == IIF_FORMAT_TIFF ||
      ifp->format == IIF_FORMAT_GIF)
    return IIFERR_BAD_FORMAT;
  else      /*  for IIF_FORMAT_IIFF  */
    {
    vars_in_header = (unsigned int)((long)ifp->swap_field_offs -
                                          ifp->vars_offset);

    vars_ptr = (INT64)ifp->vars_offset;
    if (os_fseek(ifp->file_handle, vars_ptr, SEEK_SET) != 0)
      if (feof(ifp->file_handle) != 0)
        return IIFERR_EOF;
      else
        return IIFERR_BAD_DISK;

    done = FALSE;
    while (done == FALSE)
      {

      if (fread(&type, 1, sizeof(BYTE), ifp->file_handle) != sizeof(BYTE))
        if (feof(ifp->file_handle) != 0)
          return IIFERR_EOF;
        else
          return IIFERR_BAD_DISK;

      if (type != STRING_VAR && type != DATA_VAR && type != MVM &&
          type != END_OF_USER_VARS)
        return IIFERR_BAD_DISK;

      if (type == END_OF_USER_VARS)
        {
        done = TRUE;
        continue;
        }
      else
        if (type == MVM)   /* there are more vars beyond image area */
          {
          bytes_per_line = 
            (unsigned long)(ifp->buffer_size / ifp->num_of_lines);
          image_size = (INT64)bytes_per_line * ifp->yres;
          vars_ptr = image_size + ifp->image_area_offs;

          if (os_fseek(ifp->file_handle, vars_ptr, SEEK_SET) != 0)
            if (feof(ifp->file_handle) != 0)
              return IIFERR_EOF;
            else
              return IIFERR_BAD_DISK;
          continue;
          }

      if (fread(&nam_len, 1, sizeof(BYTE), ifp->file_handle) != sizeof(BYTE))
        if (feof(ifp->file_handle) != 0)
          return IIFERR_EOF;
        else
          return IIFERR_BAD_DISK;

      if (fread(&val_len, 1, sizeof(short int), ifp->file_handle) !=
          sizeof(short int))
        if (feof(ifp->file_handle) != 0)
          return IIFERR_EOF;
        else
          return IIFERR_BAD_DISK;

      if (IIF_os_byte_order == 0)
        {
        var_ptr = (BYTE *)(&val_len);
        tmp = *var_ptr;
        *var_ptr = *(var_ptr + 1);
        *(var_ptr + 1) = tmp;
        }

      if((tmp_ptr = (VLIST *) malloc(sizeof(VLIST))) == NULL)
        {
        return IIFERR_NO_MEM;
        }

      if ((tmp_ptr->name = (char *) malloc(nam_len)) == NULL)
        {
        free(tmp_ptr);
        return IIFERR_NO_MEM;
        }

      if ((tmp_ptr->value = (char *) malloc(val_len)) == NULL)
        {
        free(tmp_ptr->name);
        free(tmp_ptr);
        return IIFERR_NO_MEM;
        }

      if (fread(tmp_ptr->name, 1, nam_len, ifp->file_handle) !=
          (unsigned int)nam_len ||
          fread(tmp_ptr->value, 1, val_len, ifp->file_handle) !=
          (unsigned int)val_len)
        {
        free(tmp_ptr->name);
        free(tmp_ptr->value);
        free(tmp_ptr);

        if (feof(ifp->file_handle) != 0)
          return IIFERR_EOF;
        else
          return IIFERR_BAD_DISK;
        }

      *(tmp_ptr->name + nam_len - 1) = '\0';
      tmp_ptr->next = NULL;
      tmp_ptr->type = type;
      tmp_ptr->nam_len = nam_len;
      tmp_ptr->val_len = val_len;

      if (ifp->first_var == NULL)
         ifp->first_var = tmp_ptr;
      else  /* seek to the end of the list */
        {
        list = ifp->first_var;
        while (list->next != NULL)
          list = list->next;
        list->next = tmp_ptr;
        }

      } /* end of WHILE statement */
    }   /* end of ELSE  statement */
  return IIF_OK;
  }

/*--------------------------------------------------------------------------
 * NAME         fl_finish_var_access
 * PURPOSE      finishes access to user-defined variables area
 * ARGUMENTS    ifp     a pointer to IIF  file.
 * RETURNS      an error code.
 * EXTERNS      None
 --------------------------------------------------------------------------*/
int  fl_finish_var_access(ifp)
IIFL  *ifp;                         /* a pointer to IIF file  */
  {
  INT64 image_size;                 /* an image area size                    */
  unsigned int vars_in_header;      /* a size of vars area in header area    */
  INT64  vars_ptr;                  /* current pointer position in a file    */
  unsigned long bytes_per_line;     /* size of one line in bytes             */
  VLIST *var;                       /* a pointer to  VLIST list              */
  BYTE   marker;                    /* a marker buffer                       */
  BYTE  *var_ptr;                   /* an auxiliary pointer                  */
  BYTE   tmp;                       /* a temporary value for swapping        */
  unsigned short int tmp_len;       /* temporary length of variable value    */

  if (ifp->format == IIF_FORMAT_TIFF ||
      ifp->format == IIF_FORMAT_GIF)
    return IIFERR_BAD_FORMAT;
  else      /*  for IIF_FORMAT_IIFF  */
    {
    vars_in_header = (unsigned int)((long)ifp->swap_field_offs -
                                    ifp->vars_offset - 1);

    if (os_fseek(ifp->file_handle, ifp->vars_offset, SEEK_SET) != 0)
      if (feof(ifp->file_handle) != 0)
        return IIFERR_EOF;
      else
        return IIFERR_BAD_DISK;

    var = ifp->first_var;
    while (var != NULL)
      {
      if (vars_in_header > 0)
        {
        if ((unsigned int)(var->nam_len + var->val_len + VARS_DESCR_LEN + 1) >
            vars_in_header)
          {
          vars_in_header = 0;
          bytes_per_line = (unsigned long)(ifp->buffer_size / ifp->num_of_lines);
          image_size = (INT64)bytes_per_line * ifp->yres;
          vars_ptr = image_size + (INT64)ifp->image_area_offs;

          marker = (BYTE)MVM;
          if (fwrite(&marker, 1, sizeof(BYTE), ifp->file_handle) !=
              sizeof(BYTE))
            return IIFERR_DISK_FULL;

          if (os_fseek(ifp->file_handle, vars_ptr, SEEK_SET) != 0)
            if (feof(ifp->file_handle) != 0)
              return IIFERR_EOF;
            else
              return IIFERR_BAD_DISK;
          }
         else
           vars_in_header -= var->nam_len + var->val_len + VARS_DESCR_LEN;
        }

      if (fwrite(&(var->type), 1, sizeof(BYTE), ifp->file_handle) !=
          sizeof(BYTE))
        return IIFERR_DISK_FULL;

      if (fwrite(&(var->nam_len), 1, sizeof(BYTE), ifp->file_handle) !=
          sizeof(BYTE))
        return IIFERR_DISK_FULL;

      tmp_len = var->val_len;
      if (IIF_os_byte_order == 0)
        {
        var_ptr = (BYTE *)(&(var->val_len));
        tmp = *var_ptr;
        *var_ptr = *(var_ptr + 1);
        *(var_ptr + 1) = tmp;
        }

      if (fwrite(&(var->val_len), 1, sizeof(var->val_len), ifp->file_handle) !=
          sizeof(var->val_len))
        return IIFERR_DISK_FULL;

      *(var->name + var->nam_len - 1) = '=';
      if (fwrite(var->name, 1, var->nam_len, ifp->file_handle) !=
          var->nam_len)
        return IIFERR_DISK_FULL;

      if (fwrite(var->value, 1, tmp_len, ifp->file_handle) !=
          (unsigned int)(tmp_len))
        return IIFERR_DISK_FULL;

      var = var->next;
      } /* end of WHILE statement */

    marker = (BYTE)END_OF_USER_VARS;
    if (fwrite(&marker, 1, sizeof(BYTE), ifp->file_handle) !=
        sizeof(BYTE))
      return IIFERR_DISK_FULL;

    return IIF_OK;
    }
  }

/*--------------------------------------------------------------------------
 * NAME         fl_get_os_ver
 * PURPOSE      retrieves string describing version which was used when
 *              file was created.
 * ARGUMENTS    ifp     a pointer to IIF  file.
 * RETURNS      a pointer to string describing version of operating system
 * EXTERNS      None
 --------------------------------------------------------------------------*/
char  *fl_get_os_ver(ifp)
IIFL  *ifp;                                      /* a pointer to IIF file  */
  {
  if (ifp->format == IIF_FORMAT_TIFF ||
      ifp->format == IIF_FORMAT_GIF)
    {
    ifp->error = IIFERR_BAD_FORMAT;
    IIF_errno = IIFERR_BAD_FORMAT;
    return NULL;
    }
  else      /*  for IIF_FORMAT_IIFF  */
    {
    return iif_get_var((IIF *)ifp, IIFVAR_SYST_VER);
    }
  }

/*--------------------------------------------------------------------------
 * NAME         fl_get_float_ver
 * PURPOSE      retrieves string describing floating point format obligatory
 *              in  ifp file.
 * ARGUMENTS    ifp     a pointer to IIF  file.
 * RETURNS      a code of floating point format
 * EXTERNS      None
 --------------------------------------------------------------------------*/
BYTE   fl_get_float_ver(ifp)
IIFL  *ifp;                                      /* a pointer to IIF file  */
  {
  BYTE fp;                              /* retrieved floating point format */

  if (ifp->format == IIF_FORMAT_TIFF ||
      ifp->format == IIF_FORMAT_GIF)
    {
    ifp->error = IIFERR_BAD_FORMAT;
    IIF_errno = IIFERR_BAD_FORMAT;
    return BAD_FP_FORMAT;
    }
  else      /*  for IIF_FORMAT_IIFF  */
    {
    fp = (BYTE)atoi(iif_get_var((IIF *)ifp, IIFVAR_SYST_VER));
    return fp;
    }
  }

/*--------------------------------------------------------------------------
 * NAME         fl_get_types
 * PURPOSE      retrieves string describing types of components set according
 *              to IIF_Types system variable.
 * ARGUMENTS    ifp     a pointer to IIF  file.
 * RETURNS      a pointer to buffer with 'types' string.
 * EXTERNS      None
 --------------------------------------------------------------------------*/
char *fl_get_types(ifp)
IIFL  *ifp;                                       /* a pointer to IIF file */
  {
  char  *types;
  char  *buffer;

  if (ifp->format == IIF_FORMAT_TIFF ||
      ifp->format == IIF_FORMAT_GIF)
    {
    ifp->error = IIFERR_BAD_FORMAT;
    IIF_errno = IIFERR_BAD_FORMAT;
    return NULL;
    }
  else      /*  for IIF_FORMAT_IIFF  */
    {
    if ((types = iif_get_var((IIF *)ifp, IIFVAR_TYPES)) != NULL)
      {
      if ((buffer = (char *)malloc(strlen(types) + 1)) != NULL)
        {
        strcpy(buffer, types);
        return buffer;
        }
      else
        return NULL;
      }
    else
      return NULL;
    }
  }

/*--------------------------------------------------------------------------
 * NAME         fl_is_correct_ver
 * PURPOSE      checks if given file was created under new IIFL library.
 * ARGUMENTS    ifp     a pointer to IIF  file.
 * RETURNS      TRUE -if library is able to service this file; FALSE -otherwise
 * EXTERNS      None
 --------------------------------------------------------------------------*/
BOOL  fl_is_correct_ver(ifp)
IIFL  *ifp;                                       /* a pointer to IIF file */
  {
  char *new_version;             /* a pointer to IIF_NewVer variable value */

  if (ifp->format == IIF_FORMAT_TIFF ||
      ifp->format == IIF_FORMAT_GIF)
    {
    ifp->error = IIFERR_BAD_FORMAT;
    IIF_errno = IIFERR_BAD_FORMAT;
    return FALSE;
    }
  else      /*  for IIF_FORMAT_IIFF  */
    {
    if ((new_version = iif_get_var((IIF *)ifp, IIFVAR_NEW_VER)) == NULL)
      return FALSE;
    if (strcmp(new_version, "1") == 0)
      return TRUE;
    else
      return FALSE;
    }
  }

/*--------------------------------------------------------------------------
 * NAME         fl_check_file_format
 * PURPOSE      checks format of given image file.
 * ARGUMENTS    ifp     a pointer to IIF  file.
 * RETURNS      recognized format, or IIFERR_BAD_FORMAT error code;
 * EXTERNS      None
 --------------------------------------------------------------------------*/
int  fl_check_file_format(ifp)
IIFL  *ifp;                                   /* a pointer to IIF file     */
  {
  BYTE checkbuf[5];                           /* a buffer for file markers */

       /* check for IIFF markers: */
  if (single_read_file(ifp->file_handle, OSFLAG_FIELD_OFFS,
                   (char *)checkbuf, OSFLAG_FIELD_LEN) != IIF_OK)
    return IIFERR_BAD_DISK;

  if ((checkbuf[0] == (BYTE)0xFF && checkbuf[1] == (BYTE)0xFF &&
       checkbuf[2] == (BYTE)0xFF && checkbuf[3] == (BYTE)0xFF) ||
      (checkbuf[0] == (BYTE)0xFF && checkbuf[1] == (BYTE)0xFF &&
       checkbuf[2] == (BYTE)0xFF && checkbuf[3] == (BYTE)0xF0))
    return IIF_FORMAT_IIFF;

       /* check for TIFF markers: */
  if (single_read_file(ifp->file_handle, TIFF_MARKER_OFFS,
                   (char *)checkbuf, TIFF_MARKER_LEN) != IIF_OK)
    return IIFERR_BAD_DISK;

  if ((checkbuf[0] == (BYTE)'I' && checkbuf[1] == (BYTE)'I'  &&
       checkbuf[2] == (BYTE)'\0' && checkbuf[3] == (BYTE)0x2A) ||
      (checkbuf[0] == (BYTE)'M' && checkbuf[1] == (BYTE)'M' &&
       checkbuf[2] == (BYTE)'\0' && checkbuf[3] == (BYTE)0x2A))
    return IIF_FORMAT_TIFF;

       /* check for GIF markers: */
  if (single_read_file(ifp->file_handle, GIF_MARKER_OFFS,
                   (char *)checkbuf, GIF_MARKER_LEN) != IIF_OK)
    return IIFERR_BAD_DISK;

  if (checkbuf[0] == (BYTE)'G' && checkbuf[1] == (BYTE)'I' &&
     checkbuf[2] ==(BYTE) 'F')
    return IIF_FORMAT_TIFF;

        /* no one allowed format, so: */
  return IIFERR_BAD_FORMAT;
  }

/*--------------------------------------------------------------------------
 * NAME         fl_read_sys_vars
 * PURPOSE      reads system variables from image file.
 * ARGUMENTS    ifp     a pointer to IIF  file.
 * RETURNS      an error code;
 * EXTERNS      None
 --------------------------------------------------------------------------*/
int  fl_read_sys_vars(ifp)
IIFL *ifp;                            /* a pointer to IIF file              */
  {
  char *vars_buff;                    /* a buffer for system vars area      */
  char *name, *value;                 /* pointers to name and value strings */
  register char *next;                /* an auxiliary pointer               */

  if (os_fseek(ifp->file_handle, ifp->cmnt_field_offs, SEEK_SET) != 0)
    return IIFERR_BAD_DISK;

  if ((vars_buff = (BYTE *)calloc(1, ifp->cmnt_field_len)) == NULL)
    return IIFERR_NO_MEM;
  if (fread(vars_buff, 1, (unsigned int)ifp->cmnt_field_len,
            ifp->file_handle) != (unsigned int)ifp->cmnt_field_len)
    {
    free(vars_buff);
    return IIFERR_BAD_DISK;
    }

  next = vars_buff;
  while (*next != '\0' || *(next + 1) != '\0')    /* double '\0' terminates */
    {                                             /* variables area         */
    name = (next == vars_buff) ? next : next + 1; /* skip '\0' if needed    */

    while ((next < vars_buff + ifp->cmnt_field_len) && (*next != '='))
      next++;

    if (*next != '=')
      {
      free(vars_buff);
      return IIFERR_OLD_VER;
      }

    *next++ = '\0';
    value = next;

    while ((next < vars_buff + ifp->cmnt_field_len) && (*next != '\0'))
       next++;               /* seek to next 'name=value' string in buffer */

    if (*next != '\0')
      {
      free(vars_buff);
      return IIFERR_OLD_VER;
      }

    if (fl_check_registered_names(name) == FALSE)
      {
      free(vars_buff);
      return IIFERR_OLD_VER;                  /* not a system variable name */
      }

    if (put_sys_var(ifp, name, value) != IIF_OK)
      {
      free(vars_buff);
      return IIFERR_NO_MEM;
      }

    }

  if (next == vars_buff)                      /* no system variables found */
    {
    free(vars_buff);
    return IIFERR_OLD_VER;
    }

  ifp->vars_offset = (ifp->cmnt_field_offs + (next - vars_buff) + 2);
  free(vars_buff);
  return IIF_OK;
  }

/*========================= LOCAL FUNCTIONS ==============================*/

/*--------------------------------------------------------------------------
 * NAME         single_read_file
 * PURPOSE      reads bytes area from file in one-byte items.
 * ARGUMENTS    handle  a pointer to file.
 *              pos     start position in a file
 *              buffer  a buffer for data storing
 *              number  amount of bytes to read;
 * RETURNS      an error code;
 * EXTERNS      None
 --------------------------------------------------------------------------*/
static int  single_read_file(handle, pos, buffer, number)
FILE  *handle;                                /* a pointer to file.        */
INT64  pos;                                   /* start position in a file  */
char  *buffer;                                /* a buffer for data storing */
int    number;                                /* amount of bytes to read;  */
  {
  int cntr;                                   /* an auxiliary counter      */

  if (os_fseek(handle, pos, SEEK_SET) != 0)
    if (feof(handle) != 0)
      return IIFERR_EOF;
    else
      return IIFERR_BAD_DISK;

  for (cntr = 0; cntr < number; cntr++)
    {
    if (fread(buffer + cntr, 1, 1, handle) != 1)
      return IIFERR_BAD_DISK;
    }
  buffer[cntr] = '\0';                     /* terminate string with null */
  return IIF_OK;
  }

/*--------------------------------------------------------------------------
 * NAME         single_read
 * PURPOSE      reads bytes area from given buffer in one-byte items.
 * ARGUMENTS    area   a pointer to area with data.
 *              buffer  a buffer for data storing
 *              number  amount of bytes to read;
 * RETURNS      None
 * EXTERNS      None
 --------------------------------------------------------------------------*/
static VOID single_read(area, buffer, number)
BYTE  *area;                                  /* a pointer to buffer area  */
BYTE  *buffer;                                /* a buffer for data storing */
int    number;                                /* amount of bytes to read;  */
  {
  int cntr;                                   /* an auxiliary counter      */

  for (cntr = 0; cntr < number; cntr++)
    {
    *buffer++ = *area++;
    }
  *buffer = '\0';                            /* terminate string with null */
  }

/*--------------------------------------------------------------------------
 * NAME         single_write
 * PURPOSE      writes bytes area to a header buffer in one-byte items.
 * ARGUMENTS    area    a pointer to header area to write data.
 *              buffer  a buffer with data to write
 *              number  amount of bytes to write
 * RETURNS      None
 * EXTERNS      None
 --------------------------------------------------------------------------*/
static VOID single_write(area, buffer, number)
BYTE  *area;                                  /* a pointer to buffer area  */
BYTE  *buffer;                                /* a buffer for data storing */
int    number;                                /* amount of bytes to read;  */
  {
  int cntr;                                   /* an auxiliary counter      */

  for (cntr = 0; cntr < number; cntr++)
    {
     *area++ = *buffer++;
    }
  }

/*--------------------------------------------------------------------------
 * NAME         ASCII_to_int
 * PURPOSE      converts ASCII string that represents hexadecimal value to
 *              an integer value;
 * ARGUMENTS    buffer  a buffer with string to convert
 * RETURNS      an integer value;
 * EXTERNS      None
 --------------------------------------------------------------------------*/
static int   ASCII_to_int(buffer)
char  *buffer;                          /* a buffer with string to convert */
  {
  return (int)strtol(buffer, NULL, 16); /* convert string to int, base 16  */
  }

/*--------------------------------------------------------------------------
 * NAME         int_to_ASCII
 * PURPOSE      converts an integer value to;ASCII string that represents
 *              hexadecimal value as a four-character string;
 * ARGUMENTS    value   an integer value to convert;
 *              buffer  a buffer for string after convertion;
 * RETURNS      None
 * EXTERNS      None
 --------------------------------------------------------------------------*/
static VOID  int_to_ASCII(value, buffer)
int    value;                           /* an integer value to convert     */
char  *buffer;                          /* a buffer with string to convert */
  {
  char tmp_bf[16];                      /* a temporary buffer for string   */
  int len;                              /* a length of converted string    */
  int i;                                /* an iteration counter            */

  sprintf(tmp_bf, "%x", value);
  len = strlen(tmp_bf);
  if (len < 4)
    {
    for (i = 0; i < 4 - len; i++)
      buffer[i] = SP;            /* fill beginning of the buffer with spaces */
    strncpy(buffer + i, tmp_bf, len);
    }
  else
    strncpy(buffer, tmp_bf, 4);
  }

/*--------------------------------------------------------------------------
* NAME         int_to_ASCII_8
* PURPOSE      converts an integer value to;ASCII string that represents
*              hexadecimal value as a eight-character string;
* ARGUMENTS    value   an integer value to convert;
*              buffer  a buffer for string after convertion;
* RETURNS      None
* EXTERNS      None
* NOTE         The buffer should be larger than 9 bytes.
--------------------------------------------------------------------------*/
static VOID  int_to_ASCII_8(value, buffer)
int    value;                           /* an integer value to convert     */
char  *buffer;                          /* a buffer with string to convert */
  {
  char tmp_bf[16];                      /* a temporary buffer for string   */
  int len;                              /* a length of converted string    */
  int i;                                /* an iteration counter            */

  sprintf(tmp_bf, "%x", value);
  len = strlen(tmp_bf);
  if (len < 8)
    {
    for (i = 0; i < 8 - len; i++)
      buffer[i] = SP;            /* fill beginning of the buffer with spaces */
    strncpy(buffer + i, tmp_bf, len);
    }
  else
    strncpy(buffer, tmp_bf, 8);
  buffer[8] = 0;
  }

/*--------------------------------------------------------------------------
 * NAME         check_o_s
 * PURPOSE      checks current working operating system and returns proper
 *              value;
 * ARGUMENTS    None
 * RETURNS      a character according tu detected operating system.
 * EXTERNS      None
 --------------------------------------------------------------------------*/
static char  check_o_s()
  {
  return   IIF_os_mark;       /* MS-DOS 'M' or UNIX 'U'*/
  }

/*--------------------------------------------------------------------------
 * NAME         check_float_ver
 * PURPOSE      checks floating point format obligatory in current working
 *              operating system;
 * ARGUMENTS    None
 * RETURNS      a character according tu detected floating point format
 * EXTERNS      None
 --------------------------------------------------------------------------*/
static  BYTE check_float_ver()
   {
   return IIF_os_fp_format;
   }

/*--------------------------------------------------------------------------
 * NAME         put_sys_var
 * PURPOSE      puts one system variable to variables list
 *              operating system;
 * ARGUMENTS    ifp       a pointer to IIF file
 *              name      a pointer to name string
 *              value     a pointer to value string
 * RETURNS      an error code
 * EXTERNS      None
 --------------------------------------------------------------------------*/
static int  put_sys_var(ifp, name, value)
IIFL  *ifp;                                   /* a pointer to IIF file     */
char *name;                                   /* a pointer to name string  */
char *value;                                  /* a pointer to value string */
  {
  VLIST *tmp_ptr, *list;
  int nam_len, val_len;

  nam_len = strlen(name) + 1;
  val_len = strlen(value) + 1;

  if((tmp_ptr = (VLIST *) malloc(sizeof(VLIST))) == NULL)
    {
    return IIFERR_NO_MEM;
    }

  if ((tmp_ptr->name = (char *) malloc(nam_len)) == NULL)
    {
    free(tmp_ptr);
    return IIFERR_NO_MEM;
    }

  if ((tmp_ptr->value = (char *) malloc(val_len)) == NULL)
    {
    free(tmp_ptr->name);
    free(tmp_ptr);
    return IIFERR_NO_MEM;
    }

  if (ifp->syst_var == NULL)
    ifp->syst_var = tmp_ptr;
  else  /* seek to the end of the list */
    {
    list = ifp->syst_var;
    while (list->next != NULL)
      list = list->next;
    list->next = tmp_ptr;
    }

  strcpy(tmp_ptr->name, name);
  strcpy(tmp_ptr->value, value);
  tmp_ptr->next = NULL;
  tmp_ptr->nam_len = (BYTE)nam_len;
  tmp_ptr->val_len = (short int)val_len;
  tmp_ptr->type = STRING_VAR;
  return IIF_OK;
  }

/*--------------------------------------------------------------------------
 * NAME         write_sys_vars
 * PURPOSE      writes all system variables to a file
 * ARGUMENTS    ifp       a pointer to IIF file
 * RETURNS      an error code
 * EXTERNS      None
 --------------------------------------------------------------------------*/
static int  write_sys_vars(ifp)
IIFL  *ifp;                                       /* a pointer to IIF file */
  {
   char *vars_buff;                   /* a buffer for system vars area      */
   int  cntr;                         /* an auxiliary counter               */
   VLIST *elem;             /* a pointer to currently selected list element */
   INT64 fpos;                       /* a file pointer beyond sys vars area */

  if ((vars_buff = (BYTE *)calloc(1, ifp->cmnt_field_len)) == NULL)
    return IIFERR_NO_MEM;
  elem = ifp->syst_var;
  cntr = 0;
  while (elem != NULL)
    {
    strcpy(vars_buff + cntr, elem->name);
    cntr += strlen(elem->name);
    vars_buff[cntr++] = '=';
    strcpy(vars_buff + cntr, elem->value);
    cntr += strlen(elem->value);
    vars_buff[cntr++] = '\0';
    elem = elem->next;
    }

  if (cntr == 0)                    /*  no system vars found; that's rather */
    vars_buff[cntr++] = '\0';       /* impossible but it's more safe to put */
                                    /* trailing '\0' in this place          */
  vars_buff[cntr++] = '\0'; /* a second '\0' on the end of system vars area */

  if (os_fseek(ifp->file_handle, ifp->cmnt_field_offs, SEEK_SET) != 0)
    if (feof(ifp->file_handle) != 0)
      {
      free(vars_buff);
      return IIFERR_EOF;
      }
    else
      {
      free(vars_buff);
      return IIFERR_BAD_DISK;
      }

  if (fwrite(vars_buff, 1, (unsigned int)cntr, ifp->file_handle) !=
       (unsigned int)cntr)
    {
    free(vars_buff);
    return IIFERR_DISK_FULL;
    }

  if ((fpos = ftell(ifp->file_handle)) == -1L)
    {
    free(vars_buff);
    return IIFERR_BAD_DISK;
    }

  ifp->vars_offset = fpos;
  free(vars_buff);
  return IIF_OK;
  }

/*--------------------------- END OF MODULE -------------------------------*/
