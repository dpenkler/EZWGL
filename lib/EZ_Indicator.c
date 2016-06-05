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
/*****************************************************************
 ***                                                           ***
 ***              Draw indicator on check/radio buttons        ***
 ***                                                           ***
 *****************************************************************/
#define _EZ_INDICATOR_C_
#include "EZ_Widget.h"

/*********************************************************************
 * 
 *  Functions implemented in this file:
 */
void             EZ_DrawIndicator MY_ANSIARGS((
					       EZ_Widget *wptr,
					       Pixmap pixmap, 
					       int onoff,
					       int type, 
					       int bwidth,
					       int adjust,
					       unsigned long color));

/*********************************************************************/
void  EZ_DrawIndicator(wptr, pixmap, onoff, type,
		       bwidth, adjust, color)
     EZ_Widget *wptr;
     Pixmap pixmap; /* pixmap to draw into */
     int onoff; /* state, on or off */
     int type; /* type of indicator */
     int bwidth; /* border width */
     int adjust;        /* indicator size adjustment */
     unsigned long color; /* used when onoff != 0 */
{
  switch(type)
    {
    case EZ_SQUARE_INDICATOR:
      {
	GC  gc;
	int x,y, ww, hh, padb = EZ_WidgetPadB(wptr);
	
	if(onoff)  /* sunken */
	  {
	    if(color == 0) 
	      {
		gc = EZ_DARKREDGC;
	      }
	    else
	      {
		XSetForeground(EZ_Display, EZ_WRITABLEGC, color);	
		gc = EZ_WRITABLEGC;
	      }
	  }
	else /* raised */
	  {
	    if(EZ_WidgetBGPixmapB(wptr) != NULL)
	      {
		EZ_Bitmap *bitmap = EZ_WidgetBGPixmapB(wptr);
		gc = EZ_NTILEGC2;
		XSetTile(EZ_Display,EZ_NTILEGC2,bitmap->pixmap);
		XSetTSOrigin(EZ_Display, EZ_NTILEGC2, 0, 0);
	      }
	    else if(EZ_WidgetParentBGPixmapB(wptr) != NULL)
	      {
		EZ_Bitmap *bitmap = EZ_WidgetParentBGPixmapB(wptr);
		gc = EZ_NTILEGC2;
		XSetTile(EZ_Display,EZ_NTILEGC2,bitmap->pixmap);
		XSetTSOrigin(EZ_Display, EZ_NTILEGC2, 0, 0);
	      }
	    else
	      {
		unsigned long junkpv;
		int tile = EZ_GetBackgroundGC(wptr,&gc, &junkpv, 0, 0);
		if(tile) 
		  {
		    int ox, oy;
		    EZ_GetWidgetTileOrigin(wptr, &ox, &oy);      
		    XSetTSOrigin(EZ_Display, gc, ox, oy);
		  }
	      }  
	  }
	ww = EZ_WidgetAuxXLeft(wptr) - 6;
	hh = ww + 1;
	x =  EZ_WidgetPadX(wptr) + EZ_WidgetBorderWidth(wptr) + padb;
	y =  (EZ_WidgetHeight(wptr) - hh) >> 1;
	XFillRectangle(EZ_DisplayForWidgets, pixmap, gc, x,y, ww, hh); 
	EZ_DrawRectBorderWithSize(wptr, pixmap, x,y, ww,hh, bwidth, 
				  ((onoff == 0) ? EZ_BORDER_UP: EZ_BORDER_DOWN),0,0,1);
      }
      break;
    case EZ_SUNKEN_SQUARE_INDICATOR:
      {
	int x,y, ww,  padb = EZ_WidgetPadB(wptr);
	GC  gc;

	ww = EZ_WidgetAuxXLeft(wptr) - 6;
	x = EZ_WidgetPadX(wptr) + EZ_WidgetBorderWidth(wptr) + padb;
	y = (EZ_WidgetHeight(wptr) - ww) >> 1;
	if(onoff)
	  {
	    if(color)
	      {
		XSetForeground(EZ_Display, EZ_WRITABLEGC, color);	
		gc = EZ_WRITABLEGC;
	      }
	    else gc = EZ_DARKREDGC;
	    XFillRectangle(EZ_DisplayForWidgets, pixmap, gc, x,y, ww, ww); 
	  }
	else
	  {
	    if(EZ_WidgetBGPixmapB(wptr) != NULL)
	      {
		EZ_Bitmap *bitmap = EZ_WidgetBGPixmapB(wptr);
		gc = EZ_NTILEGC2;
		XSetTile(EZ_Display,EZ_NTILEGC2,bitmap->pixmap);
		XSetTSOrigin(EZ_Display, EZ_NTILEGC2, 0, 0);
	      }
	    else if(EZ_WidgetParentBGPixmapB(wptr) != NULL)
	      {
		EZ_Bitmap *bitmap = EZ_WidgetParentBGPixmapB(wptr);
		gc = EZ_NTILEGC2;
		XSetTile(EZ_Display,EZ_NTILEGC2,bitmap->pixmap);
		XSetTSOrigin(EZ_Display, EZ_NTILEGC2, 0, 0);
	      }
	    else
	      {
		unsigned long junkpv;
		int tile = EZ_GetBackgroundGC(wptr,&gc, &junkpv, 0, 0);
		if(tile) 
		  {
		    int ox, oy;
		    EZ_GetWidgetTileOrigin(wptr, &ox, &oy);      
		    XSetTSOrigin(EZ_Display, gc, ox, oy);
		  }
	      }  
	    XFillRectangle(EZ_DisplayForWidgets, pixmap, gc, x,y, ww, ww); 
	  }
	EZ_DrawRectBorderWithSize(wptr, pixmap, x,y, ww,ww, 
				  bwidth, EZ_BORDER_DOWN,0,0,1);
      }
      break;
    case EZ_CHECK_INDICATOR:       
      {
	int x,y, ww, padb = EZ_WidgetPadB(wptr);
	GC gc;
	ww = EZ_WidgetAuxXLeft(wptr) - 6;
	x = EZ_WidgetPadX(wptr) + EZ_WidgetBorderWidth(wptr) + padb;
	y = (EZ_WidgetHeight(wptr) - ww) >> 1;

	/* fill the background */
	if(EZ_WidgetBGPixmapB(wptr) != NULL)
	  {
	    EZ_Bitmap *bitmap = EZ_WidgetBGPixmapB(wptr);
	    gc = EZ_NTILEGC2;
	    XSetTile(EZ_Display,EZ_NTILEGC2,bitmap->pixmap);
	    XSetTSOrigin(EZ_Display, EZ_NTILEGC2, 0, 0);
	  }
	else if(EZ_WidgetParentBGPixmapB(wptr) != NULL)
	  {
	    EZ_Bitmap *bitmap = EZ_WidgetParentBGPixmapB(wptr);
	    gc = EZ_NTILEGC2;
	    XSetTile(EZ_Display,EZ_NTILEGC2,bitmap->pixmap);
	    XSetTSOrigin(EZ_Display, EZ_NTILEGC2, 0, 0);
	  }
	else
	  {
	    unsigned long junkpv;
	    int tile = EZ_GetBackgroundGC(wptr,&gc, &junkpv, 0, 0);
	    if(tile) 
	      {
		int ox, oy;
		EZ_GetWidgetTileOrigin(wptr, &ox, &oy);      
		XSetTSOrigin(EZ_Display, gc, ox, oy);
	      }
	  }
	  XFillRectangle(EZ_DisplayForWidgets, pixmap, gc, x,y, ww, ww); 

	  if(onoff)
	    {
	      GC  gc = EZ_DARKREDGC;
	      int t = bwidth +1;
	      if(color) XSetForeground(EZ_Display, gc, color);	
	      XDrawLine(EZ_Display, pixmap, gc, x+t,y+t, x+ww-t, y+ww-t);
	      XDrawLine(EZ_Display, pixmap, gc, x+t,y+ww-t, x+ww-t, y+t);
	      if(color) XSetForeground(EZ_Display, gc, EZ_ColorArray[DARKRED]);		    
	    }
	  EZ_DrawRectBorderWithSize(wptr, pixmap, x,y, ww,ww, 
				    bwidth, EZ_BORDER_DOWN,0,0,0);
      }
      break;
    case EZ_DIAMOND_INDICATOR:
      {
	GC gc;
	int x,y,ow,ww, padb = EZ_WidgetPadB(wptr);
	if(onoff)
	  {
	    if(color == 0) gc = EZ_DARKREDGC;
	    else
	      {
		XSetForeground(EZ_Display, EZ_WRITABLEGC, color);	
		gc = EZ_WRITABLEGC;
	      }
	  }
	else 
	  {
	    if(EZ_WidgetBGPixmapB(wptr) != NULL)
	      {
		EZ_Bitmap *bitmap = EZ_WidgetBGPixmapB(wptr);
		gc = EZ_NTILEGC2;
		XSetTile(EZ_Display,EZ_NTILEGC2,bitmap->pixmap);
		XSetTSOrigin(EZ_Display, EZ_NTILEGC2, 0, 0);
	      }
	    else if(EZ_WidgetParentBGPixmapB(wptr) != NULL)
	      {
		EZ_Bitmap *bitmap = EZ_WidgetParentBGPixmapB(wptr);
		gc = EZ_NTILEGC2;
		XSetTile(EZ_Display,EZ_NTILEGC2,bitmap->pixmap);
		XSetTSOrigin(EZ_Display, EZ_NTILEGC2, 0, 0);
	      }
	    else
	      {
		unsigned long junkpv;
		int tile = EZ_GetBackgroundGC(wptr,&gc, &junkpv, 0, 0);
		if(tile) 
		  {
		    int ox, oy;
		    EZ_GetWidgetTileOrigin(wptr, &ox, &oy);      
		    XSetTSOrigin(EZ_Display, gc, ox, oy);
		  }
	      }
	  }
	ow = EZ_WidgetAuxXLeft(wptr) -4;
	x =  EZ_WidgetPadX(wptr) + EZ_WidgetBorderWidth(wptr) -2 + padb;
	y =  ((EZ_WidgetHeight(wptr)) >> 1); 
	
	ww = (ow + 1) >> 1;
	{
	  XPoint xpoints[4];
	  xpoints[0].x = x;        xpoints[0].y = y; 
	  xpoints[1].x = x+ww;     xpoints[1].y = y-ww;
	  xpoints[2].x = x+ww+ww;  xpoints[2].y = y;
	  xpoints[3].x = x+ww;     xpoints[3].y = y+ww;
	  XFillPolygon(EZ_Display, pixmap, gc, xpoints,4,Convex,CoordModeOrigin);
	}
	EZ_DrawDiamondBorderWithSize(wptr, pixmap, x,y,ow,ow,bwidth,
				     ((onoff == 0) ? EZ_BORDER_UP: EZ_BORDER_DOWN),0,0,1);
      }
      break;
    case EZ_SUNKEN_DIAMOND_INDICATOR:
      {
	GC gc;
	int x,y,ow,ww, padb = EZ_WidgetPadB(wptr);
	ow = EZ_WidgetAuxXLeft(wptr) -4;
	x =  EZ_WidgetPadX(wptr) + EZ_WidgetBorderWidth(wptr) -2 + padb;
	y =  ((EZ_WidgetHeight(wptr)) >> 1); 
	
	ww = (ow +1) >> 1;
	{
	  XPoint xpoints[4];
	  xpoints[0].x = x;        xpoints[0].y = y; 
	  xpoints[1].x = x+ww;     xpoints[1].y = y-ww;
	  xpoints[2].x = x+ww+ww;  xpoints[2].y = y;
	  xpoints[3].x = x+ww;     xpoints[3].y = y+ww;
	  if(onoff)
	    {
	      if(color == 0) gc = EZ_DARKREDGC;
	      else
		{
		  XSetForeground(EZ_Display, EZ_WRITABLEGC, color);	
		  gc = EZ_WRITABLEGC;
		}
	      XFillPolygon(EZ_Display, pixmap, gc, xpoints,4,Convex,CoordModeOrigin);
	    }
	  else
	    {
	      if(EZ_WidgetBGPixmapB(wptr) != NULL)
		{
		  EZ_Bitmap *bitmap = EZ_WidgetBGPixmapB(wptr);
		  gc = EZ_NTILEGC2;
		  XSetTile(EZ_Display,EZ_NTILEGC2,bitmap->pixmap);
		  XSetTSOrigin(EZ_Display, EZ_NTILEGC2, 0, 0);
		}
	      else if(EZ_WidgetParentBGPixmapB(wptr) != NULL)
		{
		  EZ_Bitmap *bitmap = EZ_WidgetParentBGPixmapB(wptr);
		  gc = EZ_NTILEGC2;
		  XSetTile(EZ_Display,EZ_NTILEGC2,bitmap->pixmap);
		  XSetTSOrigin(EZ_Display, EZ_NTILEGC2, 0, 0);
		}
	      else
		{
		  unsigned long junkpv;
		  int tile = EZ_GetBackgroundGC(wptr,&gc, &junkpv, 0, 0);
		  if(tile) 
		    {
		      int ox, oy;
		      EZ_GetWidgetTileOrigin(wptr, &ox, &oy);      
		      XSetTSOrigin(EZ_Display, gc, ox, oy);
		    }
		}  
	      XFillPolygon(EZ_Display, pixmap, gc, xpoints,4,Convex,CoordModeOrigin);
	    }
	}
	EZ_DrawDiamondBorderWithSize(wptr, pixmap, x,y,ow,ow,bwidth,EZ_BORDER_DOWN,0,0,1);
      }
      break;
    case EZ_RECTANGLE_INDICATOR: 
      {
	GC gc;	
	int x,y, ww, hh, padb = EZ_WidgetPadB(wptr);
	x = EZ_WidgetBorderWidth(wptr) + 3 + padb;
	y = x;
	hh = ( EZ_WidgetHeight(wptr) - (y<<1));
	ww = EZ_WidgetAuxXLeft(wptr) - 8;

	if(onoff)
	  {
	    if(color == 0) gc = EZ_DARKREDGC;
	    else
	      {
		XSetForeground(EZ_Display, EZ_WRITABLEGC, color);	
		gc = EZ_WRITABLEGC;
	      }
	    XFillRectangle(EZ_DisplayForWidgets, pixmap, gc, x,y, ww, hh); 
	  }
	else
	  {
	    if(EZ_WidgetBGPixmapB(wptr) != NULL)
	      {
		EZ_Bitmap *bitmap = EZ_WidgetBGPixmapB(wptr);
		gc = EZ_NTILEGC2;
		XSetTile(EZ_Display,EZ_NTILEGC2,bitmap->pixmap);
		XSetTSOrigin(EZ_Display, EZ_NTILEGC2, 0, 0);
	      }
	    else if(EZ_WidgetParentBGPixmapB(wptr) != NULL)
	      {
		EZ_Bitmap *bitmap = EZ_WidgetParentBGPixmapB(wptr);
		gc = EZ_NTILEGC2;
		XSetTile(EZ_Display,EZ_NTILEGC2,bitmap->pixmap);
		XSetTSOrigin(EZ_Display, EZ_NTILEGC2, 0, 0);
	      }
	    else
	      {
		unsigned long junkpv;
		int tile = EZ_GetBackgroundGC(wptr,&gc, &junkpv, 0, 0);
		if(tile) 
		  {
		    int ox, oy;
		    EZ_GetWidgetTileOrigin(wptr, &ox, &oy);      
		    XSetTSOrigin(EZ_Display, gc, ox, oy);
		  }
	      }  
	    XFillRectangle(EZ_DisplayForWidgets, pixmap, gc, x,y, ww, hh); 
	  }
	EZ_DrawRectBorderWithSize(wptr, pixmap, x,y, ww,hh, 
				  bwidth, EZ_BORDER_DOWN,0,0,0);
      }
      break;
    case EZ_CIRCLE_INDICATOR: 
      {
	int x,y, ww, padb = EZ_WidgetPadB(wptr);
	GC dgc, bgc, ngc;

	if(EZ_WidgetBGPixmapB(wptr) != NULL)
	  {
	    EZ_Bitmap *bitmap = EZ_WidgetBGPixmapB(wptr);
	    bgc = EZ_NTILEGC2;
	    dgc = EZ_NTILEGC1;
	    XSetTile(EZ_Display,EZ_NTILEGC2,bitmap->pixmap);
	    XSetTile(EZ_Display,EZ_NTILEGC1,bitmap->dkpixmap);
	    XSetTSOrigin(EZ_Display, EZ_NTILEGC2, 0, 0);
	    XSetTSOrigin(EZ_Display, EZ_NTILEGC1, 0, 0);
	  }
	else if(EZ_WidgetParentBGPixmapB(wptr) != NULL)
	  {
	    EZ_Bitmap *bitmap = EZ_WidgetParentBGPixmapB(wptr);
	    bgc = EZ_NTILEGC2;
	    dgc = EZ_NTILEGC1;
	    XSetTile(EZ_Display,EZ_NTILEGC2,bitmap->pixmap);
	    XSetTile(EZ_Display,EZ_NTILEGC1,bitmap->dkpixmap);
	    XSetTSOrigin(EZ_Display, EZ_NTILEGC2, 0, 0);
	    XSetTSOrigin(EZ_Display, EZ_NTILEGC1, 0, 0);
	  }
	else
	  {
	    int tile = EZ_GetDarkBrightNormalBDGC(wptr, &dgc, &ngc, &bgc); 
	    /* bright/normal GC switched */
	    if(tile) 
	      {
		int ox, oy;
		EZ_GetWidgetTileOrigin(wptr, &ox, &oy);      
		XSetTSOrigin(EZ_Display, bgc, ox, oy);
		XSetTSOrigin(EZ_Display, dgc, ox, oy);
	      }
	  }  
	ww = EZ_WidgetAuxXLeft(wptr) -4;	
	x =  ( EZ_WidgetPadX(wptr)>>1) + EZ_WidgetBorderWidth(wptr) + padb;
	y =  ((EZ_WidgetHeight(wptr) - ww) >> 1);
	XFillArc(EZ_DisplayForWidgets, pixmap, bgc, x,y, ww, ww, 0, 23040);
	XDrawArc(EZ_DisplayForWidgets, pixmap, dgc, x,y, ww, ww, 0, 23040);
	if(onoff)
	  {
	    GC gc;
	    if(color == 0) gc = EZ_DARKREDGC;
	    else
	      {
		XSetForeground(EZ_Display, EZ_WRITABLEGC, color);	
		gc = EZ_WRITABLEGC;
	      }
	    ww =  ww - 2 -(bwidth<<1);
	    x += bwidth +1; y += bwidth +1;
	    XFillArc(EZ_DisplayForWidgets, pixmap, gc, x,y, ww, ww, 0, 23040);
	    XDrawArc(EZ_DisplayForWidgets, pixmap, dgc, x,y, ww, ww, 0, 23040);
	  }
      }
      break;
    case EZ_MENU_BUTTON_INDICATOR: 
      {
	GC gc;
	int x,y, ww, hh, padb = EZ_WidgetPadB(wptr);
	int tmp = EZ_WidgetPadX(wptr) >> 1;
	ww = EZ_WidgetAuxXLeft(wptr)+ tmp - 6;
	hh = (ww >> 1);
	x = EZ_WidgetBorderWidth(wptr)  + padb + tmp + 2;
	y = (EZ_WidgetHeight(wptr) - hh ) >> 1;

	if(onoff)
	  {
	    if(color != 0L)
	      {
		gc = EZ_WRITABLEGC;
		XSetForeground(EZ_Display, gc, color);	
	      }
	    else gc = EZ_DARKREDGC;
	  }
	else
	  {
	    if(EZ_WidgetBGPixmapB(wptr) != NULL)
	      {
		EZ_Bitmap *bitmap = EZ_WidgetBGPixmapB(wptr);
		gc = EZ_NTILEGC2;
		XSetTile(EZ_Display,EZ_NTILEGC2,bitmap->pixmap);
		XSetTSOrigin(EZ_Display, EZ_NTILEGC2, 0, 0);
	      }
	    else if(EZ_WidgetParentBGPixmapB(wptr) != NULL)
	      {
		EZ_Bitmap *bitmap = EZ_WidgetParentBGPixmapB(wptr);
		gc = EZ_NTILEGC2;
		XSetTile(EZ_Display,EZ_NTILEGC2,bitmap->pixmap);
		XSetTSOrigin(EZ_Display, EZ_NTILEGC2, 0, 0);
	      }
	    else
	      {
		unsigned long junkpv;
		int tile = EZ_GetBackgroundGC(wptr,&gc, &junkpv, 0, 0);
		if(tile) { XSetTSOrigin(EZ_Display, gc, 0, 0);}
	      }  
	  }
      	XFillRectangle(EZ_Display, pixmap, gc, x,y, ww, hh); 	
	EZ_DrawRectBorderWithSize(wptr, pixmap, x,y, ww,hh, 
				  bwidth, EZ_BORDER_UP,0,0,1);
      }
      break;
    case EZ_TICK_INDICATOR: 
      {
	GC gc;
	int x, y, ww, hh, padb, padx, bw;
	ww = EZ_WidgetAuxXLeft(wptr) - 6;
	hh = ww + 1;
	padb = EZ_WidgetPadB(wptr);
	padx = EZ_WidgetPadX(wptr);
	bw = EZ_WidgetBorderWidth(wptr);
	x =  padx + bw + padb;
	y =  (EZ_WidgetHeight(wptr) - hh) >> 1;

	if(EZ_WidgetBGPixmapB(wptr) != NULL)
	  {
	    EZ_Bitmap *bitmap = EZ_WidgetBGPixmapB(wptr);
	    gc = EZ_NTILEGC2;
	    XSetTile(EZ_Display,EZ_NTILEGC2,bitmap->pixmap);
	    XSetTSOrigin(EZ_Display, EZ_NTILEGC2, 0, 0);
	  }
	else if(EZ_WidgetParentBGPixmapB(wptr) != NULL)
	  {
	    EZ_Bitmap *bitmap = EZ_WidgetParentBGPixmapB(wptr);
	    gc = EZ_NTILEGC2;
	    XSetTile(EZ_Display,EZ_NTILEGC2,bitmap->pixmap);
	    XSetTSOrigin(EZ_Display, EZ_NTILEGC2, 0, 0);
	  }
	else
	  {
	    unsigned long junkpv;
	    int tile = EZ_GetBackgroundGC(wptr,&gc, &junkpv, 0, 0);
	    if(tile) 
	      {
		int ox, oy;
		EZ_GetWidgetTileOrigin(wptr, &ox, &oy);      
		XSetTSOrigin(EZ_Display, gc, ox, oy);
	      }
	  }
	XFillRectangle(EZ_DisplayForWidgets, pixmap, gc, x,y, ww, ww); 
	EZ_DrawRectBorderWithSize(wptr, pixmap, x,y, ww,hh, bwidth, EZ_BORDER_DOWN,0,0,1);	
	if(onoff)
	  {
	    XPoint points[8];
	    x += (ww - 16)/2;
	    y += (hh - 16)/2;
	    points[0].x = 2+x;  points[0].y = 8+y;
	    points[1].x = 8+x;  points[1].y = 13+y;
	    points[2].x = 14+x; points[2].y = 4+y;
	    points[3].x = 15+x; points[3].y = 1+y;
	    points[4].x = 13+x; points[4].y = 2+y;
	    points[5].x = 8+x;  points[5].y = 8+y;
	    points[6].x = 6+x;  points[6].y = 6+y;
	    points[7].x = 5+x;  points[7].y = 5+y;
	    if(color == 0) gc = EZ_DARKREDGC;
	    else
	      {
		XSetForeground(EZ_Display, EZ_WRITABLEGC, color);	
		gc = EZ_WRITABLEGC;
	      }
	    XFillPolygon(EZ_Display, pixmap, gc, points, 8, Nonconvex, CoordModeOrigin); 
	  }
      }
      break;
    default:
      break;
    }
}
/***************************************************************************/
#undef _EZ_INDICATOR_C_
