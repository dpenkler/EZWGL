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
 ***              3D Canvas  Event Handle                      ***
 ***                                                           ***
 *****************************************************************/
#define _EZ_GL_EVENT_C_

#include "EZ_Widget.h"

/*********************************************************************
 * 
 *  Functions implemented in this file:
 */
int   EZ_XEvent2GLEvent MY_ANSIARGS((EZ_Widget *widget, XEvent *event));
int   EZ_GLNextEvent MY_ANSIARGS((EZ_Widget *widget, XEvent *event));
int   EZ_GLEnterEvent MY_ANSIARGS(( EZ_Widget *widget,
				    int             event_type,
				    int             *values));
int   EZ_EnterEvent MY_ANSIARGS(( EZ_Widget *widget,
				   int             event_type,
				   int             *values));
void  EZ_GLResetEvents MY_ANSIARGS((EZ_Widget *widget));
int   EZ_UpdateButtonState MY_ANSIARGS((XEvent *event));

/*********************************************************************
 * 
 *  Global  Variables.
 */
int           EZ_PointerCoordinates[2], EZ_MouseX, EZ_MouseY;
unsigned int  EZ_PressedKey, EZ_ButtonState;
unsigned int  EZ_Btn1,  EZ_Btn2, EZ_Btn3;

/*********************************************************************
 *
 *  Create a 3D Canvas widget
 */
int EZ_XEvent2GLEvent(widget,event)
     EZ_Widget *widget;
     XEvent          *event;
{
  switch(event->type)
    {
    case Expose:
      return(EZ_REDRAW); 
      break;
    case ConfigureNotify:
      return(EZ_RESIZE);
      break;
    case ButtonPress:
    case ButtonRelease:
      {
	int w, h,bw, button;
	    
	bw = EZ_WidgetBorderWidth(widget) + EZ_WidgetPadB(widget);
	w =  EZ_WidgetWidth(widget);
	h =  EZ_WidgetHeight(widget);
	button = event->xbutton.button;

	EZ_PointerCoordinates[0] = event->xbutton.x;
	EZ_PointerCoordinates[1] = event->xbutton.y;

	EZ_MouseX = event->xbutton.x - bw;
	EZ_MouseY = h - event->xbutton.y - bw;
	return(EZ_UpdateButtonState(event));
      }
      break;
    case MotionNotify:
      {
	int w, h,bw;
	XEvent tmp;

	bw = EZ_WidgetBorderWidth(widget) + EZ_WidgetPadB(widget);
	w =  EZ_WidgetWidth(widget);
	h =  EZ_WidgetHeight(widget);
	
	EZ_PointerCoordinates[0] = event->xmotion.x;
	EZ_PointerCoordinates[1] = event->xmotion.y;

	EZ_MouseX = event->xmotion.x - bw;
	EZ_MouseY = h - event->xmotion.y - bw;
	while(XCheckTypedEvent(EZ_Display, MotionNotify, &tmp))
	  EZ_FilterEvent(&tmp);
	return(EZ_POINTER_MOTION);
      }
      break;
    case KeyPress:
      {
	char str[2];
	KeySym keysym;
	int    count;
	count = XLookupString(&(event->xkey), str,1,&keysym, NULL);
	if(count > 0)  /* must be an ascii char */
	  {
	    EZ_PressedKey = str[0];
	    return(EZ_KEY_PRESS);
	  }
	else /* no ascii char */
	  {
	    switch(keysym)
	      {
	      case XK_Prior:   EZ_PressedKey = EZ_PAGE_UP_KEY;    break;
	      case XK_Next:    EZ_PressedKey = EZ_PAGE_DOWN_KEY;  break;
	      case XK_Home:    EZ_PressedKey = EZ_HOME_KEY;  break;
	      case XK_Left: 
#ifdef XK_KP_Left
	      case XK_KP_Left:
#endif
		EZ_PressedKey = EZ_LEFT_KEY;  break;
	      case XK_Right: 
#ifdef XK_KP_Right
	      case XK_KP_Right:
#endif
		EZ_PressedKey = EZ_RIGHT_KEY; break;
	      case XK_Up: 
#ifdef XK_KP_Up
	      case XK_KP_Up:
#endif
		EZ_PressedKey = EZ_UP_KEY;    break;
	      case XK_Down:   
#ifdef XK_KP_Down
	      case XK_KP_Down:
#endif
		EZ_PressedKey = EZ_DOWN_KEY;  break;
	      case XK_Begin:   EZ_PressedKey = EZ_BEGIN_KEY; break;
	      case XK_End:     EZ_PressedKey = EZ_END_KEY;   break;
	      case XK_F1:      EZ_PressedKey = EZ_F1_KEY;    break;
	      case XK_F2:      EZ_PressedKey = EZ_F2_KEY;    break;
	      case XK_F3:      EZ_PressedKey = EZ_F3_KEY;    break;
	      case XK_F4:      EZ_PressedKey = EZ_F4_KEY;    break;
	      case XK_F5:      EZ_PressedKey = EZ_F5_KEY;    break;
	      case XK_F6:      EZ_PressedKey = EZ_F6_KEY;    break;
	      case XK_F7:      EZ_PressedKey = EZ_F7_KEY;    break;
	      case XK_F8:      EZ_PressedKey = EZ_F8_KEY;    break;
	      case XK_F9:      EZ_PressedKey = EZ_F9_KEY;    break;
	      case XK_F10:     EZ_PressedKey = EZ_F10_KEY;   break;
	      case XK_F11:     EZ_PressedKey = EZ_F11_KEY;   break;
	      case XK_F12:     EZ_PressedKey = EZ_F12_KEY;   break;
	      default:         return(0);  break;
	      }
	    return(EZ_KEY_PRESS);
	  }
      }
      break;
    case EnterNotify:
      return(EZ_ENTER_WINDOW);
      break;
    case LeaveNotify:
      return(EZ_LEAVE_WINDOW);
      break;
    default:
      return(0);
      break;
    }
  return(0);
}
/*************************************************************************
 *
 *  Check the Xlib event queue to see if there is an event on the 
 *  current graphics window. Return the corresponding event if there
 *  is one. Return 0 if none.
 */

