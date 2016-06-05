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
#define WINDOW_WIDTH  EZ_WindowWidth

/***************************************************************
 *
 *  Render Zbuffered Point and Points.
 *      Write to a back XIMAGE.
 *
 */

void DRAW_A_POINT(vtx)
     int vtx;
{
#ifdef _ZBUFFERED_  	  
  int            *zbufptr;
  int            itmp,zcoor;
#endif
  unsigned long  pixel_value;

  int            xcoor, ycoor;
  int            (*screenptr)[3] = EZ_VBuf.screen;
  int            (*colorptr)[4] = EZ_VBuf.rgba;
  int            *pntptr = screenptr[vtx];
  
  xcoor = pntptr[0];
  ycoor = pntptr[1];
#ifdef _ZBUFFERED_  
  itmp = (pntptr[1]) * (WINDOW_WIDTH) + pntptr[0];	  
  zbufptr =   EZ_Zbuf + itmp;  
#endif

  if(EZ_ColorMode == EZ_CMAP_MODE)
    {
      int ir = colorptr[vtx][0] >> COLOR_SHIFT_BITS;
      pixel_value = EZ_ColorArrayFix[ir];
    }
  else
    {
      int ir, g,b;
      int *clrs = colorptr[vtx];
      ir = clrs[0];
      g =  clrs[1];
      b =  clrs[2];
      SET_PIXEL_VALUE_RGB_FIX(pixel_value,pntptr[0],pntptr[1], ir,g,b);
    }
#ifdef _ZBUFFERED_  	     
  zcoor = (pntptr[2]) - EZ_LP_Z_ADJUST;
  if( *zbufptr > zcoor)
    {
      *zbufptr = zcoor;
#endif
      XPutPixel(EZ_Image3,xcoor, ycoor, pixel_value);
#ifdef _ZBUFFERED_  	    
    }
#endif
}

/***********************************************************/

void DRAW_N_POINTS(vertices, n)
     int   *vertices;
     int   n;
{
#ifdef _ZBUFFERED_  	    
  int               *zbufptr;
  int               itmp, zcoor;
#endif
  int               vtx, wwidth, loopct;
  unsigned long     pixel_value;
  int               xcoor, ycoor;
  int              (*screenptr)[3] = EZ_VBuf.screen;
  int              (*colorptr)[4] = EZ_VBuf.rgba;
  int              *pntptr, *pntclr;
  XImage           *ximage = EZ_Image3;
  
  wwidth = WINDOW_WIDTH;
  vtx = *vertices++;  

  for(loopct = 0; loopct < n; loopct++, vtx = *vertices++)
    {
      pntptr = screenptr[vtx];
      pntclr = colorptr[vtx];
      xcoor = pntptr[0];
      ycoor = pntptr[1];
#ifdef _ZBUFFERED_  	    
      itmp = (pntptr[1]) * (WINDOW_WIDTH) + pntptr[0];	  
      zbufptr =  EZ_Zbuf + itmp;
#endif
      if(EZ_ColorMode == EZ_CMAP_MODE)
	{
	  int ir = pntclr[0] >> COLOR_SHIFT_BITS;
	  pixel_value = EZ_ColorArrayFix[ir];
	}
      else
	{
	  int ir, g,b;
	  ir = pntclr[0];
	  g =  pntclr[1];
	  b =  pntclr[2];
	  SET_PIXEL_VALUE_RGB_FIX(pixel_value,pntptr[0],pntptr[1], ir,g,b);
	}
#ifdef _ZBUFFERED_  	          
      zcoor = pntptr[2] - EZ_LP_Z_ADJUST;
      if( *zbufptr > zcoor)
	{
	  *zbufptr = zcoor;
#endif
	  XPutPixel(ximage,xcoor, ycoor, pixel_value);
#ifdef _ZBUFFERED_  	    
	}
#endif
    }
}
/****************************************************************/
#undef WINDOW_WIDTH


