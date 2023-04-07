/* -------------------------------------------------------- *
 * FILE NAME     lcd_xyz.c                                  *
 *                                                          *
 * FUNCTION      co_xyz_calculation                         *
 * PURPOSE       calculation of XYZ values of color         *
 *                                                          *
 * SPEC          Alex Voloboy (Rav)             '93.09.04   *
 * DESIGN        Alex Voloboy (Rav)             '93.09.04   *
 * DES. VV       Andrei Khodulev (Sah)          '93.09.04   *
 * CODING        Alex Voloboy (Rav)             '93.09.04   *
 * UPDATED       Alex Voloboy (Rav)             '93.12.07   *
 *    Updated for ICOL library, all REAL are replaced by    *
 *        "double", change status (global -> parameter) of  *
 *        the globals variables                             *
 * TESTING       Alex Voloboy (Rav)             '93.12.15   *
 *                                                          *
 * ARGUMENTS                                                *
 *      INPUT   sg_array - array with spectrogram of color  *
 *              wl_array1 - wavelengths for which spectro-  *
 *                          gram was defined                *
 *              wl1 - number of wavelengths for spectrogram *
 *              x_array, y_array, z_array - CIE XYZ-curves  *
 *              wl_array2 - wavelengths for which XYZ-curves*
 *                          were defined                    *
 *              wl2 - number of wavelengths for XYZ curvers *
 *                                                          *
 *      OUTPUT  xyz_res - XYZ values                        *
 *                                                          *
 * RETURN        None.                                      *
 *                                                          *
 * GLOBALS:      None.
 *                                                          *
 * NOTES         Procedure calculates XYZ as integrals of   *
 *               f(t)*g(t) over interval in which the       *
 *               functions were defined. Function f(t) is   *
 *               spectral characteristic of color. Function *
 *               g(t) is X, Y, Z curves acc. for XYZ values.*
 *               f(t) and g(t) are piece-wise linear        *
 *               functions.                                 *
 *                                                          *
 * -------------------------------------------------------- */

#include   "integra.h"
#include   "icol.h"
#include   "icol_own.h"
#include   "algebra.h"


VOID co_xyz_calculation(xyz_res,
                        sg_array, wl_array1, wl1,
                        x_array, y_array, z_array, wl_array2, wl2)
  OUT double   *xyz_res;   /* resultant XYZ values */
  IN  double   *sg_array;  /* spectrogram of color */
  IN  double   *wl_array1; /* wavelengths for which
                              spectrogram was defined */
  IN  int      wl1;        /* number of wavelengths for spectrogram */
  IN  double   *x_array;   /* CIE X-curve */
  IN  double   *y_array;   /* CIE Y-curve */
  IN  double   *z_array;   /* CIE Z-curve */
  IN  double   *wl_array2; /* wavelengths for which XYZ-curves
                              were defined */
  IN  int      wl2;        /* number of wavelengths for XYZ curvers */

  {
  int indx1, indx2;  /* indexes in wavelength arrays */
  double sg_old, sg_tmp; /* tmp values for spectrogram function */
  /* tmp values for XYZ covers function */
  double xyz_old[3], xyz_tmp[3];
  double wl_old;       /* left point of the interval */

  /* initialization */
  CLEAR_3D(xyz_res)
  indx1 = 0;
  indx2 = 0;
  sg_old = 0;
  CLEAR_3D(xyz_old)

  /* wl_old = min(wl_array1[indx1], wl_array2[indx2]); */
  wl_old = MIN2(wl_array1[0], wl_array2[0]);

  /* integrals calculation */
  while (indx1 < wl1 && indx2 < wl2)
    {
    if (wl_array1[indx1] < wl_array2[indx2])
      {
      /* linear interpolation for XYZ-curves */
      xyz_tmp[0] =
        co_li_calc(wl_array2, x_array, indx2, wl_array1[indx1]);
      xyz_tmp[1] =
        co_li_calc(wl_array2, y_array, indx2, wl_array1[indx1]);
      xyz_tmp[2] =
        co_li_calc(wl_array2, z_array, indx2, wl_array1[indx1]);

      if (indx1 != 0)
        {
        xyz_res[0] += co_integral(wl_old, wl_array1[indx1],
                                  sg_old, sg_array[indx1],
                                  xyz_old[0], xyz_tmp[0]);

        xyz_res[1] += co_integral(wl_old, wl_array1[indx1],
                                  sg_old, sg_array[indx1],
                                  xyz_old[1], xyz_tmp[1]);

        xyz_res[2] += co_integral(wl_old, wl_array1[indx1],
                                  sg_old, sg_array[indx1],
                                  xyz_old[2], xyz_tmp[2]);
        }

      wl_old = wl_array1[indx1];
      sg_old = sg_array[indx1];
      COPY_3D(xyz_old, xyz_tmp)
      indx1++;
      }
    else
      {
      /* linear interpolation for spectrogram */
      sg_tmp =
        co_li_calc(wl_array1, sg_array, indx1, wl_array2[indx2]);

      if (indx2 != 0)
        {
        xyz_res[0] += co_integral(wl_old, wl_array2[indx2],
                                  sg_old, sg_tmp,
                                  xyz_old[0], x_array[indx2]);

        xyz_res[1] += co_integral(wl_old, wl_array2[indx2],
                                  sg_old, sg_tmp,
                                  xyz_old[1], y_array[indx2]);

        xyz_res[2] += co_integral(wl_old, wl_array2[indx2],
                                  sg_old, sg_tmp,
                                  xyz_old[2], z_array[indx2]);
        }

      wl_old = wl_array2[indx2];
      sg_old = sg_tmp;
      xyz_old[0] = x_array[indx2];
      xyz_old[1] = y_array[indx2];
      xyz_old[2] = z_array[indx2];
      indx2++;
      }
    }

  }


