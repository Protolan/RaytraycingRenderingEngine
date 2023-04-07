/* -------------------------------------------------------- *
 * FILE NAME     icol_fb.c                                  *
 * PURPOSE       The RGB <-> Frame Buffer conversion        *
 *               functions                                  *
 *                                                          *
 * SPEC          Alex Voloboy (Rav) lcof03ds.Rav  '94.04.14 *
 * DESIGN        Alex Voloboy (Rav) lcof03ds.Rav  '94.04.14 *
 * DES. VV       Oek                              '94.04.14 *
 * CODING        Alex Voloboy (Rav)               '94.04.25 *
 * TESTING       Edward Kopylov (Oek)             '96.02.07
 * UPDATED       Edward Kopylov (Oek)             '96.02.07
 *                                                          *
 * NOTES         Final spec. are in file icol_201.um        *
 *                                                          *
 * -------------------------------------------------------- */


#include "integra.h"
#include "imal.h"
#include "icol.h"
#include "icol_own.h"
#include "algebra.h"


/* -------------------------------------------------------- *
 *  FUNCTION    col_set_gamma                               *
 *  PURPOSE     Function sets new values for Gamma          *
 *              correction coefficients                     *
 *                                                          *
 *  ARGUMENTS                                               *
 *      INPUT   'r', 'g', 'b' - new gamma correction values *
 *                              for rgb coordinates         *
 *      OUTPUT  None.                                       *
 *                                                          *
 *  RETURN      COL_OK - function is finished successfully  *
 *              COL_ERR_NOT_INIT - if ICOL is not initiated *
 *              COL_ERR_WRONG_VALUE - if input data are     *
 *                                    invalid               *
 *                                                          *
 *  GLOBALS     co_error_code, co_icol_init, co_gamma,      *
 *              co_mc_method                                *
 *                                                          *
 *  NOTES       None.                                       *
 *                                                          *
 * -------------------------------------------------------- */

int col_set_gamma(r, g, b)
  IN   double r, g, b;  /* Gamma correction values */
  {
  /* assertion: library is initiated */
  if (co_icol_init == NO)
    return (COL_ERR_NOT_INIT);

  co_error_code = COL_OK;

  /* assertion: r, g, b > 0.0 */
  if (r <= 0.0 || g <= 0.0 || b <= 0.0)
    {
    co_error_code = COL_ERR_WRONG_VALUE;
    return (COL_ERR_WRONG_VALUE);
    }

  /* conversion method is formula method */
  co_mc_method = COL_GAMMA;

  /* keep Gamma correction values */
  LOAD_3D(co_gamma, r, g, b)

  return (COL_OK);
  }

/* -------------------------------------------------------- *
 *  FUNCTION    col_set_lum_curves                          *
 *  PURPOSE     Function sets the luminance monitor         *
 *              correction curves                           *
 *                                                          *
 *  ARGUMENTS                                               *
 *      INPUT   'red' - luminance curve for red component   *
 *              'green' - luminance curve for green comp.   *
 *              'blue' - luminance curve for blue component *
 *      OUTPUT  None.                                       *
 *                                                          *
 *  RETURN      COL_OK - function is finished successfully  *
 *              COL_ERR_NOT_INIT - if ICOL is not initiated *
 *              COL_ERR_WRONG_VALUE - if input data are     *
 *                                    invalid               *
 *              COL_ERR_NOT_ENOUGH_MEMORY - if function     *
 *                                   can't allocate memory  *
 *                                                          *
 *  GLOBALS     co_error_code, co_icol_init, co_lum_alloc,  *
 *              co_mc_method, co_lum                        *
 *                                                          *
 *  NOTES       None.                                       *
 *                                                          *
 * -------------------------------------------------------- */

