/* -------------------------------------------------------- *
 * FILE NAME     icol_own.h                                 *
 * PURPOSE       Own header of Integra COlor Library        *
 *                                                          *
 * SPEC          Alexei Voloboy (Rav)             '93.11    *
 * DESIGN        Alexei Voloboy (Rav)             '93.11    *
 * DES. VV       Oek                              '93.11    *
 * CODING        Alexei Voloboy (Rav)             '93.12.07 *
 * UPDATED       Edward Kopylov (Oek)             '96.02.07
 *                                                          *
 * NOTES         Spec. are in file icol_100.um              *
 *                                                          *
 * -------------------------------------------------------- */

#ifndef _ICOL_OWN_H_
#define _ICOL_OWN_H_

/* Initialization macros */
#ifdef INITIALIZE
#define  ICOL_INITIATE(x) = x
#else
#define  ICOL_INITIATE(x)
#endif


#define COL_SHADES    256
#define FB_MAX        255
#define FB_MAX_REV    0.0039215686274510   /* 1 / 255 */

/* clipping constant */

#define COL_MAX_CLIP                    0.999999
#define COL_MIN_CLIP                    0.000001

/* RGB <-> FB convertion methods */

#define COL_GAMMA                       1
#define COL_TABLE                       2

/* ICOL global variables */

/* error code */
GLOBAL  int       co_error_code ICOL_INITIATE(COL_ERR_NOT_INIT);
/* ICOL working attribute */
GLOBAL  int       co_icol_init ICOL_INITIATE(NO);

/* memory allocation attribute */
GLOBAL  int       co_mem_class;
GLOBAL  int       co_wl_alloc ICOL_INITIATE(NO);
GLOBAL  int       co_xc_alloc ICOL_INITIATE(NO);
GLOBAL  int       co_yc_alloc ICOL_INITIATE(NO);
GLOBAL  int       co_zc_alloc ICOL_INITIATE(NO);
GLOBAL  int       co_wlli_alloc ICOL_INITIATE(NO);
GLOBAL  int       co_li_alloc ICOL_INITIATE(NO);
GLOBAL  int       co_lum_alloc ICOL_INITIATE(NO);

/* RGB -> XYZ conversion matrix */
GLOBAL  double    co_rgb_2_xyz[3][3];
/* XYZ -> RGB conversion matrix */
GLOBAL  double    co_xyz_2_rgb[3][3];

GLOBAL  int       co_curve_points;   /* number of points in XYZ
                                        curves */
GLOBAL  double    *co_wavelength;    /* pointer on wavelength array */
GLOBAL  double    *co_x_curve;       /* pointer on X curve */
GLOBAL  double    *co_y_curve;       /* pointer on Y curve */
GLOBAL  double    *co_z_curve;       /* pointer on Z curve */

GLOBAL  int       co_light_points;   /* number of points in light */
GLOBAL  double    *co_light_wl;      /* pointer on the light
                                        wavelength array */
GLOBAL  double    *co_light;         /* pointer on light array */

GLOBAL  double    co_r_current[2];   /* current value for basic red
                                        color */
GLOBAL  double    co_g_current[2];   /* current value for basic green
                                        color */
GLOBAL  double    co_b_current[2];   /* current value for basic blue
                                        color */
GLOBAL  double    co_w_current[2];   /* current value for white color */

GLOBAL  double    co_gamma[3];       /* Gamma correction values */
GLOBAL  int       co_mc_method;      /* monitor correction method */
GLOBAL  double    *co_lum[3];        /* pointers to luminance correction
                                        tables */
GLOBAL  int       co_gun_balance;    /* gun balance execution
                                        attribute */
GLOBAL  double    co_gun_value[3];   /* gun balance values */

/// XYZ coordinates of the default light (one set by this moment)
GLOBAL double xyz_def_light[3];


/* ICOL internal functions */

extern int co_create_xyz2rgb PROT((OUT double xyz2rgb[3][3],
                                   OUT double rgb2xyz[3][3],
             IN double *, IN double *,
             IN double *, IN double *));
extern VOID co_xyz_calculation PROT((OUT double *, IN double *,
              IN double *, IN int, IN double *, IN double *,
              IN double *, IN double *, IN int));
extern double co_integral PROT((IN double, IN double, IN double,
                IN double, IN double, IN double));
extern double co_integral3 PROT((IN double, IN double, IN double,
                IN double, IN double, IN double, IN double,
                IN double));
extern double co_li_calc PROT((IN double *, IN double *, IN int,
                IN double));
extern int co_find_lum PROT((IN double *lum, IN double value));
extern int co_rgb_clip PROT((INOUT double *));
extern int co_rgb_scale PROT((INOUT double *));
extern int co_rgb_clamp PROT((INOUT double *));

#endif
