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
 *  EZ Widget library. 
 *              Event handling.
 */
#define _EZ_WIDGET_EVENT_C_

#include "EZ_Widget.h"

/*******************************************************************
 *
 * Functions implemented in this file:
 */
void            EZ_WidgetDispatchEvent MY_ANSIARGS((XEvent *event));
void            EZ_HandleWidgetWindowEvent MY_ANSIARGS((EZ_Widget *widget, XEvent *event));
void            EZ_ServiceEvents MY_ANSIARGS((void));
void            EZ_WaitAndServiceNextEvent MY_ANSIARGS((void));
void            EZ_EventMainLoop MY_ANSIARGS((void));
void            EZ_InvokePrivateEventHandler MY_ANSIARGS((XEvent *event));
void            EZ_InitializeEventHT MY_ANSIARGS((void));
void            EZ_InsertGlobalKeyPressEvent  MY_ANSIARGS((EZ_Widget *widget,int modif, char *key));
void            EZ_RemoveGlobalKeyEvent  MY_ANSIARGS((int modi, char *key, EZ_Widget *widget));
void            EZ_RemoveAllKeyShortcutForWidget  MY_ANSIARGS((EZ_Widget *widget));
int             EZ_GetGlobalKeyEvent  MY_ANSIARGS((int mod, char *key, EZ_Widget *root, 
						   EZ_Widget **widget_ret, int *x_r, int *y_r));
void            EZ_TiePopupMenu  MY_ANSIARGS((EZ_Widget *widget, int btnNm, int where));
void            EZ_WarpPointerToWidgetWindow MY_ANSIARGS((EZ_Widget *wgt, int evt));

/*******************************************************************/
void            EZ_RemoveEvent  MY_ANSIARGS((XEvent *event));
int             EZ_CheckTimerEvents MY_ANSIARGS((void));
void            EZ_HandleGlobalKeyEvent MY_ANSIARGS((EZ_Widget *w, int x, int y));
void            EZ_TFEvents MY_ANSIARGS((void));
int             EZ_FilterEvent MY_ANSIARGS((XEvent *event));
void            EZ_WarpPointer MY_ANSIARGS((int xx, int yy));
/*******************************************************************/
Time            EZ_LastEventTime, EZ_LastResetFocusTime;
/*******************************************************************
 *
 * Local Functions:
 */
static void     EZ_HandleGlobalKeyEventCarefully  MY_ANSIARGS((EZ_Widget *widget, XEvent *event, int x, int y));
static int      EZ_HASH_CHAR MY_ANSIARGS((char c));
static void     EZ_HideBubbleHelp MY_ANSIARGS((void));
static void     EZ_HandleClientMessage MY_ANSIARGS((EZ_Widget *widget, int etype, XEvent *xevent));

/*******************************************************************
 *
 * Local variables:
 */
#define EZ_GLOBALEVENT_HASH_SIZE  128  

typedef struct _EZ_GlobalEvent_{
  EZ_Widget         *widget;         /* to which widget this event is binded to */
  int               modifiers;       /* key modifiers */
  struct  _EZ_GlobalEvent_ *next;  /* the chain     */
} EZ_GlobalEvent;

static EZ_GlobalEvent EZ_GlobalKeyEvent[EZ_GLOBALEVENT_HASH_SIZE];
static int  PopupPosition[3];  /* where to post popup menus, one for each button */

/********************************************************************
 * for bubble help
 */
static struct timeval LastBubbleTime;
static Window LastBubbleWindow = (Window)NULL;

/********************************************************************/
void EZ_GetCurrentTime(tv)
     struct timeval *tv;
{
  (void) gettimeofday(tv,NULL);
}

/*******************************************************************
 *
 *  If there are any pending X events, service them. If there is none
 *  return.
 */
void  EZ_ServiceEvents()
{
  XEvent xevent;
  if(EZ_Display == NULL) exit(1);
  while(XPending(EZ_Display) != 0) 
    {
      XNextEvent(EZ_DisplayForWidgets,&xevent);
      if(EZ_FilterEvent(&xevent))
	{
	  EZ_InvokePrivateEventHandler(&xevent);
	  EZ_WidgetDispatchEvent(&xevent);
	}
    }
  (void) EZ_CheckTimerEvents();
  (void) EZ_CheckAppInputs(0);
  EZ_ExecuteIdleCallBacks();
}
/***************************************************************
 *
 *  Wait for the next event, service it if it arrives.
 */
void  EZ_WaitAndServiceNextEvent()
{
  XEvent xevent;
  if(EZ_Display == NULL) exit(1);
  while(1)
    {
      if(XPending(EZ_Display))
	{
	  XNextEvent(EZ_Display,&xevent);
	  if(EZ_FilterEvent(&xevent))
	    {
	      EZ_InvokePrivateEventHandler(&xevent);
	      EZ_WidgetDispatchEvent(&xevent);
	    }
	  break;
	}
      else
	{
	  if(EZ_CheckTimerEvents()) break;
	  if(EZ_CheckAppInputs(1000) != 0) /* -1 or got some file events */
            {
              EZ_ExecuteIdleCallBacks();
              EZ_SitForALittleBit(1000); 
            }
	}
    }
}

/*******************************************************************
 *
 *  The main event loop. Not very useful.
 */

