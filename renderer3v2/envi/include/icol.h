/* -------------------------------------------------------- *
 * FILE NAME     icol.h                                     *
 * PURPOSE       Header of Integra COlor Library            *
 *                                                          *
 * SPEC          Alexei Voloboy (Rav)             '93.11    *
 * DESIGN        Alexei Voloboy (Rav)             '93.11    *
 * DES. VV       Oek                              '93.11    *
 * CODING        Alexei Voloboy (Rav)             '93.12.07 *
 * UPDATED       Edward Kopylov (Oek)             '96.02.07
 *                                                          *
 * NOTES         Spec. are in file icol_201.um              *
 *                                                          *
 * -------------------------------------------------------- */

#ifndef _ICOL_H_
#define _ICOL_H_


/* clipping methods */

#define COL_CLIP_METHOD_CLIP           0  /* method "preserves"
                                             intensity and hue but may
                                             change saturation */
#define COL_CLIP_METHOD_SCALE          1  /* method preserves the
                                             RGB-component color
                                             vector direction */
#define COL_CLIP_METHOD_CLAMP          2  /* method may change either
                                             intensity or hue and
                                             saturation */

/* modes for col_set_light() function */

#define COL_SET_WHITE                  1
#define COL_NULL                       0   

/* return code of ICOL functions */

#define COL_OK                             0
#define COL_ERR_NOT_INIT                   1
#define COL_ERR_WRONG_VALUE                2
#define COL_ERR_NOT_ENOUGH_MEMORY          3
#define COL_ERR_OUT_OF_RANGE               4

/* ICOL defaults  (CIE x, y) */
#define ICOL_DEF_WHITE_X           0.31275036564043807
#define ICOL_DEF_WHITE_Y           0.32905853112521194
#define ICOL_DEF_RED_X             0.622    /* Barco */  /* 0.628 */
#define ICOL_DEF_RED_Y             0.33000               /* 0.346 */
#define ICOL_DEF_GREEN_X           0.28300               /* 0.268 */
#define ICOL_DEF_GREEN_Y           0.61900               /* 0.588 */
#define ICOL_DEF_BLUE_X            0.14400               /* 0.150 */
#define ICOL_DEF_BLUE_Y            0.07000               /* 0.070 */


/* ICOL functions */
extern int  col_init PROT((VOID));
extern VOID col_term PROT((VOID));
extern int  col_error PROT((VOID));
extern char *col_strerror PROT((IN int));
extern char *icol_version PROT((VOID));

extern int  col_set_xyz PROT((IN int, IN double, IN double,
              IN double *, IN double *, IN double *));
extern int  col_set_light PROT((IN int, IN double, IN double,
              IN double *, IN int));
extern int  col_spec_xyz PROT((IN int, IN double *, IN double *,
              OUT double *));
extern int  col_attr_xyz PROT((IN int, IN double *, IN double *,
              OUT double *));

extern OKAY co_def_light_xyz PROT((OUT double xyz[3]));

extern OKAY co_get_def_light_xyz PROT((OUT double xyz[3]));

extern int  col_set_wrgb PROT((IN double *, IN double *, IN double *,
              IN double *));
extern int  col_get_wrgb PROT((OUT double *, OUT double *, OUT double *,
                               OUT double *));
extern int  col_xyz_rgb PROT((IN double *, IN double, OUT double *));
extern int  col_rgb_xyz PROT((IN double *, OUT double *));
extern int  col_clipping PROT((INOUT double *, IN int));

extern int  col_set_gamma PROT((IN double, IN double, IN double));
extern int  col_set_lum_curves PROT((IN double *, IN double *,
              IN double *));
extern int  col_set_gun_balance PROT((IN double, IN double, IN double));
extern int  col_rgb_fb PROT((IN double *, OUT int *));
extern int  col_r_fb   PROT((IN double, OUT int *));
extern int  col_g_fb   PROT((IN double, OUT int *));
extern int  col_b_fb   PROT((IN double, OUT int *));
extern int  col_fb_rgb PROT((IN int *, OUT double *));

