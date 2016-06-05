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
 *
 *   Utility functions.
 *
 *****************************************************************/
#define _EZ_WIDGET_MISC_C_
#include "EZ_Widget.h"

void EZ_SlideOpenWidgetRelativeTo MY_ANSIARGS((EZ_Widget *w, EZ_Widget *r, int dir, int sp));
void EZ_SlideOpenWidget MY_ANSIARGS((EZ_Widget *w, int x, int y, int dir, int sp));
void EZ_SlideCloseWidget MY_ANSIARGS((EZ_Widget *w, int dir, int sp));

/*****************************************************************/

void EZ_CallWidgetCallbacks(widget)
     EZ_Widget *widget;
{
  EZ_ExecuteWidgetCallBacks(widget);
}

void EZ_CallWidgetMotionCallbacks(widget)
     EZ_Widget *widget;
{
  if(EZ_LookupWidgetFromAllHT(widget) != (EZ_Widget *)NULL)
    {
      EZ_HandleMotionCallBacks(widget,
			       EZ_WidgetMotionCallBack(widget));
    }
}
EZ_Widget *EZ_GetToplevelWidget(widget)
     EZ_Widget *widget;
{
  if(EZ_LookupWidgetFromAllHT(widget) != (EZ_Widget *)NULL)
    {
      EZ_Widget *parent = widget;
      while(EZ_WidgetParent(parent) != NULL)
	{ parent = EZ_WidgetParent(parent);}
      return(parent);
    }
  return(NULL);
}

/* Find a window with WM_STATE, else return win itself, as per ICCCM */
/* modified from:  $XConsortium: ClientWin.c */

static Window TryChildren MY_ANSIARGS((Display *dpy, Window win, Atom atom));

Window EZ_FindClientWindow(win)
     Window  win;
{
    Atom          WM_STATE;
    Atom          type = None;
    int           format;
    unsigned long nitems, after;
    unsigned char *data;
    Window        inf;
    Display       *dpy = EZ_Display;
    int            (*OldErrorHandler)();
      
    WM_STATE = XInternAtom(dpy, "WM_STATE", True);
    if(!WM_STATE) return win;
    
    if(!EZ_WindowExist(win)) return(None);
    EZ_XErrorCode = 0;
    OldErrorHandler = XSetErrorHandler(EZ_XErrorHandler);
    XGetWindowProperty(dpy, win, WM_STATE, 0, 0, False, AnyPropertyType,
		       &type, &format, &nitems, &after, &data);
    XSync(EZ_Display, False);
    XSetErrorHandler(OldErrorHandler);  
    if(EZ_XErrorCode != 0) return(win);

    if(type) return(win);
    inf = TryChildren(dpy, win, WM_STATE);
    if(!inf) inf = win;
    return(inf);
}

static Window TryChildren(dpy, win, WM_STATE)
     Display *dpy;
     Window win;
     Atom WM_STATE;
{
    Window        root, parent, *children;
    unsigned int  i, nchildren;
    Atom          type = None;
    int           format;
    unsigned long nitems, after;
    unsigned char *data;
    Window        inf = None;

    if(!XQueryTree(dpy, win, &root, &parent, &children, &nchildren)) return(None);
    for(i = 0; !inf && (i < nchildren); i++) 
      {
	XGetWindowProperty(dpy, children[i], WM_STATE, 0, 0, False,
			   AnyPropertyType, &type, &format, &nitems,
			   &after, &data);
	if(type)inf = children[i];
      }
    for(i = 0; !inf && (i < nchildren); i++)
      inf = TryChildren(dpy, children[i], WM_STATE);
    if(children) XFree((char *)children);
    return(inf);
}
/*****************************************************************/
static int findSlideDirection(x,y,w,h) int x, y, w, h;
{
  int direction;
  int dw =  EZ_XDisplayWidth/2;
  int dh =  EZ_XDisplayHeight/2;
  if(x+w <= dw) direction = EZ_EAST;
  else if(y+h < dh) direction = EZ_SOUTH;
  else if(x > dw) direction = EZ_WEST;
  else if(y > dh) direction = EZ_NORTH;
  else direction = EZ_SOUTHEAST;
  return(direction);
}