void EZ_EventMainLoop()
{
  XEvent event;
  if(EZ_Display == NULL) exit(1);
  while(1)
    {
      while(XPending(EZ_Display)) /* X events are of high priority */
	{
	  XNextEvent(EZ_Display,&event);
	  if(EZ_FilterEvent(&event))
	    {
	      EZ_InvokePrivateEventHandler(&event);
	      EZ_WidgetDispatchEvent(&event);
	    }
	}
      EZ_CheckTimerEvents();
      if(EZ_CheckAppInputs(1000) != 0)
        {
          EZ_ExecuteIdleCallBacks();
          EZ_SitForALittleBit(1000);
        }
    }
}

/**************************************************************************
 * Record the event time, call user registered event filter.
 */
int EZ_FilterEvent(event)
     XEvent *event;
{
  Time NewTime;

  switch (event->type) 
    {
    case KeyPress:
    case KeyRelease:
      NewTime = event->xkey.time;
      break;
    case ButtonPress:
    case ButtonRelease:
      NewTime = event->xbutton.time;
      break;
    case MotionNotify:
      NewTime = event->xmotion.time;
      break;
    case EnterNotify:
    case LeaveNotify:
      NewTime = event->xcrossing.time;
      break;
    case PropertyNotify:
      NewTime = event->xproperty.time;
      break;
    case SelectionClear:
      NewTime = event->xselectionclear.time;
      break;
    case SelectionRequest:
      NewTime = event->xselectionrequest.time;
      break;
    case SelectionNotify:
      NewTime = event->xselection.time;
      break;
    default:
      NewTime = CurrentTime;
      break;
    }
  if(NewTime > EZ_LastEventTime)   EZ_LastEventTime = NewTime;  
  if(EZ_GlobalEventFilter) EZ_GlobalEventFilter(event,EZ_GlobalEventFilterData); 
  return(1);
}

/**************************************************************************
 *
 *  Dispatch event to the approprate enent handler.
 *
 *  ConfigureNotify is a troublesome thing.  Resizing/moving a window generats it,
 *  internal configuration generates it as well; and there is no way to tell 
 *  which is which !!!!
 *
 *  Hack: Handle those ConfigureNotify which occur at least  1 second after the last
 *  one was handled. This seems to work. Usually, two user resizing cannot
 *  happen within 1 second.
 */
static long  Last_CN_HTime = 0; /* time at which the last ConfigureNotify was handled, in seconds */