int EZ_GLNextEvent(widget, event)
     EZ_Widget *widget;
     XEvent *event;
{
  XEvent           xevent;
  Window           win;
  EZ_Widget  *canvas = widget;

  if(canvas == (EZ_Widget *)NULL && EZ_Context)
    canvas = EZ_CurrentGWindow;
  if(canvas != (EZ_Widget *)NULL) 
    win = EZ_WidgetWindow(canvas);
  else win = (Window)NULL;

  if(event == NULL) event = &xevent;

  while(1)
    {
      if(XPending(EZ_Display) != 0) 
	{
	  XNextEvent(EZ_Display, event);
	  EZ_FilterEvent(event);
          EZ_InvokePrivateEventHandler(event);
	  if(event->xany.window != win)
	    {
	      EZ_InvokePrivateEventHandler(event);
	      EZ_WidgetDispatchEvent(event);
	      return(0);
	    }
	  else return(EZ_XEvent2GLEvent(canvas,event));
	}
      else
	{
	  (void) EZ_CheckTimerEvents();
	  (void) EZ_CheckAppInputs(0);
	  EZ_SitForALittleBit(1000); 
	}
    }
  return(0);
}

/*************************************************************************
 *
 *  Enter an X event to a widget window
 */
int EZ_GLEnterEvent(canvas,event_type, values)
     EZ_Widget *canvas;
     int             event_type;
     int             *values;
{
  EZ_Widget *widget = canvas;
  if(widget == (EZ_Widget *)NULL)
    if(EZ_Context != NULL) widget = EZ_CurrentGWindow;

  if(widget && EZ_WidgetType(widget) == EZ_WIDGET_3D_CANVAS)
    return(EZ_EnterEvent(widget, event_type, values));
  return(0);
}


