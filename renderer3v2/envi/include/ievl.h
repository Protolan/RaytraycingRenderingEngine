/* FILE NAME    ievl.h
 * PURPOSE      Integra Environment Variable Library IEVL interface.
 * 
 *              Who                       PRF name     When
 * SPEC         (Ezb) Zbigniew Blaszczyk
 *              (Erk) Romuald Kujawski    ievl_100.spe  '94.12.06
 * DESIGN       (Ezb) Zbigniew Blaszczyk
 *              (Erk) Romuald Kujawski    ievl_100.spe  '94.12.06
 * DES. VV      (tfl) full name           pppppppp.ppp  'yy.mm.dd
 * CODING       (Erk) Romuald Kujawski    ievl_100.spe  '95.12.06
 * CODE VV.     (tfl) full name           pppppppp.ppp  'yy.mm.dd
 * LAST UPDATED (Erk) Romuald Kujawski    ievl_100.spe  '95.12.06
 *
 * COPYRIGHT (c) INTEGRA, Inc., 1995
 * 
 * NOTES        
 * 
 */

#ifndef _IEVL_H_
#define _IEVL_H_

extern char  *ievl_version();
extern void   ev_init();
extern void   ev_term();
extern int    ev_getconfig(IN char *filename);
extern char  *ev_getenv(IN char *env);
extern int    ev_putenv(IN char *env);
extern int    ev_update_buffer(IN char *string);
extern char  *ev_get_buffer();
extern int    ev_set_buffer(IN char *string);
extern char  *ev_gets(IN char *name, IN char *def_val);
extern int    ev_geti(IN char *name, IN int def_val);
extern long   ev_getl(IN char *name, IN long def_val);
extern double ev_getf(IN char *name, IN double def_val);


#endif /* #ifndef _IEVL_H_ */

/* END OF 'ievl.h' FILE */