void EZ_WidgetDispatchEvent(event)
     XEvent *event;
{
  EZ_Widget *widget, *widget_return;
  Window          win; 

  if(event->xany.send_event == 0xdcba) return;
  win = event->xany.window;
  if( (widget = EZ_LookupWidgetFromMappedHT(win)) == (EZ_Widget *)NULL)
    return;

  /*----------------------------------------------------------------------
   * for bubble help window does not responds to anything.
   */
  if(widget == EZ_BubbleHelp) return;
  /*-------------------------------------------------------------
   * First check if there are any active grab.
   *------------------------------------------------------------*/
  if(EZ_GrabedWidget)
    {
      /* if(EZ_WidgetIsViewable(EZ_GrabedWidget))*/
      if(EZ_LookupWidgetFromAllHT(EZ_GrabedWidget) &&
	 EZ_WidgetWindow(EZ_GrabedWidget) != None )
	{
	  int type = event->type;
	  int bnkcnt = 0;
	  switch(type)
	    {
	    case Expose:
	    case SelectionClear:
	    case ConfigureNotify:
	    case EnterNotify:
	    case LeaveNotify:
	    case FocusIn:
	    case FocusOut:
	    case MotionNotify:
	      break;
	    case ButtonPress:
	    case KeyPress:
	      bnkcnt=0;
	    default:
	      {
		EZ_Widget *tmp = widget;
		while(tmp && tmp != EZ_GrabedWidget)
		  tmp = EZ_WidgetParent(tmp);
		if(tmp != EZ_GrabedWidget)
		  {
		    EZ_BlinkWidget(EZ_GrabedWidget, bnkcnt);
		    return;
		  }
	      }
	      break;
	    }
	}
      else EZ_GrabedWidget = NULL;
    }
  

  /*-------------------------------------------------------------
   *  if this event is a KeyPress and it is binded to a global
   *  function, do it here. Otherwise, send it to the
   *  appropriate widget.  TEXT and ENTRY Widgets are special. 
   *  They handle key events themselves.
   *------------------------------------------------------------*/
  switch(event->type)
    {
    case UnmapNotify:
      if(EZ_WidgetParent(widget) == NULL)
	{
	  int idx = 0; EZ_Widget *sto[1024];
	  EZ_WidgetMapped(widget) = 0;
	  EZ_GetDetachedInferior(widget, 0, &idx, sto);
	  while(--idx >= 0) EZ_HideWidgetWork(sto[idx], 0);
	}
      break;
    case MapNotify:
      if(EZ_WidgetParent(widget) == NULL && EZ_WidgetMapped(widget) == 0)
	{
	  EZ_WidgetMapped(widget) = 1;
	  EZ_DisplayWidget(widget);
	}
      break;
    case KeyPress:
      {
	int type = EZ_WidgetType(widget);
	switch(type)
	  {
	  case EZ_WIDGET_ITEXT:
	  case EZ_WIDGET_ENTRY:
          case EZ_WIDGET_ITERM:
	  case EZ_WIDGET_SPREAD_SHEET:
	    /* case EZ_WIDGET_3D_CANVAS:*/
	    break;
	  default:
	    {
	      KeySym            keysym;
	      XComposeStatus    compose; 
	      char              tmpbuffer[8];
	      int               count;
	      int modifiers = (event->xkey.state & (ShiftMask | ControlMask | Mod1Mask));
	      event->xkey.state &= ~modifiers; 
	      count = XLookupString(&(event->xkey), tmpbuffer, 8, &keysym, &compose);
	      tmpbuffer[count] = '\0'; 
      
	      if(modifiers != 0)
		{
		  EZ_Widget   *root = widget;
		  int is_global_event = 0;

		  if(count != 0)
		    {
		      int xx, yy;
		      while(EZ_WidgetParent(root) != (EZ_Widget *)NULL) 
			root = EZ_WidgetParent(root);
		      is_global_event = EZ_GetGlobalKeyEvent(modifiers, tmpbuffer,
							     root, &widget_return, &xx, &yy);
		      if(is_global_event != 0)
			{
			  EZ_HandleGlobalKeyEvent(widget_return, xx, yy);
			  return;
			}
		    }
		}
	      /* restore xkey.state state */
	      event->xkey.state |= modifiers; 
	      
	      /* not binded to a global event */
	      switch(keysym)
		{
		case XK_Tab:  
#ifdef XK_KP_Tab
		case XK_KP_Tab:
#endif
		  {
		    int type = EZ_WidgetType(widget);
		    if(type != EZ_WIDGET_POPUP_MENU &&
		       type != EZ_WIDGET_MENU_TEAR_OFF_BAR &&
		       (type < EZ_WIDGET_MENU_SEPARATOR ||
			type > EZ_WIDGET_MENU_RADIO_BUTTON))
		      EZ_FocusNextWidget(widget, (modifiers & ShiftMask));
		  }
		break;
		case XK_Return: case XK_Linefeed: case XK_space:  
#ifdef XK_KP_Enter
		case XK_KP_Enter:
#endif
		  if(type != EZ_WIDGET_3D_CANVAS &&
		     widget == EZ_CurrentFocusingWidget &&
		     !(event->xkey.send_event))
		    {
		      EZ_HandleGlobalKeyEventCarefully(widget,event, 0, 0);
		      return;
		    }
		  break;
#ifdef KEY_MOVE_POINTER 
		case XK_Up: 
#ifdef XK_KP_Up
		case XK_KP_Up: 
#endif
		  if(modifiers && type != EZ_WIDGET_3D_CANVAS)
		    {
		      if(modifiers & ShiftMask) munit = 1;
		      else if(modifiers & ControlMask) munit = 16;
		      else munit = 4;
		      if(modifiers & Mod1Mask) munit = munit << 2;
		      EZ_WarpPointer(0,-munit);
		      return;
		    }
		  break;
		case XK_Down: 
#ifdef XK_KP_Down
		case XK_KP_Down:
#endif
		  if(modifiers && type != EZ_WIDGET_3D_CANVAS)
		    {
		      if(modifiers & ShiftMask) munit = 1;
		      else if(modifiers & ControlMask) munit = 16;
		      else munit = 4;
		      if(modifiers & Mod1Mask) munit = munit << 2;
		      EZ_WarpPointer(0,munit);
		      return;
		    }
		  break;
		case XK_Left: 
#ifdef XK_KP_Left
		case XK_KP_Left:
#endif
		  if(modifiers && type != EZ_WIDGET_3D_CANVAS)
		    {
		      if(modifiers & ShiftMask) munit = 1;
		      else if(modifiers & ControlMask) munit = 16;
		      else munit = 4;
		      if(modifiers & Mod1Mask) munit = munit << 2;
		      EZ_WarpPointer(-munit, 0);
		      return;
		    }
		  break;
		case XK_Right: 
#ifdef XK_KP_Right
		case XK_KP_Right:
#endif
		  if(modifiers && type != EZ_WIDGET_3D_CANVAS)
		    {
		      if(modifiers & ShiftMask) munit = 1;
		      else if(modifiers & ControlMask) munit = 16;
		      else munit = 4;
		      if(modifiers & Mod1Mask) munit = munit << 2;
		      EZ_WarpPointer(munit, 0);
		      return;
		    }
		  break;
#endif
		default:
		  break;
		}
	    } /* default */
	  }   /* switch */
      }
      break;
    case ButtonPress:
      {
	/*------------------------------------------
	 *  Search for globally bined mose event.
	 *  The only thing allowed is that a button
	 *  is bined to a popupmenu.
	 *------------------------------------------*/
	if((EZ_WidgetType(widget) != EZ_WIDGET_MENU_BUTTON) &&
	   (EZ_WidgetType(widget) != EZ_WIDGET_3D_CANVAS || 
	    EZ_3DCanvasReleaseMenuButton(widget) != 0))
	  {
	    /*----------------------------------------
	     * leave 3D CANVAS alone so that user
	     * supplied event handle will work
	     * properly
	     *-------------------------------------*/
	    EZ_GlobalEvent *tt =  &EZ_GlobalKeyEvent[EZ_GLOBALEVENT_HASH_SIZE-1];	  
	    EZ_Widget  *twidget = tt->widget;
	    int        which_button = tt->modifiers;  
	    if(event->xbutton.button == which_button &&
	       twidget != (EZ_Widget *)NULL &&
	       EZ_WidgetType(twidget) == EZ_WIDGET_POPUP_MENU)
	      {
		int where;
                EZ_PropagateBgInfo2Widget(widget, twidget);

		if(which_button == EZ_Btn1)
		  where = PopupPosition[0];
		else if(which_button == EZ_Btn2)
		  where = PopupPosition[1];
		else if(which_button == EZ_Btn3)
		  where = PopupPosition[2];
		else 
		  where = 0;
		EZ_DoPopup(twidget,where);
		return;
	      }
	  }
      }
      break;
      /*-----------------------------------------------------------------
       *  So this event is not binded to any global function.
       *----------------------------------------------------------------*/
    case ConfigureNotify:
      {
	/*----------------------------------------------------------------
	 *  Intercept ConfigureNotify events.  Tricky !!!
	 *------------------------------------------------------------*/
       

	if(EZ_WidgetParent(widget) == (EZ_Widget *)NULL) /* a toplevel widget */
	  {
	    int x, y, w, h;
	    
            x = event->xconfigure.x;
	    y = event->xconfigure.y;
	    w = event->xconfigure.width;
	    h = event->xconfigure.height;
	    
	    /* if widget is embeded. Send a message to its embeder inform
	     * it that the size of the embeded window may have been changed.
	     */
	    if(EZ_GetWidgetEmbededFlag(widget)) 
	      {
		Window cwin = EZ_WidgetDnDApplName(widget);
		Window pwin = EZ_WidgetDnDParent(widget);
		if(EZ_VerifyExistence(cwin) && EZ_WindowExist(pwin))
		  {
		    EZ_SendEmbedingMessage(EZ_EMBEDING_WINDOW_RESIZED_S,
					   EZ_DummyWindow, EZ_WidgetWindow(widget),
					   (unsigned long)widget, 
					   cwin, pwin, 0L, 
					   EZ_ApplicationSetupTime,
					   0,0,w,h,
					   EZ_WidgetAMinWidth(widget),EZ_WidgetAMinHeight(widget));
		  }
	      }
	    /* Must a user resize */
	    else if( w != EZ_WidgetWidth(widget) || h != EZ_WidgetHeight(widget)) 
	      {
		struct timeval tv;
		(void) gettimeofday(&tv,NULL);
                if(( (long) tv.tv_sec - Last_CN_HTime) > 1)
                  {
		    Last_CN_HTime = (long) tv.tv_sec;
		    EZ_WidgetWidth(widget) = w;
		    EZ_WidgetHeight(widget) = h;
		    EZ_WidgetWidthHint(widget) = w;
		    EZ_WidgetHeightHint(widget) = h;
		    EZ_ClearWidgetSizeComputedFlag(widget);
		    if(EZ_WidgetMapped(widget))
                      {
                        EZ_ReDisplayWidget(widget); 
                        EZ_ResetWidgetShape(widget, 0);
                      }
		  }
	      }
	    else if( x != 0 && y != 0 && (x != EZ_WidgetOriginX(widget) ||
					  y != EZ_WidgetOriginY(widget)))
	      {
                EZ_WidgetOriginX(widget) = x;
                EZ_WidgetOriginY(widget) = y;
		EZ_SetWMSizeHints(widget, 0);  /* been moved */
		if(EZ_GetWidgetTransientFlag(widget) == 0)
		  EZ_GenerateEmbedingRequest(widget, x,y, w,h);
                if(EZ_WidgetType(widget) == EZ_WIDGET_POPUP_MENU && EZ_PopupIsTearOff(widget))
                  EZ_RedisplayTearOffMenu(widget);
	      }
	  }
	while(XCheckWindowEvent(EZ_Display, win, StructureNotifyMask, event));
        /*  
	if(EZ_WidgetType(widget) != EZ_WIDGET_3D_CANVAS)
	  return;
          */
      }
      break;
    case LeaveNotify:
      break;
    case EnterNotify:
      if(event->xcrossing.focus) 
	{
	  if(widget != EZ_CurrentFocusingWidget) EZ_ResetFocus(widget, 1);
	}
      break;
    case FocusIn:
      if(EZ_WidgetParent(widget) == NULL && !event->xfocus.send_event )
	{
	  if(EZ_CurrentFocusingWidget != widget &&
	     EZ_LookupWidgetFromAllHT(EZ_PendingFocus) == NULL)
	    {
	      EZ_PendingFocus = NULL;
	      EZ_ResetFocus(widget, 0);
	    }
	}
      if(widget == EZ_PendingFocus) EZ_PendingFocus = NULL;
      if(EZ_CurrentFocusingWidget == widget && EZ_GetWidgetFocusableFlag(widget))
	{ EZ_WeAreInFocus = 1; EZ_DrawFocusIndicator(widget, None, FocusIn); }
      break;
    case FocusOut:
      if(EZ_WidgetPadB(widget) > 0 && !event->xfocus.send_event)
	{
	  if(EZ_CurrentFocusingWidget != widget)
	    EZ_DrawFocusIndicator(widget, None, FocusOut);
	  else /* check if we have the focus */
	    {
	      Window fw; int rt, out=0;
	      EZ_Widget *tmp;
	      XGetInputFocus(EZ_Display, &fw, &rt);
	      tmp = EZ_LookupWidgetFromMappedHT(fw);
	      if(tmp == NULL) out = 1;
	      else
		{
		  EZ_Widget *roota, *rootb;
		  roota = widget; while(EZ_WidgetParent(roota)) roota = EZ_WidgetParent(roota);
		  rootb = tmp;    while(EZ_WidgetParent(rootb)) rootb = EZ_WidgetParent(rootb); 
		  if(roota != rootb) out = 1;
		}
	      if(out) EZ_DrawFocusIndicator(widget, None, FocusOut);
	    }
	}
      if(EZ_CurrentFocusingWidget == widget)
	{ EZ_WeAreInFocus = 0; EZ_DrawFocusIndicator(widget, None, FocusOut); }
      break;
    case Expose:
      {
	/*
	 * dig out the last Expose event on win
	 */
	while(XCheckWindowEvent(EZ_Display,win, ExposureMask, event));
      }
      break;
    default:
      break;
    }

  /*--------------------------------------------------------
   * Event occured inside a widget window. 
   *------------------------------------------------------*/
  {
    int i = EZ_WidgetType(widget);
    (EZ_WidgetHandlingFunctions[i]->EventHandle)(widget,event);
  }

  /*
   * if a ButtonPress is sent by us, wait a little bit.
   */
  if(event->type == ButtonPress && event->xbutton.send_event)
    {
     int type = EZ_WidgetType(widget);
     if(type != EZ_WIDGET_MENU_BUTTON && type != EZ_WIDGET_3D_CANVAS)
       {
	 XFlush(EZ_Display);
	 EZ_SitForALittleBit(300000);
       }
    }
}

