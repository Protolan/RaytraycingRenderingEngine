/* -------------------------------------------------------- *
 * FILE NAME    col_obj.c
 * PURPOSE      This file contains ICOL functions adapted to handler scheme.
 *              Spectral part is not implemented yet.
 *
 *		Who			   PRF name	  When
 * SPEC         (tfl) full name            pppppppp.ppp  'yy.mm.dd
 * DESIGN       (tfl) full name            pppppppp.ppp  'yy.mm.dd
 * DES. VV      (tfl) full name            pppppppp.ppp  'yy.mm.dd
 * CODING       (Oek) Edward Kopylov       OPxx          '97.10.27
 * CODE VV      (tfl) full name            pppppppp.ppp  'yy.mm.dd
 * TEST         (tfl) full name            pppppppp.ppp  'yy.mm.dd
 * LAST UPDATED (tfl) full name 	   pppppppp.ppp  'yy.mm.dd
 *
 * -------------------------------------------------------- */

/* CONDITIONAL COMPILATION SWITCHES */

/* COMPANY INCLUDES */
#include "integra.h"
#include "imal.h"
#include "icol.h"
#include "icol_own.h"

/* INTERNAL INCLUDES */
#include "algebra.h"

/**********************************************************
 *                 STATIC VARIABLES
 **********************************************************/

static COL_HAND **obj_list = NULL;
static int obj_count = 0;

/**********************************************************
 *                 STATIC FUNCTIONS
 **********************************************************/

static int binary_search PROT((IN double x, IN double *v, IN int n)); 

/**********************************************************
 *                GLOBAL FUNCTIONS
 **********************************************************/

/* -------------------------------------------------------- *
 *  FUNCTION    col_error                                   *
 *  PURPOSE     Return error code                           *
 * -------------------------------------------------------- */

int col_obj_error(obj)
IN  COL_HAND *obj;
  {
  return obj->err_code;
  }


/* -------------------------------------------------------- *
 *  FUNCTION    col_obj_create                              *
 *  PURPOSE     Creates new ICOL handler                    *
 * -------------------------------------------------------- */
COL_HAND *col_obj_create()
  {
  COL_HAND *obj;
  int i;


  /* assertion: library is initiated */
  if (co_icol_init == NO)
    {
    co_error_code = COL_ERR_NOT_INIT;
    ASSERT(FALSE)
    return NULL;
    }

  obj = (COL_HAND *)mem_calloc(co_mem_class, "ICOL object", 1, sizeof(COL_HAND));
  if (obj == NULL)
    {
    co_error_code = COL_ERR_NOT_ENOUGH_MEMORY;
    return NULL;
    }

  obj->magic = COL_OBJ_MAGIC;
  obj->err_code = COL_OK;

  LOAD_2D(obj->w, ICOL_DEF_WHITE_X, ICOL_DEF_WHITE_Y)
  LOAD_2D(obj->r, ICOL_DEF_RED_X, ICOL_DEF_RED_Y)
  LOAD_2D(obj->g, ICOL_DEF_GREEN_X, ICOL_DEF_GREEN_Y)
  LOAD_2D(obj->b, ICOL_DEF_BLUE_X, ICOL_DEF_BLUE_Y)

  /* create xyz_2_rgb and rgb_2_xyz matrixes */
  obj->err_code = co_create_xyz2rgb(obj->xyz2rgb, obj->rgb2xyz,
                    obj->r, obj->g, obj->b, obj->w);
  ASSERT(obj->err_code == COL_OK)

  SET_3D(obj->gamma, 2.2)     /* default Gamma correction value */
  obj->method = COL_GAMMA;    /* default monitor correction method */
  obj->lum[R] = NULL;

  obj->balance_flag = NO;            /* gun balance execution attribute */
  SET_3D(obj->balance_value, 1.0)    /* default gun balance value */

  /* Pointer defaults */
  obj->points = 0;
  obj->wave = NULL;
  obj->x_curve = NULL;
  obj->y_curve = NULL;
  obj->z_curve = NULL;
  obj->light = NULL;
  obj->light_points_base = 0;
  obj->light_base = NULL;
  obj->light_wl_base = NULL;
  obj->curve_points_base = 0;
  obj->x_curve_base = NULL;
  obj->y_curve_base = NULL;
  obj->z_curve_base = NULL;
  obj->wavelength_base = NULL;

  /* Standard spectral ICOL settings */
  if (co_li_alloc == YES && co_wlli_alloc == YES && co_light_points >= 2)
    {
    if ((obj->light_base = (double *)mem_alloc(co_mem_class, "co_light_b",
                                    co_light_points * sizeof(double))) == NULL)
      {
      col_obj_destroy(obj);
      return NULL;
      }
    if ((obj->light_wl_base = (double *)mem_alloc(co_mem_class, "co_light_wl_b",
                                    co_light_points * sizeof(double))) == NULL)
      {
      col_obj_destroy(obj);
      return NULL;
      }
    obj->light_points_base = co_light_points;
    for (i = 0; i < co_light_points; i++)
      {
      obj->light_base[i] = co_light[i];
      obj->light_wl_base[i] = co_light_wl[i];
      }
    }

  if (co_zc_alloc == YES && co_zc_alloc == YES && co_zc_alloc == YES &&
      co_wl_alloc == YES && co_curve_points >= 2)
    {
    if ((obj->x_curve_base = (double *)mem_alloc(co_mem_class, "co_x_curve_b",
                                    co_curve_points * sizeof(double))) == NULL)
      {
      col_obj_destroy(obj);
      return NULL;
      }
    if ((obj->y_curve_base = (double *)mem_alloc(co_mem_class, "co_y_curve_b",
                                    co_curve_points * sizeof(double))) == NULL)
      {
      col_obj_destroy(obj);
      return NULL;
      }
    if ((obj->z_curve_base = (double *)mem_alloc(co_mem_class, "co_z_curve_b",
                                    co_curve_points * sizeof(double))) == NULL)
      {
      col_obj_destroy(obj);
      return NULL;
      }
    if ((obj->wavelength_base = (double *)mem_alloc(co_mem_class,
                 "co_wavelength_b", co_curve_points * sizeof(double))) == NULL)
      {
      col_obj_destroy(obj);
      return NULL;
      }
    obj->curve_points_base = co_curve_points;
    for (i = 0; i < co_curve_points; i++)
      {
      obj->x_curve_base[i] = co_x_curve[i];
      obj->y_curve_base[i] = co_y_curve[i];
      obj->z_curve_base[i] = co_z_curve[i];
      obj->wavelength_base[i] = co_wavelength[i];
      }
    }

  if (obj->wavelength_base != NULL && obj->light_wl_base != NULL)
    {
    if (col_obj_ref_spec(obj, co_light_points, co_light_wl) != COL_OK)
      {
      col_obj_destroy(obj);
      return NULL;
      }
    }

#define OBJ_PORTION_COUNT 10
  if (obj_list == NULL)
    {
    ASSERT(obj_count == 0)
    obj_list = (COL_HAND **)mem_alloc(co_mem_class, "ICOL obj. list",
                            OBJ_PORTION_COUNT * sizeof(VPTR));
    for (i = 1; i < OBJ_PORTION_COUNT; i++)
      obj_list[obj_count + i] = NULL;          /* clear new list segment */
    }
  else if (obj_count % OBJ_PORTION_COUNT == 0)
   {
   ASSERT(obj_count >= OBJ_PORTION_COUNT)
   obj_list = (COL_HAND **)mem_realloc(obj_list,
               (obj_count + OBJ_PORTION_COUNT) * sizeof(VPTR));
   for (i = 1; i < OBJ_PORTION_COUNT; i++)
     obj_list[obj_count + i] = NULL;          /* clear new list segment */
   }

  obj_list[obj_count] = obj;
  obj->counter = obj_count;
  obj_count++;

  return obj;
  }

