/* -------------------------------------------------------- *
 * FILE NAME     icol.c                                     *
 * PURPOSE       Integra COlor Library                      *
 *                                                          *
 * SPEC          Alex Voloboy (Rav) lcol02dm.Rav  '93.11.24 *
 * DESIGN        Alex Voloboy (Rav) lcol01df.Rav  '93.11.28 *
 * DES. VV       Oek, Andrei Khodulev (Sah)       '93.11.30 *
 * CODING        Alex Voloboy (Rav)               '93.12.07 *
 * UPDATED       Edward Kopylov (Oek)             '96.02.07
 * TESTING       Edward Kopylov (Oek)             '96.02.07
 *                                                          *
 * NOTES         Spec. are in file icol_200.um              *
 *                                                          *
 * -------------------------------------------------------- */

static char *sccsid =
"@(#)/repository/envi/REPOSI/ENVI/portab/icol/icol.c,v 1.3 1996/08/29 11:28:07 envi Exp";

#define DEFINE_GLOBALS
#include "integra.h"
#include "imal.h"
#include "icol.h"
#include "icol_own.h"
#include "algebra.h"
#undef DEFINE_GLOBALS

/* ICOL error messages */

#define COL_MAX_ERROR_CODE    4
static  char    *co_error_msgs[] = {
                       "ICOL: All Correct",
                       "ICOL: Library Is Not Initiated",
                       "ICOL: Input Value Is Incorrect",
                       "ICOL: Memory Is Not Enough",
                       "ICOL: RGB Values Is Out Of Color Space"
                                   };
static  char    *co_undefined_msg = "ICOL: Unknown Error Code";

/* -------------------------------------------------------- *
 *  FUNCTION    col_term                                    *
 *  PURPOSE     Terminate ICOL work                         *
 *                                                          *
 *  ARGUMENTS   None                                        *
 *                                                          *
 *  RETURN      None                                        *
 *                                                          *
 *  GLOBALS     co_wavelength, co_x_curve, co_y_curve,      *
 *              co_z_curve, co_wl_alloc, co_xc_alloc,       *
 *              co_yc_alloc, co_zc_alloc, co_error_code,    *
 *              co_light_wl, co_light, co_wlli_alloc,       *
 *              co_li_alloc                                 *
 *                                                          *
 *  NOTES       Free all allocated memory.                  *
 *                                                          *
 * -------------------------------------------------------- */

VOID col_term()
  {

  co_error_code = COL_ERR_NOT_INIT;

  /* free allocated memory */
  if (co_wl_alloc == YES)
    {
    mem_free((VPTR)co_wavelength);
    co_wl_alloc = NO;
    }

  /* free allocated memory */
  if (co_xc_alloc == YES)
    {
    mem_free((VPTR)co_x_curve);
    co_xc_alloc = NO;
    }

  /* free allocated memory */
  if (co_yc_alloc == YES)
    {
    mem_free((VPTR)co_y_curve);
    co_yc_alloc = NO;
    }

  /* free allocated memory */
  if (co_zc_alloc == YES)
    {
    mem_free((VPTR)co_z_curve);
    co_zc_alloc = NO;
    }

  /* free allocated memory */
  if (co_wlli_alloc == YES)
    {
    mem_free((VPTR)co_light_wl);
    co_wlli_alloc = NO;
    }

  /* free allocated memory */
  if (co_li_alloc == YES)
    {
    mem_free((VPTR)co_light);
    co_li_alloc = NO;
    }

  /* free allocated memory */
  if (co_lum_alloc == YES)
    {
    mem_free((VPTR)co_lum[0]);
    co_lum_alloc = NO;
    }

  /* Sometime it is difficult to destroy
   * ICOL handler property (ref. to TBTMATH lib).
   * This function to release these resources if any.
   */
  col_obj_destroy_all();

#ifndef DEBUG
  mem_class_close(co_mem_class);
#else
  if (mem_class_close(co_mem_class) != MEM_OK)
    {
    fprintf(stderr, "%s\n", mem_strerr(mem_error()));
    mem_print_class(stderr, co_mem_class);
    }
#endif

  co_icol_init = NO;
  }

