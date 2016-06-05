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
/********************************************************************
 *
 *  EZ_Clear.c
 */
#include "EZ_Widget.h"
#include "EZ_XImageTypes.h"

/***************************************************************
 *
 *  Functions implemented in this file
 */
void              EZ_Clear_3_Pixmap MY_ANSIARGS((int flag));

/***************************************************************
 *
 * Clear the Drawable.
 */
/***************************************************************
 *
 *  Clear the draw buffer. (XImage)
 */
void EZ_Clear_3_Pixmap(flag)
     int flag;
{
  unsigned long   pixel_value;

  if(flag & EZ_COLOR_BUFFER_BIT)
    {
      if(EZ_ColorMode == EZ_CMAP_MODE)
	pixel_value = EZ_ColorArray[(int)EZ_BackRGBA[0]];
      else 
	{
	  if(EZ_Depth <= 8)
	    {EZ_DitherRGB( pixel_value,4,5,
			  EZ_BackRGBA[0],EZ_BackRGBA[1], EZ_BackRGBA[2]);}
	  else if(EZ_Depth > 8)
	    {pixel_value =  RGB2PixelLookUp(EZ_BackRGBA[0],EZ_BackRGBA[1], 
					    EZ_BackRGBA[2]);}
	  else
	    pixel_value = 0;
	}
      /*------------------------------------------------------------
       * if the background color has changed, we have to clear the
       * whole window.
       *-----------------------------------------------------------*/
      XSetForeground(EZ_Display,EZ_MainGC,pixel_value);
      XFillRectangle(EZ_Display,EZ_Drawable,EZ_MainGC,
		     0, 0,
		     (unsigned int)EZ_WindowWidth,  (unsigned int)EZ_WindowHeight);
    }
  if((flag & EZ_DEPTH_BUFFER_BIT) && (EZ_ZBufferSet != 0))
    memset(EZ_Zbuf,0, ((EZ_WindowWidth * EZ_WindowHeight) * sizeof(int)));
}
/***************************************************************/
