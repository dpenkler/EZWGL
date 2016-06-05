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
 ***              Menu Bar                                     ***
 ***                                                           ***
 *****************************************************************/
#define _EZ_WIDGET_MBAR_C_

#include "EZ_Widget.h"

/*********************************************************************
 * 
 *  Functions implemented in this file:
 */
EZ_Widget        *EZ_CreateMBar MY_ANSIARGS((EZ_Widget *parent));
void              EZ_DrawWidgetMBar MY_ANSIARGS((EZ_Widget *widget));
void              EZ_ComputeWidgetMBarSize MY_ANSIARGS((EZ_Widget *widget, int *w, int *h));
void              EZ_MBarEventHandle MY_ANSIARGS((EZ_Widget *widget, XEvent *event));

EZ_Widget        *EZ_MenuBarAddItemAndMenu MY_ANSIARGS((EZ_Widget *widget, char *label, int ul, EZ_Widget *menu));
void              EZ_MenuBarSetItemMenu MY_ANSIARGS((EZ_Widget *widget, char *label, EZ_Widget *menu));
EZ_Widget         *EZ_MenuBarFindItemAtXY MY_ANSIARGS(( EZ_Widget *widget, int x, int y));
EZ_Widget         *EZ_MenuBarFindItemAndMenuAtXY MY_ANSIARGS(( EZ_Widget *widget, int x, int y, EZ_Widget **ret));
EZ_Widget         *EZ_MenuBarFindItem MY_ANSIARGS(( EZ_Widget *widget, char *name));
EZ_Widget         *EZ_MenuBarFindItemAndMenu MY_ANSIARGS(( EZ_Widget *widget, char *name, EZ_Widget **ret));
void              EZ_MenuBarDeleteItem MY_ANSIARGS((EZ_Widget *widget, char *label));
void              EZ_MenuBarDeleteItemAndMenu MY_ANSIARGS((EZ_Widget *widget, char *label));
void              EZ_MenuBarInsertItemAndMenu MY_ANSIARGS((EZ_Widget *mbar, char *label, int ul, EZ_Widget *menu,
							   char *ref, int where));
static EZ_Widget  *EZ_FindMBarMenu MY_ANSIARGS(( EZ_Widget *widget, int x, int y, EZ_Widget **l_ret));
static EZ_Widget  *FindAdjacentItem MY_ANSIARGS(( EZ_Widget *widget, int dir));
/*********************************************************************
 * 
 *  Local Variables.
 */
static EZ_WidgetHandle EZ_MBarHandle =
{ 
  EZ_ComputeWidgetMBarSize,
  EZ_DrawWidgetMBar,
  EZ_FreeWidgetFrameData,
  EZ_MBarEventHandle,
};

/*********************************************************************/

EZ_Widget  *EZ_CreateMBar(parent)
     EZ_Widget  *parent;     /* parent widget    */

{
  EZ_Widget  *wptr;

  wptr = EZ_CreateNewWidget(parent);
  /*--------------------------------------------------
   * Register the handling functions for CButton.
   *  has to be done after  EZ_CreateNewWidget.
   *-------------------------------------------------*/
  EZ_WidgetHandlingFunctions[EZ_WIDGET_MENU_BAR] = &EZ_MBarHandle;

  EZ_SetWidgetTypeAndNames(wptr,  EZ_WIDGET_MENU_BAR);
  EZ_WidgetFont(wptr) = EZ_GetFontFromId(EZ_BUTTON_FONT);
  EZ_WidgetStacking(wptr) = EZ_HORIZONTAL_LEFT;
  EZ_WidgetFillMode(wptr) = EZ_FILL_HORIZONTALLY;
  EZ_WidgetPadX(wptr) = EZ_WIDGET_DEFAULT_PADX;
  EZ_WidgetPadY(wptr) = 2;
  EZ_WidgetSepX(wptr) = EZ_WIDGET_DEFAULT_PADX;
  EZ_WidgetBorderWidth(wptr) = 2;
  EZ_WidgetBorderStyle(wptr)  = EZ_BORDER_UP;
  EZ_SetWidgetHeightSetFlag(wptr);
  EZ_MBarLastItem(wptr) = NULL;
  EZ_SetWidgetExpandFlag(wptr);

  /* copyed from Frame */
  EZ_FramePixmap(wptr) = (EZ_Bitmap *)NULL;
  EZ_FrameFont(wptr) = EZ_GetFontFromId(EZ_LABEL_FONT);  
  EZ_FrameLabel(wptr) = EZ_AllocCopyString(NULL);
  EZ_FrameLabelLength(wptr) = 0;
  EZ_FrameLineLength(wptr) = EZ_DEFAULT_LABEL_LINE_LENGTH;
  EZ_FrameNLines(wptr) = 1; 
  EZ_FrameTWidth(wptr) = 0;
  EZ_FrameTHeight(wptr) = 0;
  EZ_FrameFontAscent(wptr)= 0;
  EZ_FrameXIndent(wptr) = EZ_WIDGET_DEFAULT_PADX;
  EZ_FrameYIndent(wptr) = EZ_WIDGET_DEFAULT_PADY;
  EZ_FrameHasLabel(wptr) = 0;
  EZ_FrameWAdjust(wptr) = 0;
  EZ_FrameIntBdWidth(wptr) = 1;
  EZ_FrameIntBdType(wptr)  = EZ_BORDER_FRAMED_DOWN;
  EZ_FrameBrokenIntBdLength(wptr) = 0;

  return(wptr);
}

/********************************************************************
 *
 *  Figure out the dimension of a menu button
 */
void EZ_ComputeWidgetMBarSize(widget, w, h)
     EZ_Widget *widget;
     int       *w, *h;
{
  int  cw, ch, tmp;
  
  if(!(EZ_GetWidgetSizeComputedFlag(widget)))
    {
      int draghandle = EZ_FrameDragHandle(widget);
      switch(draghandle)
	{
	case EZ_LEFT:
	  EZ_WidgetAuxXLeft(widget) = 8;
	  EZ_FrameDHXL(widget) = 8;
	  EZ_FrameDHXR(widget) = 0;
	  EZ_FrameDHY(widget) = 0;
	  break;
	case EZ_RIGHT:
	  EZ_WidgetAuxXRight(widget) = 8;
	  EZ_FrameDHXL(widget) = 0;
	  EZ_FrameDHXR(widget) = 8;
	  EZ_FrameDHY(widget) = 0;
	  break;
	case EZ_TOP:
	  EZ_WidgetHAdjust(widget) = 8;
	  EZ_FrameDHXL(widget) = 0;
	  EZ_FrameDHXR(widget) = 0;
	  EZ_FrameDHY(widget) = 8;
	  break;
	default:
	  break;
	}
      EZ_SetWidgetSizeComputedFlag(widget); 
    }
  tmp =  EZ_WidgetBorderWidth(widget) + EZ_WidgetPadB(widget);
  cw = EZ_WidgetPadX(widget) + tmp +  EZ_WidgetAuxXLeft(widget);
  ch = EZ_WidgetPadY(widget) + tmp +  EZ_WidgetAuxY(widget);
  cw = cw + cw + 16;
  ch = ch + ch + 6;
  
  *w = cw;
  *h = ch;
}

