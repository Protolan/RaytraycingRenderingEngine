/* FILE NAME    itoliifl.h
 * PURPOSE      Over-IIFL protocol definition for INTEGRA's applications.
 *
 * SPEC         Rjw                                   '91.11
 * LAST UPDATED Rjw                                   '91.11
 *                 - Initially in "itbtiifl.h"
 *              Erk                                   '92.08
 *                 - Including into INTEGRA environment
 *
 * NOTES
 *
 * The format and contents of Integra applications' common IIFL
 * files are defined in this file.
 * Component names, variables names, variables formats defined here
 * should be used in application code operating on IIFL files.
 *
 * PREFIXES of defined elements:
 *   IF_C_   component
 *   IF_V_   variable
 *   IF_F_   file
 *
 * Each FILE DEFINITION is set of components and variables
 * some of them is obligatory and rest is optional
 * The constant IF_END_LIST finish component/variables list.
 *
 * Warning:
 *   The following suffixes are defined in this header:
 *
 *     IF_SUFX_IMG  ".rgb"
 *     IF_SUFX_ILL  ".lux"
 *     IF_SUFX_LUM  ".nit"
 *     IF_SUFX_FTM  ".ftm"
 *     IF_SUFX_TI   ".ti"
 *     IF_SUFX_BI   ".bi"
 *
 */


#ifndef _ITOLIIFL_H
#define _ITOLIIFL_H


/* General purpose constants */

#define IF_COMMA              ","   /* separator for component names */



/*-------------  COMPONENT DEFINITION ----------*/

#define IF_C_RED            0
#define IF_C_GRE            1
#define IF_C_BLU            2
#define IF_C_MATTE          3

#define IF_C_ILLUM_R        4
#define IF_C_ILLUM_G        5
#define IF_C_ILLUM_B        6
#define IF_C_LUM_R          7
#define IF_C_LUM_G          8
#define IF_C_LUM_B          9

#define IF_C_TRG_FLAGS     10
#define IF_C_TRG_INDEX     11
#define IF_C_TEX_AREA_X1   12
#define IF_C_TEX_AREA_Y1   13
#define IF_C_TEX_AREA_X2   14
#define IF_C_TEX_AREA_Y2   15

#define IF_C_SPECULAR_R_FL 16
#define IF_C_SPECULAR_G_FL 17
#define IF_C_SPECULAR_B_FL 18
#define IF_C_DIFFUSE_R_FL  19
#define IF_C_DIFFUSE_G_FL  20
#define IF_C_DIFFUSE_B_FL  21

#define IF_C_SPECULAR_R_INT 22
#define IF_C_SPECULAR_G_INT 23
#define IF_C_SPECULAR_B_INT 24
#define IF_C_DIFFUSE_R_INT  25
#define IF_C_DIFFUSE_G_INT  26
#define IF_C_DIFFUSE_B_INT  27

#define IF_C_GRADIENT_X     28
#define IF_C_GRADIENT_Y     29
#define IF_C_GRAY           30

/* New accuracy/intensity components (OMAO) */
#define IF_C_ILLUM_A        31
#define IF_C_LUM_A          32
#define IF_C_GON_R          33
#define IF_C_GON_G          34
#define IF_C_GON_B          35
#define IF_C_GON_A          36
#define IF_C_ILLUM_Y        37
#define IF_C_LUM_Y          38
#define IF_C_GON_Y          39

/* Component name and type macros */

#define IF_C_DEFINED(n) (ifl_cmps[n].name != NULL)
#define IF_C_NAME(n)    (ifl_cmps[n].name)
#define IF_C_TYPE(n)    (ifl_cmps[n].type)



/*------ VARIABLES DEFINITION ---------*/

#define IF_V_FILE_TYPE        0
#define IF_V_IMG_SCR_POS      1
#define IF_V_BACKGROUND       2
#define IF_V_IMG_PIX_STEP     3
#define IF_V_IMG_PIX_SIZE     4
#define IF_V_PIXEL_SIZE_MM    5
#define IF_V_IMG_ORIGIN       6
#define IF_V_TDC              7
#define IF_V_REPRES_COL       8
#define IF_V_AVER_MATTE       9
#define IF_V_DUPL_TYPE       10
#define IF_V_DUPL_DISPL      11
#define IF_V_DUPL_RNDSEED    12
#define IF_V_DESCRIPTION     13
#define IF_V_KEYWORDS        14
#define IF_V_USAGE           15
#define IF_V_KD_ATTR         16
#define IF_V_KS_ATTR         17
#define IF_V_KT_ATTR         18
#define IF_V_KA_ATTR         19
#define IF_V_GS_ATTR         20
#define IF_V_TXT_COL_CHANGE  21
#define IF_V_DEV_FACTOR      22
#define IF_V_REV_CONV        23
#define IF_V_KTD_ATTR        24

 /* RFRE  - Ovn 95.02.24 */