int col_set_lum_curves(red, green, blue)
  IN   double *red;   /* Gamma correction tables */
  IN   double *green;
  IN   double *blue;
  {
  int   i;      /* tmp variable */

  /* assertion: library is initiated */
  if (co_icol_init == NO)
    return (COL_ERR_NOT_INIT);

  co_error_code = COL_OK;

  /* assertion: red[255] = green[255] = blue[255] = 1.0, OPCI */
  if (red[FB_MAX] != 1.0 ||
      green[FB_MAX] != 1.0 ||
      blue[FB_MAX] != 1.0)
    {
    co_error_code = COL_ERR_WRONG_VALUE;
    return (COL_ERR_WRONG_VALUE);
    }

  /* assertion: all (red[], green[], blue[]) values >= 0.0 && <= 1.0,
             for each i red[i] <= red[i + 1], also for green blue */
  for (i = 0; i < FB_MAX; i++)
    {
    if (red[i] > red[i + 1] || red[i] < 0.0 || red[i] > 1.0 ||
        green[i] > green[i + 1] || green[i] < 0.0 || green[i] > 1.0 ||
        blue[i] > blue[i + 1] || blue[i] < 0.0 || blue[i] > 1.0)
      {
      co_error_code = COL_ERR_WRONG_VALUE;
      return (COL_ERR_WRONG_VALUE);
      }
    }

  /* allocate memory for tables */
  if (co_lum_alloc == NO)
    {
    co_lum[0] = (double *)mem_alloc(co_mem_class,
                "co_lum[0]", 3 * COL_SHADES * sizeof(double));
    if (co_lum[0] == NULL)
      {
      co_error_code = COL_ERR_NOT_ENOUGH_MEMORY;
      return (COL_ERR_NOT_ENOUGH_MEMORY);
      }
    co_lum[1] = co_lum[0] + COL_SHADES;
    co_lum[2] = co_lum[1] + COL_SHADES;
    co_lum_alloc = YES;
    }

  /* set table values in co_lum[][] */
  for (i = 0; i < COL_SHADES; i++)
    {
    co_lum[0][i] = red[i];
    co_lum[1][i] = green[i];
    co_lum[2][i] = blue[i];
    }

  co_mc_method = COL_TABLE;

  return (COL_OK);
  }

/* -------------------------------------------------------- *
 *  FUNCTION    col_set_gun_balance                         *
 *  PURPOSE     Function sets values for gun balance        *
 *                                                          *
 *  ARGUMENTS                                               *
 *      INPUT   'r', 'g', 'b' - gun balance values          *
 *      OUTPUT  None.                                       *
 *                                                          *
 *  RETURN      COL_OK - function is finished successfully  *
 *              COL_ERR_NOT_INIT - if ICOL is not initiated *
 *              COL_ERR_WRONG_VALUE - if input data are     *
 *                                    invalid               *
 *                                                          *
 *  GLOBALS     co_error_code, co_icol_init, co_gun_value,  *
 *              co_gun_balance                              *
 *                                                          *
 *  NOTES       None.                                       *
 *                                                          *
 * -------------------------------------------------------- */

int col_set_gun_balance(r, g, b)
  IN   double r, g, b;  /* gun balance values */
  {
  double max_value_rev;     /* tmp variable */

  /* assertion: library is initiated */
  if (co_icol_init == NO)
    return (COL_ERR_NOT_INIT);

  co_error_code = COL_OK;

  /* assertion: r, g, b > 0.0 */
  if (r <= 0.0 || g <= 0.0 || b <= 0.0)
    {
    co_error_code = COL_ERR_WRONG_VALUE;
    return (COL_ERR_WRONG_VALUE);
    }

  co_gun_balance = NO;
  max_value_rev = 1.0 / MAX3(r, g, b);

  r *= max_value_rev;
  g *= max_value_rev;
  b *= max_value_rev;

  if (r > 1.0 - EPS_FLOAT && g > 1.0 - EPS_FLOAT && b > 1.0 - EPS_FLOAT)
    {
    co_gun_balance = NO; /* additional calculation is no needed, OPCI */
    return COL_OK;
    }
  
  co_gun_balance = YES;
  LOAD_3D(co_gun_value, r, g, b)

  return (COL_OK);
  }

/* --------------------------------------------------------
 *  FUNCTION    col_rgb_fb
 *  PURPOSE     Function converts RGB values into Frame
 *              Buffer values
 *
 *  ARGUMENTS
 *      INPUT   'rgb' - RGB values
 *      OUTPUT  'fb' - resultant Frame Buffer values
 *
 *  RETURN      COL_OK - function is finished successfully
 *              COL_ERR_NOT_INIT - if ICOL is not initiated
 *              COL_ERR_WRONG_VALUE - if input data are
 *                                    invalid
 *
 *  GLOBALS     co_error_code, co_icol_init, co_gun_value,
 *              co_gun_balance, co_gamma, co_mc_method
 *
 *  NOTES       None.
 *
 */

