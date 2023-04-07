/* -------------------------------------------------------- *
 * FILE NAME    icol_ini.c                                  *
 *                                                          *
 * FUNCTION     col_init()                                  *
 * PURPOSE      Initial function of ICOL, memory allocation *
 *              default values setting                      *
 *                                                          *
 * SPEC         Alex Voloboy (Rav) lcol02dm.Rav  '93.11.24  *
 * DESIGN       Alex Voloboy (Rav) lcol01df.Rav  '93.11.28  *
 * DES. VV      Oek, Andrei Khodulev (Sah)       '93.11.30  *
 * CODING       Alex Voloboy (Rav)               '93.12.07  *
 * UPDATED      Edward Kopylov (Oek)             '96.02.07
 * TESTING      Edward Kopylov (Oek)             '96.02.07
 *                                                          *
 *  ARGUMENTS   None                                        *
 *                                                          *
 *  RETURN      None                                        *
 *                                                          *
 *  GLOBALS     co_wavelength, co_x_curve, co_y_curve,      *
 *              co_z_curve, co_wl_alloc, co_xc_alloc,       *
 *              co_yc_alloc, co_zc_alloc, co_error_code,    *
 *              co_curve_points, co_light_points,           *
 *              co_icol_init, co_wlli_alloc, co_light_wl,   *
 *              co_light, co_li_alloc, co_w_current,        *
 *              co_r_current, co_g_current, co_b_current,   *
 *              co_gamma, co_mc_method, co_gun_balance,     *
 *              co_gun_value                                *
 *                                                          *
 * NOTES        Spec. are in file icol_200.um               *
 *                                                          *
 * -------------------------------------------------------- */


#include "integra.h"
#include "imal.h"
#include "icol.h"
#include "icol_own.h"

#include "algebra.h"

