/*
 * MODULE NAME  IMG_BUFF.C
 * PURPOSE      access to buffer with image data.
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
"@(#)$Header: /home1/repository/envi/REPOSITORY/envi/portab/iifl/img_buff.c,v 1.3 1999/03/12 10:11:53 envi Exp $ ";

/* CONDITIONAL COMPILATION SWITCHES */

#define TEST  0  /* testing   switch */

#include "integra.h"

/* COMPANY INCLUDES */

#include "iifl.h"

/* PROGRAM INCLUDES */

#include "iifl_own.h"


/* CONSTANTS */
#define  SIZE_OF_TWO_BYTES    2
#define  SIZE_OF_FOUR_BYTES   4
#define  SIZE_OF_EIGHT_BYTES  8

/* MACROS    */
 
/* STATICS   */

/* INTERFACE FUNCTIONS PROTOTYPES :

int  iif_write_line(IIF*, VPTR, int, int, int);
int  iif_read_line(IIF*, VPTR, int, int, int);
int  iif_fill(IIF*, int, int, int, int);
*/

/* LOCAL FUNCTIONS PROTOTYPES */

#ifndef NO_PROTOTYPES

static   VOID move_data(IIFL*, VPTR, int, int, int, int);
static   VOID fill_line(IIFL*, int, int, int);
static   VOID convert_fp(BYTE*, BYTE*, int, int, int);
static   VOID swap_area(BYTE*, int, int);

#else

static   VOID move_data();
static   VOID fill_line();
static   VOID convert_fp();
static   VOID swap_area();

#endif

/*************************** CODE ******************************/

/*==================== PUBLIC FUNCTIONS =============================*/

 /* --------------------------------------------------------------------------
 * NAME         iif_write_line
 * PURPOSE      writes one line to IIF file
 * ARGUMENTS    iflp         a pointer to opened IIF file
 *              table        a pointer to table with buffer addresses with data
 *              y            y origin of image area, line number
 *              fpix         a number of first pixel in line to write/update;
 *              npix         a number of pixels in line to write/update;
 * RETURNS       an error code.
 * EXTERNS       None
 --------------------------------------------------------------------------*/
int  iif_write_line(iflp, table, y, fpix, npix)
IIF  *iflp;          /* a pointer to opened IIF file                       */
VPTR  table;         /* a pointer to table with buffer addresses with data */
int   y;             /* y origin of image area, line number                */
int   fpix;          /* a number of first pixel in line to write/update;   */
int   npix;          /* a number of pixels in line to write/update;        */
  {
  int err;                                                /* an error code */
  IIFL  *ifp;                               /* a pointer to IIFL structure */

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

  if (ifp->attrib == 'r')
    {
    ifp->error = IIFERR_BAD_MODE;
    return IIFERR_BAD_MODE;
    }

  if (table == NULL || fpix < -1)
    {
    ifp->error = IIFERR_BAD_PARMS;
    return IIFERR_BAD_PARMS;
    }

  if (fpix != -1 && npix <= 0)
    {
    ifp->error = IIFERR_BAD_SIZE;
    return IIFERR_BAD_SIZE;
    }

  if (y < 0 || y > ifp->yres - 1)
    {
    ifp->error = IIFERR_BAD_ORG;
    return IIFERR_BAD_ORG;
    }

  if (ifp->buffer == NULL)
    if ((err = fl_first_access(ifp, y)) != IIF_OK)
      {
      ifp->error = err;
      return err;
      }

  if (fpix != -1 && fpix > ifp->xres)
    {
    ifp->error = IIFERR_BAD_SIZE;
    return IIFERR_BAD_SIZE;
    }

  if (y < ifp->first_line || y > (ifp->first_line + ifp->valid_lines - 1))
    {
    if (ifp->affected == TRUE)
      {
      if ((err = fl_write_lines(ifp, ifp->first_line, ifp->valid_lines)) !=
                 IIF_OK)
        {
        ifp->error = err;
        return err;
        }
      }

    ifp->first_line = y;
    ifp->valid_lines = (y + ifp->num_of_lines <= ifp->yres) ?
                        ifp->num_of_lines : ifp->yres - y;

    if (ifp->attrib == 'u')
      {
      if ((err = fl_read_lines(ifp, y, ifp->valid_lines)) != IIF_OK)
        {
        ifp->error = err;
        return err;
        }
      }
    }

  move_data(ifp, table, y - ifp->first_line, fpix, npix, 0);
  ifp->changed_lines[y] = LINE_CHANGED;
  ifp->affected = TRUE;
  return IIF_OK;
  }

 /* --------------------------------------------------------------------------
 * NAME         iif_read_line
 * PURPOSE      reads one line from IIF file
 * ARGUMENTS    ifp          a pointer to opened IIF file
 *              table        a pointer to table with buffer addresses for data
 *              y            y origin of image area, line number
 *              fpix         a number of first pixel in line to read
 *              npix         a number of pixels in line to read
 * RETURNS       an error code.
 * EXTERNS       None
 --------------------------------------------------------------------------*/
