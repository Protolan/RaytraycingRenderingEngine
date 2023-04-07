/*   HEADER NAME     GLOBALS.H
 *   
 *   PURPOSE         Macros to declare and initialize global 
 *                   variables
 *   
 *   NOTES           This file must NOT be protected against 
 *                   multiple inclusions !
 *   
 */


#ifdef  GLOBAL
#undef  GLOBAL
#endif 
#ifdef  INITIALIZE
#undef  INITIALIZE
#endif
#ifdef  DEFINE_GLOBALS
#define GLOBAL
#define INITIALIZE
#else
#define GLOBAL extern
#endif

/* END OF 'GLOBALS.H' FILE */