int EZ_EnterEvent(widget, event_type, values)
     EZ_Widget *widget;
     int       event_type;
     int       *values;
{
  XEvent  xevent;
  Window  win;
  int     h, bw,ok = 1, isCanvas = 0;
  long    event_mask = 0;

  if(widget == (EZ_Widget *)NULL || (EZ_WidgetMapped(widget) == 0))
    return(0);
  if(EZ_WidgetType(widget) == EZ_WIDGET_3D_CANVAS)
    { bw = EZ_WidgetBorderWidth(widget) + EZ_WidgetPadB(widget); isCanvas = 1;}
  else bw = 0;

  h = EZ_WidgetHeight(widget);
  win = EZ_WidgetWindow(widget);
  
  xevent.xany.display = EZ_Display;
  xevent.xany.window = win;
  switch(event_type)
    {
    case EZ_REDRAW:
    case EZ_RESIZE:
      xevent.xexpose.type = Expose;
      xevent.xexpose.count = 0;
      event_mask = ExposureMask;
      break;
    case EZ_LEFT_BUTTON_PRESS:
    case EZ_MIDDLE_BUTTON_PRESS:
    case EZ_RIGHT_BUTTON_PRESS:
      event_mask = ButtonPressMask;
      xevent.xbutton.type = ButtonPress;
      xevent.xbutton.button = (event_type == EZ_LEFT_BUTTON_PRESS ? EZ_Btn1:
			       (event_type == EZ_MIDDLE_BUTTON_PRESS ? EZ_Btn2: EZ_Btn3));
      if(values)
	{
	  xevent.xbutton.x = (int)values[0];
	  xevent.xbutton.y = (int)values[1];
	}
      else
	{
	  if(isCanvas)
	    {
	      xevent.xbutton.x = EZ_MouseX + bw;
	      xevent.xbutton.y = h - EZ_MouseY -bw;
	    }
	  else
	    {
	      xevent.xbutton.x = EZ_MouseX + bw;
	      xevent.xbutton.y = EZ_MouseY + bw;
	    }
	}
      break;
    case EZ_LEFT_BUTTON_RELEASE:
    case EZ_MIDDLE_BUTTON_RELEASE:
    case EZ_RIGHT_BUTTON_RELEASE:
      event_mask = ButtonReleaseMask;
      xevent.xbutton.type = ButtonRelease;
      xevent.xbutton.button = (event_type == EZ_LEFT_BUTTON_PRESS ? EZ_Btn1:
			       (event_type == EZ_MIDDLE_BUTTON_PRESS ? EZ_Btn2: EZ_Btn3));
      if(values)
	{
	  xevent.xbutton.x = (int)values[0];
	  xevent.xbutton.y = (int)values[1];
	}
      else
	{
	  if(isCanvas)
	    {
	      xevent.xbutton.x = EZ_MouseX + bw;
	      xevent.xbutton.y = h - EZ_MouseY -bw;
	    }
	  else
	    {
	      xevent.xbutton.x = EZ_MouseX + bw;
	      xevent.xbutton.y = EZ_MouseY + bw;
	    }
	}
      break;
    case EZ_POINTER_MOTION:
      event_mask = PointerMotionMask;
      xevent.xmotion.type = MotionNotify;
      if(values)
	{
	  xevent.xbutton.x = (int)values[0];
	  xevent.xbutton.y = (int)values[1];
	}
      else
	{
	  if(isCanvas)
	    {
	      xevent.xbutton.x = EZ_MouseX + bw;
	      xevent.xbutton.y = h - EZ_MouseY -bw;
	    }
	  else
	    {
	      xevent.xbutton.x = EZ_MouseX + bw;
	      xevent.xbutton.y = EZ_MouseY + bw;
	    }
	}
      break;
    case EZ_ENTER_WINDOW:
      event_mask = EnterWindowMask;
      xevent.xcrossing.type = EnterNotify;
      break;
    case EZ_LEAVE_WINDOW:
      event_mask = LeaveWindowMask;
      xevent.xcrossing.type = LeaveNotify;
      break;
    case EZ_KEY_PRESS:
      {
	char tch[2];
	unsigned int kstate = 0;
	if(values)
	  {
	    tch[0] = (char)values[0]; tch[1]='\0';
	    kstate = ((unsigned int)values[1]) & (ControlMask | Mod1Mask | ShiftMask);
	  }
	else
	  tch[0] = '\0';
	event_mask = KeyPressMask;
	xevent.xkey.type = KeyPress;
	xevent.xkey.keycode = XKeysymToKeycode(EZ_Display, XStringToKeysym(tch));
	xevent.xkey.state = kstate;
      }
      break;
    default:
      ok = 0;
      break;
    }
  if(ok)
    XSendEvent(EZ_Display,win, False, event_mask, &xevent);

  return(ok);  
}