#define IF_V_KF_ATTR         25

/* Fgk, AT5628db.Fgk */
#define IF_V_VIRT_MAP          26
#define IF_V_MAPP_DIR          27
#define IF_V_K_DIFF_BRDF_ATTR  28
#define IF_V_K_DIFF_BTDF_ATTR  29
#define IF_V_K_SPEC_BRDF_ATTR  30
#define IF_V_K_SPEC_BTDF_ATTR  31
#define IF_V_SELF_LUM_ATTR     32
#define IF_V_GAMUT             33
#define IF_V_RAY_NUMBER        34


/* Variable name, format and default macros */

#define IF_V_DEFINED(n)  (ifl_vars[n].name != NULL)
#define IF_V_NAME(n)     (ifl_vars[n].name)
#define IF_V_FORMAT(n)   (ifl_vars[n].format)
#define IF_V_DEFAULT(n)  (ifl_vars[n].def)


/*------------ FILE DEFINITIONS ---------------*/

#define IF_SUFX_IMG  ".rgb"
#define IF_SUFX_ILL  ".lux"
#define IF_SUFX_LUM  ".nit"
#define IF_SUFX_FTM  ".ftm"
#define IF_SUFX_TI   ".ti"
#define IF_SUFX_BI   ".bi"

#define IF_END_LIST       -1

/* these constants may be enlarged if necessary */

#define IF_MAX_CMPS_NAME_LEN  40
#define IF_MAX_CMPS           15
#define IF_MAX_VARS           33  /* 25 , AT5628db.Fgk */

/* File types */

#define IF_F_RGB           0
#define IF_F_RGBM          1
#define IF_F_LUX           2 /* illuminance */
#define IF_F_NIT           3 /* luminance */
#define IF_F_FTMSI         4
#define IF_F_FTMRGB        5
#define IF_F_TI            6
#define IF_F_BI            7
#define IF_F_GON           8 /* GD file */


/* File definition access macros */

/* check if file type with specified index is end of file list */
#define IF_F_DEFINED(n)          (ifl_files[n].name != NULL)

/* get text for FILE_TYPE variable */
#define IF_F_TYPE_NAME(type)     (ifl_files[type].name)

/* get file type suffix */
#define IF_F_TYPE_SUFFIX(type)   (ifl_files[type].suffix)

/* check if end marker of obligatory/optional component list */
#define IF_F_OBLCMP_END(type,n)  (ifl_files[type].obl_cmps[n] == IF_END_LIST)
#define IF_F_OPTCMP_END(type,n)  (ifl_files[type].opt_cmps[n] == IF_END_LIST)

/* get obligatory/optional component index */
#define IF_F_OBLCMP(type, n)     (ifl_files[type].obl_cmps[n])
#define IF_F_OPTCMP(type, n)     (ifl_files[type].opt_cmps[n])

/* check if end marker of obligatory/optional variable list */
#define IF_F_OBLVAR_END(type,n)  (ifl_files[type].obl_vars[n] == IF_END_LIST)
#define IF_F_OPTVAR_END(type,n)  (ifl_files[type].opt_vars[n] == IF_END_LIST)

/* get obligatory/optional variable index */
#define IF_F_OBLVAR(type, n)     (ifl_files[type].obl_vars[n])
#define IF_F_OPTVAR(type, n)     (ifl_files[type].opt_vars[n])


/* File definition access functions */

/*  IF_F_check() return codes */
#define IF_F_FULL_DEF       4  /* obligatory + optional definition completed */
#define IF_F_OBL_DEF        3  /* obligatory definition completed */
#define IF_F_PART_DEF       2  /* less than obligatory definition */
#define IF_F_NOT_DEF        1  /* file type not defined */
#define IF_F_IIF_ACCESS     0  /* IIFL file access error */


extern int IF_F_check PROT((IIF *, int));



/*
 * DONT USE BELOW DECLARATIONS IN APPLICATION CODE
 *
 * Only for internal ITOLIIFL usage !
 */

typedef struct IF_COMPONENT
  {
  char *name;
  char type;
  } IF_COMPONENT;


typedef struct IF_VARIABLE
  {
  char *name;
  char *format;
  char *def;
  } IF_VARIABLE;


typedef struct IF_FILE
  {
  char *name;  /* for FILE_TYPE variable */
  char *suffix;
  short obl_cmps[IF_MAX_CMPS];
  short opt_cmps[IF_MAX_CMPS];
  short obl_vars[IF_MAX_VARS];
  short opt_vars[IF_MAX_VARS];
  } IF_FILE;


extern IF_COMPONENT ifl_cmps[];
extern IF_VARIABLE  ifl_vars[];
extern IF_FILE      ifl_files[];


#endif /* _ITOLIIFL_H */


/* END OF FILE 'itoliifl.h' */