/* -------------------------------------------------------- *
 *  FUNCTION    col_obj_destroy                             *
 *  PURPOSE     Destroy given ICOL handler                  *
 * -------------------------------------------------------- */
VOID col_obj_destroy(obj)
IN COL_HAND *obj;
  {
  if (obj == NULL)
    {
    ASSERT(FALSE)
    return;
    }
        
  ASSERT(co_icol_init)
  ASSERT(obj->magic == COL_OBJ_MAGIC)
  ASSERT(obj_list != NULL)

  if (obj->lum[R] != NULL)
    mem_free((VPTR)obj->lum[R]);

  if (obj->light_base != NULL)
    mem_free((VPTR)obj->light_base);
  if (obj->light_wl_base != NULL)
    mem_free((VPTR)obj->light_wl_base);

  if (obj->x_curve_base != NULL)
    mem_free((VPTR)obj->x_curve_base);
  if (obj->y_curve_base != NULL)
    mem_free((VPTR)obj->y_curve_base);
  if (obj->z_curve_base != NULL)
    mem_free((VPTR)obj->z_curve_base);
  if (obj->wavelength_base != NULL)
    mem_free((VPTR)obj->wavelength_base);

  if (obj->x_curve != NULL)
    mem_free((VPTR)obj->x_curve);
  if (obj->y_curve != NULL)
    mem_free((VPTR)obj->y_curve);
  if (obj->z_curve != NULL)
    mem_free((VPTR)obj->z_curve);
  if (obj->wave != NULL)
    mem_free((VPTR)obj->wave);
  if (obj->light != NULL)
    mem_free((VPTR)obj->light);

  ASSERT(obj == obj_list[obj->counter])
  obj_list[obj->counter] = NULL;  /* remove from the list */
  mem_free((VPTR)obj);
  }


/* -------------------------------------------------------- *
 *  FUNCTION    col_obj_destroy_all                         *
 *  PURPOSE     Destroys all ICOL handlers and the list     *
 *  NOTE:       Sometime it is difficult to create/destroy  *
 *              ICOL handler property (ref. to TBTMATH lib) *
 *              This function to release resources if any.  *
 * -------------------------------------------------------- */
VOID col_obj_destroy_all()
  {
  int i;
  ASSERT(co_icol_init)

  if (obj_list == NULL)
    {
    ASSERT(obj_count == 0)
    return;   /* nothing to destroy */
    }

  ASSERT(obj_count > 0)
  for (i = 0; i < obj_count; i++)
    if (obj_list[i] != NULL)
      col_obj_destroy(obj_list[i]);

  obj_count = 0;
  mem_free((VPTR)obj_list);
  }

/* -------------------------------------------------------- *
 *  FUNCTION    col_obj_set_wrgb                            *
 *  PURPOSE     Function sets current phosphor coordinates  *
 *              and reference white color                   *
 * -------------------------------------------------------- */
OKAY col_obj_set_wrgb(obj, white, red, green, blue)
IN COL_HAND *obj;
IN double  *white;  /* color coordinates for white color */
IN double  *red;    /* color coordinates for red color */
IN double  *green;  /* color coordinates for green color */
IN double  *blue;   /* color coordinates for blue color */
  {
  ASSERT(obj->magic == COL_OBJ_MAGIC)

  /* assertion: library is initiated */
  if (co_icol_init == NO)
    {
    obj->err_code = COL_ERR_NOT_INIT;
    ASSERT(FALSE)
    return FAILURE;
    }

  /* assertion: all input values >= 0 &&
                x + y <= 1 for any input color */
  if (white[X] < 0.0 || white[Y] < 0.0 ||
      red[X]   < 0.0 || red[Y]   < 0.0 ||
      green[X] < 0.0 || green[Y] < 0.0 ||
      blue[X]  < 0.0 || blue[Y]  < 0.0 ||
      white[X] + white[Y] > 1.0 ||
      red[X]   + red[Y]   > 1.0 ||
      green[X] + green[Y] > 1.0 ||
      blue[X]  + blue[Y]  > 1.0)
    {
    obj->err_code = COL_ERR_WRONG_VALUE;
    return FAILURE;
    }

  /* if (white_input_value == 0)
       specify correspondent previous value */

  if (white[X] == 0.0 && white[Y] == 0.0)
    COPY_2D(obj->w, co_w_current)
  else
    COPY_2D(obj->w, white)

  if (red[X] == 0.0 && red[Y] == 0.0)
    COPY_2D(obj->r, co_r_current)
  else
    COPY_2D(obj->r, red)

  if (green[X] == 0.0 && green[Y] == 0.0)
    COPY_2D(obj->g, co_g_current)
  else
    COPY_2D(obj->g, green)

  if (blue[X] == 0.0 && blue[Y] == 0.0)
    COPY_2D(obj->b, co_b_current)
  else
    COPY_2D(obj->b, blue)

  /* create xyz_2_rgb and rgb_2_xyz matrixes */
  obj->err_code = co_create_xyz2rgb(obj->xyz2rgb, obj->rgb2xyz,
                    obj->r, obj->g, obj->b, obj->w);
  if (obj->err_code != COL_OK)
    return FAILURE;

  return SUCCESS;
  }


/* -------------------------------------------------------- *
 *  FUNCTION    col_obj_get_wrgb                            *
 *  PURPOSE     Function gets current phosphor coordinates  *
 *              and reference white color                   *
 * -------------------------------------------------------- */
OKAY col_obj_get_wrgb(obj, white, red, green, blue)
IN COL_HAND *obj;
OUT double  *white;  /* color coordinates for white color */
OUT double  *red;    /* color coordinates for red color */
OUT double  *green;  /* color coordinates for green color */
OUT double  *blue;   /* color coordinates for blue color */
  {
  ASSERT(obj->magic == COL_OBJ_MAGIC)

  /* assertion: library is initiated */
  if (co_icol_init == NO)
    {
    obj->err_code = COL_ERR_NOT_INIT;
    ASSERT(FALSE)
    return FAILURE;
    }

  /* assertion: all values >= 0 && x + y <= 1 for any input color */
  ASSERT(obj->w[X] >= 0.0 && obj->w[Y] >= 0.0 &&
         obj->r[X] >= 0.0 && obj->r[Y] >= 0.0 &&
         obj->g[X] >= 0.0 && obj->g[Y] >= 0.0 &&
         obj->b[X] >= 0.0 && obj->b[Y] >= 0.0 &&
         obj->w[X] + obj->w[Y] <= 1.0 &&
         obj->r[X] + obj->r[Y] <= 1.0 &&
         obj->g[X] + obj->g[Y] <= 1.0 &&
         obj->b[X] + obj->b[Y] <= 1.0)


  COPY_2D(white, obj->w)
  COPY_2D(red, obj->r)
  COPY_2D(green, obj->g)
  COPY_2D(blue, obj->b)

  obj->err_code = COL_OK;
  return SUCCESS;
  }