/* ------- object oriented part --------- */
#define COL_OBJ_MAGIC 1710970L /* magic identifier for ball object */
typedef struct COL_HAND    /* visible part of ICOL object */
  {
  unsigned int magic;     /* manifest field */
  int  user_id;
  VPTR user_data;

  int err_code;
  int counter;            /* internal number of COL_HAND instance */
  double w[2], r[2], g[2], b[2];
  double xyz2rgb[3][3];
  double rgb2xyz[3][3];

  double gamma[3];       /* gamma correction values */
  int    method;         /* monitor correction method */
  double *lum[3];        /* pointers to luminance correction tables */
  int balance_flag;           /* gun balance execution attribute */
  double balance_value[3];    /* gun balance values */
  int curve_points_base;    /* base number of point in XYZ curves */
  double *x_curve_base;     /* base X curve */
  double *y_curve_base;     /* base Y curve */
  double *z_curve_base;     /* base Z curve */
  double *wavelength_base;  /* base curve wave lenghts */
  int light_points_base;    /* base number of point in reference spectrum */
  double *light_base;       /* base reference spectrum */
  double *light_wl_base;    /* base wave lenghts of reference spectrum */
  int points;               /* current number of wavel length */
  double *wave;             /* current curve wave lenghts */
  double *x_curve;          /* current X curve */
  double *y_curve;          /* current Y curve */
  double *z_curve;          /* current Z curve */
  double *light;            /* current reference spectrum */
  double xyz_light[3];      /* XYZ of the current light */
  } COL_HAND;

extern int  col_obj_error        PROT((IN COL_HAND *obj));
extern COL_HAND *col_obj_create  PROT((VOID));
extern VOID col_obj_destroy      PROT((IN COL_HAND *obj));
extern VOID col_obj_destroy_all  PROT((VOID));

extern OKAY col_obj_get_light_xyz PROT((IN  COL_HAND *obj, 
                                        OUT double xyz[3]));

extern OKAY col_obj_set_wrgb     PROT((IN COL_HAND *obj,
                                   IN double *, IN double *,
                                   IN double *, IN double *));
extern OKAY col_obj_get_wrgb PROT((IN COL_HAND *obj,
                                   OUT double *, OUT double *,
                                   OUT double *, OUT double *));
extern OKAY col_obj_xyz_rgb PROT((IN COL_HAND *obj, IN double *, IN double,
                                  OUT double *));
extern OKAY col_obj_rgb_xyz PROT((IN COL_HAND *obj, IN double *, OUT double *));
extern OKAY col_obj_set_gamma PROT((IN COL_HAND *obj, IN double, IN double,
                                    IN double));
extern int  col_obj_set_lum_curves PROT((IN COL_HAND *obj, IN double *,
                                    IN double *, IN double *));
extern int  col_obj_set_gun_balance PROT((IN COL_HAND *obj, IN double,
                                    IN double, IN double));
extern int  col_obj_rgb_fb PROT((IN COL_HAND *obj, IN double *, OUT int *));
extern int  col_obj_r_fb   PROT((IN COL_HAND *obj, IN double, OUT int *));
extern int  col_obj_g_fb   PROT((IN COL_HAND *obj, IN double, OUT int *));
extern int  col_obj_b_fb   PROT((IN COL_HAND *obj, IN double, OUT int *));
extern int  col_obj_fb_rgb PROT((IN COL_HAND *obj, IN int *, OUT double *));

extern int  col_obj_refresh_spec PROT((IN COL_HAND *obj));
extern int  col_obj_ref_spec     PROT((IN COL_HAND *obj, IN int, IN double *));
extern int  col_obj_set_xyz      PROT((IN COL_HAND *obj, IN int, IN double,
                                       IN double, IN double *, IN double *,
                                       IN double *));
extern int  col_obj_set_light    PROT((IN COL_HAND *obj, IN int, IN double,
                                       IN double, IN double *, IN int));
extern int  col_obj_spec_xyz     PROT((IN COL_HAND *obj, IN double *,
                                       OUT double *));
extern int  col_obj_attr_xyz     PROT((IN COL_HAND *obj, IN double *,
                                       OUT double *));



#endif