int  iif_read_line(iflp, table, y, fpix, npix)
IIF  *iflp;          /* a pointer to opened IIF file                       */
VPTR  table;         /* a pointer to table with buffer addresses for  data */
int   y;             /* y origin of image area, line number                */
int   fpix;          /* a number of first pixel in line to read            */
int   npix;          /* a number of pixels in line to read                 */
  {
  int err;                                                /* an error code */
  IIFL  *ifp;                               /* a pointer to IIFL structure */

  ifp = (IIFL *)iflp;

  if (fl_check_iifstr_ptr(ifp) != IIF_OK)
    {
    IIF_errno = IIFERR_BAD_PTR;
    return IIFERR_BAD_PTR;
    }

  if (table == NULL || fpix < -1)
    {
    ifp->error = IIFERR_BAD_PARMS;
    return IIFERR_BAD_PARMS;
    }

  if (fpix != -1 && npix <= 0)
    {
    ifp->error = IIFERR_BAD_SIZE;
    return IIFERR_BAD_SIZE;
    }

  if (fpix != -1 && fpix > ifp->xres)
    {
    ifp->error = IIFERR_BAD_SIZE;
    return IIFERR_BAD_SIZE;
    }

  if (ifp->attrib == 'w')
    {
    ifp->error = IIFERR_BAD_MODE;
    return IIFERR_BAD_MODE;
    }

  if (y < 0 || y > ifp->yres - 1)
    {
    ifp->error = IIFERR_BAD_ORG;
    return IIFERR_BAD_ORG;
    }

  if (ifp->initialized != 1)
    {
    ifp->error = IIFERR_NOT_INIT;
    return IIFERR_NOT_INIT;
    }

  if (ifp->buffer == NULL)
    if ((err = fl_first_access(ifp, y)) != IIF_OK)
      {
      ifp->error = err;
      return err;
      }

  if (y < ifp->first_line || y > (ifp->first_line + ifp->valid_lines - 1))
    {
    if (ifp->affected == TRUE)
      {
      if ((err = fl_write_lines(ifp, ifp->first_line, ifp->valid_lines)) !=
                 IIF_OK)
        {
        ifp->error = err;
        return err;
        }
      }

    ifp->first_line = y;
    ifp->valid_lines = (y + ifp->num_of_lines <= ifp->yres) ?
                        ifp->num_of_lines : ifp->yres - y;

    if ((err = fl_read_lines(ifp, y, ifp->valid_lines)) != IIF_OK)
      {
      ifp->error = err;
      return err;
      }
    }

  move_data(ifp, table, y - ifp->first_line, fpix, npix, 1);
  return IIF_OK;
  }

 /* --------------------------------------------------------------------------
 * NAME         iif_fill
 * PURPOSE      fills 'h' lines of IIF file
 * ARGUMENTS    ifp          a pointer to opened IIF file
 *              y            y origin of image area, line number
 *              h            heigth of the area
 *              fpix         a number of first pixel in line to fill
 *              npix         a number of pixels in line to fill
 * RETURNS       an error code.
 * EXTERNS       None
 --------------------------------------------------------------------------*/
