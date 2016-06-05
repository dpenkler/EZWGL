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
/*****************************************************************
 *
 *  Text properties. They are used in TEXT widget exclusively.
 */
typedef struct CharProp_ {
  unsigned long    fg;         /* background color, pixel value    */
  unsigned long    bg;         /* foreground color  pixel value    */
  int              font;       /* font id,  -1 for default         */
  char             fgset;      /* 0 means not   */
  char             bgset;      /* 0 means not   */
  char             underline;  /* 0 means not   */
  char             mark;       /* mark, used in garbage collecting */
  EZ_Bitmap        *bitmap;    /* if set, the text will be ignored */
} CharProp;

typedef CharProp TextProp;
typedef CharProp EZ_TextProp;
typedef CharProp EZ_TextProperty;

#define EZ_TextPropertyForeground(pptr)   ((pptr)->fg)
#define EZ_TextPropertyBackground(pptr)   ((pptr)->bg)
#define EZ_TextPropertyFont(pptr)         ((pptr)->font)
#define EZ_TextPropertyFGSet(pptr)        ((pptr)->fgset)
#define EZ_TextPropertyBGSet(pptr)        ((pptr)->bgset)
#define EZ_TextPropertyMark(pptr)         ((pptr)->mark)
#define EZ_TextPropertyUnderLine(pptr)    ((pptr)->underline)
#define EZ_TextPropertySpecial(pptr)      ((pptr)->bitmap)
#define EZ_TextPropertyBitmap(pptr)       ((pptr)->bitmap)

typedef struct ITextLine_ {
  int         width;          /* total width of line */
  int         height;         /* height of line      */
  int         length;         /* length in chars     */
  int         space;          /* allocated space     */
  int         x, y;           /* UL corner           */
  char        *data;          /* the string          */
  TextProp    **prop;         /* associated props    */
  int         tmp;            /* tmp marker */
  int         count;          /* # of chars for wrapped lines */
  struct ITextLine_ *leader;  /* used in wrapped lines*/
} ITextLine;


  


