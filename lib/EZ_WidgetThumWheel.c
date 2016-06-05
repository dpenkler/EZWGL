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
 ***              Widget ThumbWheel                             ***
 ***                                                           ***
 *****************************************************************/
#define _EZ_WIDGET_THUM_WHEEL_C_

#include "EZ_Widget.h"

/*********************************************************************
 * 
 *  Functions implemented in this file:
 */
EZ_Widget  *EZ_CreateThumbWheel MY_ANSIARGS((EZ_Widget *parent));
void        EZ_DrawThumbWheel MY_ANSIARGS((EZ_Widget *widget));
void        EZ_ComputeThumbWheelSize MY_ANSIARGS((EZ_Widget *widget, int *w, int *h));
void        EZ_ThumbWheelEventHandle MY_ANSIARGS((EZ_Widget *widget, XEvent *event));

float       EZ_GetThumbWheelValue MY_ANSIARGS((EZ_Widget *widget));
void        EZ_SetThumbWheelValue MY_ANSIARGS((EZ_Widget *widget, float value));
void        EZ_SetThumbWheelRange MY_ANSIARGS((EZ_Widget *widget, float f, float t));
/*********************************************************************
 * 
 *  Local Variables.
 */
static EZ_WidgetHandle EZ_ThumbWheelHandle =
{ 
  EZ_ComputeThumbWheelSize,
  EZ_DrawThumbWheel,
  EZ_FreeUnknownData,
  EZ_ThumbWheelEventHandle,
};

/*********************************************************************/

EZ_Widget  *EZ_CreateThumbWheel(parent)
     EZ_Widget  *parent;     /* parent widget    */
{
  EZ_Widget  *wptr;

  wptr = EZ_CreateNewWidget(parent);
  /*--------------------------------------------------
   * Register the handling functions for Button.
   *  has to be done after  EZ_CreateNewWiget.
   *-------------------------------------------------*/
  EZ_WidgetHandlingFunctions[EZ_WIDGET_THUMB_WHEEL] = &EZ_ThumbWheelHandle;
  EZ_SetWidgetTypeAndNames(wptr, EZ_WIDGET_THUMB_WHEEL);
  EZ_WidgetPadX(wptr) = 0;
  EZ_WidgetPadY(wptr) = 0;
  EZ_WidgetBorderWidth(wptr) = 2;
  EZ_WidgetBorderStyle(wptr) = EZ_BORDER_DOWN;
  EZ_WidgetPadB(wptr) = 2;
  EZ_SetWidgetFocusableFlag(wptr);
  
  EZ_ThumFrom(wptr) = 0.0;
  EZ_ThumTo(wptr) = 100.0;
  EZ_ThumValue(wptr) = 0.0;
  EZ_ThumNBars(wptr) = 32;

  return(wptr);
}
 
/********************************************************************
 *
 *  Figure out the dimension of a normal button
 */
#define ONEPI              (3.141592653589793231)
#define ONEOVERPI2         (0.63661977236758134307)
#define DEGREE2RADIAN(aa)  ((aa) * ONEPI/180.0)

void EZ_ComputeThumbWheelSize(widget, w, h)
    EZ_Widget *widget;
     int             *w, *h;
{
  int orien, cw, ch, ww, hh, bw;

  EZ_SetWidgetSizeComputedFlag(widget);
  EZ_ThumFIncr(widget) = (EZ_ThumTo(widget)-EZ_ThumFrom(widget))*ONEOVERPI2;
  orien = EZ_WidgetOrientation(widget);
  if(orien < EZ_VERTICAL) 
    { ww = 48; hh = 8;}
  else
    { ww = 8; hh = 48;}
  bw =  EZ_WidgetBorderWidth(widget) + EZ_WidgetPadB(widget);
  cw =  bw;
  ch =  bw;
  cw = cw + cw + ww;
  ch = ch + ch + hh;

  *w = cw+2;
  *h = ch+2;
}

/****************************************************************************
 *
 *  Draw a thum wheel
 */