/******************************************************************************
 *
 *  Draw a menu button
 */

void EZ_DrawWidgetMBar(wptr)
     EZ_Widget *wptr;
{
  int             w, h, padb, padb2, draghandle; 
  Pixmap          pixmap;
  Window          win;
  GC              gc;
  unsigned long   bgpv;

  win = EZ_WidgetWindow(wptr);
  w   = EZ_WidgetWidth(wptr);
  h   = EZ_WidgetHeight(wptr);
  padb = EZ_WidgetPadB(wptr);
  padb2 = padb + padb;
  draghandle = EZ_FrameDragHandle(wptr);

  /*-----------------------------------------------------------
   *  Create a pixmap, draw into this pixmap in background and
   *  copy to the button window when finished.
   *----------------------------------------------------------*/
  pixmap = XCreatePixmap(EZ_DisplayForWidgets, win, w, h, EZ_DepthForWidgets);    
  EZ_FixFocusHighlightBorder(wptr, pixmap, w, h, padb);

  if(EZ_GetBackgroundGC(wptr, &gc, &bgpv, 1,0))
    {
      int ox, oy;
      EZ_GetWidgetTileOrigin(wptr, &ox, &oy);      
      XSetTSOrigin(EZ_Display, gc, ox, oy);
    }
  XFillRectangle(EZ_DisplayForWidgets, pixmap, gc, padb,padb, w-padb2, h-padb2); 

  /*============================================================
   * menu labels are NW labels.
   */
  {
    EZ_Widget *children = EZ_WidgetChildren(wptr);
    if(children && EZ_WidgetType(children) == EZ_WIDGET_NW_LABEL)
      {
	EZ_DisplayNWLabel(children, pixmap, EZ_WidgetForeground(wptr), bgpv,
			  EZ_GetWidgetDisabledFlag(wptr), 0,
			  0, 0, 0, 1);
      }
  }
  /*============================================================*/  
  if(draghandle)
    {
      int tmp, jmp, dhxl,dhxr,dhy, bwpad,x,y;
      bwpad = EZ_WidgetBorderWidth(wptr) + padb;
      dhxl = EZ_FrameDHXL(wptr);
      dhxr = EZ_FrameDHXR(wptr);
      dhy = EZ_FrameDHY(wptr);

      switch(draghandle)
	{
	case EZ_LEFT:
	  y = bwpad +1;
	  x = bwpad +1;
	  tmp = h - (y+y);
	  if( EZ_GetNumberOfActiveChildren(wptr) == 0)
	    { jmp = w -(x+x); if(jmp > 16) jmp = 16; }
	  else jmp = 6;
	  EZ_OverlayDragablePixmap(wptr, pixmap, x, y, jmp, tmp);
	  break;
	case EZ_RIGHT:
	  y = bwpad;
	  tmp = h - (y+y);
	  if( EZ_GetNumberOfActiveChildren(wptr) == 0)
	    {
	      x = bwpad + 1;
	      jmp = w -(x+x);
	      if(jmp > 16) jmp = 16;
	      x = w - jmp - x;
	    }
	  else
	    {
	      x = w -(bwpad + 8);
	      jmp = 6;
	    }
	  EZ_OverlayDragablePixmap(wptr, pixmap, x, y, jmp, tmp);
	  break;
	case EZ_TOP:
	  y = bwpad + 1;
	  x = bwpad + 1;
	  tmp = w - (x+x);
	  if( EZ_GetNumberOfActiveChildren(wptr) == 0) 
	    { jmp = h-(y+y); if(jmp > 16) jmp = 16;}
	  else jmp = 6;
	  EZ_OverlayDragablePixmap(wptr, pixmap, x, y, tmp, jmp);
	  break;
	default:
	  break;
	}
    }
  /*------------------------------------
   *  Now the real border.
   *-----------------------------------*/
  EZ_DrawRectBorder(wptr, pixmap);
  XCopyArea(EZ_DisplayForWidgets,pixmap,win, EZ_WRITABLEGC,0,0,w,h,0,0); 
  XFreePixmap(EZ_DisplayForWidgets, pixmap); 
}

/*************************************************************************
 *
 *  Event handling, tricky!
 */