/* -------------------------------------------------------- *
 *  FUNCTION    col_obj_xyz_rgb                             *
 *  PURPOSE     convert CIE XYZ coordinates of color into   *
 *              RGB coordinates                             *
 * -------------------------------------------------------- */

OKAY col_obj_xyz_rgb(obj, cie_xyz, maxcolor, rgb)
IN  COL_HAND *obj;
IN  double *cie_xyz;  /* XYZ coordinates of color */
IN  double maxcolor;  /* maximal color value in RGB color space */
OUT double *rgb;      /* RGB coordinates of color */
  {
  double maxcolor_rev;

  ASSERT(obj->magic == COL_OBJ_MAGIC)

  /* assertion: library is initiated */
  if (co_icol_init == NO)
    {
    obj->err_code = COL_ERR_NOT_INIT;
    ASSERT(FALSE)
    return FAILURE;
    }

  /* assertion: maxcolor > 0 */
  if (maxcolor <= 0.0)
    {
    obj->err_code = COL_ERR_WRONG_VALUE;
    return FAILURE;
    }

  /* convert XYZ -> RGB coordinates */
  AFIN_TRANSFORM_3x3(rgb, obj->xyz2rgb, cie_xyz);

  maxcolor_rev = 1.0 / maxcolor;
  MULTIPLY_3D_BY_VALUE(rgb, maxcolor_rev)

  /* if resultant color is out of the RGB color space */
  if (rgb[R] < 0.0 || rgb[R] > 1.0 ||
      rgb[G] < 0.0 || rgb[G] > 1.0 ||
      rgb[B] < 0.0 || rgb[B] > 1.0)
    {
    obj->err_code = COL_ERR_OUT_OF_RANGE;
    return FAILURE;
    }

  obj->err_code = COL_OK;
  return SUCCESS;
  }

/* -------------------------------------------------------- *
 *  FUNCTION    col_obj_rgb_xyz                             *
 *  PURPOSE     convert RGB coordinates of color into  CIE  *
 *              XYZ coordinates                             *
 * -------------------------------------------------------- */

OKAY col_obj_rgb_xyz(obj, rgb, cie_xyz)
IN  COL_HAND *obj;
IN   double *rgb;      /* RGB coordinates of color */
OUT  double *cie_xyz;  /* XYZ coordinates of color */
  {
  ASSERT(obj->magic == COL_OBJ_MAGIC)

  /* assertion: library is initiated */
  if (co_icol_init == NO)
    {
    obj->err_code = COL_ERR_NOT_INIT;
    ASSERT(FALSE)
    return FAILURE;
    }

  /* convert RGB -> XYZ coordinates */
  AFIN_TRANSFORM_3x3(cie_xyz, obj->rgb2xyz, rgb);

  obj->err_code = COL_OK;
  return SUCCESS;
  }


/* -------------------------------------------------------- *
 *  FUNCTION    col_obj_set_gamma                           *
 *  PURPOSE     Function sets new values for Gamma          *
 *              correction coefficients                     *
 * -------------------------------------------------------- */

OKAY col_obj_set_gamma(obj, r, g, b)
IN  COL_HAND *obj;
IN  double r, g, b;  /* Gamma correction values */
  {
  ASSERT(obj->magic == COL_OBJ_MAGIC)

  /* assertion: library is initiated */
  if (co_icol_init == NO)
    {
    obj->err_code = COL_ERR_NOT_INIT;
    ASSERT(FALSE)
    return FAILURE;
    }

  /* assertion: r, g, b > 0.0 */
  if (r <= 0.0 || g <= 0.0 || b <= 0.0)
    {
    obj->err_code = COL_ERR_WRONG_VALUE;
    return FAILURE;
    }

  /* conversion method is formula method */
  obj->method = COL_GAMMA;

  /* keep Gamma correction values */
  LOAD_3D(obj->gamma, r, g, b)

  obj->err_code = COL_OK;
  return SUCCESS;
  }


/* -------------------------------------------------------- *
 *  FUNCTION    col_obj_set_lum_curves                      *
 *  PURPOSE     Function sets the luminance monitor         *
 *              correction curves                           *
 * -------------------------------------------------------- */

OKAY col_obj_set_lum_curves(obj, red, green, blue)
IN  COL_HAND *obj;
IN  double *red;   /* Gamma correction tables */
IN  double *green;
IN  double *blue;
  {
  int   i;      /* tmp variable */

  ASSERT(obj->magic == COL_OBJ_MAGIC)

  /* assertion: library is initiated */
  if (co_icol_init == NO)
    {
    obj->err_code = COL_ERR_NOT_INIT;
    ASSERT(FALSE)
    return FAILURE;
    }

  /* assertion: red[255] = green[255] = blue[255] = 1.0, OPCI */
  if (red[FB_MAX]   != 1.0 ||
      green[FB_MAX] != 1.0 ||
      blue[FB_MAX]  != 1.0)
    {
    obj->err_code = COL_ERR_WRONG_VALUE;
    return FAILURE;
    }

  /* assertion: all (red[], green[], blue[]) values >= 0.0 && <= 1.0,
             for each i red[i] <= red[i + 1], also for green blue */
  for (i = 0; i < FB_MAX; i++)
    {
    if (red[i] > red[i + 1] || red[i] < 0.0 || red[i] > 1.0 ||
        green[i] > green[i + 1] || green[i] < 0.0 || green[i] > 1.0 ||
        blue[i] > blue[i + 1] || blue[i] < 0.0 || blue[i] > 1.0)
      {
      obj->err_code = COL_ERR_WRONG_VALUE;
      return FAILURE;
      }
    }

  /* allocate memory for tables */
  if (obj->lum[R] == NULL)
    {
    obj->lum[R] = (double *)mem_alloc(co_mem_class,
                  "co_lum[]", 3 * COL_SHADES * sizeof(double));
    if (obj->lum[R] == NULL)
      {
      obj->err_code = COL_ERR_NOT_ENOUGH_MEMORY;
      return FAILURE;
      }
    obj->lum[G] = obj->lum[R] + COL_SHADES;
    obj->lum[B] = obj->lum[G] + COL_SHADES;
    }

  /* set table values in lum[][] */
  for (i = 0; i < COL_SHADES; i++)
    {
    obj->lum[R][i] = red[i];
    obj->lum[G][i] = green[i];
    obj->lum[B][i] = blue[i];
    }

  obj->method = COL_TABLE;

  obj->err_code = COL_OK;
  return SUCCESS;
  }


/* -------------------------------------------------------- *
 *  FUNCTION    col_obj_set_gun_balance                     *
 *  PURPOSE     Function sets values for gun balance        *
 * -------------------------------------------------------- */

