/* FILE NAME    itoliifl.c
 * PURPOSE      ITOLIIFL - Utility library for IIFL
 *
 * SPEC         Rjw                                  '91.11
 * DESIGN       Riw                                  '91.11
 * DES. VV      Rjw                                  '91.11
 * CODING       Rjw                                  '91.11
 * CODE VV.     Rjw                                  '91.11
 * TEST         Rjw                                  '91.11
 *
 * SWITCHES     None.
 * NOTES        None.
 *
 */

static char *sccsid =
"@(#)$Header: /home1/repository/envi/REPOSITORY/envi/portab/itol/itoliifl/itoliifl.c,v 1.2 1998/11/19 13:52:40 envi Exp $";


#include "integra.h"
#include "iifl.h"
#include "itoliifl.h"
#include "imal.h"

IF_COMPONENT ifl_cmps[] =
  {
    { "red",                  'b'}, /* 0 */
    { "gre",                  'b'},
    { "blu",                  'b'},
    { "matte",                'b'},
    { "ill red",              'f'},
    { "ill gre",              'f'},
    { "ill blu",              'f'},
    { "lum red",              'f'},
    { "lum gre",              'f'},
    { "lum blu",              'f'},
    { "tflags",               'b'},     /* 10 */
    { "tindex",               'l'},
    { "tax1",                 's'},
    { "tay1",                 's'},
    { "tax2",                 's'},
    { "tay2",                 's'},
    { "sredf",                'f'},
    { "sgref",                'f'},
    { "sbluf",                'f'},
    { "dredf",                'f'},
    { "dgref",                'f'},     /* 20 */
    { "dbluf",                'f'},
    { "sredi",                's'},
    { "sgrei",                's'},
    { "sblui",                's'},
    { "dredi",                's'},
    { "dgrei",                's'},
    { "dblui",                's'},
    { "grx",                  'f'},
    { "gry",                  'f'},     /* 29 */
    { "gray",                 'b'},     /* 30 */
    { "ill acc",              'f'},     /* OMAO GD/accuracy components */
    { "lum acc",              'f'},
    { "gon red",              'f'},
    { "gon gre",              'f'},
    { "gon blu",              'f'},
    { "gon acc",              'f'},     /* 36 */
    { "ill ray",              'f'},
    { "lum ray",              'f'},
    { "gon ray",              'f'},     /* 39 */
    { NULL, }  /* end of components */
  };


IF_VARIABLE ifl_vars[] =
  {
    { "FILE TYPE",                 "%s",        NULL },     /*0*/
    { "image screen offset",       "%d %d",     "0 0" },
    { "background color",          "%d %d %d",  "0 0 0" },
    { "image pixel step",          "%d %d",     "1 1" },
    { "step size [m]",             "%f %f",     NULL },
    { "pixel size [0.1 mm]",       "%d %d",     NULL },
    { "image TM origin",           "%d %d",     "0 0" },
    { "total design color",        "%d %d %d",  NULL },
    { "representative color",      "%d %d %d",  NULL },
    { "average matte",             "%d",        NULL },
    { "duplication type",          "%d",        "0" },      /*10*/
    { "duplication displacement",  "%d",        "0" },
    { "duplication rnd gen seed",  "%d",        "0" },
    { "image description",         "%s",        NULL},
    { "image keywords",            "%s",        NULL},
    { "image usage",               "%s",        NULL},      /*15*/
    { "kd_attr",                   "%f",        NULL},
    { "ks_attr",                   "%f",        NULL},
    { "kt_attr",                   "%f",        "0.0"},
    { "ka_attr",                   "%f",        NULL},
    { "gs_attr",                   "%d",        NULL},      /*20*/
    { "txt color change",          "%d",        "1"},
    { "dev_factor",                "%f",        NULL},
    { "convexity_rev",             "%d",        NULL},
    { "ktd_attr",                  "%f",        NULL},

    { "kf_attr",                   "%f",        NULL}, /* RFRE-Ovn 95.02.24 */

    { "virt_map",                  "%s",        ""},
    { "mapping direction",         "%f %f",     "0.0 1.0"},
    { "diff_brdf_attr",            "%f",        NULL},
    { "diff_btdf_attr",            "%f",        NULL},
    { "spec_brdf_attr",            "%f",        NULL},      /*30*/
    { "spec_btdf_attr",            "%f",        NULL},
    { "self_lum_attr",             "%f",        NULL},
    { "gamut",                     "%32.30f",   NULL},      /* 33 */
    { "RAY_NUMBER",                "%I64d",     "0"},       /* 34 */
    { NULL, } /* end of variables */
  };


