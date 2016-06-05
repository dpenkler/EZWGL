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
 ***              Scroll Bar                                   ***
 ***                                                           ***
 *****************************************************************/
/*
 * Scrollbar is not a widget by itself. It is used to control
 * the display of other widgets (currently listbox and browser).
 */
#define _EZ_WIDGET_SCROLL_C_

#include "EZ_Widget.h"
/*********************************************************************
 * 
 *  Functions implemented in this file:
 */
EZ_Widget        *EZ_CreateScrollbar MY_ANSIARGS((EZ_Widget *widget, int type));
void             EZ_UpdateScrollbar MY_ANSIARGS((EZ_Widget *widget, int maxs, int pags, int start));
void             EZ_GetScrollbarState MY_ANSIARGS((EZ_Widget *widget, int *ms, int *ps, int *st));
int              EZ_SetScrollbarDiscreteSpeed MY_ANSIARGS((EZ_Widget *widget, int speed));
int              EZ_SetHScrollbarDiscreteSpeed MY_ANSIARGS((EZ_Widget *widget, int speed));
int              EZ_SetVScrollbarDiscreteSpeed MY_ANSIARGS((EZ_Widget *widget, int speed));

void             EZ_UpdateScrollbarI MY_ANSIARGS((EZ_Widget *widget, int maxs, int pags, int start));
void             EZ_DoHScroll MY_ANSIARGS((EZ_Widget *widget, int offset, int flag));
void             EZ_DoVScroll  MY_ANSIARGS((EZ_Widget *widget, int offset, int flag));
void             EZ_DoScroll   MY_ANSIARGS((EZ_Widget *widget, int offset, int flag));

void             EZ_ComputeWidgetHScrollBarSize  MY_ANSIARGS((EZ_Widget *widget, int *w, int *h));
void             EZ_ComputeWidgetVScrollBarSize  MY_ANSIARGS((EZ_Widget *widget, int *w, int *h));
void             EZ_DrawWidgetHScrollBar  MY_ANSIARGS((EZ_Widget *widget));
void             EZ_DrawWidgetVScrollBar  MY_ANSIARGS((EZ_Widget *widget));
void             EZ_HScrollBarEventHandle MY_ANSIARGS((EZ_Widget *widget, XEvent *event));
void             EZ_VScrollBarEventHandle MY_ANSIARGS((EZ_Widget *widget, XEvent *event));

/*********************************************************************
 * 
 *  Local Functions
 */
static void      EZ_UpdateHScrollBar  MY_ANSIARGS((EZ_Widget *widget));
static void      EZ_UpdateVScrollBar  MY_ANSIARGS((EZ_Widget *widget));

/*********************************************************************
 * 
 *  Local Variables.
 */
static EZ_WidgetHandle EZ_HScrollBarHandle =
{
  EZ_ComputeWidgetHScrollBarSize,
  EZ_DrawWidgetHScrollBar,
  EZ_FreeUnknownData,
  EZ_HScrollBarEventHandle,
};

static EZ_WidgetHandle EZ_VScrollBarHandle =
{
  EZ_ComputeWidgetVScrollBarSize,
  EZ_DrawWidgetVScrollBar,
  EZ_FreeUnknownData,
  EZ_VScrollBarEventHandle,
};

/*********************************************************************/

EZ_Widget  *EZ_CreateScrollbar(parent, type)
     EZ_Widget  *parent;     /* parent widget    */
     int        type;
{
  EZ_Widget  *wptr;

  wptr = EZ_CreateNewWidget(parent);
  /*--------------------------------------------------
   * Register the handling functions for ...
   *  has to be done after  EZ_CreateNewWidget.
   *-------------------------------------------------*/
  if(type == EZ_WIDGET_HORIZONTAL_SCROLLBAR)
    {
      EZ_WidgetHandlingFunctions[EZ_WIDGET_HORIZONTAL_SCROLLBAR] = &EZ_HScrollBarHandle;
      EZ_SetWidgetHeightSetFlag(wptr);
    }
  else if(type == EZ_WIDGET_VERTICAL_SCROLLBAR)
    {
      EZ_WidgetHandlingFunctions[EZ_WIDGET_VERTICAL_SCROLLBAR] = &EZ_VScrollBarHandle;
      EZ_SetWidgetWidthSetFlag(wptr);
    }
  else 
    {
      (void)fprintf(stderr,"In EZ_CreateScrollBar, invalid widget type\n");
      return(NULL);
    }

  EZ_SetWidgetTypeAndNames(wptr,type);
  EZ_WidgetBorderStyle(wptr)  = EZ_BORDER_DOWN;
  EZ_WidgetBorderWidth(wptr)  = 2;
  EZ_WidgetPadX(wptr) = 0;
  EZ_WidgetPadY(wptr) = 0;
  EZ_WidgetPadB(wptr) = 2;  

  EZ_ScrollObject(wptr) = (EZ_Widget *)NULL;

  EZ_SetWidgetExpandFlag(wptr);
  /* border of the scroll bar and the arrows */
  EZ_ScrollBorderWidth(wptr) = 2;
  EZ_ScrollBorderStyle(wptr) = EZ_BORDER_UP;
  EZ_ScrollAW1BorderStyle(wptr) = EZ_BORDER_UP;
  EZ_ScrollAW2BorderStyle(wptr) = EZ_BORDER_UP;

  EZ_ScrollSSize(wptr) = 10;
  EZ_ScrollLSize(wptr) = 100;  /* to be computed */

  EZ_ScrollPosition(wptr) = 0;

  EZ_ScrollTotalSize(wptr) = 1;
  EZ_ScrollPageSize(wptr) = 1;
  EZ_ScrollObjectBegin(wptr) = 0;
  EZ_ScrollObjectEnd(wptr) = 0;
  EZ_ScrollFIncrement(wptr)  = 1.0;
  EZ_ScrollIIncrement(wptr)  = 1;  
  EZ_ScrollIIFactor(wptr)  = 1;  
  EZ_ScrollDirty(wptr)  = 1;

  EZ_SetWidgetFocusableFlag(wptr);  

  return(wptr);
}

/********************************************************************
 *
 *  Figure out the dimension of a H scrollbar.
 */ 
 
void EZ_ComputeWidgetHScrollBarSize(widget, w, h)
     EZ_Widget *widget;
     int       *w, *h;
{
  int width, cw, ch, padb;

  if(!(EZ_GetWidgetSizeComputedFlag(widget)))
    { 
      /*if(EZ_ScrollSSize(widget) < (EZ_ScrollBorderWidth(widget)<<1) + 6)*/
	EZ_ScrollSSize(widget) = (EZ_ScrollBorderWidth(widget)<<1) + 6;
      
      width = EZ_ScrollArrowSize(widget) = 
	EZ_ScrollSSize(widget) + (EZ_ScrollBorderWidth(widget)<<1);

      EZ_SetWidgetSizeComputedFlag(widget);
    }

  width = EZ_ScrollSSize(widget) + (EZ_ScrollBorderWidth(widget)<<1);
  cw = EZ_WidgetPadX(widget) + EZ_WidgetBorderWidth(widget);
  ch = EZ_WidgetPadY(widget) + EZ_WidgetBorderWidth(widget);

  cw = width + ((cw + width)<<1);  /* just make sure the two arrows can fit in */
  ch = ch + ch + width;
  padb = EZ_WidgetPadB(widget) << 1;

  *w = cw + padb;
  *h = ch + padb;
}

/****************************************************************************
 *
 *  Draw a ScrollBar widget
 */