OKAY col_obj_set_gun_balance(obj, r, g, b)
IN COL_HAND *obj;
IN double r, g, b;  /* gun balance values */
  {
  double max_value_rev;     /* tmp variable */

  ASSERT(obj->magic == COL_OBJ_MAGIC)

  /* assertion: library is initiated */
  if (co_icol_init == NO)
    {
    obj->err_code = COL_ERR_NOT_INIT;
    ASSERT(FALSE)
    return FAILURE;
    }

  /* assertion: r, g, b > 0.0 */
  if (r <= 0.0 || g <= 0.0 || b <= 0.0)
    {
    obj->err_code = COL_ERR_WRONG_VALUE;
    return FAILURE;
    }

  obj->balance_flag = NO;

  /* max_value = max(r, g, b) */
  max_value_rev = 1.0 / MAX3(r, g, b);

  r *= max_value_rev;
  g *= max_value_rev;
  b *= max_value_rev;

  if (r > 1.0 - EPS_FLOAT &&
      g > 1.0 - EPS_FLOAT &&
      b > 1.0 - EPS_FLOAT)
    {
    obj->balance_flag = NO; /* additional calculation is no needed, OPCI */
    return COL_OK;
    }
  
  obj->balance_flag = YES;

  LOAD_3D(obj->balance_value, r, g, b)

  obj->err_code = COL_OK;
  return SUCCESS;
  }

/* -------------------------------------------------------- *
 *  FUNCTION    col_obj_rgb_fb                              *
 *  PURPOSE     Function converts RGB values into Frame     *
 *              Buffer values                               *
 * -------------------------------------------------------- */

OKAY col_obj_rgb_fb(obj, rgb, fb)
IN COL_HAND *obj;
IN  double *rgb;  /* RGB values */
OUT int    *fb;   /* resultant FB RGB values */
  {
  double r, g, b;

  ASSERT(obj->magic == COL_OBJ_MAGIC)

  /* assertion: library is initiated */
  if (co_icol_init == NO)
    {
    obj->err_code = COL_ERR_NOT_INIT;
    ASSERT(FALSE)
    return FAILURE;
    }

  r = rgb[R];
  g = rgb[G];
  b = rgb[B];

  /* assertion: 0.0 <= rgb values <= 1.0 */
  if (r < 0 || r > 1 || g < 0 || g > 1 || b < 0 || b > 1)
    {
    obj->err_code = COL_ERR_WRONG_VALUE;
    return FAILURE;
    }

  /* gun balance correction */
  if (obj->balance_flag == YES)   /* if gun balancing is needed */
    {
    r *= obj->balance_value[R];
    g *= obj->balance_value[G];
    b *= obj->balance_value[B];
    }

  /* if (conversion method is formula method) */
  if (obj->method == COL_GAMMA)
    {
    fb[R] = (int)(FB_MAX * pow(r, (1.0 / obj->gamma[R])) + 0.5);
    fb[G] = (int)(FB_MAX * pow(g, (1.0 / obj->gamma[G])) + 0.5);
    fb[B] = (int)(FB_MAX * pow(b, (1.0 / obj->gamma[B])) + 0.5);
    }
  else /* if (conversion method is table method) */
    {
    fb[R] = co_find_lum(obj->lum[R], r);
    fb[G] = co_find_lum(obj->lum[G], g);
    fb[B] = co_find_lum(obj->lum[B], b);
    }

  obj->err_code = COL_OK;
  return SUCCESS;
  }

/* -------------------------------------------------------- *
 *  FUNCTION    col_obj_r_fb                                *
 *  PURPOSE     Function converts R value into RED Frame Buffer component
 * -------------------------------------------------------- */

OKAY col_obj_r_fb(obj, v, fb)
IN COL_HAND *obj;
IN  double v;   /* R value                */
OUT int *fb;    /* resultant FB RED value */
  {
  ASSERT(obj->magic == COL_OBJ_MAGIC)

  /* assertion: library is initiated */
  if (co_icol_init == NO)
    {
    obj->err_code = COL_ERR_NOT_INIT;
    ASSERT(FALSE)
    return FAILURE;
    }

  /* assertion: 0.0 <= rgb value <= 1.0 */
  if (v < 0.0 || v > 1.0)
    {
    obj->err_code = COL_ERR_WRONG_VALUE;
    return FAILURE;
    }

  /* gun balance correction */
  if (obj->balance_flag == YES)   /* if gun balancing is needed */
    v *= obj->balance_value[R];

  /* if (conversion method is formula method) */
  if (obj->method == COL_GAMMA)
    *fb = (int)(FB_MAX * pow(v, (1.0 / obj->gamma[R])) + 0.5);
  else /* if (conversion method is table method) */
    *fb = co_find_lum(obj->lum[R], v);

  obj->err_code = COL_OK;
  return SUCCESS;
  }


/* -------------------------------------------------------- *
 *  FUNCTION    col_obj_g_fb                                *
 *  PURPOSE     Function converts G value into GREEN Frame Buffer component
 * -------------------------------------------------------- */

OKAY col_obj_g_fb(obj, v, fb)
IN COL_HAND *obj;
IN  double v;   /* G value                */
OUT int *fb;    /* resultant FB GREEN value */
  {
  ASSERT(obj->magic == COL_OBJ_MAGIC)

  /* assertion: library is initiated */
  if (co_icol_init == NO)
    {
    obj->err_code = COL_ERR_NOT_INIT;
    ASSERT(FALSE)
    return FAILURE;
    }

  /* assertion: 0.0 <= rgb value <= 1.0 */
  if (v < 0.0 || v > 1.0)
    {
    obj->err_code = COL_ERR_WRONG_VALUE;
    return FAILURE;
    }

  /* gun balance correction */
  if (obj->balance_flag == YES)   /* if gun balancing is needed */
    v *= obj->balance_value[G];

  /* if (conversion method is formula method) */
  if (obj->method == COL_GAMMA)
    *fb = (int)(FB_MAX * pow(v, (1.0 / obj->gamma[G])) + 0.5);
  else /* if (conversion method is table method) */
    *fb = co_find_lum(obj->lum[G], v);

  obj->err_code = COL_OK;
  return SUCCESS;
  }


/* -------------------------------------------------------- *
 *  FUNCTION    col_obj_b_fb                                *
 *  PURPOSE     Function converts B value into BLUE Frame Buffer component
 * -------------------------------------------------------- */

OKAY col_obj_b_fb(obj, v, fb)
IN COL_HAND *obj;
IN  double v;   /* B value                */
OUT int *fb;    /* resultant FB BLUE value */
  {
  ASSERT(obj->magic == COL_OBJ_MAGIC)

  /* assertion: library is initiated */
  if (co_icol_init == NO)
    {
    obj->err_code = COL_ERR_NOT_INIT;
    ASSERT(FALSE)
    return FAILURE;
    }

  /* assertion: 0.0 <= rgb value <= 1.0 */
  if (v < 0.0 || v > 1.0)
    {
    obj->err_code = COL_ERR_WRONG_VALUE;
    return FAILURE;
    }

  /* gun balance correction */
  if (obj->balance_flag == YES)   /* if gun balancing is needed */
    v *= obj->balance_value[B];

  /* if (conversion method is formula method) */
  if (obj->method == COL_GAMMA)
    *fb = (int)(FB_MAX * pow(v, (1.0 / obj->gamma[B])) + 0.5);
  else /* if (conversion method is table method) */
    *fb = co_find_lum(obj->lum[B], v);

  obj->err_code = COL_OK;
  return SUCCESS;
  }