/********************************************************************************
 * invoke user-defined event handler 
 */
void EZ_InvokePrivateEventHandler(event)
     XEvent *event;
{
  EZ_Widget *widget;
  Window win;
  EZ_EventHandlerStruct *handlers;
  int type;

  win = event->xany.window;
  type  = event->type;
  /*
   * property Notify: used for inter-client communication
   */
  if(type == PropertyNotify && win == EZ_DummyWindow)
    {
      EZ_HandlePropertyNotify(event);
      return;
    }

  widget = EZ_LookupWidgetFromMappedHT(win);
   
  /*
   * client message: communication with the window manager
   */
  if(type == ClientMessage)
    {
      EZ_HandleClientMessage(widget, EZ_CLIENT_MESSAGE, event);
      return;
    }

  /* if event is not associated to a widget, invoke NotWidgetEventHandlers */
  if(widget == (EZ_Widget *)NULL)
    {
      handlers = EZ_NoWidgetEventHandlers;
      while(handlers)
	{
	  if(handlers->handler) (handlers->handler)(NULL,handlers->data, 0, event);
	  handlers = handlers->next;
	}
      return;
    }

  /* Button events and Crossing events hides the current bubble help */
  if(EZ_BubbleHelpOn)
    {
      switch(event->type)
	{
	case EnterNotify:  /* These two events reset LastBubbleWidget */
	  LastBubbleWindow = win;
	  EZ_HideBubbleHelp();   /* remove bubble */
	  EZ_GetCurrentTime( &LastBubbleTime); /* record current time */
	  break;
	case LeaveNotify:
	  LastBubbleWindow = (Window)NULL;
	  EZ_HideBubbleHelp();
	  break;
	  /* These two events block bubble help be displayed for widget */
	case ButtonPress:  
	case ButtonRelease:
	case KeyPress:
	  LastBubbleWindow = (Window)NULL;
	  EZ_HideBubbleHelp();
	  break;
	  break;
	default:
	  break;
	}
    }

  /* now invoke the private event handler */
  handlers = EZ_WidgetEventHandlers(widget);
  
  if(handlers != NULL)
    {
      if(EZ_GrabedWidget)
	{
	  /* if(EZ_WidgetIsViewable(EZ_GrabedWidget))*/
	  if(EZ_LookupWidgetFromAllHT(EZ_GrabedWidget) &&
	     EZ_WidgetWindow(EZ_GrabedWidget) != None)
	    {
	      switch(type)
		{
		case Expose:
		case ConfigureNotify:
		case EnterNotify:
		case LeaveNotify:
		case FocusIn:
		case FocusOut:
		case MotionNotify:
		  break;
		default:
		  return;
		  break;
		}
	    }
	  else EZ_GrabedWidget = NULL;
	}
      {
	int etype = EZ_XEvent2GLEvent(widget, event);
	EZ_WidgetBlocked(widget) += 1;
	while(handlers)
	  {
	    if(handlers->handler) (handlers->handler)(widget,handlers->data, etype, event);
	    handlers = handlers->next;
	  }
	EZ_WidgetBlocked(widget) -= 1;
      }
    }
}