void  EZ_DrawWidgetHScrollBar(wptr)
     EZ_Widget *wptr;
{
  int             w, h,x,y,xx,yy,xxx,yyy, ww, hh, itmp, haw1, haw2;
  int             offset, padb, padb2, tile, highlight, swapBg;
  Pixmap          pixmap;
  Window          win;
  GC              gc, gc1;
  unsigned long   bgpv;

  if(EZ_ScrollDirty(wptr))
    {
      EZ_UpdateHScrollBar(wptr);
      EZ_ScrollDirty(wptr) = 0;
    }

  win = EZ_WidgetWindow(wptr);
  w   = EZ_WidgetWidth(wptr);
  h   = EZ_WidgetHeight(wptr);
  padb = EZ_WidgetPadB(wptr);
  padb2 = padb + padb;
  /*-----------------------------------------------------------
   *  Create a pixmap, draw into this pixmap in background and
   *  copy to the button window when finished.
   *----------------------------------------------------------*/
  pixmap = XCreatePixmap(EZ_Display, win, w, h, EZ_DepthForWidgets);    
  EZ_FixFocusHighlightBorder(wptr, pixmap, w, h, padb);

  /*----------------------------------------------------------
   * Fill the background and draw the border.
   *---------------------------------------------------------*/
  tile = EZ_GetBackgroundGC(wptr, &gc, &bgpv, 0, 0);  
  if(tile)
    {
      int ox, oy;
      EZ_GetWidgetTileOrigin(wptr, &ox, &oy);      
      XSetTSOrigin(EZ_Display, gc, ox, oy);
    }
  XFillRectangle(EZ_Display, pixmap, gc, padb,padb, w-padb2, h-padb2);   
  /*----------------------------------------------------------
   * Draw the scroll button
   *---------------------------------------------------------*/
  xx = EZ_WidgetPadX(wptr) + EZ_WidgetBorderWidth(wptr) + padb;
  yy = EZ_WidgetBorderWidth(wptr) + EZ_WidgetPadY(wptr) + padb;

  x = EZ_ScrollPosition(wptr);
  y = yy;

  ww = EZ_ScrollLSize(wptr) + (EZ_ScrollBorderWidth(wptr)<<1);
  hh = EZ_ScrollSSize(wptr) + (EZ_ScrollBorderWidth(wptr)<<1);
  swapBg = EZ_ScrollHighlight(wptr);
  highlight = EZ_GetWidgetHighlightFlag(wptr);

  if(swapBg) { EZ_InstallHighlightColor(wptr); } /*4/29/99*/

  if(EZ_WidgetBGPixmapB(wptr) != NULL)
    {
      EZ_Bitmap *bitmap = EZ_WidgetBGPixmapB(wptr);
      gc = EZ_NTILEGC2;
      if(highlight) XSetTile(EZ_Display,EZ_NTILEGC2,bitmap->htpixmap);
      else XSetTile(EZ_Display,EZ_NTILEGC2,bitmap->pixmap);
      XSetTSOrigin(EZ_Display, EZ_NTILEGC2, x, y);
    }
  else if(EZ_WidgetParentBGPixmapB(wptr) != NULL)
    {
      EZ_Bitmap *bitmap = EZ_WidgetParentBGPixmapB(wptr);
      gc = EZ_NTILEGC2;
      if(highlight) XSetTile(EZ_Display,EZ_NTILEGC2,bitmap->htpixmap);
      else XSetTile(EZ_Display,EZ_NTILEGC2,bitmap->pixmap);
      XSetTSOrigin(EZ_Display, EZ_NTILEGC2, x, y);
    }
  else
    {
      int tile = EZ_GetBackgroundGC(wptr,&gc, &bgpv, 0, highlight);
      if(tile) 
	{
	  int ox, oy;
	  EZ_GetWidgetTileOrigin(wptr, &ox, &oy);      
	  XSetTSOrigin(EZ_Display, gc, ox, oy);
	}
    }  
  XFillRectangle(EZ_DisplayForWidgets, pixmap, gc, x,y, ww, hh); 
  offset = (EZ_ScrollBorderStyle(wptr) == EZ_BORDER_DOWN? 1 : 0);
  yyy = y + EZ_ScrollBorderWidth(wptr);
  xxx = x - 1 + (ww>>1);

  if(EZ_ScrollStyle(wptr))
    {
      int iw = EZ_ScrollLSize(wptr);
      int ih = EZ_ScrollSSize(wptr);
      int ix = x + EZ_ScrollBorderWidth(wptr) + offset;
      iw = (iw>>1) -2;
      EZ_OverlayDragablePixmap(wptr, pixmap, ix, yyy, iw, ih);
      EZ_OverlayDragablePixmap(wptr, pixmap, ix+iw+4, yyy, iw, ih);
    }  
  
  EZ_SetupUpBdGC(wptr, &gc1, &gc1, &gc1, &gc1,&gc, &gc, x, y, 1);
  /* EZ_GetDarkBrightNormalBDGC(wptr, &gc, &gc1, &junkgc);*/
  XDrawLine(EZ_DisplayForWidgets,pixmap, gc,
	    xxx + offset, yyy,
	    xxx + offset, yyy + EZ_ScrollSSize(wptr));
  XDrawLine(EZ_DisplayForWidgets,pixmap, gc1,
	    xxx+2 + offset, yyy,
	    xxx+2 + offset, yyy + EZ_ScrollSSize(wptr));
  EZ_DrawRectBorderWithSize(wptr, pixmap,  x, y,  ww, hh,
			    EZ_ScrollBorderWidth(wptr), EZ_ScrollBorderStyle(wptr),x,y,1);

  if(swapBg) { EZ_UninstallHighlightColor(wptr); } /*4/29/99*/  

  /*----------------------------------------------------------
   * Draw the two arrows
   *---------------------------------------------------------*/
  itmp = EZ_ScrollArrowSize(wptr);

  haw1 = EZ_ScrollAW1Highlight(wptr) && EZ_HighlightEnabled;
  if(haw1) { EZ_InstallHighlightColor(wptr); } /*4/29/99*/  
  EZ_DrawHLArrow(wptr,pixmap, xx, yy,
		 itmp,
		 EZ_ScrollAW1BorderStyle(wptr),
		 EZ_ScrollBorderWidth(wptr),haw1);
  if(haw1) { EZ_UninstallHighlightColor(wptr); } /*4/29/99*/    

  haw2 = EZ_ScrollAW2Highlight(wptr) && EZ_HighlightEnabled;
  if(haw2) { EZ_InstallHighlightColor(wptr); } /*4/29/99*/  

  EZ_DrawHRArrow(wptr,pixmap, EZ_WidgetWidth(wptr) - xx - itmp,  yy, 
		 itmp,
		 EZ_ScrollAW2BorderStyle(wptr),
		 EZ_ScrollBorderWidth(wptr), haw2);
  if(haw2) { EZ_UninstallHighlightColor(wptr); } /*4/29/99*/    

  EZ_DrawRectBorder(wptr, pixmap);
  XCopyArea(EZ_DisplayForWidgets,pixmap,win, EZ_WRITABLEGC,0,0,w,h,0,0); 
  XFreePixmap(EZ_DisplayForWidgets, pixmap); 
}

/*************************************************************************
 *
 *  Event handling
 */