int  iif_fill(iflp, y, h, fpix, npix)
IIF  *iflp;          /* a pointer to opened IIF file                       */
int   y;             /* y origin of image area, line number                */
int   h;             /* a heigth of the area                               */
int   fpix;          /* a number of first pixel in line to read            */
int   npix;          /* a number of pixels in line to read                 */
  {
  int err;                                                /* an error code */
  int last;                                         /* last line to fill   */
  int k;                                            /* current line number */
  IIFL  *ifp;                               /* a pointer to IIFL structure */

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

  if (ifp->attrib == 'r')
    {
    ifp->error = IIFERR_BAD_MODE;
    return IIFERR_BAD_MODE;
    }

  if (y < 0 || y > ifp->yres)
    {
    ifp->error = IIFERR_BAD_ORG;
    return IIFERR_BAD_ORG;
    }

  if (fpix != -1 && npix <= 0)
    {
    ifp->error = IIFERR_BAD_SIZE;
    return IIFERR_BAD_SIZE;
    }

  if (fpix < -1 || h <= 0)
    {
    ifp->error = IIFERR_BAD_PARMS;
    return IIFERR_BAD_PARMS;
    }

  if (fpix != -1 && fpix > ifp->xres)
    {
    ifp->error = IIFERR_BAD_SIZE;
    return IIFERR_BAD_SIZE;
    }

  if (ifp->fill_val == NULL)
    {
    ifp->error = IIFERR_FILL_NDEF;
    return IIFERR_FILL_NDEF;
    }

  if (ifp->buffer == NULL)
    if ((err = fl_first_access(ifp, y)) != IIF_OK)
      {
      ifp->error = err;
      return err;
      }

  if (y < ifp->first_line || y > (ifp->first_line + ifp->valid_lines))
    {
    if (ifp->affected == TRUE)
      {
      if ((err = fl_write_lines(ifp, ifp->first_line, ifp->valid_lines)) !=
                 IIF_OK)
        {
        ifp->error = err;
        return err;
        }
      }

    ifp->first_line = y;
    ifp->valid_lines = (y + ifp->num_of_lines <= ifp->yres) ?
                        ifp->num_of_lines : ifp->yres - y;

    if (ifp->attrib == 'u')   /* in write mode - no reading from file! */
      {
      if ((err = fl_read_lines(ifp, y, ifp->valid_lines)) != IIF_OK)
        {
        ifp->error = err;
        return err;
        }
      }
    }

  last = (y + h <= ifp->yres) ? y + h : ifp->yres;
  for (k = y; k < last; k++)
    {
    if (k < ifp->first_line || k > (ifp->first_line + ifp->valid_lines - 1))
      {
      if ((err = fl_write_lines(ifp, ifp->first_line, ifp->valid_lines)) !=
                 IIF_OK)
        {
        ifp->error = err;
        return err;
        }

      ifp->first_line = k;
      ifp->valid_lines = (k + ifp->num_of_lines <= ifp->yres) ?
                          ifp->num_of_lines : ifp->yres - k;

      if (ifp->attrib == 'u')   /* in write mode - no reading from file! */
        {
        if ((err = fl_read_lines(ifp, k, ifp->valid_lines)) != IIF_OK)
          {
          ifp->error = err;
          return err;
          }
        }
      }

    fill_line(ifp, k - ifp->first_line, fpix, npix);
    ifp->changed_lines[k] = LINE_CHANGED;
    }

  ifp->affected = TRUE;
  return IIF_OK;
  }


/*==================== LOCAL FUNCTIONS ==============================*/

/*--------------------------------------------------------------------------
 * NAME         fill_line
 * PURPOSE      fills one line in lines buffer
 * ARGUMENTS    ifp          a pointer to opened IIF file
 *              y            y origin of image area, line number
 *              fpix         a number of first pixel in line to fill
 *              npix         a number of pixels in line to fill
 * RETURNS       an error code.
 * EXTERNS       None
 --------------------------------------------------------------------------*/
