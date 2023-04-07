/* FILE NAME    ikbl.h
 * PURPOSE      This file contains all definitions of IKBL library.
 * 
 * SPEC         NRK                                   1989.06
 * UPDATED      NRK                                   1990.05
 * UPDATED      Ujs                                   1996.12
 * 
 * NOTES        None.
 * 
 */

#ifndef _IKBL_H_
#define _IKBL_H_

/* ASCII control codes */

#define K_BS         8       /* backspace */
#define K_TAB        9       /* tab */
#define K_ENTER      13      /* return, enter, accept */
#define K_ESC        27      /* escape */
#define K_DEL        127     /* delete */

/* other control codes */

#define K_LEFT       0x200   /* left arrow */
#define K_RIGHT      0x201   /* right arrow */
#define K_HOME       0x205   /* home - start of field */
#define K_END        0x206   /* end - end of field */


extern char *ikbl_version PROT((VOID));
extern int   kbd_begin PROT((VOID));
extern int   kbd_end PROT((VOID));
extern int   kbd_get PROT((VOID));
extern int   kbd_ready PROT((VOID));
extern VOID  kbd_set_break PROT((int));
extern int   kbd_check_break PROT((VOID));

#endif /* _IKBL_H_ */

/* END OF 'ikbl.h' FILE */