void  EZ_MBarEventHandle(widget, event)
     EZ_Widget *widget;
     XEvent    *event;
{
  EZ_Widget *mmenu, *theBtn = NULL;
  int bx, by;

  if(widget == (EZ_Widget *)NULL) return;
  if(event->type == Expose)   EZ_DrawWidget(widget);      
  if( EZ_GetWidgetDisabledFlag(widget) ) return;

  switch(event->type)
    {
    case ButtonPress:
      if(event->xbutton.button == EZ_Btn1)      
	{
	  int w = EZ_WidgetWidth(widget);
	  int h = EZ_WidgetHeight(widget);
	  bx = event->xbutton.x;
	  by = event->xbutton.y;

	  if(EZ_FrameDetachable(widget))
	    {
	      int  onhandle, corner = 0;
	      onhandle = EZ_OnFrameDragHandle(widget, bx, by);
	      if(EZ_GetWidgetDetachedFlag(widget))	      
		corner =  EZ_XYIsAtCorner(bx,by,w,h);
	      /* if pointer is on the drag handle */
	      if(onhandle | corner)
		{
		  if(corner)
		    {
		      int x0 = EZ_WidgetOriginX(widget);
		      int y0 = EZ_WidgetOriginY(widget);
		      int x1,y1,x2, y2;
                      switch(corner)
                        {
                        case XC_top_left_corner:
			  x1 = x0; y1 = y0; 
			  x2 = x0 +w; y2 = y0 +h;
                          break;
                        case XC_top_right_corner:
			  x1 = x0 +w; y1 = y0;
			  x2 = x0; y2 = y0 +h;
                          break;
                        case XC_bottom_right_corner:
			  x2 = x0; y2 = y0; 
			  x1 = x0 +w; y1 = y0 +h;
                          break;
                        case XC_bottom_left_corner:
			  x2 = x0 +w; y2 = y0;
			  x1 = x0; y1 = y0 +h;			  
                          break;
                        default:
                          { x1=x2=y1=y2 = 0;}
                          break;
                        }
                      {
			int     x,y, dx,dy, rootx, rooty, oldx, oldy;
			int     dflag = 0;
			int     rx, ry,rw,rh;
			XEvent  xevent;
			Window  root, child, win;
			unsigned int    mask;
			GC       gc = EZ_XORGC;

			win = RootWindow(EZ_Display, EZ_ScreenNum);
			XQueryPointer(EZ_Display, RootWindow(EZ_Display, EZ_ScreenNum),
				      &root,       /* root return win */
				      &child,      /* child ret win   */
				      &rootx, &rooty,   /* x, y in root    */
				      &x,&y,            /* x, y in win     */
				      &mask);  
			oldx = rootx; oldy = rooty;
			rw = x1 - x2; if(rw<0) rw = -rw;
			rh = y1 - y2; if(rh<0) rh = -rh;
			rx = x2 < x1 ? x2: x1;
			ry = y2 < y1 ? y2: y1;
			while(1)
			  {
			    XNextEvent(EZ_Display, &xevent); 
			    EZ_FilterEvent(&xevent);
                            EZ_InvokePrivateEventHandler(&xevent);
			    if(xevent.type == Expose) EZ_WidgetDispatchEvent(&xevent);
			    if(xevent.type == MotionNotify)
			      {
				if(dflag) 
				  {
				    XDrawRectangle(EZ_Display, win, gc, rx,ry,rw, rh);
				    dflag = 0;
				  }
				while (XCheckTypedEvent(EZ_Display, MotionNotify, &xevent))
				  EZ_FilterEvent(&xevent);
				XQueryPointer(EZ_Display, RootWindow(EZ_Display, EZ_ScreenNum),
					      &root,       /* root return win */
					      &child,      /* child ret win   */
					      &rootx, &rooty,   /* x, y in root    */
					      &x,&y,            /* x, y in win     */
					      &mask);  
				dx = rootx - oldx;  dy = rooty - oldy;
				oldx = rootx; oldy = rooty;

				x1 += dx; y1 += dy;
				rw = x1 - x2; if(rw<0) rw = -rw;
				rh = y1 - y2; if(rh<0) rh = -rh;
				rx = x2 < x1 ? x2: x1;
				ry = y2 < y1 ? y2: y1;
				XDrawRectangle(EZ_Display, win, gc, rx,ry,rw, rh);
				dflag = 1;
			      }
			    else if(xevent.type == ButtonRelease)
			      {
				if(dflag) XDrawRectangle(EZ_Display, win, gc, rx,ry,rw, rh);
				EZ_WidgetOriginX(widget) = rx;
				EZ_WidgetOriginY(widget) = ry;
				rw = (rw > 0 ? rw : 10);
				rh = (rh > 0 ? rh : 10);
				EZ_WidgetWidth(widget) = rw;
				EZ_WidgetHeight(widget) = rh;
				    
				EZ_ClearWidgetSizeComputedFlag(widget);      
				EZ_ReDisplayWidget(widget); 
                                XRaiseWindow(EZ_Display,EZ_WidgetWindow(widget));      
				break;
			      }
			  }
		      }
		      return; /* done resizing, return */
		    } /* already detached */

		  /* if detached, move window if ptr is on handle 
		   * otherwise, detach and move window 
		   */
		  if(onhandle)
		    {
		      int     x,y, dx,dy, rootx, rooty, oldx, oldy;
		      int     done, moved, detached, grabed;
		      XEvent  xevent;
		      Window  root, child;
		      unsigned int    mask;


		      done = 0;
		      moved = 0;
		      grabed = 0;
		      XQueryPointer(EZ_Display, RootWindow(EZ_Display, EZ_ScreenNum),
				    &root,       /* root return win */
				    &child,      /* child ret win   */
				    &oldx, &oldy,   /* x, y in root    */
				    &x,&y,            /* x, y in win     */
				    &mask);  
		      detached = EZ_GetWidgetDetachedFlag(widget);
                      XDefineCursor(EZ_Display, EZ_WidgetWindow(widget),
                                    EZ_GetCursor(EZ_C_HAND2));

		      while(!done)
			{
			  XNextEvent(EZ_Display, &xevent);  EZ_FilterEvent(&xevent);
                          EZ_InvokePrivateEventHandler(&xevent);
			  if(xevent.type == Expose || xevent.type == ConfigureNotify)
                            EZ_WidgetDispatchEvent(&xevent);
                          
			  if(xevent.type == MotionNotify)
			    {
			      if(detached == 0) 
				{
				  EZ_DetachFrame(widget,0,0);
				  XSync(EZ_Display, False);
				  XGrabPointer(EZ_Display, EZ_WidgetWindow(widget), True, 
					       ButtonReleaseMask|ButtonPressMask|PointerMotionMask,
					       GrabModeAsync, GrabModeAsync, None, 
					       EZ_GetCursor(EZ_C_HAND2), CurrentTime);
				  grabed = 1;
				  detached = 1;
				}
			      XQueryPointer(EZ_Display, RootWindow(EZ_Display, EZ_ScreenNum),
					    &root,       /* root return win */
					    &child,      /* child ret win   */
					    &rootx, &rooty,   /* x, y in root    */
					    &x,&y,            /* x, y in win     */
					    &mask);  
			      dx = rootx - oldx;  dy = rooty - oldy;
			      if(dx|dy)
				{
				  EZ_WidgetOriginX(widget) += dx;
				  EZ_WidgetOriginY(widget) += dy;
				  EZ_WidgetMaped(widget) = 3; 
				  EZ_ReDisplayWidget(widget);
				  oldx = rootx;   oldy = rooty;
				}
			      while(XCheckTypedEvent(EZ_Display, MotionNotify, &xevent))
				EZ_FilterEvent(&xevent);
			    }
			  else if(xevent.type == ButtonRelease)
			    {
			      if(grabed) {XUngrabPointer(EZ_Display, CurrentTime);}
			      done = 1;
			    }
			}
		      if(detached) EZ_DockFrame(widget,0);
		    }
                  XDefineCursor(EZ_Display, EZ_WidgetWindow(widget),
                                EZ_WidgetCursor(widget));     
		  return;
		}
	    }
	  /* Pointer is not on the drag handle, try to find and post a menu */
	  mmenu = EZ_FindMBarMenu(widget, bx, by, &theBtn);
	  
	  if(EZ_CurrentPopupMenu != (EZ_Widget *)NULL)
	    {
	      if(mmenu == EZ_CurrentPopupMenu) return;
	      else EZ_HideCurrentPopupMenu();
	    }
	  /*-----------------------------------------------------
	   * Now we do a popup. Grab the pointer and wait for
	   * a selection.
	   *----------------------------------------------------*/
	startagain:
	  if( mmenu  && EZ_WidgetNumChildren(mmenu) == 0) mmenu = NULL;
	  if(mmenu == NULL) return;
	  
	  if(1)
	    { 
	      int          srx, sry, rx,ry,x,y, itmp;
	      unsigned int mask;
	      Window       root,win;
	      
	      /* first set the background ptr */
	      if(mmenu) EZ_PropagateBgInfo2Widget(theBtn, mmenu);
	      
	      /*-----------------------------------------------------------
	       * find out the position to popup the menu and do the popup.
	       *----------------------------------------------------------*/
	      if(mmenu)
		{
		  XQueryPointer(EZ_Display, EZ_WidgetWindow(widget),
				&root,                                    /* root return win */
				&win,                                     /* child ret win   */
				&rx, &ry,                                 /* x, y in root    */
				&x,&y,                                    /* x, y in win     */
				&mask
				);
                  srx = EZ_WidgetOriginY(theBtn) + EZ_WidgetHeight(theBtn) + 2;
		  sry = EZ_WidgetHeight(widget);
                  if(sry > srx) itmp = srx; else itmp = sry;
		  srx = rx = rx - x + EZ_WidgetOriginX(theBtn);
		  sry = ry = ry -y + itmp;
		  {
		    int ww, hh;
		    if( !(EZ_GetWidgetSizeComputedFlag(mmenu)) )
		      {
			EZ_MarkAllChildrenWidgetSizeChanged(mmenu,0);
			EZ_ComputeWidgetWindowSize(mmenu, &ww, &hh, NULL);
		      }
		    else
		      {
			ww = EZ_WidgetWidth(mmenu);
			hh = EZ_WidgetHeight(mmenu);
		      }
		    if(rx < 0) rx = 0;
		    else if(rx + ww > EZ_XDisplayWidth)  rx = (int)EZ_XDisplayWidth - ww;
		    if(ry < 0) ry = 0;
		    else if(ry + hh > EZ_XDisplayHeight) ry = (int)EZ_XDisplayHeight - hh;
		  }
		  EZ_SetWidgetHighlightFlag(theBtn);
                  { EZ_InstallHighlightColor(theBtn); } /*4/29/99*/
		  EZ_MBarLastItem(widget) = theBtn;
		  EZ_WidgetBorderStyle(theBtn) = EZ_BORDER_UP;
		  EZ_DrawWidget(widget);
                  EZ_PopupInitiator(mmenu) = widget;
                  EZ_PopupXoff(mmenu) = rx - srx + EZ_WidgetOriginX(theBtn);
                  EZ_PopupYoff(mmenu) = EZ_WidgetHeight(widget) + ry -sry;
		  EZ_SetupPopup(mmenu);
		  EZ_DisplayPopup(mmenu, rx, ry);
		}
	      /*
	       * Now wait until a selection is made.
	       */
	      {
		XEvent          xevent;
		Window          event_window;
		EZ_Widget *tmp, *cmenu, *the_popup = EZ_CurrentPopupMenu;
		EZ_Widget *shortcutItem, *CurrentItem;
		EZ_Widget *out, *sitem;
		int             done = 0, xx=0, yy=0;
		
		XGrabPointer(EZ_Display,
			     EZ_WidgetWindow(the_popup),
			     True,
			     ButtonPressMask|ButtonReleaseMask|ButtonMotionMask|PointerMotionMask,
			     GrabModeAsync,
			     GrabModeAsync, 
			     None,
			     EZ_GetCursor(EZ_C_RIGHT_PTR),
			     CurrentTime);
	      
		/*
		 * fvwm eats some keystrokes, so...
		 */

		XGrabKeyboard(EZ_Display,
			      EZ_WidgetWindow(the_popup),
			      True,
			      GrabModeAsync,
			      GrabModeAsync, 
			      CurrentTime);
		
		cmenu = the_popup;
		/* if the menu is poped via key press, select the first item */
		if(event->xbutton.send_event)
		  {
		    CurrentItem = EZ_FirstMenuItem(cmenu);
		    event->xbutton.send_event = False;
		  }
		else CurrentItem = NULL;
		EZ_PopupLink2(cmenu) = CurrentItem;
		while(1)
		  {
		    EZ_TFEvents();
		    XNextEvent(EZ_Display, &xevent);
		    EZ_FilterEvent(&xevent);
                    EZ_InvokePrivateEventHandler(&xevent);
		    event_window = xevent.xany.window;
		    tmp = EZ_LookupWidgetFromMappedHT(event_window);

		    /*---------------------------------------------------------
		     * Handle KeyPress first.
		     *--------------------------------------------------------*/
		    if(xevent.type == KeyPress)
		      {
#define TEMP_BUFFER_SIZE    32
			int               count,modifiers;
			KeySym            keysym;
			XComposeStatus    compose; 
			char              tmpbuffer[TEMP_BUFFER_SIZE];
			int               buffersize = TEMP_BUFFER_SIZE;
			int               x_move, y_move;
#undef TEMP_BUFFER_SIZE
			shortcutItem = NULL;
			sitem = NULL;
			out = NULL;
			modifiers = xevent.xkey.state & (ShiftMask | ControlMask | Mod1Mask);
			
			xevent.xkey.state &= ~modifiers;
			count = XLookupString(&(xevent.xkey), tmpbuffer, buffersize, &keysym, &compose);
			tmpbuffer[count] = '\0'; 


			if(modifiers == 0 || count == 0 ||
			   (EZ_LookForPopupShortCuts(EZ_CurrentPopupMenu,
						     modifiers, tmpbuffer,&x_move, &y_move,
						     &shortcutItem) == 0 &&
			    EZ_GetGlobalKeyEvent(modifiers, tmpbuffer, NULL, &out, &xx, &yy) == 0))
			  {
			    switch(keysym)
			      {
			      case XK_Up:  case XK_k:  case XK_p: case XK_K:  case XK_P:
#ifdef XK_KP_Up
			      case XK_KP_Up:
#endif				
				CurrentItem = EZ_MoveMenuSelection(cmenu, CurrentItem, 1);
				EZ_PopupLink2(cmenu) = CurrentItem;
				break;
			      case XK_Down: case XK_n: case XK_j: case XK_N: case XK_J:  
#ifdef XK_KP_Down
			      case XK_KP_Down:
#endif
				CurrentItem = EZ_MoveMenuSelection(cmenu, CurrentItem, -1);
				EZ_PopupLink2(cmenu) = CurrentItem;
				break;
			      case XK_Tab: 
#ifdef XK_KP_Tab
			      case XK_KP_Tab:
#endif
				CurrentItem = EZ_MoveMenuSelection(cmenu, CurrentItem, 
								   (modifiers&ShiftMask)==0? -1: 1);
				EZ_PopupLink2(cmenu) = CurrentItem;
				break;
			      case XK_Left: case XK_b:  case XK_h: case XK_B:  case XK_H: 
#ifdef XK_KP_Left
			      case XK_KP_Left:
#endif
				{
				  EZ_Widget *tmpa = EZ_PopupLink1(cmenu);
				  if(tmpa)
				    {
				      cmenu = tmpa;
				      EZ_DeselectCurrentMenuItem(CurrentItem);
				      CurrentItem = EZ_MoveMenuSelection(cmenu, EZ_PopupLink2(cmenu), 0);
				    }
				  else
				    {
				      EZ_Widget *tmpBB, *mu, *bn = NULL;
				      tmpBB = FindAdjacentItem(theBtn, -1);
				      if(tmpBB && tmpBB != theBtn)
					{
					  int cx = EZ_WidgetOriginX(tmpBB);
					  int cy = EZ_WidgetOriginY(tmpBB);
					  mu = EZ_FindMBarMenu(widget, cx, cy, &bn);
					  if(mu)
					    {
                                              EZ_HideCurrentPopupMenuA();
                                              XUngrabPointer(EZ_Display, CurrentTime); 
                                              XUngrabKeyboard(EZ_Display,CurrentTime); 
					      EZ_ClearWidgetHighlightFlag(theBtn);
                                              { EZ_UninstallHighlightColor(theBtn); } /*4/29/99*/
					      EZ_WidgetBorderStyle(theBtn) = EZ_BORDER_NONE;
					      theBtn = bn;
					      mmenu = mu;
					      goto startagain;
					    }
					}
				      }
				}
				break;
			      case XK_Right: case XK_f: case XK_l: case XK_F: case XK_L: 
#ifdef XK_KP_Right
			      case XK_KP_Right:
#endif
				if(CurrentItem && EZ_WidgetType(CurrentItem) == EZ_WIDGET_MENU_SUBMENU)
				  {
				    EZ_Widget *tmenu = EZ_SubMenuTheMenu(CurrentItem);
				    if(tmenu )
				      {
					if(! EZ_WidgetMapped(tmenu)) EZ_DisplayPopupSubMenuOf(CurrentItem);
					EZ_DeselectCurrentMenuItem(CurrentItem);
					EZ_PopupLink1(tmenu) = cmenu;
					cmenu = tmenu;
					CurrentItem = EZ_MoveMenuSelection(cmenu, EZ_PopupLink2(cmenu), 0);
					EZ_PopupLink2(cmenu) = CurrentItem;			    
				      }
				  }
				else
				  {
				    EZ_Widget *tmpBB, *mu, *bn = NULL;
				    tmpBB = FindAdjacentItem(theBtn, 1);
				    if(tmpBB && tmpBB != theBtn)
				      {
					int cx = EZ_WidgetOriginX(tmpBB);
					int cy = EZ_WidgetOriginY(tmpBB);
					mu = EZ_FindMBarMenu(widget, cx, cy, &bn);
					if(mu)
					  {
                                            EZ_HideCurrentPopupMenuA();
                                            XUngrabPointer(EZ_Display, CurrentTime); 
                                            XUngrabKeyboard(EZ_Display,CurrentTime); 
					    EZ_ClearWidgetHighlightFlag(theBtn);
                                            { EZ_UninstallHighlightColor(theBtn); } /*4/29/99*/
					    EZ_WidgetBorderStyle(theBtn) = EZ_BORDER_NONE;
					    theBtn = bn;
					    mmenu = mu;
					    goto startagain;
					  }
				      }
				  }
				break;
			      case XK_Return: case XK_Linefeed: case XK_space:  
#ifdef XK_KP_Enter
			      case XK_KP_Enter:
#endif
				if(CurrentItem && EZ_WidgetType(CurrentItem) == EZ_WIDGET_MENU_SUBMENU)
				  {
				    EZ_Widget *tmenu = EZ_SubMenuTheMenu(CurrentItem);
				    if(tmenu)
				      {
					if(! EZ_WidgetMapped(tmenu)) EZ_DisplayPopupSubMenuOf(CurrentItem);
					EZ_PopupLink1(tmenu) = cmenu;
					cmenu = tmenu;
					CurrentItem = EZ_MoveMenuSelection(cmenu, EZ_PopupLink2(cmenu), 0);
					EZ_PopupLink2(cmenu) = CurrentItem;			    
				      }
				  }
				else
				  {
				    sitem = CurrentItem;
				    done = 1;
				  }
				break;	
                              case XK_Escape:
                                sitem = NULL;
                                done = 1;    
                                break;
			      default:
				break;
			      }
			  }
			/*--------------------------------------------
			 * if a selection has been made, finish up
			 *-------------------------------------------*/
			if(shortcutItem) 
			  {
			    EZ_DeselectCurrentMenuItem(CurrentItem);
                            if(EZ_WidgetType(shortcutItem) == EZ_WIDGET_MENU_SUBMENU)
                              {
                                EZ_Widget *tmenu = EZ_SubMenuTheMenu(shortcutItem);
                                if(tmenu )
                                  {
                                    if(!EZ_WidgetMapped(tmenu)) EZ_DisplayPopupSubMenuOf(shortcutItem);
                                    EZ_DeselectCurrentMenuItem(shortcutItem);
                                    EZ_PopupLink1(tmenu) = cmenu;
                                    cmenu = tmenu;
                                    CurrentItem = EZ_MoveMenuSelection(cmenu, EZ_PopupLink2(cmenu), 0);
                                    EZ_PopupLink2(cmenu) = CurrentItem;			    
                                  }
                              }
                            else sitem = shortcutItem;
			  }
			if(sitem)
			  {
			    done = 1;
			    if(EZ_GetWidgetDisabledFlag(sitem) == 0)
			      {
				int doit = 1;
				
				switch(EZ_WidgetType(sitem))
				  {
				  case EZ_WIDGET_MENU_CHECK_BUTTON:
				    if( EZ_CButtonOn(sitem) )
				      {  EZ_CButtonSetOffValue(sitem);}
				    else { EZ_CButtonSetOnValue(sitem);}
				    break;
				  case EZ_WIDGET_MENU_RADIO_BUTTON:	  
				    if( !( EZ_RButtonOn(sitem)) )
				      {
					EZ_RButtonList  *friends;
					EZ_Widget *old = (EZ_Widget *)NULL;
					friends = EZ_RButtonGroup(sitem)->list;
					while(friends)
					  {
					    if( EZ_RButtonOn( friends->rbutton) )
					      {
						old = friends->rbutton;  /* currently checked  */
						break;
					      }
					    friends = friends->next;
					  }
					EZ_RButtonSetValue(sitem);
					if(old && EZ_WidgetMapped(old)) EZ_DrawWidget(old);
				      }
				    break;
				  case EZ_WIDGET_MENU_NORMAL_BUTTON:
				  case EZ_WIDGET_MENU_TEAR_OFF_BAR:
				    break;
				  case EZ_WIDGET_MENU_SUBMENU:
				    EZ_DisplayPopupSubMenuOf(sitem);
				    doit = 0;
				    done = 0;
				    break;
				  default:
				    doit = 0;
				    break;
				  }
				if(doit)
				  {
				    EZ_WidgetBorderStyle(sitem) = EZ_BORDER_UP;
				    EZ_DrawWidget(sitem);
				    
				    if(EZ_CurrentPopupMenu != (EZ_Widget *)NULL)
				      {  /* return value */
					EZ_WidgetRetData(EZ_CurrentPopupMenu) = EZ_WidgetRetData(sitem);
				 	/* this is the selected item     */
					EZ_PopupLSItem(EZ_CurrentPopupMenu) = sitem;  
				      }
				  }
			      }
			  }
			else if(out) /* get out */
			  {
			    XButtonEvent xbevent;
			    xbevent.type = ButtonRelease;
			    xbevent.button = EZ_Btn1;
			    xbevent.display = EZ_Display;
			    xbevent.window = EZ_WidgetWindow(out);
			    xbevent.time = CurrentTime;
			    xbevent.x=xx; xbevent.y=yy;
			    XSendEvent(EZ_Display, EZ_WidgetWindow(out),
				       False, ButtonReleaseMask, (XEvent *)&xbevent);
			    EZ_HandleGlobalKeyEvent(out, xx, yy);
			    done = 1;
			  }
		      } /* KeyPress */
		    else
		      {
			/*---------------------------------------------------------
			 *  Non KeyPress Events.
			 *--------------------------------------------------------*/
			if(tmp == (EZ_Widget *)NULL) continue;
			else if(tmp == widget) /* event on menubar */
			  {  
			    if(xevent.type == ButtonPress || xevent.type == ButtonRelease)
			      {
				int x = xevent.xbutton.x;
				int y = xevent.xbutton.y;
				EZ_Widget *bn;
				if( x < 0 || x > EZ_WidgetWidth(tmp) ||
				    y < 0 || y > EZ_WidgetHeight(tmp) ||
				    EZ_FindMBarMenu(widget, x, y, &bn) == NULL)
				  done = 1;
			      }
			    else if(xevent.type == MotionNotify)   /* 'cause of the grab */
			      {
				int x = xevent.xmotion.x;
				int y = xevent.xmotion.y;
				EZ_Widget *mu, *bn;
				mu = EZ_FindMBarMenu(widget, x, y, &bn);
				if(mu && mu != mmenu)
				  {
				    EZ_ClearWidgetHighlightFlag(theBtn);
                                    { EZ_UninstallHighlightColor(theBtn); } /*4/29/99*/
				    EZ_WidgetBorderStyle(theBtn) = EZ_BORDER_NONE;
				    EZ_HideCurrentPopupMenuA();
				    XUngrabPointer(EZ_Display, CurrentTime); 
				    XUngrabKeyboard(EZ_Display,CurrentTime); 
				    theBtn = bn;
				    mmenu = mu;
				    event->xbutton.x = x;
				    event->xbutton.y = x;
				    goto startagain;
				  }
			      }
			  }
			else  /* event happend on some other widgets */
			  {
			    if(xevent.type == Expose)
			      EZ_HandleWidgetWindowEvent(tmp, &xevent);
			    else if(xevent.type == EnterNotify)
			      {
				if(EZ_WidgetType(tmp) == EZ_WIDGET_MENU_BUTTON)
				  {
				    EZ_WidgetBorderStyle(theBtn) = EZ_BORDER_NONE;
				    EZ_ClearWidgetHighlightFlag(theBtn);
                                    { EZ_UninstallHighlightColor(theBtn); } /*4/29/99*/
				    EZ_DrawWidget(widget);

				    /* if we get here, we've entered into another
				     * menu-button. Just start over again.
				     */
				    if(xevent.xcrossing.mode != NotifyGrab  &&
				       xevent.xcrossing.mode != NotifyUngrab)
				      {
					EZ_HideCurrentPopupMenuA();
					XUngrabPointer(EZ_Display, CurrentTime); 
					XUngrabKeyboard(EZ_Display,CurrentTime); 
					event->xbutton.window = EZ_WidgetWindow(tmp);
					event->xbutton.x = event->xbutton.y = 1;
					EZ_WidgetBorderStyle(theBtn) = EZ_BORDER_NONE;
					EZ_ClearWidgetHighlightFlag(theBtn);
                                        { EZ_UninstallHighlightColor(theBtn); } /*4/29/99*/
					EZ_DrawWidget(widget);
					EZ_HandleWidgetWindowEvent(tmp, &xevent);
					return;
				      }
				  }
				else if(EZ_GetWidgetIsCloneFlag(tmp) == 0)
				  {
				    int type = EZ_WidgetType(tmp);
				    if(type == EZ_WIDGET_MENU_TEAR_OFF_BAR ||
				       (type >= EZ_WIDGET_MENU_SEPARATOR &&
					type <= EZ_WIDGET_MENU_RADIO_BUTTON))
				      {
					if(xevent.xcrossing.mode != NotifyGrab  &&
					   xevent.xcrossing.mode != NotifyUngrab)
					  {
					    EZ_DeselectCurrentMenuItem(CurrentItem);
					    EZ_HandleWidgetWindowEvent(tmp, &xevent);
					    CurrentItem = tmp;
					  }
				      }
				  }
			      }
			    else if(xevent.type == LeaveNotify)
			      {
				if(EZ_WidgetType(tmp) == EZ_WIDGET_MENU_BUTTON)
				  {
				    EZ_WidgetBorderStyle(tmp) = EZ_BORDER_NONE;
				    EZ_ClearWidgetHighlightFlag(tmp);
                                    { EZ_UninstallHighlightColor(tmp); } /*4/29/99*/
				    EZ_DrawWidget(tmp);
				  }
				else if(EZ_GetWidgetIsCloneFlag(tmp) == 0)
				  {
				    int type = EZ_WidgetType(tmp);
				    if(type == EZ_WIDGET_MENU_TEAR_OFF_BAR ||
				       (type >= EZ_WIDGET_MENU_SEPARATOR &&
					type <= EZ_WIDGET_MENU_RADIO_BUTTON))
				      {
					if(xevent.xcrossing.mode != NotifyGrab  &&
					   xevent.xcrossing.mode != NotifyUngrab)
					  {
					    EZ_DeselectCurrentMenuItem(CurrentItem);
					    CurrentItem = (EZ_Widget *)NULL;
					    EZ_HandleWidgetWindowEvent(tmp, &xevent);
					  }
				      }
				  }
			      }
			    else if(xevent.type == FocusIn || xevent.type == FocusOut)
			      EZ_HandleWidgetWindowEvent(tmp, &xevent);
			    else if(xevent.type == ButtonPress || xevent.type == ButtonRelease)
			      {
				if(xevent.type == ButtonRelease &&
				   (EZ_WidgetType(tmp) != EZ_WIDGET_MENU_SUBMENU)) done = 1;
				if(EZ_GetWidgetIsCloneFlag(tmp) == 0)
				  {
				    int type = EZ_WidgetType(tmp);
				    if(type == EZ_WIDGET_MENU_TEAR_OFF_BAR ||
				       (type >= EZ_WIDGET_MENU_SEPARATOR &&
					type <= EZ_WIDGET_MENU_RADIO_BUTTON))
				      EZ_HandleWidgetWindowEvent(tmp, &xevent);
				  }
			      }
			  }
		      } 
		    if(done) break;
		  }
		EZ_HideCurrentPopupMenu();
		XUngrabPointer(EZ_Display, CurrentTime); 
		XUngrabKeyboard(EZ_Display,CurrentTime); 
		/*------------------------------------------------------
		 * Callbacks. 
		 *  handle popup menu callback first, and
		 *  handle menu-buttoncallback the second.
		 *-----------------------------------------------------*/
		if(the_popup)
		  {
		    EZ_Widget *lsitem = EZ_PopupLSItem(the_popup);
		    if(lsitem)
		      {
			EZ_ClearWidgetHighlightFlag(lsitem);
                        { EZ_UninstallHighlightColor(lsitem); } /*4/29/99*/
			EZ_WidgetBorderStyle(lsitem) = EZ_BORDER_NONE;

			EZ_HandlePopupCallBack(the_popup); /* popup callback */
		      }
		  }
	      }
	    }
	  if(theBtn)
	    {
	      EZ_WidgetBorderStyle(theBtn) = EZ_BORDER_NONE;
	      EZ_ClearWidgetHighlightFlag(theBtn);
              { EZ_UninstallHighlightColor(theBtn); } /*4/29/99*/
	    }
	  EZ_DrawWidget(widget);
	}
      else if(event->xbutton.button == EZ_Btn3)
	{
	  /*
	    #include "EZ_DnDHandler1.c"
	    EZ_DnDEventHandler1(widget, event);
	    */
          EZ_HandleDnDEvents(widget, event);
	}
      else if(event->xbutton.button == EZ_Btn2)
        {
          if(EZ_GetWidgetDetachedFlag(widget))
            {
              int ix = event->xbutton.x;
              int iy = event->xbutton.y;
              int w = EZ_WidgetWidth(widget);
              int h = EZ_WidgetHeight(widget);
              int corner =  EZ_XYIsAtCorner(ix,iy,w,h);
              if(corner)
                {
                  int    x,y, dx,dy, rootx, rooty, oldx, oldy;
                  XEvent  xevent;
                  Window  root, child, win;
                  unsigned int    mask;

                  win = RootWindow(EZ_Display, EZ_ScreenNum);
                  XQueryPointer(EZ_Display, RootWindow(EZ_Display, EZ_ScreenNum),
                                &root,       /* root return win */
                                &child,      /* child ret win   */
                                &rootx, &rooty,   /* x, y in root    */
                                &x,&y,            /* x, y in win     */
                                &mask);  
                  oldx = rootx; oldy = rooty;
                  while(1)
                    {
                      XNextEvent(EZ_Display, &xevent); 
                      EZ_FilterEvent(&xevent);
                      EZ_InvokePrivateEventHandler(&xevent);
                      if(xevent.type == Expose) EZ_WidgetDispatchEvent(&xevent);
                      if(xevent.type == MotionNotify)
                        {
                          XQueryPointer(EZ_Display, RootWindow(EZ_Display, EZ_ScreenNum),
                                        &root,       /* root return win */
                                        &child,      /* child ret win   */
                                        &rootx, &rooty,   /* x, y in root    */
                                        &x,&y,            /* x, y in win     */
                                        &mask);  
                          dx = rootx - oldx;  dy = rooty - oldy;
                          if(dx|dy)
                            {
                              EZ_WidgetOriginX(widget) += dx;
                              EZ_WidgetOriginY(widget) += dy;
                              EZ_WidgetMaped(widget) = 3; 
                              EZ_ReDisplayWidget(widget);
                              oldx = rootx;   oldy = rooty;
                            }
                          while (XCheckTypedEvent(EZ_Display, MotionNotify, &xevent))
                            EZ_FilterEvent(&xevent);
                        }
                      else if(xevent.type == ButtonRelease)
                        {
                          XRaiseWindow(EZ_Display,EZ_WidgetWindow(widget));
                          break;
                        }
                    }
                }
            }
        }
      break;
    case MotionNotify:
      {
	int ix = event->xmotion.x;
	int iy = event->xmotion.y;
	EZ_Widget *bn = NULL, *last, *nmenu;

	if(EZ_FrameDragHandle(widget))
	  {
	    int onoff =  EZ_OnFrameDragHandle(widget, ix, iy);
	    EZ_HighlightFrameDragHandle(widget, onoff);
	    
	    if(EZ_GetWidgetDetachedFlag(widget))
	      {
		int w = EZ_WidgetWidth(widget);
		int h = EZ_WidgetHeight(widget);
                int corner = EZ_XYIsAtCorner(ix,iy,w,h);
		if(corner && event->xmotion.state == 0)
		  {
		    if(EZ_FrameCursorChanged(widget) == 0)
		      {
			EZ_FrameCursorChanged(widget) = 1;
			XDefineCursor(EZ_Display, EZ_WidgetWindow(widget),
				      EZ_GetCursor(corner));
		      }
		  }
		else if(EZ_FrameCursorChanged(widget))
		  {
		    EZ_FrameCursorChanged(widget) = 0;
		    XDefineCursor(EZ_Display, EZ_WidgetWindow(widget),
				  EZ_WidgetCursor(widget));
		  }
	      }
	  }
	nmenu = EZ_FindMBarMenu(widget, ix, iy, &bn);
	/*if(bn != NULL)*/
	  {
	    last = EZ_MBarLastItem(widget);
	    if(bn != last)
	      {
		if(EZ_LookupWidgetFromAllHT(last))
                  {
                    EZ_ClearWidgetHighlightFlag(last);
                    { EZ_UninstallHighlightColor(last); } /*4/29/99*/
                  }

		if(nmenu)
		  {
		    EZ_MBarLastItem(widget) = bn;
		    EZ_SetWidgetHighlightFlag(bn);
                    { EZ_InstallHighlightColor(bn); } /*4/29/99*/
		  }
		else EZ_MBarLastItem(widget) = NULL;
		EZ_DrawWidget(widget);
	      }
	  }
      }
    break;
    case EnterNotify:
      break;
    case LeaveNotify:
      {
	EZ_Widget *last = EZ_MBarLastItem(widget);
	if(EZ_LookupWidgetFromAllHT(last))
	  {
	    EZ_ClearWidgetHighlightFlag(last);
            { EZ_UninstallHighlightColor(last); } /*4/29/99*/
	    EZ_MBarLastItem(widget) = NULL;
	    EZ_DrawWidget(widget);
	  }
	if(EZ_FrameDragHandle(widget))
	  EZ_HighlightFrameDragHandle(widget, 0);
	if(EZ_FrameCursorChanged(widget))
	  {
	    EZ_FrameCursorChanged(widget) = 0;
	    XDefineCursor(EZ_Display, EZ_WidgetWindow(widget),
			  EZ_WidgetCursor(widget));
	  }
      }
    break;
    default:
      break;
    }
}
/********************************************************************************/
void EZ_MenuBarDeleteItem(mbar, label)
     EZ_Widget *mbar; char *label;
{
  EZ_DestroyWidget(EZ_MenuBarFindItem(mbar, label));
}

