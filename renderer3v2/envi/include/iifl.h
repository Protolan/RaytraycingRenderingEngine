/*
 * NAME     IIFL.H
 * PURPOSE  Header file for INTEGRA's image file functions library.
 *          It contains error code definitions and library funcitons
 *          prototypes.
 *          This header should be included if any of this functions
 *          is used.
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

#ifndef _IIFL_H_
#define _IIFL_H_
 /* DEFINITION OF IIF TYPE: */

typedef BYTE IIF;

     /* USER - DEFINED VARIABLES LIMITS AND CONSTANTS */

#define VAR_SIZE_LIMIT     4090 /* a limit for 'value' string length in       */
                                /* 'variable=value' assignment                */
#define NAM_SIZE_LIMIT      255 /* a limit for 'name'  string length in       */
                                /* 'variable=value' assignment                */

#define DATA_BLOCK_LIMIT  65530 /* a limit for 'data_block' length in         */
                                /* 'data_block_name=data_block' assignment    */

#define COMPS_LEN_LIMIT   65530 /* a limit for string that form  component    */
                                /* names separated in commas in call to       */
                                /* iif_init_file function                     */

 /* ERROR AND WARNING CODES DEFINITIONS */

#define IIF_OK                0
#define IIFERR_BAD_DISK       1
#define IIFERR_DISK_FULL      2
#define IIFERR_EOF            3
#define IIFERR_NO_MEM         4
#define IIFERR_BAD_PTR       -1
#define IIFERR_BAD_FILE      10
#define IIFERR_NO_FILE       11
#define IIFERR_OLD_VER       12
#define IIFERR_BAD_FORMAT    13
#define IIFERR_BAD_COMPS     14
#define IIFERR_BAD_PARMS     15
#define IIFERR_NOT_INIT      20
#define IIFERR_FILL_NDEF     21
#define IIFERR_BAD_ORG       22
#define IIFERR_BAD_SIZE      23
#define IIFERR_BAD_MODE      24
#define IIFERR_NOT_A_VAR     30
#define IIFERR_RES_VAR       31
#define IIFERR_VAR_TOO_BIG   32

#define IIFWARN_NO_SWAP      40
#define IIFWARN_OVERWRTN     41
#define IIFWARN_INITIALIZED  42
#define IIFWARN_STRING_DATA  50
#define IIFWARN_BINARY_DATA  51

 /* ALLOWED IMAGE FILE FORMATS */

#define IIF_FORMAT_IIFF       1
#define IIF_FORMAT_TIFF       2
#define IIF_FORMAT_GIF        3


#ifndef  NO_PROTOTYPES
int     iif_put_var(IIF*, char*, char*);
char   *iif_get_var(IIF*, char*);
char   *iif_get_first_var(IIF*);
char   *iif_get_next_var(IIF*);
int     iif_copy_vars(IIF*, IIF*);
int     iif_put_data(IIF*, char*, BYTE*, unsigned int);
BYTE   *iif_get_data(IIF*, char*, unsigned short int*);
IIF    *iif_open(char*, char*);
int     iif_close(IIF*);
int     iif_init_file(IIF*, int, int, char*, char*);
int     iif_def_components(IIF*, int, char*);
int     iif_def_fill(IIF*, VPTR);
int     iif_get_resolution(IIF*, int*, int*);
int     iif_get_components(IIF*, char**, char**);
int     iif_get_size(IIF*, BYTE**, unsigned short*);
int     iif_get_format(IIF*);
int     iif_set_format(int);
int     iif_enable_swap(IIF*);
int     iif_disable_swap(IIF*);
int     iif_get_swap(IIF*, BYTE*);
int     iif_error(IIF*);
int     iif_clrerror(IIF*);
char   *iif_strerror(IIF*);
int     iif_set_buffer(IIF*, int);
int     iif_write_line(IIF*, VPTR, int, int, int);
int     iif_read_line(IIF*, VPTR, int, int, int);
int     iif_fill(IIF*, int, int, int, int);
int     iif_setlocale(char*);
char   *iif_getlocale(VOID);

#else
int     iif_put_var();
char   *iif_get_var();
char   *iif_get_first_var();
char   *iif_get_next_var();
int     iif_copy_vars();
int     iif_put_data();
BYTE   *iif_get_data();
IIF    *iif_open();
int     iif_close();
int     iif_init_file();
int     iif_def_components();
int     iif_def_fill();
int     iif_get_resolution();
int     iif_get_components();
int     iif_get_size();
int     iif_get_format();
int     iif_set_format();
int     iif_enable_swap();
int     iif_disable_swap();
int     iif_get_swap();
int     iif_error();
int     iif_clrerror();
char   *iif_strerror();
int     iif_set_buffer();
int     iif_write_line();
int     iif_read_line();
int     iif_fill();
int     iif_setlocale();
char   *iif_getlocale();

#endif

        /* Fields definitions for iif_setlocale argument string: */

#define OS_MARK_FIELD           "OS_MARK"
#define OS_NAME_FIELD           "OS_NAME"
#define OS_BYTE_ORDER_FIELD     "OS_BYTE_ORDER"
#define OS_FP_FORMAT_FIELD      "OS_FP_FORMAT"


#endif /* of #ifndef _IIFL_H_ */
/*-------------------------- END OF MODULE ----------------------------------*/
