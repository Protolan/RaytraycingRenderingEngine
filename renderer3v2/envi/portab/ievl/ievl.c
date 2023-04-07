/* FILE NAME    ievl.c
 * PURPOSE      OS-independent functions for ENVI variables handling.
 * 
 *              Who                       PRF name     When
 * SPEC         (Ezb) Zbigniew Blaszczyk  ievl_100.spe  '94.11.xx
 *              (on base of Nrc code)
 * DESIGN       (Ezb) Zbigniew Blaszczyk  ievl_100.spe  '94.11.xx
 * DES. VV      (Erk) Romuald Kujawski    ievl_100.um   '94.11.xx
 * CODING       (Nrc) Robert Chula (IEVL protoplast)    '91
 *              (Ezb) Zbigniew Blaszczyk                '94.11.xx
 *              (Erk) Romuald Kujawski                  '94.11.xx
 * CODE VV.     (Erk) Romuald Kujawski    ievl_100.tm   '95.12.06
 * TEST         (Erk) Romuald Kujawski    ievl_100.tm   '95.12.06
 * LAST UPDATED (Erk) Romuald Kujawski                  '96.01.17
 *              - PPIP_400 compliance verified.
 * LAST UPDATED (Esg) Sergey Grimailo                   '96.08.17
 *              LF as last string's symbol is eliminated (if any)
 * LAST UPDATED (Esg) Sergey Grimailo                   '98.02.01
 *              Updates in area of the LEVM task
 *              (MACROs, "simple" variables processing)
 * 
 * SWITCHES     None.
 * NOTES        Don't forget to update 'sccsid' when changed IEVL version.
 * 
 * ievl.c,v
 * Revision 1.1  1996/03/15  08:35:05  ENVI
 * Envi 2.75
 *  ----------------------------------------------------------------------
 *
 */

static char *sccsid = "@(#)IEVL v. 1.01 /repository/envi/REPOSI/ENVI/portab/ievl/ievl.c,v 1.1 1996/03/15 08:35:05 ENVI Exp";

/* COMPANY INCLUDES */
#include "integra.h"
#include "iosl.h"
#include "ievl.h"
#include "imal.h"

/**********************************************************
 *           MACRO DEFINITIONS AND TYPEDEFS
 **********************************************************/

/* special characters */
#define ENV_DEF      ':'       /* separator of field name and value */
#define ENV_SEP      ','       /* field definition separator */
#define ENV_STR      '"'       /* string separator (optional) */
#define ENV_VAR      '='       /* separator of variable and definition */
#define ENV_VAR_STR  "="
#define ENV_COMMENT  '#'       /* comment line marker */
#define ENV_LF       '\n'      /* last string's symbol in file (:Esg) */

/* standard Integra configuration file */
#define INTEGRA_ENV  "INTEGRA"      /* $INTEGRA - Integra directory */
#define INTEGRA_CFG  "integra.env"  /* standard configuration file */

#define SINGLE_CHUNK        30      /* allocation portion (in objects) */
#define MESSAGE_LENGTH_MAX  256     /* error message max length */
#define MEM_CLASS_NAME      "IEVL"  /* name of memory class of IEVL */

/* debugging flags */
#define DBM_TRACE   0x0001     /* trace IEVL parameters & results */
#define DBM_DUMP    0x0002     /* dump settings state */

#define DB_MASK     debug_mask   /* actual mask set via EV DBG_VAR */
#define DBG_VAR     "DEBUG_IEVL" /* debugging control EV */


/* Switch on / off environment processing for macro substitute */
#define MACRO_ENVI_ON  env_proc = TRUE;
#define MACRO_ENVI_OFF env_proc = FALSE;


/**********************************************************
 *                 STATIC VARIABLES
 **********************************************************/

static BOOL env_proc = TRUE;  /* Switch to turn on / off envi processing */

static int  is_initialized = FALSE; /* flag of IEVL initialization */
static int  mem_class_id;           /* memory class id of IEVL */

/* The current variable buffer */
static char   lib_env_buf[OS_ENV_VAR_LENGTH] = ""; /* buffer */
static char  *env_string = NULL;      /* alias to buffer */

/* Configuration variables list */
static char **root           = NULL;  /* the list root */
static int    max_envi_vars  = 0;     /* number of variables */
static int    total_vars     = 0;     /* num. of allocated entries */

/* name of configuration file '.env' */
static char config_filename[MAX_PATHNAME_LENGTH];

/* debugging control */
static unsigned int debug_mask = 0x0;

/**********************************************************
 *             STATIC FUNCTION PROTOTYPES
 **********************************************************/

static int   env_atof           PROT ((IN char *, OUT double *));
static int   env_atol           PROT ((IN char *, OUT long *));
static int   env_atoi           PROT ((IN char *, OUT int *));
static char *env_get_from       PROT ((IN char *, IN char *));
static int   check_field_var    PROT ((IN char*));
static int   get_field_var      PROT ((IN char *, OUT char *));
static char *get_next_field_var PROT ((IN char *));
static char *get_var_from_table PROT ((IN char *));
static char *get_variable_name  PROT ((IN char *));
static int   add_def            PROT ((IN char *t_line, IN int line_nr));
static OKAY  add_var_to_table   PROT ((IN char *));
static int   resize_buffer      PROT ((INOUT char ***, IN int, IN int));
static VOID  free_buffer        PROT ((INOUT char **));
static VOID  error_msg          PROT((IN char *msg, IN int line_nr, 
                                      IN int ch_nr, IN char *line));
static VOID  dump_config        PROT ((VOID));
static VOID  set_debugging      PROT((VOID));

/* Added '98.02.01   :Esg */
static VOID reset_table_ptr      PROT((VOID));
static char *next_var_from_table PROT((VOID));
static BOOL is_simple_var        PROT((char *var_str));
static char *macro_substitute    PROT((char *var_str));
static VOID clear_table          PROT((VOID));


/**********************************************************
 *                GLOBAL FUNCTIONS
 **********************************************************/

/* --------------------------------------------------------------------------
 * NAME      ievl_version
 * PURPOSE   Returns version name of the library.
 * RETURNS   return contents of variable 'sccsid', that contains 
 *           the library version name.
 * EXTERNS   sccsid - version name.
--------------------------------------------------------------------------*/

char *ievl_version()
  {
  return sccsid;
  }


/* --------------------------------------------------------------------------
 * NAME      ev_init
 * PURPOSE   Initializes IEVL services.
 * EXTERNS   is_initialized - flag of IEVL initialization.
 *           mem_class_id   - memory class id for IEVL allocations.
 * NOTES
 *           IEVL cannot be REinitialized - call ev_term() before
 *           calling ev_init() again.
 --------------------------------------------------------------------------*/