/* -------------------------------------------------------- *
 *  FUNCTION    col_obj_fb_rgb                              *
 *  PURPOSE     Function converts Frame Buffer values into  *
 *              RGBcoordinates of color                     *
 * -------------------------------------------------------- */

int col_obj_fb_rgb(obj, fb, rgb)
IN COL_HAND *obj;
IN   int    *fb;   /* Frame Buffer RGB values */
OUT  double *rgb;  /* RGB coordinates of color */
  {
  ASSERT(obj->magic == COL_OBJ_MAGIC)

  /* assertion: library is initiated */
  if (co_icol_init == NO)
    {
    obj->err_code = COL_ERR_NOT_INIT;
    ASSERT(FALSE)
    return FAILURE;
    }

  /* assertion: 0 <= fb values <= 255 */
  if (fb[R] < 0 || fb[R] > FB_MAX ||
      fb[G] < 0 || fb[G] > FB_MAX ||
      fb[B] < 0 || fb[B] > FB_MAX)
    {
    obj->err_code = COL_ERR_WRONG_VALUE;
    return FAILURE;
    }

  /* if (conversion method is formula method) */
  if (obj->method == COL_GAMMA)
    {
    rgb[R] = pow(fb[R] * FB_MAX_REV, obj->gamma[R]);
    rgb[G] = pow(fb[G] * FB_MAX_REV, obj->gamma[G]);
    rgb[B] = pow(fb[B] * FB_MAX_REV, obj->gamma[B]);
    }
  else /* if (conversion method is table method) */
    { 
    /* for each color component (RED, GREEN, BLUE) */
    /* RED */
    rgb[R] = (fb[R] == 0) ? 0.0 : ((fb[R] == FB_MAX) ? 1.0 :
             (obj->lum[R][fb[R]] + obj->lum[R][fb[R] - 1]) * 0.5);
    /* GREEN */
    rgb[G] = (fb[G] == 0) ? 0.0 : ((fb[G] == FB_MAX) ? 1.0 :
             (obj->lum[G][fb[G]] + obj->lum[G][fb[G] - 1]) * 0.5);
    /* BLUE */
    rgb[B] = (fb[B] == 0) ? 0.0 : ((fb[B] == FB_MAX) ? 1.0 :
             (obj->lum[B][fb[B]] + obj->lum[B][fb[B] - 1]) * 0.5);
    }

  /* gun balance correction */
  if (obj->balance_flag == YES)
    {
    rgb[R] /= obj->balance_value[R];
    rgb[G] /= obj->balance_value[G];
    rgb[B] /= obj->balance_value[B];
    }

  obj->err_code = COL_OK;
  return SUCCESS;
  }

/* -------------------------------------------------------- *
 *  FUNCTION    col_obj_ref_spec                            *
 *  PURPOSE     Function forms current XYZ curves and       *
 *              reference light in accorgance with defined  *
 *              wave lenght array                           *
 *  ARGUMENTS                                               *
 *      INPUT   'obj' - pointer to color object             *
 *              'nref' - number of wave lenght values       *
 *                       defining current spectrogram       *
 *              'w_ref' - wave lenght values defining       *
 *                        current spectrogram               *
 *      OUTPUT  None.                                       *
 *                                                          *
 *  RETURN      COL_OK - function is finished successfully  *
 *              COL_ERR_NOT_INIT - if ICOL is not initiated *
 *              COL_ERR_OUT_OF_RANGE - if input data are    *
 *                                     out of range         *
 *              COL_ERR_NOT_ENOUGH_MEMORY - if there is not *
 *                                          memory for data *
 *              COL_ERR_WRONG_VALUE - wrong wave lenght     *
 *                                    sequence              *
 *                                                          *
 *  NOTES       None.                                       *
 *                                                          *
 * -------------------------------------------------------- */
