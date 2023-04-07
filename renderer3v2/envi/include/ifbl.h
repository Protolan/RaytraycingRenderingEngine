/*
 * NAME         "ifbl.h"
 * PURPOSE      Header file for Integra Frame Buffer Library(IFBL)
 *
 * SPEC         Takashi Hyodo                   1988-May-20
 * DESIGN       Takashi Hyodo                   1988-May-20
 * DES. INSP.   Nobuhiro Watanabe               1988-May-20
 * CODING       Takashi Hyodo                   1988-May-24
 * CODE INSP.   Nobuhiro Watanabe               1988-May-20
 * TEST         Takashi Hyodo                   1988-May-31
 *
 * NOTE
 *
 * UPDATED      Takashi Hyodo                   1988-May-17
 *              - Correct declaration of 'fb_get_icon_cursor'
 *              Takashi Hyodo                   1988-DEC-22
 *              - Add NO_PROTOTYPE declarations
 *              Takashi Hyodo                   1990.04.24
 *              - Add fb_close declaration
 *              NRK                             1990.05.06
 *              - Change FB_STORAGE definition
 *              - Add fb_copy_image declaration
 *              - Add fb_line_cursor declaration
 *              - Change parameters of fb_get_new_storage
 *              Ujs                             1993.05.14
 *              - Add write mode and cursor constants
 *              Evv
 *              - Stereo support added.
 *              Onc                             Tue  95.02.28
 *              - Dithering support added.
 *              Evv                             Fri  95.05.05
 *              - Stereo/Dithering integrated with OPEN_GL
 *                update done by Erk.
 *              _vr                                  95.08.29
 *              - Added query features.
 *              Romuald Kujawski (Erk)      ELFW_100.SPE 96.01.17
 *              - Added multi-windowing API extension.
 * UPDATED      Eed                                  00.02.04
 *                FB_WIN_EXTERNAL attribute processing
 * UPDATED     Mike Morozov, Vladimir Gumennikov     00.07.05
 *                   FB_WIN_RAM attribute processing
 *
 * Copyright (C) INTEGRA Inc. 1989. All rights reserved.
 */

/*
 * range of variables assumed:
 *      char            0~255 (8bits)
 *      unsigned short  0~65535 (16bits)
 */

#ifndef _IFBL_H_
#define _IFBL_H_

typedef struct
  {
  int   hot_spot_x;
  int   hot_spot_y;
  struct
    {
    int r;
    int g;
    int b;
    } form_color;
  struct
    {
    int r;
    int g;
    int b;
    } mask_color;
  unsigned short        *form_data;     /* pointer to icon form(32byte) */
  unsigned short        *mask_data;     /* pointer to mask data(32bytes) */
  } FB_ICON;


/* handler for storage allocation */
/* - concept from IPDL.H          */

typedef struct
  {
  int x;
  int y;
  int width;
  int height;
  union
    {
    int   i_value;
    long        l_value;
    float       f_value;
    double      d_value;
    VPTR        handle;
    } data;                     /* for IFBL's internal use */
  } FB_STORAGE;

typedef VPTR FB_WIN;                 /* window ID type */

#define FB_MODE_REPLACE 0
#define FB_MODE_OR      1
#define FB_MODE_AND     2
#define FB_MODE_XOR     3

#define FB_CURSOR_NONE  0
#define FB_CURSOR_CROSS 1      /* obsolete */
#define FB_CURSOR_FRAME 2      /* obsolete */
#define FB_CURSOR_HAIR  3      /* obsolete */
#define FB_CURSOR_DHAIR 4      /* obsolete */
#define FB_CURSOR_ICON  5
#define FB_CURSOR_LINE  6      /* obsolete */

#define FB_STEREO_OFF    0
#define FB_STEREO_LEFT   1
#define FB_STEREO_RIGHT  2
#define FB_STEREO_BOTH   3