/***************************************************************
 *
 *  Reset the X event queue
 */
void  EZ_GLResetEvents(widget)
     EZ_Widget *widget;
{
  EZ_Widget *canvas = widget;
  if(canvas == NULL && EZ_Context != NULL) canvas =  EZ_CurrentGWindow;
  if(canvas != (EZ_Widget *)NULL && EZ_WidgetType(canvas) == EZ_WIDGET_3D_CANVAS &&
     EZ_WidgetMapped(canvas))
    {
      XEvent xevent;
      XSync(EZ_Display, False);
      while(XCheckWindowEvent(EZ_Display, EZ_MainWindow,
			      KeyPressMask|ButtonReleaseMask|ExposureMask|
			      ButtonPressMask|StructureNotifyMask|ButtonMotionMask|
			      PointerMotionMask,
			      &xevent))
	EZ_FilterEvent(&xevent);
    }
}
/***************************************************************
 *
 *  update EZ_ButtonState and return the corresponding event
 *  identifier.
 */
int EZ_UpdateButtonState(event)
     XEvent *event;
{
  int button = event->xbutton.button;
  if(event->type == ButtonPress)
    {
      if(button == EZ_Btn1)
	{
	  EZ_ButtonState |= EZ_BUTTON1;
	  return(EZ_LEFT_BUTTON_PRESS);
	}
      else if(button == EZ_Btn2)
	{
	  EZ_ButtonState |= EZ_BUTTON2;
	  return(EZ_MIDDLE_BUTTON_PRESS);
	}
      else if(button == EZ_Btn3)
	{
	  EZ_ButtonState |= EZ_BUTTON3;
	  return(EZ_RIGHT_BUTTON_PRESS);
	}
      else
	{
	  (void)fprintf(stderr,"Warning: Unknown Button type in EZ_NextEvent()\n");
	  return(0);
	}
    }
  else
    {
      if(button == EZ_Btn1)
	{
	  EZ_ButtonState &= ~EZ_BUTTON1;
	  return(EZ_LEFT_BUTTON_RELEASE);
	}
      else if(button == EZ_Btn2)
	{
	  EZ_ButtonState &= ~EZ_BUTTON2;
	  return(EZ_MIDDLE_BUTTON_RELEASE);
	}
      else if(button == EZ_Btn3)
	{
	  EZ_ButtonState &= ~EZ_BUTTON3;
	  return(EZ_RIGHT_BUTTON_RELEASE);
	}
      else
	{
	  (void)fprintf(stderr,"Warning: Unknown Button type in EZ_NextEvent()\n");
	  return(0);
	}
    }
}
/****************************************************************************************/
#undef _EZ_GL_EVENT_C_