void  EZ_HScrollBarEventHandle(widget, event)
     EZ_Widget *widget;
     XEvent          *event;
{
  int     x, y, xbgn, ybgn, xbgn2, ybgn2, arrowsize, tmp, tmp1, done;
  int     onarrow1 = 0, onarrow2 = 0, onbutton = 0, first_time = 1, offset=0;
  int     butn,wox,woy;
  Window  junkwin;
  XEvent  xevent;

  if(widget == (EZ_Widget *)NULL) return;
  if(EZ_GetWidgetDisabledFlag(widget) )  return;
  if(event->type == Expose)
    {
      EZ_ScrollDirty(widget) = 1;
      EZ_DrawWidget(widget);  
      return;
    }
  if(event->type == ButtonPress)
    {
      butn = event->xbutton.button;
      if(butn == EZ_Btn3)
	{
	  EZ_HandleDnDEvents(widget, event);
	  return;
	}
    }

  switch(event->type)
    {
    case MotionNotify:
      x = event->xmotion.x;
      y = event->xmotion.y;
      /* check if pointer is on the arrows */
      tmp = EZ_WidgetBorderWidth(widget) + EZ_WidgetPadB(widget);
      xbgn = tmp + EZ_WidgetPadX(widget);
      ybgn = tmp + EZ_WidgetPadY(widget);
      arrowsize = EZ_ScrollArrowSize(widget);
      xbgn2 = EZ_WidgetWidth(widget) - xbgn - arrowsize;
      ybgn2 = ybgn;

      tmp = EZ_ScrollLSize(widget) + (EZ_ScrollBorderWidth(widget)<<1);
      tmp1 = EZ_ScrollPosition(widget);

      if( x > xbgn && x < xbgn + arrowsize && y > ybgn && y < ybgn + arrowsize)
	{
	  /* pointer is on the left arrow */
          if(EZ_ScrollAW1Highlight(widget) == 0)
            {
              EZ_ScrollAW1Highlight(widget) = 1;
              EZ_DrawWidget(widget);
            }
	}
      else if(x > xbgn2 && x < xbgn2 + arrowsize && y > ybgn2 && y < ybgn2 + arrowsize)
	{
	  /* pointer is on the left arrow */
          if(EZ_ScrollAW2Highlight(widget) == 0)
            {
              EZ_ScrollAW2Highlight(widget) = 1;
              EZ_DrawWidget(widget);
            }
	}
      else if(x > tmp1 && x < tmp1 + tmp && y > ybgn && y < ybgn + arrowsize)
	{
          if(EZ_ScrollHighlight(widget) == 0)
            {
              EZ_SetWidgetHighlightFlag(widget);
              EZ_ScrollHighlight(widget) = 1;
              EZ_DrawWidget(widget);
            }
	}
      else
	{
	  if( EZ_ScrollAW1Highlight(widget) ||  EZ_ScrollAW2Highlight(widget)
	     || EZ_GetWidgetHighlightFlag(widget))
	    {
              EZ_ScrollAW1Highlight(widget) = 0;
              EZ_ScrollAW2Highlight(widget) = 0;
              EZ_ScrollHighlight(widget) = 0;
              EZ_ClearWidgetHighlightFlag(widget);
              EZ_DrawWidget(widget);	      
	    }
	}
      break;
    case EnterNotify:
      break;
    case LeaveNotify:
      EZ_ScrollAW1Highlight(widget) = 0;
      EZ_ScrollAW2Highlight(widget) = 0;
      EZ_ScrollHighlight(widget) = 0;
      EZ_ClearWidgetHighlightFlag(widget);
      EZ_DrawWidget(widget);	      
      break;
    case ButtonPress:  /* must be button 1 */
      butn = event->xbutton.button;
      x = event->xbutton.x;
      y = event->xbutton.y;
      /* check if pointer is on the arrows */
      tmp = EZ_WidgetBorderWidth(widget) + EZ_WidgetPadB(widget);
      xbgn = tmp + EZ_WidgetPadX(widget);
      ybgn = tmp + EZ_WidgetPadY(widget);
      arrowsize = EZ_ScrollArrowSize(widget);
      xbgn2 = EZ_WidgetWidth(widget) - xbgn - arrowsize;
      ybgn2 = ybgn;
      
      if( x > xbgn && x < xbgn + arrowsize && y > ybgn && y < ybgn + arrowsize)
	{
	  /* pointer is on the left arrow */
	  EZ_ScrollAW1Highlight(widget) = 1;
	  if(EZ_DepresionDisabled == 0)
	    EZ_ScrollAW1BorderStyle(widget) = EZ_BORDER_DOWN;
	  onarrow1 = 1;
	}
      else if(x > xbgn2 && x < xbgn2 + arrowsize && y > ybgn2 && y < ybgn2 + arrowsize)
	{
	  /* pointer is on the left arrow */
	  EZ_ScrollAW2Highlight(widget) = 1;
	  if(EZ_DepresionDisabled == 0)
	    EZ_ScrollAW2BorderStyle(widget) = EZ_BORDER_DOWN;
	  onarrow2 = 1;
	}
      else
	{
	  tmp = EZ_ScrollLSize(widget) + (EZ_ScrollBorderWidth(widget)<<1);
	  tmp1 = EZ_ScrollPosition(widget);
	  if(x > tmp1 && x < tmp1 + tmp && y > ybgn && y < ybgn + arrowsize)
	    {
	      EZ_SetWidgetHighlightFlag(widget);
              EZ_ScrollHighlight(widget) = 1;
	      if(EZ_DepresionDisabled == 0)
		EZ_ScrollBorderStyle(widget) = EZ_BORDER_DOWN;
	      onbutton = 1;
	      offset  = x - EZ_ScrollPosition(widget);
	    } 
	}
      if(!(onarrow1 | onarrow2 | onbutton) && butn == EZ_Btn2 &&
	 (x > xbgn && y > ybgn && x < xbgn2 && y < EZ_WidgetHeight(widget)-ybgn))
	{
	  int oldx = EZ_ScrollPosition(widget);  
	  tmp = EZ_ScrollLSize(widget) + (EZ_ScrollBorderWidth(widget)<<1);
	  tmp1 = tmp>>1;

	  tmp = x - oldx - tmp1;
	  offset  =  tmp1;
	  tmp1 = (int) (tmp * EZ_ScrollFIncrement(widget) - 0.9);
	  EZ_DoHScroll(widget, tmp1, 0);
	  EZ_SetWidgetHighlightFlag(widget);
          EZ_ScrollHighlight(widget) = 1;
	  if(EZ_DepresionDisabled == 0)
	    EZ_ScrollBorderStyle(widget) = EZ_BORDER_DOWN;
	  onbutton = 1;
	}
      EZ_DrawWidget(widget);
      if(onarrow1 | onarrow2 | onbutton)
	{
	  XTranslateCoordinates(EZ_Display, EZ_WidgetWindow(widget),
				RootWindow(EZ_Display, EZ_ScreenNum),
				(int)(EZ_ScrollBgnPosition(widget)), 6,
				&wox,&woy, &junkwin);
	  done = 0;
	  xevent.type = 0;
	  while(!done) /* wait for a matching ButtonRelease */
	    { 
	      if(!done)
		{
		  if(onarrow1)      EZ_DoHScroll(widget, -EZ_ScrollIIncrement(widget), 0);
		  else if(onarrow2) EZ_DoHScroll(widget, EZ_ScrollIIncrement(widget), 0);
		  if(first_time == 1)
		    {
		      if(onarrow1 | onarrow2) 		  
			{
			  XFlush(EZ_Display);
			  EZ_SitForALittleBit(200000);
			}
		      first_time = 0;
		    }
		}
	      if( onbutton )
		{
		  XNextEvent(EZ_Display, &xevent); 
		  EZ_FilterEvent(&xevent);
                  EZ_InvokePrivateEventHandler(&xevent);
		  if(xevent.xany.window != EZ_WidgetWindow(widget))
		    EZ_WidgetDispatchEvent(&xevent);  
		}
	      else  EZ_SitForALittleBit(30000); 

	      while(XPending(EZ_Display))
		{
		  if(xevent.type == ButtonRelease && xevent.xbutton.button == butn) break;
		  else if(xevent.type == Expose || xevent.type == FocusIn || xevent.type == FocusOut)    
		    EZ_WidgetDispatchEvent(&xevent);  
		  XNextEvent(EZ_Display, &xevent);
		  EZ_FilterEvent(&xevent);
                  EZ_InvokePrivateEventHandler(&xevent);
		  if(xevent.xany.window != EZ_WidgetWindow(widget))
		    EZ_WidgetDispatchEvent(&xevent);  
		}

	      if(xevent.type == ButtonRelease)
		{
		  if(xevent.xbutton.button == butn)
		    {
		      EZ_ScrollAW1BorderStyle(widget) = EZ_BORDER_UP;
		      EZ_ScrollAW2BorderStyle(widget) = EZ_BORDER_UP;
		      EZ_ScrollBorderStyle(widget) = EZ_BORDER_UP;
		      x = xevent.xbutton.x;
		      y = xevent.xbutton.y;
		      if(onarrow1)
			{
			  if(!(x> xbgn && x< xbgn + arrowsize && y > ybgn && y < ybgn + arrowsize))
			    EZ_ScrollAW1Highlight(widget) = 0;
			}
		      else if(onarrow2)
			{
			  if(!(x>xbgn2 && x< xbgn2 + arrowsize && y> ybgn2 && y< ybgn2 + arrowsize))
			    EZ_ScrollAW2Highlight(widget) = 0;			
			}
		      else if(onbutton)
			{
			  tmp = EZ_ScrollLSize(widget) + (EZ_ScrollBorderWidth(widget)<<1);
			  tmp1 = EZ_ScrollPosition(widget);
			  if(!(x > tmp1 && x < tmp1 + tmp && y > ybgn && y < ybgn + arrowsize))
                            {
                              EZ_ClearWidgetHighlightFlag(widget);
                              EZ_ScrollHighlight(widget) = 0;
                            }
			}
		      EZ_DrawWidget(widget);
		      done = 1;
		    }
		}
	      if(!done && onbutton)
		{
		  int             rx,ry,x,y;
		  unsigned int    mask;
		  Window          root,win;
		  XQueryPointer(EZ_Display, RootWindow(EZ_Display, EZ_ScreenNum),
				&root,
				&win, 
				&rx, &ry, 
				&x,&y, 
				&mask );

		  tmp = rx - wox  - offset;
		  if(tmp < 0) tmp =  0;
		  else 
		    {
		      tmp1 = EZ_ScrollEndPosition(widget) - EZ_ScrollBgnPosition(widget) -
			(EZ_ScrollLSize(widget) + (EZ_ScrollBorderWidth(widget)<<1));
		      if(tmp > tmp1) tmp = tmp1;
		    }
		  tmp1 = (int) (tmp * EZ_ScrollFIncrement(widget) - 0.9);
		  EZ_DoHScroll(widget, tmp1, 1);		
		}
	    }
	}
      else 
	{
	  /* must be Button1 press and Press is not on the slider  */
	  /* scroll towards the pointer */
	  int yend = EZ_WidgetHeight(widget)-ybgn;
	  if(x > xbgn && x < xbgn2 && y > ybgn && y < yend)
	    {
	      int lastposition = -10000;
	      done = 0;
	      tmp = EZ_ScrollLSize(widget) + (EZ_ScrollBorderWidth(widget)<<1) -2;
	      tmp = (x < EZ_ScrollPosition(widget)) ? -tmp : tmp;

	      while(!done) /* wait for a matching ButtonRelease */
		{ 
		  int pos = EZ_ScrollPosition(widget);
		  if(pos != lastposition)
		    {
		      lastposition = pos;
		      tmp1 = (int) (tmp * EZ_ScrollFIncrement(widget));
		      EZ_DoHScroll(widget, tmp1, 0);     
		    }
		  if(first_time)
		    {
		      first_time = 0;
		      XFlush(EZ_Display);
		      EZ_SitForALittleBit(300000); 
		    }
		  else EZ_SitForALittleBit(30000); 

		  while(XPending(EZ_Display))
		    {
		      XNextEvent(EZ_Display, &xevent);
		      EZ_FilterEvent(&xevent);
                      EZ_InvokePrivateEventHandler(&xevent);
		      if(xevent.xany.window != EZ_WidgetWindow(widget))
			EZ_WidgetDispatchEvent(&xevent);
		      if(xevent.type == ButtonRelease && xevent.xbutton.button == butn)
			{
			  done = 1;
			  break;
			}
		    }
		}
	    }
	}
      break;
    case KeyPress:
      {
	char              tmpbuffer[8];
	int               buffersize = 8, move_unit;
	KeySym            keysym;
	XComposeStatus    compose; 
	int count = XLookupString(&(event->xkey), tmpbuffer, buffersize, &keysym, &compose);
	tmpbuffer[count] = '\0';

	switch(keysym)
	  {
	  case XK_Left: case XK_b:  case XK_h: case XK_B:  case XK_H:  
#ifdef XK_KP_Left
	  case XK_KP_Left:
#endif
	    move_unit = -EZ_ScrollIIncrement(widget);
	    if(event->xkey.state & ShiftMask) move_unit = 1;
	    else if(event->xkey.state & ControlMask) move_unit = move_unit * 4;
	    if(event->xkey.state & Mod1Mask) move_unit *= 4 ;
	    EZ_DoHScroll(widget, move_unit, 0);	    
	    break;
	  case XK_Right: case XK_f: case XK_l: case XK_F: case XK_L:  
#ifdef XK_KP_Right
	  case XK_KP_Right:
#endif
	    move_unit = EZ_ScrollIIncrement(widget);
	    if(event->xkey.state & ShiftMask)  move_unit = 1;
	    else if(event->xkey.state & ControlMask) move_unit = move_unit * 4;
	    if(event->xkey.state & Mod1Mask) move_unit *= 4 ;
	    EZ_DoHScroll(widget, move_unit, 0);
	    break;
	  default:
	    break;
	}
	{ 
	  XEvent tmpevent;
	  while(XCheckTypedEvent(EZ_Display, KeyPress, &tmpevent))
	    if(EZ_FilterEvent(&tmpevent))
	      EZ_InvokePrivateEventHandler(&tmpevent);
	}
      }
      break;
    default:
      break;
    }
}
/**************************************************************************************/
static void  EZ_UpdateHScrollBar(widget)
     EZ_Widget *widget;
{
  int    itmp,itmp1;
  float  ftmp, ftmp1,ftmp2;
  int    maxsize, pagesize, start;

  if(widget == (EZ_Widget *)NULL || EZ_WidgetType(widget) != EZ_WIDGET_HORIZONTAL_SCROLLBAR)
    return;

  maxsize = EZ_ScrollTotalSize(widget);
  pagesize = EZ_ScrollPageSize(widget);
  start = EZ_ScrollObjectBegin(widget);

  EZ_ScrollArrowSize(widget) = EZ_ScrollSSize(widget) + (EZ_ScrollBorderWidth(widget)<<1);  
  EZ_ScrollBgnPosition(widget) = EZ_ScrollArrowSize(widget) + EZ_ScrollBorderWidth(widget) +
    EZ_WidgetBorderWidth(widget) + EZ_WidgetPadX(widget) + EZ_WidgetPadB(widget);
  EZ_ScrollEndPosition(widget) = EZ_WidgetWidth(widget) - EZ_ScrollBgnPosition(widget);

  if(pagesize < maxsize)
    {
      /* computer the size of the scroll bar */
      ftmp = (float)(EZ_ScrollEndPosition(widget) - EZ_ScrollBgnPosition(widget));
      ftmp1 = (float)pagesize/(float)maxsize;
      ftmp2 = ftmp1 * ftmp;
      EZ_ScrollLSize(widget) = (int)ftmp2  - (EZ_ScrollBorderWidth(widget)<<1);
      if(EZ_ScrollLSize(widget) < 10) EZ_ScrollLSize(widget) =  10;

      ftmp1 = ((float)(EZ_ScrollLSize(widget)+ (EZ_ScrollBorderWidth(widget)<<1)) - ftmp);
      if(ftmp1 == 0.0) ftmp1 = 1.0;
      EZ_ScrollFIncrement(widget)  = ftmp = (float)(maxsize - pagesize)/ftmp1;
      itmp = (int)( 2.0 * EZ_ScrollFIncrement(widget));
      itmp = (itmp == 0? (ftmp < 0.0 ? -1 : 1) : itmp );
      EZ_ScrollIIncrement(widget)  = itmp * EZ_ScrollIIFactor(widget);
      itmp = (int) ((float)start /  EZ_ScrollFIncrement(widget))
	+ EZ_ScrollBgnPosition(widget);
      itmp1 = EZ_ScrollLSize(widget) + (EZ_ScrollBorderWidth(widget)<<1);
      if(itmp < EZ_ScrollBgnPosition(widget)) itmp = EZ_ScrollBgnPosition(widget);
      else if(itmp > EZ_ScrollEndPosition(widget) - itmp1)
	itmp = EZ_ScrollEndPosition(widget) - itmp1;
      EZ_ScrollPosition(widget) = itmp;
    }
  else
    {
      EZ_ScrollLSize(widget) = EZ_ScrollEndPosition(widget) - EZ_ScrollBgnPosition(widget)
	- (EZ_ScrollBorderWidth(widget)<<1);
      EZ_ScrollFIncrement(widget)  = 0.0;
      EZ_ScrollIIncrement(widget)  = 0;
      EZ_ScrollPosition(widget) =  EZ_ScrollBgnPosition(widget);
    }
}
/**************************************************************************************/
void  EZ_DoScroll(widget,  offset, flag)
     EZ_Widget *widget;
     int       offset, flag;
{
  if(widget)
    {
      int type = EZ_WidgetType(widget);
      if(type == EZ_WIDGET_VERTICAL_SCROLLBAR)
        EZ_DoVScroll(widget, offset, flag);
      else if(type == EZ_WIDGET_HORIZONTAL_SCROLLBAR)
	EZ_DoHScroll(widget, offset, flag);
    }
}
/**************************************************************************************/
void EZ_DoHScroll(widget, offset, flag)
     EZ_Widget *widget;
     int       offset, flag;
{
  EZ_Widget *obj;
  int itmp;

  if(widget == (EZ_Widget *)NULL || EZ_WidgetType(widget) != EZ_WIDGET_HORIZONTAL_SCROLLBAR)
    return;

  if(EZ_WidgetCallBackFunc(widget))
    {
      int tmp = (flag == 0? (EZ_ScrollObjectBegin(widget) + offset) : offset);
      EZ_UpdateScrollbarI(widget, EZ_ScrollTotalSize(widget), EZ_ScrollPageSize(widget), tmp);
      { EZ_ExecuteWidgetCallBacks(widget);}
      return;
    }
  /*
   * for internal use only
   */
  obj = EZ_ScrollObject(widget);
  if( obj == (EZ_Widget *)NULL)  return;

  switch(EZ_WidgetType(obj))
    {
    case EZ_WIDGET_ILIST_BOX:
      {
	itmp = (flag == 0 ? (EZ_IListBoxHStart(obj) + offset): offset);
	if(itmp >= 0) itmp = 0;
	else if( itmp < EZ_IListBoxHPageSize(obj) - EZ_IListBoxMaxHSize(obj))
	  itmp =  EZ_IListBoxHPageSize(obj) - EZ_IListBoxMaxHSize(obj);

	if(EZ_IListBoxHStart(obj) != itmp)
	  {
	    EZ_IListBoxHStart(obj) = itmp;
	    if(EZ_WidgetWindow(obj) != (Window )NULL)
              EZ_DrawWidgetAndDescendants(obj, 0);
	  }
      }
      break;
    case EZ_WIDGET_ITEXT:
      {
	itmp = (flag == 0 ? (EZ_ITextHStart(obj) + offset): offset);
	if(itmp >= 0) itmp = 0;
	else if( itmp < EZ_ITextHPageSize(obj) - EZ_ITextMaxHSize(obj))
	  itmp = EZ_ITextHPageSize(obj) - EZ_ITextMaxHSize(obj);

	if(EZ_ITextHStart(obj) != itmp)
	  {
	    EZ_ITextHStart(obj) = itmp;
	    EZ_ITextHScrollInAction(obj) = 1;
            if(EZ_ITextEditable(obj)) EZ_ITextSelectionFlag(obj)=0;
	    if(EZ_WidgetWindow(obj) != (Window )NULL)
              EZ_DrawWidgetAndDescendants(obj, 0);
	    EZ_ITextHScrollInAction(obj) = 0;
	  }
      }
      break;
    case EZ_WIDGET_ITREE:
      {
	itmp = (flag == 0 ? (EZ_ITreeWXStart(obj) + offset): offset);
	if(itmp >= 0) itmp = 0;
	else if( itmp < EZ_ITreeWXPageSize(obj) - EZ_ITreeWXSize(obj))
	  itmp = EZ_ITreeWXPageSize(obj) - EZ_ITreeWXSize(obj);

	if(EZ_ITreeWXStart(obj) != itmp)
	  {
	    EZ_ITreeWXStart(obj) = itmp;
	    if(EZ_WidgetWindow(obj) != (Window )NULL)
              EZ_DrawWidgetAndDescendants(obj, 0);
	  }
      }
    break;
    case EZ_WIDGET_IFANCY_LIST_BOX:
      {
	itmp = (flag == 0 ? (EZ_IFListBoxXStart(obj) + offset): offset);
	if(itmp >= 0) itmp = 0;
	else if( itmp < EZ_IFListBoxXPageSize(obj) - EZ_IFListBoxXSize(obj))
	  itmp = EZ_IFListBoxXPageSize(obj) - EZ_IFListBoxXSize(obj);

	if(EZ_IFListBoxXStart(obj) != itmp)
	  {
	    EZ_IFListBoxXStart(obj) = itmp;
	    if(EZ_WidgetWindow(obj) != (Window )NULL)
              EZ_DrawWidgetAndDescendants(obj, 0);
	  }
      }
    break;
    case EZ_WIDGET_IWORK_AREA:
      {
	itmp = (flag == 0 ? (EZ_IWorkAreaXStart(obj) + offset): offset);
	if(itmp >= 0) itmp = 0;
	else if( itmp < EZ_IWorkAreaXPageSize(obj) - EZ_IWorkAreaXSize(obj))
	  itmp = EZ_IWorkAreaXPageSize(obj) - EZ_IWorkAreaXSize(obj);
        
	if(EZ_IWorkAreaXStart(obj) != itmp)
	  {
	    EZ_IWorkAreaXStart(obj) = itmp;
	    if(EZ_WidgetWindow(obj) != (Window )NULL)
              EZ_DrawWidgetAndDescendants(obj, 0);
	  }
      }
      break;
    case EZ_WIDGET_SPREAD_SHEET:
      {
        int ss = (int)EZ_SSheetX1(obj) - (int)EZ_SSheetX0(obj) -(int)EZ_SSheetNColumns(obj)+1;
	itmp = (flag == 0 ? (offset-(int)EZ_SSheetX0(obj)): offset);
	if(itmp >= 0) itmp = 0;
        if(itmp < ss) itmp=ss;
	if(EZ_SSheetX0(obj) != -itmp)
	  {
	    EZ_SSheetX0(obj) = -itmp;
            EZ_SSheetDirty(obj) = 1;
	    if(EZ_WidgetWindow(obj) != (Window )NULL)
              EZ_DrawWidgetAndDescendants(obj, 0);
	  }
      }
    break;
    default:
      break;
    }
}