#define FB_IS_STEREO               1
#define FB_IS_STEREO_ENABLED       2
#define FB_IS_DITHER               3
#define FB_IS_DOUBLE_BUFFER        4
#define FB_IS_Z_BUFFER             5
#define FB_IS_ACCUMULATION_BUFFER  6
#define FB_IS_MULTISAMPLING        7
#define FB_IS_STENCIL              8
#define FB_IS_ACC_BUFFER           9
#define FB_IS_AUX_BUFFER          10

#define FB_BITS_PER_R           1
#define FB_BITS_PER_G           2
#define FB_BITS_PER_B           3
#define FB_X_MAX_RESOLUTION     4
#define FB_Y_MAX_RESOLUTION     5

#define FB_DITHER_FAST          0
#define FB_DITHER_FINE          1

#define FB_BLEND_NONE           0
#define FB_BLEND_MODULATE       1
#define FB_BLEND_ADD            2

/* 
 * Window attributes (types, buffers, geometry, decoration)
 * used in fb_win_open().
 */

/* window types: */
#define FB_WIN_OVERLAY       0x0001 /* in overlay planes */

/* additional buffers: */
#define FB_WIN_DOUBLE_BUFFER 0x0002 /* double buffered window */
#define FB_WIN_Z_BUFFER      0x0004 /* with depth buffer (Z-buffer) */
#define FB_WIN_STEREO        0x0008 /* with stereo buffers */
#define FB_WIN_STENCIL       0x0010 /* with stencil buffer */
#define FB_WIN_ACC_BUFFER    0x0020 /* with accumulation buffer */
#define FB_WIN_AUX_BUFFER    0x0040 /* with auxiliary buffer */

/* geometry and decoration: */
#define FB_WIN_FIX_RATIO     0x0080 /* with fixed width to height ratio */
#define FB_WIN_FIX_SIZE      0x0100 /* with fixed size */
#define FB_WIN_NO_FRAME      0x0200 /* without frame */
#define FB_WIN_SAVE_UNDER    0x0400 /* preserve screen under the window */

#define FB_WIN_EXTERNAL      0x0800 /* do not create window - use existent */
#define FB_WIN_RAM           0x1000 /* window in memory - not visible */

/* Display flush mode in fb_fush() */
#define FB_WAIT     0        /* wait for buffer flush completion */
#define FB_NO_WAIT  1        /* don't wait for buffer flush completion */

#define FB_WIN_NONE  ((FB_WIN)NULL)  /* invalid window ID */


/* General functions */
extern char *ifbl_version PROT((VOID));
extern VOID  fb_config PROT((IN char *config));
extern int   fb_init PROT((VOID));
extern int   fb_term PROT((VOID));
extern VOID  fb_get_resolution PROT((int *, int *));
extern VOID  fb_get_work_area PROT((int *, int *, int *, int *));
extern VOID  fb_flush PROT((IN int wait));
extern int   fb_autoflush PROT((IN int sw));

/* Drawing control functions */
extern VOID  fb_set_color PROT((int, int, int));
extern VOID  fb_get_color PROT((int *, int *, int *));
extern int   fb_write_mode PROT((int));

/* Primitive drawing functions */
extern VOID  fb_put_pix PROT((int, int));
extern VOID  fb_get_pix PROT((int, int, int *, int *, int *));
extern VOID  fb_move_cp PROT((int, int));
extern VOID  fb_get_cp PROT((int *, int *));
extern VOID  fb_draw_cp PROT((int, int));
extern VOID  fb_line PROT((int, int, int, int));
extern VOID  fb_antialiased_line PROT((int, int, int, int));
extern VOID  fb_rect PROT((int, int, int, int));
extern VOID  fb_fill_rect PROT((int, int, int, int));

/* Area manipulation functions */
extern VOID  fb_read_area PROT((int, int, int, int,
               unsigned char *, unsigned char *, unsigned char *));
extern VOID  fb_write_area PROT((int, int, int, int,
               unsigned char *, unsigned char *, unsigned char *));
extern VOID  fb_copy_area PROT((int, int, int, int, int, int));

