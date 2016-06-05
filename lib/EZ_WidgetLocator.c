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
 ***              Locator                                      ***
 ***                                                           ***
 *****************************************************************/
#define _EZ_WIDGET_LOCATOR_C_

#include "EZ_Widget.h"

/*********************************************************************
 * 
 *  Functions implemented in this file:
 */

/*********************************************************************/
EZ_Widget *EZ_CreateLocator   MY_ANSIARGS((EZ_Widget *parent));
void      EZ_GetLocatorXY    MY_ANSIARGS((EZ_Widget *locator, float *x, float *y));
void      EZ_SetLocatorXY    MY_ANSIARGS((EZ_Widget *locator, float x, float y));

/*********************************************************************
 * 
 *  Local Variables.
 */
static void EZ_DrawLocator MY_ANSIARGS((EZ_Widget *widget));
static void EZ_LocatorEventHandle MY_ANSIARGS((EZ_Widget *widget, XEvent *event));
static void EZ_ComputeLocatorSize MY_ANSIARGS((EZ_Widget *widget, int *w, int *h));

static EZ_WidgetHandle EZ_LocatorHandle =
{ 
  EZ_ComputeLocatorSize,
  EZ_DrawLocator,
  EZ_FreeUnknownData,
  EZ_LocatorEventHandle,
};
/*********************************************************************/

