/*
 * FILE NAME         iosl_env.c
 * PURPOSE           Processing IOSL_CFG environment variable
 *
 * SPEC              Esa           '93.11
 * DESIGN            Esa           '93.11
 * DES. VV           Ezb
 * CODING            Esa           '93.11
 * CODE VV           Ezb
 * TEST              Esa           '93.11
 * LAST UPDATED
 *
 * SWITCHES          IOSL_EXT_ENVI
 * NOTES
 *
 */

#ifdef TIPE
#undef TIPE
#endif

/*** DEDUG Switch ***/
#define IOSL_EXT_ENVI   0

/*** Headers ***/
#include "integra.h"
#include "ievl.h"
#include "iosl_env.h"

/*** Own Functions ***/
static char *env_get_from PROT((IN char *, IN char *));
static int   env_atol     PROT((IN char *, OUT long *));

#if IOSL_EXT_ENVI       /* If additional IOSL_CFG fields */
static int   env_atoi     PROT((IN char *, OUT int *));
#endif

/*** String With Parameters for IOSL ***/
static char env_string[ENV_MAX_STRING_LEN + 1];


/***************************************************************************
 * NAME         static char *env_get_from()                                *
 * PURPOSE                                                                 *
 * ARGUMENTS                                                               *
 * RETURNS                                                                 *
 * EXTERNS                                                                 *
 * NOTES                                                                   *
 ***************************************************************************/
static char *env_get_from(env, name)
IN char *env;
IN char *name;
  {
  char *ee;  /* current position in 'env' string  */
  char *nn;  /* current position in 'name' string */
  static char buf[ENV_MAX_STRING_LEN + 1]; /* place for extracted string */

  ee = env;
  for (;;)
    {
    /* return NULL when not found */
    if (*ee == '\0')
      return NULL;

    /* compare starting from current position with name */
    for (nn = name; *nn != '\0'; nn++)
      if (toupper(*ee) != toupper(*nn))
        break;
      else
        ee++;

    if (*nn != '\0' || *ee != ENV_DEF)
      /* if not match */
      {
      /* skip to end of 'env' or to ENV_SEP character or to ENV_STR char */
      while (*ee != ENV_SEP && *ee != ENV_STR && *ee != '\0')
        ee++;

      if (*ee == ENV_SEP)
        /* skip ENV_SEP character */
        ee++;

      if (*ee == ENV_STR)
      /* skip string limited by ENV_STR characters */
        {
        /* skip begining ENV_STR character */
        ee++;

        /* skip string between ENV_STR characters */
        while (*ee != ENV_STR && *ee != '\0')
          ee++;

        if (*ee == ENV_STR)
          /* skip ending ENV_STR character */
          ee++;
        }

      }
    else
      /* string match OK */
      {
      char *b;

      /* skip ENV_DEF character */
      ee++;

      if (*ee == ENV_STR)
      /* string between ENV_STR chars */
        {
        /* skip beginning ENV_STR character */
        ee++;

        /* copy characters starting from 'ee' to 'buf' */
        b = buf;

        while (*ee != ENV_STR && *ee != '\0')
          *b++ = *ee++;

        if (*ee == ENV_STR)
          /* skipp ending ENV_STR character */
          ee++;
        }
      else
        {
        /* copy characters starting from 'ee' to 'buf' */
        b = buf;
        while (*ee != ENV_SEP && *ee != '\0')
          *b++ = *ee++;
        }

      /* mark end of string in 'buf' */
      *b = '\0';

      return buf;
      }
    }
  } /* end of 'char *env_get_from()' function */

#if IOSL_EXT_ENVI       /* If additional IOSL_CFG fields */
/***************************************************************************
 * NAME         static int env_atoi()                                      *
 * PURPOSE      This function is for future evoluation of IOSL             *
 * ARGUMENTS                                                               *
 * RETURNS                                                                 *
 * EXTERNS                                                                 *
 * NOTES                                                                   *
 ***************************************************************************/
static int env_atoi(b, val)
IN  char *b;
OUT int *val;
  {
  int  i;
  long l;

  i = env_atol(b, &l);
  if (i != SUCCESS)
    return FAILURE;

  i = (int)l;
  if (((long)i) != l)
    return FAILURE;

  *val = i;
  return SUCCESS;
  } /* end of 'int env_atoi()' function */
#endif
/***************************************************************************
 * NAME         static int env_atol()                                      *
 * PURPOSE      Read long value from Environment Variable                  *
 * ARGUMENTS    IN char*b; OUT long *val                                   *
 * RETURNS      Read value                                                 *
 * EXTERNS      NONE                                                       *
 * NOTES        NONE                                                       *
 ***************************************************************************/
static int env_atol(b, val)
IN  char *b;
OUT long *val;
  {
  long i;
  long j;
  int sign;

  *val = 0;
  while (*b == ' ')
    b++;

  if (*b == '-' || *b == '+')
    {
    sign = *b == '-';
    b++;
    while (*b == ' ')
      b++;
    }
  else
    sign = 0;

  if (*b == '\0')
    return FAILURE;

  i = 0;
  while (*b >= '0' && *b <= '9')
    {
    j = i * 10;
    if (j / 10 != i)
      return FAILURE;
    i = j + (*b++ - '0');
    if (i < 0)
      return FAILURE;
    }

  if (*b != '\0')
    return FAILURE;

  if (sign)
    i = -i;

  *val = i;

  return SUCCESS;
  } /* end of 'int env_atol()' function */

