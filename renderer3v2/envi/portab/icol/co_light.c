/* -------------------------------------------------------- *
 * FILE NAME     co_light.c                                 *
 * PURPOSE       Attr -> CIE XYZ conversion functions       *
 *                                                          *
 * SPEC          Alex Voloboy (Rav) lcof03ds.Rav  '94.04.14 *
 * DESIGN        Alex Voloboy (Rav) lcof03ds.Rav  '94.04.14 *
 * DES. VV       Oek                              '94.04.14 *
 * CODING        Alex Voloboy (Rav)               '94.04.25 *
 * TESTING       Edward Kopylov (Oek)             '96.02.07
 * UPDATED       Edward Kopylov (Oek)             '96.02.07
 *                                                          *
 * NOTES         Final spec. are in file icol_200.um        *
 *                                                          *
 * -------------------------------------------------------- */


#include "integra.h"
#include "imal.h"
#include "icol.h"
#include "icol_own.h"
#include "algebra.h"


/* -------------------------------------------------------- *
 *  FUNCTION    col_set_light                               *
 *  PURPOSE     Function sets new values for light          *
 *              spectrogram which is used in ATTR -> XYZ    *
 *              conversion                                  *
 *                                                          *
 *  ARGUMENTS                                               *
 *      INPUT   'number' - number of values defining        *
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
 *  GLOBALS     co_light_wl, co_light, co_wlli_alloc,       *
 *              co_li_alloc, co_error_code,                 *
 *              co_light_points, co_icol_init               *
 *                                                          *
 *  NOTES       None.                                       *
 *                                                          *
 * -------------------------------------------------------- */


int col_set_light(number, first, last, light, mode)
  IN   int    number; /* number of values */
  IN   double first;  /* minimum value of wavelength */
  IN   double last;   /* maximum value of wavelength */
  IN   double *light; /* spectrogram values */
  IN   int    mode;   /* white color recalculation attribute */
  {
  int     i;           /* tmp variable */
  double  r_nothing[2], g_nothing[2], b_nothing[2];  /* tmp variable */
  double  interval;    /* wavelength interval */
  double  res_xyz[3];  /* CIE XYZ coordinates of light */
  double  white[2];    /* CIE xy coordinates of light */

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

  co_light_points = number;

  /* free memory allocated in col_init() */
  if (co_wlli_alloc == YES)
    {
    mem_free((VPTR)co_light_wl);
    co_wlli_alloc = NO;
    }

  if (co_li_alloc == YES)
    {
    mem_free((VPTR)co_light);
    co_li_alloc = NO;
    }

  /* allocate memory for light spectrogram and for light wavelength */

  co_light_wl = (double *)mem_alloc(co_mem_class,
                "co_light_wl", co_light_points * sizeof(double));
  if (co_light_wl == NULL)
    {
    co_error_code = COL_ERR_NOT_ENOUGH_MEMORY;
    return (COL_ERR_NOT_ENOUGH_MEMORY);
    }
  co_wlli_alloc = YES;

  /* allocation memory for spectrogram */
  co_light = (double *)mem_alloc(co_mem_class,
             "co_light", co_light_points * sizeof(double));
  if (co_light == NULL)
    {
    co_error_code = COL_ERR_NOT_ENOUGH_MEMORY;
    return (COL_ERR_NOT_ENOUGH_MEMORY);
    }
  co_li_alloc = YES;


  /* fill light_wl array with values from
       'first' to 'last' by (last - first) / (number - 1) nm */

  interval = (last - first) / (number - 1);
  for (i = 0; i < number; i++)
    co_light_wl[i] = first + (interval * i);

  /* fill co_light array */
  for (i = 0; i < number; i++)
    co_light[i] = light[i];

  if (mode == COL_SET_WHITE)
    {
    /* calculate (x, y) color coordinates of light */
    co_error_code =
      col_spec_xyz(co_light_points, co_light_wl, co_light, res_xyz);
    if (co_error_code != COL_OK)
      return (co_error_code);

    white[0] = res_xyz[0] / (res_xyz[0] + res_xyz[1] + res_xyz[2]);
    white[1] = res_xyz[1] / (res_xyz[0] + res_xyz[1] + res_xyz[2]);
    r_nothing[0] = 0.0;
    r_nothing[1] = 0.0;
    g_nothing[0] = 0.0;
    g_nothing[1] = 0.0;
    b_nothing[0] = 0.0;
    b_nothing[1] = 0.0;

    /* set new white color in XYZ -> RGB conversion */
    co_error_code = col_set_wrgb(white, r_nothing,
                                              g_nothing, b_nothing);
    if (co_error_code != COL_OK)
      return (co_error_code);
    }

  return (COL_OK);
  }