/**************************************************************************************/
void EZ_DoVScroll(widget, offset, flag)
     EZ_Widget *widget;
     int       offset,flag;
{
  EZ_Widget *obj;
  int       itmp;

  if(widget == (EZ_Widget *)NULL || EZ_WidgetType(widget) != EZ_WIDGET_VERTICAL_SCROLLBAR)
    return;

  if(EZ_WidgetCallBackFunc(widget))
    {
      int tmp = (flag == 0? (EZ_ScrollObjectBegin(widget) + offset) : offset);
      EZ_UpdateScrollbarI(widget, EZ_ScrollTotalSize(widget), EZ_ScrollPageSize(widget),tmp);
      { EZ_ExecuteWidgetCallBacks(widget);}
      return;
    }
  /*
   * for internal use only
   */
  obj = EZ_ScrollObject(widget);
  if(obj == (EZ_Widget *)NULL) return;

  switch(EZ_WidgetType(obj))
    {
    case EZ_WIDGET_ITERM:
      {
        int total = EZ_ITermNScrolled(obj);
        int start = EZ_ITermViewStart(obj);

        start = total - start;
	itmp = (flag == 0 ? (start + offset) : offset);
        
        if(itmp > total) itmp = total;
	if(itmp <= 0) itmp = 0;	

	if(start != itmp)
	  {
	    EZ_ITermViewStart(obj) = (total-itmp);
	    if(EZ_WidgetWindow(obj) != (Window )NULL)
              EZ_DrawWidgetAndDescendants(obj, 0);
	  }
      }
      break;
    case EZ_WIDGET_ILIST_BOX:
      {
	itmp = (flag == 0 ? (EZ_IListBoxVStart(obj) + offset): offset);

	if(itmp > EZ_IListBoxNLines(obj) - EZ_IListBoxPageSize(obj))
	  itmp =  EZ_IListBoxNLines(obj) - EZ_IListBoxPageSize(obj);
	if(itmp <= 0) itmp = 0;	

	if(EZ_IListBoxFirstLine(obj) != itmp)
	  {
	    EZ_IListBoxFirstLine(obj) = itmp;
	    if(EZ_WidgetWindow(obj) != (Window )NULL)
              EZ_DrawWidgetAndDescendants(obj, 0);
	  }
      }
    break;
    case EZ_WIDGET_ITEXT:
      {
	itmp = (flag == 0 ? (EZ_ITextVStart(obj) + offset): offset);

	if(itmp > EZ_ITextNLines(obj) - EZ_ITextPageSize(obj))
	  itmp = EZ_ITextNLines(obj) - (EZ_ITextPageSize(obj));
	if(itmp <= 0) itmp = 0;	

	if(EZ_ITextFirstLine(obj) != itmp)
	  {
	    EZ_ITextFirstLine(obj) = itmp;
	    EZ_ITextYPosition(obj) = itmp;
	    EZ_ITextXPosition(obj) = 0;
            if(EZ_ITextEditable(obj)) EZ_ITextSelectionFlag(obj)=0;
	    if(EZ_WidgetWindow(obj) != (Window )NULL)
              EZ_DrawWidgetAndDescendants(obj, 0);
	  }
      }
    break;
    case EZ_WIDGET_ITREE:
      {
	itmp = (flag == 0 ? (-EZ_ITreeWYStart(obj) + offset): offset);
	if(itmp > EZ_ITreeWYSize(obj) - EZ_ITreeWYPageSize(obj))
	  itmp = EZ_ITreeWYSize(obj) - EZ_ITreeWYPageSize(obj);
	if(itmp <= 0) itmp = 0;	

	if(EZ_ITreeWYStart(obj) != -itmp)
	  {
	    EZ_ITreeWYStart(obj) = -itmp;
	    if(EZ_WidgetWindow(obj) != (Window )NULL)
              EZ_DrawWidgetAndDescendants(obj, 0);
	  }
      }
    break;
    case EZ_WIDGET_IFANCY_LIST_BOX:
      {
	itmp = (flag == 0 ? (-EZ_IFListBoxYStart(obj) + offset): offset);
	if(itmp > EZ_IFListBoxYSize(obj) - EZ_IFListBoxYPageSize(obj))
	  itmp = EZ_IFListBoxYSize(obj) - EZ_IFListBoxYPageSize(obj);
	if(itmp <= 0) itmp = 0;	

	if(EZ_IFListBoxYStart(obj) != -itmp)
	  {
	    EZ_IFListBoxYStart(obj) = -itmp;
	    if(EZ_WidgetWindow(obj) != (Window )NULL)
              EZ_DrawWidgetAndDescendants(obj, 0);
	  }
      }
    break;
    case EZ_WIDGET_IWORK_AREA:
      {
	itmp = (flag == 0 ? (-EZ_IWorkAreaYStart(obj) + offset): offset);
	if(itmp > EZ_IWorkAreaYSize(obj) - EZ_IWorkAreaYPageSize(obj))
	  itmp = EZ_IWorkAreaYSize(obj) - EZ_IWorkAreaYPageSize(obj);
	if(itmp <= 0) itmp = 0;	

	if(EZ_IWorkAreaYStart(obj) != -itmp)
	  {
	    EZ_IWorkAreaYStart(obj) = -itmp;
	    if(EZ_WidgetWindow(obj) != (Window )NULL)
              EZ_DrawWidgetAndDescendants(obj, 0);
	  }
      }
    break;
    case EZ_WIDGET_SPREAD_SHEET:
      {
        int ss = (int)EZ_SSheetNRows(obj)+(int)EZ_SSheetY0(obj)-(int)EZ_SSheetY1(obj)-1;
	itmp = (flag == 0 ? (offset+(int)EZ_SSheetY0(obj)): offset);
	if(itmp < 0) itmp = 0;
        if(itmp > ss) itmp = ss;
	if(EZ_SSheetY0(obj) != itmp)
	  {
	    EZ_SSheetY0(obj) = itmp;
            EZ_SSheetDirty(obj) = 1;
	    if(EZ_WidgetWindow(obj) != (Window )NULL)
              EZ_DrawWidgetAndDescendants(obj, 0);
	  }
      }
    break;
    default:
      break;
    }
}
/**************************************************************************************/
static void  EZ_UpdateVScrollBar(widget)
     EZ_Widget *widget;
{
  int    itmp,itmp1;
  float  ftmp, ftmp1,ftmp2;
  int    maxsize, pagesize, start;
  
  if(widget == (EZ_Widget *)NULL || EZ_WidgetType(widget) != EZ_WIDGET_VERTICAL_SCROLLBAR)
    return;

  maxsize = EZ_ScrollTotalSize(widget);
  pagesize = EZ_ScrollPageSize(widget);
  start = EZ_ScrollObjectBegin(widget);
      
  EZ_ScrollArrowSize(widget) = EZ_ScrollSSize(widget) + (EZ_ScrollBorderWidth(widget)<<1);  
  EZ_ScrollBgnPosition(widget) = EZ_ScrollArrowSize(widget) + EZ_WidgetPadB(widget) +
    EZ_ScrollBorderWidth(widget) + EZ_WidgetBorderWidth(widget) + EZ_WidgetPadY(widget);
  EZ_ScrollEndPosition(widget) = EZ_WidgetHeight(widget) - EZ_ScrollBgnPosition(widget);

  if(pagesize < maxsize)
    {
      /* computer the size of the scroll bar */
      ftmp = (float)(EZ_ScrollEndPosition(widget) - EZ_ScrollBgnPosition(widget));
      ftmp1 = (float)pagesize/(float)(maxsize);
      ftmp2 = ftmp1 * ftmp;
      EZ_ScrollLSize(widget) = (int)ftmp2 - (EZ_ScrollBorderWidth(widget)<<1);
      if(EZ_ScrollLSize(widget) < 10) EZ_ScrollLSize(widget) =  10;
      
      ftmp1 = (ftmp -(float)(EZ_ScrollLSize(widget)+(EZ_ScrollBorderWidth(widget)<<1)));
      if(ftmp1 == 0.0) ftmp1 = 1.0;
      EZ_ScrollFIncrement(widget) = (float)(maxsize - pagesize)/ftmp1;
      
      EZ_ScrollIIncrement(widget)  =  EZ_ScrollIIFactor(widget);
      itmp = (int) ((float)start /  EZ_ScrollFIncrement(widget))
	+ EZ_ScrollBgnPosition(widget);
      itmp1 = EZ_ScrollLSize(widget) + (EZ_ScrollBorderWidth(widget)<<1);
      if(itmp < EZ_ScrollBgnPosition(widget)) itmp = EZ_ScrollBgnPosition(widget);
      else if(itmp > EZ_ScrollEndPosition(widget) - itmp1)
	itmp = EZ_ScrollEndPosition(widget) - itmp1;
      EZ_ScrollPosition(widget) = itmp;
    }
  else
    {
      EZ_ScrollLSize(widget) = EZ_ScrollEndPosition(widget) - EZ_ScrollBgnPosition(widget)
	- (EZ_ScrollBorderWidth(widget)<<1);
      EZ_ScrollFIncrement(widget)  = 0.0;
      EZ_ScrollIIncrement(widget)  = 0;
      EZ_ScrollPosition(widget) =  EZ_ScrollBgnPosition(widget);
    }
}
/**************************************************************************************/