/********************************************************************************/
void EZ_WarpPointer(xx, yy)
     int xx, yy;
{
  int          rx,ry,x,y;
  unsigned int mask;
  Window       root,child_win;
                          
  XQueryPointer(EZ_Display, RootWindow(EZ_Display,EZ_ScreenNum),
		&root,  &child_win,   &rx, &ry,  &x,&y,
		&mask);
  XWarpPointer(EZ_Display, RootWindow(EZ_Display,EZ_ScreenNum),
	       None,  rx, ry, 0, 0, xx, yy);
}
/********************************************************************************
 * 
 *  Handle widget window events
 */

void EZ_HandleWidgetWindowEvent(widget, event)
     EZ_Widget *widget;
     XEvent          *event;
{
  int  i;
  if(widget == (EZ_Widget *)NULL) return;
  
  i = EZ_WidgetType(widget);
  (EZ_WidgetHandlingFunctions[i]->EventHandle)(widget,event);
}

/********************************************************************************
 * 
 *   Set the global popup menu
 */
void  EZ_TiePopupMenu(widget, button, where)
     EZ_Widget *widget;       /* the popup  */
     int   button, where;     /* the button the popup is binded to */
{
  if(widget != (EZ_Widget *)NULL &&
     EZ_WidgetType(widget) == EZ_WIDGET_POPUP_MENU)
    {
      EZ_GlobalEvent *tt =  &EZ_GlobalKeyEvent[EZ_GLOBALEVENT_HASH_SIZE-1];
      tt->modifiers = button;
      tt->widget = widget;
      
      if(button == EZ_Btn1)
	PopupPosition[0] = where;
      else if(button == EZ_Btn2)
	PopupPosition[1] = where;
      else if(button == EZ_Btn3)
	PopupPosition[2] = where;
    }
}