/* -------------------------------------------------------- *
 *  FUNCTION    col_set_xyz                                 *
 *  PURPOSE     Function sets new values for XYZ-curves.    *
 *                                                          *
 *  ARGUMENTS                                               *
 *      INPUT   'number' - number of values defining curves *
 *              'first' - minimum value of wavelength for   *
 *                        which XYZ values will be defined  *
 *              'last' - maximum value of wavelength for    *
 *                       which XYZ values  will be defined  *
 *              'cie_x', 'cie_y', 'cie_z' - value arrays    *
 *                                          containing the  *
 *                                          "number" double *
 *                                          numbers         *
 *      OUTPUT  None.                                       *
 *                                                          *
 *  RETURN      COL_OK - function is finished successfully  *
 *              COL_ERR_NOT_INIT - if ICOL is not initiated *
 *              COL_ERR_NOT_ENOUGH_MEMORY - if function     *
 *                                   can't allocate memory  *
 *              COL_ERR_WRONG_VALUE - if input data are     *
 *                                    invalid               *
 *                                                          *
 *  GLOBALS     co_wavelength, co_x_curve, co_y_curve,      *
 *              co_z_curve, co_wl_alloc, co_xc_alloc,       *
 *              co_yc_alloc, co_zc_alloc, co_error_code,    *
 *              co_curve_points, co_icol_init               *
 *                                                          *
 *  NOTES       None.                                       *
 *                                                          *
 * -------------------------------------------------------- */


int col_set_xyz(number, first, last, cie_x, cie_y, cie_z)
  IN   int    number; /* number of values */
  IN   double first;  /* minimum value of wavelength */
  IN   double last;   /* maximum value of wavelength */
  IN   double *cie_x; /* X values */
  IN   double *cie_y; /* Y values */
  IN   double *cie_z; /* Z values */
  {
  int  i;            /* tmp variable */
  double  interval;  /* wavelength interval */

  /* assertion: library is initiated */
  if (co_icol_init == NO)
    return (COL_ERR_NOT_INIT);

  co_error_code = COL_OK;

  /* assertion: number > 1 && 0 <= first < last */
  if (number <= 1 || first < 0 || first >= last)
    {
    co_error_code = COL_ERR_WRONG_VALUE;
    return (COL_ERR_WRONG_VALUE);
    }

  co_curve_points = number;

  /* free memory allocated in col_init() */
  if (co_wl_alloc == YES)
    {
    mem_free((VPTR)co_wavelength);
    co_wl_alloc = NO;
    }

  if (co_xc_alloc == YES)
    {
    mem_free((VPTR)co_x_curve);
    co_xc_alloc = NO;
    }

  if (co_yc_alloc == YES)
    {
    mem_free((VPTR)co_y_curve);
    co_yc_alloc = NO;
    }

  if (co_zc_alloc == YES)
    {
    mem_free((VPTR)co_z_curve);
    co_zc_alloc = NO;
    }

  /* allocate memory for new XYZ-curves and wavelength arrays */

  co_wavelength = (double *)mem_alloc(co_mem_class,
                  "co_wavelength", co_curve_points * sizeof(double));
  if (co_wavelength == NULL)
    {
    co_error_code = COL_ERR_NOT_ENOUGH_MEMORY;
    return (COL_ERR_NOT_ENOUGH_MEMORY);
    }
  co_wl_alloc = YES;

  /* allocation memory for X-curve */
  co_x_curve = (double *)mem_alloc(co_mem_class,
               "co_x_curve", co_curve_points * sizeof(double));
  if (co_x_curve == NULL)
    {
    co_xc_alloc = NO;
    co_error_code = COL_ERR_NOT_ENOUGH_MEMORY;
    return (COL_ERR_NOT_ENOUGH_MEMORY);
    }
  co_xc_alloc = YES;

  /* allocation memory for Y-curves */
  co_y_curve = (double *)mem_alloc(co_mem_class,
               "co_y_curve", co_curve_points * sizeof(double));
  if (co_y_curve == NULL)
    {
    co_yc_alloc = NO;
    co_error_code = COL_ERR_NOT_ENOUGH_MEMORY;
    return (COL_ERR_NOT_ENOUGH_MEMORY);
    }
  co_yc_alloc = YES;

  /* allocation memory for XYZ-curves and array for wavelength */
  co_z_curve = (double *)mem_alloc(co_mem_class,
               "co_z_curve", co_curve_points * sizeof(double));
  if (co_z_curve == NULL)
    {
    co_zc_alloc = NO;
    co_error_code = COL_ERR_NOT_ENOUGH_MEMORY;
    return (COL_ERR_NOT_ENOUGH_MEMORY);
    }
  co_zc_alloc = YES;


  /* fill wavelength array with values from
           'first' to 'last' by (last - first) / (number - 1) nm */

  interval = (last - first) / (number - 1);
  for (i = 0; i < number; i++)
    co_wavelength[i] = first + (interval * i);

  /* fill XYZ-curves arrays */
  for (i = 0; i < number; i++)
    {
    co_x_curve[i] = cie_x[i];
    co_y_curve[i] = cie_y[i];
    co_z_curve[i] = cie_z[i];
    }

  return (COL_OK);
  }