void  EZ_DrawThumbWheel(wptr)
     EZ_Widget *wptr;
{
  int             bw,bw2,w, h, padb, padb2, hori, ppp, ppp2;
  int             width, length, tmp, nbars, tile, tox, toy, i, ioff=0;
  Pixmap          pixmap;
  Window          win;
  GC              gc, bgc, lgc, ngc;
  unsigned long   bgpv;
  float           theta, dtheta, ftmp, offset;
  int             vvv[100];

  win = EZ_WidgetWindow(wptr);
  w   = EZ_WidgetWidth(wptr);
  h   = EZ_WidgetHeight(wptr);
  padb = EZ_WidgetPadB(wptr);
  padb2 = padb + padb;

  bw = EZ_WidgetBorderWidth(wptr);
  bw2 = bw+bw;
  ppp = bw+padb;
  ppp2 = ppp+ppp;
  pixmap = XCreatePixmap(EZ_DisplayForWidgets, win, w, h, EZ_Depth);    
  EZ_FixFocusHighlightBorder(wptr, pixmap, w, h, padb);

  hori = (EZ_WidgetOrientation(wptr) < EZ_VERTICAL);
  tmp = bw2 + padb2;
  if(hori) { width = h -tmp; length = w -tmp;}
  else  { width = w -tmp; length = h -tmp;}
  nbars = EZ_ThumNBars(wptr);  
  if(nbars > 100) nbars = EZ_ThumNBars(wptr) = 100;
  if( (length>>2) < nbars) nbars = (length>>2);
  if( EZ_ThumbWidth(wptr) != width || EZ_ThumLength(wptr) != length)
    {
      EZ_ThumbWidth(wptr) = width;
      EZ_ThumLength(wptr) = length;
      EZ_ThumOffset(wptr) = 0.0;
      EZ_ThumFactor(wptr) = 1.0/(float)length;
    }
  offset = EZ_ThumOffset(wptr);  
  dtheta = ONEPI/(float)nbars;
  while(offset > dtheta) offset = EZ_ThumOffset(wptr) = offset -dtheta;
  while(offset < -dtheta) offset = EZ_ThumOffset(wptr) = offset +dtheta;
  theta =  EZ_ThumOffset(wptr) - 0.5 *ONEPI-dtheta;
  ftmp = 0.5*length;
  i = 0; while(theta < 0.5*ONEPI) 
    {vvv[i] = (int)(ftmp * (1.0+sin(theta))); theta += dtheta; i++;}
  nbars = i;

  tile = EZ_GetBackgroundGC(wptr, &gc, &bgpv,1, 0);
  if(tile)
    {
      ftmp = 0.785*(EZ_ThumValue(wptr) - EZ_ThumFrom(wptr)) * (float)length /
        (EZ_ThumTo(wptr) - EZ_ThumFrom(wptr));
      ioff = (int)(ftmp);
      EZ_GetWidgetTileOrigin(wptr, &tox, &toy);      
      if(hori) tox += ioff; else toy += ioff;
      XSetTSOrigin(EZ_Display, gc, tox, toy);
    }
  XFillRectangle(EZ_Display, pixmap, gc, padb,padb, w-padb2, h-padb2); 
  
  if(hori)
    {
      int x0, y0, lll = width+ppp-2;
      int itmp = nbars>>2;
      if(EZ_GetDarkBrightNormalBDGC(wptr, &bgc, &lgc, &ngc))
        {
          XSetTSOrigin(EZ_Display, lgc, tox, toy);
          XSetTSOrigin(EZ_Display, bgc, tox, toy);
        }
      ngc = gc;
      XDrawLine(EZ_Display, pixmap, lgc, ppp,ppp,length+ppp,ppp);          
      XDrawLine(EZ_Display, pixmap, bgc, ppp,ppp+width-1,length+ppp,ppp+width-1);          
      for(y0=ppp+1, i = 0; i < nbars; i++)
        {
          x0 = vvv[i] +ppp;
          if(x0 >= ppp-2 && x0 <= length+ppp+2) 
            {
              if(i>itmp && i < nbars-itmp)
                XDrawLine(EZ_Display, pixmap, lgc, x0-1,y0,x0-1,lll);          
              XDrawLine(EZ_Display, pixmap, ngc, x0,y0,x0,lll);          
              XDrawLine(EZ_Display, pixmap, bgc, x0+1,y0,x0+1,lll);          
            }
        }
      XFillRectangle(EZ_Display, pixmap,bgc,ppp,ppp,2,lll);
      XFillRectangle(EZ_Display, pixmap,bgc,ppp+length-2,ppp,2,lll);
      EZ_DrawRectBorder(wptr, pixmap);
      XDrawRectangle(EZ_Display, pixmap, bgc, ppp-1, ppp-1, length+1, width+1);
    }
  else
    {
      int x0, y0, lll = width+ppp-2;
      int itmp = nbars>>2;
      if(EZ_GetDarkBrightNormalBDGC(wptr, &bgc, &lgc, &ngc))
        {
          XSetTSOrigin(EZ_Display, lgc, tox, toy);
          XSetTSOrigin(EZ_Display, bgc, tox, toy);
        }
      ngc = gc;
      XDrawLine(EZ_Display, pixmap, lgc, ppp,ppp,ppp, length+ppp);          
      XDrawLine(EZ_Display, pixmap, bgc, ppp+width-1,ppp,ppp+width-1, length+ppp);          
      for(x0=ppp+1, i = 0; i < nbars; i++)
        {
          y0 = vvv[i] +ppp;
          if(y0 >= ppp-2 && y0 <= length+ppp+2) 
            {
              if(i>itmp && i < nbars-itmp)
                XDrawLine(EZ_Display, pixmap, lgc, x0,y0-1,lll,y0-1);          
              XDrawLine(EZ_Display, pixmap, ngc, x0,y0,lll,y0);          
              XDrawLine(EZ_Display, pixmap, bgc, x0,y0+1,lll,y0+1);          
            }
        }
      XFillRectangle(EZ_Display, pixmap,bgc, ppp,ppp,lll,2);
      XFillRectangle(EZ_Display, pixmap,bgc, ppp,ppp+length-2,lll,2);
      EZ_DrawRectBorder(wptr, pixmap);
      XDrawRectangle(EZ_Display, pixmap, bgc, ppp-1, ppp-1, width+1, length+1);
    }
  XCopyArea(EZ_Display,pixmap,win, EZ_WRITABLEGC,0,0,w,h,0,0);  
  XFreePixmap(EZ_Display, pixmap);                             
}