static VOID fill_line(ifp, y, fpix, npix)
IIFL  *ifp;          /* a pointer to opened IIF file                       */
int   y;             /* y origin of image area, line number                */
int   fpix;          /* a number of first pixel in line to read            */
int   npix;          /* a number of pixels in line to read                 */
  {
  unsigned long bytes_per_line;   /* size of one line in bytes             */
  BYTE  *beginning;               /* beginning of line in buffer to fill   */
  BOOL swap_bytes;                /* flag; TRUE if byte swapping is needed */
  BOOL float_conv;  /* flag; TRUE if floating point conversion is required */
  int all_cmps;                   /* amount of all components in ifp file  */
  register int n, k, i;           /* an iteration counters                 */
  register BYTE size;             /* a size of component (in bytes)        */
  register int  amount;           /* amount of pixels in line t fill       */
  BYTE file_fp_format;     /* a floating point format obligatory in a file */
  char   char_fill;              /* a temporary varable for fill value     */
  BYTE   byte_fill;              /* a temporary varable for fill value     */
  double double_fill;            /* a temporary varable for fill value     */
  float  float_fill;             /* a temporary varable for fill value     */
  long   long_fill;              /* a temporary varable for fill value     */
  short  short_fill;             /* a temporary varable for fill value     */
  register BYTE *src, *dest;    /* pointers to data source and destination */

  if ((ifp->swap == TRUE  && IIF_os_byte_order == 0) ||
      (ifp->swap == FALSE && IIF_os_byte_order == 1))
    {
    swap_bytes = ifp->enable_swap;
    }
  else
    swap_bytes = FALSE;

  float_conv  = (BOOL)((fl_get_float_ver(ifp) == IIF_os_fp_format) ?
                                                              FALSE : TRUE);
  file_fp_format =  fl_get_float_ver(ifp);
  all_cmps = ifp->cmps_number;
  bytes_per_line = (unsigned long)(ifp->buffer_size / ifp->num_of_lines);
  beginning = (BYTE*)(ifp->buffer) + bytes_per_line * y;
  if (fpix == -1)
    amount = ifp->xres;
  else
    amount = (fpix + npix <= ifp->xres) ? npix : ifp->xres;

  for (n = 0; n < all_cmps; n++)
    {
    size = *(ifp->lcmps + n);
    dest =(BYTE*)(beginning + fl_get_comp_offset(ifp, n));

    switch (*(ifp->types + n))
      {
      case 'a': char_fill = *(*((char**)(ifp->fill_val) + n));
                dest = (BYTE*)(beginning + fl_get_comp_offset(ifp, n));
                src = (BYTE*)&char_fill;

           break;
      case 'b': byte_fill = *(*((BYTE**)(ifp->fill_val) + n));
                dest = (BYTE*)(beginning + fl_get_comp_offset(ifp, n));
                src = (BYTE*)&byte_fill;

           break;
      case 'd': double_fill = *(*((double**)(ifp->fill_val) + n));
                dest = (BYTE*)(beginning + fl_get_comp_offset(ifp, n));
                src = (BYTE*)&double_fill;

           break;
      case 'f': float_fill = *(*((float**)(ifp->fill_val) + n));
                dest = (BYTE*)(beginning + fl_get_comp_offset(ifp, n));
                src = (BYTE*)&float_fill;

                if (float_conv == TRUE)
                  convert_fp((BYTE*)(&float_fill), (BYTE*)(&float_fill), 1,
                              (int)IIF_os_fp_format, (int)file_fp_format);

           break;
      case 'l': long_fill = *(*((long**)(ifp->fill_val) + n));
                dest = (BYTE*)(beginning + fl_get_comp_offset(ifp, n));
                src = (BYTE*)&long_fill;

           break;
      case 's': short_fill = *(*((short**)(ifp->fill_val) + n));
                dest = (BYTE*)(beginning + fl_get_comp_offset(ifp, n));
                src  = (BYTE*)&short_fill;

           break;
      default:
#ifdef DEBUG
printf("MODULE: img_buff, func.: fill_line - Unknown component type '%d'\n",
       *(ifp->types + n))
#endif
              ;
            break;
      }

      if (size > 1 && swap_bytes == TRUE)
        swap_area(src, 1, (int)size);

      if (fpix != -1)
        dest += fpix * size;

      for (k = 0; k < amount; k++)
        for (i = 0; i < (int)size; i++)
          *dest++ = src[i];
    }
  }

