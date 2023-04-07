/* FILE NAME	ietl.h
 * PURPOSE	Integra EvenT Library definitions
 *
 *              Who                       PRF name     When
 * SPEC		Erk
 * DESIGN	Enk
 * DES. VV	Erk
 * CODING	Enk
 * CODE VV.	Erk
 * LAST UPDATED Eed et_whole_queue_reading() added     00.01
 *
 * COPYRIGHT (c) INTEGRA, Inc., 1996
 *
 */

#ifndef _IETL_H_
#define _IETL_H_

/*  INCLUDE DIRECTIVES  */
#include "ifbl.h"		/* include IFBL just in case */

/**********************************************************
 *           MACRO DEFINITIONS AND TYPEDEFS
 **********************************************************/

/* Device handler type */

typedef VPTR ET_PD;

/* invalid device (returned by et_device_open) */
#define ET_PD_NONE ((ET_PD *)NULL)

/* Pointing device types */

#define ET_PD_MOUSE		0

/*    its button codes (from left): */
#define		ET_PD_MOUSE_LEFT		0
#define		ET_PD_MOUSE_MIDDLE		1
#define		ET_PD_MOUSE_RIGHT		2

#define ET_PD_TABLET		1
  
/*    its valuator codes: */
#define		ET_PD_TABLET_X		0
#define		ET_PD_TABLET_Y		1
/*    its button codes: */
#define		ET_PD_TABLET_1		0
#define		ET_PD_TABLET_2		1
#define		ET_PD_TABLET_3		2
#define		ET_PD_TABLET_4		3

#define  ET_PD_DIAL		2

/*    its valuator codes: */
#define		ET_PD_DIAL_1		0
#define		ET_PD_DIAL_2		1
#define		ET_PD_DIAL_3		2
#define		ET_PD_DIAL_4		3
#define		ET_PD_DIAL_5		4
#define		ET_PD_DIAL_6		5
#define		ET_PD_DIAL_7		6
#define		ET_PD_DIAL_8		7
#define		ET_PD_DIAL_9		8

/* Keyboard codes */

/*  modifier codes: */
#define ET_MOD_SHIFT		0x0001
#define ET_MOD_CONTROL		0x0002
/*  key codes: */
#define ET_K_BS		0x0008	/* backspace (ASCII code 8) */
#define ET_K_TAB	0x0009	/* tab       (ASCII code 9) */
#define ET_K_ENTER	0x000d	/* Enter/CR  (ASCII code 13) */
#define ET_K_ESC	0x001b	/* Esc       (ASCII code 27) */
/*    0x20-0x7E  - all printable ASCII codes */
#define ET_K_DEL	0x007f	/* Del       (ASCII code 127) */
#define ET_K_F1		0x0100	/* functional key F1 */
#define ET_K_F2		0x0101	/* functional key F2 */
#define ET_K_F3		0x0102	/* functional key F3 */
#define ET_K_F4		0x0103	/* functional key F4 */
#define ET_K_F5		0x0104	/* functional key F5 */
#define ET_K_F6		0x0105	/* functional key F6 */
#define ET_K_F7		0x0106	/* functional key F7 */
#define ET_K_F8		0x0107	/* functional key F8 */
#define ET_K_F9		0x0108	/* functional key F9 */
#define ET_K_F10	0x0109	/* functional key F10 */
#define ET_K_LEFT	0x0200	/* left arrow key */
#define ET_K_RIGHT	0x0201	/* right arrow key */
#define ET_K_UP		0x0202	/* up arrow key */
#define ET_K_DOWN	0x0203	/* down arrow key */
#define ET_K_HOME	0x0204	/* home key */
#define ET_K_END	0x0205	/* end key */
#define ET_K_PGDN	0x0206	/* page down key */
#define ET_K_PGUP	0x0207	/* page up key */

/* Windows' event codes: */

#define ET_EVENT_REDRAW		0	/* window redraw request */
#define ET_EVENT_RESIZE		1	/* window changed geometry */
#define ET_EVENT_FOCUS		2	/* keyboard focus changed */
#define ET_EVENT_DESTROY	3	/* window destroyed */
#define ET_EVENT_CREATE		4	/* window created */
#define ET_EVENT_ICONIZE	5	/* window minimized (iconized) */
#define ET_EVENT_CLOSE		6	/* close the window */

/* Private message code: */

#define ET_EVENT_CLIENT_MSG	7	/* application's private message */

/* Keyboard and pointing device event codes: */

#define ET_EVENT_BUTTON_PRESS	8	/* button pressed */
#define ET_EVENT_BUTTON_RELEASE	9	/* button released */
#define ET_EVENT_POINTER	10	/* the default pointer movement */
#define ET_EVENT_VALUATOR	11	/* valuator changed */
#define ET_EVENT_KEY		12	/* keyboard code received */

/* Event masks: */