int col_rgb_fb(rgb, fb)
IN  double *rgb;  /* RGB values */
OUT int    *fb;   /* resultant FB RGB values */
  {
  double r, g, b;

  /* assertion: library is initiated */
  if (co_icol_init == NO)
    return COL_ERR_NOT_INIT;

  co_error_code = COL_OK;

  r = rgb[0];
  g = rgb[1];
  b = rgb[2];

  /* assertion: 0.0 <= rgb values <= 1.0 */
  if (r < 0 || r > 1 || g < 0 || g > 1 || b < 0 || b > 1)
    {
    co_error_code = COL_ERR_WRONG_VALUE;
    return COL_ERR_WRONG_VALUE;
    }

  /* gun balance correction */
  if (co_gun_balance == YES)   /* if gun balancing is needed */
    {
    r *= co_gun_value[0];
    g *= co_gun_value[1];
    b *= co_gun_value[2];
    }

  /* if (conversion method is formula method) */
  if (co_mc_method == COL_GAMMA)
    {
    fb[0] = (int)(FB_MAX * pow(r, (1.0 / co_gamma[0])) + 0.5);
    fb[1] = (int)(FB_MAX * pow(g, (1.0 / co_gamma[1])) + 0.5);
    fb[2] = (int)(FB_MAX * pow(b, (1.0 / co_gamma[2])) + 0.5);
    }
  else /* if (conversion method is table method) */
    {
    fb[0] = co_find_lum(co_lum[0], r);
    fb[1] = co_find_lum(co_lum[1], g);
    fb[2] = co_find_lum(co_lum[2], b);
    }

  return COL_OK;
  }

/* --------------------------------------------------------
 *  FUNCTION    col_r_fb
 *  PURPOSE     Function converts R value into RED Frame Buffer component
 *
 *  ARGUMENTS
 *      INPUT   'r'   - R value
 *      OUTPUT  'fbr' - resultant Frame Buffer value
 *
 *  RETURN      COL_OK - function is finished successfully
 *              COL_ERR_NOT_INIT - if ICOL is not initiated
 *              COL_ERR_WRONG_VALUE - if input data are
 *                                    invalid
 *
 *  GLOBALS     co_error_code, co_icol_init, co_gun_value,
 *              co_gun_balance, co_gamma, co_mc_method
 *
 *  NOTES       This function is analog of col_rgb_fb for only one
 *              component.
 *
 */

int col_r_fb(r, fbr)
IN  double r;   /* R value                */
OUT int *fbr;   /* resultant FB RED value */
  {
  /* assertion: library is initiated */
  if (co_icol_init == NO)
    return COL_ERR_NOT_INIT;

  co_error_code = COL_OK;

  /* assertion: 0.0 <= rgb value <= 1.0 */
  if (r < 0 || r > 1)
    {
    co_error_code = COL_ERR_WRONG_VALUE;
    return COL_ERR_WRONG_VALUE;
    }

  /* gun balance correction */
  if (co_gun_balance == YES)   /* if gun balancing is needed */
    r *= co_gun_value[0];

  /* if (conversion method is formula method) */
  if (co_mc_method == COL_GAMMA)
    *fbr = (int)(FB_MAX * pow(r, (1.0 / co_gamma[0])) + 0.5);
  else /* if (conversion method is table method) */
    *fbr = co_find_lum(co_lum[0], r);

  return COL_OK;
  }


/* --------------------------------------------------------
 *  FUNCTION    col_g_fb
 *  PURPOSE     Function converts G value into GREEN Frame Buffer component
 *
 *  ARGUMENTS
 *      INPUT   'g'   - G value
 *      OUTPUT  'fbg' - resultant Frame Buffer value
 *
 *  RETURN      COL_OK - function is finished successfully
 *              COL_ERR_NOT_INIT - if ICOL is not initiated
 *              COL_ERR_WRONG_VALUE - if input data are
 *                                    invalid
 *
 *  GLOBALS     co_error_code, co_icol_init, co_gun_value,
 *              co_gun_balance, co_gamma, co_mc_method
 *
 *  NOTES       This function is analog of col_rgb_fb for only one
 *              component.
 *
 */