void EZ_ComputeWidgetVScrollBarSize(widget, w,h)
     EZ_Widget *widget;
     int       *w, *h;
{ 
  int width, cw, ch, padb;

  if(!(EZ_GetWidgetSizeComputedFlag(widget)))
    { 
      /* if(EZ_ScrollSSize(widget) < (EZ_ScrollBorderWidth(widget)<<1) + 6)*/
	EZ_ScrollSSize(widget) = (EZ_ScrollBorderWidth(widget)<<1) + 6;
      
      width = EZ_ScrollArrowSize(widget) = 
	EZ_ScrollSSize(widget) + (EZ_ScrollBorderWidth(widget)<<1);

      EZ_SetWidgetSizeComputedFlag(widget);
    }
  width = EZ_ScrollSSize(widget) + (EZ_ScrollBorderWidth(widget)<<1);
  cw = EZ_WidgetPadX(widget) + EZ_WidgetBorderWidth(widget);
  ch = EZ_WidgetPadY(widget) + EZ_WidgetBorderWidth(widget);

  ch = width + ((ch + width)<<1); /* just make sure the two arrows can fit in */
  cw = cw + cw + width;
  padb = EZ_WidgetPadB(widget) << 1;
  *w = cw + padb;
  *h = ch + padb;
}


void EZ_DrawWidgetVScrollBar(wptr)
     EZ_Widget *wptr;
{
  int             w, h,x,y,xx,yy,xxx,yyy, ww, hh, itmp, haw1, haw2;
  int             offset,  padb, padb2, tile, highlight, swapBg;
  Pixmap          pixmap;
  Window          win;
  GC              gc, gc1;
  unsigned long   bgpv;

  if(EZ_ScrollDirty(wptr))
    {
      EZ_UpdateVScrollBar(wptr);
      EZ_ScrollDirty(wptr) = 0;
    }

  win = EZ_WidgetWindow(wptr);
  w   = EZ_WidgetWidth(wptr);
  h   = EZ_WidgetHeight(wptr);
  padb = EZ_WidgetPadB(wptr);
  padb2 = padb + padb;  
  /*-----------------------------------------------------------
   *  Create a pixmap, draw into this pixmap in background and
   *  copy to the button window when finished.
   *----------------------------------------------------------*/
  pixmap = XCreatePixmap(EZ_DisplayForWidgets, win, w, h, EZ_DepthForWidgets);    
  EZ_FixFocusHighlightBorder(wptr, pixmap, w, h, padb);

  /*----------------------------------------------------------
   * Fill the background and draw the border.
   *---------------------------------------------------------*/
  tile = EZ_GetBackgroundGC(wptr, &gc, &bgpv, 0, 0);  
  if(tile)
    {
      int ox, oy;
      EZ_GetWidgetTileOrigin(wptr, &ox, &oy);      
      XSetTSOrigin(EZ_Display, gc, ox, oy);
    }
  XFillRectangle(EZ_DisplayForWidgets, pixmap, gc, padb,padb, w-padb2, h-padb2);   
  
  /*----------------------------------------------------------
   * Draw the scroll button
   *---------------------------------------------------------*/
  xx = EZ_WidgetPadX(wptr) + EZ_WidgetBorderWidth(wptr) + padb;
  yy = EZ_WidgetBorderWidth(wptr) + EZ_WidgetPadY(wptr) + padb;

  y = EZ_ScrollPosition(wptr);
  x = xx;

  hh = EZ_ScrollLSize(wptr) + (EZ_ScrollBorderWidth(wptr)<<1);
  ww = EZ_ScrollSSize(wptr) + (EZ_ScrollBorderWidth(wptr)<<1);
  swapBg = EZ_ScrollHighlight(wptr);
  highlight = EZ_GetWidgetHighlightFlag(wptr);

  if(swapBg) { EZ_InstallHighlightColor(wptr); } /*4/29/99*/

  if(EZ_WidgetBGPixmapB(wptr) != NULL)
    {
      EZ_Bitmap *bitmap = EZ_WidgetBGPixmapB(wptr);
      gc = EZ_NTILEGC2;
      if(highlight) XSetTile(EZ_Display,EZ_NTILEGC2,bitmap->htpixmap);
      else XSetTile(EZ_Display,EZ_NTILEGC2,bitmap->pixmap);
      XSetTSOrigin(EZ_Display, EZ_NTILEGC2, x, y);
    }
  else if(EZ_WidgetParentBGPixmapB(wptr) != NULL)
    {
      EZ_Bitmap *bitmap = EZ_WidgetParentBGPixmapB(wptr);
      gc = EZ_NTILEGC2;
      if(highlight) XSetTile(EZ_Display,EZ_NTILEGC2,bitmap->htpixmap);
      else XSetTile(EZ_Display,EZ_NTILEGC2,bitmap->pixmap);
      XSetTSOrigin(EZ_Display, EZ_NTILEGC2, x, y);
    }
  else
    {
      int tile = EZ_GetBackgroundGC(wptr,&gc, &bgpv, 0, highlight);
      if(tile) 
	{
	  int ox, oy;
	  EZ_GetWidgetTileOrigin(wptr, &ox, &oy);      
	  XSetTSOrigin(EZ_Display, gc, ox, oy);
	}
    }  
  XFillRectangle(EZ_DisplayForWidgets, pixmap, gc, x,y, ww, hh); 
  
  yyy = y -1 + (hh>>1);
  xxx = x + EZ_ScrollBorderWidth(wptr);
  offset = (EZ_ScrollBorderStyle(wptr) == EZ_BORDER_DOWN? 1 : 0);

  if(EZ_ScrollStyle(wptr))
    {
      int iw = EZ_ScrollSSize(wptr);
      int ih = EZ_ScrollLSize(wptr);
      int iy = y + EZ_ScrollBorderWidth(wptr) + offset;
      ih = (ih>>1) -2;
      EZ_OverlayDragablePixmap(wptr, pixmap, xxx,iy, iw, ih);
      EZ_OverlayDragablePixmap(wptr, pixmap, xxx, iy+4+ih, iw, ih);
    }  

  EZ_SetupUpBdGC(wptr, &gc1, &gc1, &gc1, &gc1,&gc, &gc, x, y, 1);
  XDrawLine(EZ_DisplayForWidgets,pixmap, gc,
	    xxx, yyy + offset,
	    xxx + EZ_ScrollSSize(wptr), yyy + offset);
  XDrawLine(EZ_DisplayForWidgets,pixmap, gc1,
	    xxx, yyy+2+offset,
	    xxx + EZ_ScrollSSize(wptr), yyy + 2 + offset);
  EZ_DrawRectBorderWithSize(wptr, pixmap,  x, y,  ww, hh,
			    EZ_ScrollBorderWidth(wptr), EZ_ScrollBorderStyle(wptr),x,y,1);

  if(swapBg) { EZ_UninstallHighlightColor(wptr); } /*4/29/99*/    
  /*----------------------------------------------------------
   * Draw the two arrows
   *---------------------------------------------------------*/
  itmp = EZ_ScrollArrowSize(wptr);
  haw1 = EZ_ScrollAW1Highlight(wptr) && EZ_HighlightEnabled;
  if(haw1) { EZ_InstallHighlightColor(wptr); } /*4/29/99*/  
  EZ_DrawVUArrow(wptr, pixmap, xx, yy,
		 itmp,
		 EZ_ScrollAW1BorderStyle(wptr),
		 EZ_ScrollBorderWidth(wptr), haw1);
  if(haw1) { EZ_UninstallHighlightColor(wptr); } /*4/29/99*/    

  haw2 = EZ_ScrollAW2Highlight(wptr) && EZ_HighlightEnabled;
  if(haw2) { EZ_InstallHighlightColor(wptr); } /*4/29/99*/  
  EZ_DrawVDArrow(wptr,pixmap, xx, EZ_WidgetHeight(wptr) - yy - itmp,
		 itmp,
		 EZ_ScrollAW2BorderStyle(wptr),
		 EZ_ScrollBorderWidth(wptr), haw2);
  if(haw2) { EZ_UninstallHighlightColor(wptr); } /*4/29/99*/      

  EZ_DrawRectBorder(wptr, pixmap);
  XCopyArea(EZ_DisplayForWidgets,pixmap,win, EZ_WRITABLEGC,0,0,w,h,0,0); 
  XFreePixmap(EZ_DisplayForWidgets, pixmap); 
}

