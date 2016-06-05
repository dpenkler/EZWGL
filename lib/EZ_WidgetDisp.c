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
/*******************************************************************
 *
 *  Map and display the widget window.
 */
#define  _EZ_WIDGET_DISP_C_
#include "EZ_Widget.h"
/*********************************************************************
 * 
 *  Functions implemented in this file:
 */
void                 EZ_DisplayWidget MY_ANSIARGS((EZ_Widget *widget));
void                 EZ_DisplayWidgetUnderPointer MY_ANSIARGS((EZ_Widget *widget, int xoff, int yoff));
void                 EZ_ReDisplayWidget MY_ANSIARGS((EZ_Widget *widget));
void                 EZ_DisplayWidgetWithChoice MY_ANSIARGS((EZ_Widget *widget, int chi, int sib));
void                 EZ_DrawWidget MY_ANSIARGS((EZ_Widget *widget));
void                 EZ_DrawWidgetAndDescendants MY_ANSIARGS((EZ_Widget *widget, int l));
void                 EZ_CreateWindowForWidget MY_ANSIARGS((EZ_Widget *widget, int flag));
void                 EZ_SetWindowShape MY_ANSIARGS((EZ_Widget *widget, int rem_flag));
void                 EZ_ForgetGeometry MY_ANSIARGS((EZ_Widget *widget));
void                 EZ_MakeWidgetWindows MY_ANSIARGS((EZ_Widget *widget));
void                 EZ_FreezeDisplay MY_ANSIARGS((void));
void                 EZ_ThawDisplay MY_ANSIARGS((void));
/*********************************************************************
 * 
 *  Local Functions.
 */
static  void          EZ_RenderWidget MY_ANSIARGS((EZ_Widget *widget));
static  unsigned long EZ_GetBackgroundPixel  MY_ANSIARGS((EZ_Widget *widget));
static  void          EmbedingTimerCallback  MY_ANSIARGS((EZ_Timer *timer, void *data));
static  EZ_Widget     *EZ_GetGeometryDirtyWidget MY_ANSIARGS((EZ_Widget *widget));
static void  EZ_MakeWWin  MY_ANSIARGS((EZ_Widget *widget, int chi, int sib));

/*********************************************************************/
int EZ_DisplayIsFreezed = 0;
/*********************************************************************/

void  EZ_DisplayWidget(widget)
     EZ_Widget *widget;
{ 
  if(widget && EZ_LookupWidgetFromAllHT(widget) == widget)
    {
      EZ_DisplayWidgetWithChoice(widget, 1, 1);
      XSync(EZ_Display, False);
    }
}

/************************************************************
 *
 *  Redisplay widget. Don't need to redisply widgets on the
 *  sibling branch.
 */
void  EZ_ReDisplayWidget(widget)
     EZ_Widget *widget;
{
  /*----------------------------------------
   * Redraw widget and all its children. 
   *---------------------------------------*/
  if(EZ_LookupWidgetFromAllHT(widget) == widget)
    EZ_DisplayWidgetWithChoice(widget, 1, 0);
  XSync(EZ_Display, False);
}
/************************************************************
 *
 *  Display widget. 
 *  If children != 0, recursively display all widgets on 
 *     the children brach. 
 *  If sibling != 0, recursively display all widgets on
 *     the sibling branch.
 */

