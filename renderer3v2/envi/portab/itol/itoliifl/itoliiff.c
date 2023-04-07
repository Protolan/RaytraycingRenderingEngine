/* FILE NAME    itoliiff.c
 * PURPOSE      Overlay to read IIFL files.
 * 
 * SPEC         NRC                                  '91.01
 * DESIGN       NRC                                  '91.01
 * DES. VV      NRC                                  '91.01
 * CODING       NRC                                  '91.01
 * CODE VV.     NRC                                  '91.01
 * TEST         NRC                                  '91.01
 * LAST UPDATED Robert Chyla                         '94.12.06
 *
 * NOTES        None.
 * 
 */

static char *sccsid =
"@(#)$Header: /home1/repository/envi/REPOSITORY/envi/portab/itol/itoliifl/itoliiff.c,v 1.2 1998/11/19 13:52:40 envi Exp $";

#include "integra.h"
#include "itoliiff.h"
#include "iosl.h"
#include "iifl.h"

#define DEFAULT_PIX_SIZE 10

static BOOL opened = FALSE;        /* is file opened?     */

static IIF *f;                     /* file handle         */

static int x_res, y_res;           /* resolution of image */
static int x_pix_size, y_pix_size; /* size of pixel       */
static int x_origin, y_origin;     /* image origin        */

/* definition of fields from IIFL header */
static char component_names[]     = "red,gre,blu";
static char pixel_size[]          = "pixel size [0.1 mm]";
static char image_screen_offset[] = "image screen offset";

/*
 * NAME         int Iiff_Open_Read(name)
 * PURPOSE      Open IIFL file for reading
 * ARGUMENTS    Name of file to be opened
 * RETURNS      SUCCESS - OK, FAILURE - some error
 */

int Iiff_Open_Read(name)
IN char *name; /* name of file to be read */
  {
  char *var;                  /* variable pointer (from IIFL file header) */

  /* close, when was not closed explicitly */
  if (opened)
    iif_close(f);

  opened = FALSE;

  if ((f = iif_open(name, "r")) == NULL)
    return FAILURE;

  /* get resolution */
  iif_get_resolution(f, &x_res, &y_res);

  /* get pixel size */
  x_pix_size = 0;
  y_pix_size = 0;
  var = iif_get_var(f, pixel_size);
  if (var != NULL)
    sscanf(var, "%d %d", &x_pix_size, &y_pix_size);

  /* adjust pixel size (0 is not much pleasant value) */
  if (x_pix_size == 0)
    x_pix_size = DEFAULT_PIX_SIZE;
  if (y_pix_size == 0)
    y_pix_size = DEFAULT_PIX_SIZE;

  /* get origin */
  x_origin = 0;
  y_origin = 0;
  var = iif_get_var(f, image_screen_offset);
  if (var != NULL)
    sscanf(var, "%d %d", &x_origin, &y_origin);

  /* RGB components MUST exist, otherwise error */
  if (iif_def_components(f, 1, component_names) != IIF_OK)
    {
    iif_close(f);
    return FAILURE;
    }

  opened = TRUE;
  return SUCCESS;
  }


/*
 * NAME         int Iiff_Close_Read()
 * PURPOSE      Close file opened for reading
 * ARGUMENTS    None
 * RETURNS      SUCCESS - OK, FAILURE - improper calling or cannot close
 */

int Iiff_Close_Read()
  {
  /* error, when inproper calling */
  if (!opened)
    return FAILURE;

  opened = FALSE;

  if (iif_close(f))
    return FAILURE;

  return SUCCESS;
  }


/*
 * NAME         int Iiff_Get_Resolution(x, y)
 * PURPOSE      Get resolution of opened image
 * ARGUMENTS    None
 * RETURNS      SUCCESS - OK, FAILURE - file not opened
 */

int Iiff_Get_Resolution(x, y)
OUT int *x, *y; /* resultant image resolution */
  {
  /* error, when inproper calling */
  if (!opened)
    return FAILURE;

  /* pass local variables to caller */
  *x = x_res;
  *y = y_res;
  return SUCCESS;
  }


/*
 * NAME         int Iiff_Get_Pixel_Size(x, y)
 * PURPOSE      Get pixel size of opened image
 * ARGUMENTS    None
 * RETURNS      SUCCESS - OK, FAILURE - file not opened
 */

int Iiff_Get_Pixel_Size(x, y)
OUT int *x, *y; /* resultant pixel size */
  {
  /* error, when inproper calling */
  if (!opened)
    return FAILURE;

  /* pass local variables to caller */
  *x = x_pix_size;
  *y = y_pix_size;
  return SUCCESS;
  }


/*
 * NAME         int Iiff_Get_Origin(x, y)
 * PURPOSE      Get image origin
 * ARGUMENTS    None
 * RETURNS      SUCCESS - OK, FAILURE - file not opened
 */

int Iiff_Get_Origin(x, y)
OUT int *x, *y; /* resultant image origin */
  {
  /* error, when inproper calling */
  if (!opened)
    return FAILURE;

  /* pass local variables to caller */
  *x = x_origin;
  *y = y_origin;
  return SUCCESS;
  }


/*
 * NAME         int Iiff_Get_RGB_Row(y, r, g, b)
 * PURPOSE      Get RGB row from opened file
 * ARGUMENTS    Row index and pointers to output buffers for RGB components
 * RETURNS      SUCCESS or FAILURE. In case of SUCCESS output buffers are set
 */

int Iiff_Get_RGB_Row(y, r, g, b)
IN int y;            /* index of row to be read */
OUT BYTE *r, *g, *b; /* pointer to buffers for RGB components */
  {
  BYTE *table[3]; /* pointers to output buffers (RGB triple) */

  /* error, when inproper calling */
  if (!opened)
    return FAILURE;

  /* error, when inproper wrong parameter */
  if (y < 0 || y >= y_res)
    return FAILURE;

  /* define table of pointers needed by iif_read_line (see IIFL UM) */
  table[0] = r;
  table[1] = g;
  table[2] = b;

  /* and read line to buffers */
  if (iif_read_line(f, (VPTR)table, y, -1, 0) != IIF_OK)
    return FAILURE;

  return SUCCESS;
  }

/* END OF 'itoliiff.c' FILE */