EZ_Widget  *EZ_CreateLocator(parent)
     EZ_Widget  *parent;     /* parent widget  */
{
  EZ_Widget  *frame;

  frame = EZ_CreateNewWidget(parent);
  EZ_WidgetHandlingFunctions[EZ_WIDGET_LOCATOR] = &EZ_LocatorHandle;
  EZ_SetWidgetTypeAndNames(frame,EZ_WIDGET_LOCATOR);
  EZ_WidgetPadX(frame) = 0;
  EZ_WidgetPadY(frame) = 0;
  EZ_WidgetSepX(frame) = 0;
  EZ_WidgetSepY(frame) = 0;
  EZ_LocatorXX(frame) = 0.5;
  EZ_LocatorYY(frame) = 0.5;
  EZ_WidgetBorderWidth(frame) = 2;
  EZ_WidgetBorderStyle(frame) = EZ_BORDER_DOWN;

  return(frame);
}
/*******************************************************************/
static void EZ_ComputeLocatorSize(widget, w, h)
     EZ_Widget *widget; int *w, *h;
{
  int cw, ch;
  int bw = EZ_WidgetBorderWidth(widget) + EZ_WidgetPadB(widget);
  if(!(EZ_GetWidgetSizeComputedFlag(widget)))
    {
      EZ_SetWidgetSizeComputedFlag(widget);
    }
  cw = bw + EZ_WidgetPadX(widget);
  ch = bw + EZ_WidgetPadY(widget);
  *w = cw + cw + 64; 
  *h = cw + cw + 64; 
}
/*******************************************************************/
static void EZ_DrawLocator(widget) EZ_Widget *widget;
{
  int focus_pad = 0;   /* kbd focus highlight padding */
  int bwidth    = 0;   /* border width */
  int padx, pady;      /* XY padding, not used */
  int width, height;   /* widget window width and height */
  Window   win;        /* widget window */
  int      tile;       /* is bg tiled ? */
  int      x, y, tmp, tmp2;
  Pixmap   pixmap;
  GC       gc;
  unsigned long bgpv;
  float fx, fy;
  
  padx = EZ_WidgetPadX(widget);
  pady = EZ_WidgetPadY(widget);
  focus_pad = EZ_WidgetPadB(widget);
  bwidth = EZ_WidgetBorderWidth(widget);
  width = EZ_WidgetWidth(widget);
  height = EZ_WidgetHeight(widget);
  win = EZ_WidgetWindow(widget);

  tmp = focus_pad + bwidth;        
  tmp2 = tmp + tmp;

  /* draw into a tmp pixmap, copy to the widget window when done */
  pixmap = XCreatePixmap(EZ_Display, win, width, height, EZ_Depth); 

  /* fix kbd focus highlight border */
  EZ_FixFocusHighlightBorder(widget, pixmap, width, height, focus_pad);

  /* fill the background */
  tile = EZ_GetBackgroundGC(widget, &gc,&bgpv,1,0);  
  if(tile)
    {
      int tileOx=0, tileOy=0;
      EZ_GetWidgetTileOrigin(widget, &tileOx, &tileOy);      
      XSetTSOrigin(EZ_Display, gc, tileOx, tileOy);
    }
  XFillRectangle(EZ_Display, pixmap, gc, 0, 0, width, height);
  
  /* render the foreground, the crosshair */
  if(EZ_WidgetForeground(widget) != EZ_DefaultForeground)
    {
      XSetForeground(EZ_Display, EZ_WRITABLEGC, EZ_WidgetForeground(widget));
      gc =  EZ_WRITABLEGC;
    }
  else gc = EZ_BUTTON_TEXTGC;
  
  fx = EZ_LocatorXX(widget);
  fy = EZ_LocatorYY(widget);
  x = tmp + (int)((width - tmp2) * fx + 0.5);
  y = tmp + (int)((height- tmp2) * fy + 0.5);
  
  XDrawLine(EZ_Display, pixmap, gc, x, tmp, x, height -tmp); 
  XDrawLine(EZ_Display, pixmap, gc, tmp, y, width - tmp,y);

  /* render the widget border */
  EZ_DrawRectBorder(widget, pixmap);

  /* and finally  ... */
  XCopyArea(EZ_Display,pixmap,win, gc,0,0,width,height,0,0); 
  XFreePixmap(EZ_Display, pixmap); 
}
/*******************************************************************/
static void EZ_LocatorEventHandle(EZ_Widget *widget, XEvent *event)
{
  if(event->type == Expose) EZ_DrawLocator(widget);
  else if(event->type == ButtonPress || event->type == KeyPress)
    {
      int padx, pady, focus_pad=0;                /* paddings  */
      int bwidth = 0, bwpad, bwpad2;  /* border .. */
      int x,y, width, height;                     /* dimension */
      XEvent xevent;
      float save_x, save_y, fx, fy;

      padx = EZ_WidgetPadX(widget);
      pady = EZ_WidgetPadY(widget);
      focus_pad = EZ_WidgetPadB(widget);
      bwidth = EZ_WidgetBorderWidth(widget);
      width = EZ_WidgetWidth(widget);
      height = EZ_WidgetHeight(widget);
      bwpad = focus_pad + bwidth;
      bwpad2 = bwpad + bwpad;

      if(event->type == ButtonPress)
        {
          if(event->xbutton.button == EZ_Btn1)
            {
              x = event->xbutton.x;
              y = event->xbutton.y;
              save_x = EZ_LocatorXX(widget);
              save_y = EZ_LocatorYY(widget);
              while(1)
                {
                  /* loop and wait for ButtonRelease. Invoke
                   * private event handlers on other events. Handle
                   * MotionNotify events and execute MotionCallbacks
                   * if needed.
                   */
                  if(x >= 0)
                    {
                      fx  = (float)(x-bwpad)/(float)(width  - bwpad2);
                      fy = (float)(y-bwpad)/(float)(height - bwpad2);
                      if(fx < 0.0) fx = 0.0; else if(fx > 1.0) fx=1.0;
                      if(fy < 0.0) fy = 0.0; else if(fy > 1.0) fy=1.0;
                      EZ_LocatorXX(widget) = fx;
                      EZ_LocatorYY(widget) = fy;
                      EZ_DrawLocator(widget);
                      /* if locator has changed it state, invoke motion callbacks */
                      if(save_x != fx || save_y != fy)
                        {EZ_HandleMotionCallBacks(widget,EZ_WidgetMotionCallBack(widget));}
                    }
                  XNextEvent(EZ_Display, &xevent);
              
                  if(xevent.type == Expose || xevent.type == FocusIn || xevent.type == FocusOut) 
                    EZ_WidgetDispatchEvent(&xevent);
                  else if(xevent.type == MotionNotify)
                    {
                      x = xevent.xmotion.x;
                      y = xevent.xmotion.y;
                      while(XCheckTypedEvent(EZ_Display, MotionNotify, &xevent));
                    }
                  else if(xevent.type == ButtonRelease && xevent.xbutton.button == Button1)
                    break;
                  else x = -1111;
                }
            } 
          else if(event->xbutton.button == EZ_Btn3) 
            {
              EZ_HandleDnDEvents(widget, event);
            }
        }
      else /* key press */
        {
          char              tmpbuffer[8];
          int               buffersize = 8, xmove_unit, ymove_unit, count;
          KeySym            keysym;
          XComposeStatus    compose; 

          count = XLookupString(&(event->xkey), tmpbuffer, buffersize, &keysym, &compose);
          tmpbuffer[count] = '\0';
          xmove_unit = 0;
          ymove_unit = 0;

          switch(keysym)
            {
            case XK_Left: case XK_b:  case XK_h: case XK_B:  case XK_H:  
#ifdef XK_KP_Left
            case XK_KP_Left:
#endif
              if(event->xkey.state & ShiftMask)        xmove_unit = -4;
              else if(event->xkey.state & Mod1Mask)    xmove_unit = -10 ;
              else if(event->xkey.state & ControlMask) xmove_unit = -20;
              else xmove_unit = -1;
              break;
            case XK_Right: case XK_f: case XK_l: case XK_F: case XK_L:  
#ifdef XK_KP_Right
            case XK_KP_Right:
#endif
              if(event->xkey.state & ShiftMask)         xmove_unit = 4;
              else if(event->xkey.state & Mod1Mask)     xmove_unit = 10 ;
              else if(event->xkey.state & ControlMask)  xmove_unit = 20;
              else xmove_unit = 1;
              break;
            case XK_Up: case XK_k:  case XK_p: case XK_K:  case XK_P: 
#ifdef XK_KP_Up
            case XK_KP_Up:
#endif
              if(event->xkey.state & ShiftMask)         ymove_unit = -4;
              else if(event->xkey.state & Mod1Mask)     ymove_unit = -10 ;
              else if(event->xkey.state & ControlMask)  ymove_unit = -20;
              else ymove_unit = -1;
              break;
              break;
            case XK_Down: case XK_n: case XK_j: case XK_N: case XK_J:  
#ifdef XK_KP_Down
            case XK_KP_Down:
#endif
              if(event->xkey.state & ShiftMask)         ymove_unit = 4;
              else if(event->xkey.state & Mod1Mask)     ymove_unit = 10 ;
              else if(event->xkey.state & ControlMask)  ymove_unit = 20;
              else ymove_unit = 1;
              break;
            default:
              break;
            }
              
          if(xmove_unit | ymove_unit)
            {
              fx = EZ_LocatorXX(widget) + (xmove_unit) * 0.01;
              fy = EZ_LocatorYY(widget) + (ymove_unit) * 0.01;
              if(fx < 0.0) fx = 0.0; else if(fx > 1.0) fx=1.0;
              if(fy < 0.0) fy = 0.0; else if(fy > 1.0) fy=1.0;
              EZ_LocatorXX(widget) = fx;
              EZ_LocatorYY(widget) = fy;
              EZ_DrawLocator(widget);
              {EZ_HandleMotionCallBacks(widget,EZ_WidgetMotionCallBack(widget));}
            }
          /* normally there are too many KeyPress events ... */
          while(XCheckTypedEvent(EZ_Display, KeyPress, &xevent));
        }
    }
}
/*******************************************************************/
void EZ_GetLocatorXY(widget,x,y)
     EZ_Widget *widget; float *x; float *y;
{
  if(widget && EZ_WidgetType(widget) == EZ_WIDGET_LOCATOR)
    {
      *x = EZ_LocatorXX(widget); 
      *y = EZ_LocatorYY(widget); 
    }
}
/**********************************************************************/

void EZ_SetLocatorXY(widget, x, y)
     EZ_Widget *widget; float x, y;
{
  if(widget && EZ_WidgetType(widget) == EZ_WIDGET_LOCATOR)
    {
      if(x < 0.0) x = 0.0; else if(x > 1.0) x = 1.0;
      if(y < 0.0) y = 0.0; else if(y > 1.0) y = 1.0;
      if(EZ_LocatorXX(widget) != x ||  EZ_LocatorYY(widget) != y )
        {
          EZ_LocatorXX(widget) = x;
          EZ_LocatorYY(widget) = y;
          if(EZ_WidgetMapped(widget)) EZ_DrawWidget(widget);
          {EZ_HandleMotionCallBacks(widget,EZ_WidgetMotionCallBack(widget));}
        }
    }
}
/*******************************************************************/
#undef _EZ_WIDGET_LOCATOR_C_