void  EZ_DisplayWidgetWithChoice(widget, children, sibling) 
    EZ_Widget *widget;
     int       children, sibling;
{
  EZ_Widget     *parent;
  int           WType;

  if(widget == (EZ_Widget *)NULL || EZ_DisplayIsFreezed ) return;

  WType = EZ_WidgetType(widget);

  if(EZ_GetWidgetNonActiveFlag(widget) | EZ_GetWidgetFreezedFlag(widget))
    {
      if(sibling)
	{
	  EZ_Widget *tmp = EZ_WidgetSibling(widget);
	  if(tmp != (EZ_Widget *)NULL) EZ_DisplayWidgetWithChoice(tmp, 1, 1); 
	}
      return;
    }
  /* determine if we need to update the display.
   * 1. If no windows has been created for widget, process
   *    as usual, i.e., update the whole widget tree
   * 2. If windows has already been created for all
   *    ancestors and one of its ancestor is not
   *    viewable, return immediately.
   */
  if(EZ_WidgetWindow(widget) != (Window)NULL)
    { 
      EZ_Widget *pp = EZ_WidgetParent(widget);
      while(pp)
	{
	  if(EZ_WidgetMapped(pp) == 0) return;
	  pp = EZ_WidgetParent(pp);
	}
    }
  
  if((parent = EZ_WidgetParent(widget))) EZ_PropagateBgInfo2Widget(parent, widget);

  /* if widget geometry is dirty, recompute geometry of the
   * widget tree up to the topmost dirty widget and redisplay
   * the subtree
   */
  {
    EZ_Widget *root = EZ_GetGeometryDirtyWidget(widget);
    if(root)
      {
	EZ_DisplayWidgetWithChoice(root, 1,0);
	return;
      }
  }

  /*--------------------------------------------
   *
   *  No Window Labels. Can not do anything
   *  about them.
   */
  if(WType == EZ_WIDGET_NW_LABEL)  
    {
      if(sibling)
	{
	  EZ_Widget *sibling_widget  = EZ_WidgetSibling(widget);
	  EZ_DisplayWidgetWithChoice(sibling_widget, children+1, sibling+1); 
	}
      return;
    }
  else  if(WType == EZ_WIDGET_PANE_HANDLE)
    {
      int or = EZ_PaneHandleOrientation(widget);
      EZ_Widget *par = EZ_WidgetParent(widget);
      int bw = EZ_WidgetBorderWidth(par);
      if(or >= EZ_HORIZONTAL && or <= EZ_HORIZONTAL_CENTER)
	{
	  int h = EZ_WidgetHeight(par);
	  EZ_WidgetOriginY(widget) = bw;
	  EZ_WidgetHeight(widget) = h - (bw+bw);
	}
      else
	{
	  int w = EZ_WidgetWidth(par);
	  EZ_WidgetOriginX(widget) = bw;
	  EZ_WidgetWidth(widget) = w - (bw+bw);
	}
    }

  /*-----------------------------------------
   *
   * + If the actual widget window has not been
   *   created (or has been destroyed), we have
   *   to create the actual X Window.
   *
   * + Otherwise, check if the window is visible,
   *   if not, remap the X window.
   *----------------------------------------*/
  if(EZ_WidgetWindow(widget) == None)
    {
      XEvent event, saveEvents[32];
      int           ii, secnt = 0;
      extern int EZ_WidgetStartsAsIcon MY_ANSIARGS((EZ_Widget *w));
      
      EZ_CreateWindowForWidget(widget, 1);
      XMapWindow(EZ_Display, EZ_WidgetWindow(widget));
      if(EZ_WidgetStartsAsIcon(widget) == 0)
	{
	  for(; ;)                    /* wait till the window is */
	    {                         /* really mapped           */
	      XWindowEvent(EZ_Display, EZ_WidgetWindow(widget),
			   SubstructureNotifyMask, &event);
              event.xany.send_event = 0xdcba;
              if(secnt < 31)
                {
                  memcpy(&saveEvents[secnt], &event, sizeof(XEvent));
                  secnt++;
                }
	      if(event.type == MapNotify) break;
      	    }
	  EZ_WidgetMaped(widget) = 1;
          /* 11-16-98 */
          if(EZ_WidgetParent(widget) == NULL)
            {
              Window junkwin, win = EZ_WidgetWindow(widget);
              int rx, ry;
              XTranslateCoordinates(EZ_Display, win, RootWindow(EZ_Display, EZ_ScreenNum),
                                    0,0, &rx, &ry, &junkwin); 	  
              EZ_WidgetOriginX(widget) = rx;
              EZ_WidgetOriginY(widget) = ry;
              if(EZ_WidgetTOSet(widget) > 1)
                {
                  EZ_WidgetTOX(widget) = -rx;
                  EZ_WidgetTOY(widget) = -ry;
                }
            }
          for(ii = 0; ii < secnt; ii++)
            XPutBackEvent(EZ_Display, &saveEvents[ii]);
        }
      /*-------------------------------------------------------------
       * For a toplevel window, record its location
       *------------------------------------------------------------*/
      if(EZ_WidgetParent(widget) == (EZ_Widget *) NULL &&
	 EZ_WidgetType(widget) != EZ_WIDGET_POPUP_MENU)
	{
	  if(EZ_GetWidgetEmbededFlag(widget) == 0 && 
	     EZ_GetWidgetXYSetFlag(widget)  != EZ_WIDGET_XY_SET_BITS)
	    {
	      Window        junkroot, junkchild;
	      int           rx,ry,x,y;
	      unsigned int  mask;

	      XQueryPointer(EZ_Display, RootWindow(EZ_Display, EZ_ScreenNum),
			    &junkroot,   
			    &junkchild,  
			    &rx, &ry,    
			    &x,&y,       
			    &mask
			    ); 
	      if(EZ_GetWidgetXSetFlag(widget) == 0)  EZ_WidgetOriginX(widget) = rx+2;
	      if(EZ_GetWidgetYSetFlag(widget) == 0)  EZ_WidgetOriginY(widget) = ry+16;
	    }
	}
      
      /* toplevel, not a transient and has not been embeded */
      if(EZ_WidgetParent(widget) == NULL &&
	 EZ_GetWidgetEmbededFlag(widget) == 0 &&
	 EZ_GetWidgetTransientFlag(widget) == 0 &&
	 WType < EZ_WIDGET_POPUP_MENU)
	{
	  EZ_GenerateEmbedingRequest(widget, 
				     EZ_WidgetOriginX(widget), EZ_WidgetOriginY(widget),
				     EZ_WidgetWidth(widget), EZ_WidgetHeight(widget));
	} 
      
      /*------------------------------------------------------
       *  Create the unique GC for Canvas. Set widget to be 
       *  the current canvas.
       *----------------------------------------------------*/
      if(WType == EZ_WIDGET_3D_CANVAS)
	{
	  /*
	   * the canvas whose window is created the last is 
	   * always the current canvas.
	   */
	  EZ_Set3DCanvas(widget);  
	  EZ_MainWindow = EZ_WidgetWindow(widget);
	  /*--------------------------------------------------
	   * If graphics primitives are set for this canvas,
	   * the next line configure the canvas. 
	   * 
	   * It also set widget to be the current canvas.
	   *------------------------------------------------*/
	  EZ_Configure3DCanvas(widget);
	  /*---------------------------------------------------------
	   * drawing into a canvas always goes to the window by 
	   * default. EZ_Configure3DCanvas() or EZ_GLConfig()
	   * will take care of the exact configuration.
	   *---------------------------------------------------------*/
	  if(EZ_Drawable == (Drawable)NULL) EZ_Drawable = EZ_MainWindow;  
	}
      /*--------------------------------------------
       *  Draw the widget.
       *-------------------------------------------*/
      /*zzzEZ_RenderWidget(widget);*/
    }
  else /* window has been created already */
    {
      /*--------------------------------------------
       * the widget window has already been created.
       *--------------------------------------------*/
      if(EZ_WidgetMaped(widget) == 0)
	{
	  /*
	   * Internal windows and popup menus are not managed by the WM. 
	   * Have to do this b/c the size or position may be changed.
	   */
	  if(EZ_WidgetParent(widget) != (EZ_Widget *)NULL ||
             EZ_GetWidgetToplevelFlag(widget) || 
	     EZ_GetWidgetEmbededFlag(widget) != 0 || WType > EZ_WIDGET_POPUP_MENU ||
	     (WType == EZ_WIDGET_POPUP_MENU && EZ_PopupIsTearOff(widget) == 0))
	    {
	      XMoveResizeWindow(EZ_Display,
				EZ_WidgetWindow(widget),
				EZ_WidgetOriginX(widget),
				EZ_WidgetOriginY(widget),
				(unsigned int)EZ_WidgetWidth(widget),
				(unsigned int)EZ_WidgetHeight(widget));
	    }
	  XMapRaised(EZ_Display, EZ_WidgetWindow(widget));

	  if(EZ_WidgetStartsAsIcon(widget) == 0)
	    {
              XEvent event, saveEvents[32];
              int           ii, secnt = 0;
	      for(; ;)                    /* wait till the window is */
		{                         /* really mapped           */
		  XWindowEvent(EZ_Display, EZ_WidgetWindow(widget),
			       SubstructureNotifyMask, &event);
                  event.xany.send_event = 0xdcba;
                  if(secnt < 31)
                    {
                      memcpy(&saveEvents[secnt], &event, sizeof(XEvent));
                      secnt++;
                    }
		  if(event.type == MapNotify)  break;
		}
              EZ_WidgetMaped(widget) = 1;
              for(ii = 0; ii < secnt; ii++)
                XPutBackEvent(EZ_Display, &saveEvents[ii]);
	    }

	  /* for embeded window, send a message to wakeup its embeder */
	  if(EZ_GetWidgetEmbededFlag(widget))
	    {
	      Window commWin = EZ_WidgetDnDApplName(widget);
	      Window pWin = EZ_WidgetDnDParent(widget);
	      if(EZ_VerifyExistence(commWin) && EZ_WindowExist(pWin))
		EZ_SendEmbedingMessage(EZ_EMBEDING_REMAPPED_S,
				       EZ_DummyWindow, EZ_WidgetWindow(widget), (unsigned long)widget,
				       commWin, pWin, 0, 
				       EZ_ApplicationSetupTime,
				       0,0,0,0,0,0);
	    }
	}
      else if(EZ_WidgetMaped(widget) == 2)
	{
	  /* 
	   * must be an internal reconfiguration. The size of an internal
	   * window has been changed, it affects possiblly the size and
	   * position of all windows.
	   */
	  if(EZ_WidgetParent(widget) != (EZ_Widget *)NULL ||
	     EZ_GetWidgetEmbededFlag(widget) != 0 || WType > EZ_WIDGET_POPUP_MENU ||
	     (WType == EZ_WIDGET_POPUP_MENU && EZ_PopupIsTearOff(widget) == 0))
	    {
	      XMoveResizeWindow(EZ_Display,
				EZ_WidgetWindow(widget),
				EZ_WidgetOriginX(widget),
				EZ_WidgetOriginY(widget),
				EZ_WidgetWidth(widget),
				EZ_WidgetHeight(widget)); 
	    }
	  else
	    {
	      /*
	       * For toplevel windows, just resize it.
	       */
              XResizeWindow(EZ_Display, EZ_WidgetWindow(widget), 
                            EZ_WidgetWidth(widget), EZ_WidgetHeight(widget));
            }
	  if(WType >= EZ_WIDGET_POPUP_MENU && EZ_WidgetParent(widget) == NULL)
	    XRaiseWindow(EZ_Display, EZ_WidgetWindow(widget));
	  EZ_WidgetMaped(widget) = 1;
	}
      else if(EZ_WidgetMaped(widget) == 3)
	{
	  /*
	   * widget is inserted to here. Have to do MoveResize or
	   * widget need to be displayed under the pointer.
	   */
	  XMoveResizeWindow(EZ_Display,
			    EZ_WidgetWindow(widget),
			    EZ_WidgetOriginX(widget),
			    EZ_WidgetOriginY(widget),
			    EZ_WidgetWidth(widget),
			    EZ_WidgetHeight(widget));
	  { /* check map status */
	    XWindowAttributes window_attributes;
	    int status = XGetWindowAttributes(EZ_Display, EZ_WidgetWindow(widget),
					      &window_attributes);
	    if(status == 0 || window_attributes.map_state == IsUnmapped)
	      {
		XMapRaised(EZ_Display,EZ_WidgetWindow(widget));	  
		if(EZ_WidgetStartsAsIcon(widget) == 0)
		  {
                    XEvent event, saveEvents[32];
                    int           ii, secnt = 0;
		    for(; ;)                    /* wait till the window is */
		      {                         /* really mapped           */
			XWindowEvent(EZ_Display, EZ_WidgetWindow(widget),
				     SubstructureNotifyMask, &event);
                        event.xany.send_event = 0xdcba;
                        if(secnt < 31)
                          {
                            memcpy(&saveEvents[secnt], &event, sizeof(XEvent));
                            secnt++;
                          }
			if(event.type == MapNotify)  break;
		      }
		    EZ_WidgetMaped(widget) = 1;
                    for(ii = 0; ii < secnt; ii++)
                      XPutBackEvent(EZ_Display, &saveEvents[ii]);
		  }
	      }
	    else  
	      {
		XRaiseWindow(EZ_Display, EZ_WidgetWindow(widget));
		EZ_WidgetMaped(widget) = 1;
	      }
	  }


	  /* for embeded window, send a message to wakeup its embeder */
	  if(EZ_GetWidgetEmbededFlag(widget))
	    {
	      Window commWin = EZ_WidgetDnDApplName(widget);
	      Window pWin = EZ_WidgetDnDParent(widget);
	      if(EZ_VerifyExistence(commWin) && EZ_WindowExist(pWin))
		EZ_SendEmbedingMessage(EZ_EMBEDING_REMAPPED_S,
				       EZ_DummyWindow, EZ_WidgetWindow(widget), (unsigned long)widget,
				       commWin, pWin, 0, 
				       EZ_ApplicationSetupTime,
				       0,0,0,0,0,0);
	    }
	}
      else if(EZ_WidgetMaped(widget) == 4)
	{
	  /* 
	   * answer to message: 
	   *   EZ_EMBEDING_MOVE_WINDOW
	   *   EZ_EMBEDING_MOVE_REIZE_WINDOW
	   *   EZ_EMBEDING_REIZE_WINDOW
	   */
	  XMoveResizeWindow(EZ_Display,
			    EZ_WidgetWindow(widget),
			    EZ_WidgetOriginX(widget),
			    EZ_WidgetOriginY(widget),
			    EZ_WidgetWidth(widget),
			    EZ_WidgetHeight(widget)); 
	  EZ_WidgetMaped(widget) = 1;
	}
      else  if(EZ_WidgetMaped(widget) == 5)
	{
	  EZ_WidgetMaped(widget) = 1;
	}
      else 
	{
	  /*----------------------------------------------------
	   * if a toplevel window is iconified, a call to
	   * XMapWindow will de-iconify it.
	   *---------------------------------------------------*/
	  if(EZ_WidgetParent(widget) == (EZ_Widget *)NULL )
	    {
	      XWindowAttributes window_attributes;
	      int status = XGetWindowAttributes(EZ_Display, EZ_WidgetWindow(widget),
						&window_attributes);
	      if(status == 0 || window_attributes.map_state == IsUnmapped)
		{
                  XEvent event, saveEvents[32];
                  int           ii, secnt = 0;
		  XMapRaised(EZ_Display,EZ_WidgetWindow(widget));	  
		  for(; ;)                    /* wait till the window is */
		    {                         /* really mapped           */
		      XWindowEvent(EZ_Display, EZ_WidgetWindow(widget),
				   SubstructureNotifyMask, &event);
                      event.xany.send_event = 0xdcba;
                      if(secnt < 31)
                        {
                          memcpy(&saveEvents[secnt], &event, sizeof(XEvent));
                          secnt++;
                        }
		      if(event.type == MapNotify)  break;
		    }
                  EZ_WidgetMapped(widget) = 1;
                  for(ii = 0; ii < secnt; ii++)
                    XPutBackEvent(EZ_Display, &saveEvents[ii]);                  
		}
	    }
        }
      /*zzz EZ_RenderWidget(widget);*/
    }
  /*-------------------------------------------------
   * Display children widgets and/or sibling widgets.
   *-------------------------------------------------*/

  if(children)
    {
      EZ_Widget  *children_widget = EZ_WidgetChildren(widget);
      EZ_DisplayWidgetWithChoice(children_widget, children+1, sibling+1);
    }
  if(sibling)
    {
      EZ_Widget *sibling_widget  = EZ_WidgetSibling(widget);
      EZ_DisplayWidgetWithChoice(sibling_widget, children+1, sibling+1); 
    }
  EZ_RenderWidget(widget);
}
/************************************************************
 *
 *  We are guaranteed that widget exists and has been mapped !
 */
