/*
 * NAME     IIFL_OWN.H
 * PURPOSE  Header file for INTEGRA's image file functions library.
 *          It contains constants definitions and interface functions
 *          prototypes.
 *          This header should be included only when creating new iifl
 *          library.
 *
 * SPEC       INTEGRA
 * DESIGN     INTEGRA
 * DES. INSP. INTEGRA
 * CODING     Z. Blaszczyk                       1991.08.01
 * CODE INSP.
 * TEST       Z. Blaszczyk                       1991.08.15
 * UPDATED
 *
 * NOTE	 Do not modify this file without permission of INTEGRA.
 *
 * Copyright (C) INTEGRA Inc. 1991, All Rights Reserved.
 */


#ifndef _IIFL_OWN_H_
#define _IIFL_OWN_H_
/* TYPE DEFINITION OF VLIST STRUCTURE: */

typedef struct vars_list
  {
  char                *name;
  char                *value;
  BYTE                 nam_len;
  unsigned short int   val_len;
  BYTE                 type;
  struct vars_list    *next;
  }VLIST;

/* TYPE DEFINITION OF IIF STRUCTURE: */

typedef struct iifstr
  {
  char    marker[4];
  FILE   *file_handle;
  char   *filename;
  int     error;
  VPTR    buffer;
  long    buffer_size;
  int     first_line;
  int     num_of_lines;
  int     valid_lines;
  BYTE   *changed_lines;
  BOOL    swap;
  BOOL    enable_swap;
  int     xres;
  int     yres;
  unsigned short   cmps_number;
  char   *cmps;
  char   *types;
  BYTE   *lcmps;
  BYTE    format;
  VPTR    fill_val;
  char   *sub_cmps;
  unsigned short   def_cmps_amount;
  BYTE   *def_cmps_sizes;
  char   *def_cmps_types;
  int    *def_cmps_position;
  long   *def_cmps_offsets;
  BYTE    float_ver;
  BYTE    initialized;
  char    attrib;
  BOOL    affected;
  VLIST   *syst_var;
  VLIST   *first_var;
  VLIST   *curr_var;
  long    vars_offset;
  BOOL    new_header_sizes;
  long    lcmp_field_offs;
  long    file_field_offs;
  long    cmnt_field_offs;
  long    swap_field_offs;
  long    image_area_offs;
  long    cmnt_field_len;
  long    iiff_header_size;
  }IIFL;

/* MARKERS FOR USER VARS AREA */

#define STRING_VAR         1    /* a marker for 'string' variable type        */
#define DATA_VAR           2    /* a marker for 'data block' variable type    */
#define END_OF_USER_VARS   3    /* a marker following last user-defined       */

/* MARKERS FOR IMAGE BUFFER LINES CONTROL */

#define LINE_NOT_CHANGED    0
#define LINE_CHANGED        1

 /* variables in IIFL file */
#define IIFVAR_TYPES       "IIF_Types"
#define IIFVAR_FLOAT_VER   "IIF_FloatVer"
#define IIFVAR_NEW_VER     "IIF_NewVer"
#define IIFVAR_SYST_VER    "IIF_SystVer"
#define IIFVAR_NEW_XRES    "IIF_NewXRes"
#define IIFVAR_NEW_YRES    "IIF_NewYRes"

     /* GLOBAL VARIABLES  */

GLOBAL int IIF_errno
#ifdef INITIALIZE
            = IIF_OK
#endif
             ;

GLOBAL BYTE IIF_os_mark
#ifdef INITIALIZE
            = OS_MARK
#endif
             ;
GLOBAL BYTE IIF_os_byte_order
#ifdef INITIALIZE
            = OS_BYTE_ORDER
#endif
             ;
GLOBAL BYTE IIF_os_fp_format
#ifdef INITIALIZE
            = OS_FP_FORMAT
#endif
             ;

GLOBAL char IIF_os_name[20];


      /* FLOATING POINT FORMAT ERROR CODE */
#define  BAD_FP_FORMAT          255  /* bad floating point format; error    */

    /* INTERFACE FUNCTIONS PROTOTYPES (WITHOUT IIF_..) */
#ifndef  NO_PROTOTYPES
           /* var_serv.c */
BOOL    fl_check_registered_names(char*);
VLIST  *find_var_name(VLIST*, char*);

           /* iifl.c */
int     fl_check_iifstr_ptr(IIFL*);
unsigned short fl_comps_amount(char*);
VOID    fl_get_comp_name(char*, char*, int);
int     fl_first_access(IIFL*, int);
VOID    fl_free_VLIST(VLIST*);
long    fl_get_comp_offset(IIFL*, int);

           /* fserv.c */
int     fl_write_header(IIFL*);
int     fl_read_header(IIFL*);
VOID    fl_default_header_sizes(IIFL*);
VOID    fl_recalc_header_sizes(IIFL*, int, int);
int     fl_write_lines(IIFL*, int, int);
int     fl_read_lines(IIFL*, int, int);
int     fl_init_var_access(IIFL*);
int     fl_finish_var_access(IIFL*);
char   *fl_get_os_ver(IIFL*);
BYTE    fl_get_float_ver(IIFL*);
char   *fl_get_types(IIFL*);
BOOL    fl_is_correct_ver(IIFL*);
int     fl_check_file_format(IIFL*);
int     fl_read_sys_vars(IIFL*);

           /* img_buf.c */

#else
           /* var_serv.c */
BOOL    fl_check_registered_names();

           /* iifl.c */
int     fl_check_iifstr_ptr();
BYTE    fl_comps_amount();
VOID    fl_get_comp_name();
int     fl_first_access();
VOID    fl_free_VLIST();
long    fl_get_comp_offset();

           /* fserv.c */
int     fl_write_header();
int     fl_read_header();
VOID    fl_default_header_sizes();
VOID    fl_recalc_header_sizes();
int     fl_write_lines();
int     fl_read_lines();
int     fl_init_var_access();
int     fl_finish_var_access();
char   *fl_get_os_ver();
BYTE    fl_get_float_ver();
char   *fl_get_types();
BOOL    fl_is_correct_ver();
int     fl_check_file_format();
int     fl_read_sys_vars();

           /* img_buf.c */

#endif

#endif /* of #ifndef _IIFL_OWN_H_ */

/*-------------------------- END OF MODULE ----------------------------*/