/***************************************************************************/

void  EZ_VScrollBarEventHandle(widget, event)
     EZ_Widget *widget;
     XEvent          *event;
{
  int     x, y, xbgn, ybgn, xbgn2, ybgn2, arrowsize, tmp, tmp1, done;
  int     onarrow1 = 0, onarrow2 = 0, onbutton = 0, first_time = 1, offset=0;
  int     butn, wox,woy;
  Window  junkwin;
  XEvent  xevent;

  if(widget == (EZ_Widget *)NULL) return;
  if(event->type == Expose)
    {
      EZ_ScrollDirty(widget) = 1;
      EZ_DrawWidget(widget);      
    }
  if(EZ_GetWidgetDisabledFlag(widget) ) return;
  if(event->type == ButtonPress && event->xbutton.button == EZ_Btn3)
    {
      EZ_HandleDnDEvents(widget, event);
      return;
    }

  switch(event->type)
    {
    case MotionNotify:
      x = event->xmotion.x;
      y = event->xmotion.y;
      /* check if pointer is on the arrows */
      tmp = EZ_WidgetBorderWidth(widget) + EZ_WidgetPadB(widget);
      xbgn = tmp + EZ_WidgetPadX(widget);
      ybgn = tmp + EZ_WidgetPadY(widget);
      arrowsize = EZ_ScrollArrowSize(widget);
      xbgn2 = xbgn;
      ybgn2 = EZ_WidgetHeight(widget) - ybgn - arrowsize;
      
      tmp = EZ_ScrollLSize(widget) + (EZ_ScrollBorderWidth(widget)<<1);
      tmp1 = EZ_ScrollPosition(widget);

      if( x > xbgn && x < xbgn + arrowsize && y > ybgn && y < ybgn + arrowsize)
	{
	  /* pointer is on the top arrow */
          if(EZ_ScrollAW1Highlight(widget)== 0)
            {
              EZ_ScrollAW1Highlight(widget) = 1;
              EZ_DrawWidget(widget);
            }
	}
      else if(x > xbgn2 && x < xbgn2 + arrowsize && y > ybgn2 && y < ybgn2 + arrowsize)
	{
	  /* pointer is on the bottom arrow */
          if(EZ_ScrollAW2Highlight(widget) == 0)
            {
              EZ_ScrollAW2Highlight(widget) = 1;
              EZ_DrawWidget(widget);
            }
	}
      else if(y > tmp1 && y < tmp1 + tmp && x > xbgn && x < xbgn + arrowsize)
	{
          if(EZ_ScrollHighlight(widget) == 0)
            {
              EZ_SetWidgetHighlightFlag(widget);
              EZ_ScrollHighlight(widget) = 1;
              EZ_DrawWidget(widget);
            }
	}
      else
	{
	  if( EZ_ScrollAW1Highlight(widget) ||  EZ_ScrollAW2Highlight(widget)
	     || EZ_GetWidgetHighlightFlag(widget))
	    {
	      EZ_ScrollAW1Highlight(widget)  = 0;
	      EZ_ScrollAW2Highlight(widget)  = 0;
              EZ_ScrollHighlight(widget) = 0;
	      EZ_ClearWidgetHighlightFlag(widget);
	      EZ_DrawWidget(widget);	      
	    }
	}
      break;
    case EnterNotify:
      break;
    case LeaveNotify:
      EZ_ScrollAW1Highlight(widget)  = 0;
      EZ_ScrollAW2Highlight(widget)  = 0;
      EZ_ScrollHighlight(widget) = 0;
      EZ_ClearWidgetHighlightFlag(widget);      
      EZ_DrawWidget(widget);	      
      break;
    case ButtonPress:
      butn = event->xbutton.button;
      x = event->xbutton.x;
      y = event->xbutton.y;
      /* check if pointer is on the arrows */
      tmp = EZ_WidgetBorderWidth(widget) + EZ_WidgetPadB(widget);
      xbgn = tmp + EZ_WidgetPadX(widget);
      ybgn = tmp + EZ_WidgetPadY(widget);
      arrowsize = EZ_ScrollArrowSize(widget);
      xbgn2 = xbgn;
      ybgn2 = EZ_WidgetHeight(widget) - ybgn - arrowsize;

      if( x > xbgn && x < xbgn + arrowsize && y > ybgn && y < ybgn + arrowsize)
	{
	  /* pointer is on the top arrow */
	  EZ_ScrollAW1Highlight(widget) = 1;
	  if(EZ_DepresionDisabled == 0)
	    EZ_ScrollAW1BorderStyle(widget) = EZ_BORDER_DOWN;
	  onarrow1 = 1;
	}
      else if(x > xbgn2 && x < xbgn2 + arrowsize && y > ybgn2 && y < ybgn2 + arrowsize)
	{
	  /* pointer is on the bottom arrow */
	  EZ_ScrollAW2Highlight(widget) = 1;
	  if(EZ_DepresionDisabled == 0)
	    EZ_ScrollAW2BorderStyle(widget) = EZ_BORDER_DOWN;
	  onarrow2 = 1;
	}
      else
	{
	  tmp = EZ_ScrollLSize(widget) + (EZ_ScrollBorderWidth(widget)<<1);
	  tmp1 = EZ_ScrollPosition(widget);
	  if(y > tmp1 && y < tmp1 + tmp && x > xbgn && x < xbgn + arrowsize)
	    {
	      EZ_SetWidgetHighlightFlag(widget);
              EZ_ScrollHighlight(widget) = 1;
	      if(EZ_DepresionDisabled == 0)
		EZ_ScrollBorderStyle(widget) = EZ_BORDER_DOWN;
	      onbutton = 1;
	      offset = y - EZ_ScrollPosition(widget);
	    } 
	}
      if(!(onarrow1 | onarrow2 | onbutton) && butn == EZ_Btn2 &&
	 (x > xbgn && y > ybgn && x < EZ_WidgetWidth(widget)-xbgn && y < ybgn2))
	{
	  int oldy = EZ_ScrollPosition(widget);  
	  tmp = EZ_ScrollLSize(widget) + (EZ_ScrollBorderWidth(widget)<<1);
	  tmp1 = tmp>>1;

	  tmp = y - oldy - tmp1;
	  offset  =  tmp1;
	  tmp1 = (int) (tmp * EZ_ScrollFIncrement(widget) + 0.9);
	  EZ_DoVScroll(widget, tmp1, 0);
	  EZ_SetWidgetHighlightFlag(widget);
          EZ_ScrollHighlight(widget) = 1;
	  if(EZ_DepresionDisabled == 0)
	    EZ_ScrollBorderStyle(widget) = EZ_BORDER_DOWN;
	  onbutton = 1;
	}
      EZ_DrawWidget(widget);

      if(onarrow1 | onarrow2 | onbutton)
	{
	  XTranslateCoordinates(EZ_Display, EZ_WidgetWindow(widget),
				RootWindow(EZ_Display, EZ_ScreenNum),
				6,(int)(EZ_ScrollBgnPosition(widget)),
				&wox,&woy, &junkwin);
	  done = 0;
	  xevent.type = 0;

	  while(!done) /* wait for a matching ButtonRelease */
	    { 
	      if(!done)
		{
		  if(onarrow1) EZ_DoVScroll(widget, -EZ_ScrollIIncrement(widget), 0);
		  else if(onarrow2) EZ_DoVScroll(widget, EZ_ScrollIIncrement(widget), 0);
		  if(first_time == 1)
		    {
		      if(onarrow1 | onarrow2 )
			{
			  XFlush(EZ_Display);
			  EZ_SitForALittleBit(200000);
			}
		      first_time = 0;
		    }
		}
	      if(onbutton)
		{
		  XNextEvent(EZ_Display, &xevent);
		  EZ_FilterEvent(&xevent);
                  EZ_InvokePrivateEventHandler(&xevent);
		  if(xevent.xany.window != EZ_WidgetWindow(widget))
		    EZ_WidgetDispatchEvent(&xevent);
		}
	      else
		EZ_SitForALittleBit(30000); 
	      
	      while(XPending(EZ_Display))
		{
		  if(xevent.type == ButtonRelease && xevent.xbutton.button == butn) break;
		  else if(xevent.type == Expose || xevent.type == FocusIn || xevent.type == FocusOut) 
		    EZ_WidgetDispatchEvent(&xevent);
		  XNextEvent(EZ_Display, &xevent);
		  EZ_FilterEvent(&xevent);
                  EZ_InvokePrivateEventHandler(&xevent);
		  if(xevent.xany.window != EZ_WidgetWindow(widget))
		    EZ_WidgetDispatchEvent(&xevent);
		}
	      if(xevent.type == ButtonRelease)
		{
		  if(xevent.xbutton.button == butn)
		    {
		      EZ_ScrollAW1BorderStyle(widget) = EZ_BORDER_UP;
		      EZ_ScrollAW2BorderStyle(widget) = EZ_BORDER_UP;
		      EZ_ScrollBorderStyle(widget) = EZ_BORDER_UP;
		      x = xevent.xbutton.x;
		      y = xevent.xbutton.y;
		      if(onarrow1)
			{
			  if(!(x> xbgn && x< xbgn + arrowsize && y > ybgn && y < ybgn + arrowsize))
			    EZ_ScrollAW1Highlight(widget) = 0;
			}
		      else if(onarrow2)
			{
			  if(!(x> xbgn2 && x< xbgn2 +arrowsize && y> ybgn2 && y< ybgn2 + arrowsize))
			    EZ_ScrollAW2Highlight(widget) = 0;			
			}
		      else if(onbutton)
			{
			  tmp = EZ_ScrollLSize(widget) + (EZ_ScrollBorderWidth(widget)<<1);
			  tmp1 = EZ_ScrollPosition(widget);
			  if(!(y > tmp1 && y < tmp1 + tmp && x > xbgn && x < xbgn + arrowsize))
                            {
                              EZ_ClearWidgetHighlightFlag(widget);
                              EZ_ScrollHighlight(widget) = 0;
                            }
			}
		      EZ_DrawWidget(widget);
		      done = 1;
		    }
		}
	      if(!done && onbutton)
		{
		  int             rx,ry,x,y;
		  unsigned int    mask;
		  Window          root,win;
		  XQueryPointer(EZ_Display, RootWindow(EZ_Display, EZ_ScreenNum),
				&root,
				&win, 
				&rx, &ry, 
				&x,&y, 
				&mask );

		  tmp = ry - woy - offset;
		  if(tmp < 0) tmp = 0;
		  else 
		    {
		      tmp1 = EZ_ScrollEndPosition(widget) - EZ_ScrollBgnPosition(widget) -
			(EZ_ScrollLSize(widget) + (EZ_ScrollBorderWidth(widget)<<1));
		      if(tmp > tmp1) tmp = tmp1;
		    }
		  tmp1 = (int) (tmp * EZ_ScrollFIncrement(widget) + 0.9);
		  EZ_DoVScroll(widget, tmp1, 1);
		}
	    }
	}
      else
	{
	  /* must be Button1 press and Press is not on the slider  */
	  /* scroll towards the pointer */
	  int xend = EZ_WidgetWidth(widget)-xbgn;
	  if(x > xbgn && x < xend && y > ybgn && y < ybgn2)
	    {
	      int lastposition = -10000;
	      done = 0;
	      tmp = EZ_ScrollLSize(widget) + (EZ_ScrollBorderWidth(widget)<<1)-1;
	      tmp = (y < EZ_ScrollPosition(widget)) ? -tmp : tmp;

	      while(!done) /* wait for a matching ButtonRelease */
		{ 
		  int pos = EZ_ScrollPosition(widget);
		  if(pos != lastposition)
		    {
		      lastposition = pos;
		      tmp1 = (int) (tmp * EZ_ScrollFIncrement(widget));
		      EZ_DoVScroll(widget, tmp1, 0);	      
		    }
		  if(first_time)
		    {
		      first_time = 0;
		      XFlush(EZ_Display);
		      EZ_SitForALittleBit(300000); 
		    }
		  else EZ_SitForALittleBit(30000); 

		  while(XPending(EZ_Display))
		    {
		      XNextEvent(EZ_Display, &xevent);
		      EZ_FilterEvent(&xevent);
                      EZ_InvokePrivateEventHandler(&xevent);
		      if(xevent.xany.window != EZ_WidgetWindow(widget))
			EZ_WidgetDispatchEvent(&xevent);
		      if(xevent.type == ButtonRelease && xevent.xbutton.button == butn)
			{
			  done = 1;
			  break;
			}
		    }
		}
	    }
	}
      break;
    case KeyPress:
      {
	char              tmpbuffer[8];
	int               buffersize = 8, move_unit;
	KeySym            keysym;
	XComposeStatus    compose; 
	int count = XLookupString(&(event->xkey), tmpbuffer, buffersize, &keysym, &compose);
	tmpbuffer[count] = '\0';

	switch(keysym)
	  {
	  case XK_Up: case XK_k:  case XK_p: case XK_K:  case XK_P: 
#ifdef XK_KP_Up
	  case XK_KP_Up:
#endif
	    move_unit = -EZ_ScrollIIncrement(widget);
	    if(event->xkey.state & ShiftMask) move_unit = 1;
	    else if(event->xkey.state & ControlMask) move_unit = move_unit * 4;
	    if(event->xkey.state & Mod1Mask) move_unit *= 4 ;
	    EZ_DoVScroll(widget, move_unit, 0);
	    break;
	  case XK_Down: case XK_n: case XK_j: case XK_N: case XK_J:  
#ifdef XK_KP_Down
	  case XK_KP_Down:
#endif
	    move_unit = EZ_ScrollIIncrement(widget);
	    if(event->xkey.state & ShiftMask)  move_unit = 1;
	    else if(event->xkey.state & ControlMask) move_unit = move_unit * 4;
	    if(event->xkey.state & Mod1Mask) move_unit *= 4 ;
	    EZ_DoVScroll(widget, move_unit, 0);
	    break;
	  default:
	  break;
	}
	{
	  XEvent tmpevent; 
	  while(XCheckTypedEvent(EZ_Display, KeyPress, &tmpevent))
	    if(EZ_FilterEvent(&tmpevent))
	      EZ_InvokePrivateEventHandler(&tmpevent);
	}
      }
      break;
    default:
      break;
    }
}
/*****************************************************************************************/
void EZ_GetScrollbarState(widget, total, page, start)
     EZ_Widget *widget;
     int *total, *page, *start;
{
  if(widget)
    {
      int type = EZ_WidgetType(widget);
      if(type == EZ_WIDGET_HORIZONTAL_SCROLLBAR ||
	 type == EZ_WIDGET_VERTICAL_SCROLLBAR)
	{
	  *total = EZ_ScrollTotalSize(widget);
	  *page = EZ_ScrollPageSize(widget);
	  if(type == EZ_WIDGET_VERTICAL_SCROLLBAR)
	    *start = -EZ_ScrollObjectBegin(widget);
	  else
	    *start = EZ_ScrollObjectBegin(widget);
	}
    }
}
/*****************************************************************************************
 *
 *  Unfortunately, VScroll and HScroll are implemented differently. This little hack
 *  and the hack above make the usage of the two scrolls the same.
 */