extern int col_obj_ref_spec(obj, nref, w_ref)
IN COL_HAND *obj;   /* color handler */
IN int       nref;  /* number of wave lenght values defining spectrogram */
IN double   *w_ref; /* wave lenght values defining spectrogram */
  {
  int i, n;
  double d, w1, w2;

  /* assertion: library is initiated */
  if (co_icol_init == NO || obj == NULL || w_ref == NULL)
    {
    ASSERT(FALSE)
    return COL_ERR_NOT_INIT;
    }
  ASSERT(obj->magic == COL_OBJ_MAGIC)

  obj->err_code = COL_OK;

  /* Are spectral data defined */
  if (obj->wavelength_base == NULL || obj->x_curve_base == NULL ||
      obj->y_curve_base == NULL || obj->z_curve_base == NULL ||
      obj->light_wl_base == NULL || obj->light_base == NULL ||
      obj->curve_points_base < 2 || obj->light_points_base < 2 || nref < 2 )
    {
    obj->err_code = COL_ERR_NOT_INIT;
    ASSERT(FALSE)
    return COL_ERR_NOT_INIT;
    }

  /* Check rnage to avoid extrapolation */
  if (w_ref[0] < obj->wavelength_base[0] ||
      w_ref[nref - 1] > obj->wavelength_base[obj->curve_points_base - 1] ||
      w_ref[0] < obj->light_wl_base[0] ||
      w_ref[nref - 1] > obj->light_wl_base[obj->light_points_base - 1])
    {
    obj->err_code = COL_ERR_OUT_OF_RANGE;
    return COL_ERR_OUT_OF_RANGE;
    }

  /* Check correct order of wave lenghts */
  for (i = 0; i < nref - 1; i++)
    {
    if (w_ref[i] > w_ref[i + 1])
      {
      co_error_code = COL_ERR_WRONG_VALUE;
      return COL_ERR_WRONG_VALUE;
      }
    }

  /* Reallocate (if it is necessary) curren arrays */
  if (nref != obj->points || obj->wave == NULL)
    {
    if (obj->wave != NULL)
      mem_free(obj->wave);
    obj->wave = NULL;
    if ((obj->wave = (double *)mem_alloc(co_mem_class,
                                    "co_wave", nref * sizeof(double))) == NULL)
      {
      obj->err_code = COL_ERR_NOT_ENOUGH_MEMORY;
      ASSERT(FALSE)
      return COL_ERR_NOT_ENOUGH_MEMORY;
      }
    }
  if (nref != obj->points || obj->x_curve == NULL)
    {
    if (obj->x_curve != NULL)
      mem_free(obj->x_curve);
    obj->x_curve = NULL;
    if ((obj->x_curve = (double *)mem_alloc(co_mem_class,
                                 "co_x_curve", nref * sizeof(double))) == NULL)
      {
      obj->err_code = COL_ERR_NOT_ENOUGH_MEMORY;
      ASSERT(FALSE)
      return COL_ERR_NOT_ENOUGH_MEMORY;
      }
    }
  if (nref != obj->points || obj->y_curve == NULL)
    {
    if (obj->y_curve != NULL)
      mem_free(obj->y_curve);
    obj->y_curve = NULL;
    if ((obj->y_curve = (double *)mem_alloc(co_mem_class,
                                 "co_y_curve", nref * sizeof(double))) == NULL)
      {
      obj->err_code = COL_ERR_NOT_ENOUGH_MEMORY;
      ASSERT(FALSE)
      return COL_ERR_NOT_ENOUGH_MEMORY;
      }
    }
  if (nref != obj->points || obj->z_curve == NULL)
    {
    if (obj->z_curve != NULL)
      mem_free(obj->z_curve);
    obj->z_curve = NULL;
    if ((obj->z_curve = (double *)mem_alloc(co_mem_class,
                                 "co_z_curve", nref * sizeof(double))) == NULL)
      {
      obj->err_code = COL_ERR_NOT_ENOUGH_MEMORY;
      ASSERT(FALSE)
      return COL_ERR_NOT_ENOUGH_MEMORY;
      }
    }
  if (nref != obj->points || obj->light == NULL)
    {
    if (obj->light != NULL)
      mem_free(obj->light);
    obj->light = NULL;
    if ((obj->light = (double *)mem_alloc(co_mem_class,
                                   "co_light", nref * sizeof(double))) == NULL)
      {
      obj->err_code = COL_ERR_NOT_ENOUGH_MEMORY;
      ASSERT(FALSE)
      return COL_ERR_NOT_ENOUGH_MEMORY;
      }
    }
   obj->points = nref;

  /* Interpolate XYZ curves an reference light */
  for (i = 0; i < nref; i++)
    {
    obj->wave[i] = w_ref[i];

    /* XYZ curves */
    n = binary_search(obj->wave[i], obj->wavelength_base,
                      obj->curve_points_base);
    if (n < 0)
      {
      if (fabs(obj->wave[i] - obj->wavelength_base[0]) > EPS_FLOAT)
        {
        obj->x_curve[i] = 0.;
        obj->y_curve[i] = 0.;
        obj->z_curve[i] = 0.;
        }
      else
        {
        obj->x_curve[i] = obj->x_curve_base[0];
        obj->y_curve[i] = obj->y_curve_base[0];
        obj->z_curve[i] = obj->z_curve_base[0];
        }
      }
    else if (n > obj->curve_points_base - 2)
      {
      if (fabs(obj->wave[i] - 
               obj->wavelength_base[obj->curve_points_base - 1]) > EPS_FLOAT)
        {
        obj->x_curve[i] = 0.;
        obj->y_curve[i] = 0.;
        obj->z_curve[i] = 0.;
        }
      else
        {
        obj->x_curve[i] = obj->x_curve_base[obj->curve_points_base - 1];
        obj->y_curve[i] = obj->y_curve_base[obj->curve_points_base - 1];
        obj->z_curve[i] = obj->z_curve_base[obj->curve_points_base - 1];
        }
      }
    else
      {
      d = obj->wavelength_base[n + 1] - obj->wavelength_base[n];
      w1 = (obj->wavelength_base[n + 1] - obj->wave[i]) / d;
      w2 = (obj->wave[i] - obj->wavelength_base[n]) / d;
      obj->x_curve[i] = obj->x_curve_base[n] * w1 +
                        obj->x_curve_base[n + 1] * w2;
      obj->y_curve[i] = obj->y_curve_base[n] * w1 +
                        obj->y_curve_base[n + 1] * w2;
      obj->z_curve[i] = obj->z_curve_base[n] * w1 +
                        obj->z_curve_base[n + 1] * w2;
      }

    /* Referent light */
    n = binary_search(obj->wave[i], obj->light_wl_base,
                      obj->light_points_base);
    if (n < 0)
      {
      if (fabs(obj->wave[i] - obj->light_wl_base[0]) > EPS_FLOAT)
        obj->light[i] = 0.;
      else
        obj->light[i] = obj->light_base[0];
      }
    else if (n > nref - 2)
      {
      if (fabs(obj->wave[i] -
               obj->light_wl_base[obj->light_points_base - 1]) > EPS_FLOAT)
        obj->light[i] = 0.;
      else
        obj->light[i] = obj->light_base[obj->light_points_base - 1];
      }
    else
      {
      d = obj->light_wl_base[n + 1] - obj->light_wl_base[n];
      w1 = (obj->light_wl_base[n + 1] - obj->wave[i]) / d;
      w2 = (obj->wave[i] - obj->light_wl_base[n]) / d;
      obj->light[i] = obj->light_base[n] * w1 + obj->light_base[n + 1] * w2;
      }
    }

  return col_spec_xyz(obj->points, obj->wave, obj->light, obj->xyz_light);
  }

/* -------------------------------------------------------- *
 *  FUNCTION    col_obj_refresh_spec                        *
 *  PURPOSE     Function refresh current XYZ curves and     *
 *              reference light in accorgance with defined  *
 *              wave lenght array                           *
 *  ARGUMENTS                                               *
 *      INPUT   'obj' - pointer to color object             *
 *      OUTPUT  None.                                       *
 *                                                          *
 *  RETURN      COL_OK - function is finished successfully  *
 *              COL_ERR_NOT_INIT - if ICOL is not initiated *
 *              COL_ERR_OUT_OF_RANGE - if input data are    *
 *                                     out of range         *
 *              COL_ERR_NOT_ENOUGH_MEMORY - if there is not *
 *                                          memory for data *
 *                                                          *
 *  NOTES       None.                                       *
 *                                                          *
 * -------------------------------------------------------- */
extern int col_obj_refresh_spec(obj)
IN COL_HAND *obj; /* color handler */
  {
  /* assertion: library is initiated */
  if (co_icol_init == NO || obj == NULL)
    {
    ASSERT(FALSE)
    return COL_ERR_NOT_INIT;
    }
  ASSERT(obj->magic == COL_OBJ_MAGIC)

  obj->err_code = COL_OK;
  return col_obj_ref_spec(obj, obj->points, obj->wave);
  }

/* -------------------------------------------------------- *
 *  FUNCTION    col_set_light                               *
 *  PURPOSE     Function sets new values for light          *
 *              spectrogram which is used in ATTR -> XYZ    *
 *              conversion                                  *
 *                                                          *
 *  ARGUMENTS                                               *
 *      INPUT   'obj' - pointer to color object             *
 *              'number' - number of values defining        *
 *                         spectrogram                      *
 *              'first' - minimum value of wavelength for   *
 *                        which spectrogram will be defined *
 *              'last' - maximum value of wavelength for    *
 *                       which spectrogram will be defined  *
 *              'light' - arrays containing the "number"    *
 *                     double numbers                       *
 *              'mode' - attribute for recalculation        *
 *                       rgb_2_xyz and xyz_2_rgb matrixes   *
 *      OUTPUT  None.                                       *
 *                                                          *
 *  RETURN      COL_OK - function is finished successfully  *
 *              COL_ERR_NOT_INIT - if ICOL is not initiated *
 *              COL_ERR_NOT_ENOUGH_MEMORY - if function     *
 *                                   can't allocate memory  *
 *              COL_ERR_WRONG_VALUE - if input data are     *
 *                                    invalid               *
 *                                                          *
 *  NOTES       None.                                       *
 *                                                          *
 * -------------------------------------------------------- */