/* -------------------------------------------------------- *
 *  FUNCTION    col_attr_xyz                                *
 *  PURPOSE     convert spectrogram values into CIE XYZ     *
 *              coordinates of surface attribute            *
 *                                                          *
 *  ARGUMENTS                                               *
 *      INPUT   number - number of values defining          *
 *                       spectrogram                        *
 *              wavelength - array with wavelengths for     *
 *                           which spectrogram is defined   *
 *              spec_gram - values of the attribute         *
 *                          spectrogram                     *
 *      OUTPUT  cie_xyz - CIE XYZ coordinates of color      *
 *                                                          *
 *  RETURN      COL_OK - function is finished successfully  *
 *              COL_ERR_NOT_INIT - if ICOL is not initiated *
 *              COL_ERR_WRONG_VALUE - if input data are     *
 *                                    invalid               *
 *                                                          *
 *  GLOBALS     co_error_code, co_x_curve, co_y_curve,      *
 *              co_z_curve, co_wavelength, co_curve_points, *
 *              co_icol_init, co_light_wl, co_light         *
 *                                                          *
 *  NOTES       None.                                       *
 *                                                          *
 * -------------------------------------------------------- */

int col_attr_xyz(number, wavelength, spec_gram, cie_xyz)
  IN  int number;
  IN  double *wavelength;
  IN  double *spec_gram;
  OUT double *cie_xyz;
  { 
  int i;
  int indx1, indx2, indx3;       /* indexes in wavelength arrays */
  /* tmp values for spectrogram function */
  double sg_old, sg_tmp;
  double light_old, light_tmp;   /* tmp values for light function */
  /* tmp values for XYZ curves function */
  double xyz_old[3], xyz_tmp[3];
  double wl_old;                 /* left point of the interval */

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
    if (wavelength[i] > wavelength[i + 1])
      {
      co_error_code = COL_ERR_WRONG_VALUE;
      return (COL_ERR_WRONG_VALUE);
      }

  /*
   Procedure  calculates XYZ  as integrals of f(t)*g(t)*w(t) over 
   interval in which the functions were defined. Function f(t) is 
   spectral characteristic of color. Function g(t) is CIE X, Y, Z 
   curves  acc. for XYZ values. Function w(t) is  spectrogram  of 
   the  light fell into surface. f(t), g(t) and w(t)  are  piece-
   linear functions.
  */

  /* initialization */
  CLEAR_3D(cie_xyz)
  indx1 = 0;
  indx2 = 0;
  indx3 = 0;
  sg_old = 0;
  CLEAR_3D(xyz_old)
  light_old = 0;

  /* wl_old = min(wavelength[indx1],
                  co_wavelength[indx2],
                  co_light_wl[indx3]); */

  wl_old = MIN3(wavelength[0], co_wavelength[0], co_light_wl[0]);

  /* integrals calculation */
  while (indx1 < number &&
         indx2 < co_curve_points &&
         indx3 < co_light_points)
    {
    if (wavelength[indx1] <= co_wavelength[indx2] &&
        wavelength[indx1] <= co_light_wl[indx3])
      {
      /* linear interpolation for XYZ-curves and for light */
      xyz_tmp[0] = co_li_calc(co_wavelength, co_x_curve,
                                     indx2, wavelength[indx1]);
      xyz_tmp[1] = co_li_calc(co_wavelength, co_y_curve,
                                     indx2, wavelength[indx1]);
      xyz_tmp[2] = co_li_calc(co_wavelength, co_z_curve,
                                     indx2, wavelength[indx1]);
      light_tmp = co_li_calc(co_light_wl, co_light,
                                     indx3, wavelength[indx1]);

      if (indx1 != 0)
        {
        cie_xyz[0] += co_integral3(wl_old, wavelength[indx1],
                                   sg_old, spec_gram[indx1],
                                   xyz_old[0], xyz_tmp[0],
                                   light_old, light_tmp);

        cie_xyz[1] += co_integral3(wl_old, wavelength[indx1],
                                   sg_old, spec_gram[indx1],
                                   xyz_old[1], xyz_tmp[1],
                                   light_old, light_tmp);

        cie_xyz[2] += co_integral3(wl_old, wavelength[indx1],
                                   sg_old, spec_gram[indx1],
                                   xyz_old[2], xyz_tmp[2],
                                   light_old, light_tmp);
        }

      wl_old = wavelength[indx1];
      sg_old = spec_gram[indx1];
      COPY_3D(xyz_old, xyz_tmp)
      light_old = light_tmp;
      indx1++;
      continue;
      }

    if (co_wavelength[indx2] <= wavelength[indx1] &&
        co_wavelength[indx2] <= co_light_wl[indx3])
      {
      /* linear interpolation for spectrogram and for light */
      sg_tmp =
        co_li_calc(wavelength, spec_gram, indx1, co_wavelength[indx2]);
      light_tmp =
        co_li_calc(co_light_wl, co_light, indx3, co_wavelength[indx2]);

      if (indx2 != 0)
        {
        cie_xyz[0] += co_integral3(wl_old, co_wavelength[indx2],
                                   sg_old, sg_tmp,
                                   xyz_old[0], co_x_curve[indx2],
                                   light_old, light_tmp);

        cie_xyz[1] += co_integral3(wl_old, co_wavelength[indx2],
                                   sg_old, sg_tmp,
                                   xyz_old[1], co_y_curve[indx2],
                                   light_old, light_tmp);

        cie_xyz[2] += co_integral3(wl_old, co_wavelength[indx2],
                                   sg_old, sg_tmp,
                                   xyz_old[2], co_z_curve[indx2],
                                   light_old, light_tmp);
        }

      wl_old = co_wavelength[indx2];
      sg_old = sg_tmp;
      xyz_old[0] = co_x_curve[indx2];
      xyz_old[1] = co_y_curve[indx2];
      xyz_old[2] = co_z_curve[indx2];
      light_old = light_tmp;
      indx2++;
      continue;
      }

    if (co_light_wl[indx3] <= co_wavelength[indx2] &&
        co_light_wl[indx3] <= wavelength[indx1])
      {
      /* linear interpolation for XYZ-curves and for spectrogram */
      sg_tmp =
        co_li_calc(wavelength, spec_gram, indx1, co_light_wl[indx3]);
      xyz_tmp[0] = co_li_calc(co_wavelength, co_x_curve,
                                     indx2, co_light_wl[indx3]);
      xyz_tmp[1] = co_li_calc(co_wavelength, co_y_curve,
                                     indx2, co_light_wl[indx3]);
      xyz_tmp[2] = co_li_calc(co_wavelength, co_z_curve,
                                     indx2, co_light_wl[indx3]);

      if (indx3 != 0)
        {
        cie_xyz[0] += co_integral3(wl_old, co_light_wl[indx3],
                                   sg_old, sg_tmp,
                                   xyz_old[0], xyz_tmp[0],
                                   light_old, co_light[indx3]);

        cie_xyz[1] += co_integral3(wl_old, co_light_wl[indx3],
                                   sg_old, sg_tmp,
                                   xyz_old[1], xyz_tmp[1],
                                   light_old, co_light[indx3]);

        cie_xyz[2] += co_integral3(wl_old, co_light_wl[indx3],
                                   sg_old, sg_tmp,
                                   xyz_old[2], xyz_tmp[2],
                                   light_old, co_light[indx3]);
        }

      wl_old = co_light_wl[indx3];
      sg_old = sg_tmp;
      COPY_3D(xyz_old, xyz_tmp)
      light_old = co_light[indx3];
      indx3++;
      continue;
      }
    }

  return (COL_OK);
  }