/* -------------------------------------------------------- *
 *  FUNCTION    col_spec_xyz                                *
 *  PURPOSE     convert spectrogram values into CIE XYZ     *
 *              coordinates of color                        *
 *                                                          *
 *  ARGUMENTS                                               *
 *      INPUT   number - number of values defining          *
 *                       spectrogram                        *
 *              wavelength - array with wavelengths for     *
 *                           which spectrogram is defined   *
 *              spec_gram - values of spectrogram           *
 *      OUTPUT  cie_xyz - CIE XYZ coordinates of color      *
 *                                                          *
 *  RETURN      COL_OK - function is finished successfully  *
 *              COL_ERR_NOT_INIT - if ICOL is not initiated *
 *              COL_ERR_WRONG_VALUE - if input data are     *
 *                                    invalid               *
 *                                                          *
 *  GLOBALS     co_error_code, co_x_curve, co_y_curve,      *
 *              co_z_curve, co_wavelength, co_curve_points  *
 *              co_icol_init                                *
 *                                                          *
 *  NOTES       None.                                       *
 *                                                          *
 * -------------------------------------------------------- */

int col_spec_xyz(number, wavelength, spec_gram, cie_xyz)
  IN  int number;
  IN  double *wavelength;
  IN  double *spec_gram;
  OUT double *cie_xyz;
  { 
  int i;

  /* assertion: library is initiated */
  if (co_icol_init == NO)
    return (COL_ERR_NOT_INIT);

  co_error_code = COL_OK;

  /* assertion: number > 0 &&
                 wavelength[i] <= wavelength[i + 1] */
  if (number <= 0)
    {
    co_error_code = COL_ERR_WRONG_VALUE;
    return (COL_ERR_WRONG_VALUE);
    }

  for (i = 0; i < number - 1; i++)
    {
    if (wavelength[i] > wavelength[i + 1])
      {
      co_error_code = COL_ERR_WRONG_VALUE;
      return (COL_ERR_WRONG_VALUE);
      }
    }

  /* calculate XYZ coordinates of color from spectrogram */
  co_xyz_calculation(cie_xyz,
                     spec_gram, wavelength, number,
                     co_x_curve, co_y_curve, co_z_curve,
                     co_wavelength, co_curve_points);

  return (COL_OK);
  }

/* -------------------------------------------------------- *
 *  FUNCTION    col_set_wrgb                                *
 *  PURPOSE     Function sets new co_xyz_2_rgb and          *
 *              co_rgb_2_xyz matrixes                       *
 *                                                          *
 *  ARGUMENTS                                               *
 *      INPUT   white - (x, y) coordinates for white color  *
 *              red   - (x, y) coordinates for red color    *
 *              green - (x, y) coordinates for green color  *
 *              blue  - (x, y) coordinates for blue color   *
 *      OUTPUT  None.                                       *
 *                                                          *
 *  RETURN      COL_OK - function is finished successfully  *
 *              COL_ERR_NOT_INIT - if ICOL is not initiated *
 *              COL_ERR_WRONG_VALUE - if input data are     *
 *                                    invalid               *
 *                                                          *
 *  GLOBALS     co_error_code, co_icol_init, co_w_current,  *
 *              co_r_current, co_g_current, co_b_current    *
 *                                                          *
 *  NOTES       None.                                       *
 *                                                          *
 * -------------------------------------------------------- */