void EZ_MenuBarDeleteItemAndMenu(mbar, label)
     EZ_Widget *mbar; char *label;
{
  EZ_Widget *item = EZ_MenuBarFindItem(mbar, label);
  if(item)
    {
      EZ_DestroyWidget(EZ_LabelMenu(item));
      EZ_DestroyWidget(item);
    }
}
/********************************************************************************/
EZ_Widget *EZ_MenuBarAddItemAndMenu(mbar, label, ul, menu)
     EZ_Widget *mbar, *menu; char *label; int ul;
{
  if(mbar && EZ_WidgetType(mbar) == EZ_WIDGET_MENU_BAR)
    {
      EZ_Widget *tmp = EZ_CreateNWLabel(mbar, label);
      EZ_WidgetGroupLeader(tmp) = mbar;
      EZ_WidgetBorderWidth(tmp) = 2;
      EZ_WidgetPadX(tmp)  = 4;
      EZ_WidgetPadY(tmp)  = 3;
      if(label && label[0] != '\0') 
        {
          EZ_SetWidgetWidthSetFlag(tmp);
          EZ_WidgetIName(tmp) = EZ_StringToQuark(label);
        }
      else
        EZ_WidgetIName(tmp) = EZ_StringToQuark("NoNameSeparator");

      EZ_WidgetBorderStyle(tmp) = EZ_BORDER_NONE;
      if(label && ul >= 0 && ul < EZ_LabelLineLength(tmp) && isprint(label[ul]))
	{
	  EZ_LabelUnderline(tmp) = ul;
	  EZ_LabelModifiers(tmp) = Mod1Mask;
	  EZ_LabelShortcut(tmp)  = (EZ_LabelString(tmp) + ul);
	  EZ_InsertGlobalKeyPressEvent(tmp, Mod1Mask, (label+ ul));
	}
      EZ_GetAndUseWidgetResources(tmp);
      if(label) EZ_LabelMenu(tmp) = menu;
      return(tmp);
    }
  return(NULL);
}