/********************************************************************************
 * 
 *  Handle global key shortcut events. (Allowed widget Type: buttons )
 */
void  EZ_HandleGlobalKeyEvent(widget,x,y)
     EZ_Widget *widget; int x, y;
{
  XEvent       event;
  
  if(widget != (EZ_Widget *)NULL)
    {
      /*---------------------------------------
       *  Just send a button press-release pair
       *---------------------------------------*/
      event.xbutton.type = ButtonPress;
      event.xbutton.display = EZ_Display;
      event.xbutton.window = EZ_WidgetWindow(widget);
      event.xbutton.button = EZ_Btn1;
      event.xbutton.time  = CurrentTime;
      event.xbutton.x = x;
      event.xbutton.y = y;
      XSendEvent(EZ_Display, EZ_WidgetWindow(widget), False,
		 ButtonPressMask, &event);

      event.xbutton.type = ButtonRelease;
      XSendEvent(EZ_Display, EZ_WidgetWindow(widget), False,
		 ButtonReleaseMask, &event);	  
    }
}

static void  EZ_HandleGlobalKeyEventCarefully(widget,event, x, y)
     EZ_Widget *widget;
     XEvent *event; 
     int x, y;
{
  if(widget != (EZ_Widget *)NULL)
    {
      switch(EZ_WidgetType(widget))
	{
	  /* cannot send a button press event, need a double press */
	case EZ_WIDGET_ILIST_BOX: 
	case EZ_WIDGET_IWORK_AREA:
	case EZ_WIDGET_ITREE:
	case EZ_WIDGET_IFANCY_LIST_BOX:
	  {
	    XSendEvent(EZ_Display, EZ_WidgetWindow(widget), False,
		       KeyPressMask, event);
	  }
	  break;
	  /* may be some more later */
	default:
	  EZ_HandleGlobalKeyEvent(widget, x, y);
	  break;
	}
    }
}

/**********************************************************************
 *
 *  Warp the pointer to a widget window. 
 *  if where = 0, warp the pointer near upper left corner.
 *  if where = 1, warp the pointer near the lower right corner.
 */
void EZ_WarpPointerToWidgetWindow(widget, where)
     EZ_Widget *widget;
     int       where;
{
  Window       junkwin, root, win;
  int          wx,wy, rx,ry,x,y;
  unsigned int mask;

  win = EZ_WidgetWindow(widget);
  if(where == 0) { x = 3; y = 3;}
  else if(where > 0)
    {
      x = EZ_WidgetWidth(widget) - 3;
      y = EZ_WidgetHeight(widget) - 3;
    }
  else
    {
      x = -3; y = -3;
    }

  XTranslateCoordinates(EZ_DisplayForWidgets, win,
			RootWindow(EZ_DisplayForWidgets, EZ_ScreenNumForWidgets),
			x,y, &wx,&wy, &junkwin); 
  XQueryPointer(EZ_DisplayForWidgets,
		RootWindow(EZ_DisplayForWidgets, EZ_ScreenNumForWidgets),
		&root,  &junkwin,   &rx, &ry,  &x,&y,  &mask);

  XWarpPointer(EZ_DisplayForWidgets,
	       RootWindow(EZ_DisplayForWidgets,EZ_ScreenNumForWidgets),
	       None,  rx, ry, 0, 0, 
	       (wx -rx), (wy -ry));
}

/********************************************************************************
 ***                                                                          ***
 ***              Handling Globally Binded KeyPress Events                    ***
 ***                                                                          ***
 ********************************************************************************/

/*
 *
 *  Clear the event hash table.
 */
void EZ_InitializeEventHT()
{
  int i;
  for(i = 0; i < EZ_GLOBALEVENT_HASH_SIZE; i++)
    EZ_GlobalKeyEvent[i].next = (EZ_GlobalEvent *)NULL;
  
  /*-----------------------------------------
   * the last one is saved for popupmenu
   *----------------------------------------*/
  EZ_GlobalKeyEvent[i-1].widget = (EZ_Widget *)NULL;
  EZ_GlobalKeyEvent[i-1].modifiers = 0;      
}

/****************************************************
 *
 *  Insert a  global event into the event hash table
 */