/// Calculates X, Y, Z of the predefined LS (D65 by default)
/// @param[out] xyz - XYZ colour of the predefined light spectrogram
/// @return SUCCESS/FAILURE
OKAY co_def_light_xyz(OUT double xyz[3])
  {
  return col_spec_xyz(co_light_points, co_light_wl, co_light, xyz);
  }


/* -------------------------------------------------------- *
 * FUNCTION      co_integral                                *
 * PURPOSE       calculation of intergal from t1 to t2      *
 *                                                          *
 * SPEC          Alex Voloboy (Rav)             '93.09.04   *
 * DESIGN        Alex Voloboy (Rav)             '93.09.04   *
 * DES. VV       Andrei Khodulev (Sah)          '93.09.04   *
 * CODING        Alex Voloboy (Rav)             '93.09.04   *
 * ARGUMENTS                                                *
 *      INPUT   t1, t2 - integral from t1 to t2             *
 *              f1, f2 - values f(t1) and f(t2)             *
 *              g1, g2 - values g(t1) and g(t2)             *
 *                                                          *
 *      OUTPUT  None.                                       *
 *                                                          *
 * RETURN        resultant value of intergal                *
 *                                                          *
 * GLOBALS:      None.                                      *
 *                                                          *
 * NOTES         integral from t1 to t2 of f(x) * g (x)     *
 *               where f, g - linear functions and          *
 *               f(t1) = f1, f(t2) = f2, g(t1) = g1,        *
 *               g(t2) = g2                                 *
 *                                                          *
 * -------------------------------------------------------- */

double co_integral(t1, t2, f1, f2, g1, g2)
  IN  double  t1, t2;   /* interval */
  IN  double  f1, f2;   /* f(t) values */
  IN  double  g1, g2;   /* g(t) values */
  {
  double res1, res2;    /* tmp variables */
  
  res1 = (f1 + f2) * (g1 + g2) * 0.25;
  res2 = (f2 - f1) * (g2 - g1) * 0.25;
  return (res1 + (res2 / 3.0)) * (t2 - t1);
  }

/* -------------------------------------------------------- *
 * FUNCTION      co_li_calc                                 *
 * PURPOSE       calculation of function value by linear    *
 *               interpolation                              *
 *                                                          *
 * SPEC          Alex Voloboy (Rav)             '93.09.04   *
 * DESIGN        Alex Voloboy (Rav)             '93.09.04   *
 * DES. VV       Andrei Khodulev (Sah)          '93.09.04   *
 * CODING        Alex Voloboy (Rav)             '93.09.04   *
 * ARGUMENTS                                                *
 *      INPUT   wl_array - pointer to wavelength array      *
 *              values - pointer to array with values       *
 *              ind - index of rigth value for interpolation*
 *              cur_wl - wavelength of required value       *
 *                                                          *
 *      OUTPUT  None.                                       *
 *                                                          *
 * RETURN        resultant value of linear interpolation    *
 *                                                          *
 * GLOBALS:      None.                                      *
 *                                                          *
 * NOTES        linear interpolation of function values[x]  *
 *              in segment [wl_array[ind-1], wl_array[ind]] *
 *              for x = cur_wl                              *
 *                                                          *
 * -------------------------------------------------------- */

double co_li_calc(wl_array, values, ind, cur_wl)
  IN  double *wl_array;    /* wavelength array */
  IN  double *values;      /* array with values of function */
  IN  int  ind;            /* index in arrays */
  IN  double cur_wl;       /* wavelength of required value */
  {
  double res;      /* resultant value */
  double tmp;

  /* if (function wasn't defined in cur_wl) */
  if (ind == 0)
    return (0.0);

  /* if (interpolation in the zero length segment) */
  if (wl_array[ind - 1] == wl_array[ind])
    return (values[ind]);

  res = values[ind - 1];
  tmp = (cur_wl - wl_array[ind - 1]) / (wl_array[ind] - wl_array[ind - 1]);
  res += (values[ind] - values[ind - 1]) * tmp;

  return (res);
  }
