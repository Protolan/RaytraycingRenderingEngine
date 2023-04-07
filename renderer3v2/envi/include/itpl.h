/* FILE NAME    itpl.h
 * PURPOSE      Header file for application programs linked with
 *              ITPL library.
 * 
 * CODING       Robert Chyla                         '95.02
 * UPDATED      Robert Chyla                         '95.03.07
 * LAST UPDATED Yuri Tkachov                         '97.05.12
 *              UREE task
 * 
 * UPDATED      Evg Vladimir Golubev   udxx02dn.Evg  '99.11.19
 *   tipe_ctrl_*() family of functions added
 *
 * NOTES        To be edited, when PROT will be corrected in 'integra.h'
 * 
 */

#ifndef _ITPL_H_
#define _ITPL_H_

/* make sure, that NO_TIPE has priority over TIPE */
#ifdef NO_TIPE
#ifdef TIPE
#undef TIPE
#endif
#endif

/* make sure, that it is new TIPE - important for IUDL */
#define NEW_TIPE 1

#define TIPE_VERSION 200 /* this is version of TIPE library (2.00) */

/* This file usually (except ITPL library) is included from 'integra.h' */
/* However, PROT is too far in 'integra.h', so define it for a moment.  */

/* used to mark if UNDEF it on end or not */
#undef  TEMPORARY_PROT

#ifndef PROT

/* PROT is not defined - mark and do it now */
#define TEMPORARY_PROT

/* define PROT as in 'integra.h' */
#ifndef NO_PROTOTYPES
#define PROT(x) x
#else
#define PROT(x) ()
#endif

#endif /* PROT */

/* include original IPDL/IKBL and IOSL prototypes */
#include "ikbl.h"
#include "iosl.h"
#include "ipdl.h"
#include "ietl.h"

/* general purpose functions */

extern char *itpl_version PROT((VOID));
extern VOID  tipe_begin PROT((int *, char * * *));
extern VOID  tipe_end PROT((VOID));

/* redirection functions (cannot be called directly) */
extern int   tipe_PD_QueryValuator PROT((PD *, int));
extern int   tipe_PD_QueryButton PROT((PD *, int));
extern int   tipe_kbd_ready PROT((VOID));
extern int   tipe_kbd_get PROT((VOID));

/* redirection functions added in NEW TIPE */

extern OS_TIME_T tipe_os_time PROT((OS_TIME_T *));
extern VOID      tipe_os_mtime_reset PROT((VOID));
extern long      tipe_os_mtime PROT((VOID));
extern long      tipe_os_mtime_wait PROT((long));

/* redirection functions added in NEWEST TIPE */

extern BOOL tipe_et_event_ready PROT((VOID));
extern int  tipe_et_event_read PROT((OUT ET_EVENT_T *event));
extern BOOL tipe_et_event_test PROT((OUT int *type, OUT ET_EVENT_T *event));

/* these are additional NEW TIPE functions */

extern int       tipe_run_application PROT((char *, char **));

extern VOID      tipe_system      PROT((char *, char *));

extern VOID      tipe_check_start PROT((char *));
extern VOID      tipe_check_str   PROT((char *));
extern VOID      tipe_check_int   PROT((int));
extern VOID      tipe_check_end   PROT((VOID));

void tipe_ctrl_main_win_open_in  (void);
void tipe_ctrl_main_win_open_out (IN FB_WIN niudl_win);

void tipe_ctrl_tandem_win_open_in  (IN FB_WIN niudl_win);
void tipe_ctrl_tandem_win_open_out (IN FB_WIN niudl_win, IN FB_WIN idml_win);

void tipe_ctrl_start_ui (IN FB_WIN niudl_win);

/* for easier checking of single values */
extern VOID      tipe_check_str1  PROT((char *, char *));
extern VOID      tipe_check_int1  PROT((char *, int));

/* this is for internal IUDL use - will be removed somewhere      */

extern VOID      tipe_PD_disable_calling PROT((VOID));
extern VOID      tipe_PD_start PROT((PD *));
extern VOID      tipe_PD_end   PROT((VOID));

#ifdef TIPE

/* functions which where redefined also by old TIPE */

#define kbd_get            tipe_kbd_get
#define kbd_ready          tipe_kbd_ready
#define PD_QueryValuator   tipe_PD_QueryValuator
#define PD_QueryButton     tipe_PD_QueryButton
#define et_event_ready     tipe_et_event_ready
#define et_event_read      tipe_et_event_read
#define et_event_test      tipe_et_event_test

/* NOTE: PD_Open is not redefined currently, but prototype is left */

/* new redefined functions */

#define os_time            tipe_os_time
#define os_mtime_reset     tipe_os_mtime_reset
#define os_mtime           tipe_os_mtime
#define os_mtime_wait      tipe_os_mtime_wait

#else /* TIPE */

/* 
   No TIPE is active, so remove allowed direct calls to TIPE functions
   by defining them as empty.
 */

#define itpl_version() "no_tipe"
#define tipe_begin(a, b)
#define tipe_end()

#define tipe_run_application(a, b) os_run(a, b)

#define tipe_system(a, b)
#define tipe_check_start(a)   
#define tipe_check_str(a)     
#define tipe_check_int(a)     
#define tipe_check_end(a)     
#define tipe_check_str1(a, b) 
#define tipe_check_int1(a, b) 

/* this is for internal IUDL use - will be removed somewhere      */

#define tipe_PD_disable_calling()
#define tipe_PD_start(a)
#define tipe_PD_end()

#endif /* TIPE */

#ifdef TEMPORARY_PROT

/* PROT was defined temporarilly, so undefine it and marker */
/* PROT will be defined again in 'integra.h'                */

#undef PROT
#undef TEMPORARY_PROT

#endif /* TEMPORARY_PROT */

#endif /* _ITPL_H_ */

/* END OF 'itpl.h' FILE */