/*************************************************************************
 *
 *  Event handling
 */

void  EZ_ThumbWheelEventHandle(widget, event)
     EZ_Widget *widget;
     XEvent          *event;
{
  XEvent xevent;
  float nvalue, value, from, to;
  if(event->type == Expose)   EZ_DrawWidget(widget);      
  if( EZ_GetWidgetDisabledFlag(widget) )  return;

  value = EZ_ThumValue(widget);
  from = EZ_ThumFrom(widget);
  to = EZ_ThumTo(widget);
  if(to < from) { nvalue = from; from = to; to = nvalue;}

  switch(event->type)
    {
    case ButtonPress:
      if(event->xbutton.button == EZ_Btn1)
	{
          int ppp = EZ_WidgetBorderWidth(widget) + EZ_WidgetPadB(widget)+3;
          int vert = EZ_WidgetOrientation(widget) >= EZ_VERTICAL;
          int x = event->xbutton.x;
          int y = event->xbutton.y;
          if(vert)
            {
              int h = EZ_WidgetHeight(widget);
              if(y < ppp || y > h-ppp)
                {
                  int done=0, first_time = 1;
                  float acel;
                  while(!done)
                    {
                      if(first_time)
                        {
                          first_time = 0;
                          XFlush(EZ_Display);
                          EZ_SitForALittleBit(100000); 
                        }
                      else EZ_SitForALittleBit(50000); 
                      if(y<ppp) acel = -0.04; else acel = 0.04;
                      nvalue = value + acel * EZ_ThumFIncr(widget);
                      if(nvalue < from) nvalue = from;
                      else if(nvalue >to) nvalue = to;
                      if(nvalue != value)
                        {
                          EZ_ThumOffset(widget) += acel;
                          EZ_ThumValue(widget) = value = nvalue;
                          EZ_DrawWidget(widget);
                          { EZ_ExecuteWidgetCallBacks(widget);}
                          while(XPending(EZ_Display))
                            {
                              XNextEvent(EZ_Display, &xevent);
                              if(EZ_FilterEvent(&xevent)) EZ_InvokePrivateEventHandler(&xevent);
                              if(xevent.type == ButtonRelease && xevent.xbutton.button == EZ_Btn1) 
                                {
                                  done = 1;
                                  break;
                                }
                            } 
                        }
                      else done = 1;
                    }
                }
              else
                {
		  int             rx,ry, sx, sy;
		  unsigned int    mask;
		  Window          root,win;
		  XQueryPointer(EZ_Display, RootWindow(EZ_Display, EZ_ScreenNum),
				&root,
				&win, 
				&sx, &sy, 
				&x,&y, 
				&mask );
                  while(1)
                    {
                      XNextEvent(EZ_Display, &xevent);
                      if(EZ_FilterEvent(&xevent))
                        EZ_InvokePrivateEventHandler(&xevent);
                      if(xevent.type == Expose || xevent.type == FocusIn || xevent.type == FocusOut) 
                        EZ_WidgetDispatchEvent(&xevent);
                      while(XPending(EZ_Display))
                        {
                          if(xevent.type == ButtonRelease && xevent.xbutton.button == EZ_Btn1) break;
                          XNextEvent(EZ_Display, &xevent);
                          if(EZ_FilterEvent(&xevent))
                            EZ_InvokePrivateEventHandler(&xevent);
                          if(xevent.type == Expose || xevent.type == FocusIn || xevent.type == FocusOut) 
                            EZ_WidgetDispatchEvent(&xevent);
                        } 
                      if(xevent.type == ButtonRelease && xevent.xbutton.button == EZ_Btn1)
                        {
                          { EZ_ExecuteWidgetCallBacks(widget);}
                          break;
                        }
                      /* not over yet */
                      XQueryPointer(EZ_Display, RootWindow(EZ_Display, EZ_ScreenNum),
                                    &root, &win,  &rx, &ry, &x,&y, 
                                    &mask );
                      if(sy != ry)
                        {
                          float  ftmp = EZ_ThumFactor(widget) * (float)(ry-sy);
                          nvalue = value + ftmp * EZ_ThumFIncr(widget);
                          if(nvalue < from) nvalue = from; else if(nvalue > to) nvalue = to;
                          sy = ry;
                          if(nvalue != value)
                            {
                              EZ_CallBackStruct *callbacks = EZ_WidgetMotionCallBack(widget);
                              EZ_ThumOffset(widget) += ftmp;
                              EZ_ThumValue(widget) = value = nvalue;
                              EZ_DrawWidget(widget);
                              if(callbacks) {EZ_HandleMotionCallBacks(widget,callbacks);}
                              else { EZ_ExecuteWidgetCallBacks(widget);}
                            }
                        }                  
                    }
                }
            }
          else
            {
              int w = EZ_WidgetWidth(widget);
              if(x < ppp || x > w-ppp)
                {
                  int done=0, first_time = 1;
                  float acel;
                  while(!done)
                    {
                      if(first_time)
                        {
                          first_time = 0;
                          XFlush(EZ_Display);
                          EZ_SitForALittleBit(100000); 
                        }
                      else EZ_SitForALittleBit(50000); 
                      if(x<ppp) acel = -0.04; else acel = 0.04;
                      nvalue = value + acel * EZ_ThumFIncr(widget);
                      if(nvalue < from) nvalue = from;
                      else if(nvalue >to) nvalue = to;
                      if(nvalue != value)
                        {
                          EZ_ThumOffset(widget) += acel;
                          EZ_ThumValue(widget) = value = nvalue;
                          EZ_DrawWidget(widget);
                          { EZ_ExecuteWidgetCallBacks(widget);}
                          while(XPending(EZ_Display))
                            {
                              XNextEvent(EZ_Display, &xevent);
                              if(EZ_FilterEvent(&xevent)) EZ_InvokePrivateEventHandler(&xevent);
                              if(xevent.type == ButtonRelease && xevent.xbutton.button == EZ_Btn1) 
                                {
                                  done = 1;
                                  break;
                                }
                            }                  
                        }
                      else done = 1;
                    }
                }
              else
                {
		  int             rx,ry, sx, sy;
		  unsigned int    mask;
		  Window          root,win;
		  XQueryPointer(EZ_Display, RootWindow(EZ_Display, EZ_ScreenNum),
				&root,
				&win, 
				&sx, &sy, 
				&x,&y, 
				&mask );
                  while(1)
                    {
                      XNextEvent(EZ_Display, &xevent);
                      if(EZ_FilterEvent(&xevent))
                        EZ_InvokePrivateEventHandler(&xevent);
                      if(xevent.type == Expose || xevent.type == FocusIn || xevent.type == FocusOut) 
                        EZ_WidgetDispatchEvent(&xevent);
                      while(XPending(EZ_Display))
                        {
                          if(xevent.type == ButtonRelease && xevent.xbutton.button == EZ_Btn1) break;
                          XNextEvent(EZ_Display, &xevent);
                          if(EZ_FilterEvent(&xevent))
                            EZ_InvokePrivateEventHandler(&xevent);
                          if(xevent.type == Expose || xevent.type == FocusIn || xevent.type == FocusOut) 
                            EZ_WidgetDispatchEvent(&xevent);
                        } 
                      if(xevent.type == ButtonRelease && xevent.xbutton.button == EZ_Btn1)
                        {
                          { EZ_ExecuteWidgetCallBacks(widget);}
                          break;
                        }
                      /* not over yet */
                      XQueryPointer(EZ_Display, RootWindow(EZ_Display, EZ_ScreenNum),
                                    &root, &win,  &rx, &ry, &x,&y, 
                                    &mask );

                      if(sx != rx)
                        {
                          float  ftmp = EZ_ThumFactor(widget) * (float)(rx-sx);
                          nvalue = value + ftmp * EZ_ThumFIncr(widget);
                          if(nvalue < from) nvalue = from; else if(nvalue > to) nvalue = to;
                          sx = rx;
                          if(nvalue != value)
                            {
                              EZ_CallBackStruct *callbacks = EZ_WidgetMotionCallBack(widget);
                              EZ_ThumOffset(widget) += ftmp;
                              EZ_ThumValue(widget) = value = nvalue;
                              EZ_DrawWidget(widget);
                              if(callbacks) {EZ_HandleMotionCallBacks(widget,callbacks);}
                              else { EZ_ExecuteWidgetCallBacks(widget);}
                            }             
                        }     
                    }
                }
            }
	}
      else if(event->xbutton.button == EZ_Btn3)
	{
          EZ_HandleDnDEvents(widget, event);
	}
      break;
    case ButtonRelease:
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
      break;
    case KeyPress:
      {
	char              tmpbuffer[8];
	int               vert, buffersize = 8;
	KeySym            keysym;
	XComposeStatus    compose; 
	int count = XLookupString(&(event->xkey), tmpbuffer, buffersize, &keysym, &compose);
        float acel;
	tmpbuffer[count] = '\0';

	switch(keysym)
	  {
	  case XK_Up: case XK_k:  case XK_p: case XK_K:  case XK_P: 
	  case XK_Right: case XK_f: case XK_l: case XK_F: case XK_L:  
#ifdef XK_KP_Right
	  case XK_KP_Right:
#endif
#ifdef XK_KP_Up
	  case XK_KP_Up:
#endif
	    if(event->xkey.state & ShiftMask) acel = 0.05;
	    else if(event->xkey.state & Mod1Mask)  acel = 0.1;
            else acel = 0.03; 
            vert = (EZ_WidgetOrientation(widget) >= EZ_VERTICAL);
            if(vert) acel = -acel;
            nvalue = value + acel * EZ_ThumFIncr(widget);
            if(nvalue < from) nvalue = from; else if(nvalue > to) nvalue = to;
            if(nvalue != value)
              {
                EZ_ThumOffset(widget) += acel;
                EZ_ThumValue(widget) = value = nvalue;
                EZ_DrawWidget(widget);
                {EZ_ExecuteWidgetCallBacks(widget);}
              }
	    break;
	  case XK_Down: case XK_n: case XK_j: case XK_N: case XK_J:  
	  case XK_Left: case XK_b:  case XK_h: case XK_B:  case XK_H:  
#ifdef XK_KP_Left
	  case XK_KP_Left:
#endif
#ifdef XK_KP_Down
	  case XK_KP_Down:
#endif
	    if(event->xkey.state & ShiftMask) acel = -0.05;
	    else if(event->xkey.state & Mod1Mask)  acel = -0.1;
            else acel = -0.03; 
            vert = (EZ_WidgetOrientation(widget) >= EZ_VERTICAL);
            if(vert) acel = -acel;
            nvalue = value + acel * EZ_ThumFIncr(widget);
            if(nvalue < from) nvalue = from; else if(nvalue > to) nvalue = to;
            if(nvalue != value)
              {
                EZ_ThumOffset(widget) += acel;
                EZ_ThumValue(widget) = value = nvalue;
                EZ_DrawWidget(widget);
                {EZ_ExecuteWidgetCallBacks(widget);}
              }
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
/********************************************************************************/
float EZ_GetThumbWheelValue(widget) EZ_Widget *widget;
{
  if(widget && EZ_WidgetType(widget) == EZ_WIDGET_THUMB_WHEEL)
    {
      return(EZ_ThumValue(widget));
    }
  return(0.0);
}
/********************************************************************************/
void EZ_SetThumbWheelValue(widget, value) EZ_Widget *widget; float value;
{
  if(widget && EZ_WidgetType(widget) == EZ_WIDGET_THUMB_WHEEL)
    {
      EZ_ThumValue(widget) = value;
      if(EZ_WidgetMapped(widget)) EZ_DrawWidget(widget);
    }
}
void EZ_SetThumbWheelRange(widget, f, t)
     EZ_Widget *widget; float f, t;
{
  if(widget && EZ_WidgetType(widget) == EZ_WIDGET_THUMB_WHEEL)
    {
      EZ_ThumFrom(widget) = f; EZ_ThumTo(widget) = t;
      EZ_ThumFIncr(widget) = (t-f)*ONEOVERPI2;
      if(EZ_WidgetMapped(widget)) EZ_DrawWidget(widget);
    }
}
/********************************************************************************/
#undef _EZ_WIDGET_THUM_WHEEL_C_