/************************************************************************************/
void EZ_MenuBarSetItemMenu(mbar, label, menu)
     EZ_Widget *mbar, *menu; char *label;
{
  if(label && mbar && EZ_WidgetType(mbar) == EZ_WIDGET_MENU_BAR)
    {
      EZ_Widget *tmp = EZ_MenuBarFindItem(mbar, label);
      if(tmp) { EZ_LabelMenu(tmp) = menu;}
    }
}
/************************************************************************************/
EZ_Widget *EZ_MenuBarFindItem(mbar, label)
     EZ_Widget *mbar; char *label;
{
  if(mbar && EZ_WidgetType(mbar) == EZ_WIDGET_MENU_BAR)
    {
      EZ_Widget *child = EZ_WidgetChildren(mbar);
      while(child)
	{
	  char *str = EZ_LabelString(child);
	  if(label == NULL) {if(str[0] == '\0') return(child);}
	  else if(!strcmp(label, str)) return(child);
	  child = EZ_WidgetSibling(child);
	}
    }
  return(NULL);
}
EZ_Widget *EZ_MenuBarFindItemAndMenu(mbar, label, ret)
     EZ_Widget *mbar, **ret; char *label;     
{
  EZ_Widget *lab = EZ_MenuBarFindItem(mbar, label);
  if(lab && ret) *ret = EZ_LabelMenu(lab);
  return(lab);
}
/************************************************************************************/
EZ_Widget *EZ_MenuBarFindItemAtXY(widget, x, y)
     EZ_Widget *widget; int x,y;
{
  EZ_Widget *tmp = NULL;
  EZ_FindMBarMenu(widget, x, y, &tmp);
  return(tmp);
}
EZ_Widget *EZ_FindMBarItemAndMenuAtXY(widget, x, y, ret)
     EZ_Widget *widget; int x,y; EZ_Widget **ret;
{
  EZ_Widget *lab = NULL;
  EZ_Widget *menu = EZ_FindMBarMenu(widget, x, y, &lab);
  if(ret) *ret = menu;
  return(lab);
}
/*****************************************************************/
static EZ_Widget *EZ_FindMBarMenu(widget, x, y, nwlab_ret) 
     EZ_Widget *widget; int x,y; EZ_Widget **nwlab_ret; 
{
  if(widget && EZ_WidgetType(widget) == EZ_WIDGET_MENU_BAR)
    {
      EZ_Widget *child = EZ_WidgetChildren(widget);
      while(child)
	{
	  int x0 = EZ_WidgetOriginX(child);
	  int y0 = EZ_WidgetOriginY(child);
	  int w0 = EZ_WidgetWidth(child);	  
	  int h0 = EZ_WidgetHeight(child);	
	  /* printf("[%d %d]x[%d %d]==> %d %d\n",x0, x0+w0, y0, y0+h0, x, y);*/
	  if(x >= x0 && x < x0 + w0 && y >= y0 && y < y0 + h0) break;
	  child = EZ_WidgetSibling(child);
	}
      if(child)
	{
	  if(EZ_WidgetType(child) == EZ_WIDGET_NW_LABEL &&
	     (EZ_GetWidgetNonActiveFlag(child) |EZ_GetWidgetDisabledFlag(child)) == 0)
	    {
	      if(nwlab_ret) *nwlab_ret = child;
	      return(EZ_LabelMenu(child));
	    }
	}
    }
  return(NULL);
}
/************************************************************************************/
void EZ_MenuBarInsertItemAndMenu(mbar, label, ul, menu, ref, where)
     EZ_Widget *mbar, *menu; char *label, *ref; int ul, where;
{
  if(mbar && EZ_WidgetType(mbar) == EZ_WIDGET_MENU_BAR)  
    {
      EZ_Widget *ritem = EZ_MenuBarFindItem(mbar, ref);
      if(ritem == NULL)
	{
	  EZ_Widget *tmp = EZ_MenuBarAddItemAndMenu(mbar, label, ul, menu);
	  if(where < 0) 
	    {
	      EZ_Widget *chase, *head = EZ_WidgetChildren(mbar);
	      if(head != tmp)
		{
		  chase = head;
		  while(EZ_WidgetSibling(chase) != tmp) chase = EZ_WidgetSibling(chase);
		  EZ_WidgetSibling(chase) = NULL;
		  EZ_WidgetSibling(tmp) = head;
		  EZ_WidgetChildren(mbar) = tmp;
		}
	    }
	}
      else
	{
	  EZ_Widget *tmp = EZ_MenuBarAddItemAndMenu(mbar, label, ul, menu);
	  EZ_Widget *chase, *head = EZ_WidgetChildren(mbar);
	  /* remove tmp */
	  chase = head;
	  while(EZ_WidgetSibling(chase) != tmp) chase = EZ_WidgetSibling(chase);
	  EZ_WidgetSibling(chase) = NULL;

	  if(where < 0) 
	    {

	      if(head != ritem)
		{
		  chase = head;
		  while(EZ_WidgetSibling(chase) != ritem) chase = EZ_WidgetSibling(chase);
		  EZ_WidgetSibling(tmp) = ritem;
		  EZ_WidgetSibling(chase) = tmp;
		}
	      else
		{
		  EZ_WidgetSibling(tmp) = ritem;
		  EZ_WidgetChildren(mbar) = tmp;		  
		}
	    }
	  else
	    {
	      EZ_WidgetSibling(tmp) = EZ_WidgetSibling(ritem);
	      EZ_WidgetSibling(ritem) = tmp;		  
	    }	      
	}
    }
}
/************************************************************************************/
static EZ_Widget *FindAdjacentItem(widget, dir)
     EZ_Widget *widget; int dir;
{
  EZ_Widget *parent;
  if(widget && (parent = EZ_WidgetParent(widget)) != NULL)
    {
      if(dir == 1) /* move right */
	{
	  EZ_Widget *next = EZ_WidgetSibling(widget);
	  while(next != widget)
	    {
	      if(next == NULL) next = EZ_WidgetChildren(parent);
	      if(EZ_WidgetType(next) == EZ_WIDGET_NW_LABEL &&
		 EZ_LabelMenu(next) != NULL)
		return(next);
	      else next = EZ_WidgetSibling(next);
	    }
	  return(widget);
	}
      else
	{
	  EZ_Widget *widgets[64];
	  int idx=0, cnt = 0;
	  EZ_Widget *next = EZ_WidgetChildren(parent);
	  while(next)
	    {
	      if(EZ_WidgetType(next) == EZ_WIDGET_NW_LABEL &&
		 EZ_LabelMenu(next) != NULL)	      
		{ 
		  if(next == widget) idx = cnt;
		  widgets[cnt++] = next;
		}
	      next = EZ_WidgetSibling(next);
	    }
	  if(idx > 0) return(widgets[idx-1]);
	  else if(cnt > 0) return(widgets[cnt-1]);
	  else return(widget);
	}
    }
  return(NULL);
}

/************************************************************************************/
#undef _EZ_WIDGET_MBAR_C_