#define ET_EVENT_MASK_WINDOW	0x0001 /* enable window's events */
#define ET_EVENT_MASK_CLIENT	0x0002 /* enable client events */
#define ET_EVENT_MASK_POINTING	0x0004 /* enable pointing device events */
#define ET_EVENT_MASK_KEYBOARD	0x0008 /* enable keyboard events */

/* Client message data types: */

#define ET_DATA_BYTE		0	/* data of type BYTE */
#define ET_DATA_WORD		1	/* data of type WORD */
#define ET_DATA_DWORD		2	/* data of type DWORD */
#define ET_DATA_INT		3	/* data of type int */
#define ET_DATA_LONG		4	/* data of type long */

/* Event descriptor type: */

typedef struct
  {
  FB_WIN window;
  int    x, y;
  int    w, h;
  } ET_EVENT_REDRAW_T;

typedef struct
  {
  FB_WIN window;
  int    x, y;
  int    w, h;
  } ET_EVENT_RESIZE_T;

typedef struct
  {
  FB_WIN window;
  int    in;
  } ET_EVENT_FOCUS_T;

typedef struct
  {
  FB_WIN window;
  } ET_EVENT_DESTROY_T;

typedef struct
  {
  FB_WIN window;
  FB_WIN parent;
  int    x, y;
  int    w, h;
  } ET_EVENT_CREATE_T;

typedef struct
  {
  FB_WIN window;
  } ET_EVENT_ICONIZE_T;

typedef struct
  {
  FB_WIN window;
  } ET_EVENT_CLOSE_T;

typedef struct
  {
  FB_WIN window;
  int    type;
  union
    {
    BYTE  byte_val;
    WORD  word_val;
    DWORD dword_val;
    int   int_val;
    long  long_val;
    } data;
  } ET_EVENT_CLIENT_T;

typedef struct
  {
  FB_WIN window;
  int    device;
  int    button;
  int    modifier;
  } ET_EVENT_BUTTON_T;

typedef struct
  {
  FB_WIN window;
  int    x, y;
  int    modifier;
  } ET_EVENT_POINTER_T;

typedef struct
  {
  FB_WIN window;
  int    device;
  int    valuator;
  int    value;
  int    modifier;
  } ET_EVENT_VALUATOR_T;

typedef struct
  {
  FB_WIN window;
  int    code;
  int    modifier;
  } ET_EVENT_KEY_T;

typedef union
  {
  ET_EVENT_REDRAW_T redraw;
  ET_EVENT_RESIZE_T resize;
  ET_EVENT_FOCUS_T focus;
  ET_EVENT_DESTROY_T destroy;
  ET_EVENT_CREATE_T create;
  ET_EVENT_ICONIZE_T iconize;
  ET_EVENT_CLOSE_T close;
  ET_EVENT_CLIENT_T client_msg;
  ET_EVENT_BUTTON_T button;
  ET_EVENT_POINTER_T pointer;
  ET_EVENT_VALUATOR_T valuator;
  ET_EVENT_KEY_T key;
  } ET_EVENT_T;


/**********************************************************
 *               FUNCTION PROTOTYPES
 **********************************************************/

/* get IETL version label */
char *ietl_version(void);

/* General functions */

/* initialize IETL */
OKAY et_init(void);

/* terminate IETL */
void et_term(void);

/* Event service */

/* set events mask */
void et_mask_set(IN unsigned int mask);

/* get events mask */
unsigned int et_mask_get(void);

/* is any event ready to be get */
BOOL et_event_ready(void);

/* read event */
int et_event_read(OUT ET_EVENT_T *event);

/* query event in queue */
BOOL et_event_test(OUT int *type, OUT ET_EVENT_T *event);

/* send event */
OKAY et_event_send(IN FB_WIN window, IN int event_type,	IN ET_EVENT_T *event);

/* Pointer grabbing */

/* grab pointer events */
OKAY et_pointer_grab(IN FB_WIN window);

/* release grabbed pointer */
void et_pointer_ungrab(void);

/* Keyboard focus setting */

/* sets focus for given window */
void et_focus_set(IN FB_WIN window);

/* Pointing devices */

/* query list of supported pointing device */
int et_device_list(OUT int *default_dev,
		   OUT int **types,
		   OUT char ***names,
		   OUT int **n_valuators,
		   OUT int **n_buttons);

/* open pointing device */
ET_PD et_device_open(IN int type, IN int n_valuators, IN int n_buttons);

/* close pointing device */
void et_device_close(ET_PD device);

/* set pointing device valuator range */
void et_device_set_range(IN ET_PD device, IN int valuator,
			 IN int minv, IN int maxv);

/* query valuator range */
void et_device_get_range(IN ET_PD device, IN int valuator,
			 IN int *minv, IN int *maxv);

/* set reading of whole system events queue */
void et_whole_queue_reading(void);

#endif /* #ifndef _IETL_H_ */

/* END OF 'ietl.h' FILE */