IF_FILE ifl_files[] =
  {
    /* RGB file */
    { "RGB",    IF_SUFX_IMG,
      { IF_C_RED, IF_C_GRE, IF_C_BLU, IF_END_LIST, },
      { IF_END_LIST, },
      { IF_V_FILE_TYPE, IF_END_LIST, },
      { IF_V_IMG_SCR_POS, IF_END_LIST, }
    },

    /* RGBM file */
    { "RGBM", IF_SUFX_IMG,
      { IF_C_RED, IF_C_GRE, IF_C_BLU, IF_C_MATTE, IF_END_LIST, },
      { IF_END_LIST, },
      { IF_V_FILE_TYPE, IF_END_LIST, },
      { IF_V_BACKGROUND, IF_V_IMG_SCR_POS, IF_END_LIST, }
    },

    /* LUX file */
    { "ILLUMINANCE", IF_SUFX_ILL,
      { IF_C_ILLUM_R, IF_C_ILLUM_G, IF_C_ILLUM_B, IF_END_LIST,  },
      { IF_END_LIST, },
      { IF_V_FILE_TYPE, IF_END_LIST, },
      { IF_V_IMG_PIX_STEP, IF_V_IMG_PIX_SIZE, IF_END_LIST, }
    },

    /* NIT file */
    { "LUMINANCE", IF_SUFX_LUM,
      { IF_C_LUM_R, IF_C_LUM_G, IF_C_LUM_B, IF_END_LIST, },
      { IF_END_LIST, },
      { IF_V_FILE_TYPE, IF_END_LIST, },
      { IF_V_IMG_PIX_STEP, IF_V_IMG_PIX_SIZE, IF_END_LIST, }
    },

    /* FTM SI file */
    { "FTM_SI", IF_SUFX_FTM,
      { IF_C_TRG_FLAGS, IF_C_TRG_INDEX,
        IF_C_TEX_AREA_X1, IF_C_TEX_AREA_Y1, IF_C_TEX_AREA_X2, IF_C_TEX_AREA_Y2,
        IF_C_SPECULAR_R_FL, IF_C_SPECULAR_G_FL, IF_C_SPECULAR_B_FL,
        IF_C_DIFFUSE_R_FL, IF_C_DIFFUSE_G_FL, IF_C_DIFFUSE_B_FL,
        IF_END_LIST, },
      { IF_END_LIST, },
      { IF_V_FILE_TYPE, IF_END_LIST, },
      { IF_END_LIST, }
    },

    /* FTM RGB file */
    { "FTM_RGB", IF_SUFX_FTM,
      { IF_C_TRG_FLAGS, IF_C_TRG_INDEX,
        IF_C_TEX_AREA_X1, IF_C_TEX_AREA_Y1, IF_C_TEX_AREA_X2, IF_C_TEX_AREA_Y2,
        IF_C_SPECULAR_R_INT, IF_C_SPECULAR_G_INT, IF_C_SPECULAR_B_INT,
        IF_C_DIFFUSE_R_INT, IF_C_DIFFUSE_G_INT, IF_C_DIFFUSE_B_INT,
        IF_END_LIST, },
      { IF_END_LIST, },
      { IF_V_FILE_TYPE, IF_END_LIST, },
      { IF_END_LIST, }
    },

    /* TI file */
    { "TEXTURE IMAGE", IF_SUFX_TI,
      { IF_C_RED, IF_C_GRE, IF_C_BLU, IF_END_LIST, },
      { IF_C_MATTE, IF_END_LIST, },
      { IF_V_FILE_TYPE, IF_V_PIXEL_SIZE_MM, IF_END_LIST, },
      { IF_V_IMG_ORIGIN, IF_V_TDC, IF_V_REPRES_COL, IF_V_AVER_MATTE,
        IF_V_DUPL_TYPE, IF_V_DUPL_DISPL, IF_V_DUPL_RNDSEED,
        IF_V_DESCRIPTION, IF_V_KEYWORDS, IF_V_USAGE, IF_V_KTD_ATTR,
        IF_V_KD_ATTR, IF_V_KS_ATTR, IF_V_KT_ATTR, IF_V_KA_ATTR, IF_V_GS_ATTR,

/* RFRE-Ovn 95.02.24 */

        IF_V_KF_ATTR,

        IF_V_DESCRIPTION, IF_V_KEYWORDS, IF_V_USAGE, IF_V_TXT_COL_CHANGE,
        IF_V_DEV_FACTOR, IF_V_REV_CONV, IF_V_VIRT_MAP, IF_V_MAPP_DIR,
        IF_V_K_DIFF_BRDF_ATTR, IF_V_K_DIFF_BTDF_ATTR,
        IF_V_K_SPEC_BRDF_ATTR, IF_V_K_SPEC_BTDF_ATTR, IF_V_SELF_LUM_ATTR,
        IF_END_LIST, }
    },

    /* BI file */
    { "BUMP IMAGE", IF_SUFX_BI,
      { IF_C_GRADIENT_X, IF_C_GRADIENT_Y, IF_END_LIST, },
      { IF_END_LIST, },
      { IF_V_FILE_TYPE, IF_V_PIXEL_SIZE_MM, IF_END_LIST, },
      { IF_V_IMG_ORIGIN, IF_V_DUPL_TYPE, IF_V_DUPL_DISPL, IF_V_DUPL_RNDSEED,
        IF_V_DESCRIPTION, IF_V_KEYWORDS, IF_V_USAGE, IF_END_LIST, }
    },

    /* LUX file (GD) */
    { "INTENSITY", IF_SUFX_ILL,
      { IF_C_GON_R, IF_C_GON_G, IF_C_GON_B, IF_END_LIST,  },
      { IF_END_LIST, },
      { IF_V_FILE_TYPE, IF_END_LIST, },
      { IF_V_IMG_PIX_STEP, IF_V_IMG_PIX_SIZE, IF_END_LIST, }
    },
    { NULL, } /* end of file array */
  };