void  EZ_UpdateScrollbar(widget, maxsize, pagesize, start)
     EZ_Widget *widget;
     int maxsize, pagesize, start;
{
  if(widget)
    {
      int type = EZ_WidgetType(widget);
      if(type == EZ_WIDGET_VERTICAL_SCROLLBAR)
	EZ_UpdateScrollbarI(widget, maxsize, pagesize, -start);
      else if(type == EZ_WIDGET_HORIZONTAL_SCROLLBAR)
	EZ_UpdateScrollbarI(widget, maxsize, pagesize, start);
    }
}
/*****************************************************************************************/
void  EZ_UpdateScrollbarI(widget, maxsize, pagesize, start)
     EZ_Widget *widget;
     int maxsize, pagesize, start;
{
  if(EZ_ScrollObjectBegin(widget) != start ||
     EZ_ScrollTotalSize(widget) != maxsize  ||
     EZ_ScrollPageSize(widget) != pagesize)
    {
      EZ_ScrollTotalSize(widget) = maxsize > 0 ? maxsize : 1;     
      EZ_ScrollPageSize(widget) = pagesize > 0 ? pagesize : 1;
      if(EZ_WidgetType(widget) == EZ_WIDGET_VERTICAL_SCROLLBAR)
	{
	  int tmp = EZ_ScrollTotalSize(widget) - EZ_ScrollPageSize(widget);
	  if(start > tmp) start = tmp;
	  if(start <= 0) start = 0;
	}
      else
	{
	  int tmp = EZ_ScrollPageSize(widget) - EZ_ScrollTotalSize(widget);
	  if(start < tmp) start = tmp;
	  if(start > 0) start = 0;
	}
      EZ_ScrollObjectBegin(widget) = start;
      EZ_ScrollDirty(widget) = 1;
      if(EZ_WidgetMapped(widget)) { EZ_DrawWidget(widget); XFlush(EZ_Display);}
    }
}

