/*********************************************************************
 *
 *         EZWGL, the EZ Widget and Graphics Library
 *
 * *             Copyright (C) 1996, 1997, 1998  Maorong Zou
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
/*******************************************************************
 *  
 *  Misc function dealing with files.
 */
#define _EZ_WIDGET_FILE_C_

#include "EZ_Widget.h"
/*******************************************************************
 *
 * Functions implemented in this file:
 */

int  EZ_FileReadable  MY_ANSIARGS((char *name));

/*******************************************************************
 *
 *  Return True if file exists and is readable.
 */
int  EZ_FileReadable(fname)
     char *fname;
{
  if(access(fname, F_OK) == 0) 
    {
      if(access(fname, R_OK) != 0) return(0);
      else return(1);
    }
  return(0);
}

/*******************************************************************/
#undef _EZ_WIDGET_FILE_C_