int col_set_wrgb(white, red, green, blue)
  IN   double  *white;  /* color coordinates for white color */
  IN   double  *red;    /* color coordinates for red color */
  IN   double  *green;  /* color coordinates for green color */
  IN   double  *blue;   /* color coordinates for blue color */

  {
  int return_code;   /* return code of the convert matrix function */

  /* assertion: library is initiated */
  if (co_icol_init == NO)
    return (COL_ERR_NOT_INIT);

  co_error_code = COL_OK;

  /* assertion: all input values >= 0 &&
                 x + y <= 1 for any input color */
  if (white[0] < 0 || white[1] < 0 ||
      red[0] < 0 || red[1] < 0 ||
      green[0] < 0 || green[1] < 0 ||
      blue[0] < 0 || blue[1] < 0 ||
      white[0] + white[1] > 1 ||
      red[0] + red[1] > 1 ||
      green[0] + green[1] > 1 ||
      blue[0] + blue[1] > 1)
    {
    co_error_code = COL_ERR_WRONG_VALUE;
    return (COL_ERR_WRONG_VALUE);
    }

  /* if (white_input_value == 0)
       specify correspondent previous value */

  if (white[0] == 0.0 && white[1] == 0.0)
    COPY_2D(white, co_w_current)
  else
    COPY_2D(co_w_current, white)

  /* if (red_input_value == 0)
       specify correspondent previous value */

  if (red[0] == 0.0 && red[1] == 0.0)
    COPY_2D(red, co_r_current)
  else
    COPY_2D(co_r_current, red)

  /* if (green_input_value == 0)
       specify correspondent previous value */

  if (green[0] == 0.0 && green[1] == 0.0)
    COPY_2D(green, co_g_current)
  else
    COPY_2D(co_g_current, green)

  /* if (blue_input_value == 0)
       specify correspondent previous value */

  if (blue[0] == 0.0 && blue[1] == 0.0)
    COPY_2D(blue, co_b_current)
  else
    COPY_2D(co_b_current, blue)

  /* create co_xyz_2_rgb and co_rgb_2_xyz matrixes */
  return_code = co_create_xyz2rgb(co_xyz_2_rgb, co_rgb_2_xyz,
                                  red, green, blue, white);
  if (return_code != COL_OK)
    {
    co_error_code = return_code;
    return (return_code);
    }

  return (COL_OK);
  }


/* -------------------------------------------------------- *
 *  FUNCTION    col_get_wrgb                                *
 *  PURPOSE     Function gets current phosphor coordinates  *
 *              and reference white color                   *
 *                                                          *
 *  ARGUMENTS                                               *
 *      INPUT   None.                                       *
 *      OUTPUT  white - (x, y) coordinates for white color  *
 *              red   - (x, y) coordinates for red color    *
 *              green - (x, y) coordinates for green color  *
 *              blue  - (x, y) coordinates for blue color   *
 *                                                          *
 *  RETURN      COL_OK - function is finished successfully  *
 *              COL_ERR_NOT_INIT - if ICOL is not initiated *
 *                                                          *
 *  GLOBALS     co_error_code, co_icol_init, co_w_current,  *
 *              co_r_current, co_g_current, co_b_current    *
 *                                                          *
 *  NOTES       None.                                       *
 *                                                          *
 * -------------------------------------------------------- */


int col_get_wrgb(white, red, green, blue)
OUT double  *white;  /* color coordinates for white color */
OUT double  *red;    /* color coordinates for red color */
OUT double  *green;  /* color coordinates for green color */
OUT double  *blue;   /* color coordinates for blue color */

  {
  /* assertion: library is initiated */
  if (co_icol_init == NO)
    return COL_ERR_NOT_INIT;

  co_error_code = COL_OK;

  /* assertion: all values >= 0 && x + y <= 1 for any input color */
  ASSERT(co_w_current[0] >= 0 && co_w_current[1] >= 0 &&
         co_r_current[0] >= 0 && co_r_current[1] >= 0 &&
         co_g_current[0] >= 0 && co_g_current[1] >= 0 &&
         co_b_current[0] >= 0 && co_b_current[1] >= 0 &&
         co_w_current[0] + co_w_current[1] <= 1 &&
         co_r_current[0] + co_r_current[1] <= 1 &&
         co_g_current[0] + co_g_current[1] <= 1 &&
         co_b_current[0] + co_b_current[1] <= 1)

  COPY_2D(white, co_w_current)
  COPY_2D(red,   co_r_current)
  COPY_2D(green, co_g_current)
  COPY_2D(blue,  co_b_current)

  return COL_OK;
  }

