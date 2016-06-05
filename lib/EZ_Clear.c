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
void              EZ_Clear_3_XImage00 MY_ANSIARGS((int flag));
void              EZ_Clear_3_XImage MY_ANSIARGS((int flag));
void              EZ_Clear_316_XImage MY_ANSIARGS((int flag));
void              EZ_Clear_332_XImage MY_ANSIARGS((int flag));
void              EZ_Clear_324_XImage MY_ANSIARGS((int flag));


void EZ_Clear_3_XImage00(flag)
     int flag;
{
  unsigned long   pixel_value;
  int              length;
  
  if(flag & EZ_COLOR_BUFFER_BIT)
    {
      if(EZ_ColorMode == EZ_CMAP_MODE)
	pixel_value = EZ_ColorArrayFix[(int)EZ_BackRGBA[0]];
      else 
	pixel_value = RGB2PixelLookUp_Fix(EZ_BackRGBA[0],EZ_BackRGBA[1], EZ_BackRGBA[2]);

      if(pixel_value == 0)
	{
	  length = EZ_Image3->bytes_per_line * EZ_Image3->height;
	  memset(EZ_ImageData, (int)pixel_value, length);
	}
      else
	{
	  int x, y, rows, cols;
	  XImage *ximage = EZ_Image3;
	  rows = EZ_WindowHeight;
	  cols = EZ_WindowWidth;

	  for(y = 0; y < rows; y++) for(x = 0; x < cols; x++)
	    XPutPixel(ximage, x, y, pixel_value);
	}
    }
  if((flag & EZ_DEPTH_BUFFER_BIT) && (EZ_ZBufferSet != 0))
   memset(EZ_Zbuf,0, ((EZ_WindowWidth * EZ_WindowHeight) * sizeof(int)));
}


void EZ_Clear_3_XImage(flag)
     int flag;
{
  unsigned long   pixel_value;
  int             length;
 
  if(flag & EZ_COLOR_BUFFER_BIT)
    {
      if(EZ_ColorMode == EZ_CMAP_MODE)
	pixel_value = EZ_ColorArrayFix[(int)EZ_BackRGBA[0]];  /* EZ_BackRGBA[] are integers */
      else 
	{EZ_DitherRGB(pixel_value,4,5,
		      (int)EZ_BackRGBA[0],
		      (int)EZ_BackRGBA[1], 
		      (int)EZ_BackRGBA[2]);}

      length = EZ_Image3->bytes_per_line * EZ_Image3->height;
      memset(EZ_ImageData, (int)pixel_value, length);
    }
   if((flag & EZ_DEPTH_BUFFER_BIT) && (EZ_ZBufferSet != 0))
     memset(EZ_Zbuf,0, ((EZ_WindowWidth * EZ_WindowHeight)*sizeof(int)));
}

void EZ_Clear_316_XImage(flag)
     int flag;
{
  unsigned long   pixel_value;
  int             m, length;
  unsigned short  *pixptr;
 
  if(flag & EZ_COLOR_BUFFER_BIT)
    {
      if(EZ_ColorMode == EZ_CMAP_MODE)
	pixel_value = EZ_ColorArrayFix[(int)EZ_BackRGBA[0]];
      else 
	pixel_value = RGB2PixelLookUp_Fix(EZ_BackRGBA[0],EZ_BackRGBA[1], EZ_BackRGBA[2]);

      length = EZ_Image3->bytes_per_line * EZ_Image3->height;
      if(pixel_value == 0)
	{
	  memset(EZ_ImageData, (int)pixel_value, length);
	}
      else
	{
	  pixptr = (unsigned short *) EZ_ImageData;
	  for(m = 0; m < (length >> 1); m++)
	    *pixptr++ =(unsigned short )pixel_value;
	}
    }
  if((flag & EZ_DEPTH_BUFFER_BIT) && (EZ_ZBufferSet != 0))
   memset(EZ_Zbuf,0, ((EZ_WindowWidth * EZ_WindowHeight) * sizeof(int)));
}


void EZ_Clear_332_XImage(flag)
     int flag;
{
  unsigned long   pixel_value;
  int             m, length;
  unsigned int   *pixptr;
 
  if(flag & EZ_COLOR_BUFFER_BIT)
    {
      if(EZ_ColorMode == EZ_CMAP_MODE)
	pixel_value = EZ_ColorArrayFix[(int)EZ_BackRGBA[0]];
      else 
	pixel_value = RGB2PixelLookUp_Fix(EZ_BackRGBA[0],EZ_BackRGBA[1], EZ_BackRGBA[2]);
      
      length = EZ_Image3->bytes_per_line * EZ_Image3->height;

      if(pixel_value == 0)
	{
	  memset(EZ_ImageData, (int)pixel_value, length);
	}
      else
	{
	  pixptr = (unsigned int *) EZ_ImageData;
	  for(m = 0; m < (length >> 2); m++)
	    *pixptr++ =(unsigned int )pixel_value;
	}
    }
  if((flag & EZ_DEPTH_BUFFER_BIT) && (EZ_ZBufferSet != 0))
   memset(EZ_Zbuf,0, ((EZ_WindowWidth * EZ_WindowHeight) * sizeof(int)));
}

void EZ_Clear_324_XImage(flag)
     int flag;
{
  unsigned long   pixel_value;
  int             height;
 
  if(flag & EZ_COLOR_BUFFER_BIT)
    {
      if(EZ_ColorMode == EZ_CMAP_MODE)
	pixel_value = EZ_ColorArrayFix[(int)EZ_BackRGBA[0]];
      else 
	pixel_value = RGB2PixelLookUp_Fix(EZ_BackRGBA[0],EZ_BackRGBA[1], EZ_BackRGBA[2]);
      
      height = EZ_Image3->height;

      if(pixel_value == 0)
	{
	  int length = EZ_Image3->bytes_per_line * height;
	  memset(EZ_ImageData, (int)pixel_value, length);
	}
      else
	{
	  int y, x, width;
	  int bpl = EZ_Image3->bytes_per_line;
	  unsigned char *pixptr;
	  unsigned char pv[4];
	  width = EZ_WindowWidth;
	  pixptr = (unsigned char *) &pixel_value;
	  pv[0] = *pixptr++; pv[1] = *pixptr++;
	  pv[2] = *pixptr++; pv[3] = 0;

	  pixptr = (unsigned char *) EZ_ImageData;
	  for(y = 0; y < height; y++, pixptr += bpl)
	    {
	      unsigned char *ptr = pixptr;
	      for(x = 0; x < width; x++)
		{ *ptr++ = pv[0]; *ptr++ = pv[1]; *ptr++ = pv[2];}
	    }
	}
    }
  if((flag & EZ_DEPTH_BUFFER_BIT) && (EZ_ZBufferSet != 0))
   memset(EZ_Zbuf,0, ((EZ_WindowWidth * EZ_WindowHeight) * sizeof(int)));
}
/***************************************************************************/



