/* FILE NAME    itoliiff.h
 * PURPOSE      Exported ITOLIIFF module header.
 * 
 * CODING       Robert Chyla                         '91.01
 * LAST UPDATED Robert Chyla                         '94.12.06
 * 
 * NOTES        None.
 * 
 */

#ifndef _ITOLIIFF_H_
#define _ITOLIIFF_H_

int Iiff_Open_Read PROT((char *));
int Iiff_Close_Read PROT((VOID));
int Iiff_Get_Resolution PROT((int *, int *));
int Iiff_Get_Pixel_Size PROT((int *, int *));
int Iiff_Get_Origin PROT((int *, int *));
int Iiff_Get_RGB_Row PROT((int, BYTE *, BYTE *, BYTE *));

#endif

/* END OF 'itoliiff.h' FILE */