/*--------------------------------------------------------------------------
 * NAME         swap_area
 * PURPOSE      swaps bytes in given area
 * ARGUMENTS    buffer       a pointer of  beginning of swapped area
 *              amount       amount of items to swap
 *              size         a size of each item
 * RETURNS      None
 * EXTERNS      None
 --------------------------------------------------------------------------*/
static VOID  swap_area(buffer, amount, size)
BYTE  *buffer;                /* a pointer of  beginning of swapped area */
int    amount;                /* amount of items to swap                 */
int    size;                  /* a size of each item                     */
  {
  BYTE tmp;                   /* temporary value                         */
  int  n;                     /* an auxiliary iteration counter          */

  if (size == SIZE_OF_TWO_BYTES)
    {
    for (n = 0; n < amount; n++)
       {
       tmp = *buffer;
       *buffer = *(buffer + 1);
       *(buffer + 1) = tmp;
       buffer += 2;
       }
    }
  else
    if (size == SIZE_OF_FOUR_BYTES)
      for (n = 0; n < amount; n++)
         {
         tmp = *buffer;
         *buffer = *(buffer + 3);
         *(buffer + 3) = tmp;
         tmp = *(buffer + 2);
         *(buffer + 2) = *(buffer + 1);
         *(buffer + 1) = tmp;
         buffer += 4;
         }
  else
    if (size == SIZE_OF_EIGHT_BYTES)
      for (n = 0; n < amount; n++)
         {
         tmp = *buffer;
         *buffer = *(buffer + 7);
         *(buffer + 7) = tmp;
         tmp = *(buffer + 6);
         *(buffer + 6) = *(buffer + 1);
         *(buffer + 1) = tmp;
         tmp = *(buffer + 5);
         *(buffer + 5) = *(buffer + 2);
         *(buffer + 2) = tmp;
         tmp = *(buffer + 4);
         *(buffer + 4) = *(buffer + 3);
         *(buffer + 3) = tmp;
         buffer += 8;
         }
  }

/*--------------------------------------------------------------------------
 * NAME         move_data
 * PURPOSE      copies pixel data between lines buffer and user's buffers
 * ARGUMENTS    ifp          a pointer to opened IIF file
 *              table        a pointer to a table with pointers to buffers
 *                           for data, one buffer for one component
 *              line         number of line in buffer to move
 *              fpix         a number of first pixel in line to access; if set
 *                           to -1, all pixels have to be accessed.
 *              npix         a number of pixels in line to access; has no
 *                           meaning if fpix is set to -1.
 *              dir          direction in which data are copied;
 *                           0 - from user's buffer to lines buffer;
 *                           1 - from lines buffer to user's buffers;
 * RETURNS       an error code.
 * EXTERNS       None
 --------------------------------------------------------------------------*/
static VOID move_data(ifp, table, line, fpix, npix, dir)
IIFL  *ifp;           /* a pointer to opened IIF file                      */
VPTR  table;          /* a pointer to a table with pointers to buffers     */
int   line;           /* line number in image buffer                       */
int   fpix;           /* for data, one buffer for one component            */
                      /* a number of first pixel in line to access; if set */
int   npix;           /* to -1, all pixels have to be accessed.            */
                      /* a number of pixels in line to access; has no      */