/* Blending control (for fb_image_restore() only) */
VOID fb_set_blending PROT((IN int mode));
int  fb_get_blending PROT((VOID));

/* Image manipulation functions */
BYTE *fb_image_store PROT((IN int x, IN int y, IN int w, IN int h));
VOID fb_image_restore PROT((IN int x, IN int y, IN int w, IN int h, IN BYTE *image));
VOID fb_image_release PROT((INOUT BYTE *image));

/* Image storage manipulation functions */
extern FB_STORAGE *fb_get_new_storage PROT((int, int, int, int));
extern int   fb_free_storage PROT((FB_STORAGE *));
extern int   fb_store_image PROT((FB_STORAGE *));
extern int   fb_restore_image PROT((FB_STORAGE *));
extern int   fb_copy_image PROT((FB_STORAGE *, int, int));
extern int   fb_swap_image PROT((FB_STORAGE *));

/* Cursor control functions */
extern int   fb_init_cursor PROT((VOID));
extern int   fb_get_cursor_type PROT((VOID));
extern int   fb_set_icon_cursor PROT((FB_ICON *));
extern VOID  fb_get_icon_cursor PROT((FB_ICON *));

/* Cursor drawing functions */
extern VOID  fb_cross_cursor PROT((int, int));           /* obsolete! */
extern VOID  fb_frame_cursor PROT((int, int, int, int)); /* obsolete! */
extern VOID  fb_line_cursor PROT((int, int, int, int));  /* obsolete! */
extern VOID  fb_hair_cursor PROT((int, int));            /* obsolete! */
extern VOID  fb_dh_cursor PROT((int, int, int, int));    /* obsolete! */
extern VOID  fb_icon_cursor PROT((int, int));
extern int   fb_erase_cursor PROT((VOID));
extern VOID  fb_win_set_cursor PROT((IN FB_WIN window, IN FB_ICON *data));
extern VOID  fb_win_get_cursor PROT((IN FB_WIN window, IN FB_ICON *data));

/* Icon drawing functions */
extern int   fb_set_icon PROT((FB_ICON *));
extern VOID  fb_draw_icon PROT((int, int));

/* Stereo control functions */
extern int   fb_set_stereo_view PROT((IN int));
extern int   fb_get_stereo_view PROT((VOID));

/* Query features functions */
extern int   fb_query_feature_bool PROT((IN int));
extern int   fb_query_feature_int PROT((IN int));

/* Dithering control functions */
extern int   fb_dither PROT((IN int));
extern int   fb_set_dither_type PROT((IN int));

/* Multi-windowing control */
extern FB_WIN fb_win_open PROT((IN FB_WIN parent, IN int attributes,
                                IN int x, IN int y, IN int w, IN int h,
                                IN char *title));
extern VOID   fb_win_close PROT((IN FB_WIN window));
extern FB_WIN fb_win_set_current PROT((IN FB_WIN window));
extern FB_WIN fb_win_get_current PROT((VOID));
extern VOID   fb_win_raise PROT((IN FB_WIN window));
extern VOID   fb_win_get_attrs PROT((IN FB_WIN window, OUT int *attrs));
extern VOID   fb_win_set_geometry PROT((IN FB_WIN window,
                                 IN int x, IN int y, IN int w, IN int h));
extern VOID   fb_win_get_geometry PROT((IN FB_WIN window,
                           OUT int *x, OUT int *y, OUT int *w, OUT int *h));
extern VOID   fb_win_query_frame PROT((OUT int *left, OUT int *top,
                                     OUT int *right, OUT int *bottom));
extern VOID   fb_win_iconify PROT((IN FB_WIN window));
extern FB_WIN fb_win_obtain_pos PROT((OUT int *x, OUT int *y));


/* Double-buffer control functions */
extern VOID  fb_win_swap_buffers PROT((IN FB_WIN window));
extern VOID  fb_win_front PROT((IN FB_WIN window));
extern VOID  fb_win_back PROT((IN FB_WIN window));

#endif

/* End of file 'ifbl.h' */