extern int  col_obj_set_light(obj, number, first, last, light, mode)
IN COL_HAND *obj;    /* color handler */
IN int       number; /* number of values */
IN double    first;  /* minimum value of wavelength */
IN double    last;   /* maximum value of wavelength */
IN double   *light;  /* spectrogram values */
IN int       mode;   /* white color recalculation attribute */
  {
  int     i;           /* tmp variable */
  double  r_nothing[2], g_nothing[2], b_nothing[2];  /* tmp variable */
  double  interval;    /* wavelength interval */
  double  res_xyz[3];  /* CIE XYZ coordinates of light */
  double  white[2];    /* CIE xy coordinates of light */

  /* assertion: library is initiated */
  if (co_icol_init == NO || obj == NULL || light == NULL)
    {
    ASSERT(FALSE)
    return COL_ERR_NOT_INIT;
    }
  ASSERT(obj->magic == COL_OBJ_MAGIC)

  obj->err_code = COL_OK;

  /* assertion: number > 1 && 0 <= first < last */
  if (number <= 1 || first < 0 || first >= last)
    {
    obj->err_code = COL_ERR_WRONG_VALUE;
    return COL_ERR_WRONG_VALUE;
    }

  /* free/allocate memory for light spectrogram and for light wavelength */
  if (obj->light_base == NULL || number != obj->light_points_base)
    {
    if (obj->light_base != NULL)
      mem_free(obj->light_base);
    obj->light_base = NULL;
    if ((obj->light_base = (double *)mem_alloc(co_mem_class,
                               "co_light_b", number * sizeof(double))) == NULL)
      {
      obj->err_code = COL_ERR_NOT_ENOUGH_MEMORY;
      ASSERT(FALSE)
      return COL_ERR_NOT_ENOUGH_MEMORY;
      }
    }
  if (obj->light_wl_base == NULL || number != obj->light_points_base)
    {
    if (obj->light_wl_base != NULL)
      mem_free(obj->light_wl_base);
    obj->light_wl_base = NULL;
    if ((obj->light_wl_base = (double *)mem_alloc(co_mem_class,
                            "co_light_wl_b", number * sizeof(double))) == NULL)
      {
      obj->err_code = COL_ERR_NOT_ENOUGH_MEMORY;
      ASSERT(FALSE)
      return COL_ERR_NOT_ENOUGH_MEMORY;
      }
    }

  obj->light_points_base = number;

  /* fill light_wl array with values from
       'first' to 'last' by (last - first) / (number - 1) nm */

  interval = (last - first) / (number - 1);
  for (i = 0; i < number; i++)
    obj->light_wl_base[i] = first + (interval * i);

  /* fill co_light array */
  for (i = 0; i < number; i++)
    obj->light_base[i] = light[i];

  if (mode == COL_SET_WHITE)
    {
    /* calculate (x, y) color coordinates of light */
    if ((obj->err_code = col_obj_spec_xyz(obj, light, res_xyz)) != COL_OK)
      return obj->err_code;

    white[0] = res_xyz[0] / (res_xyz[0] + res_xyz[1] + res_xyz[2]);
    white[1] = res_xyz[1] / (res_xyz[0] + res_xyz[1] + res_xyz[2]);
    r_nothing[0] = 0.0;
    r_nothing[1] = 0.0;
    g_nothing[0] = 0.0;
    g_nothing[1] = 0.0;
    b_nothing[0] = 0.0;
    b_nothing[1] = 0.0;

    /* set new white color in XYZ -> RGB conversion */
    if ((obj->err_code = col_obj_set_wrgb(obj, white, r_nothing,
                                          g_nothing, b_nothing)) != COL_OK)
      return co_error_code;
    }

  obj->err_code = col_obj_refresh_spec(obj);
  return obj->err_code;
  }

/* -------------------------------------------------------- *
 *  FUNCTION    col_obj_set_xyz                             *
 *  PURPOSE     Function sets new values for XYZ-curves.    *
 *                                                          *
 *  ARGUMENTS                                               *
 *      INPUT   'obj' - pointer to color object             *
 *              'number' - number of values defining curves *
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
 *  NOTES       None.                                       *
 *                                                          *
 * -------------------------------------------------------- */


int col_obj_set_xyz(obj, number, first, last, cie_x, cie_y, cie_z)
IN COL_HAND *obj;    /* color handler */
IN int       number; /* number of values */
IN double    first;  /* minimum value of wavelength */
IN double    last;   /* maximum value of wavelength */
IN double   *cie_x; /* X values */
IN double   *cie_y; /* Y values */
IN double   *cie_z; /* Z values */
  {
  int  i;            /* tmp variable */
  double  interval;  /* wavelength interval */

  /* assertion: library is initiated */
  if (co_icol_init == NO ||
      obj == NULL || cie_x == NULL || cie_y == NULL || cie_z == NULL)
    {
    ASSERT(FALSE)
    return COL_ERR_NOT_INIT;
    }
  ASSERT(obj->magic == COL_OBJ_MAGIC)

  obj->err_code = COL_OK;

  /* assertion: number > 1 && 0 <= first < last */
  if (number <= 1 || first < 0 || first >= last)
    {
    obj->err_code = COL_ERR_WRONG_VALUE;
    return COL_ERR_WRONG_VALUE;
    }

  /* free/allocate memory for new XYZ-curves and wavelength arrays */
  if (obj->wavelength_base == NULL || number != obj->curve_points_base)
    {
    if (obj->wavelength_base != NULL)
      mem_free(obj->wavelength_base);
    obj->wavelength_base = NULL;
    if ((obj->wavelength_base = (double *)mem_alloc(co_mem_class,
                          "co_wavelength_b", number * sizeof(double))) == NULL)
      {
      obj->err_code = COL_ERR_NOT_ENOUGH_MEMORY;
      ASSERT(FALSE)
      return COL_ERR_NOT_ENOUGH_MEMORY;
      }
    }
  if (obj->x_curve_base == NULL || number != obj->curve_points_base)
    {
    if (obj->x_curve_base != NULL)
      mem_free(obj->x_curve_base);
    obj->x_curve_base = NULL;
    if ((obj->x_curve_base = (double *)mem_alloc(co_mem_class,
                             "co_x_curve_b", number * sizeof(double))) == NULL)
      {
      obj->err_code = COL_ERR_NOT_ENOUGH_MEMORY;
      ASSERT(FALSE)
      return COL_ERR_NOT_ENOUGH_MEMORY;
      }
    }
  if (obj->y_curve_base == NULL || number != obj->curve_points_base)
    {
    if (obj->y_curve_base != NULL)
      mem_free(obj->y_curve_base);
    obj->y_curve_base = NULL;
    if ((obj->y_curve_base = (double *)mem_alloc(co_mem_class,
                             "co_y_curve_b", number * sizeof(double))) == NULL)
      {
      obj->err_code = COL_ERR_NOT_ENOUGH_MEMORY;
      ASSERT(FALSE)
      return COL_ERR_NOT_ENOUGH_MEMORY;
      }
    }
  if (obj->z_curve_base == NULL || number != obj->curve_points_base)
    {
    if (obj->z_curve_base != NULL)
      mem_free(obj->z_curve_base);
    obj->z_curve_base = NULL;
    if ((obj->z_curve_base = (double *)mem_alloc(co_mem_class,
                             "co_z_curve_b", number * sizeof(double))) == NULL)
      {
      obj->err_code = COL_ERR_NOT_ENOUGH_MEMORY;
      ASSERT(FALSE)
      return COL_ERR_NOT_ENOUGH_MEMORY;
      }
    }

  obj->curve_points_base = number;

  /* fill wavelength array with values from
           'first' to 'last' by (last - first) / (number - 1) nm */

  interval = (last - first) / (number - 1);
  for (i = 0; i < number; i++)
    obj->wavelength_base[i] = first + (interval * i);

  /* fill XYZ-curves arrays */
  for (i = 0; i < number; i++)
    {
    obj->x_curve_base[i] = cie_x[i];
    obj->y_curve_base[i] = cie_y[i];
    obj->z_curve_base[i] = cie_z[i];
    }

  obj->err_code = col_obj_refresh_spec(obj);
  return obj->err_code;
  }