int EZ_SetScrollbarDiscreteSpeed(widget, speed)
     EZ_Widget *widget; int speed;
{
  int osd = 0;
  if(widget && speed > 0)
    {
      int type = EZ_WidgetType(widget);
      if(type == EZ_WIDGET_VERTICAL_SCROLLBAR || type == EZ_WIDGET_HORIZONTAL_SCROLLBAR)
	{ osd = EZ_ScrollIIFactor(widget); EZ_ScrollIIFactor(widget) = speed;}
    }
  return(osd);
}

int EZ_SetHScrollbarDiscreteSpeed(widget, speed)
     EZ_Widget *widget; int speed;
{
  int osd = 0;
  if(widget && speed > 0)
    {
      int type = EZ_WidgetType(widget);
      EZ_Widget *twidget;
      switch(type)
	{
	case  EZ_WIDGET_HORIZONTAL_SCROLLBAR:
	  twidget = widget;
	  break;
	case EZ_WIDGET_TEXT:
	  twidget = EZ_TextHScroll(widget);
	  break;
	case EZ_WIDGET_ITEXT:
	  twidget = EZ_ITextHScroll(widget);
	  break;
	case EZ_WIDGET_LIST_BOX:
	  twidget = EZ_ListBoxHScroll(widget);
	  break;
	case EZ_WIDGET_ILIST_BOX:
	  twidget = EZ_IListBoxHScroll(widget);
	  break;
	case EZ_WIDGET_FANCY_LIST_BOX:
	  twidget = EZ_FListBoxHScroll(widget);
	  break;
	case EZ_WIDGET_IFANCY_LIST_BOX:
	  twidget = EZ_IFListBoxHScroll(widget);
	  break;
	case EZ_WIDGET_TREE:
	  twidget = EZ_TreeWHScroll(widget);
	  break;
	case EZ_WIDGET_ITREE:
	  twidget = EZ_ITreeWHScroll(widget);
	  break;
	case EZ_WIDGET_WORK_AREA:
	  twidget = EZ_WorkAreaHScroll(widget);
	  break;
	case EZ_WIDGET_IWORK_AREA:
	  twidget = EZ_IWorkAreaHScroll(widget);
	  break;
        case EZ_WIDGET_SPREAD_SHEET:
          twidget = EZ_SSheetHScroll(widget);
          break;
	default:
	  twidget = NULL;
	  break;
	}
      if(twidget)
	{osd = EZ_ScrollIIFactor(twidget); EZ_ScrollIIFactor(twidget) = speed;}
    }
  return(osd);
}

int EZ_SetVScrollbarDiscreteSpeed(widget, speed)
     EZ_Widget *widget; int speed;
{
  int osd = 0;
  if(widget && speed > 0)
    {
      int type = EZ_WidgetType(widget);
      EZ_Widget *twidget;
      switch(type)
	{
	case  EZ_WIDGET_VERTICAL_SCROLLBAR:
	  twidget = widget;
	  break;
	case EZ_WIDGET_TERM:
	  twidget = EZ_TermVScroll(widget);          
	  break;
	case EZ_WIDGET_ITERM:
	  twidget = EZ_ITermVScroll(widget);          
	  break;
	case EZ_WIDGET_TEXT:
	  twidget = EZ_TextVScroll(widget);
	  break;
	case EZ_WIDGET_ITEXT:
	  twidget = EZ_ITextVScroll(widget);
	  break;
	case EZ_WIDGET_LIST_BOX:
	  twidget = EZ_ListBoxVScroll(widget);
	  break;
	case EZ_WIDGET_ILIST_BOX:
	  twidget = EZ_IListBoxVScroll(widget);
	  break;
	case EZ_WIDGET_FANCY_LIST_BOX:
	  twidget = EZ_FListBoxVScroll(widget);
	  break;
	case EZ_WIDGET_IFANCY_LIST_BOX:
	  twidget = EZ_IFListBoxVScroll(widget);
	  break;
	case EZ_WIDGET_TREE:
	  twidget = EZ_TreeWVScroll(widget);
	  break;
	case EZ_WIDGET_ITREE:
	  twidget = EZ_ITreeWVScroll(widget);
	  break;
	case EZ_WIDGET_WORK_AREA:
	  twidget = EZ_WorkAreaVScroll(widget);
	  break;
	case EZ_WIDGET_IWORK_AREA:
	  twidget = EZ_IWorkAreaVScroll(widget);
	  break;
        case EZ_WIDGET_SPREAD_SHEET:
          twidget = EZ_SSheetVScroll(widget);
          break;
	default:
	  twidget = NULL;
	  break;
	}
      if(twidget)
	{ osd =  EZ_ScrollIIFactor(twidget);  EZ_ScrollIIFactor(twidget) = speed;}
    }
  return(osd);
}
/*****************************************************************************************/
#undef _EZ_WIDGET_SCROLL_C_
