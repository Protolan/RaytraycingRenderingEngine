/* -------------------------------------------------------- *
 * FILE NAME     lcd_cre.c                                  *
 *                                                          *
 * FUNCTION      co_create_xyz2rgb                          *
 * PURPOSE       creation from XYZ to RGB converting matrix *
 *                                                          *
 * SPEC          Rogers D. "Procedural ..."          1985   *
 * DESIGN        Alex Voloboy (Rav)             '93.09.04   *
 * DES. VV       Alex Voloboy (Rav)             '93.09.04   *
 * CODING        Alex Voloboy (Rav)             '93.09.04   *
 * UPDATED       Alex Voloboy (Rav)             '93.12.07   *
 *    Updated for ICOL Version 1.0 library.                 *
 * UPDATED       Alex Voloboy (Rav)             '94.04.27   *
 *    Updated for ICOL Version 2.0 library.                 *
 * TESTING       Alex Voloboy (Rav)               '94.05.11 *
 *                                                          *
 * ARGUMENTS                                                *
 *      INPUT    red - x, y components for RED color        *
 *               gre - x, y components for GREEN color      *
 *               blu - x, y components for BLUE color       *
 *               whi - x, y components for WHITE color      *
 *      OUTPUT  None.                                       *
 *                                                          *
 * GLOBALS:      co_xyz_2_rgb[3][3], co_rgb_2_xyz[3][3]     *
 *                                                          *
 * NOTES         Creation of co_xyz_2_rgb and co_rgb_2_xyz  *
 *               matrixes is well described in "Procedural  *
 *               elements for computer graphics" by Rogers  *
 *                                                          *
 * -------------------------------------------------------- */

#include   "integra.h"
#include   "icol.h"
#include   "icol_own.h"
#include   "algebra.h"


int co_create_xyz2rgb(xyz2rgb, rgb2xyz, red, gre, blu, whi)
OUT double xyz2rgb[3][3];
OUT double rgb2xyz[3][3];
IN double *red;        /* x, y components for RED color */
IN double *gre;        /* x, y components for GREEN color */
IN double *blu;        /* x, y components for BLUE color */
IN double *whi;        /* x, y components for WHITE color */

  {
  double  crd, cgd, cbd, d;   /* variables name are correspondent to
                              formulae in Rogers D. "Procedural..." */
  /* subtractions of y-component */
  double  sub_ygb, sub_ybr, sub_yrg;
  /* subtractions of x-component */
  double  sub_xgb, sub_xbr, sub_xrg;
  double  rev_yw;                       /* = 1.0/yw */

  /* check input value */
  if (whi[1] == 0)
    return (COL_ERR_WRONG_VALUE);

  rev_yw = 1 / whi[1];
  sub_xgb = gre[0] - blu[0];
  sub_xbr = blu[0] - red[0];
  sub_xrg = red[0] - gre[0];
  sub_ygb = gre[1] - blu[1];
  sub_ybr = blu[1] - red[1];
  sub_yrg = red[1] - gre[1];

  crd = whi[0] * sub_ygb - whi[1] * sub_xgb +
        gre[0] * blu[1] - blu[0] * gre[1];
  crd *= rev_yw;

  cgd = whi[0] * sub_ybr - whi[1] * sub_xbr -
        red[0] * blu[1] + blu[0] * red[1];
  cgd *= rev_yw;

  cbd = whi[0] * sub_yrg - whi[1] * sub_xrg +
       red[0] * gre[1] - gre[0] * red[1];
  cbd *= rev_yw;

  /* check input value */
  if (IS_ZERO_1D(crd) || IS_ZERO_1D(cgd) || IS_ZERO_1D(cbd))
    return (COL_ERR_WRONG_VALUE);

  {
  double tmp, crd_rev;

  tmp = blu[1] * gre[0] - gre[1] * blu[0];
  crd_rev = 1.0 / crd;
  xyz2rgb[0][0] = (tmp + sub_ygb) * crd_rev;
  xyz2rgb[0][1] = (tmp - sub_xgb) * crd_rev;
  xyz2rgb[0][2] = tmp * crd_rev;
  }

  {
  double tmp, cgd_rev;

  tmp = blu[0] * red[1] - red[0] * blu[1];
  cgd_rev = 1.0 / cgd;
  xyz2rgb[1][0] = (tmp + sub_ybr) * cgd_rev;
  xyz2rgb[1][1] = ( tmp - sub_xbr) * cgd_rev;
  xyz2rgb[1][2] = tmp * cgd_rev;
  }

  {
  double tmp, cbd_rev;

  tmp = red[0] * gre[1] - gre[0] * red[1];
  cbd_rev = 1.0 / cbd;
  xyz2rgb[2][0] = (tmp + sub_yrg) * cbd_rev;
  xyz2rgb[2][1] = (tmp - sub_xrg) * cbd_rev;
  xyz2rgb[2][2] = tmp * cbd_rev;
  }

  d = red[0] * sub_ygb + gre[0] * sub_ybr + blu[0] * sub_yrg;

  /* check input value */
  if (IS_ZERO_1D(d))
    return COL_ERR_WRONG_VALUE;

  {
  double crd_d, cgd_d, cbd_d;

  crd_d = crd / d;
  cgd_d = cgd / d;
  cbd_d = cbd / d;
  rgb2xyz[0][0] = red[0] * crd_d;
  rgb2xyz[0][1] = gre[0] * cgd_d;
  rgb2xyz[0][2] = blu[0] * cbd_d;

  rgb2xyz[1][0] = red[1] * crd_d;
  rgb2xyz[1][1] = gre[1] * cgd_d;
  rgb2xyz[1][2] = blu[1] * cbd_d;

  rgb2xyz[2][0] = (1 - red[0] - red[1]) * crd_d;
  rgb2xyz[2][1] = (1 - gre[0] - gre[1]) * cgd_d;
  rgb2xyz[2][2] = (1 - blu[0] - blu[1]) * cbd_d;
  }

  return (COL_OK);
  }
/*********************************************************************/