/*****************************************************************/
int col_init()
  {
  int   i, j;     /* tmp variables */
  double white[2];  /* basic white color */
  double red[2];    /* basic red color */
  double green[2];  /* basic green color */
  double blue[2];   /* basic blue color */
  
  if (co_icol_init == NO)
    /* imal start */
    if ((co_mem_class = mem_class_create("ICOL")) == MEM_NOT_CLASS)
      {
      co_error_code = COL_ERR_NOT_ENOUGH_MEMORY;
      return COL_ERR_NOT_ENOUGH_MEMORY;
      }

  co_error_code = COL_OK;

  /* default value of number of XYZ curves points */
  co_curve_points = 81;

  /* allocation memory for wavelength */
  if (co_wl_alloc == YES)
    mem_free((VPTR)co_wavelength);

  co_wavelength = (double *)mem_alloc(co_mem_class,
                  "co_wavelength", co_curve_points * sizeof(double));
  if (co_wavelength == NULL)
    {
    co_error_code = COL_ERR_NOT_ENOUGH_MEMORY;
    return (COL_ERR_NOT_ENOUGH_MEMORY);
    }
  co_wl_alloc = YES;

  /* allocation memory for X-curve */
  if (co_xc_alloc == YES)
    mem_free((VPTR)co_x_curve);
  co_x_curve = (double *)mem_alloc(co_mem_class,
                  "co_x_curve", co_curve_points * sizeof(double));
  if (co_x_curve == NULL)
    {
    co_error_code = COL_ERR_NOT_ENOUGH_MEMORY;
    return (COL_ERR_NOT_ENOUGH_MEMORY);
    }
  co_xc_alloc = YES;

  /* allocation memory for Y-curve */
  if (co_yc_alloc == YES)
    mem_free((VPTR)co_y_curve);
  co_y_curve = (double *)mem_alloc(co_mem_class,
               "co_y_curve", co_curve_points * sizeof(double));
  if (co_y_curve == NULL)
    {
    co_error_code = COL_ERR_NOT_ENOUGH_MEMORY;
    return (COL_ERR_NOT_ENOUGH_MEMORY);
    }
  co_yc_alloc = YES;

  /* allocation memory for Z-curve */
  if (co_zc_alloc == YES)
    mem_free((VPTR)co_z_curve);
  co_z_curve = (double *)mem_alloc(co_mem_class,
               "co_z_curve", co_curve_points * sizeof(double));
  if (co_z_curve == NULL)
    {
    co_error_code = COL_ERR_NOT_ENOUGH_MEMORY;
    return (COL_ERR_NOT_ENOUGH_MEMORY);
    }
  co_zc_alloc = YES;

  co_light_points = 41;   /* default value of number of light points */

  /* allocation memory for light wavelength */
  if (co_wlli_alloc == YES)
    mem_free((VPTR)co_light_wl);
  co_light_wl = (double *)mem_alloc(co_mem_class,
                "co_light_wl", co_light_points * sizeof(double));
  if (co_light_wl == NULL)
    {
    co_error_code = COL_ERR_NOT_ENOUGH_MEMORY;
    return (COL_ERR_NOT_ENOUGH_MEMORY);
    }
  co_wlli_alloc = YES;

  /* allocation memory for light spectrogram */
  if (co_li_alloc == YES)
    mem_free((VPTR)co_light);
  co_light = (double *)mem_alloc(co_mem_class,
             "co_light", co_light_points * sizeof(double));
  if (co_light == NULL)
    {
    co_error_code = COL_ERR_NOT_ENOUGH_MEMORY;
    return (COL_ERR_NOT_ENOUGH_MEMORY);
    }
  co_li_alloc = YES;

  /* fill wavelength array with values from 380 to 780 by 5 nm */
  for (j = 0, i = 380; i <= 780; j++, i += 5)
    co_wavelength[j] = (double)i;

  /* fill light wavelength array with values from 380 to 780 by 10 nm */
  for (j = 0, i = 380; i <= 780; j++, i += 10)
    co_light_wl[j] = (double)i;

  /* fill X-curve array */

  co_x_curve[0] = 0.001368;
  co_x_curve[1] = 0.002236;
  co_x_curve[2] = 0.004243;
  co_x_curve[3] = 0.00765;
  co_x_curve[4] = 0.01431;
  co_x_curve[5] = 0.02319;
  co_x_curve[6] = 0.04351;
  co_x_curve[7] = 0.07763;
  co_x_curve[8] = 0.13438;
  co_x_curve[9] = 0.21477;
  co_x_curve[10] = 0.2839;
  co_x_curve[11] = 0.3285;
  co_x_curve[12] = 0.34828;
  co_x_curve[13] = 0.34806;
  co_x_curve[14] = 0.3362;
  co_x_curve[15] = 0.3187;
  co_x_curve[16] = 0.2908;
  co_x_curve[17] = 0.2511;
  co_x_curve[18] = 0.19536;
  co_x_curve[19] = 0.1421;
  co_x_curve[20] = 0.09564;
  co_x_curve[21] = 0.05795001;
  co_x_curve[22] = 0.03201;
  co_x_curve[23] = 0.0147;
  co_x_curve[24] = 0.0049;
  co_x_curve[25] = 0.0024;
  co_x_curve[26] = 0.0093;
  co_x_curve[27] = 0.0291;
  co_x_curve[28] = 0.06327;
  co_x_curve[29] = 0.1096;
  co_x_curve[30] = 0.1655;
  co_x_curve[31] = 0.2257499;
  co_x_curve[32] = 0.2904;
  co_x_curve[33] = 0.3597;
  co_x_curve[34] = 0.4334499;
  co_x_curve[35] = 0.5120501;
  co_x_curve[36] = 0.5945;
  co_x_curve[37] = 0.6784;
  co_x_curve[38] = 0.7621;
  co_x_curve[39] = 0.8425;
  co_x_curve[40] = 0.9163;
  co_x_curve[41] = 0.9786;
  co_x_curve[42] = 1.0263;
  co_x_curve[43] = 1.0567;
  co_x_curve[44] = 1.0622;
  co_x_curve[45] = 1.0456;
  co_x_curve[46] = 1.0026;
  co_x_curve[47] = 0.9384;
  co_x_curve[48] = 0.8544499;
  co_x_curve[49] = 0.7514;
  co_x_curve[50] = 0.6424;
  co_x_curve[51] = 0.5419;
  co_x_curve[52] = 0.4479;
  co_x_curve[53] = 0.3608;
  co_x_curve[54] = 0.2835;
  co_x_curve[55] = 0.2187;
  co_x_curve[56] = 0.1649;
  co_x_curve[57] = 0.1212;
  co_x_curve[58] = 0.0874;
  co_x_curve[59] = 0.0636;
  co_x_curve[60] = 0.04677;
  co_x_curve[61] = 0.0329;
  co_x_curve[62] = 0.0227;
  co_x_curve[63] = 0.01584;
  co_x_curve[64] = 0.01135916;
  co_x_curve[65] = 0.008110916;
  co_x_curve[66] = 0.005790346;
  co_x_curve[67] = 0.004106457;
  co_x_curve[68] = 0.002899327;
  co_x_curve[69] = 0.00204919;
  co_x_curve[70] = 0.001439971;
  co_x_curve[71] = 0.000999949;
  co_x_curve[72] = 0.000690079;
  co_x_curve[73] = 0.000476021;
  co_x_curve[74] = 0.000332301;
  co_x_curve[75] = 0.000234826;
  co_x_curve[76] = 0.000166151;
  co_x_curve[77] = 0.000117413;
  co_x_curve[78] = 8.30753E-05;
  co_x_curve[79] = 5.87065E-05;
  co_x_curve[80] = 4.15099E-05;

  /* fill Y-curve array */

  co_y_curve[0] =  0.000039;
  co_y_curve[1] =  0.000064;
  co_y_curve[2] =  0.00012;
  co_y_curve[3] =  0.000217;
  co_y_curve[4] =  0.000396;
  co_y_curve[5] =  0.00064;
  co_y_curve[6] =  0.00121;
  co_y_curve[7] =  0.00218;
  co_y_curve[8] =  0.004;
  co_y_curve[9] =  0.0073;
  co_y_curve[10] = 0.0116;
  co_y_curve[11] = 0.01684;
  co_y_curve[12] = 0.023;
  co_y_curve[13] = 0.0298;      /* CIE !!! */
  co_y_curve[14] = 0.038;
  co_y_curve[15] = 0.048;
  co_y_curve[16] = 0.06;
  co_y_curve[17] = 0.0739;
  co_y_curve[18] = 0.09098;
  co_y_curve[19] = 0.1126;
  co_y_curve[20] = 0.13902;
  co_y_curve[21] = 0.1693;
  co_y_curve[22] = 0.20802;
  co_y_curve[23] = 0.2586;
  co_y_curve[24] = 0.323;
  co_y_curve[25] = 0.4073;
  co_y_curve[26] = 0.503;
  co_y_curve[27] = 0.6082;
  co_y_curve[28] = 0.71;
  co_y_curve[29] = 0.7932;
  co_y_curve[30] = 0.862;
  co_y_curve[31] = 0.9148501;
  co_y_curve[32] = 0.954;
  co_y_curve[33] = 0.9803;
  co_y_curve[34] = 0.9949501;
  co_y_curve[35] = 1.0;
  co_y_curve[36] = 0.995;
  co_y_curve[37] = 0.9786;
  co_y_curve[38] = 0.952;
  co_y_curve[39] = 0.9154;
  co_y_curve[40] = 0.87;
  co_y_curve[41] = 0.8163;
  co_y_curve[42] = 0.757;
  co_y_curve[43] = 0.6949;
  co_y_curve[44] = 0.631;
  co_y_curve[45] = 0.5668;
  co_y_curve[46] = 0.503;
  co_y_curve[47] = 0.4412;
  co_y_curve[48] = 0.381;
  co_y_curve[49] = 0.321;
  co_y_curve[50] = 0.265;
  co_y_curve[51] = 0.217;
  co_y_curve[52] = 0.175;
  co_y_curve[53] = 0.1382;
  co_y_curve[54] = 0.107;
  co_y_curve[55] = 0.0816;
  co_y_curve[56] = 0.061;
  co_y_curve[57] = 0.04458;
  co_y_curve[58] = 0.032;
  co_y_curve[59] = 0.0232;
  co_y_curve[60] = 0.017;
  co_y_curve[61] = 0.01192;
  co_y_curve[62] = 0.00821;
  co_y_curve[63] = 0.005723;
  co_y_curve[64] = 0.004102;
  co_y_curve[65] = 0.002929;
  co_y_curve[66] = 0.002091;
  co_y_curve[67] = 0.001484;
  co_y_curve[68] = 0.001047;
  co_y_curve[69] = 0.00074;
  co_y_curve[70] = 0.00052;
  co_y_curve[71] = 0.0003611;
  co_y_curve[72] = 0.0002492;
  co_y_curve[73] = 0.0001719;
  co_y_curve[74] = 0.00012;
  co_y_curve[75] = 0.0000848;
  co_y_curve[76] = 0.00006;
  co_y_curve[77] = 0.0000424;
  co_y_curve[78] = 0.00003;
  co_y_curve[79] = 0.0000212;
  co_y_curve[80] = 0.00001499;

  /* fill Z-curve array */

  co_z_curve[0] = 0.006450001;
  co_z_curve[1] = 0.01054999;
  co_z_curve[2] = 0.02005001;
  co_z_curve[3] = 0.03621;
  co_z_curve[4] = 0.06785001;
  co_z_curve[5] = 0.1102;
  co_z_curve[6] = 0.2074;
  co_z_curve[7] = 0.3713;
  co_z_curve[8] = 0.6456;
  co_z_curve[9] = 1.0390501;
  co_z_curve[10] = 1.3856;
  co_z_curve[11] = 1.62296;
  co_z_curve[12] = 1.74706;
  co_z_curve[13] = 1.7826;
  co_z_curve[14] = 1.77211;
  co_z_curve[15] = 1.7441;
  co_z_curve[16] = 1.6692;
  co_z_curve[17] = 1.5281;
  co_z_curve[18] = 1.28764;
  co_z_curve[19] = 1.0419;
  co_z_curve[20] = 0.8129501;
  co_z_curve[21] = 0.6162;
  co_z_curve[22] = 0.46518;
  co_z_curve[23] = 0.3533;
  co_z_curve[24] = 0.272;
  co_z_curve[25] = 0.2123;
  co_z_curve[26] = 0.1582;
  co_z_curve[27] = 0.1117;
  co_z_curve[28] = 0.07824999;
  co_z_curve[29] = 0.05725001;
  co_z_curve[30] = 0.04216;
  co_z_curve[31] = 0.02984;
  co_z_curve[32] = 0.0203;
  co_z_curve[33] = 0.0134;
  co_z_curve[34] = 0.008749999;
  co_z_curve[35] = 0.005749999;
  co_z_curve[36] = 0.0039;
  co_z_curve[37] = 0.002749999;
  co_z_curve[38] = 0.0021;
  co_z_curve[39] = 0.0018;
  co_z_curve[40] = 0.001650001;
  co_z_curve[41] = 0.0014;
  co_z_curve[42] = 0.0011;
  co_z_curve[43] = 0.001;
  co_z_curve[44] = 0.0008;
  co_z_curve[45] = 0.0006;
  co_z_curve[46] = 0.00034;
  co_z_curve[47] = 0.00024;
  co_z_curve[48] = 0.00019;
  co_z_curve[49] = 0.0001;
  co_z_curve[50] = 5E-05;      
  co_z_curve[51] = 0.00003;    
  co_z_curve[52] = 0.00002;    
  co_z_curve[53] = 0.00001;

  for (i = 54; i <= 80; i++)
    co_z_curve[i] = 0.0000;
    
  /* fill default light spectrogram as D6500 point */
  
  co_light[0] = 0.500;
  co_light[1] = 0.546;
  co_light[2] = 0.828;
  co_light[3] = 0.915;
  co_light[4] = 0.934;
  co_light[5] = 0.867;
  co_light[6] = 1.049;
  co_light[7] = 1.170;
  co_light[8] = 1.178;
  co_light[9] = 1.149;
  co_light[10] = 1.159;
  co_light[11] = 1.088;
  co_light[12] = 1.094;
  co_light[13] = 1.078;
  co_light[14] = 1.048;
  co_light[15] = 1.077;
  co_light[16] = 1.044;
  co_light[17] = 1.040;
  co_light[18] = 1.000;
  co_light[19] = 0.963;
  co_light[20] = 0.958;
  co_light[21] = 0.887;
  co_light[22] = 0.900;
  co_light[23] = 0.896;
  co_light[24] = 0.877;
  co_light[25] = 0.833;
  co_light[26] = 0.837;
  co_light[27] = 0.800;
  co_light[28] = 0.802;
  co_light[29] = 0.823;
  co_light[30] = 0.783;
  co_light[31] = 0.697;
  co_light[32] = 0.716;
  co_light[33] = 0.743;
  co_light[34] = 0.616;
  co_light[35] = 0.699;
  co_light[36] = 0.751;
  co_light[37] = 0.636;
  co_light[38] = 0.464;
  co_light[39] = 0.668;
  co_light[40] = 0.634;

  /* specify basic colors: white, red, green, blue */
  LOAD_2D(white, ICOL_DEF_WHITE_X, ICOL_DEF_WHITE_Y)
  LOAD_2D(red,   ICOL_DEF_RED_X, ICOL_DEF_RED_Y)
  LOAD_2D(green, ICOL_DEF_GREEN_X, ICOL_DEF_GREEN_Y)
  LOAD_2D(blue,  ICOL_DEF_BLUE_X, ICOL_DEF_BLUE_Y)

  COPY_2D(co_w_current, white)
  COPY_2D(co_r_current, red)
  COPY_2D(co_g_current, green)
  COPY_2D(co_b_current, blue)

  /* create co_xyz_2_rgb and co_rgb_2_xyz matrixes */
  if (co_create_xyz2rgb(co_xyz_2_rgb, co_rgb_2_xyz,
                                  red, green, blue, white) != COL_OK)
    ASSERT(FALSE)   /* defaults must be OK */

  SET_3D(co_gamma, 2.2)       /* default Gamma correction value */
  co_mc_method = COL_GAMMA;   /* default monitor correction method */

  SET_3D(co_gun_value, 1.0)   /* default gun balance value */
  co_gun_balance = NO;        /* gun balance execution attribute */

  /* ICOL is initiated !!! */
  co_icol_init = YES;

  return co_def_light_xyz(xyz_def_light);
  }
/*****************************************************************/