static void EZ_RenderWidget(widget)
     EZ_Widget *widget;
{
  if( EZ_GetWidgetFreezedFlag(widget) == 0)
    {
      int  i = EZ_WidgetType(widget);
      (EZ_WidgetHandlingFunctions[i]->DrawWidget)(widget);
      /* 4//98 */
      EZ_ForgetGeometry(widget);
    }
}

/************************************************************
 *
 *  This is the routine which is responsible for repaint 
 *  widget window. Such as in response to an Expose event.
 *    
 *  Due to the Asyn nature of events, by the time this routine
 *  is called, the widget may has been destroyed. So we perform
 *  some checks. 
 */
void EZ_DrawWidget(widget)
     EZ_Widget *widget;
{
  EZ_Widget *tmp = EZ_LookupWidgetFromAllHT(widget);

  if(tmp)
    {
      if(EZ_WidgetMapped(widget) && EZ_GetWidgetFreezedFlag(widget) == 0)
	{
	  int i = EZ_WidgetType(widget);
	  (EZ_WidgetHandlingFunctions[i]->DrawWidget)(widget);
	}
    }
}

void EZ_DrawWidgetAndDescendants(widget, level)
     EZ_Widget *widget; int level;
{
  if(widget && EZ_WidgetMapped(widget) &&  EZ_GetWidgetFreezedFlag(widget) == 0)
    {
      EZ_Widget *children = EZ_WidgetChildren(widget);
      int i = EZ_WidgetType(widget);
      (EZ_WidgetHandlingFunctions[i]->DrawWidget)(widget);      
      EZ_DrawWidgetAndDescendants(children, 1);
      if(level)
        {
          EZ_Widget *sibling = EZ_WidgetSibling(widget);
          EZ_DrawWidgetAndDescendants(sibling, 1);          
        }
    }
}
/************************************************************
 *
 *  Create an X window for widget. Do not map the window.
 *  If widget has a parent, assume that the window for its
 *  parent has already been created.
 */