VOID ev_init()
  {
  ASSERT(!is_initialized)

  /* explicit termination required */
  if (is_initialized)
    return;

  mem_class_id = mem_class_create(MEM_CLASS_NAME);
  if (mem_class_id == MEM_NOT_CLASS)
    mem_class_id = MEM_CLASS_DEFAULT;
  
  set_debugging();

  clear_table();

  is_initialized = TRUE;
  } /* end of 'ev_init' function */


/* --------------------------------------------------------------------------
 * NAME      ev_term
 * PURPOSE   Terminates IEVL service.
 * RETURNS   SUCCESS if terminated correctly,
 *           FAILURE if something went wrong 
 *                   (anyway final state is always "terminated").
 * EXTERNS   is_initialized - flag of IEVL initialization.
 *           mem_class_id   - memory class id for IEVL allocations.
 --------------------------------------------------------------------------*/
VOID ev_term()
  {
  ASSERT(is_initialized)

  if (!is_initialized)
    return;

  clear_table();

  if (mem_class_id != MEM_CLASS_DEFAULT)
    mem_class_close(mem_class_id);

  is_initialized = FALSE;
  } /* end of 'ev_term' function */


/****
 * LOCAL STATIC VARIABLE
 ****/
static char chn_var[OS_ENV_VAR_LENGTH + 1] = ""; /* For change of var */

/* --------------------------------------------------------------------------
 * NAME         ev_getconfig
 * PURPOSE      get envi strings from a config file(s)
 *              - first, each variable present in $INTEGRA/integra.env
 *                file is stored in the internal table;
 *              - second, for all variables present in the table and
 *                defined in environment replace their values in the table
 *                by values from environment;
 *              - third, each variable present in application configuration
 *                file is stored in the internal table (with possible
 *                overwriting);
 *              - fourth, all variables in internal table are put into 
 *                environment (by os_putenv(POINTER TO THE INTERNAL
 *                TABLE's ELEMENT))
 * ARGUMENTS    env_file - a full name of file  with envi strings;
 * RETURNS      SUCCESS - on success; FAILURE - if something went wrong.
 * EXTERNS      None
 * NOTES
 *              if 'env_file' is NULL, then only "$INTEGRA/integra.env"
 *              is taken into account.
 *
 *              Function reads subsequent lines from configuration 
 *              file and places them into internal library buffer. 
 *              It is assumed that every line consists of separate 
 *              definition for a single environment variable. Pls. 
 *              see  also  notes to "Assumptions, Implementation" 
 ------------------------------------------------------------------------*/
OKAY ev_getconfig(env_file)
IN char *env_file;
  {
  FILE *f;                       /* .env file handle */
  char  line[OS_ENV_VAR_LENGTH]; /* line of text from .env file */
  char *val;                     /* environment variable value */
  int   line_nr;                 /* number of source line */
  char *variable;                /* Pointer to variable string */

  ASSERT(is_initialized)

  /* Clear all previous data */
  clear_table();

  /* Read from "$INTEGRA/integra.env" */
  val = os_getenv(INTEGRA_ENV);
  if (val != NULL)
    {
    MACRO_ENVI_OFF       /* Switch off all envi processing */

    sprintf(config_filename, "%s%c%s", val, PATH_SEPARATOR, INTEGRA_CFG);
    DBM(DBM_TRACE, printf("Try read config from: %s\n", config_filename))

    f = os_fopen(config_filename, "r");
    line[OS_ENV_VAR_LENGTH - 1] = '\0';
    if (f != NULL)
      {
      line_nr = 1;
      while (fgets(line, OS_ENV_VAR_LENGTH - 1, f) != NULL)
	{
	if (add_def(line, line_nr) == FAILURE)
	  {
	  os_fclose(f);
          clear_table();
	  return FAILURE;
	  }
	line_nr++;
	}
      os_fclose(f);
      }

    DBM(DBM_DUMP, printf("Config from: %s\n", config_filename))
    DBM(DBM_DUMP, dump_config())
    }

  /* Scan internal table and replace each variable which is also present
   * in environment by its value from environment
   */
  reset_table_ptr();
  while ((variable = next_var_from_table()) != NULL)
    {   /* Loop on variables */
    char *e_var, *name;

    /* Check if such variable is present in environment */
    e_var = os_getenv((name = get_variable_name(variable)));
    if (e_var != NULL)
      {   /* Such variable is present in environment -> put it into table */
      /* Form variable string */
      strcpy(chn_var, name);          /* Name */
      strcat(chn_var, ENV_VAR_STR);   /* Separator ("=") */
      strcat(chn_var, e_var);         /* New value */
      /* add var (will replace old value) */
       add_var_to_table(chn_var);   
      }
    }   /* End of loop on variables */

  /* Read from application's defined file */
  MACRO_ENVI_ON     /* Switch on all envi processing */
  if (env_file != NULL)
    {
    strcpy(config_filename, env_file);
    DBM(DBM_TRACE, printf("Try read config from: %s\n", config_filename))

    f = os_fopen(env_file, "r");
    if (f != NULL)
      {
      line_nr = 1;
      line[OS_ENV_VAR_LENGTH - 1] = '\0';
      while (fgets(line, OS_ENV_VAR_LENGTH - 1, f) != NULL)
        {
        if (add_def(line, line_nr) == FAILURE)
          {
          os_fclose(f);
          clear_table();
          return FAILURE;
          }
        line_nr++;
        }

      os_fclose(f);
      }
    DBM(DBM_DUMP, printf("Config from: %s\n", config_filename))
    DBM(DBM_DUMP, dump_config())
    }

  /* Put all variables from internal table into environment */ 
  reset_table_ptr();
  while ((variable = next_var_from_table()) != NULL)
    {   /* Loop on variables */
    if (os_putenv(variable) != SUCCESS)
      {
      /* Fail to put the variable into envi */
      clear_table();
      return FAILURE;
      }
    }   /* End of loop on variables */

  return SUCCESS;
  } /* end of 'ev_getconfig' function */


/* --------------------------------------------------------------------------
 * NAME         ev_getenv
 * PURPOSE      get environment variable value
 * ARGUMENTS    variable - a name of the variable;
 * RETURNS      A value of this variable, 
 *              or NULL - if function cannot determine it
 * EXTERNS      None
 * NOTES        Function simply looks for variable in the environment
 *              and returns pointer to its value (if any)
 *
 ------------------------------------------------------------------------*/