int   dir;            /* meaning if fpix is set to -1.                     */
                      /* direction in which data are copied;               */
                      /* 0 - from user's buffer to lines buffer;           */
                      /* 1 - from lines buffer to user's buffers;          */
  {
  unsigned long bytes_per_line;   /* size of one line in bytes             */
  BYTE  *beginning;               /* beginning of line in buffer to fill   */
  BOOL swap_bytes;                /* flag; TRUE if byte swapping is needed */
  BOOL float_conv;  /* flag; TRUE if floating point conversion is required */
  int all_cmps;                   /* amount of all components in ifp file  */
  int n;                          /* an iteration counters                 */
  unsigned short size;            /* a size of component (in bytes)        */
  BYTE file_fp_format;     /* a floating point format obligatory in a file */
  BYTE   *src, *dest;
  BYTE   *temp;             /* a temporary pointer for exchange src<->dest */

  if ((ifp->swap == TRUE  && IIF_os_byte_order == 0) ||
      (ifp->swap == FALSE && IIF_os_byte_order == 1))
    {
    swap_bytes = ifp->enable_swap;
    }
  else
    swap_bytes = FALSE;

  float_conv  = (BOOL)((fl_get_float_ver(ifp) == IIF_os_fp_format) ?
                                                            FALSE : TRUE);
  file_fp_format =  fl_get_float_ver(ifp);
  all_cmps = ifp->def_cmps_amount;
  bytes_per_line = (unsigned long)(ifp->buffer_size / ifp->num_of_lines);
  beginning = (BYTE*)(ifp->buffer) + bytes_per_line * line;
  if (fpix == -1)
    npix = ifp->xres;
  else
    npix = (fpix + npix <= ifp->xres) ? npix : ifp->xres;

  for (n = 0; n < all_cmps; n++)
    {
    size = *(ifp->def_cmps_sizes + n);
    src = *((BYTE**)table + *(ifp->def_cmps_position + n));
    dest = (BYTE*)(beginning + *(ifp->def_cmps_offsets + n));

    if (fpix != -1)
      {
      src += fpix * size;
      dest += fpix * size;
      }

    if (dir == 1)        /* replace source with destination */
      {
      temp = src;
      src = dest;
      dest = temp;
      }

    switch (*(ifp->def_cmps_types + n))
      {
      case 'a':
      case 'b': memcpy(dest, src, npix * size);
           break;
      case 'd':
      case 'l':
      case 's': memcpy(dest, src, npix * size);

                if (swap_bytes == TRUE)
                  swap_area(dest, npix, size);
           break;
      case 'f': if (float_conv == TRUE)
                  {
                  if (dir == 0)
                    convert_fp(dest, src, npix, (int)IIF_os_fp_format,
                               (int)file_fp_format);
                  else
                    convert_fp(dest, src, npix, (int)file_fp_format,
                               (int)IIF_os_fp_format);
                  }
                else
                  memcpy(dest, src, npix * size);

                if (swap_bytes == TRUE)
                  swap_area(dest, npix, size);
           break;
      default :
#ifdef DEBUG
   printf("MODULE: img_buff.c, FUNC. : move_data; Unknown component type !\n");
#endif
           break;
      }
    }
  }

/*--------------------------------------------------------------------------
 * NAME         convert_fp
 * PURPOSE      converts one floating point format to another
 * ARGUMENTS    buffer       a pointer with data to convert
 *              amount       amount of items to convert
 *              in_format    input_format
 *              out_format   output (required) format
 * RETURNS      None
 * EXTERNS      None
 --------------------------------------------------------------------------*/
static VOID  convert_fp(source, dest, amount, in_format, out_format)
BYTE  *source;                           /* a pointer with data to convert */
BYTE  *dest;                             /* a pointer to destination place */
int    amount;                           /* amount of items to convert     */
int    in_format;                        /* input_format                   */
int    out_format;                       /* output (required) format       */
  {
  /* since only one format is in use (and is really known), this function
     presently is doing nothing                                            */
           /* to avoid MICROSOFT C 6.0 warnings: */
  source = source;
  dest = dest;
  amount = amount;
  in_format = in_format;
  out_format = out_format;

  }

/*--------------------------- END OF MODULE -------------------------------*/
