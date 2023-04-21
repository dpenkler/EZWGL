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
 *   Polygon, Points and Polyline funcs
 *   Render directly into a X11 Drawable.
 *   
 */

/******************************************************************/

#include "EZ_Widget.h"

/***************************************************************
 *
 *  Functions implemented in this file
 */
void     EZ_DrawPolygonX11 MY_ANSIARGS((int *vts, int nvts));
void     EZ_DrawLineX11 MY_ANSIARGS((int vtx1, int vtx2));
void     EZ_DrawPolyLineX11 MY_ANSIARGS((int *vts, int nvts));
void     EZ_DrawPointX11 MY_ANSIARGS((int vtx));
void     EZ_DrawPointsX11 MY_ANSIARGS((int *vts, int nvts));
void     EZ_ClearX11 MY_ANSIARGS((int flag));
void     EZ_SwapBuffersX11 MY_ANSIARGS((void));

/******************************************************************
 *
 * Internal storage for X11 vertices.
 */
static XPoint _ez_x11_vts_[VBUFSIZE];

/******************************************************************
 *
 *   General polygon.
 */

void EZ_DrawPolygonX11(vts, n)
     int  *vts, n;
{
  register int        i;
  register XPoint     *xptr, *xptrtmp;
  register int        vtidx;
  unsigned long       tmp;  
  int                 (*screen)[3] = EZ_VBuf.screen;
  int                 (*rgba)[4] = EZ_VBuf.rgba;
  int                 *pntptr;

  xptr = _ez_x11_vts_;
  xptrtmp = xptr;

  for(i = 0; i < n; i++)
    {
      vtidx = vts[i];
      pntptr = screen[vtidx];
      xptrtmp->x = (short)pntptr[0];
      xptrtmp->y = (short)pntptr[1];
      xptrtmp++;
    }
  /*------------- 
   * set color;
   *------------*/
  if(EZ_ColorMode == EZ_CMAP_MODE)
    {
      int ir = rgba[vts[0]][0] >> COLOR_SHIFT_BITS;
      tmp = EZ_ColorArray[ir];
    }
  else
    {
      int i = vts[0];
      int *clrs = rgba[i];
      int r = clrs[0] >> COLOR_SHIFT_BITS;
      int g = clrs[1] >> COLOR_SHIFT_BITS;
      int b = clrs[2] >> COLOR_SHIFT_BITS;
      if(EZ_Depth <= 8)
	{
	  EZ_DitherRGB(tmp,4,5, r,g,b);
	}
      else
	tmp = RGB2PixelLookUp(r,g,b);
    }
  /*-------------------
   * fill the polygon
   *------------------*/
  XSetForeground(EZ_Display,EZ_MainGC,tmp);
  XFillPolygon(EZ_Display, EZ_Drawable, EZ_MainGC,
	       xptr, n, Nonconvex, CoordModeOrigin);

}

/******************************************************************
 *
 *   Line
 */

void EZ_DrawLineX11(vtx0, vtx1)
     int vtx0, vtx1;
{
  unsigned long tmp;
  int           (*screen)[3] = EZ_VBuf.screen;
  int           (*rgba)[4] = EZ_VBuf.rgba;
  int           *bgnptr, *endptr;
  /*------------- 
   * set color;
   *------------*/
  if(EZ_ColorMode == EZ_CMAP_MODE)
    {
      int ir = rgba[vtx0][0] >> COLOR_SHIFT_BITS;
      tmp = EZ_ColorArray[ir];
    }
  else
    {
      int *clrs = rgba[vtx0];
      int r = clrs[0] >> COLOR_SHIFT_BITS;
      int g = clrs[1] >> COLOR_SHIFT_BITS;
      int b = clrs[2] >> COLOR_SHIFT_BITS;
      if(EZ_Depth <= 8)
	{
	  EZ_DitherRGB(tmp,4,5, r,g,b);
	}
      else
	tmp = RGB2PixelLookUp(r,g,b);
    }
  /*----------------
   * draw the line
   *---------------*/
  XSetForeground(EZ_Display,EZ_MainGC,tmp);
  XSetLineAttributes(EZ_Display,EZ_MainGC,EZ_LineWidth, EZ_LineStyle,CapButt,JoinMiter);
  bgnptr = screen[vtx0];
  endptr = screen[vtx1];  
  XDrawLine(EZ_Display, EZ_Drawable, EZ_MainGC,
	    (int)(bgnptr[0]), 
	    (int)(bgnptr[1]),
	    (int)(endptr[0]),
	    (int)(endptr[1]));
}

/****************************************************************
 *
 *   X11 Polyline.
 */

void EZ_DrawPolyLineX11(vts,n)
     int *vts,  n;
{
  register int         i, idx;
  unsigned long        tmp;  
  int                 (*screen)[3] = EZ_VBuf.screen;
  int                 (*rgba)[4] = EZ_VBuf.rgba;
  int                 *pntptr;
  int                  retval;
  /*------------- 
   * set color;
   *------------*/
  if(EZ_ColorMode == EZ_CMAP_MODE)
    {
      int ir = rgba[vts[0]][0] >> COLOR_SHIFT_BITS;
      tmp = EZ_ColorArray[ir];
    }
  else
    {
      int i = vts[0];
      int *clrs = rgba[i];
      int r = clrs[0] >> COLOR_SHIFT_BITS;
      int g = clrs[1] >> COLOR_SHIFT_BITS;
      int b = clrs[2] >> COLOR_SHIFT_BITS;
      if(EZ_Depth <= 8)
	{
	  EZ_DitherRGB(tmp,4,5, r,g,b);
	}
      else
	tmp = RGB2PixelLookUp(r,g,b);
    }
  for(i=0; i< n; i++)
    {
      idx = *vts++;
      pntptr = screen[idx];
      _ez_x11_vts_[i].x = (short) pntptr[0];
      _ez_x11_vts_[i].y = (short) pntptr[1];
    }
  XSetForeground(EZ_Display,EZ_MainGC,tmp);
  XSetLineAttributes(EZ_Display, EZ_MainGC,EZ_LineWidth,
		     EZ_LineStyle, CapButt, JoinMiter);
  XDrawLines(EZ_Display,EZ_Drawable,EZ_MainGC,_ez_x11_vts_, n, CoordModeOrigin);
}
  
