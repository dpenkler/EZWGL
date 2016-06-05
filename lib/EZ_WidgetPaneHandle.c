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
 ***       Pane Handle                                         ***
 ***                                                           ***
 *****************************************************************/
#define _EZ_WIDGET_PANE_HANDLE_C_

#include "EZ_Widget.h"

/*********************************************************************
 * 
 *  Functions implemented in this file:
 */
EZ_Widget       *EZ_CreatePaneHandle MY_ANSIARGS((EZ_Widget *parent));
void            EZ_DrawPaneHandle MY_ANSIARGS((EZ_Widget *widget));
void            EZ_PaneHEventHandle MY_ANSIARGS((EZ_Widget *widget,XEvent *event));
void            EZ_ComputePaneHandleSize MY_ANSIARGS((EZ_Widget *widget, int *w, int *h));

/*********************************************************************
 * 
 *  Local Variables.
 */
static void finish_paneHandle MY_ANSIARGS((EZ_Widget *widget, int x,int y));

static EZ_WidgetHandle EZ_PaneDividerHandle =
{ 
  EZ_ComputePaneHandleSize,
  EZ_DrawPaneHandle,
  EZ_FreeUnknownData,
  EZ_PaneHEventHandle,
};

/*********************************************************************/
EZ_Widget  *EZ_CreatePaneHandle(parent)
     EZ_Widget *parent;
{
  EZ_Widget  *tmp;

  if(parent == (EZ_Widget *)NULL)
    {
      (void)fprintf(stderr,
		    "Error in EZ_CreatePaneHandle, parent widget is NULL\n");
      return(NULL);
    }
  tmp = EZ_CreateNewWidget(parent);
  EZ_WidgetHandlingFunctions[EZ_WIDGET_PANE_HANDLE] = &EZ_PaneDividerHandle;

  EZ_SetWidgetTypeAndNames(tmp, EZ_WIDGET_PANE_HANDLE);
  EZ_SetWidgetExpandFlag(tmp);
  EZ_SetWidgetTransientFlag(tmp);
  EZ_WidgetPadX(tmp) = 0;
  EZ_WidgetPadY(tmp) = 0;
  EZ_WidgetPadB(tmp) = 1;
  EZ_WidgetBorderWidth(tmp)  = 0;

  EZ_WidgetCursor(tmp) = EZ_GetCursor(EZ_C_HAND2);

  EZ_PaneHandleOrientation(tmp) = EZ_HORIZONTAL;
  EZ_PaneHandleStyle(tmp) = 1;
  EZ_PaneHandleMovable(tmp) = 1;

  EZ_SetDoNotPropagateFlag(tmp);
  EZ_SetWidgetFocusableFlag(tmp);  

  return(tmp);
}
/****************************************************************************/
void  EZ_ComputePaneHandleSize(widget, w,h)
     EZ_Widget *widget;
     int             *w, *h;
{
  EZ_Widget *parent = EZ_WidgetParent(widget);
  EZ_GManager       *gmanager; 
  int orien = EZ_WidgetStacking(parent);
  int fill = EZ_WidgetFillMode(parent);
  /* if parent is a gridbag, paneHandle has nothing to do. */
  gmanager = EZ_WidgetGManager(parent);
  if(gmanager && EZ_GMType(gmanager) == EZ_GB_GEOM_MANAGER)
    {
      if(EZ_WidgetGBW(widget) > 1) orien = EZ_VERTICAL;
      else orien = EZ_HORIZONTAL;
      EZ_WidgetGBFill(widget) = EZ_FILL_NONE;
      EZ_WidgetGBAnchor(widget) = EZ_CENTER;
      EZ_ClearWidgetExpandFlag(widget);
    }
  
  if(!(EZ_GetWidgetSizeComputedFlag(widget)))
    {
      EZ_SetWidgetSizeComputedFlag(widget);
    }      
  if(orien >= EZ_HORIZONTAL && orien <= EZ_HORIZONTAL_CENTER)
    {
      int px = (EZ_WidgetPadB(widget) + EZ_WidgetPadX(widget)) << 1;
      *w = 6 + px;
      *h = 32;
      EZ_ClearWidgetHeightSetFlag(widget);
      EZ_SetWidgetWidthSetFlag(widget);
      EZ_PaneHandleOrientation(widget) = EZ_HORIZONTAL;
      switch(fill)
	{
	case EZ_FILL_NONE:
	  EZ_WidgetFillMode(parent) = EZ_FILL_HORIZONTALLY;
	  break;
	case EZ_FILL_VERTICALLY:
	  EZ_WidgetFillMode(parent) = EZ_FILL_BOTH;
	  break;
	default:
	  break;
	}
    }
  else
    {
      int py = (EZ_WidgetPadB(widget) + EZ_WidgetPadY(widget)) << 1;
      *w = 32;
      *h = 6 + py;
      EZ_ClearWidgetWidthSetFlag(widget);
      EZ_SetWidgetHeightSetFlag(widget);
      EZ_PaneHandleOrientation(widget) = EZ_VERTICAL;
      switch(fill)
	{
	case EZ_FILL_NONE:
	  EZ_WidgetFillMode(parent) = EZ_FILL_VERTICALLY;
	  break;
	case EZ_FILL_HORIZONTALLY:
	  EZ_WidgetFillMode(parent) = EZ_FILL_BOTH;
	  break;
	default:
	  break;
	}
    }
}
/****************************************************************************/
void EZ_DrawPaneHandle(wptr)
     EZ_Widget *wptr;
{
  int             w, h, x,y, padb, padb2, ox,oy,tile, style, movable;
  Pixmap          pixmap;
  Window          win;
  GC              gc, gc1, gc2, junkgc;
  unsigned long   bgpv;
  int orien = EZ_PaneHandleOrientation(wptr);

  win = EZ_WidgetWindow(wptr);
  w   = EZ_WidgetWidth(wptr);
  h   = EZ_WidgetHeight(wptr);
  padb = EZ_WidgetPadB(wptr);
  padb2 = padb + padb;
  style = EZ_PaneHandleStyle(wptr);
  movable = EZ_PaneHandleMovable(wptr);

  if(movable == 0) { EZ_ClearWidgetFocusableFlag(wptr);  style = 2;}

  pixmap = XCreatePixmap(EZ_DisplayForWidgets, win, w, h, EZ_DepthForWidgets); 
  EZ_FixFocusHighlightBorder(wptr, pixmap, w, h, padb);

  if((tile= EZ_GetBackgroundGC(wptr, &gc,&bgpv, 1, 0)))
    {
      EZ_GetWidgetTileOrigin(wptr, &ox, &oy);      
      XSetTSOrigin(EZ_Display, gc, ox, oy);
    }
  XFillRectangle(EZ_Display, pixmap, gc, padb,padb, w-padb2, h-padb2); 
  
  if(EZ_GetDarkBrightNormalBDGC(wptr, &gc1, &gc2, &junkgc)) /* dark, bright */
    {
      if(tile==0) EZ_GetWidgetTileOrigin(wptr, &ox, &oy);      
      XSetTSOrigin(EZ_Display, gc1, ox, oy);
      XSetTSOrigin(EZ_Display, gc2, ox, oy);
    }
  if(orien == EZ_HORIZONTAL)
    {
      int px = EZ_WidgetPadX(wptr) + padb;
      if(style != 0)
        {
          if(style > 1)
            {
              x = (w>>1);
              XDrawLine(EZ_DisplayForWidgets,pixmap, gc1,  x, 0,   x, h);
              x+=2;
              XDrawLine(EZ_DisplayForWidgets,pixmap, gc2,  x, 0,   x, h);
              if(movable) XFillRectangle(EZ_Display, pixmap, gc, px,h-32, w-1, w-1); 
            }
          if(movable)
            EZ_DrawRectBorderWithSize(wptr, pixmap, px,h-32, w-1,w-1, 1, 
                                      EZ_BORDER_UP,0,0,0);          
        }
      else
        {
          int ll;
          x = (w>>1) + 1;
          XDrawLine(EZ_Display,pixmap, EZ_DARKREDGC,  px, h-32, w-px, h-32);
          y = x--;
          ll = x - px;
          XDrawLine(EZ_Display,pixmap, EZ_DARKREDGC,  x, h-32-ll, x, h-32+ll);
        }
    }
  else
    {
      int py = EZ_WidgetPadY(wptr) + padb;
      if(style != 0)
        {
          if(style > 1)
            {
              y = (h>>1) - 1;
              XDrawLine(EZ_DisplayForWidgets,pixmap, gc1,  0, y,   w, y);
              y = (h>>1) + 1;
              XDrawLine(EZ_DisplayForWidgets,pixmap, gc2,  0, y,   w, y);
              if(movable) XFillRectangle(EZ_Display, pixmap, gc, w-32,py, h-1, h-1); 
            }
          if(movable)
            EZ_DrawRectBorderWithSize(wptr, pixmap, w-32,py, h-1,h-1, 1, 
                                      EZ_BORDER_UP,0,0,0);          
        }
      else
        {
          int ll;
          y = (h>>1) + 1;
          x = y--;
          ll = y - py;
          XDrawLine(EZ_Display,pixmap, EZ_DARKREDGC,  w-32-ll, y, w-32+ll,y);
          XDrawLine(EZ_Display,pixmap, EZ_DARKREDGC,  w-32, py, w-32, h-py);
        }
    }
  EZ_DrawRectBorder(wptr, pixmap);
  XCopyArea(EZ_Display,pixmap,win, EZ_WRITABLEGC,0,0,w,h,0,0); 
  XFreePixmap(EZ_Display, pixmap);                             
} 