/// Retrieves previously calculated XYZ colour of the
/// default light (set by the moment of calculation)
OKAY co_get_def_light_xyz(OUT double xyz[3])
  {
  /* assertion: library is initiated */
  if (co_icol_init == NO)
    return COL_ERR_NOT_INIT;

  COPY_3D(xyz, xyz_def_light);
  return SUCCESS;
  }

/* -------------------------------------------------------- *
 *  FUNCTION    col_xyz_rgb                                 *
 *  PURPOSE     convert CIE XYZ coordinates of color into   *
 *              RGB coordinates                             *
 *                                                          *
 *  ARGUMENTS                                               *
 *      INPUT   cie_xyz - CIE XYZ coordinates of color      *
 *              maxcolor - maximal value for R, G, B        *
 *                         coordinates                      *
 *      OUTPUT  rgb - RGB coordinates of color              *
 *                                                          *
 *  RETURN      COL_OK - if RGB values are "displayable"    *
 *              COL_ERR_NOT_INIT - if ICOL is not initiated *
 *              COL_ERR_WRONG_VALUE - if input data are     *
 *                                    invalid               *
 *              COL_ERR_OUT_OF_RANGE - if resultant color   *
 *                         is not contain in resultant RGB  *
 *                         color space                      *
 *                                                          *
 *  GLOBALS     co_error_code, co_xyz_2_rgb, co_icol_init   *
 *                                                          *
 *  NOTES       None.                                       *
 *                                                          *
 * -------------------------------------------------------- */

int  col_xyz_rgb(cie_xyz, maxcolor, rgb)
  IN   double *cie_xyz;  /* XYZ coordinates of color */
  IN   double maxcolor;  /* maximal color value in RGB color space */
  OUT  double *rgb;      /* RGB coordinates of color */
  {

  /* assertion: library is initiated */
  if (co_icol_init == NO)
    return (COL_ERR_NOT_INIT);

  co_error_code = COL_OK;

  /* assertion: maxcolor > 0 */
  if (maxcolor <= 0.0)
    {
    co_error_code = COL_ERR_WRONG_VALUE;
    return (COL_ERR_WRONG_VALUE);
    }

  /* convert XYZ -> RGB coordinates */
  AFIN_TRANSFORM_3x3(rgb, co_xyz_2_rgb, cie_xyz);

  rgb[0] /= maxcolor;
  rgb[1] /= maxcolor;
  rgb[2] /= maxcolor;

  /* if resultant color is out of the RGB color space */
  if (rgb[0] < 0 || rgb[0] > 1 ||
      rgb[1] < 0 || rgb[1] > 1 ||
      rgb[2] < 0 || rgb[2] > 1)
    {
    co_error_code = COL_ERR_OUT_OF_RANGE;
    return (COL_ERR_OUT_OF_RANGE);
    }

  return (COL_OK);
  }

/* -------------------------------------------------------- *
 *  FUNCTION    col_rgb_xyz                                 *
 *  PURPOSE     convert RGB coordinates of color into  CIE  *
 *              XYZ coordinates                             *
 *                                                          *
 *  ARGUMENTS                                               *
 *      INPUT   rgb - RGB coordinates of color              *
 *      OUTPUT  cie_xyz - CIE XYZ coordinates of color      *
 *                                                          *
 *  RETURN      COL_OK - if color coordinates are converted *
 *              COL_ERR_NOT_INIT - if ICOL is not initiated *
 *                                                          *
 *  GLOBALS     co_error_code, co_xyz_2_rgb, co_icol_init   *
 *                                                          *
 *  NOTES       None.                                       *
 *                                                          *
 * -------------------------------------------------------- */