void  EZ_InsertGlobalKeyPressEvent(widget, modifiers, key)
     EZ_Widget *widget;
     int             modifiers;
     char            *key;
{
  int           i, hash; 
  EZ_GlobalEvent *event, *prev; 

  hash = EZ_HASH_CHAR((*key));
  /*
   * first, remove any binded accelerator for this widget
   */
  for(i = 0; i < EZ_GLOBALEVENT_HASH_SIZE - 1; i++)
    {
      prev =  &EZ_GlobalKeyEvent[i];
      event = prev->next;
      while(event !=  (EZ_GlobalEvent *)NULL)
	{
	  if(event->widget == widget)
	    {
	      prev->next = event->next;
	      (void)my_free( (char *)event); 
	      break;
	    }
	  prev = event;
	  event= event->next;	    
	}
    }
  /*----------------------------------------
   * insert this event
   *---------------------------------------*/
  prev =  &EZ_GlobalKeyEvent[hash];
  event = (EZ_GlobalEvent *)my_malloc(sizeof(EZ_GlobalEvent),_KEY_EVENT_HT_ENTRY_);
  if(!event) EZ_OutOfMemory("EZ_InsertGlobalKeyPressEvent");
  event->widget = widget;
  event->modifiers = modifiers;
  event->next = prev->next;
  prev->next = event;
}

/******************************************************************
 *
 *  Given a modifier and char, search a event in
 *  the global KeyPress event hash table. x_r and y_r returns
 *  a point inside the window the widget resides.
 */
int EZ_GetGlobalKeyEvent(modifiers, key, root,widget_return, x_r, y_r)
     EZ_Widget *root, **widget_return;
     int             modifiers, *x_r, *y_r;
     char            *key;
{
  int              hash = EZ_HASH_CHAR( (*key));
  EZ_GlobalEvent   *head = &(EZ_GlobalKeyEvent[hash]);
  EZ_GlobalEvent   *event = head->next;
  EZ_Widget        *widget;
  
  while(event != (EZ_GlobalEvent *)NULL)
    {
      if(event->modifiers == modifiers)
	{
	  /*-------------------------------------------
	   * find a match, check if the widget window
	   * is mapped and it is a descendant of root.
	   *------------------------------------------*/
	  widget = event->widget;
	  if(EZ_LookupWidgetFromAllHT(widget) == widget)
	    {
	      if(EZ_WidgetMapped(widget) != 0 && EZ_GetWidgetDisabledFlag(widget) == 0)
		{
		  EZ_Widget *parent = EZ_WidgetParent(widget);
		  int ok = 1;
		  while(parent != (EZ_Widget *)NULL)
		    {
		      if(EZ_WidgetMapped(parent) == 0) {ok = 0; break;}
		      parent = EZ_WidgetParent(parent);
		    }
		  if(ok)
		    {
		      int xx = 2; int yy = 2;
		      EZ_Widget *tmp = widget;
		      while(EZ_WidgetType(tmp) == EZ_WIDGET_NW_LABEL)
			{
			  xx += EZ_WidgetOriginX(tmp);
			  yy += EZ_WidgetOriginY(tmp);
			  tmp = EZ_WidgetParent(tmp);
			}
		      *widget_return = tmp;
		      if(x_r) *x_r = xx;
		      if(y_r) *y_r = yy;
		      head->next = event->next;
		      event->next = NULL;
		      while(head->next != NULL) head = head->next;
		      head->next = event;
		      return(1);
		    }
		}
	       /* try next: widget is not viewable */
	      head = event; 
	      event = event->next;
	    }
	  else
	    {
	      head->next = event->next;
	      (void)my_free(event);
	      event = head->next;
	    }
	}
      else
	{
	  head = event; 
	  event = event->next;
	}
    }
  return(0);
}

/****************************************************
 *
 *  Remove the key shortcut for widget from the 
 *  global KeyEvent table.
 */
void EZ_RemoveGlobalKeyEvent(modifiers, key, widget)
     EZ_Widget *widget;
     int             modifiers;
     char            *key;
{
  int              hash = EZ_HASH_CHAR( (*key));
  EZ_GlobalEvent   *prev =  &EZ_GlobalKeyEvent[hash];
  EZ_GlobalEvent   *event = prev->next;
  
  while(event != (EZ_GlobalEvent *)NULL)
    {
      if(event->widget == widget)
	{
	  prev->next = event->next;
	  (void)my_free( (char *)event);
	  return;
	}
      prev = event;
      event= event->next;
    }
}

void EZ_RemoveAllKeyShortcutForWidget(widget)
     EZ_Widget *widget;     
{
  int              i;
  EZ_GlobalEvent   *prev, *event;


  if(widget != (EZ_Widget *)NULL)
    for(i = 0; i < EZ_GLOBALEVENT_HASH_SIZE - 1; i++)
      {
	prev =  &EZ_GlobalKeyEvent[i];
	event = prev->next;
	while(event !=  (EZ_GlobalEvent *)NULL)
	  {
	    if(event->widget == widget)
	      {
		prev->next = event->next;
		(void)my_free( (char *)event);		
		break;
	      }
	    prev = event;
	    event= event->next;	    
	  }
      }
}

void EZ_RemoveAllGlobalKeyEvents()
{
  int              i;
  EZ_GlobalEvent   *prev, *event;


  for(i = 0; i < EZ_GLOBALEVENT_HASH_SIZE - 1; i++)
    {
      prev =  &EZ_GlobalKeyEvent[i];
      event = prev->next;
      prev->next = (EZ_GlobalEvent *)NULL;
      
      while(event !=  (EZ_GlobalEvent *)NULL)
	{
	  prev = event;
	  event= event->next;	    
	  (void)my_free( (char *)prev);		
	}
    }
}
  
