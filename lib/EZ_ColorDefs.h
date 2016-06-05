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

/************************************************************
 *  The colors listed below are guarnteed to be mapped once
 *  EZ_GL is initialized. The pixel values are stored in
 *  the corresponding entries in EZ_Context.XSInfo.ColorArray
 *  (EZ_ColorArray).
 */

/* don't change the ordering! */

#ifndef GRAY0

#define GRAY0           0
#define GRAY1           1
#define GRAY2           2
#define GRAYF           3  /* foreground */  
#define GRAY4           4
#define GRAY5           5
#define GRAY6           6
#define GRAY7           7
#define GRAY8           8
#define DARKRED         9
#define RED             10 
#define GREEN           11 
#define BLUE            12 
#define YELLOW          13 
#define MAGENTA         14 
#define CYAN            15 

#define BLACK           GRAY0
#define WHITE           GRAY8
#define EZ_GRAYF        GRAYF

#define EZ_DARK_GRAY1    GRAY1
#define EZ_DARK_GRAY2    GRAY2
#define EZ_NORMAL_GRAY1  GRAY4
#define EZ_NORMAL_GRAY2  GRAY5
#define EZ_BRIGHT_GRAY1  GRAY6
#define EZ_BRIGHT_GRAY2  GRAY7
#define EZ_BLACK         BLACK
#define EZ_WHITE         WHITE
#endif

/********************************************************
 *
 *  Listed below is the set of colors which are mapped
 *  at initialization
 */

#ifdef EZ_AllocateColorEntry
EZ_AllocateColorEntry(GRAY0,  0,  0,  0 , "gray0")
EZ_AllocateColorEntry(GRAY1,  97, 97, 97, "gray38")
EZ_AllocateColorEntry(GRAY2,  117,117,117,"gray46")
EZ_AllocateColorEntry(GRAYF,  0,  0,  0,  "gray0")
EZ_AllocateColorEntry(GRAY4,  189,189,189,"gray74")
EZ_AllocateColorEntry(GRAY5,  214,214,214,"gray84")
EZ_AllocateColorEntry(GRAY6,  222,222,222,"gray87")
EZ_AllocateColorEntry(GRAY7,  237,237,237,"gray93")
EZ_AllocateColorEntry(GRAY8,  255,255,255,"gray100") 
EZ_AllocateColorEntry(DARKRED,192,0,  0,  "red4" )
EZ_AllocateColorEntry(RED,    255,0,  0,  "red1")
EZ_AllocateColorEntry(GREEN,  0,  255,0,  "green1")
EZ_AllocateColorEntry(BLUE,   0,  0,  255,"blue1")
EZ_AllocateColorEntry(YELLOW, 255,255,0,  "yellow")  
EZ_AllocateColorEntry(MAGENTA,255,0,  255,"magenta")  
EZ_AllocateColorEntry(CYAN,   0,255,  255,"cyan")  
#endif
/********************************************************/





