/*
 * NAME	    IPDL.H
 * PURPOSE	Header file for INTEGRA Pointing Device Library(IPDL)
 *
 * SPEC		Nobuhiro Watanabe		1989.07.10
 * DESIGN	Takashi Hyodo			1989.07.10
 * DES. INSP.	Nobuhiro Watanabe		1989.07.12
 * 		Akira Fujimoto			1989.07.13
 * CODING	Takashi Hyodo			1989.07.14
 * CODE INSP.	Nobuhiro Watanabe		1989.07.14
 * TEST		Takashi Hyodo			1989.07.14
 * 		Nobuhiro Watanabe		1989.07.14
 * UPDATED      RAW, CPR                        1990.07.31
 * UPDATED      NRC (PD_SetValuatorValue)       91.04.16
 *
 * NOTE		Do not modify this file without permission of INTEGRA.
 *
 * Copyright (C) INTEGRA Inc. 1989, All Rights Reserved.
 */

#ifndef _IPDL_H_
#define	_IPDL_H_

#define	PD_MOUSE		1
#define	PD_TABLET		2
#define	PD_DIGITIZER		2
#define	PD_DIAL			3
/* following PDs are reserved for future */
#if 0
#define	PD_BUTTON_BOX		4
#define	PD_TRACK_BALL		5
#define	PD_LIGHT_PEN		6
#define	PD_JOY_STICK		7
#define	PD_TOUCH_PANNEL		8
#define	PD_DIGITIZER_3D		9
#define	PD_SPACE_BALL		10
#define	PD_DATA_GLOVE		11
#define	PD_DATA_SUIT		12
#endif

/* the following two constants must have exactly the same values as */
/* respective X, Y locators for mouse, tablet and the other devices */
/* eg: PD_LOCATOR_X == PD_MOUSE_X == PD_TABLET_X etc.               */

#define	PD_LOCATOR_X		0
#define	PD_LOCATOR_Y		1

#define	PD_MOUSE_X		PD_LOCATOR_X
#define	PD_MOUSE_Y		PD_LOCATOR_Y
#define	PD_MOUSE_1		0
#define	PD_MOUSE_2		1
#define	PD_MOUSE_3		2

#define	PD_TABLET_X		PD_LOCATOR_X
#define	PD_TABLET_Y		PD_LOCATOR_Y
#define	PD_TABLET_1		0
#define	PD_TABLET_2		1
#define	PD_TABLET_3		2
#define	PD_TABLET_4		3

#define	PD_DIAL_1		0
#define	PD_DIAL_2		1
#define	PD_DIAL_3		2
#define	PD_DIAL_4		3
#define	PD_DIAL_5		4
#define	PD_DIAL_6		5
#define	PD_DIAL_7		6
#define	PD_DIAL_8		7
#define	PD_DIAL_9		8

/* PD management structure */
typedef struct
  {
  int	type;		/* PD 'real' type */
  int	n_valuator;	/* # of valuators */
  int	n_button;	/* # of buttons */
  union
    {
    int	value[2];
    long	l_value;
    float	f_value;
    double	d_value;
    VPTR	handle;
    } data;			/* for IPDL's internal use */
  } PD;

#ifndef NO_PROTOTYPES

char    *ipdl_version(VOID);

PD      *PD_Open(int, int, int);
int      PD_Close(PD *);

int      PD_QueryValuator(PD *, int);
int      PD_QueryButton(PD *, int);

VOID     PD_SetValuatorRange(PD *, int, int, int);
VOID     PD_QueryValuatorRange(PD *, int, int *, int *);

int      PD_SetValuatorValues(PD *, int *);

int      PD_QueryType(PD *);

#else

char    *ipdl_version();

PD      *PD_Open();
int      PD_Close();

int      PD_QueryValuator();
int      PD_QueryButton();

VOID     PD_SetValuatorRange();
VOID     PD_QueryValuatorRange();

int      PD_SetValuatorValues();

int      PD_QueryType();

#endif

#endif