/****************************************************************************/
void  EZ_PaneHEventHandle(widget, event)
     EZ_Widget *widget;
     XEvent    *event;
{
  int     Ox,Oy,x,y, dx,dy, rootx, rooty, oldx, oldy, buttonP, orien;
  XEvent  xevent;
  Window  root, child;
  unsigned int    mask;

  if(EZ_PaneHandleMovable(widget) == 0)
    {
      if(event->type == Expose)
        EZ_DrawWidget(widget);      
      return;
    }

  switch(event->type)
    {
    case Expose:
      EZ_DrawWidget(widget);      
      break;

    case ButtonPress:
      XRaiseWindow(EZ_Display, EZ_WidgetWindow(widget));
      EZ_DrawWidget(widget);      
      buttonP = event->xbutton.button;
      XQueryPointer(EZ_Display, RootWindow(EZ_Display, EZ_ScreenNum),
		    &root,                                    /* root return win */
		    &child,                                   /* child ret win   */
		    &oldx, &oldy,                             /* x, y in root    */
		    &x,&y,                                    /* x, y in win     */
		    &mask);  
      /*
       * wait for a matching ButtonRelease. In the mean time, process
       * MotionNotify and SelectionRequest events.
       */
      Ox = EZ_WidgetOriginX(widget);
      Oy = EZ_WidgetOriginY(widget);
      orien = EZ_PaneHandleOrientation(widget);
      while(1)
	{
	  XNextEvent(EZ_Display, &xevent); 
	  EZ_FilterEvent(&xevent);
          EZ_InvokePrivateEventHandler(&xevent);
	  if(xevent.xany.window != EZ_WidgetWindow(widget))
	    EZ_WidgetDispatchEvent(&xevent);
	  else
	    {
	      switch(xevent.type)
		{
		case MotionNotify:
		  XQueryPointer(EZ_Display, RootWindow(EZ_Display, EZ_ScreenNum),
				&root,                                    /* root return win */
				&child,                                   /* child ret win   */
				&rootx, &rooty,                           /* x, y in root    */
				&x,&y,                                    /* x, y in win     */
				&mask);  
		  dx = rootx - oldx;  dy = rooty - oldy;
		  oldx = rootx;   oldy = rooty;
		  if(orien == EZ_HORIZONTAL)
		    EZ_WidgetOriginX(widget) += dx;
		  else
		    EZ_WidgetOriginY(widget) += dy;
		  EZ_ClearWidgetSizeComputedFlag(widget);
		  EZ_ReDisplayWidget(widget);
		  while(XCheckTypedEvent(EZ_Display, MotionNotify, &xevent))
		    EZ_FilterEvent(&xevent);
		  break;
		case ButtonRelease:
		  if(xevent.xbutton.button == buttonP)
		    {
		      finish_paneHandle(widget,Ox,Oy);
		      return;
		    }
		  break;
		default:
		  break;
		}
	    }
	}
      break;
    case KeyPress:
      {
	KeySym            keysym;
	XComposeStatus    compose; 
	char              tmpbuffer[8];
	int               count,move_unitX = 0, move_unitY = 0;
	count = XLookupString(&(event->xkey), tmpbuffer, 8, &keysym, &compose);
	tmpbuffer[count] = '\0';
	orien = EZ_PaneHandleOrientation(widget);
	Ox = EZ_WidgetOriginX(widget);
	Oy = EZ_WidgetOriginY(widget);
	if(orien == EZ_HORIZONTAL)
	  {
	    switch(keysym)
	      {
	      case XK_Left: case XK_b:  case XK_h: case XK_B:  case XK_H: 
#ifdef XK_KP_Left
	      case XK_KP_Left:
#endif
		if(event->xkey.state & ShiftMask) move_unitX = -20;
		else move_unitX = -10;
		
		break;
	      case XK_Right: case XK_f: case XK_l: case XK_F: case XK_L:  
#ifdef XK_KP_Right
	      case XK_KP_Right: 
#endif
		if(event->xkey.state & ShiftMask) move_unitX = 20;
		else move_unitX = 10;
		break;
	      default:
		break;
	      }
	  }
	else
	  {
	    switch(keysym)
	      {
	      case XK_Down: case XK_n: case XK_j: case XK_N: case XK_J:  
#ifdef XK_KP_Down
	      case XK_KP_Down:
#endif
		if(event->xkey.state & ShiftMask) move_unitY = 20;
		else move_unitY = 10;
		break;
	      case XK_Up:  case XK_k:  case XK_p: case XK_K:  case XK_P: 
#ifdef XK_KP_Up
	      case XK_KP_Up:
#endif
		if(event->xkey.state & ShiftMask) move_unitY = -20;
		else move_unitY = -10;
		break;
	      default:
		break;
	      }
	  }
	if(move_unitX || move_unitY)
	  {
	    EZ_WidgetOriginX(widget) += move_unitX;
	    EZ_WidgetOriginY(widget) += move_unitY;
	    EZ_ClearWidgetSizeComputedFlag(widget);
	    EZ_ReDisplayWidget(widget);
	    /* resize parent */
	    finish_paneHandle(widget, Ox, Oy);
	    /* take focus  again */
	    /* EZ_WarpPointerToWidgetWindow(widget, 1);*/
	  }
	{
	  XEvent xevent;
	  while(XCheckTypedEvent(EZ_Display, KeyPress, &xevent))
	    if(EZ_FilterEvent(&xevent))
	      EZ_InvokePrivateEventHandler(&xevent);	    
	}
      }
      break;
    case EnterNotify:
      EZ_SetWidgetHighlightFlag(widget);
      { EZ_InstallHighlightColor(widget); } /*4/29/99*/
      EZ_DrawWidget(widget);      
      break;
    case LeaveNotify:
      EZ_ClearWidgetHighlightFlag(widget);
      { EZ_UninstallHighlightColor(widget); } /*4/29/99*/
      EZ_DrawWidget(widget);
    default:
      break;
    }
}
/******************************************************************/
static void finish_paneHandle(widget, Ox, Oy)
     EZ_Widget *widget;
     int Ox, Oy;
{
  int cx,cy, Bflag, Aflag, orien;
  EZ_Widget *parent, *tmp, *before, *after;
  EZ_GManager       *gmanager; 

  parent = EZ_WidgetParent(widget);
  gmanager = EZ_WidgetGManager(parent);
  if(gmanager && EZ_GMType(gmanager) != EZ_DEFAULT_GEOM_MANAGER) return;

  orien = EZ_PaneHandleOrientation(widget);
  /* if parent is a grid bag, better not interfere with it. */

  before = after = NULL;
  tmp = EZ_WidgetChildren(parent);
  while(tmp && tmp != widget)
    {
      before = tmp;
      tmp = EZ_WidgetSibling(tmp);
    }
  if(tmp) after = EZ_WidgetSibling(tmp);
  if(before && after)
    {
      Bflag = (before== NULL)? 0: EZ_WidgetFlags(before);
      Aflag = (after == NULL)? 0: EZ_WidgetFlags(after);
      if(orien == EZ_HORIZONTAL)
	{
	  cx = EZ_WidgetOriginX(widget) - Ox;
	  if(cx < 0)
	    {
	      int ww = EZ_WidgetWidth(before);
	      if(ww + cx < 16) cx = 16 - ww;
	      EZ_WidgetWidthHint(before) = ww + cx;
	      EZ_ClearWidgetSizeComputedFlag(before);
	      EZ_SetWidgetWidthSetFlag(before);
	      
	      ww = EZ_WidgetWidth(after);
	      EZ_WidgetWidthHint(after) = ww - cx;
	      EZ_ClearWidgetSizeComputedFlag(after);
	      EZ_SetWidgetWidthSetFlag(after);				  
	    }
	  else
	    {
	      int ww = EZ_WidgetWidth(after);
	      if(ww - cx < 16) cx = ww - 16;
	      EZ_WidgetWidthHint(after) = ww - cx;
	      EZ_ClearWidgetSizeComputedFlag(after);
	      EZ_SetWidgetWidthSetFlag(after);				  
	      
	      ww = EZ_WidgetWidth(before);
	      EZ_WidgetWidthHint(before) = ww + cx;
	      EZ_ClearWidgetSizeComputedFlag(before);
	      EZ_SetWidgetWidthSetFlag(before);
	    }
	}
      else
	{
	  cy = EZ_WidgetOriginY(widget) - Oy;
	  if(cy < 0)
	    {
	      int hh = EZ_WidgetHeight(before);
	      if(hh + cy < 16) cy = 16 - hh;
	      EZ_WidgetHeightHint(before) = hh + cy;
	      EZ_ClearWidgetSizeComputedFlag(before);
	      EZ_SetWidgetHeightSetFlag(before);
	      
	      hh = EZ_WidgetHeight(after);
	      EZ_WidgetHeightHint(after) = hh - cy;
	      EZ_ClearWidgetSizeComputedFlag(after);
	      EZ_SetWidgetHeightSetFlag(after);				  
	    }
	  else
	    {
	      int hh = EZ_WidgetHeight(after);
	      if(hh - cy < 16) cy = hh - 16;				  
	      EZ_WidgetHeightHint(after) = hh - cy;
	      EZ_ClearWidgetSizeComputedFlag(after);
	      EZ_SetWidgetHeightSetFlag(after);				  
	      hh = EZ_WidgetHeight(before);
	      EZ_WidgetHeightHint(before) = hh + cy;
	      EZ_ClearWidgetSizeComputedFlag(before);
	      EZ_SetWidgetHeightSetFlag(before);
	    }
	}
      EZ_ClearWidgetSizeComputedFlag(parent);
      EZ_ReDisplayWidget(parent);
      if(before) EZ_WidgetFlags(before) = Bflag;
      if(after) EZ_WidgetFlags(after) = Aflag;
    }
  else
    {
      EZ_WidgetOriginX(widget) = Ox;
      EZ_WidgetOriginY(widget) = Oy;
      EZ_ClearWidgetSizeComputedFlag(widget);
      EZ_ReDisplayWidget(widget);
    }
}
/******************************************************************/
#undef _EZ_WIDGET_PANE_HANDLE_C_
