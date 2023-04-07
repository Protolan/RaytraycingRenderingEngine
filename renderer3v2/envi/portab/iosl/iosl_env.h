/*
 * FILE NAME         iosl_env.h
 * PURPOSE           Include file for iosl_env.c
 *
 * SPEC              Esa           '93.11
 * DESIGN            Esa           '93.11
 * DES. VV           Ezb
 * CODING            Esa           '93.11
 * CODE VV           Ezb
 * TEST              Esa           '93.11
 * UPDATED           Esa           '94.04.27
 *      (Added IRIX supporting)
 * UPDATED           Esa           '94.07.11
 *      (Added support for rest platforms)
 * UPDATED           Esa           '94.09.12
 *      (Added support for WINDOWS)
 * UPDATED	     Esa	   '95.01.11
 *	(Added support for LINUX)
 * UPDATED           Esa           '95.04.15
 *      (Added support for WINDOWS NT)
 *
 * SWITCHES
 * NOTES
 *
 */

#ifndef _IOSL_ENV_H_
#define _IOSL_ENV_H_

#define ENV_MAX_STRING_LEN      255
#define ENV_DEF                 ':'
#define ENV_SEP                 ','
#define ENV_STR                 '"'


/*
 *                      CONFIGURATION STRING
 *
 * Syntax:
 *        <field> = <key> ':' <value>
 *        <key>   = IDENTIFIER
 *        <value> = STRING | REAL_NUM | INT_NUM
 *
 * Descrition format:
 *
 *   field_name - field purpose
 *      value1  - value description
 *      ...
 *      valuen  - value description
 *
 * Example:
 * IOSL_CFG=MEM:16000
 * (16000 KB)
 */

#define IOSL_ENV             "IOSL_CFG"
#define IOSL_MEM_FIELD       "MEM"

#if (OS_MARK == 'O' || OS_MARK == 'L')
                                                /* For SCO/Linux        */
#define IOSL_MEM_DEFAULT     (16L * 1024L)
#endif

#if (OS_MARK == 'H')
#define IOSL_MEM_DEFAULT     (32L * 1024L)      /* For HP-UX            */
#endif

#if (OS_MARK == 'U' || OS_MARK == 'I')
#define IOSL_MEM_DEFAULT     (0)                /* For IRIX and NT      */
#endif

#if (OS_MARK == 'N')
#define IOSL_MEM_DEFAULT     (32L * 1024L)      /* For SUN              */
#endif

#if (OS_MARK == 'R')
#define IOSL_MEM_DEFAULT     (32L * 1024L)      /* For IBM              */
#endif

#if (OS_MARK == 'T')
#define IOSL_MEM_DEFAULT     (32L * 1024L)      /* For TITAN            */
#endif

extern int    iosl_env_envstr PROT((IN char *));
extern int    iosl_env_setstr PROT((IN char *));
extern char  *iosl_env_gets   PROT((IN char *, IN char *));
extern int    iosl_env_geti   PROT((IN char *, IN int));
extern long   iosl_env_getl   PROT((IN char *, IN long));
extern double iosl_env_getd   PROT((IN char *, IN double));

#endif

/* END OF 'iosl_env.h' FILE */