/***************************************************************************
 * NAME         int iosl_env_setstr()                                      *
 * PURPOSE                                                                 *
 * ARGUMENTS                                                               *
 * RETURNS                                                                 *
 * EXTERNS                                                                 *
 * NOTES                                                                   *
 ***************************************************************************/
int iosl_env_setstr(string)
IN char *string;
  {
  if (string == NULL)
    return FAILURE;

  if (strlen(string) > ENV_MAX_STRING_LEN)
    return FAILURE;

  strcpy(env_string, string);

  return SUCCESS;
  } /* end of 'int iosl_env_setstr()' function */

/***************************************************************************
 * NAME         int iosl_env_envstr()                                      *
 * PURPOSE                                                                 *
 * ARGUMENTS                                                               *
 * RETURNS                                                                 *
 * EXTERNS                                                                 *
 * NOTES                                                                   *
 ***************************************************************************/
int iosl_env_envstr(env_name)
INOUT char *env_name;
  {
  char *env;
  char *upper_name;
  char *u;

  /* get space for upper case string */
  upper_name = malloc(strlen(env_name) + 1);
  if (upper_name == NULL)
    return FAILURE;

#ifdef DEBUG
  /*printf("Envname : %s\n", env_name); */
#endif

  u = upper_name;
  while ((*u++ = (char)toupper(*env_name)) != '\0')
    env_name++;

#ifdef DEBUG
  /*printf("Upper_name : %s\n", upper_name); */
#endif

  env = ev_getenv(upper_name);

#ifdef DEBUG
  /*if (env == NULL)
    printf("Environment string is NULL\n");
  else 
    printf("Environment string: %s\n", env); */
#endif

  if((env == NULL) || (strlen(env) > ENV_MAX_STRING_LEN))
    {
    free(upper_name);
    return FAILURE;
    }

  strcpy(env_string, env);

#ifdef DEBUG
  /*printf("Envs: %s\n", env_string); */
#endif

  free(upper_name);

  return SUCCESS;
  } /* end of 'char *iosl_env_envstr()' function */

/***************************************************************************
 * NAME         long iosl_env_getl()                                       *
 * PURPOSE                                                                 *
 * ARGUMENTS                                                               *
 * RETURNS                                                                 *
 * EXTERNS                                                                 *
 * NOTES                                                                   *
 ***************************************************************************/
long iosl_env_getl(name, def_val)
IN char *name;
IN long  def_val;
  {
  long i;
  char *v;

  if (env_string == NULL)
    return def_val;

  v = env_get_from(env_string, name);

  if (v == NULL || env_atol(v, &i))
    i = def_val;

  return i;
  } /* end of 'int iosl_env_getl()' function */

/***************************************************************************
 * NAME         char *iosl_env_gets()                                      *
 * PURPOSE                                                                 *
 * ARGUMENTS                                                               *
 * RETURNS                                                                 *
 * EXTERNS                                                                 *
 * NOTES                                                                   *
 ***************************************************************************/
char *iosl_env_gets(name, def_val)
IN char *name;
IN char *def_val;
  {
  char *v;

  if (env_string == NULL)
    return def_val;

  v = env_get_from(env_string, name);

  if (v == NULL)
    return def_val;
  else
    return v;
  } /* end of 'char *iosl_env_gets()' function */

#if IOSL_EXT_ENVI       /* If additional IOSL_CFG fields */
/***************************************************************************
 * NAME         int iosl_env_geti()                                        *
 * PURPOSE                                                                 *
 * ARGUMENTS                                                               *
 * RETURNS                                                                 *
 * EXTERNS                                                                 *
 * NOTES                                                                   *
 ***************************************************************************/
int iosl_env_geti(name, def_val)
IN char *name;
IN int  def_val;
  {
  int i;
  char *v;

  if (env_string == NULL)
    return def_val;

  v = env_get_from(env_string, name);

  if (v == NULL || env_atoi(v, &i))
    i = def_val;

  return i;
  } /* end of 'int iosl_env_geti()' function */

/***************************************************************************
 * NAME         double iosl_env_getd()                                     *
 * PURPOSE                                                                 *
 * ARGUMENTS                                                               *
 * RETURNS                                                                 *
 * EXTERNS                                                                 *
 * NOTES                                                                   *
 ***************************************************************************/
double iosl_env_getd(name, def_val)
IN char *name;
IN double def_val;
  {
  double ret;
  char *v;

  if (env_string == NULL)
    return def_val;

  v = env_get_from(env_string, name);

  if (v == NULL)
    ret = def_val;
  else
    sscanf(v, "%ld", &ret);

  return ret;
  } /* end of 'int iosl_env_getd()' function */
#endif

/* END OF 'iosl_env.c' FILE */