char *ev_getenv(variable)
IN char *variable;
  {
  char *val;    /* environment variable value */

  /* check if variable define in environment */
  val = os_getenv(variable);
  if (val != NULL)
    {
    /* store variable value from environment */
    strcpy(lib_env_buf, val);
    env_string = lib_env_buf;

    DBM(DBM_TRACE, printf("ev_getenv: from EV lib_env_buf=%s\n", lib_env_buf))
    }
  else
    env_string = NULL;

  return env_string;
  } /* end of 'ev_getenv' function */


/* --------------------------------------------------------------------------
 * NAME         ev_putenv
 * PURPOSE      add variable to local environment
 * ARGUMENTS    variable - a name of the variable to be added;
 * RETURNS      SUCCESS - if OK;  FAILURE - if something went wrong;
 * EXTERNS      None
   ------------------------------------------------------------------------*/
OKAY ev_putenv(variable)
IN char *variable;
  {
  char var_buff[OS_ENV_VAR_LENGTH + 1]; /* buffer for 'var=string' */
  char *var_ptr;
  char *v;

  ASSERT(is_initialized)

  if (env_string == NULL)
    return FAILURE;

  /* !!! notice that this variable is added to the list,
   *     then its pointer passed to env var.
   */
  v = macro_substitute(variable);  /* Macros substitution */
  if (v == NULL)
    return FAILURE;
  sprintf(var_buff, "%s=%s", v, env_string);
  if (add_var_to_table(var_buff) == FAILURE)
    return FAILURE;

  /* Looking for pointer to variable in the table */
  reset_table_ptr();
  while ((var_ptr = next_var_from_table()) != NULL)
    { 
    if (strcmp(var_ptr, var_buff) == 0)
      break;
    } 

  return os_putenv(var_ptr);

  } /* end of 'ev_putenv' function */


/* --------------------------------------------------------------------------
 * NAME         ev_gets
 * PURPOSE      get string value for given option
 * ARGUMENTS    name     - a name of option field;
 *              def_val  - a default string value  for this option;
 * RETURNS      String value for given option field.
 * EXTERNS      None
   ------------------------------------------------------------------------*/
char *ev_gets(name, def_val)
IN char *name;
IN char *def_val;
  {
  char *v;     /* field value (string) */

  if (env_string == NULL)
    return def_val;

  v = env_get_from(env_string, name);

  if (v == NULL)
    return def_val;
  else
    return v;
  } /* end of 'ev_gets' function */


/* --------------------------------------------------------------------------
 * NAME         ev_geti
 * PURPOSE      get integer for given field
 * ARGUMENTS    name     - a name of  option field;
 *              def_val  - a default  value  for this option;
 * RETURNS      A value for given option field.
 * EXTERNS      None
 * NOTES
 *              Function  tries  to retrieve a  value  string  for 
 *              given  option field, and converts it into  integer 
 *              value - which is returned to the caller.
 *              If  'name'  field is not  defined  (or  conversion 
 *              failed), a default value is returned.
 ------------------------------------------------------------------------*/
int ev_geti(name, def_val)
IN char *name;
IN int   def_val;
  {
  char *v;   /* field value (string) */
  int i;     /* field value converted to integer */

  if (env_string == NULL)
    return def_val;

  v = env_get_from(env_string, name);

  if (v == NULL || env_atoi(v, &i) != 0)
    i = def_val;

  return i;
  } /* end of 'ev_geti' function */


/* --------------------------------------------------------------------------
 * NAME         ev_getl
 * PURPOSE      get long integer value for given option field
 * ARGUMENTS    name     - a name of  option field;
 *              def_val  - a default  value  for this option;
 * RETURNS      Long integer field value.
 * EXTERNS      None
 * NOTES
 *              Function  tries  to retrieve a  value string for 
 *              given  option field, and converts it into long integer 
 *              value - which is returned to the caller.
 *              If  'name'  field is not  defined  (or  conversion 
 *              failed), a default value is returned.
   ------------------------------------------------------------------------*/
long ev_getl(name, def_val)
IN char *name;
IN long  def_val;
  {
  char *v;   /* field value (string) */
  long i;    /* field value converted to long */

  if (env_string == NULL)
    return def_val;

  v = env_get_from(env_string, name);

  if (v == NULL || env_atol(v, &i) != 0)
    i = def_val;

  return i;
  } /* end of 'ev_getl' function */


/* --------------------------------------------------------------------------
 * NAME         ev_getf
 * PURPOSE      get floating-point value for given option field
 * ARGUMENTS    name     - a name of  option field;
 *              def_val  - a default  value  for this option;
 * RETURNS      Floating-point value for given option field.
 * EXTERNS      None
   ------------------------------------------------------------------------*/
double ev_getf(name, def_val)
IN char *name;
IN double def_val;
  {
  char *v;   /* field value (string) */
  double i;  /* field value converted to double */

  if (env_string == NULL)
    return def_val;

  v = env_get_from(env_string, name);

  if (v == NULL || env_atof(v, &i) != 0)
    i = def_val;

  return i;
  } /* end of 'ev_getf' function */


/* --------------------------------------------------------------------------
 * NAME         ev_update_buffer
 * PURPOSE      update local environment buffer for further operations
 * ARGUMENTS    string - a pointer  to  string  with value of environment
 *                       variable; it can be completed (i.e. have a form of
 *                       "Field_1:Val_1,Field_2:Val_2,..") or simple one
 *                       (any text string).
 * RETURNS      SUCCESS - if string is constructed properly and can fit into
 *              internal buffer; FAILURE - otherwise;
 * EXTERNS      None
 * NOTES       
 *              Function updates current configuration string into local
 *              static buffer for further operations in following way:
 *              - if types of variables are different or both variables
 *                are simple then buffer contents is replaced in full;
 *              - if 'string' consists of field that is already defined
 *                in internal buffer, an old value of this field is
 *                overwritten with new one;
 *              - if 'string' consists of field that have not been
 *                defined in local buffer, this field is added to the
 *                contents  of  local buffer altogether  with  its 
 *                value;
 *              - other fields in the buffer (as well as as their values)
 *                remain unchanged.
 *
 *              A total length of the string should not exceed
 *              OS_ENV_VAR_LENGTH characters, including terminating '\0'.
 *
 --------------------------------------------------------------------------*/
