/*********************************************************************
 *
 *         EZWGL, the EZ Widget and Graphics Library
 *
 *             Copyright (C) 1996, 1997  Maorong Zou
 *  
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the Free
 * Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 **********************************************************************/
/*
 *  June 1996.  Beta Release.
 *  Sept 1996.  Release Version 1.0
 *  Dec 1996.  Release Version 1.1 Beta
 *  April 1997.  Release Version 1.2
 *  November 1997.  Release Version 1.3
 *  November 1998.  Release Version 1.4
 *  December 1999.  Release Version 1.50
 */
/*****************************************************************/
#include <X11/Xresource.h>

#ifndef EZ_INT
#define EZ_INT     0
#define EZ_FLOAT   1
#define EZ_POINTER 2
#define EZ_ULONG   3
#define EZ_LONG    4
#define EZ_UCHAR   5
#define EZ_CHAR    6
#define EZ_USHORT  7
#define EZ_SHORT   8
#define EZ_UINT    9
#define EZ_STRING  10
#define EZ_ENUM    11
#define EZ_BOOL    12
#define EZ_FUNCTION 13
#define EZ_FONTNAME 14
#define EZ_FILENAME 15
#define EZ_COLOR    16
#define EZ_REPEAT   100
#endif
/*****************************************************************/
typedef struct EZ_Value_ {
  int  type; 
  union {
    char    c;    unsigned char  uc;
    short   s;    unsigned short us;
    int     i;    unsigned int   ui;
    float   f;
    void    *p;
    void    *function;
    char    *str;
    unsigned long l;
  } value;
} EZ_Value;
/*****************************************************************/
typedef struct _EZ_SymbConst
{
  XrmQuark quark;
  EZ_Value value;
} EZ_SymbQConst;
/*****************************************************************/
#ifndef _EZ_RESOURCE_C_
extern  EZ_SymbQConst *EZ_QConstants;
extern int             EZ_NumQConstants;
#endif
#ifndef  EZ_ArraySize
#define  EZ_ArraySize(_ary_) (sizeof(_ary_)/sizeof((_ary_)[0]))
#endif
/*****************************************************************/