void EZ_SlideOpenWidgetRelativeTo(widget, rel, direction, speed)
     EZ_Widget *widget, *rel; int direction, speed;
{
  int x=0, y=0;
  if(widget && EZ_WidgetExist(rel) && EZ_WidgetMapped(rel))
    {
      int w1, h1;
      int w  = EZ_WidgetWidth(rel);
      int h  = EZ_WidgetHeight(rel);
      Window junkwin;
      XTranslateCoordinates(EZ_Display, EZ_WidgetWindow(rel),
                            RootWindow(EZ_Display, EZ_ScreenNum),
                            0,0, &x,&y, &junkwin); 
      if(direction < 0) direction = findSlideDirection(x,y,0,0);
      EZ_ComputeWidgetWindowSize(widget, &w1, &h1, EZ_WidgetParent(widget));

      switch(direction)
        {
        case EZ_EAST:  x += w; y -= (h1-h)/2; break;
        case EZ_WEST:  y-=  (h1-h)/2;  break;
        case EZ_NORTHEAST:  break;
        case EZ_NORTHWEST: x += w; break;
        case EZ_SOUTH: x -= (w1-w)/2; y += h; break;
        case EZ_NORTH: x -= (w1-w)/2; break;
        case EZ_SOUTHEAST: 
          y += h; break;
        case EZ_SOUTHWEST: 
          x += w; y += h; break;
        default: break;
        }
    }
  EZ_SlideOpenWidget(widget, x, y, direction, speed);
}