OKAY ev_update_buffer(t_string)
IN char *t_string;
  {
  char new_sett[OS_ENV_VAR_LENGTH];  /* buffer for new setting of variable */
  char new_field[OS_ENV_VAR_LENGTH]; /* new field from replacement string */
  char fld_name[OS_ENV_VAR_LENGTH];  /* processed field name */
  char fld_val[OS_ENV_VAR_LENGTH];   /* processed field value */
  int last_ch;                       /* index of last character in string */
  char *v;       /* variable string ptr */
  char *w;       /* replacement string ptr */
  char *p;       /* field value ptr in variable string */
  BOOL tp, tp_1;
  char *string;

  ASSERT(is_initialized)
  ASSERT(strlen(t_string) < OS_ENV_VAR_LENGTH)
  ASSERT(env_string != NULL)

  if (env_string == NULL)
    return FAILURE;

  DBM(DBM_TRACE, printf("ev_update_buffer:\n"))
  DBM(DBM_TRACE, printf("  env_string=%s\n", env_string))
  DBM(DBM_TRACE, printf("  string    =%s\n", t_string))
  
  /* MACROS substitution */
  string = macro_substitute(t_string);
  if (string == NULL)
    return FAILURE;

  if (strlen(string) > OS_ENV_VAR_LENGTH - 1)
    return FAILURE;

  /* Compare variables' types */
  tp = is_simple_var(string);
  tp_1 = is_simple_var(env_string);
  if ((tp != tp_1) ||
      ((tp == TRUE) && (tp_1 == TRUE)))
    {   /* Types are different or both simple --> replace fully */
    strcpy(lib_env_buf, string);
    return SUCCESS;
    }

  v = env_string;
  w = string; 
  
  /* update all fields that occure in both source & dest buffer */ 
  new_sett[0] = '\0'; 
  do 
    { 
    /* get next field in replacement and its value from actual variable */ 
    if (get_field_var(v, fld_name) == SUCCESS &&
        (p = env_get_from(env_string, fld_name)) != NULL)
      {
      /* store original value of the field */
      strcpy(fld_val, p);

      /* find this field in replacement string */
      p = env_get_from(w, fld_name);
      if (p != NULL)
        sprintf(new_field, "%s%c%s%c", fld_name, ENV_DEF, p, ENV_SEP);
      else
        sprintf(new_field, "%s%c%s%c", fld_name, ENV_DEF, fld_val, ENV_SEP);

      /* store field setting */
      strcat(new_sett, new_field);
      ASSERT(strlen(new_sett) < OS_ENV_VAR_LENGTH)
      }
    } 
  while ((v = get_next_field_var(v)) != NULL);


  /* append non-existing fields */
  do
    {
    /* get next field from replacement string and 
     * make sure that it is not defined in actual variable 
     */ 
    if (get_field_var(w, fld_name) == SUCCESS &&
        (env_get_from(env_string, fld_name)) == NULL)
      {
      /* get field value from replacement string */
      p = env_get_from(w, fld_name);
      if (p != NULL)
        {
	/* add new field */
        sprintf(new_field, "%s%c%s%c", fld_name, ENV_DEF, p, ENV_SEP);
        strcat(new_sett, new_field);

        ASSERT(strlen(new_sett) < OS_ENV_VAR_LENGTH)
        }
      }
    } while ((w = get_next_field_var(w)) != NULL);

  /* remove ENV_SEP character after the last field definition */
  last_ch = (int)strlen(new_sett) - 1;
  if (new_sett[last_ch] == ENV_SEP)
    new_sett[last_ch] = '\0';

  strcpy(lib_env_buf, new_sett);

  ASSERT(strlen(lib_env_buf) < OS_ENV_VAR_LENGTH)

  DBM(DBM_TRACE, printf("  result    =%s\n", lib_env_buf))

  return SUCCESS;
  } /* end of 'ev_update_buffer' function */


/* --------------------------------------------------------------------------
 * NAME         ev_get_buffer
 * PURPOSE      get contents of local environment buffer
 * ARGUMENTS    None
 * RETURNS      A pointer to library internal local buffer with configuration
 *              string that is currently in use;
 * EXTERNS      None
 * NOTES
 *              It is recommended not to directly manipulate  with
 *              contents of this buffer; instead, user should copy
 *              its  contents  into his own  buffer,  make  proper
 *              changes  and  set new  configuration  string  with
 *              ev_set_buffer() function.
 *
 *              A  total  length of the string should  not  exceed
 *              OS_ENV_VAR_LENGTH characters, including terminating
 *              '\0'.
 --------------------------------------------------------------------------*/
char *ev_get_buffer()
  {
  return lib_env_buf;
  } /* end of 'ev_get_buffer' function */


/* --------------------------------------------------------------------------
 * NAME         ev_set_buffer
 * PURPOSE      set local environment buffer for further operations
 * ARGUMENTS    str -  a pointer to a configuration;
 *                     it can be 'completed' (i.e. like to 
 *                     "Field_1:Val_1,Field_2:Val_2,..") or simple
 *                     one (any text string).
 * RETURNS      SUCCESS  - if string is constructed  properly  and
 *              can fit into internal buffer;
 *              FAILURE - if 'str' is NULL or is badly constructed;
 * EXTERNS      None
   ------------------------------------------------------------------------*/
OKAY ev_set_buffer(t_str)
IN char *t_str;
  {
  char *str;

  ASSERT(is_initialized)
  ASSERT(t_str != NULL)
  ASSERT(strlen(t_str) < OS_ENV_VAR_LENGTH)

  if (t_str == NULL)
    return FAILURE;

  /* MACROs substitution */
  str = macro_substitute(t_str);
  if (str == NULL)
    return FAILURE;

  /* Variable string's length is to be checked after MACROs substitution */
  if (strlen(str) + 1 > OS_ENV_VAR_LENGTH)
    return FAILURE;

  strcpy(lib_env_buf, str);
  env_string = lib_env_buf;

  DBM(DBM_TRACE, printf("ev_set_buffer(%s)=%s\n", str, lib_env_buf))

  return SUCCESS;
  } /* end of 'ev_set_buffer' function */


/**********************************************************
 *                STATIC FUNCTIONS
 **********************************************************/

/* --------------------------------------------------------------------------
 * NAME         env_atol
 * PURPOSE      convert ASCII string to long value
 * ARGUMENTS    b   - a pointer  to ASCII string
 *              val - a pointer to long variable where coversion result
 *                    should be stored
 * RETURNS      0 - on success, non-zero - otherwise
 *              (1 - empty or none-number field, 2 - overflow)
 * EXTERNS      None
   ------------------------------------------------------------------------*/
