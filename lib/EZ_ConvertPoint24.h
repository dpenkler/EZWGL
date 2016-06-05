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
#define BYTES_PER_LINE  (EZ_Image3->bytes_per_line)
/***************************************************************
 *
 *  Render Zbuffered Point and Points.
 *      Write to a back XIMAGE.
 *
 */

void DRAW_A_POINT(vtx)
     int vtx;
{
  unsigned char  *pixelptr;
#ifdef _ZBUFFERED_  	  
  int            *zbufptr;
  int            zcoor;
#endif

  int            xcoor, ycoor, bpl;
  int            (*screenptr)[3] = EZ_VBuf.screen;
  int            (*colorptr)[4] = EZ_VBuf.rgba;
  int            *pntptr = screenptr[vtx];
  unsigned long  pixel_value;
  unsigned char  *pv4 = (unsigned char *)&pixel_value;


  bpl = BYTES_PER_LINE;
  xcoor = pntptr[0];
  ycoor = pntptr[1];

  pixelptr = ((unsigned char *)EZ_ImageData) + ycoor * bpl + (xcoor+xcoor+xcoor);
#ifdef _ZBUFFERED_  	  
  zbufptr =   EZ_Zbuf + ycoor * WINDOW_WIDTH + xcoor;  
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
      pixelptr[0] =pv4[0]; pixelptr[1] =pv4[1];
      pixelptr[2] =pv4[2];		      
#ifdef _ZBUFFERED_  	    
    }
#endif
}

/***********************************************************/

void DRAW_N_POINTS(vertices, n)
     int   *vertices;
     int   n;
{
  unsigned char     *pixelptr;
#ifdef _ZBUFFERED_  	    
  int               *zbufptr;
  int               zcoor;
#endif
  int               vtx,  wwidth, loopct, bpl;
  int               xcoor, ycoor;
  int              (*screenptr)[3] = EZ_VBuf.screen;
  int              (*colorptr)[4] = EZ_VBuf.rgba;
  int              *pntptr, *pntclr;
  unsigned long           pixel_value;
  unsigned char           *pv4 = (unsigned char *)&pixel_value;

  wwidth = WINDOW_WIDTH;
  bpl = BYTES_PER_LINE;
  vtx = *vertices++;  

  for(loopct = 0; loopct < n; loopct++, vtx = *vertices++)
    {
      pntptr = screenptr[vtx];
      pntclr = colorptr[vtx];

      xcoor = pntptr[0];
      ycoor = pntptr[1];
      pixelptr = ((unsigned char *)EZ_ImageData) + ycoor * bpl + (xcoor+xcoor+xcoor);
#ifdef _ZBUFFERED_  	    
      zbufptr =  EZ_Zbuf + ycoor * wwidth + xcoor;
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
	  pixelptr[0] =pv4[0]; pixelptr[1] =pv4[1];
	  pixelptr[2] =pv4[2];		      
#ifdef _ZBUFFERED_  	    
	}
#endif
    }
}
/****************************************************************/
#undef WINDOW_WIDTH
#undef BYTES_PER_LINE