void  EZ_CreateWindowForWidget(widget, isApplWindow)
     EZ_Widget *widget; 
     int isApplWindow;
{
  /*-----------------------------------
   * Create the X window. If the parent
   * ptr is NULL, this is a top level
   * window.
   *----------------------------------*/
  Window                 parent_window;
  XSetWindowAttributes   setWAttr;
  unsigned long          valuemask = 0;
  int                    toplevel = 0, borderwidth = 0;
  int                    itplvl = EZ_GetWidgetToplevelFlag(widget);
  int                    istransient = EZ_GetWidgetTransientFlag(widget);
  int                    embeding = 0, positionSet = 0;
  int                    WType = EZ_WidgetType(widget);

  setWAttr.backing_store     = (EZ_UseBackingStore == 1? WhenMapped : NotUseful);
  setWAttr.bit_gravity       = CenterGravity;
  setWAttr.colormap          = EZ_Colormap;
  setWAttr.background_pixel  = EZ_GetBackgroundPixel(widget);
  setWAttr.override_redirect = False;
  setWAttr.save_under        = False;
  setWAttr.background_pixmap = None;
  setWAttr.border_pixel      = 0;

  if(itplvl) istransient = 0;
  /*----------------------------------------------------
   * if Geometry is set. Use it once
   *---------------------------------------------------*/
  if(isApplWindow && EZ_GeometrySet)
    {
      int w,h,recompute = 0;

      if( EZ_GeometrySet & 1) /* width set */
	{
	  if(EZ_GeometryW > 0)
	    {
	      recompute = 1;
	      EZ_WidgetWidth(widget) = EZ_WidgetWidthHint(widget) = EZ_GeometryW;
	      { EZ_SetWidgetWidthSetFlag(widget);}
	    }
	}
      if( EZ_GeometrySet & 2) /* height set */
	{
	  if(EZ_GeometryH > 0)
	    {
	      recompute = 1;
	      EZ_WidgetHeight(widget) = EZ_WidgetHeightHint(widget) = EZ_GeometryH;
	      { EZ_SetWidgetHeightSetFlag(widget);}
	    }
	}
      if(recompute) EZ_ComputeWidgetWindowSize(widget, &w, &h,NULL);

      if((EZ_GeometrySet & 12) == 12) /* position is set */
	{
	  positionSet = 1;
	  EZ_SetWidgetXYSetFlag(widget);

	  if(EZ_GeometryX < 0)
	    {
	      int dw = DisplayWidth(EZ_Display, EZ_ScreenNum);
	      EZ_WidgetOriginX(widget) = dw -EZ_WidgetWidth(widget) + EZ_GeometryX;
	    }
	  else EZ_WidgetOriginX(widget) = EZ_GeometryX;

	  if(EZ_GeometryY < 0)
	    {
	      int dh = DisplayHeight(EZ_Display, EZ_ScreenNum);
	      EZ_WidgetOriginY(widget) = dh - EZ_WidgetHeight(widget) + EZ_GeometryY;
	    }
	  else EZ_WidgetOriginY(widget) = EZ_GeometryY;
	}
      EZ_GeometrySet = 0;  
    }

  /*----------------------------------------------------
   * a potential toplevel window 
   *---------------------------------------------------*/
  if(EZ_WidgetParent(widget) == (EZ_Widget *)NULL)
    {
      if(istransient == 0)
	{
	  Window win1, win2;
	  /* check if this widget need to be run embded.
	   * We'll check only the first window (assume its the main window)
	   */
	  win1 = EZ_WidgetDnDApplName(widget);
	  win2 = EZ_WidgetDnDParent(widget);
	  if(win1 && win2 && EZ_VerifyExistence(win1) != 0 && EZ_WindowExist(win2))
	    {
	      parent_window = win2;
	      embeding = 1;
	      EZ_SetWidgetEmbededFlag(widget);
	    }
	  else if(EZ_DnDCommuWin != (Window)NULL && EZ_DnDParentWin != (Window)NULL &&
		  EZ_VerifyExistence(EZ_DnDCommuWin) != 0 && EZ_WindowExist(EZ_DnDParentWin))
	    {
	      parent_window = EZ_DnDParentWin;
	      embeding = 2;
	      EZ_WidgetDnDApplName(widget) = EZ_DnDCommuWin;
	      EZ_DnDCommuWin = (Window)NULL;
	      EZ_WidgetDnDParent(widget) = EZ_DnDParentWin;
	      EZ_DnDParentWin = (Window)NULL;
	      EZ_SetWidgetEmbededFlag(widget);
	    }
	  else 	parent_window = RootWindow(EZ_Display, EZ_ScreenNum);
	}
      else parent_window = RootWindow(EZ_Display, EZ_ScreenNum);
	  
      if(WType < EZ_WIDGET_POPUP_MENU 
		 || (WType == EZ_WIDGET_POPUP_MENU && EZ_PopupIsTearOff(widget)))
	toplevel = (itplvl == 0); 
    }
  else  parent_window = EZ_WidgetWindow(EZ_WidgetParent(widget));

  /*---------------------------------------------------------------
   * For internal windows and popup menus, hide them from the 
   * window manager.
   *--------------------------------------------------------------*/
  if(EZ_WidgetOverrideRedirect(widget)) setWAttr.override_redirect = True;
  else if(toplevel == 0 || embeding != 0) setWAttr.override_redirect = True;
  
  if(toplevel && istransient)
    {
      /*-------------------------------------------------------------
       * For a transient window, hint the WM a
       * resonable initial position for the windows.
       *------------------------------------------------------------*/
      if(EZ_GetWidgetXYSetFlag(widget)  != EZ_WIDGET_XY_SET_BITS)
        {
          Window        junkroot, junkchild;
          int           rx,ry,x,y;
          unsigned int  mask;

          XQueryPointer(EZ_Display, parent_window,
                        &junkroot,   
                        &junkchild,  
                        &rx, &ry,    
                        &x,&y,       
                        &mask
                        ); 
          if(EZ_GetWidgetXSetFlag(widget) == 0)  rx = rx - (EZ_WidgetWidth(widget)>>1);
          if(EZ_GetWidgetYSetFlag(widget) == 0)  ry = ry - (EZ_WidgetHeight(widget)>>1);

          if(rx < 0) rx = 0; 
          else if(rx + EZ_WidgetWidth(widget) > EZ_XDisplayWidth) 
            rx = (int)EZ_XDisplayWidth - EZ_WidgetWidth(widget);
          if(ry < 0) ry = 0;       
          else if(ry + EZ_WidgetHeight(widget) > EZ_XDisplayHeight)
            ry =  EZ_XDisplayHeight - EZ_WidgetHeight(widget);

          if(EZ_GetWidgetXSetFlag(widget) == 0) EZ_WidgetOriginX(widget) = rx;
          if(EZ_GetWidgetYSetFlag(widget) == 0) EZ_WidgetOriginY(widget) = ry;
        }
    }
  /*---------------------------------------------------------------
   * For transient widgets and popup menus, set save-under.
   *--------------------------------------------------------------*/
  if(istransient != 0 || WType >= EZ_WIDGET_POPUP_MENU )
    setWAttr.save_under = True;
  
  if((WType == EZ_WIDGET_3D_CANVAS && EZ_3DCanvasBackingStore(widget) != 0) ||
     WType == EZ_WIDGET_LED )
    setWAttr.backing_store = Always; 

  valuemask = CWBackingStore|CWBackPixel|CWColormap|CWSaveUnder|CWOverrideRedirect|
    CWBorderPixel|CWBackPixmap;

  EZ_WidgetWindow(widget) = 
    XCreateWindow(EZ_Display,
		  parent_window,
		  EZ_WidgetOriginX(widget), EZ_WidgetOriginY(widget),
		  EZ_WidgetWidth(widget),  
		  EZ_WidgetHeight(widget),
		  borderwidth,
		  EZ_Depth,
		  InputOutput,
		  EZ_Visual,
		  valuemask,
		  &setWAttr);

  XSelectInput(EZ_Display, EZ_WidgetWindow(widget), ALL_EVENT_MASK);
  XDefineCursor(EZ_Display, EZ_WidgetWindow(widget), EZ_WidgetCursor(widget));
  
  if(toplevel != 0) 
    {
      extern char EZ_TopWinState;
      int tmp = 0;
      if(EZ_TopWinState == IconicState && isApplWindow && 
	 !(istransient | embeding) && WType < EZ_WIDGET_POPUP_MENU)
	{
	  tmp = 256; /* just bitmask to set IconicState in EZ_SetWMHints... */
	  EZ_TopWinState = 1; /* NormalState*/
	}
      EZ_SetWMHintsAndSizeHints(widget, (embeding | positionSet | tmp) );
      {
	XClassHint *classhint = XAllocClassHint();
	classhint->res_class = EZ_GetClassName();
	classhint->res_name  = EZ_GetInstanceName();
	XSetClassHint(EZ_Display, EZ_WidgetWindow(widget), classhint);
	XFree((char *)classhint);
      }
      /*-----------------------------------------------------------
       * For transient windows, set the XA_TRANSIENT_FOR property.
       *----------------------------------------------------------*/
      if(istransient != 0)  /* transient widget */
	XSetTransientForHint(EZ_Display, EZ_WidgetWindow(widget),  parent_window); 
      (void) XSetWMProtocols(EZ_Display, EZ_WidgetWindow(widget),
			     &(EZ_WM_DELETE_WINDOW_ATOM), 1);
    }
  /*********************************************************
   * check if its a shaped window 
   ********************************************************/
  EZ_SetWindowShape(widget, 0);

  /*--------------------------------------------
   * Insert this widget into the hash table.
   *-------------------------------------------*/
  EZ_InsertWidgetToMappedHT(widget);

  /* if this widget is a DND src, register it */
  if(EZ_WidgetIsDnDSrc(widget))
    {
      char msg[32];
      sprintf(msg, "EZ_DND_SWIN %lx ", (unsigned long)EZ_DummyWindow);
      XChangeProperty(EZ_Display,
		      EZ_WidgetWindow(widget),
		      EZ_DND_SWIN_ATOM,
		      XA_STRING, 8,
		      PropModeReplace,
		      (unsigned char *)msg,
		      32);
    }

  /* if this widget is a DND target, register it.
   * Three widgets dealing with items are DnD targets by default */
  if(EZ_WidgetIsDnDTarget(widget) || WType == EZ_WIDGET_IWORK_AREA ||
     WType == EZ_WIDGET_ITREE || WType == EZ_WIDGET_IFANCY_LIST_BOX)
    {
      char msg[40];
      sprintf(msg, "EZ_DND_TWIN %lx ", (unsigned long)EZ_DummyWindow);
      XChangeProperty(EZ_Display,
		      EZ_WidgetWindow(widget),
		      EZ_DND_TWIN_ATOM,
		      XA_STRING, 8,
		      PropModeReplace,
		      (unsigned char *)msg,
		      32);
      EZ_Insert2DnDWindowList(widget); 
    }


  /* if executed from an executor, send a confirmation */
  if(embeding == 2)
    {
      Window commWin = EZ_WidgetDnDApplName(widget);
      Window pWin = EZ_WidgetDnDParent(widget);
      unsigned long   pid  = EZ_VerifyExistence(commWin);
      if(pid && EZ_WindowExist(pWin))
	{
	  EZ_SendEmbedingMessage(EZ_EMBEDING_CONFIRM_S,
				 EZ_DummyWindow, EZ_WidgetWindow(widget), (unsigned long)widget,
				 commWin, pWin, 0,
				 EZ_ApplicationSetupTime,
				 EZ_WidgetOriginX(widget), EZ_WidgetOriginY(widget),
				 EZ_WidgetWidth(widget), EZ_WidgetHeight(widget),
				 EZ_WidgetAMinWidth(widget), EZ_WidgetAMinHeight(widget)); 
	  {
	    (void) EZ_CreateTimer(10,0,-1, (EZ_CallBack) EmbedingTimerCallback,
				  (void *)widget, (int)pid); 
	  }
	}
    }
}
/************************************************************/
void EZ_ForgetGeometry(widget)
     EZ_Widget *widget;
{
  int forget = EZ_GetWidgetForgetGFlag(widget);
  if(forget != 0)
    {
      if(forget & EZ_WIDGET_FORGET_X_BIT)
	{ EZ_ClearWidgetXSetFlag(widget);}
      if(forget & EZ_WIDGET_FORGET_Y_BIT)
	{ EZ_ClearWidgetYSetFlag(widget);}
      if(forget & EZ_WIDGET_FORGET_W_BIT)
	{ EZ_ClearWidgetWidthSetFlag(widget);
	EZ_ClearWidgetWidthHintFlag(widget); }
      if(forget & EZ_WIDGET_FORGET_H_BIT)
	{ EZ_ClearWidgetHeightSetFlag(widget);
	EZ_ClearWidgetHeightHintFlag(widget); }
      EZ_ClearWidgetForgetWHFlag(widget);
      EZ_ClearWidgetForgetXYFlag(widget);
    }
}
/************************************************************/