/******************************************************************
 *
 *   Points
 */
void EZ_DrawPointsX11(vts,n)
     int *vts, n;
{
  register int        i, idx;
  unsigned long       tmp;    
  int                 (*screen)[3] = EZ_VBuf.screen;
  int                 (*rgba)[4] = EZ_VBuf.rgba;
  int                 *pntptr;

  for(i=0; i< n; i++)
    {
      /*------------- 
       * set color;
       *------------*/
      idx = *vts++;
      if(EZ_ColorMode == EZ_CMAP_MODE)
	{
	  int ir = rgba[idx][0] >> COLOR_SHIFT_BITS;
	  tmp = EZ_ColorArray[ir];
	}
      else
	{
	  int *clrs = rgba[idx];
	  int r = clrs[0] >> COLOR_SHIFT_BITS;
	  int g = clrs[1] >> COLOR_SHIFT_BITS;
	  int b = clrs[2] >> COLOR_SHIFT_BITS;
	  if(EZ_Depth <= 8)
	    {
	      EZ_DitherRGB(tmp,4,5, r,g,b);
	    }
	  else
	    tmp = RGB2PixelLookUp(r,g,b);
	}
      /*---------------
       * draw a point
       *--------------*/
      XSetForeground(EZ_Display,EZ_MainGC,tmp);
      pntptr = screen[idx];
      XDrawPoint(EZ_Display, EZ_Drawable, EZ_MainGC,
		 (int)(pntptr[0]),
		 (int)(pntptr[1]));
    }
}

  
/******************************************************************
 *
 *   Points
 */
void EZ_DrawPointX11(vtx)
     int vtx;
{
  unsigned long        tmp;    
  int                 (*screen)[3] = EZ_VBuf.screen;
  int                 (*rgba)[4] = EZ_VBuf.rgba;

  /*------------- 
   * set color;
   *------------*/
  if(EZ_ColorMode == EZ_CMAP_MODE)
    {
      int ir = rgba[vtx][0] >> COLOR_SHIFT_BITS;
      tmp = EZ_ColorArray[ir];
    }
  else
    {
      int *clrs = rgba[vtx];
      int r = clrs[0] >> COLOR_SHIFT_BITS;
      int g = clrs[1] >> COLOR_SHIFT_BITS;
      int b = clrs[2] >> COLOR_SHIFT_BITS;
      if(EZ_Depth <= 8)
	{
	  EZ_DitherRGB(tmp,4,5, r,g,b);
	}
      else
	tmp = RGB2PixelLookUp(r,g,b);
    }
  /*----------------
   * draw the point
   *---------------*/
  XSetForeground(EZ_Display,EZ_MainGC,tmp);
  XDrawPoint(EZ_Display, EZ_Drawable, EZ_MainGC,
	     (int)(screen[vtx][0]),
	     (int)(screen[vtx][1]));
}
/*****************************************************************
 *
 * Clear the current X11 Drawable
 */
void  EZ_ClearX11(flag)
     int flag;
{
  unsigned long pixel_value;
  int x0,x1,y0,y1;
  x0 = EZ_WindowCenterX - EZ_WindowScaleX;
  x1 = EZ_WindowCenterX + EZ_WindowScaleX;
  y0 = EZ_WindowCenterY - EZ_WindowScaleY;
  y1 = EZ_WindowCenterY + EZ_WindowScaleY;
  if (x0 < 0) x0 = 0;
  if (y0 < 0) y0 = 0;
  if (x1 > EZ_WindowWidth) x1 = EZ_WindowWidth;
  if (y1 > EZ_WindowHeight) y1 = EZ_WindowHeight;
  if(flag & EZ_COLOR_BUFFER_BIT)
    {
      if(EZ_ColorMode == EZ_CMAP_MODE)
	{
	  pixel_value = EZ_ColorArray[EZ_BackRGBA[0]];
	}
      else
	{
	  if(EZ_Depth <= 8)
	    {
	      EZ_DitherRGB(pixel_value,4,5, EZ_BackRGBA[0],EZ_BackRGBA[1], EZ_BackRGBA[2]);
	    }
	  else
	    pixel_value = RGB2PixelLookUp( EZ_BackRGBA[0],EZ_BackRGBA[1], EZ_BackRGBA[2]);
	}  
  
      XSetForeground(EZ_Display, EZ_MainGC, pixel_value);
      XFillRectangle(EZ_Display, 
		     EZ_Drawable, 
		     EZ_MainGC, 
		     x0, y0, x1, y1);
      XSync(EZ_Display,0);
    }
}

/*****************************************************************
 *
 * Update the front buffer for X11, i.e., copy the back pixmap
 * to the display window.
 */
void EZ_SwapBuffersX11()
{
  if(EZ_DoubleBufferSet == EZ_PIXMAP )
    {
      XCopyArea(EZ_Display,
		EZ_Pixmap3,
		EZ_MainWindow,
		EZ_MainGC,
		0, 0,
		(unsigned int)EZ_WindowWidth, (unsigned int)EZ_WindowHeight,
		0, 0);
      XSync(EZ_Display, 0);
    }
}

/**************************************************************************/