/*****************************************************************/
void EZ_SlideOpenWidget(widget, x, y, direction, speedIn)
     EZ_Widget *widget; int x, y, direction, speedIn;
{
  if(EZ_WidgetExist(widget))
    {
      int sx,sy, x0,y0, itmp, jtmp,  w, h, w1, h1;
      int speedx, speedy, ss=0;
      Window window;
      EZ_Widget *parent = EZ_WidgetParent(widget);
      if(EZ_WidgetWindow(widget) == (Window)None)
        EZ_MakeWidgetWindows(widget);
      window = EZ_WidgetWindow(widget);
      EZ_ComputeWidgetWindowSize(widget, &w, &h, parent);
      w1=w = EZ_WidgetWidth(widget);
      h1=h = EZ_WidgetHeight(widget);

      if(parent)
        {
          if( !EZ_WidgetIsViewable(parent)) return;
          sx=x0 = EZ_WidgetOriginX(widget);
          sy=y0 = EZ_WidgetOriginY(widget);
          if(direction < 0)
            direction = findSlideDirection(x0,y0,0,0);
          switch(direction)
            {
            case EZ_EAST:   break;
            case EZ_WEST:  x0 += w; break;
            case EZ_NORTHEAST: y0 += h; break;
            case EZ_NORTHWEST: x0 += w; y0 += h; break;
            case EZ_SOUTH:  break;
            case EZ_SOUTHEAST: break;
            case EZ_SOUTHWEST: x0 += w; break;
            default: break;
            }
        }
      else 
        {
          sx=x0 = x;   sy=y0 = y;
          if(direction < 0)
            direction = findSlideDirection(x0,y0,0,0);
        }
      
      if(speedIn <= 0) speedIn = 1;
      if(w > h) /* wider */
        {
          speedx = speedIn;
          speedy = speedIn * h/w;
          if(speedy == 0) speedy = 1;
        }
      else
        {
          speedy = speedIn;
          speedx = speedIn * w/h;
          if(speedx == 0) speedx = 1;
        }

      EZ_SetWidgetSizeComputedFlag(widget);
      EZ_WidgetOriginX(widget) = x0;
      EZ_WidgetOriginY(widget) = y0; 
      EZ_SetWidgetForgetGFlag(widget);
      
      switch(direction)
        {
        case EZ_EAST:
          {
            itmp = 2;
            EZ_WidgetWidth(widget) = 2;
            while(itmp != w)
              {
                itmp += speedx;
                if(itmp > w) itmp = w;
                XMoveResizeWindow(EZ_Display, window, x0, y0, itmp, h);
                if(ss==0) {EZ_DisplayWidget(widget); ss = 1;}
                XSync(EZ_Display, False);
              }
          }
        break;
        case EZ_WEST:
          {
            itmp = 2;
            EZ_WidgetWidth(widget) = 2;
            while(itmp != w)
              {
                itmp += speedx;
                x0 -= speedx;
                if(itmp > w)
                  {
                    itmp = w;
                    x0 = x-w;
                  }
                XMoveResizeWindow(EZ_Display, window, x0, y0, itmp, h);
                if(ss==0) {EZ_DisplayWidget(widget); ss = 1;}
                XSync(EZ_Display, False);
              }    
          }
        break;
        case EZ_NORTH:
          {
            itmp = 2;
            EZ_WidgetHeight(widget) = 2;
            while(itmp != h)
              {
                itmp += speedy;
                y0 -= speedy;
                if(itmp > h)
                  {
                    itmp = h;
                    y0 = y-h;
                  }
                XMoveResizeWindow(EZ_Display, window, x0, y0, w, itmp);
                if(ss==0){EZ_DisplayWidget(widget); ss = 1;}
                XSync(EZ_Display, False);
              }    
          }
        break;
        case EZ_SOUTH:
          {
            itmp = 2;
            EZ_WidgetHeight(widget) = 2;
            while(itmp != h)
              {
                itmp += speedy;
                if(itmp > h) itmp = h;
                XMoveResizeWindow(EZ_Display, window, x0, y0, w, itmp);
                if(ss==0){EZ_DisplayWidget(widget); ss = 1;}
                XSync(EZ_Display, False);
              }    
          }
        break;
        case EZ_NORTHEAST:
          {
            itmp = 2;
            jtmp = 2;
            EZ_WidgetWidth(widget) = 2;
            EZ_WidgetHeight(widget) = 2;
            while(itmp != h || jtmp != w)
              {
                itmp += speedy;
                y0 -= speedy;
                if(itmp > h)
                  {
                    itmp = h;
                    y0 = y-h;
                  }
                jtmp += speedx;
                if(jtmp > w) jtmp = w;
                XMoveResizeWindow(EZ_Display, window, x0, y0, jtmp, itmp);
                if(ss==0){EZ_DisplayWidget(widget); ss = 1;}
                XSync(EZ_Display, False);
              }    
          }
        break;
        case EZ_NORTHWEST:
          {
            itmp = 2;
            jtmp = 2;
            EZ_WidgetWidth(widget) = 2;
            EZ_WidgetHeight(widget) = 2;
            while(itmp != h || jtmp != w)
              {
                itmp += speedy;
                y0 -= speedy;
                if(itmp > h)
                  {
                    itmp = h;
                    y0 = y-h;
                  }
                jtmp += speedx;
                x0 -= speedx;
                if(jtmp > w)
                  {
                    jtmp = w;
                    x0 = x-w;
                  }
                XMoveResizeWindow(EZ_Display, window, x0, y0, jtmp, itmp);
                if(ss==0){EZ_DisplayWidget(widget); ss = 1;}
                XSync(EZ_Display, False);
              }    
          }
        break;
        case EZ_SOUTHEAST:
          {
            itmp = 2;
            jtmp = 2;
            EZ_WidgetWidth(widget) = 2;
            EZ_WidgetHeight(widget) = 2;
            while(itmp != h || jtmp != w)
              {
                itmp += speedy;
                if(itmp > h) itmp = h;
                jtmp += speedx;
                if(jtmp > w) jtmp = w;
                XMoveResizeWindow(EZ_Display, window, x0, y0, jtmp, itmp);
                if(ss==0){EZ_DisplayWidget(widget); ss = 1;}
                XSync(EZ_Display, False);
              }    
          }
        break;
        case EZ_SOUTHWEST:
          {
            itmp = 2;
            jtmp = 2;
            EZ_WidgetWidth(widget) = 2;
            EZ_WidgetHeight(widget) = 2;
            while(itmp != h || jtmp != w)
              {
                itmp += speedy;
                if(itmp > h) itmp = h;
                jtmp += speedx;
                x0 -= speedx;
                if(jtmp > w)
                  {
                    jtmp = w;
                    x0 = x-w;
                  }
                XMoveResizeWindow(EZ_Display, window, x0, y0, jtmp, itmp);
                if(ss==0){EZ_DisplayWidget(widget); ss = 1;}
                XSync(EZ_Display, False);
              }    
          }
          break;
        default:
          break;
        }

      XMoveResizeWindow(EZ_Display, window, x0, y0, w1,h1);
      XSync(EZ_Display, False);

      if(parent == NULL) { sx = x0; sy = y0;}
      EZ_WidgetOriginX(widget) =sx;
      EZ_WidgetOriginY(widget) =sy;
      EZ_WidgetWidth(widget) = w1;
      EZ_WidgetHeight(widget) = h1;
      EZ_RaiseWidgetWindow(widget);
      EZ_ClearWidgetForgetGFlag(widget);
    }
}