static unsigned long EZ_GetBackgroundPixel(wptr)
     EZ_Widget *wptr;
{
  if(EZ_WidgetBackground(wptr) != 0)
    return(EZ_WidgetBackground(wptr));
  else if(EZ_WidgetParentBG(wptr) && *(EZ_WidgetParentBG(wptr)))
    return(*(EZ_WidgetParentBG(wptr)));
  else return(EZ_ColorArray[EZ_NORMAL_GRAY1]);
}
/************************************************************/

static void EmbedingTimerCallback(timer, data)
     EZ_Timer *timer;
     void *data;
{
  int      idata;
  EZ_Widget *widget;

  EZ_GetTimerClientData(timer,(void **)&widget, &idata);
  
  if(EZ_LookupWidgetFromAllHT(widget))
    {
      Window commWin = EZ_WidgetDnDApplName(widget);
      Window pWin = EZ_WidgetDnDParent(widget);
      int pid = (int)EZ_VerifyExistence(commWin);
      
      if(pid != idata || EZ_WindowExist(pWin) == 0)
	{
	  XSync(EZ_Display, False);
	  EZ_Shutdown();
	  exit(EZ_EXIT_OK);
	}
    }
  /* ptr does'nt exist or is not an embeder, release timer */
  else  EZ_CancelTimer(timer); 
}
/*******************************************************************/
void EZ_SetWindowShape(widget, remove_flag)
     EZ_Widget *widget; int remove_flag; /* remove shape */
{
  if(EZ_XServerHasSHAPEExt)
    {
      int wtype = EZ_WidgetType(widget);
      if(wtype != EZ_WIDGET_ITOPLEVEL || EZ_LabelIsLabeledIcon(widget) == 0)
	{
	  EZ_Bitmap *bitmap = EZ_WidgetShapeMask(widget);
          EZ_Bitmap *shapeSet = bitmap;
          if(bitmap == NULL && wtype != EZ_WIDGET_FRAME)  /* exclude labeled frame */
            bitmap = EZ_WidgetPixmap(widget);
	  
	  if(remove_flag)
            {
              if(EZ_GetWidgetShapedSetFlag(widget))
                {
                  XShapeCombineMask(EZ_Display,  EZ_WidgetWindow(widget),
                                    ShapeBounding, 0, 0,
                                    None, ShapeSet);
                  EZ_ClearWidgetShapedSetFlag(widget);
                }
            }
	  else if( bitmap && bitmap->shape && EZ_GetWidgetShapedWinFlag(widget))
	    {
              if(shapeSet)
                {
                  XShapeCombineMask(EZ_Display,  EZ_WidgetWindow(widget),
                                    ShapeBounding, 0, 0,
                                    bitmap->shape, ShapeSet);
                }
              else
                {
                  int fillx, filly;
                  int x,y,xx,yy;
                  EZ_GetWidgetLabelPosition(widget, &fillx, &filly);
                  xx = fillx + EZ_WidgetXOffset(widget);
                  yy = filly + EZ_WidgetYOffset(widget);  
                  x = xx + EZ_WidgetPadX(widget) + EZ_WidgetBorderWidth(widget);
                  y = yy + EZ_WidgetBorderWidth(widget) + EZ_WidgetPadY(widget);
	      
                  XShapeCombineMask(EZ_Display,  EZ_WidgetWindow(widget),
                                    ShapeBounding, x, y,
                                    bitmap->shape, ShapeSet);
                  if(EZ_OffsetEnabled && EZ_GetWidgetShapeShiftFlag(widget))
                    {
                      XShapeCombineMask(EZ_Display,  EZ_WidgetWindow(widget),
                                        ShapeBounding, x+1, y+1,
                                        bitmap->shape, ShapeUnion);
                    }
                }
              EZ_SetWidgetShapedSetFlag(widget);
	    }
	}
    }
}
/*******************************************************************/
static EZ_Widget *GetDirtyInferior(widget) EZ_Widget *widget;
/* this function returns the first dirty inferior widget in the widget tree */
{
  if(widget)
    {
      if( EZ_GetWidgetSizeComputedFlag(widget) == 0  &&
	  EZ_GetDoNotPropagateFlag(widget) == 0 ) return(widget);
      {
	EZ_Widget *children = EZ_WidgetChildren(widget);
	EZ_Widget *sibling = EZ_WidgetSibling(widget);
	EZ_Widget *tmp = GetDirtyInferior(children);
	if(tmp) return(tmp);
	return(GetDirtyInferior(sibling));
      }
    }
  return(NULL);
}
/*******************************************************************/

