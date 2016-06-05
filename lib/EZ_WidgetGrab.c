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
 *            Simple Local Grabs
 */
#define _EZ_WIDGET_GRAB_C_
#include "EZ_Widget.h"

/*******************************************************************
 *
 * Functions implemented in this file:
 */
void            EZ_SetGrab MY_ANSIARGS((EZ_Widget *widget));
void            EZ_ReleaseGrab MY_ANSIARGS((void));
int             EZ_WidgetIsViewable MY_ANSIARGS((EZ_Widget *widget));
void            EZ_RedisplayWidgetTree MY_ANSIARGS((EZ_Widget *widget));
void            EZ_RaiseWidgetWindow MY_ANSIARGS((EZ_Widget *widget));
void            EZ_BlinkWidget  MY_ANSIARGS((EZ_Widget *widget, int cnt));
int             EZ_GrabPointer MY_ANSIARGS((EZ_Widget *widget, Cursor csr, int tout));
void            EZ_UngrabPointer MY_ANSIARGS((void));

/*******************************************************************
 *
 * Local Functions:
 */
EZ_Widget *EZ_GrabedWidget = (EZ_Widget *)NULL;

void  EZ_SetGrab(widget)
     EZ_Widget *widget;
{
  EZ_GrabedWidget = widget;
}

void EZ_ReleaseGrab()
{
  EZ_GrabedWidget = NULL;
}

int EZ_WidgetIsViewable(widget)
     EZ_Widget *widget;
{
  if(widget)
    {
      int ok = (EZ_LookupWidgetFromAllHT(widget) &&
		EZ_WidgetWindow(widget) != (Window)NULL &&
		EZ_WidgetMapped(widget));
      if(ok)
	{
	  EZ_Widget *parent = EZ_WidgetParent(widget);
	  while(parent)
	    {
	      if(EZ_WidgetMapped(parent) == 0) return(0);
	      parent = EZ_WidgetParent(parent);
	    }
	  return(1);
	}
    }
  return(0);
}

void EZ_RedisplayWidgetTree(widget)
     EZ_Widget *widget;
{
  if(widget)
    {
      while(EZ_WidgetParent(widget))
	{
	  widget = EZ_WidgetParent(widget);
	}
      EZ_ReDisplayWidget(widget);
      XRaiseWindow(EZ_Display,EZ_WidgetWindow(widget));	  
      XSync(EZ_Display, False);
    }
}
void EZ_RaiseWidgetWindow(widget)
     EZ_Widget *widget;
{
  if(EZ_WidgetIsViewable(widget))
    EZ_RedisplayWidgetTree(widget);
}

void EZ_BlinkWidget(widget, cnt) 
     EZ_Widget *widget; int cnt;
{
  if(widget && EZ_LookupWidgetFromAllHT(widget))
    {
      int x0, y0, type = EZ_WidgetType(widget);
      int w0 = EZ_WidgetWidth(widget);
      int h0 = EZ_WidgetHeight(widget);      
      if(type != EZ_WIDGET_NW_LABEL)
        {
          x0 = 0; y0 = 0;
        }
      else
        {
          EZ_Widget *parent = EZ_WidgetParent(widget);

          x0 = EZ_WidgetOriginX(widget);
          y0 = EZ_WidgetOriginY(widget);

          while(parent && EZ_WidgetType(parent) == EZ_WIDGET_NW_LABEL)
            {
              x0 += EZ_WidgetOriginX(parent);
              y0 += EZ_WidgetOriginY(parent);
              parent = EZ_WidgetParent(parent);
            }
          widget = parent;
        }
      if(EZ_WidgetWindow(widget) != (Window)NULL && EZ_WidgetMapped(widget))
        {
          Window       junk, win = EZ_WidgetWindow(widget);
          int          rx,ry, i;
          Pixmap pixmap;
          
          /* if toplevel, raise the widget window */
          if(EZ_WidgetParent(widget) == NULL)
            {
              EZ_ReDisplayWidget(widget);
              XRaiseWindow(EZ_Display,EZ_WidgetWindow(widget));	  
              XSync(EZ_Display, False);
            }
          if(cnt > 0)
            {
              XTranslateCoordinates(EZ_Display, win, RootWindow(EZ_Display, EZ_ScreenNum),
                                    0, 0, &rx, &ry, &junk);
              pixmap = XCreatePixmap(EZ_Display, win, w0, h0, EZ_Depth); 
              XFillRectangle(EZ_Display, pixmap, EZ_WHITEGC, 0,0, w0, h0); 
              cnt <<= 1;
              for(i = 0; i < cnt; i++)
                {
                  XCopyArea(EZ_Display, pixmap,
                            RootWindow(EZ_Display,EZ_ScreenNum),
                            EZ_XORGC, 0,0,w0,h0, rx+x0, ry+y0);
                  XFlush(EZ_Display);
                  EZ_SitForALittleBit(400000);
                }
              XFreePixmap(EZ_Display, pixmap);
            }
        }
    }
}
/******************************************************************/
static EZ_Widget *ptr_g_widget;

static void timer_callback(timer, cdata)
     EZ_Timer *timer; 
     void *cdata;
{
  EZ_Widget *widget = (EZ_Widget *)cdata;
  if(EZ_PointerGrabed && ptr_g_widget == widget)
    {
      XUngrabPointer(EZ_Display, CurrentTime);
      EZ_PointerGrabed = 0;
      ptr_g_widget = NULL;
    }
}
/******************************************************************/

int  EZ_GrabPointer(widget, cursor, tout)
     EZ_Widget *widget;
     Cursor    cursor;
     int       tout;
{
  if(EZ_WidgetIsViewable(widget))
    {
      int status;
      if(EZ_PointerGrabed == 0)
	{ 
	  status = XGrabPointer(EZ_Display, EZ_WidgetWindow(widget), True, 0L,
				GrabModeAsync, GrabModeAsync, None,
				(cursor == (Cursor)NULL)? None:cursor, CurrentTime);
	  if(status == GrabSuccess)
	    {
	      ptr_g_widget = widget;
	      EZ_PointerGrabed = 8;      
	      if(tout > 0) (void) EZ_CreateTimer((long)tout, 0, 1, timer_callback, (void *)widget, 0);
	    }
	  return(status);
	}
      else if(EZ_PointerGrabed == 8 && cursor != (Cursor)NULL)
	{
	  XChangeActivePointerGrab(EZ_Display, 0L, cursor, CurrentTime);
	  return(AlreadyGrabbed);
	}
    }
  return(GrabNotViewable);
}

void EZ_UngrabPointer()
{
  if(EZ_PointerGrabed && ptr_g_widget != NULL)
    {
      XUngrabPointer(EZ_Display, CurrentTime);
      EZ_PointerGrabed = 0;
      ptr_g_widget = NULL;
    }
}
/******************************************************************/
#undef _EZ_WIDGET_GRAB_C_