/* -------------------------------------------------------- *
 *  FUNCTION    col_obj_spec_xyz                            *
 *  PURPOSE     convert spectrogram values into CIE XYZ     *
 *              coordinates of color                        *
 *                                                          *
 *  ARGUMENTS                                               *
 *      INPUT   obj - color handler                         *
 *              spec_gram - values of spectrogram           *
 *      OUTPUT  cie_xyz - CIE XYZ coordinates of color      *
 *                                                          *
 *  RETURN      COL_OK - function is finished successfully  *
 *              COL_ERR_NOT_INIT - if ICOL is not initiated *
 *                                                          *
 *  NOTES       None.                                       *
 *                                                          *
 * -------------------------------------------------------- */

int col_obj_spec_xyz(obj, spec_gram, cie_xyz)
IN  COL_HAND *obj;       /* color handler */
IN  double   *spec_gram; /* values of spectrogram */
OUT double   *cie_xyz;   /* CIE XYZ coordinates of color */
  { 
  int i;

  /* assertion: library is initiated */
  if (co_icol_init == NO || obj == NULL || spec_gram == NULL || cie_xyz == NULL)
    {
    ASSERT(FALSE)
    return COL_ERR_NOT_INIT;
    }
  ASSERT(obj->magic == COL_OBJ_MAGIC)

  if (obj->points < 2 || obj->wave == NULL ||
      obj->x_curve == NULL || obj->y_curve == NULL || obj->z_curve == NULL)
    {
    ASSERT(FALSE)
    return COL_ERR_NOT_INIT;
    }

  obj->err_code = COL_OK;

  /* calculate XYZ coordinates of color from spectrogram */
  CLEAR_3D(cie_xyz)
  for (i = 1; i < obj->points; i++)
    {
    cie_xyz[0] += co_integral(obj->wave[i - 1], obj->wave[i],
                              spec_gram[i - 1], spec_gram[i],
                              obj->x_curve[i - 1], obj->x_curve[i]);
    cie_xyz[1] += co_integral(obj->wave[i - 1], obj->wave[i],
                              spec_gram[i - 1], spec_gram[i],
                              obj->y_curve[i - 1], obj->y_curve[i]);
    cie_xyz[2] += co_integral(obj->wave[i - 1], obj->wave[i],
                              spec_gram[i - 1], spec_gram[i],
                              obj->z_curve[i - 1], obj->z_curve[i]);
    }                                                    
               
  return COL_OK;
  }

/* -------------------------------------------------------- *
 *  FUNCTION    col_obj_attr_xyz                            *
 *  PURPOSE     convert spectrogram values into CIE XYZ     *
 *              coordinates of surface attribute            *
 *                                                          *
 *  ARGUMENTS                                               *
 *      INPUT   obj - color handler                         *
 *              spec_gram - values of spectrogram           *
 *      OUTPUT  cie_xyz - CIE XYZ coordinates of color      *
 *                                                          *
 *  RETURN      COL_OK - function is finished successfully  *
 *              COL_ERR_NOT_INIT - if ICOL is not initiated *
 *                                                          *
 *  NOTES       None.                                       *
 *                                                          *
 * -------------------------------------------------------- */

int col_obj_attr_xyz(obj, spec_gram, cie_xyz)
IN  COL_HAND *obj;       /* color handler */
IN  double   *spec_gram; /* values of spectrogram */
OUT double   *cie_xyz;   /* CIE XYZ coordinates of color */
  { 
  int i;

  /* assertion: library is initiated */
  if (co_icol_init == NO || obj == NULL || spec_gram == NULL || cie_xyz == NULL)
    {
    ASSERT(FALSE)
    return COL_ERR_NOT_INIT;
    }
  ASSERT(obj->magic == COL_OBJ_MAGIC)

  if (obj->points < 2 || obj->wave == NULL || obj->light == NULL ||
      obj->x_curve == NULL || obj->y_curve == NULL || obj->z_curve == NULL)
    {
    ASSERT(FALSE)
    return COL_ERR_NOT_INIT;
    }

  obj->err_code = COL_OK;

  /* calculate XYZ coordinates of color from spectrogram */
  CLEAR_3D(cie_xyz)
  for (i = 1; i < obj->points; i++)
    {
    cie_xyz[0] += co_integral3(obj->wave[i - 1], obj->wave[i],
                               spec_gram[i - 1], spec_gram[i],
                               obj->x_curve[i - 1], obj->x_curve[i],
                               obj->light[i - 1], obj->light[i]);
    cie_xyz[1] += co_integral3(obj->wave[i - 1], obj->wave[i],
                               spec_gram[i - 1], spec_gram[i],
                               obj->y_curve[i - 1], obj->y_curve[i],
                               obj->light[i - 1], obj->light[i]);
    cie_xyz[2] += co_integral3(obj->wave[i - 1], obj->wave[i],
                               spec_gram[i - 1], spec_gram[i],
                               obj->z_curve[i - 1], obj->z_curve[i],
                               obj->light[i - 1], obj->light[i]);
    }                                                     
               
  return COL_OK;
  }

/// Retrieves previously calculated XYZ colour of the
/// default light (set by the moment of calculation)
OKAY col_obj_get_light_xyz(IN COL_HAND *obj, OUT double xyz[3])
  {
  /* assertion: library is initiated */
  if (co_icol_init == NO || obj == NULL)
    {
    ASSERT(FALSE)
    return COL_ERR_NOT_INIT;
    }
  ASSERT(obj->magic == COL_OBJ_MAGIC)

  COPY_3D(xyz, obj->xyz_light);
  return SUCCESS;
  }


/* -------------------------------------------------------- *
 *  FUNCTION    binary_search                               *
 *  PURPOSE     Binary searching procedure                  *
 *  ARGUMENTS                                               *
 *      INPUT   'x' - value to search                       *
 *              'v' - matrix to search through              *
 *              'n' - dimension of the matrix               *
 *      OUTPUT  None.                                       *
 *                                                          *
 *  RETURN      index of the left boundary                  *
 * -------------------------------------------------------- */
static int binary_search(x, v, n) 
  IN double  x;     /* value to search          */
  IN double *v;     /* matrix to search through */
  IN int     n;     /* dimension of the matrix  */
  {
  int low, high, mid;

  low = 0;
  high = n - 1;
  while (low <= high)
    {
    mid = (low + high) / 2;
    if (x < *(v + mid))
      high = mid - 1;
    else
      {
      if (x > *(v + mid))
        low = mid + 1;
      else
        return mid;
      }
      /* found exactly x */
    }

  return (--low);
  /* if there isn't exactly x it returns lower index */
  /* of number close to x                            */
  } /* end of 'binary_search()' function */