static int env_atol(b, val)
IN  char *b;
OUT long *val;
  {
  long i, j;
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
    return 1;

  i = 0;
  while (*b >= '0' && *b <= '9')
    {
    j = i * 10;
    if (j / 10 != i)
      return 2;
    i = j + (*b++ - '0');
    if (i < 0)
      return 2;
    }

  if (*b != '\0')
    return 1;

  if (sign)
    i = -i;

  *val = i;

  return 0;
  } /* end of 'int env_atol()' function */

/* --------------------------------------------------------------------------
 * NAME         env_atof
 * PURPOSE      convert ASCII string to float value
 * ARGUMENTS    b   - a pointer  to ASCII string
 *              val - a pointer to float variable where coversion result
 *                    should be stored
 * RETURNS      0 - on success, non-zero - otherwise
 * EXTERNS      None
   ------------------------------------------------------------------------*/
static int env_atof(b, val)
IN  char *b;
OUT double *val;
  {
  double d;

  *val = 0;

  if (sscanf(b, "%lf", &d) != 1)    /* uses indirect call to strtod */
    return 1;

  *val = d;

  return 0;
  }

/* --------------------------------------------------------------------------
 * NAME         env_atoi
 * PURPOSE      convert ASCII string to int value
 * ARGUMENTS    b   - a pointer  to ASCII string
 *              val - a pointer to int variable where coversion result
 *                    should be stored
 * RETURNS      0 - on success, non-zero - otherwise
 *              (2 - overflow)
 * EXTERNS      None
   ------------------------------------------------------------------------*/
static int env_atoi(b, val)
IN  char *b;
OUT int *val;
  {
  int  i;
  long l;

  i = env_atol(b, &l);
  if (i != 0)
    return i;

  i = (int)l;
  if (((long)i) != l)
    return 2;

  *val = i;
  return 0;
  } /* end of 'int env_atoi()' function */


/* --------------------------------------------------------------------------
 * NAME         env_get_from
 * PURPOSE      get 'value' of 'name' field from 'env' string
 * ARGUMENTS    env  - a pointer to environment string
 *              name - an field name
 * RETURNS      A pointer to 'value' string, or NULL - otherwise
 * EXTERNS      None
   ------------------------------------------------------------------------*/