int col_g_fb(g, fbg)
IN  double g;   /* G value                  */
OUT int *fbg;   /* resultant FB GREEN value */
  {
  /* assertion: library is initiated */
  if (co_icol_init == NO)
    return COL_ERR_NOT_INIT;

  co_error_code = COL_OK;

  /* assertion: 0.0 <= rgb value <= 1.0 */
  if (g < 0 || g > 1)
    {
    co_error_code = COL_ERR_WRONG_VALUE;
    return COL_ERR_WRONG_VALUE;
    }

  /* gun balance correction */
  if (co_gun_balance == YES)   /* if gun balancing is needed */
    g *= co_gun_value[1];

  /* if (conversion method is formula method) */
  if (co_mc_method == COL_GAMMA)
    *fbg = (int)(FB_MAX * pow(g, (1.0 / co_gamma[1])) + 0.5);
  else /* if (conversion method is table method) */
    *fbg = co_find_lum(co_lum[1], g);

  return COL_OK;
  }


/* --------------------------------------------------------
 *  FUNCTION    col_b_fb
 *  PURPOSE     Function converts B value into BLUE Frame Buffer component
 *
 *  ARGUMENTS
 *      INPUT   'b'   - B value
 *      OUTPUT  'fbb' - resultant Frame Buffer value
 *
 *  RETURN      COL_OK - function is finished successfully
 *              COL_ERR_NOT_INIT - if ICOL is not initiated
 *              COL_ERR_WRONG_VALUE - if input data are
 *                                    invalid
 *
 *  GLOBALS     co_error_code, co_icol_init, co_gun_value,
 *              co_gun_balance, co_gamma, co_mc_method
 *
 *  NOTES       This function is analog of col_rgb_fb for only one
 *              component.
 *
 */

int col_b_fb(b, fbb)
IN  double b;   /* B value                 */
OUT int *fbb;   /* resultant FB BLUE value */
  {
  /* assertion: library is initiated */
  if (co_icol_init == NO)
    return COL_ERR_NOT_INIT;

  co_error_code = COL_OK;

  /* assertion: 0.0 <= rgb value <= 1.0 */
  if (b < 0 || b > 1)
    {
    co_error_code = COL_ERR_WRONG_VALUE;
    return COL_ERR_WRONG_VALUE;
    }

  /* gun balance correction */
  if (co_gun_balance == YES)   /* if gun balancing is needed */
    b *= co_gun_value[2];

  /* if (conversion method is formula method) */
  if (co_mc_method == COL_GAMMA)
    *fbb = (int)(FB_MAX * pow(b, (1.0 / co_gamma[2])) + 0.5);
  else /* if (conversion method is table method) */
    *fbb = co_find_lum(co_lum[2], b);

  return COL_OK;
  }


/* -------------------------------------------------------- *
 *  FUNCTION    co_find_lum                                 *
 *  PURPOSE     Function converts RGB values into Frame     *
 *              Buffer values                               *
 *                                                          *
 *  ARGUMENTS                                               *
 *      INPUT   'col' - color component                     *
 *              'value' - intensity                         *
 *      OUTPUT  None.                                       *
 *                                                          *
 *  RETURN      Frame Buffer value (index in table)         *
 *                                                          *
 *  GLOBALS     co_lum                                      *
 *                                                          *
 *  NOTES       Function co_find_lum() executes binary      *
 *              search in luminance tables specified by     *
 *              col_set_lum_curves() function and return    *
 *              minimal index of table item that bigger     *
 *              than specified value, or 255 (maximal index *
 *              of table) if such index was not found.      *
 *              The three instances below demonstrates      *
 *              what is result.                             *
 *
 *   1. Let 0.91 is specified value, then:
 *   .2     .22    .26    .28       .84    .89    .95    1
 *   |------+------+------+-- ... --+------+-*----+------|
 *  [0]    [1]    [2]    [3]      [252]  [253]  [254]  [255]
 *                                              ^
 *   2. Let 1.2 is specified value, then:
 *   .2     .22    .26    .28       .84    .89    .95    1
 *   |------+------+------+-- ... --+------+------+------|--*--
 *  [0]    [1]    [2]    [3]      [252]  [253]  [254]  [255]
 *                                                     ^
 *   3. Let 0.1 is specified value, then:
 *     .2     .22    .26    .28       .84    .89    .95    1
 * -*--|------+------+------+-- ... --+------+------+------|
 *    [0]    [1]    [2]    [3]      [252]  [253]  [254]  [255]
 *    ^
 *   It  shows clearly that any Frame Buffer value can be  obtained
 *   via co_find_lum() and nothing else.
 */