void EZ_SlideCloseWidget(widget, direction, speedIn)
     EZ_Widget *widget; int direction, speedIn;
{
  if(EZ_WidgetExist(widget) && EZ_WidgetMapped(widget))
    {
      int x, y, sx,sy, x0,y0,itmp,jtmp, w, h, w1, h1;
      int speedx, speedy,ss=0;
      Window window, junkwin;
      EZ_Widget *parent = EZ_WidgetParent(widget);
      EZ_MakeWidgetWindows(widget);
      window = EZ_WidgetWindow(widget);
      w1=w = EZ_WidgetWidth(widget);
      h1=h = EZ_WidgetHeight(widget);
      
      if(parent)
        {
          if( !EZ_WidgetIsViewable(parent)) return;
          sx=x0 = EZ_WidgetOriginX(widget);
          sy=y0 = EZ_WidgetOriginY(widget); 
        }
      else
        {
          XTranslateCoordinates(EZ_Display, window, RootWindow(EZ_Display, EZ_ScreenNum),
                                0,0, &x,&y, &junkwin); 
          sx=x0 = x;   sy=y0 = y; 
        }

      if(speedIn <= 0) speedIn = 1;
      if(w > h) /* wider */
        {
          speedx = speedIn;
          speedy = speedIn * h/w;
          if(speedy == 0) speedy = 1;
        }
      else
        {
          speedy = speedIn;
          speedx = speedIn * w/h;
          if(speedx == 0) speedx = 1;
        }

      EZ_SetWidgetForgetGFlag(widget);
      switch(direction)
        {
        case EZ_EAST:
          {
            itmp = w;
            while(itmp != 2)
              {
                itmp -= speedx;
                if(itmp <= 2) itmp = 2;
                XMoveResizeWindow(EZ_Display, window, x0, y0, itmp, h);
                if(ss==0){EZ_DisplayWidget(widget); ss = 1;}
                XSync(EZ_Display, False);
              }
          }
        break;
        case EZ_WEST:
          {
            itmp = w;
            while(itmp != 2)
              {
                itmp -= speedx;
                x0 += speedx;
                if(itmp <= 2) 
                  {
                    itmp = 2;
                    x0 = x+w -2;
                  }
                XMoveResizeWindow(EZ_Display, window, x0, y0, itmp, h);
                if(ss==0){EZ_DisplayWidget(widget); ss = 1;}
                XSync(EZ_Display, False);
              }
          }
        break;
        case EZ_NORTH:
          {
            itmp = h;
            while(itmp != 2)
              {
                itmp -= speedy;
                y0 += speedy;
                if(itmp <= 2) 
                  {
                    itmp = 2;
                    y0 = y+h -2;
                  }
                XMoveResizeWindow(EZ_Display, window, x0, y0, w, itmp);
                if(ss==0){EZ_DisplayWidget(widget); ss = 1;}
                XSync(EZ_Display, False);
              }
          }
        break;
        case EZ_SOUTH:
          {
            itmp = h;
            while(itmp != 2)
              {
                itmp -= speedy;
                if(itmp <= 2)  itmp = 2;
                XMoveResizeWindow(EZ_Display, window, x0, y0, w, itmp);
                if(ss==0){EZ_DisplayWidget(widget); ss = 1;}
                XSync(EZ_Display, False);
              }
          }
        break;
        case EZ_NORTHEAST:
          {
            itmp = h;
            jtmp = w;
            while(itmp != 2 || jtmp != 2)
              {
                itmp -= speedy;
                jtmp -= speedx;
                y0 += speedy;
                if(itmp <= 2) 
                  {
                    itmp = 2;
                    y0 = y+h -2;
                  }
                if(jtmp < 2) jtmp = 2;
                XMoveResizeWindow(EZ_Display, window, x0, y0, jtmp, itmp);
                if(ss==0){EZ_DisplayWidget(widget); ss = 1;}
                XSync(EZ_Display, False);
              }
          }
        break;
        case EZ_NORTHWEST:
          {
            itmp = h;
            jtmp = w;
            while(itmp != 2 || jtmp != 2)
              {
                itmp -= speedy;
                jtmp -= speedx;
                y0 += speedy;
                x0 += speedx;
                if(itmp <= 2) 
                  {
                    itmp = 2;
                    y0 = y+h -2;
                  }
                if(jtmp < 2)
                  {
                    jtmp = 2;
                    x0 = x + w -2;
                  }
                XMoveResizeWindow(EZ_Display, window, x0, y0, jtmp, itmp);
                if(ss==0){EZ_DisplayWidget(widget); ss = 1;}
                XSync(EZ_Display, False);
              }
          }
        break;
        case EZ_SOUTHEAST:
          {
            itmp = h;
            jtmp = w;
            while(itmp != 2 || jtmp != 2)
              {
                itmp -= speedy;
                jtmp -= speedx;
                if(itmp < 2) itmp = 2;
                if(jtmp < 2) jtmp = 2;
                XMoveResizeWindow(EZ_Display, window, x0, y0, jtmp, itmp);
                if(ss==0){EZ_DisplayWidget(widget); ss = 1;}
                XSync(EZ_Display, False);
              }
          }
        break;
        case EZ_SOUTHWEST:
          {
            itmp = h;
            jtmp = w;
            while(itmp != 2 || jtmp != 2)
              {
                itmp -= speedy;
                jtmp -= speedx;
                x0 += speedx;
                if(itmp < 2) itmp = 2;
                if(jtmp < 2){ jtmp = 2; x0 = x + w -2;}
                XMoveResizeWindow(EZ_Display, window, x0, y0, jtmp, itmp);
                if(ss==0){EZ_DisplayWidget(widget); ss = 1;}
                XSync(EZ_Display, False);
              }
          }
        break;
        default:
          break;
        }
      EZ_WidgetOriginX(widget) =sx;
      EZ_WidgetOriginY(widget) =sy;
      EZ_ClearWidgetForgetGFlag(widget);
      EZ_HideWidget(widget);
      EZ_WidgetWidth(widget) = w1;
      EZ_WidgetHeight(widget) = h1;
      XResizeWindow(EZ_Display, window, w1,h1);
    }
}



/*****************************************************************/
#undef _EZ_WIDGET_MISC_C_