/***************************************************************************************/

static int EZ_HASH_CHAR(c)
     char c;
{
  if(isupper(c)) c = tolower(c); 
  return (((int) c) & 0x7F);
}

/***************************************************************************************/

int EZ_CheckTimerEvents() 
{
  struct timeval ctv,*tv;
  long ts, tus;
  
  tv = &ctv;
  EZ_GetCurrentTime(tv);
  ts = tv->tv_sec; tus = tv->tv_usec;
  
  /******* handle bubble help ************/
  if(EZ_BubbleHelpOn)
    {
      EZ_Widget *widget;
      long       delay;

      delay = (ts - LastBubbleTime.tv_sec) * 1000000 +
	(tus - LastBubbleTime.tv_usec);
    
      if(delay > 800000 &&
	 (widget = EZ_LookupWidgetFromMappedHT(LastBubbleWindow)) &&
	 EZ_WidgetBubbleString(widget))
	{
	  int             rx,ry,x,y,w,h;
	  unsigned int    mask;
	  Window          root,win;
   
	  XQueryPointer(EZ_Display, RootWindow(EZ_Display, EZ_ScreenNum),
			&root,              /* root return win */
			&win,               /* child ret win   */
			&rx, &ry,           /* x, y in root    */
			&x,&y,              /* x, y in win     */
			&mask
			);

          rx += 2;
          ry += 8;
	  EZ_SetWidgetPosition(EZ_BubbleHelp,rx,ry);
	  EZ_SetWidgetLabelString(EZ_BubbleHelp,EZ_WidgetBubbleString(widget));
          EZ_ComputeWidgetWindowSize(EZ_BubbleHelp, &w, &h,NULL);
          if(rx + w > (int)EZ_XDisplayWidth )
            EZ_WidgetOriginX(EZ_BubbleHelp) = (int)EZ_XDisplayWidth - w;
          if(ry + h > (int)EZ_XDisplayHeight)
            EZ_WidgetOriginY(EZ_BubbleHelp) = ry -10 -h;
	  EZ_DisplayWidget(EZ_BubbleHelp);
	  LastBubbleWindow = (Window) NULL;
	}
    }
  /******* end of bubble help ************/
  return(EZ_CheckRealTimers(ts, tus));
}
/***************************************************************************************/
static void EZ_HideBubbleHelp()
{
  EZ_Widget *wdt = EZ_BubbleHelp;

  if(EZ_WidgetMapped(wdt) != 0)
    {
      XUnmapWindow(EZ_Display, EZ_WidgetWindow(wdt));
      EZ_WidgetMapped(wdt) = 0;
    }
}
/***************************************************************************************/
static void EZ_HandleClientMessage(widget, eventType, event)
     EZ_Widget *widget;
     int       eventType;
     XEvent    *event;
{
  XClientMessageEvent *ev = (XClientMessageEvent *)event;   
  Atom mtype = ev->message_type;
  
  if(mtype == EZ_WM_PROTOCOLS_ATOM)
    {
      Atom c = (ev->data.l)[0];
      if(c == EZ_WM_DELETE_WINDOW_ATOM)
	{
	  if(widget && EZ_WidgetType(widget) == EZ_WIDGET_POPUP_MENU &&
	     EZ_PopupIsTearOff(widget))
	    {
	      EZ_Widget *bar = EZ_PopupIsTearLink(widget);
	      if(EZ_LookupWidgetFromAllHT(bar) == bar &&
		 EZ_WidgetType(bar) == EZ_WIDGET_MENU_TEAR_OFF_BAR)
		{
		  EZ_Widget *menu = EZ_WidgetParent(bar);
		  EZ_PopupCompanion(menu) = (EZ_Widget *)NULL;
		  EZ_MenuTearOffBarMenu(bar) = (EZ_Widget *)NULL;
		}
	      EZ_DestroyWidget(widget);
	      return;
	    }
	}
    }
  if(EZ_ClientMessageHandler.handler) 
    {
      (EZ_ClientMessageHandler.handler)(widget, EZ_ClientMessageHandler.data, 
					eventType, event);
    }
  else 
    {
      if(mtype == EZ_WM_PROTOCOLS_ATOM)
	{
	  Atom c = (ev->data.l)[0];
	  if(c == EZ_WM_DELETE_WINDOW_ATOM)
	    {
	      EZ_Shutdown();
	      exit(EZ_EXIT_OK);
	    }
	}
    }
}
/***************************************************************************************/
void EZ_RemoveEvent(event)
     XEvent *event;
{
  if(event) event->type  = - event->type;
}
/***************************************************************************************/
void EZ_TFEvents()
{
  (void) EZ_CheckTimerEvents();
  (void) EZ_CheckAppInputs(0);
}
/***************************************************************************************/
#ifdef EZ_DEBUG
void EZ_DumpGEventTable()
{
  int i;
  EZ_GlobalEvent   *event;  
  for(i = 0; i <  EZ_GLOBALEVENT_HASH_SIZE; i++)
    {
      event =  EZ_GlobalKeyEvent[i].next;
      while(event != (EZ_GlobalEvent *)NULL)
	{
	  printf("%c===> %d  %d  %lx\n",(char)i, event->modifiers,
		 EZ_WidgetType(event->widget),(unsigned long)event->widget);
	  event = event->next;
	}
    }
}
#endif

#undef _EZ_WIDGET_EVENT_C_