static char *env_get_from(env, name)
IN char *env;
IN char *name;
  {
  char *ee;  /* current position in 'env' string  */
  char *nn;  /* current position in 'name' string */
  static char buf[OS_ENV_VAR_LENGTH + 1]; /* place for extracted string */

  ASSERT(env != NULL)
  ASSERT(name != NULL)

  DBM(DBM_TRACE, printf("GETTING \"%s\" FROM \"%s\"\n", name, env))

  ee = env;
  for (;;)
    {
    /* return NULL when config empty */
    if (*ee == '\0')
      return NULL;

    /* find field name in config */ 
    for (nn = name; *nn != '\0' && (toupper(*ee) == toupper(*nn)); nn++)
      ee++;

    /* if not match */
    if (*nn != '\0' || *ee != ENV_DEF)
      {
      /* skip to end of var/field, or begin of string value */
      while (*ee != ENV_SEP && *ee != ENV_STR && *ee != '\0')
        ee++;

      /* skip ENV_SEP character */
      if (*ee == ENV_SEP)
        ee++;
      else if (*ee == ENV_STR)
      /* skip field value of string type (limited by ENV_STR) */
	{
	/* skip begining ENV_STR character */
	ee++;

	/* skip string between ENV_STR characters */
	while (*ee != ENV_STR && *ee != '\0')
	  ee++;

	/* skip ending ENV_STR character */
	if (*ee == ENV_STR)
	  ee++;
	}
      }
    else
      /* string match OK */
      {
      char *b;

      /* skip ENV_DEF character */
      ASSERT(*ee == ENV_DEF)
      ee++;

      /* if string between ENV_STR chars */
      if (*ee == ENV_STR)
	{
	/* skip beginning ENV_STR character */
	ee++;

        /* copy till ending ENV_STR */
        b = buf;
        while (*ee != ENV_STR && *ee != '\0')
          *b++ = *ee++;

        /* skipp ending ENV_STR character */
	if (*ee == ENV_STR)
	  ee++;
	}
      else
	{
        /* copy value till end of field/var */
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


/* --------------------------------------------------------------------------
 * NAME         check_field_var
 * PURPOSE      checks whether variable field is properly constructed
 * ARGUMENTS    str -  a pointer to a variable field definition like:
 *                     "Field_1:Val_1
 * RETURNS      SUCCESS  - if string is constructed  properly
 *              FAILURE - if 'str' is badly constructed;
 * EXTERNS      None
   ------------------------------------------------------------------------*/
static OKAY check_field_var(var)
IN char *var;
  {
  char *v;

  ASSERT(var != NULL)

  v = var;

  /* field name must be IDENT (like in C) */
  if (*v == '\0' || !isalpha(*v))
    return FAILURE;

  v++;
  while (*v != '\0' && *v != ENV_DEF)
    {
    if (!(isalnum(*v) || *v == '_'))
      return FAILURE;
    v++;
    }

  /* value separator expected */
  if (*v != ENV_DEF)
    return FAILURE;

  v++;

  /* value can be any text, even empty,
   * but string in quotation marks require special treatment
   */
  while (*v != '\0' && *v != ENV_SEP)
    {
    if (*v == ENV_STR)
      {
      /* skip all between ENV_STR marks */
      do
        {
        v++;
        if (*v == '\0')
          return FAILURE;
        } while (*v != ENV_STR);
      }
    v++;
    }

  return SUCCESS;
  } /* end of 'check_field_var' function */


/* --------------------------------------------------------------------------
 * NAME         get_field_name
 * PURPOSE      gets field name from the string
 * ARGUMENTS    str -  a pointer to a string definition like:
 *                     "Field_1:Val_1
 *              name  - a pointer to buffer for field name
 * RETURNS      SUCCESS  - if string is constructed  properly
 *              FAILURE - if 'str' is badly constructed;
 * EXTERNS      None
   ------------------------------------------------------------------------*/
static OKAY get_field_var(str, name)
IN char *str;
OUT char *name;
  {
  char *v, *vv;

  v = str;

  /* field name must be IDENT (like in C) */
  if (*v == '\0' || !isalpha(*v))
    return FAILURE;

  v++;
  while (*v != '\0' && *v != ENV_DEF)
    {
    if (!(isalnum(*v) || *v == '_'))
      return FAILURE;
    v++;
    }

  /* value separator expected */
  if (*v != ENV_DEF)
    return FAILURE;

  /* copy field name */
  for (vv = str; vv != v; vv++)
    *name++ = *vv;

  *name = '\0';

  return SUCCESS;
  } /* end of 'get_field_var' function */


/* --------------------------------------------------------------------------
 * NAME         get_next_field_var
 * PURPOSE      get next variable field
 * ARGUMENTS    v -  a pointer to configuration string 
 *                   like: "Field_1:Val_1,Field_2:Val_2,.."
 * RETURNS      A pointer to beginning of this field, or NULL;
 * EXTERNS      None
   ------------------------------------------------------------------------*/
static char *get_next_field_var(v)
IN char *v;
  {
  ASSERT(v != NULL)

  while (*v != '\0' && *v != ENV_SEP)
    {
    if (*v == ENV_STR)
      {
      /* skip all between ENV_STR marks */
      do
        {
        v++;
        if (*v == '\0')
          return NULL;
        } while (*v != ENV_STR);
      }
    v++;
    }

  if (*v == ENV_SEP)
    return v + 1;

  return NULL;
  } /* end of 'get_next_field_var' function */


/* --------------------------------------------------------------------------
 * NAME         get_var_from_table
 * PURPOSE      searches for variable 'name' in dynamic table 
 *              and returns it value
 * ARGUMENTS    name - a string with variable 'name'
 * RETURNS      A pointer to 'value' or NULL - if not found
 * EXTERNS      None
   ------------------------------------------------------------------------*/
static char *get_var_from_table(name)
IN char *name;
  {
  int i;    /* index in EV entry table */
  char *p;  /* env var definition string pointer */
  char *v;  /* env var value string pointer */

  v = NULL;
  for (i = 0; i < max_envi_vars; i++)
    {
    p = strchr(root[i], ENV_VAR);
    if (p != NULL)
      {
      /* temporarily replace ENV_VAR character by end of string char */
      *p = '\0';
      if (strcmp(root[i], name) == 0)
        v = p + 1; /* just after ENV_VAR */
      *p = ENV_VAR;
      }
    }

  return v;
  } /* end of 'get_var_from_table' function */


/* --------------------------------------------------------------------------
 * NAME      get_variable_name  
 * PURPOSE   get name of variable from the buffer.
 * ARGUMENTS buff - buffer with variable definition.
 * RETURNS   variable name.
--------------------------------------------------------------------------*/
static char *get_variable_name(buff)
IN char *buff;
  {
  static char var_name[OS_ENV_VAR_LENGTH]; /* storage for var name */
  char *p; /* end of var name */

  strcpy(var_name, buff);

  p = strchr(var_name, ENV_VAR);
  ASSERT(p != NULL);

  *p = '\0';

  return var_name;
  } /* end of 'get_variable_name' function */


/* --------------------------------------------------------------------------
 * NAME         add_def
 * PURPOSE      adds single definition of the variable
 * ARGUMENTS    t_line - a pointer  to buffer with this def.
 * RETURNS      SUCCESS - if passed, or FAILURE - otherwise
 * EXTERNS      None
 * NOTES
 *              Lexical analysis.
 *              (1) Comment line is skipped (line that begins with '#').
 *              (2) Leading spaces are skipped.
 *              (3) Speces between variable name and definition are skipped
 *                  (i.e. <var_name><spaces>'='<spaces><fields-definition>).
 *              (4) Variable and field name is converted to upper case.
 *
 *              Semantic.
 *              (1) Existing variable is replaced by the new one.
 --------------------------------------------------------------------------*/

#define SKIP_BLANK \
  while (isspace(*(ch)))  \
    { \
    (ch)++; \
    (ch_nr)++; \
    }

/* copy character from input line to buffer 'var' */
#define GET_CHAR(ch) \
  { \
  *var_ptr = *ch; \
  ch++; \
  ch_nr++; \
  var_ptr++; \
  }

/* copy character from input line to buffer 'var' (convert to uppercase) */
#define GET_CHARU(ch) \
  { \
  *var_ptr = (char)toupper(*ch); \
  ch++; \
  ch_nr++; \
  var_ptr++; \
  }

#define SYNTAX_ERROR(msg, format) \
  { \
  sprintf format; \
  error_msg(msg, line_nr, ch_nr, line); \
  return FAILURE; \
  }

/* parse env var name (C-like identifier) and store it in 'name' */
#define GET_IDENT \
  { \
  i = 0; \
  if (*ch == '\0' || !isalpha(*ch)) \
    SYNTAX_ERROR(msg, (msg, "IDENT expected")) \
 \
  name[i] = (char)toupper(*ch); \
  i++; \
  GET_CHARU(ch) \
 \
  while (isalnum(*ch) || *ch == '_') \
    { \
    name[i] = (char)toupper(*ch); \
    i++; \
    GET_CHARU(ch) \
    } \
 \
  /* variable name already in 'name' buffer */ \
  name[i] = '\0'; \
  }

#if 0  // not used
/* parse field name (C-like identifier) and convert to uppercase */
#define GET_FIELD_NAME \
  { \
  if (*ch == '\0' || !isalpha(*ch)) \
    SYNTAX_ERROR(msg, (msg, "IDENT expected")) \
 \
  GET_CHARU(ch) \
 \
  while (isalnum(*ch) || *ch == '_') \
    GET_CHARU(ch) \
  }

/* parse field value (any text or dbl-quoted string) */
#define GET_FIELD_VALUE \
  { \
  while (*ch != '\0' && *ch != ENV_SEP) \
    { \
    if (*ch == ENV_STR) \
      { \
      /* skip all between ENV_STR marks */ \
      do \
	{ \
        GET_CHAR(ch) \
	if (*ch == '\0') \
	  SYNTAX_ERROR(msg, (msg, "'%c' expected", ENV_STR)) \
	}  \
      while (*ch != ENV_STR); \
 \
      /* skip closing ENV_STR */ \
      GET_CHAR(ch) \
      break; \
      } \
 \
    GET_CHAR(ch) \
    } \
  }

/* parse field definitione (<fld_name>:<value>) */
#define GET_FIELD_DEF \
  { \
  GET_FIELD_NAME \
 \
  if (*ch != ENV_DEF) \
    SYNTAX_ERROR(msg, (msg, "'%c' expected", ENV_DEF)) \
  GET_CHAR(ch) \
 \
  GET_FIELD_VALUE \
  }
#endif

static OKAY add_def(t_line, line_nr)
IN char *t_line;
IN int   line_nr;
  {
  char  var[OS_ENV_VAR_LENGTH]; /* buffer for variable definitiion */
  char *var_ptr;                /* pointer to current position in 'var' */
  char *ch;                     /* processed character */
  int   ch_nr;                  /* index of processed character (in line) */
  char  name[OS_ENV_VAR_LENGTH]; /* variable name */
  int   i;                      /* temporary counter */
  char  msg[MESSAGE_LENGTH_MAX];
  char *line;  /* Pointer to line after MACROs substitution */

  DBM(DBM_TRACE, printf("Line %3.3d:'%s'\n", line_nr, t_line))

  /* Eliminate LF as last string's symbol (if any)   :Esg */
  i = (int)strlen(t_line);
  if (t_line[i - 1] == ENV_LF)
    t_line[i - 1] = '\0';

  ch = t_line;
  ch_nr = 0;

  SKIP_BLANK

  /* empty line ? */
  if (*ch == '\0')
    return SUCCESS;

  /* comment line ? */
  if (*ch == ENV_COMMENT)
    return SUCCESS;

  /* MACRO substitution */
  line = macro_substitute(ch);
  if (line == NULL)
    return FAILURE;

  ch = line;
  ch_nr = 0;
  var_ptr = var;

  GET_IDENT

  SKIP_BLANK

  if (*ch != ENV_VAR)
    SYNTAX_ERROR(msg, (msg, "'%c' expected", ENV_VAR))
  GET_CHAR(ch)

  SKIP_BLANK

  /* Copy variable string's "tail" (actually value) */
  while (*ch != '\0')
    {
    GET_CHAR(ch)
    }

  /* variable buffer already field */
  *var_ptr = '\0';

  DBM(DBM_TRACE, printf("Variable:'%s'\n", var))

  return add_var_to_table(var);
  } /* end of 'add_def' function */


/* --------------------------------------------------------------------------
 * NAME         add_var_to_table
 * PURPOSE      Adds variable 'var' to variable list
 * ARGUMENTS    var - variable definition in format <var>=<setting>
 * RETURNS      SUCCESS - if added, FAILURE - no room or incorrect variable.
   ------------------------------------------------------------------------*/
static OKAY add_var_to_table(var)
IN char *var;
  {
  char  name[OS_ENV_VAR_LENGTH]; /* variable name */
  char *new_var;                 /* storage for new variable */
  int   i;                       /* temporary counter */
  int   result;


  /* make storage for the new definition and store it */
  new_var = (char *)mem_alloc(mem_class_id, "new_var", strlen(var) + 1);
  if (new_var == NULL)
    {
    DBM(DBM_TRACE, printf("Allocation error: %s\n", mem_strerr(mem_error())))
    return FAILURE;
    }
  strcpy(new_var, var);

  strcpy(name, get_variable_name(var));

  /* check if this variable already defined */
  for (i = 0; i < max_envi_vars; i++)
    {
    if (strcmp(get_variable_name(root[i]), name) == 0)
      break;
    }

  if (i < max_envi_vars)
    {
    /* replace previous definition by the new one */
    mem_free(root[i]);
    root[i] = new_var;
    }
  else /* add new variable to the configuration list */
    {
    /* increase list of variables (if necessary) */
    if (root == NULL || max_envi_vars >= total_vars)
      {
      result = resize_buffer(&root, total_vars, total_vars + SINGLE_CHUNK);
      if (result != SUCCESS)
	{
	mem_free(new_var);
	return FAILURE;
	}
      total_vars += SINGLE_CHUNK;
      }

    /* add new variable to the list */
    root[max_envi_vars++] = new_var;
    }

  return SUCCESS;
  } /* end of 'add_var_to_table' function */


/* --------------------------------------------------------------------------
 * NAME         resize_buffer
 * PURPOSE      Increases buffer for envi vars;
 * ARGUMENTS    r      - an address to buffer with pointers to envi vars
 *              oldsiz - amount of current size of the buffer
 *              newsiz - required amount of items
 * RETURNS      SUCCESS - if passed, FAILURE - otherwise
 * EXTERNS      None
   ------------------------------------------------------------------------*/
static OKAY resize_buffer(r, oldsize, newsize)
INOUT char ***r;
IN    int oldsize;
IN    int newsize;
  {
  char **p;
  int i;

  p = (char **)mem_alloc(mem_class_id, "p", sizeof(char*) * newsize);
  if (p == NULL)
    return FAILURE;

  for (i = 0; i < newsize; i++)
    p[i] = NULL;

  if (*r != NULL)
    {
    memcpy((VPTR)p, (VPTR)(*r), sizeof(char*) * oldsize);
    mem_free(*r);
    }

  *r = p;

  return SUCCESS;
  } /* end of 'resize_buffer' function */


/* --------------------------------------------------------------------------
 * NAME         free_buffer
 * PURPOSE      Release memory that keeps envi vars
 * ARGUMENTS    r - a pointer to buffer with pointers to vars
 * RETURNS      None
 * EXTERNS      None
   ------------------------------------------------------------------------*/
static VOID free_buffer(r)
INOUT char **r;
  {
  int i;

  if (r != NULL)
    {
    for (i = 0; i < total_vars; i++)
      if (r[i] != NULL)
	mem_free(r[i]);
    mem_free(r);
    }
  } /* end of 'free_buffer' function */


/* --------------------------------------------------------------------------
 * NAME      error_msg
 * PURPOSE   Display error message when syntax error in configuration file.
 * ARGUMENTS 
 *           msg     - syntax error mesage,
 *           line_nr - number of line in configuration file,
 *           ch_nr   - pointer onto erorneous character in line,
 *           line    - text of erorneous line.
 * EXTERNS   config_filename - name of processed configuration file.
--------------------------------------------------------------------------*/
static VOID error_msg(msg, line_nr, ch_nr, line)
IN char *msg;
IN int line_nr;
IN int ch_nr;
IN char *line;
  {
  int i;

  if (line_nr > 0)
    printf("Syntax error in configuration definition in file %s, line %d:\n", 
           config_filename, line_nr);
  else
    printf("Syntax error in environment variable\n");

  /* show the erorneous character */
  printf("%s\n", line);
  for (i = 0; i < ch_nr; i++)
    printf(" ");
  printf("^-- %s\n", msg);
  } /* end of 'error_msg' function */


/* --------------------------------------------------------------------------
 * NAME      dump_config
 * PURPOSE   Printout stored configuration.
 * EXTERNS   root - storage (list of variables).
--------------------------------------------------------------------------*/

static VOID dump_config()
  {
  int i;

  printf("Environment configuration (%d variable(s)):\n", max_envi_vars);
  for (i = 0; i < max_envi_vars; i++)
    printf("  %s\n", root[i]);

  printf("End of environment configuration.\n");
  } /* end of 'dump_config' function */


/* --------------------------------------------------------------------------
 * NAME      set_debugging
 * PURPOSE   Sets debugging mask from env var DEBUG_IEVL.
--------------------------------------------------------------------------*/

static VOID set_debugging()
  {
  char *env;                     /* pointer to EV */

  env = os_getenv(DBG_VAR);
  if (env == NULL)
    return;

  if (strlen(env) == 0)
    return;

  /* mask as decimal or hexadecimal number */
  if (strlen(env) > 2)
    {
    /* string begins with hex format indicator 0x */
    if (env[0] == '0' && toupper(env[1]) == 'X')
      {
      sscanf(env, "%x", &debug_mask);
      return;
      }
    }

  /* it must be in decimal format */
  sscanf(env, "%d", &debug_mask);
  } /* end of 'set_debugging' function */


/***
 * LOCAL STATIC VARIABLES
 ***/
static int table_ptr = 0;    /* Pointer to variables table */

/* --------------------------------------------------------------------------
 * NAME         reset_table_ptr
 * PURPOSE      Reset pointer to internal table of variables
 * ARGUMENTS    None
 * RETURNS      None
 * EXTERNS      None
 * NOTES        After this function execution first call to
 *              next_var_from_table() will return pointer to the
 *              FIRST varaible string in the table
   ------------------------------------------------------------------------*/
static VOID reset_table_ptr(VOID)
  {
  table_ptr = 0;
  }    /* End of reset_table_ptr() function */


/* --------------------------------------------------------------------------
 * NAME         next_var_from_table
 * PURPOSE      Return pointer to the "next" variable
 *              in the table
 * ARGUMENTS    None
 * RETURNS      Pointer to variable string;
 *              NULL if there are no more variables
 * EXTERNS      None
 * NOTES        None
   ------------------------------------------------------------------------*/
static char *next_var_from_table(VOID)
  {
  /* Check the end of the table */
  if ((max_envi_vars == 0) ||       /* Table is empty */
      (table_ptr >= max_envi_vars))
    return NULL;

  /* Return pointer */
  table_ptr++;
  return root[table_ptr - 1];

  }   /* End of next_var_from_table() function */


/* --------------------------------------------------------------------------
 * NAME         is_simple_var
 * PURPOSE      Check if specified string specified "simple" or
 *              "completed" variable value
 * ARGUMENTS    IN char *var_str - variable value string
 * RETURNS      TRUE  - variable is simple
 *              FALSE - variable is completed
 * EXTERNS      None
 * NOTES        Variable is completed if var_str has structure:
 *              "field1:val1; ...; fieldN:valN".
 *              Otherwise it is simpled
   ------------------------------------------------------------------------*/
static BOOL is_simple_var(var_str)
IN char *var_str;
  {
  char *v;

  /* Loop for checking */
  v = var_str;
  do
    {
    if (check_field_var(v) != SUCCESS)
      return TRUE;   /* Improper field structure --> variable is simple */
    } 
  while ((v = get_next_field_var(v)) != NULL);

  /* All field are proper --> variable is completed */
  return FALSE;

  }   /* End of is_simple_var() function */


/*****
 * LOCAL STATIC VARIABLES
 *****/
static char result_str[OS_ENV_VAR_LENGTH] = ""; /* Resulted string  */
static char temp_str[OS_ENV_VAR_LENGTH]   = ""; /* Temporary string */
static char macro_ref[OS_ENV_VAR_LENGTH]  = ""; /* MACRO reference  */


/* --------------------------------------------------------------------------
 * NAME         macro_substitute
 * PURPOSE      Substitute the MACROs definition in specified
 *              string
 * ARGUMENTS    IN char *var_str - variable value string
 * RETURNS      Pointer to resulted string
 *              NULL - there are unresolved reference(s) or
 *                     bad MACROs reference
 * EXTERNS      None
 * NOTES        The function doesn't execute any syntax parse
 *               
   ------------------------------------------------------------------------*/
static char *macro_substitute(var_str)
IN char *var_str;
  {
  /* Check source string length */
  if (strlen(var_str) >= OS_ENV_VAR_LENGTH)
    return NULL;
  strcpy(temp_str, var_str);

  /* Loop to look for all '$' characters in source string */
  for ( ; ; )
    {
    char *src, *dst;

    /* Looking for '$' character */
    src = temp_str;
    dst = result_str;

    do
      {   /* Loop on characters in the source string */
      if (*src == '$')
        {   /* It is MACRO reference --> search its definition */
        char *v, *val;

        /* Looking for longest identifier following '$' */
        src++;
        if (*src != '_' && !isalpha(*src))
          return NULL;    /* Bad identifier specification */
        v = macro_ref;
        *v++ = (char)toupper(*src++);
        while (*src == '_' || isdigit(*src) || isalpha(*src))
          *v++ = (char)toupper(*src++);
        *v = '\0';

        /* Search the MACRO definition */
        val = get_var_from_table(macro_ref);
        if (env_proc && val == NULL)
          val = os_getenv(macro_ref);

        *dst = '\0';
        if ((val == NULL) || (*val == '\0'))
          {   /* Undefined reference */
          printf("\nUnresolved reference '%s' in '%s' variable\n",
                    macro_ref, var_str);
          return NULL;
          }
        if (strlen(dst) + strlen(val) + strlen(src) >= OS_ENV_VAR_LENGTH)
          {  /* Too long string */
          printf("\nToo long resulted string after substition the '%s' into '%s'",
                 macro_ref, var_str);
          return NULL;
          }

        /* Substitution */
        v = val;
        do
          *dst++ = *v++;
        while (*v != '\0');
        }   /* End of MACRO processing */
      else
        *dst++ = *src++;  /* It is not MACRO reference --> copying */
      }
    while (*src != '\0');
    *dst = '\0';

    /* Check the next step necessity */
    if (strchr(result_str, (int)'$') != NULL)
      strcpy(temp_str, result_str);  /* There is one more MACRO reference */
    else
      break;                         /* It is all */
    }

  /* Successfull termination */
  return result_str;


  }   /* End of macro_substitute() function */


/* --------------------------------------------------------------------------
 * NAME         clear_table
 * PURPOSE      Clear all internal data structures
 * ARGUMENTS    None
 * RETURNS      None
   ------------------------------------------------------------------------*/
static VOID clear_table(VOID)
  {
  free_buffer(root);
  root = NULL;
  lib_env_buf[0] = '\0';
  env_string     = NULL; 
  max_envi_vars  = 0;
  total_vars     = 0;  
  } /* End of clear_table() function */


/* END OF 'ievl.c' FILE */
