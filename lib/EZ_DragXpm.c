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
 *  May 1997. Handling of raw rgb images added by Arno Dabekaussen.
 */
/**********************************************************************/
#define _EZ_DRAG_XPM_C_
#include "EZ_Widget.h"

void EZ_SetupDragablePixmap MY_ANSIARGS((int flag));
void EZ_OverlayDragablePixmap MY_ANSIARGS((EZ_Widget *wgt, Drawable d,
					   int x, int y, int w, int h));
/**********************************************************************/
#include "EZ_DragHandle.xpm"
/**********************************************************************/
EZ_Bitmap *EZ_DragHandlePixmap;
/**********************************************************************/
void EZ_SetupDragablePixmap(flag) int flag;
{
  if(flag) EZ_DragHandlePixmap = EZ_CreateLabelPixmapFromXpmData(dragHandleXPMdata);
  else EZ_DragHandlePixmap = NULL;
}
/**************************************************************************/
void EZ_OverlayDragablePixmap(widget, drawable, x, y, w, h)
     EZ_Widget *widget; Drawable drawable; int x,y,w,h;
{
  int i, j, wcnt, hcnt, wrem, hrem, x0, y0 ;
  GC gc = EZ_WRITABLEGC;
  XSetClipMask(EZ_Display, gc, EZ_DragHandlePixmap->shape);

  wcnt = w >> 4; wrem = w & 0xf;
  hcnt = h >> 4; hrem = h & 0xf;
  
  for(y0 = y, j = 0; j < hcnt; j++)
    {
      for(x0 = x, i = 0; i < wcnt; i++)
	{
	  XSetClipOrigin(EZ_Display, gc,x0,y0);  
	  XCopyArea(EZ_Display,
		    EZ_DragHandlePixmap->pixmap,
		    drawable,
		    gc, 0,0, 16, 16,
		    x0,y0);
	  x0 += 16;
	}
      if(wrem)
	{
	  XSetClipOrigin(EZ_Display, gc,x0,y0);  
	  XCopyArea(EZ_Display,
		    EZ_DragHandlePixmap->pixmap,
		    drawable,
		    gc, 0,0, wrem, 16,
		    x0,y0);
	}
      y0 += 16;
    }
  if(hrem)
    {
      for(x0 = x, i = 0; i < wcnt; i++)
	{
	  XSetClipOrigin(EZ_Display, gc,x0,y0);  
	  XCopyArea(EZ_Display,
		    EZ_DragHandlePixmap->pixmap,
		    drawable,
		    gc, 0,0, 16, hrem,
		    x0,y0);
	  x0 += 16;
	}
      if(wrem)
	{
	  XSetClipOrigin(EZ_Display, gc,x0,y0);  
	  XCopyArea(EZ_Display,
		    EZ_DragHandlePixmap->pixmap,
		    drawable,
		    gc, 0,0, wrem, hrem,
		    x0,y0);
	}
    }
  XSetClipMask(EZ_Display, gc, None);

  EZ_DrawRectBorderWithSize(widget, drawable, x-1,y-1, w+2,h+2, 1, 
			    EZ_BORDER_UP,0,0,0);

}
/**************************************************************************/
#undef _EZ_DRAG_XPM_C_
