/*
 *  MODULE NAME: rgb_clip.c
 *
 *  
 *  SPEC      R. Hall "Illumintion and Color in Comp. Generated Graph."
 *  DESIGN    R. Hall "Illumintion and Color in Comp. Generated Graph."
 *  CODING    M(MW) Nov. 1991
 *  UPDATED   Alexei Voloboy (Rav)   Dec. 1993
 *       Updated for ICOL library (change return codes, add comments)
 * TESTING       Alex Voloboy (Rav)             '93.12.15
 *
 *
 *  MODULE PURPOSE: module contains different color clipping functions
 *
 */ 

/********************************************************************/


#include "integra.h"
#include "icol.h"
#include "icol_own.h"


/*******************************************************************/

/*
 *
 *    FUNCTION: int co_rgb_clip()
 *
 *    PURPOSE: color cliping to get monitor displayable RGB values
 *      (clipping "preserves" intensity and hue, but may change
 *       saturation)
 *
 *    INPUT: 
 *      PARAMETERS: double rgb[3] - color components
 *      GLOBALS: none
 *    OUTPUT:
 *      PARAMETERS: double rgb[3] - clipped (if necessary) color
 *                                  components
 *      GLOBALS: none
 *
 *    RETURN VALUES:
 *      COL_OK
 *
 */

int co_rgb_clip(rgb)
  INOUT double *rgb;

  {
  int i;
  double color[3], white_cmp, tmp_sat;
  double satur_ratio = 1.;

  /* check if clipping is required */
  if (rgb[0] >= 0. && rgb[0] <= 1. && rgb[1] >= 0. && rgb[1] <= 1. &&
    rgb[2] >= 0. && rgb[2] <= 1.)
    return (COL_OK);

  /* value of white_cmp ("whitness") may be understood
     as color "intensity" */
  white_cmp = (rgb[0] + rgb[1] + rgb[2]) / 3.;

  if (white_cmp < COL_MIN_CLIP)
    {
    /* "intensity" is to small or negative */
    rgb[0] = rgb[1] = rgb[2] = 0.;
    return (COL_OK);
    }

  if (white_cmp > COL_MAX_CLIP)
    {
    /* "intensity" is to large to express any color
                          (full white displayed) */
    rgb[0] = rgb[1] = rgb[2] = 1.;
    return (COL_OK);
    }

  /* direction of color[] vector corresponds to hue, its length to
    saturation */
  color[0] = rgb[0] - white_cmp;
  color[1] = rgb[1] - white_cmp;
  color[2] = rgb[2] - white_cmp;

  /* calculation of how much saturation remains after clipping:
     satur_ratio */
  for (i = 0; i < 3; i++)
    if (rgb[i] > 1.)
      {
      if ((tmp_sat = (COL_MAX_CLIP - white_cmp) / color[i])
                              < satur_ratio)
        satur_ratio = tmp_sat;
      }
    else if (rgb[i] < 0.)
      {
      if ((tmp_sat = (COL_MIN_CLIP - white_cmp) / color[i])
                              < satur_ratio)
        satur_ratio = tmp_sat;
      }

  /* RGB clipping */
  rgb[0] =  white_cmp + (satur_ratio * color[0]);
  rgb[1] =  white_cmp + (satur_ratio * color[1]);
  rgb[2] =  white_cmp + (satur_ratio * color[2]);

  return (COL_OK);
  }

/*******************************************************************/

/*
 *
 *    FUNCTION: int co_rgb_scale()
 *
 *    PURPOSE: color cliping to get monitor displayable RGB values
 *      (clipping may change either intensity or hue and saturation;
 *      it preserves the RGB-component color vector direction)
 *
 *    INPUT: 
 *      PARAMETERS: double rgb[3] - color components
 *      GLOBALS: none
 *    OUTPUT:
 *      PARAMETERS: double rgb[3] - clipped (if necessary) color
 *                                  components
 *      GLOBALS: none
 *
 *    RETURN VALUES:
 *      COL_OK
 *
 *    NOTES: function shortens RGB-component color vector
 *           (if it lies outside the displayable color gamut)
 *           to the point, that after shortening it lies on the
 *           boundary of the displayable color cube (see
 *           OPAR03DS.MMW for more)
 *
 */

int co_rgb_scale(rgb)
  INOUT double *rgb;

  {
  double tmp_scale, scale = 1.;

  /* check of range of R color component */
  if (rgb[0] < 0.)
    rgb[0] = 0.;
  else if ((rgb[0] > 1.) && ((tmp_scale = (1. / rgb[0])) < scale))
    scale = tmp_scale;

  /* check of range of G color component */
  if (rgb[1] < 0.)
    rgb[1] = 0.;
  else if ((rgb[1] > 1.) && ((tmp_scale = (1. / rgb[1])) < scale))
    scale = tmp_scale;

  /* check of range of B color component */
  if (rgb[2] < 0.)
    rgb[2] = 0.;
  else if ((rgb[2] > 1.) && ((tmp_scale = (1. / rgb[2])) < scale))
    scale = tmp_scale;

  /* if clipping is necessary */
  if (scale < 1.)
    {
    rgb[0] *= scale;
    rgb[1] *= scale;
    rgb[2] *= scale;
    }

  return (COL_OK);
  }

/*******************************************************************/

/*
 *
 *    FUNCTION: int co_rgb_clamp()
 *
 *    PURPOSE: color cliping to get monitor displayable RGB values
 *      (clipping may change either intensity or hue and saturation;
 *      each component exceeding allowable value is clamped to that
 *      value)
 *
 *    INPUT: 
 *      PARAMETERS: double rgb[3] - color components
 *      GLOBALS: none
 *    OUTPUT:
 *      PARAMETERS: double rgb[3] - clipped (if necessary) color
 *                                  components
 *      GLOBALS: none
 *
 *    RETURN VALUES:
 *      COL_OK
 *
 *    NOTES: function simply clamps this color RGB components which
 *           are outside the displayable color gamut (see
 *           OPAR03DS.MMW for more)
 *
 */

int co_rgb_clamp(rgb)
  INOUT double *rgb;

  {

  /* check of range of R color component */
  if (rgb[0] < 0.)
    rgb[0] = 0.;
  else if (rgb[0] > 1.)
    rgb[0] = 1.;

  /* check of range of G color component */
  if (rgb[1] < 0.)
    rgb[1] = 0.;
  else if (rgb[1] > 1.)
    rgb[1] = 1.;

  /* check of range of B color component */
  if (rgb[2] < 0.)
    rgb[2] = 0.;
  else if (rgb[2] > 1.)
    rgb[2] = 1.;

  return (COL_OK);
  }

/******************************************************************/