int  col_rgb_xyz(rgb, cie_xyz)
IN   double *rgb;      /* RGB coordinates of color */
OUT  double *cie_xyz;  /* XYZ coordinates of color */
  {

  /* assertion: library is initiated */
  if (co_icol_init == NO)
    return (COL_ERR_NOT_INIT);

  co_error_code = COL_OK;

  /* convert RGB -> XYZ coordinates */
  AFIN_TRANSFORM_3x3(cie_xyz, co_rgb_2_xyz, rgb);

  return (COL_OK);
  }

/* -------------------------------------------------------- *
 *  FUNCTION    col_clipping                                *
 *  PURPOSE     clipping the color in the color space       *
 *                                                          *
 *  ARGUMENTS                                               *
 *      INPUT   method - number of clipping method          *
 *      INOUT   rgb - RGB values of color                   *
 *                                                          *
 *  RETURN      COL_OK - function is finished successfully  *
 *              COL_ERR_NOT_INIT - if ICOL is not initiated *
 *              COL_ERR_WRONG_VALUE - if parameter "method" *
 *                                was specified incorrectly *
 *                                                          *
 *  GLOBALS     co_error_code, co_icol_init                 *
 *                                                          *
 *  NOTES       None                                        *
 *                                                          *
 * -------------------------------------------------------- */

int col_clipping(rgb, method)
  INOUT double *rgb;    /* RGB coordinates of color */
  IN    int method;     /* clipping method */
  {

  /* assertion: library is initiated */
  if (co_icol_init == NO)
    return (COL_ERR_NOT_INIT);

  switch (method)
    {
    case COL_CLIP_METHOD_CLIP:      /* clip method */
      co_error_code = co_rgb_clip(rgb);
      break;
    case COL_CLIP_METHOD_SCALE:     /* scale method */
      co_error_code = co_rgb_scale(rgb);
      break;
    case COL_CLIP_METHOD_CLAMP:     /* clamp method */
      co_error_code = co_rgb_clamp(rgb);
      break;
    default:
      /* assertion: method is known */
      co_error_code = COL_ERR_WRONG_VALUE;
    }

  return (co_error_code);
  }
/* -------------------------------------------------------- *
 *  FUNCTION    icol_version()                              *
 *  PURPOSE     Returns name of ICOL library version        *
 *                                                          *
 *  ARGUMENTS                                               *
 *      INPUT   None.                                       *
 *      OUTPUT  None.                                       *
 *                                                          *
 *  RETURN      pointer to name of ICOL library version     *
 *                                                          *
 *  GLOBALS     sccsid                                      *
 *                                                          *
 *  NOTES       not for application usage                   *
 *                                                          *
 * -------------------------------------------------------- */

char *icol_version()
  {
  return sccsid;
  } /* end of function 'icol_version()' */

/* -------------------------------------------------------- *
 *  FUNCTION    col_error                                   *
 *  PURPOSE     Return error code                           *
 *                                                          *
 *  ARGUMENTS                                               *
 *      INPUT   None.                                       *
 *      OUTPUT  None.                                       *
 *                                                          *
 *  RETURN      error code                                  *
 *                                                          *
 *  GLOBALS     co_error_code                               *
 *                                                          *
 *  NOTES       None                                        *
 *                                                          *
 * -------------------------------------------------------- */

int col_error()
  {
  return (co_error_code);
  }

/* -------------------------------------------------------- *
 *  FUNCTION    col_strerror                                *
 *  PURPOSE     Return pointer to string with error         *
 *              description                                 *
 *                                                          *
 *  ARGUMENTS                                               *
 *      INPUT   'error' - error code                        *
 *      OUTPUT  None.                                       *
 *                                                          *
 *  RETURN      pointer to string                           *
 *                                                          *
 *  GLOBALS     co_error_msgs[], co_undefined_msg           *
 *                                                          *
 *  NOTES       None                                        *
 *                                                          *
 * -------------------------------------------------------- */

char *col_strerror(error)
  IN int  error;                 /* ICOL function return code */
  {

  /* if (error is not defined error) */
  if (error < 0 || error > COL_MAX_ERROR_CODE)
    return (co_undefined_msg);
 
  return (co_error_msgs[error]);
  }