/* -------------------------------------------------------- *
 * FUNCTION      co_integral3                               *
 * PURPOSE       calculation of intergal from t1 to t2  for *
 *               f(t) * g(t) * w(t)                         *
 *                                                          *
 * ARGUMENTS                                                *
 *      INPUT   t1, t2 - integral from t1 to t2             *
 *              f1, f2 - values f(t1) and f(t2)             *
 *              g1, g2 - values g(t1) and g(t2)             *
 *              w1, w2 - values w(t1) and w(t2)             *
 *                                                          *
 *      OUTPUT  None.                                       *
 *                                                          *
 * RETURN        resultant value of intergal                *
 *                                                          *
 * GLOBALS:      None.                                      *
 *                                                          *
 * NOTES         integral from t1 to t2 of f(x)*g(x)*w(x),  *
 *               where f, g, w - linear functions and       *
 *               f(t1) = f1, f(t2) = f2, g(t1) = g1,        *
 *               g(t2) = g2, w(t1) = w1, w(t2) = w2         *
 *                                                          *
 * -------------------------------------------------------- */

double co_integral3(t1, t2, f1, f2, g1, g2, w1, w2)
  IN  double  t1, t2;   /* interval */
  IN  double  f1, f2;   /* f(t) values */
  IN  double  g1, g2;   /* g(t) values */
  IN  double  w1, w2;   /* w(t) values */
  {
  double fsub, gsub, wsub, res;    /* tmp variables */
  
  fsub = f2 - f1;
  gsub = g2 - g1;
  wsub = w2 - w1;

  res = (fsub * gsub * wsub) * 0.25;
  res += (f1 * gsub * wsub + fsub * g1 * wsub + fsub * gsub * w1) / 3.0;
  res += (f1 * g1 * wsub + f1 * gsub * w1 + fsub * g1 * w1) * 0.5;
  res += f1 * g1 * w1;
  res *= t2 - t1;

  return (res);
  }