int IF_F_check(ifp, ftype)
IIF *ifp;
int ftype;
  {
  char *cmps, *types;
  char *cm[max(IF_MAX_CMPS, IF_MAX_VARS)];
  char *cp;
  int i, j, k, f, x;
  int obc, opc, obv, opv;

  for (i = 0; IF_F_DEFINED(i); i++)
   ;

  if (ftype >= i)
    return IF_F_NOT_DEF;

  if (iif_get_components(ifp, &cmps, &types) != IIF_OK)
    return IF_F_IIF_ACCESS;

  k = 0;
  cp = cmps;
  if (*cp != 0 && cp != NULL)
    {
    cm[k] = cmps;
    k++;
    }

  while (*(++cp))
    if (*cp == ',')
      {
      *cp = 0;
      cm[k++] = cp + 1;
      }

  for (f = 0, i = 0; ! IF_F_OBLCMP_END(ftype, i); i++)
    {
    x = IF_F_OBLCMP(ftype, i);

    for (j = 0; j < k; j++)
      {
      if ((strcmp(cm[j], IF_C_NAME(x)) == 0) &&
          (IF_C_TYPE(x) == types[j]))
        {
        f++;
        break;
        }
      }
    }

  obc = (f == i);

  for (f = 0, i = 0; ! IF_F_OPTCMP_END(ftype, i); i++)
    {
    x = IF_F_OPTCMP(ftype, i);

    for (j = 0; j < k; j++)
      {
      if ((strcmp(cm[j], IF_C_NAME(x)) == 0) &&
          (IF_C_TYPE(x) == types[j]))
        {
        f++;
        break;
        }
      }
    }

  opc = (f == i);

  RTL_free(cmps);
  RTL_free(types);

  for (f = 0, i = 0; ! IF_F_OBLVAR_END(ftype, i); i++)
    {
    x = IF_F_OBLVAR(ftype, i);

    if (x == IF_V_FILE_TYPE)
      f++;
    else
      {
      if ((iif_get_var(ifp, IF_V_NAME(x)) != NULL) ||
          (IF_V_DEFAULT(x) != NULL))
        {
        f++;
        }
      }
    }

  obv = (f == i);

  for (f = 0, i = 0; ! IF_F_OPTVAR_END(ftype, i); i++)
    {
    x = IF_F_OPTVAR(ftype, i);

    if (x == IF_V_FILE_TYPE)
      f++;
    else
      {
      if ((iif_get_var(ifp, IF_V_NAME(x)) != NULL) ||
          (IF_V_DEFAULT(x) != NULL))
        {
        f++;
        }
      }
    }

  opv = (f == i);

  if (obc && opc && obv && opv)
    return IF_F_FULL_DEF;

  if (obc && obv)
    return IF_F_OBL_DEF;

  return IF_F_PART_DEF;
  }



/* END OF FILE 'itoliifl.c' */