int co_find_lum(lum, value)
IN double *lum; /* lum table for a color component to look at */
IN double value; /* normalized intensity from [0,1] */
  {
  int low, up, middle; /* tmp variables */

  /* Binary search */
  low = 0;
  up = COL_SHADES;   /* this point will be not reached in any way */

  /* while there is at the least one point between low and up */
  while (up > low + 1)
    {
    /* set middle point */
    middle = (int)((low + up) / 2 + 0.5);
    if (lum[middle - 1] <= value)
      low = middle;
    else 
      up = middle;
    }

  return low;
  } 

/* -------------------------------------------------------- *
 *  FUNCTION    col_fb_rgb                                  *
 *  PURPOSE     Function converts Frame Buffer values into  *
 *              RGBcoordinates of color                     *
 *                                                          *
 *  ARGUMENTS                                               *
 *      INPUT   'fb' - Frame Buffer values of color         *
 *      OUTPUT  'rgb' - RGB coordinates of color            *
 *                                                          *
 *  RETURN      COL_OK - function is finished successfully  *
 *              COL_ERR_NOT_INIT - if ICOL is not initiated *
 *              COL_ERR_WRONG_VALUE - if input data are     *
 *                                    invalid               *
 *                                                          *
 *  GLOBALS     co_error_code, co_icol_init, co_gun_value,  *
 *              co_gun_balance, co_gamma, co_mc_method,     *
 *              co_lum                                      *
 *                                                          *
 *  NOTES       None.                                       *
 *                                                          *
 * -------------------------------------------------------- */

int col_fb_rgb(fb, rgb)
  IN   int    *fb;   /* Frame Buffer RGB values */
  OUT  double *rgb;  /* RGB coordinates of color */
  {
  /* assertion: library is initiated */
  if (co_icol_init == NO)
    return (COL_ERR_NOT_INIT);

  co_error_code = COL_OK;

  /* assertion: 0 <= fb values <= 255 */
  if (fb[0] < 0 || fb[0] > FB_MAX ||
      fb[1] < 0 || fb[1] > FB_MAX ||
      fb[2] < 0 || fb[2] > FB_MAX)
    {
    co_error_code = COL_ERR_WRONG_VALUE;
    return (COL_ERR_WRONG_VALUE);
    }

  /* if (conversion method is formula method) */
  if (co_mc_method == COL_GAMMA)
    {
    rgb[0] = pow(fb[0] * FB_MAX_REV, co_gamma[0]);
    rgb[1] = pow(fb[1] * FB_MAX_REV, co_gamma[1]);
    rgb[2] = pow(fb[2] * FB_MAX_REV, co_gamma[2]);
    }
  else /* if (conversion method is table method) */
    { 
    /* for each color component (RED, GREEN, BLUE) */
    /* RED */
    rgb[0] = (fb[0] == 0) ? 0 : ((fb[0] == FB_MAX) ? 1 :
             (co_lum[0][fb[0]] + co_lum[0][fb[0] - 1]) * 0.5);
    /* GREEN */
    rgb[1] = (fb[1] == 0) ? 0 : ((fb[1] == FB_MAX) ? 1 :
             (co_lum[1][fb[1]] + co_lum[1][fb[1] - 1]) * 0.5);
    /* BLUE */
    rgb[2] = (fb[2] == 0) ? 0 : ((fb[2] == FB_MAX) ? 1 :
             (co_lum[2][fb[2]] + co_lum[2][fb[2] - 1]) * 0.5);
    }

  /* gun balance correction */
  if (co_gun_balance == YES)
    {
    rgb[0] /= co_gun_value[0];
    rgb[1] /= co_gun_value[1];
    rgb[2] /= co_gun_value[2];
    }

  return (COL_OK);
  }