void EZ_DisplayWidgetUnderPointer(widget, x, y)
     EZ_Widget *widget; int x, y;
{
  if(widget && EZ_LookupWidgetFromAllHT(widget) == widget)  
    {
      EZ_Widget *toplevel = widget;
      EZ_Widget *dirty = widget;

      /* figure out if the geometry of the widget tree need to
       * be recomputed. This function behaves differently from
       * EZ_DisplayWidget. It makes that the whole widget
       * tree viewable.
       */
      while(EZ_WidgetParent(dirty)) dirty = EZ_WidgetParent(dirty);
      dirty = GetDirtyInferior(dirty);
      if(dirty) (void)EZ_GetGeometryDirtyWidget(dirty);
      
      if(EZ_WidgetWindow(widget) == (Window)None)
	EZ_MakeWidgetWindowExist(widget);
      
      if(1)
	{
	  Window junk; unsigned int mask;
	  int  rx, ry, tmp;

	  if(XQueryPointer(EZ_Display, RootWindow(EZ_Display, EZ_ScreenNum),
			   &junk, &junk, &rx, &ry, &tmp, &tmp, &mask))
	    {
	      int  xoff = 0, yoff = 0;
	      int  width = EZ_WidgetWidth(widget);
	      int  height = EZ_WidgetHeight(widget);
	      while(toplevel)
		{
		  if(EZ_WidgetParent(toplevel) == NULL) break;
		  xoff += EZ_WidgetOriginX(toplevel);
		  yoff += EZ_WidgetOriginY(toplevel);
		  toplevel = EZ_WidgetParent(toplevel);
		}
	      if(x == 0 && y == 0)
		{
		  x = width >> 1;
		  y = height >> 1;
		}
	      else
		{
		  if(x < 0) x = width  +x;
		  if(y < 0) y = height +y;
		}
	      {
		/* make widget completely visible */
		int ox = rx - xoff - x;
		int oy = ry - yoff - y;
		if(ox < 0) ox = 0;
		else if(ox + width > EZ_XDisplayWidth)  ox = (int)EZ_XDisplayWidth - width;
		if(oy < 0) oy = 0;
		else if(oy + height > EZ_XDisplayHeight) oy = (int)EZ_XDisplayHeight - height;

		EZ_WidgetOriginX(toplevel) = ox;
		EZ_WidgetOriginY(toplevel) = oy;
		EZ_SetWMSizeHints(toplevel, 1);
		/* alter MappedFlag to force a XMoveResizeWindow call */
                EZ_WidgetMaped(toplevel) = 3; 
	      }
	    }
	}
      EZ_DisplayWidget(toplevel);
      XSync(EZ_Display,False);
    }
}
/*******************************************************************/
static EZ_Widget *EZ_GetGeometryDirtyWidget(widget)
     EZ_Widget *widget;
{
  if( !(EZ_GetWidgetSizeComputedFlag(widget)) )
    { 
      int         w,h;
      int         do_not_propagate, tmp;
      EZ_Widget  *root=NULL, *parent, *EZ_TopmostGeomDirtyWidget;

      if(EZ_GetWidgetDetachedFlag(widget))
	{
	  EZ_MarkAllChildrenWidgetSizeChanged(widget,0);
	  EZ_ComputeWidgetWindowSize(widget, &w, &h, NULL);
	  return(NULL);
	}
      /*---------------------------------------
       * The dimension of widget must have been
       * changed.  It will possibly affects all
       * ancestor widgets. Trace to the topmost
       * ancestor (root) and 
       *  + recompute the geometry of root;
       *  + recompute the position of all descends of root.
       *
       * The recursive nature of EZ_ComputeWidgetWindowSize 
       * actually does not pose any serious problems. The
       * geometry of each widget is computed only once.
       *--------------------------------------*/
      parent = widget;
      if(EZ_WidgetWindow(widget)==(Window)NULL)	do_not_propagate = -1;
      else do_not_propagate = EZ_GetDoNotPropagateFlag(widget);

      EZ_TopmostGeomDirtyWidget = NULL;
      while( parent != (EZ_Widget *)NULL)
	{
	  root = parent;
	  EZ_ClearWidgetSizeComputedFlag(root);  
	  if(do_not_propagate >= 0)
	    {
	      if( (tmp = EZ_GetDoNotPropagateFlag(root)) != 0)
		{
		  do_not_propagate = 1;
		  break;
		}
	    }
	  parent = EZ_WidgetParent(root);
	}
      /*-------------------------------------
       * recompute thr geometry of root
       *------------------------------------*/
      if(do_not_propagate > 0) EZ_TopmostGeomDirtyWidget = root; 
      EZ_MarkAllChildrenWidgetSizeChanged(root,0);
      EZ_ComputeWidgetWindowSize(root, &w, &h, EZ_TopmostGeomDirtyWidget);
      /*-----------------------------------------
       * finally, redisplay root.
       *-----------------------------------------*/
      return(root);
    }
  return(NULL);
}
/***************************************************************************/
void EZ_MakeWidgetWindows(widget) EZ_Widget *widget;
{ EZ_MakeWWin(widget, 1, 0); }
/***************************************************************************/
static void  EZ_MakeWWin(widget, children, sibling) 
    EZ_Widget *widget;
    int       children, sibling;
{
  EZ_Widget     *root;
  int            WType;

  if(widget == (EZ_Widget *) NULL) return;
  if(EZ_WidgetWindow(widget) == (Window)None)
    {
      WType = EZ_WidgetType(widget);
      /* if widget geometry is dirty, recompute geometry of the
       * widget tree up to the topmost dirty widget and redisplay
       * the subtree
       */
      root = EZ_GetGeometryDirtyWidget(widget);
      if(root) { EZ_MakeWWin(root, 1,0);   return; }

      if(WType == EZ_WIDGET_PANE_HANDLE)
	{
	  int or = EZ_PaneHandleOrientation(widget);
	  EZ_Widget *par = EZ_WidgetParent(widget);
	  int bw = EZ_WidgetBorderWidth(par);
	  if(or >= EZ_HORIZONTAL && or <= EZ_HORIZONTAL_CENTER)
	    {
	      int h = EZ_WidgetHeight(par);
	      EZ_WidgetOriginY(widget) = bw;
	      EZ_WidgetHeight(widget) = h - (bw+bw);
	    }
	  else
	    {
	      int w = EZ_WidgetWidth(par);
	      EZ_WidgetOriginX(widget) = bw;
	      EZ_WidgetWidth(widget) = w - (bw+bw);
	    }
	}
      EZ_CreateWindowForWidget(widget, 1);
    }
  if(children)
    {
      EZ_Widget  *children_widget = EZ_WidgetChildren(widget);
      EZ_MakeWWin(children_widget, children+1, sibling+1);
    }
  if(sibling)
    {
      EZ_Widget *sibling_widget  = EZ_WidgetSibling(widget);
      EZ_MakeWWin(sibling_widget, children+1, sibling+1); 
    }
}
/***************************************************************************/
void EZ_FreezeDisplay()
{
  EZ_DisplayIsFreezed++;
}
void EZ_ThawDisplay()
{
  EZ_DisplayIsFreezed--;
  if(EZ_DisplayIsFreezed < 0) EZ_DisplayIsFreezed = 0;
}
/***************************************************************************/

#undef _EZ_WIDGET_DISP_C_
