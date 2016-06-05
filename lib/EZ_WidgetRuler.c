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
 ***              Widget H and V Ruler                         ***
 ***                                                           ***
 *****************************************************************/
#define _EZ_WIDGET_RULER_C_

#include "EZ_Widget.h"

/*********************************************************************
 * 
 *  Functions implemented in this file:
 */
EZ_Widget   *EZ_CreateRuler  MY_ANSIARGS((EZ_Widget *parent, int type));
void         EZ_WidgetNotifyRuler  MY_ANSIARGS((EZ_Widget *ruler, int x, int y, 
                                                EZ_Widget *fromWgt));
void         EZ_XWindowNotifyRuler  MY_ANSIARGS((EZ_Widget *ruler, int x, int y, 
                                                 Window fromWin));
void         EZ_WidgetMarkRuler  MY_ANSIARGS((EZ_Widget *ruler, int x, int y, 
                                                EZ_Widget *fromWgt));
void         EZ_XWindowMarkRuler  MY_ANSIARGS((EZ_Widget *ruler, int x, int y, 
                                               Window fromWin));

void         EZ_SetRulerOffset MY_ANSIARGS((EZ_Widget *ruler, int offset));
void         EZ_SetupRuler MY_ANSIARGS((EZ_Widget *ruler, int tunit, int tlength,
                                        int nscale, int offset));
int          *EZ_GetRulerMarks  MY_ANSIARGS((EZ_Widget *ruler));

void         EZ_ComputeWidgetHRulerSize  MY_ANSIARGS((EZ_Widget *widget, int *w, int *h));
void         EZ_ComputeWidgetVRulerSize  MY_ANSIARGS((EZ_Widget *widget, int *w, int *h));
void         EZ_DrawWidgetHorizontalRuler  MY_ANSIARGS((EZ_Widget *widget));
void         EZ_DrawWidgetVerticalRuler MY_ANSIARGS((EZ_Widget *widget));
void         EZ_FreeWidgetRulerData  MY_ANSIARGS((EZ_Widget *widget));
void         EZ_RulerEventHandle  MY_ANSIARGS((EZ_Widget *widget, XEvent *event));
/*********************************************************************
 * 
 *  Local functions:
 */
static void  showHRulerMarker  MY_ANSIARGS((EZ_Widget *widget));
static void  showVRulerMarker  MY_ANSIARGS((EZ_Widget *widget));
static void  timerCb MY_ANSIARGS((EZ_Timer *timer, void *pdata));
static void  checkRulerUnit    MY_ANSIARGS((EZ_Widget *widget));
/*********************************************************************
 * 
 *  Local Variables.
 */
static EZ_WidgetHandle EZ_HRulerHandle =
{
  EZ_ComputeWidgetHRulerSize,
  EZ_DrawWidgetHorizontalRuler,
  EZ_FreeWidgetRulerData,
  EZ_RulerEventHandle,
};

static EZ_WidgetHandle EZ_VRulerHandle =
{
  EZ_ComputeWidgetVRulerSize,
  EZ_DrawWidgetVerticalRuler,
  EZ_FreeWidgetRulerData,
  EZ_RulerEventHandle,
};

/*********************************************************************/

EZ_Widget  *EZ_CreateRuler(parent, type)
     EZ_Widget  *parent;     /* parent widget    */
     int        type;
{
  EZ_Widget  *wptr;

  wptr = EZ_CreateNewWidget(parent);
  if(type == EZ_WIDGET_HORIZONTAL_RULER) 
    {
      EZ_WidgetHandlingFunctions[EZ_WIDGET_HORIZONTAL_RULER] = &EZ_HRulerHandle;
      EZ_SetWidgetHeightSetFlag(wptr); 
      EZ_RulerOrientation(wptr) = EZ_TOP;
    }
  else
    {
      EZ_WidgetHandlingFunctions[EZ_WIDGET_VERTICAL_RULER] = &EZ_VRulerHandle;
      EZ_SetWidgetWidthSetFlag(wptr);
      EZ_RulerOrientation(wptr) = EZ_LEFT;
    }
  EZ_SetWidgetTypeAndNames(wptr, type);
  
  EZ_WidgetFont(wptr) = EZ_GetFontFromId(EZ_TINY_FONT);
  EZ_WidgetBorderStyle(wptr)  = EZ_BORDER_UP;
  EZ_WidgetBorderWidth(wptr)  = 2;
  EZ_WidgetPadB(wptr) = 0;
  EZ_WidgetPadX(wptr) = 0;
  EZ_WidgetPadY(wptr) = 0;

  EZ_RulerBigTick(wptr) = 10;
  EZ_RulerTickError(wptr) = 0;
  EZ_RulerTickScale(wptr) = 10;
  EZ_RulerTickSize(wptr) = 5;
  EZ_RulerTickLength(wptr) = 16;
  {
    int *pos= EZ_RulerPosition(wptr);
    pos[0] = pos[1] = pos[2] = -1;
    pos[6] = pos[7] = -1;
  }
  EZ_RulerDisplayValue(wptr) = 1;
  EZ_RulerTimer(wptr) = EZ_CreateTimer(0, 10000, -1, 
                                       (EZ_CallBack)timerCb, 
                                       (void *)wptr, 0);
  return(wptr);
}

void EZ_FreeWidgetRulerData(widget)
     EZ_Widget *widget;
{EZ_CancelTimer(EZ_RulerTimer(widget)); EZ_RulerTimer(widget)=NULL;}

/********************************************************************
 *
 *  Figure out the dimension of a horizontal Ruler
 */
void EZ_ComputeWidgetHRulerSize(widget, w, h)
    EZ_Widget *widget;
     int             *w, *h;
{
  int  height, cw, ch, tlength, htkl;

  if(!(EZ_GetWidgetSizeComputedFlag(widget)))
    {
      EZ_RulerFontHeight(widget) = EZ_WidgetFont(widget)->ascent +
        EZ_WidgetFont(widget)->descent;
      EZ_RulerFontAscent(widget) = EZ_WidgetFont(widget)->ascent;
      EZ_RulerFontWidth(widget) = EZ_WidgetFont(widget)->max_bounds.width;
      EZ_SetWidgetSizeComputedFlag(widget);
    }
  height = EZ_RulerFontHeight(widget);
  cw = EZ_WidgetPadX(widget) + EZ_WidgetBorderWidth(widget) + EZ_WidgetPadB(widget);
  ch = EZ_WidgetPadY(widget) + EZ_WidgetBorderWidth(widget) + EZ_WidgetPadB(widget);

  /*-------------------------------
   *  minimal size of the X window
   *------------------------------*/
  tlength = EZ_RulerTickLength(widget);
  htkl = tlength >> 1;
  height += htkl + 4;
  if(height < tlength) height = tlength;
  ch = ch + ch + height;
  cw = cw + cw + 8;
  *w = cw;
  *h = ch;
  checkRulerUnit(widget);
}	  
/**************************************************************************/

void  EZ_DrawWidgetHorizontalRuler(wptr)
     EZ_Widget *wptr;
{
  int             bw, w, h, cw, ch, padb, padb2, tile, orientation;
  int             tick, htk, qtk, tlength, tox = 0, toy = 0, itmp, cnt;
  int             toffset, tscale, bigtick;
  float           tickerror, err;
  Pixmap          pixmap;
  Window          win;
  GC              gc;

  unsigned long   bgpv;

  win = EZ_WidgetWindow(wptr);
  w   = EZ_WidgetWidth(wptr);
  h   = EZ_WidgetHeight(wptr);
  bw = EZ_WidgetBorderWidth(wptr);
  padb = EZ_WidgetPadB(wptr);
  padb2 = padb + padb;

  /*-----------------------------------------------------------
   *  Create a pixmap, draw into this pixmap in background and
   *  copy to the button window when finished.
   *----------------------------------------------------------*/
  pixmap = XCreatePixmap(EZ_DisplayForWidgets, win, w, h, EZ_DepthForWidgets);    
  EZ_FixFocusHighlightBorder(wptr, pixmap, w, h, padb);

  tile = EZ_GetBackgroundGC(wptr, &gc, &bgpv,0, 0);
  if(tile)
    {
      EZ_GetWidgetTileOrigin(wptr, &tox, &toy);      
      XSetTSOrigin(EZ_Display, gc, tox, toy);
    }
  XFillRectangle(EZ_Display, pixmap, gc, padb,padb, w-padb2, h-padb2); 

  cw = EZ_WidgetBorderWidth(wptr) + EZ_WidgetPadX(wptr) + padb;
  ch = EZ_WidgetBorderWidth(wptr) + EZ_WidgetPadY(wptr) + padb;

  gc = EZ_WRITABLEGC;
  XSetFont(EZ_DisplayForWidgets, gc, EZ_WidgetFont(wptr)->fid);
  XSetForeground(EZ_DisplayForWidgets, gc, EZ_WidgetForeground(wptr));	
  /*--------------------------------------------------------
   *  Draw ticks
   *-------------------------------------------------------*/
  orientation = EZ_RulerOrientation(wptr);
  toffset = EZ_RulerTOffset(wptr);
  tick = EZ_RulerTickSize(wptr);
  tlength = EZ_RulerTickLength(wptr);
  bigtick = EZ_RulerBigTick(wptr);
  tickerror = EZ_RulerTickError(wptr);
  qtk = tlength/3;
  htk = tlength>>1;
  {
    int aa = toffset/tick;
    itmp = toffset - aa *tick;
    cnt = -aa % bigtick;
  }
  err = 0.0;
  while(itmp < w)
    {
      int yy1, yy2;
      cnt++;
      itmp += tick;
      err += tickerror;
      if(err > 1.0) { err -= 1.0; itmp += 1;}
      if(orientation == EZ_TOP)
        {
          yy2 = h-bw;
          if(cnt & 1) yy1 =(h - qtk)-bw;
          else 
            {
              if(cnt == bigtick) { cnt = 0; yy1 = h-tlength-bw; }
              else yy1 = h-htk-bw;
            }
        }
      else
        {
          yy2 = bw;
          if(cnt & 1) yy1 = qtk+bw;
          else 
            {
              if(cnt == bigtick) { cnt = 0; yy1 = tlength+bw; }
              else yy1 = htk+bw;
            }
        }
      XDrawLine(EZ_Display, pixmap, gc, itmp, yy1, itmp, yy2);
    }
  /*--------------------------------------------------------
   *  Draw label and values.
   *-------------------------------------------------------*/
  tscale = EZ_RulerTickScale(wptr);
  if(EZ_RulerDisplayValue(wptr) && tscale)
    {
      int shadow = EZ_WidgetLabelShadow(wptr);
      int shadowx, shadowy, yy;
      char str[8];

      if(shadow)
        {
          if(shadow &0xf0)
            {
              shadowx = (shadow & 0xf0) >> 4;
              shadowy = (shadow & 0x0f);
            }
          else shadowx = shadowy = shadow;
          if(shadowx & 8) shadowx= (8-shadowx);
          if(shadowy & 8) shadowy= (8-shadowy);
          XSetFont(EZ_Display, EZ_SHADOWGC, EZ_WidgetFont(wptr)->fid);
        } else { shadowx = shadowy = 0;}
      tick = tick * bigtick;
      {
        int aa = toffset/tick;
        itmp = toffset - aa * tick + 2;
        cnt = -aa;
      }
      if(orientation == EZ_TOP)  yy = h - htk -2 -bw;
      else  yy = htk+EZ_RulerFontAscent(wptr) +2 + bw;
      cnt *= tscale;
      err = 0.0;
      tickerror *= (float)bigtick;
      while(itmp < w)
        {
          int len;
          cnt += tscale;
          sprintf(str, "%d", cnt);
          itmp += tick;
          err += tickerror;
          if(err > 1.0) { err -= 1.0; itmp += 1;}
          len = strlen(str);
          XDrawString(EZ_Display, pixmap, gc, itmp, yy, str, len);
          if(shadow)
            XDrawString(EZ_Display, pixmap, EZ_SHADOWGC, itmp+shadowx, yy+shadowy, str, len);
        }
    }
  EZ_DrawRectBorder(wptr, pixmap);
  XCopyArea(EZ_Display,pixmap,win, EZ_WRITABLEGC,0,0,w,h,0,0); 
  XFreePixmap(EZ_Display, pixmap);
  {
    char *t = EZ_RulerMarkStatus(wptr);
    t[0] = t[1] = t[2] =  0;
  }
  showHRulerMarker(wptr);
}

static void showHRulerMarker(wptr) EZ_Widget *wptr;
{
  int bw, xy, hh, orientation = EZ_RulerOrientation(wptr);
  GC gc = EZ_XORGC;
  XPoint pts[3];
  int i, *pos = EZ_RulerPosition(wptr);
  char   *mstat = EZ_RulerMarkStatus(wptr);

  bw = EZ_WidgetBorderWidth(wptr);
  hh = EZ_WidgetHeight(wptr)-bw;

  for(i = 0; i < 3; i++)
    {
      if(pos[i] != pos[i+3])
        {
          if(mstat[i])
            {
              xy = pos[i+3];
              if(orientation==EZ_TOP)
                {
                  pts[0].x=xy;    pts[0].y=hh-2;
                  pts[1].x=xy-4;  pts[1].y=hh-8;
                  pts[2].x=xy+4;  pts[2].y=hh-8;
                }
              else
                {
                  pts[0].x=xy;    pts[0].y=2+bw;
                  pts[1].x=xy-4;  pts[1].y=8+bw;
                  pts[2].x=xy+4;  pts[2].y=8+bw;
                }
              XFillPolygon(EZ_Display, EZ_WidgetWindow(wptr), gc,
                           pts, 3, Convex, CoordModeOrigin);
            }
          xy = pos[i];
          if(orientation==EZ_TOP)
            {
              pts[0].x=xy;    pts[0].y=hh-2;
              pts[1].x=xy-4;  pts[1].y=hh-8;
              pts[2].x=xy+4;  pts[2].y=hh-8;
            }
          else
            {
              pts[0].x=xy;    pts[0].y=2+bw;
              pts[1].x=xy-4;  pts[1].y=8+bw;
              pts[2].x=xy+4;  pts[2].y=8+bw;
            }
          XFillPolygon(EZ_Display, EZ_WidgetWindow(wptr), gc,
                       pts, 3, Convex, CoordModeOrigin);  
          pos[i+3] = pos[i];
          mstat[i] = 1;
        }
      gc = EZ_DNDXORGC;
    }
}
/**************************************************************************/
void  EZ_RulerEventHandle(widget, event)
     EZ_Widget *widget;
     XEvent          *event;
{
  if(widget == (EZ_Widget *)NULL) return;
  if(event->type == Expose && event->xexpose.count==0)
    EZ_DrawWidget(widget);      
  if(event->type == ButtonPress && event->xbutton.button == EZ_Btn3)
    {
      EZ_HandleDnDEvents(widget, event);
      return;
    }
  if(event->type == MotionNotify)
    {
      int *pos = EZ_RulerPosition(widget);
      if(EZ_WidgetType(widget) == EZ_WIDGET_HORIZONTAL_RULER)
        {
          int x = event->xmotion.x;
          if(pos[0] != x)
            {
              pos[0] = x;
              showHRulerMarker(widget);
            }
        }
      else
        {
          int y = event->xmotion.y;
          if(pos[0] != y)
            {
              pos[0] = y;
              showVRulerMarker(widget);
            }
        }
    }
  else  if(event->type == ButtonPress)
    {
      if(event->xbutton.button == EZ_Btn1)
        {
          if(EZ_WidgetType(widget) == EZ_WIDGET_HORIZONTAL_RULER)
            {
              int x = event->xbutton.x;
              int *pos = EZ_RulerPosition(widget);
              int mk;

              if(pos[1] < 0) mk = 1;
              else if(pos[2] < 0) mk = 2;
              else
                {
                  int a,b,tmp;
                  tmp = x - pos[1]; a = tmp >=0 ? tmp : -tmp;
                  tmp = x - pos[2]; b = tmp >=0 ? tmp : -tmp;
                  mk = (a > b ? 2 : 1);
                }
              EZ_RulerMarker(widget) = mk;
              if(pos[mk] != x)
                {
                  pos[mk] = x;
                  showHRulerMarker(widget);
                }
            }
          else
            {
              int y = event->xmotion.y;
              int *pos = EZ_RulerPosition(widget);
              int mk;
              if(pos[1] < 0) mk = 1;
              else if(pos[2] < 0) mk = 2;
              else
                {
                  int a,b,tmp;
                  tmp = y - pos[1]; a = tmp >=0 ? tmp : -tmp;
                  tmp = y - pos[2]; b = tmp >=0 ? tmp : -tmp;
                  mk = (a > b ? 2 : 1);
                }
              EZ_RulerMarker(widget) = mk;
              if(pos[mk] != y)
                {
                  pos[mk] = y;
                  showVRulerMarker(widget);
                }
            }
        }
      else /* must be button 2 */
        {
          int xr, yr, a1, a2, *pos = EZ_RulerPosition(widget);
          char *marks = EZ_RulerMarkStatus(widget);
          Window child, win = RootWindow(EZ_Display, EZ_ScreenNum);
          int  xx0, xx1, xx2,xx3, yy0, yy1, yy2,yy3, vv, vx, vy, slen;
          int on=0, moved = 0, itmp, px, py;
          float uu, bigtick;
          bigtick = (float)EZ_RulerBigTick(widget) *(1.0 + EZ_RulerTickError(widget)) *
            (float) EZ_RulerTickSize(widget);
          a1 = pos[1]; a2 = pos[2];
          if(marks[1] && a1 >=0 && a2 >=0 && a1 != a2)
            {
              if(XTranslateCoordinates(EZ_Display, EZ_WidgetWindow(widget),
                                       win, a1, a2, &xr, &yr, &child))
                {
                  XEvent xevent;
                  char str[32];
                  vv=(a2>a1? a2-a1: a1-a2);
                  uu = (float)vv/(bigtick) * (float)EZ_RulerTickScale(widget);
                  sprintf(str, "%.2f (%d pixels)",uu, vv);                  

                  px = event->xbutton.x;  
                  py = event->xbutton.y;          
                  if(EZ_WidgetType(widget) == EZ_WIDGET_HORIZONTAL_RULER)
                    {
                      xx0 = xr;    yy0 = 0; 
                      xx1 = xr;    yy1 = EZ_XDisplayHeight;
                      xx2 = xr+a2-a1; yy2 = 0;
                      xx3 = xx2; yy3 = yy1;
                      itmp = px - a1;  if(ABSV(itmp) < 4) on =1;
                      else { itmp = px - a2;  if(ABSV(itmp) < 4) on =2;}
                    }
                  else
                    {
                      xx0 = 0;                yy0 = yr;
                      xx1 = EZ_XDisplayWidth; yy1 = yr;
                      xx2 = 0;                yy2 = yr + a1 -a2;
                      xx3 = xx1;              yy3 = yy2;        
                      itmp = py - a1;  if(ABSV(itmp) < 4) on =1;
                      else { itmp = py - a2;  if(ABSV(itmp) < 4) on =2;}
                    }
                  vx = xr + event->xbutton.x-a1 + 4;
                  vy = yr + event->xbutton.y-a2 + 10;
                  slen = strlen(str);
                  EZ_GrabServer();
                  XDrawLine(EZ_Display,win, EZ_XORGC, xx0, yy0, xx1, yy1);
                  XDrawLine(EZ_Display,win, EZ_XORGC, xx2, yy2, xx3, yy3);
                  XDrawString(EZ_Display, win, EZ_DNDXORGC,  vx, vy, str, slen);
                  while(1)
                    {
                      XNextEvent(EZ_Display, &xevent);
                      if(xevent.type == ButtonRelease)
                        {
                          XDrawLine(EZ_Display,win, EZ_XORGC, xx0, yy0, xx1, yy1);
                          XDrawLine(EZ_Display,win, EZ_XORGC, xx2, yy2, xx3, yy3);
                          XDrawString(EZ_Display, win, EZ_DNDXORGC,  vx, vy, str, slen);
                          if(moved)
                            {
                              pos[on] = moved;
                              if(EZ_WidgetType(widget) == EZ_WIDGET_HORIZONTAL_RULER)   
                                showHRulerMarker(widget);
                              else 
                                showVRulerMarker(widget);
                            }
                          break;
                        }
                      else if(on && xevent.type == MotionNotify)
                        {
                          if(EZ_WidgetType(widget) == EZ_WIDGET_HORIZONTAL_RULER)
                            {
                              if(on==2) 
                                {
                                  XDrawLine(EZ_Display,win, EZ_XORGC, xx2, yy2, xx3, yy3);
                                  XDrawString(EZ_Display, win, EZ_DNDXORGC,  vx, vy, str, slen);
                                  xx2 += xevent.xmotion.x - a2;
                                  xx3 = xx2;
                                  moved = a2 = xevent.xmotion.x;
                                  vv=(a2>a1? a2-a1: a1-a2);
                                  uu = (float)vv/(bigtick) * (float)EZ_RulerTickScale(widget);
                                  sprintf(str, "%.2f (%d pixels)",uu, vv);  
                                  XDrawLine(EZ_Display,win, EZ_XORGC, xx2, yy2, xx3, yy3);
                                  XDrawString(EZ_Display, win, EZ_DNDXORGC,  vx, vy, str, slen);
                                }
                              else
                                {
                                  XDrawLine(EZ_Display,win, EZ_XORGC, xx0, yy0, xx1, yy1);  
                                  XDrawString(EZ_Display, win, EZ_DNDXORGC,  vx, vy, str, slen);
                                  xx0 += xevent.xmotion.x - a1;
                                  xx1 = xx0;
                                  moved = a1 = xevent.xmotion.x;
                                  vv=(a2>a1? a2-a1: a1-a2);
                                  uu = (float)vv/(bigtick) * (float)EZ_RulerTickScale(widget);
                                  sprintf(str, "%.2f (%d pixels)",uu, vv);  
                                  XDrawLine(EZ_Display,win, EZ_XORGC, xx0, yy0, xx1, yy1);  
                                  XDrawString(EZ_Display, win, EZ_DNDXORGC,  vx, vy, str, slen);
                                }
                            }
                          else
                            {
                              if(on==1) 
                                {
                                  XDrawLine(EZ_Display,win, EZ_XORGC, xx2, yy2, xx3, yy3);
                                  XDrawString(EZ_Display, win, EZ_DNDXORGC,  vx, vy, str, slen);
                                  yy2 += xevent.xmotion.y - a1;
                                  yy3 = yy2;
                                  moved = a1 = xevent.xmotion.y;
                                  vv=(a2>a1? a2-a1: a1-a2);
                                  uu = (float)vv/(bigtick) * (float)EZ_RulerTickScale(widget);
                                  sprintf(str, "%.2f (%d pixels)",uu, vv);  
                                  XDrawLine(EZ_Display,win, EZ_XORGC, xx2, yy2, xx3, yy3);
                                  XDrawString(EZ_Display, win, EZ_DNDXORGC,  vx, vy, str, slen);
                                }                              
                              else
                                {
                                  XDrawLine(EZ_Display,win, EZ_XORGC, xx0, yy0, xx1, yy1);  
                                  XDrawString(EZ_Display, win, EZ_DNDXORGC,  vx, vy, str, slen);
                                  yy0 += xevent.xmotion.y - a2;
                                  yy1 = yy0;
                                  moved = a2 = xevent.xmotion.y;
                                  vv=(a2>a1? a2-a1: a1-a2);
                                  uu = (float)vv/(bigtick) * (float)EZ_RulerTickScale(widget);
                                  sprintf(str, "%.2f (%d pixels)",uu, vv);  
                                  XDrawLine(EZ_Display,win, EZ_XORGC, xx0, yy0, xx1, yy1);  
                                  XDrawString(EZ_Display, win, EZ_DNDXORGC,  vx, vy, str, slen);
                                }
                            }
                        }
                    }
                  EZ_UngrabServer();
                }
            }
        }
    }
  else if(event->type == ConfigureNotify)
    {
      int *t = EZ_RulerPosition(widget);
      t[0] = t[1] = t[2] = -1;
    }
}
/********************************************************************
 *
 *  Figure out the dimension of a vertical Ruler
 */
void EZ_ComputeWidgetVRulerSize(widget, w, h)
    EZ_Widget *widget;
     int             *w, *h;
{
  int width, cw, ch, tlength, htkl;

  if(!(EZ_GetWidgetSizeComputedFlag(widget)))
    { 
      EZ_RulerFontHeight(widget) = EZ_WidgetFont(widget)->ascent + EZ_WidgetFont(widget)->descent;
      EZ_RulerFontAscent(widget) = EZ_WidgetFont(widget)->ascent;
      EZ_RulerFontWidth(widget) = EZ_WidgetFont(widget)->max_bounds.width;      
      EZ_SetWidgetSizeComputedFlag(widget);
    }
  width = EZ_RulerFontWidth(widget);
  tlength = EZ_RulerTickLength(widget);
  htkl = tlength >> 1;
  width += (htkl + 4);
  if(width < tlength) width = tlength;
  cw = EZ_WidgetPadX(widget) + EZ_WidgetBorderWidth(widget) + EZ_WidgetPadB(widget);
  ch = EZ_WidgetPadY(widget) + EZ_WidgetBorderWidth(widget) + EZ_WidgetPadB(widget);

  ch = ch + ch + 8;
  cw = cw + cw + width;
  
  *w = cw;
  *h = ch;
  checkRulerUnit(widget);
}

/**************************************************************************/

void  EZ_DrawWidgetVerticalRuler(wptr)
     EZ_Widget *wptr;
{
  int             bw, w, h, cw, ch, padb, padb2, tile, orientation;
  int             tick, htk, qtk, tlength, tox = 0, toy = 0, itmp, cnt;
  int             toffset, tscale, bigtick;
  float           tickerror, err;
  Pixmap          pixmap;
  Window          win;
  GC              gc;

  unsigned long   bgpv;

  win = EZ_WidgetWindow(wptr);
  w   = EZ_WidgetWidth(wptr);
  h   = EZ_WidgetHeight(wptr);
  bw = EZ_WidgetBorderWidth(wptr);
  padb = EZ_WidgetPadB(wptr);
  padb2 = padb + padb;

  /*-----------------------------------------------------------
   *  Create a pixmap, draw into this pixmap in background and
   *  copy to the button window when finished.
   *----------------------------------------------------------*/
  pixmap = XCreatePixmap(EZ_DisplayForWidgets, win, w, h, EZ_DepthForWidgets);    
  EZ_FixFocusHighlightBorder(wptr, pixmap, w, h, padb);

  tile = EZ_GetBackgroundGC(wptr, &gc, &bgpv,0, 0);
  if(tile)
    {
      EZ_GetWidgetTileOrigin(wptr, &tox, &toy);      
      XSetTSOrigin(EZ_Display, gc, tox, toy);
    }
  XFillRectangle(EZ_Display, pixmap, gc, padb,padb, w-padb2, h-padb2); 

  cw = EZ_WidgetBorderWidth(wptr) + EZ_WidgetPadX(wptr) + padb;
  ch = EZ_WidgetBorderWidth(wptr) + EZ_WidgetPadY(wptr) + padb;

  gc = EZ_WRITABLEGC;
  XSetFont(EZ_DisplayForWidgets, gc, EZ_WidgetFont(wptr)->fid);
  XSetForeground(EZ_DisplayForWidgets, gc, EZ_WidgetForeground(wptr));	
  /*--------------------------------------------------------
   *  Draw ticks
   *-------------------------------------------------------*/
  orientation = EZ_RulerOrientation(wptr);
  toffset = EZ_RulerTOffset(wptr);
  tick = EZ_RulerTickSize(wptr);
  tlength = EZ_RulerTickLength(wptr);
  bigtick = EZ_RulerBigTick(wptr);
  tickerror = EZ_RulerTickError(wptr);
  qtk = tlength/3;
  htk = tlength>>1;
  {
    int aa = toffset/tick;
    itmp = toffset - aa * tick;
    cnt = -aa % bigtick;
  }
  err = 0.0;
  while(itmp < h)
    {
      int xx1, xx2;
      cnt++;
      itmp += tick;
      err += tickerror;
      if(err > 1.0) { err -= 1.0; itmp += 1;}
      if(orientation == EZ_LEFT)
        {
          xx2 = w-bw;
          if(cnt & 1) xx1 =(w - qtk)-bw;
          else 
            {
              if(cnt == bigtick) { cnt = 0; xx1 = w-tlength-bw; }
              else xx1 = w-htk-bw;
            }
        }
      else
        {
          xx2 = bw;
          if(cnt & 1) xx1 = qtk+bw;
          else 
            {
              if(cnt == bigtick) { cnt = 0; xx1 = tlength+bw; }
              else xx1 = htk+bw;
            }
        }
      XDrawLine(EZ_Display, pixmap, gc, xx1, itmp,xx2, itmp);
    }
  /*--------------------------------------------------------
   *  Draw label and values.
   *-------------------------------------------------------*/
  tscale = EZ_RulerTickScale(wptr);
  if(EZ_RulerDisplayValue(wptr) && tscale)
    {
      int shadow = EZ_WidgetLabelShadow(wptr);
      int shadowx, shadowy, xx, yy, fwidth, fheight;
      char str[8];

      if(shadow)
        {
          if(shadow &0xf0)
            {
              shadowx = (shadow & 0xf0) >> 4;
              shadowy = (shadow & 0x0f);
            }
          else shadowx = shadowy = shadow;
          if(shadowx & 8) shadowx= (8-shadowx);
          if(shadowy & 8) shadowy= (8-shadowy);
          XSetFont(EZ_Display, EZ_SHADOWGC, EZ_WidgetFont(wptr)->fid);
        } else { shadowx = shadowy = 0;}

      tick = tick * bigtick;
      itmp = toffset;
      {
        int aa = toffset/tick;
        itmp = toffset -aa * tick;
        cnt = -aa;
        }
      itmp += (2 + EZ_RulerFontAscent(wptr));
      fheight = EZ_RulerFontHeight(wptr);
      fwidth = EZ_RulerFontWidth(wptr);
      if(orientation == EZ_LEFT)  xx = w - htk -2 -bw - fwidth;
      else  xx = htk+2 + bw;

      cnt *= tscale;
      err = 0;
      tickerror *= (float)bigtick;
      while(itmp < h)
        {
          int i,len;
          cnt += tscale;
          sprintf(str, "%d", cnt);
          itmp += tick;
          err += tickerror;
          if(err > 1.0) { err -= 1.0; itmp += 1;}          
          len = strlen(str);
          yy = itmp;
          for(i = 0; i < len; i++)
            {
              XDrawString(EZ_Display, pixmap, gc, xx,yy, str+i, 1);
              if(shadow)
                XDrawString(EZ_Display, pixmap, EZ_SHADOWGC,
                            xx+shadowx, yy+shadowy, str+i, 1);
              yy += fheight;
            }
        }
    }
  EZ_DrawRectBorder(wptr, pixmap);
  XCopyArea(EZ_Display,pixmap,win, EZ_WRITABLEGC,0,0,w,h,0,0); 
  XFreePixmap(EZ_Display, pixmap);
  {
    char *t = EZ_RulerMarkStatus(wptr);
    t[0] = t[1] = t[2] =  0;
  }
  showVRulerMarker(wptr);
}

static void showVRulerMarker(wptr) EZ_Widget *wptr;
{
  int bw, xy, ww, orientation = EZ_RulerOrientation(wptr);
  GC gc = EZ_XORGC;
  XPoint pts[3];
  int i, *pos = EZ_RulerPosition(wptr);
  char   *mstat = EZ_RulerMarkStatus(wptr);

  bw = EZ_WidgetBorderWidth(wptr);
  ww = EZ_WidgetWidth(wptr)-bw;
  for(i = 0; i < 3; i++)
    {
      if(pos[i] != pos[i+3])
        {
          if(mstat[i])
            {
              xy = pos[i+3];
              if(orientation==EZ_LEFT)
                {
                  pts[0].x=ww-2;  pts[0].y=xy;
                  pts[1].x=ww-8;  pts[1].y=xy-4;
                  pts[2].x=ww-8;  pts[2].y=xy+4;
                }
              else
                {
                  pts[0].x=2+bw;  pts[0].y=xy;
                  pts[1].x=8+bw;  pts[1].y=xy-4;
                  pts[2].x=8+bw;  pts[2].y=xy+4;
                }
              XFillPolygon(EZ_Display, EZ_WidgetWindow(wptr), gc,
                           pts, 3, Convex, CoordModeOrigin);

            }
          xy = pos[i];
          if(orientation==EZ_LEFT)
            {
              pts[0].x=ww-2;  pts[0].y=xy;
              pts[1].x=ww-8;  pts[1].y=xy-4;
              pts[2].x=ww-8;  pts[2].y=xy+4;
            }
          else
            {
              pts[0].x=2+bw;  pts[0].y=xy;
              pts[1].x=8+bw;  pts[1].y=xy-4;
              pts[2].x=8+bw;  pts[2].y=xy+4;
            }
          XFillPolygon(EZ_Display, EZ_WidgetWindow(wptr), gc,
                       pts, 3, Convex, CoordModeOrigin);  
          pos[i+3]=pos[i];
          mstat[i] = 1;
        }
      gc = EZ_DNDXORGC;          
    }
}
/**************************************************************************/
void EZ_WidgetNotifyRuler(ruler, x, y, fwidget)
     EZ_Widget *ruler; int x, y;
     EZ_Widget *fwidget;
{
  if(ruler && EZ_WidgetMapped(ruler))
    {
      Window srcwin = None, child;
      int xr, yr, type = EZ_WidgetType(ruler);
      if(type != EZ_WIDGET_HORIZONTAL_RULER && 
         type != EZ_WIDGET_VERTICAL_RULER) return;
      if(fwidget==NULL) srcwin = RootWindow(EZ_Display, EZ_ScreenNum);
      else if(fwidget && EZ_WidgetMapped(fwidget))
        srcwin = EZ_WidgetWindow(fwidget);

      if(srcwin != None)
        {
          if(XTranslateCoordinates(EZ_Display, srcwin,
                                   EZ_WidgetWindow(ruler), x,y, &xr, &yr, &child))
            {
              int *pos = EZ_RulerPosition(ruler);
              if(type == EZ_WIDGET_HORIZONTAL_RULER)
                {
                  pos[0] = xr;
                  showHRulerMarker(ruler);
                }
              else
                {
                  pos[0] = yr;
                  showVRulerMarker(ruler);
                }
            }
        }
    }
}

void EZ_XWindowNotifyRuler(ruler, x, y, window)
     EZ_Widget *ruler; int x, y;
     Window window;
{
  if(ruler && EZ_WidgetMapped(ruler) && window != None)
    {
      Window srcwin = window, child;
      int xr, yr, type = EZ_WidgetType(ruler);
      if(type != EZ_WIDGET_HORIZONTAL_RULER && 
         type != EZ_WIDGET_VERTICAL_RULER) return;

      if(XTranslateCoordinates(EZ_Display, srcwin,
                               EZ_WidgetWindow(ruler), x,y, &xr, &yr, &child))
        {
          int *pos = EZ_RulerPosition(ruler);
          if(type == EZ_WIDGET_HORIZONTAL_RULER)
            {
              pos[0] = xr;
              showHRulerMarker(ruler);
            }
          else
            {
              pos[0] = yr;
              showVRulerMarker(ruler);
            }
        }
    }
}

void EZ_WidgetMarkRuler(ruler, x, y, fwidget)
     EZ_Widget *ruler; int x, y;
     EZ_Widget *fwidget;
{
  if(ruler && EZ_WidgetMapped(ruler))
    {
      Window srcwin = None, child;
      int xr, yr, type = EZ_WidgetType(ruler);
      if(type != EZ_WIDGET_HORIZONTAL_RULER && 
         type != EZ_WIDGET_VERTICAL_RULER) return;
      if(fwidget==NULL) srcwin = RootWindow(EZ_Display, EZ_ScreenNum);
      else if(fwidget && EZ_WidgetMapped(fwidget))
        srcwin = EZ_WidgetWindow(fwidget);

      if(srcwin != None)
        {
          if(XTranslateCoordinates(EZ_Display, srcwin,
                                   EZ_WidgetWindow(ruler), x,y, &xr, &yr, &child))
            {
              int *pos = EZ_RulerPosition(ruler);
              int mk = EZ_RulerMarker(ruler) + 1;
              if(mk > 2) mk = 1;
              EZ_RulerMarker(ruler) = mk;
              if(type == EZ_WIDGET_HORIZONTAL_RULER)
                {
                  if(pos[mk] != xr)
                    {
                      pos[mk] = xr;
                      showHRulerMarker(ruler);
                    }
                }
              else
                {
                  if(pos[mk] != yr)
                    {
                      pos[mk] = yr;
                      showVRulerMarker(ruler);
                    }
                }
            }
        }
    }
}

void EZ_XWindowMarkRuler(ruler, x, y, window)
     EZ_Widget *ruler; int x, y;
     Window window;
{
  if(ruler && EZ_WidgetMapped(ruler) && window != None)
    {
      Window srcwin = window, child;
      int xr, yr, type = EZ_WidgetType(ruler);
      if(type != EZ_WIDGET_HORIZONTAL_RULER && 
         type != EZ_WIDGET_VERTICAL_RULER) return;

      if(XTranslateCoordinates(EZ_Display, srcwin,
                               EZ_WidgetWindow(ruler), x,y, &xr, &yr, &child))
        {
          int *pos = EZ_RulerPosition(ruler);
          int mk = EZ_RulerMarker(ruler) + 1;
          if(mk > 2) mk = 1;
          EZ_RulerMarker(ruler) = mk;
          if(type == EZ_WIDGET_HORIZONTAL_RULER)
            {
              if(pos[mk] != xr)
                {
                  pos[mk] = xr;
                  showHRulerMarker(ruler);
                }
            }
          else
            {
              if(pos[mk] != yr)
                {
                  pos[mk] = yr;
                  showVRulerMarker(ruler);
                }
            }
        }
    }
}
/**************************************************************************/
void  EZ_SetRulerOffset(ruler, offset) EZ_Widget *ruler; int offset;
{
 if(ruler)
    {
      int type = EZ_WidgetType(ruler);
      if(type == EZ_WIDGET_HORIZONTAL_RULER ||
         type == EZ_WIDGET_VERTICAL_RULER)
        {
          EZ_RulerTOffset(ruler) = offset;
          if(EZ_WidgetMapped(ruler)) EZ_DrawWidget(ruler);
        }
    }
}
void EZ_SetupRuler(ruler, tunit, tlen, nscale, offset)
     EZ_Widget *ruler; int offset, tunit, tlen, nscale;
{
  if(ruler)
    {
      int type = EZ_WidgetType(ruler);
      if(type == EZ_WIDGET_HORIZONTAL_RULER ||
         type == EZ_WIDGET_VERTICAL_RULER)
        {
          EZ_RulerTOffset(ruler) = offset;
          EZ_RulerTickSize(ruler) = tunit;
          EZ_RulerTickLength(ruler) = tlen;
          EZ_RulerTickScale(ruler) = nscale;
          if(EZ_WidgetMapped(ruler)) EZ_DrawWidget(ruler);
        }
    }

}
/**************************************************************************/
int  *EZ_GetRulerMarks(ruler) EZ_Widget *ruler;
{
 if(ruler)
    {
      int type = EZ_WidgetType(ruler);
      if(type == EZ_WIDGET_HORIZONTAL_RULER ||
         type == EZ_WIDGET_VERTICAL_RULER)
        {
          return(EZ_RulerPosition(ruler));
        }
    }
 return(NULL);
}
/**************************************************************************/
static void  timerCb(timer, data)
     EZ_Timer *timer; void *data;
{
  EZ_Widget *widget = (EZ_Widget *)data;
  if(EZ_LookupWidgetFromAllHT(widget) && EZ_WidgetMapped(widget))
    {
      int             rx,ry,x,y;
      unsigned int    mask;
      Window          root,win;
      int             *pos;
      if( XQueryPointer(EZ_Display, RootWindow(EZ_Display, EZ_ScreenNum),
                        &root,    &win, &rx, &ry, &x,&y, &mask) &&
          XTranslateCoordinates(EZ_Display, RootWindow(EZ_Display, EZ_ScreenNum),
                                EZ_WidgetWindow(widget), rx, ry, &x, &y, &win))
        {
          pos = EZ_RulerPosition(widget);
          if(EZ_WidgetType(widget) == EZ_WIDGET_HORIZONTAL_RULER)
            {
              if(pos[0] != x)
                {
                  pos[0] = x;
                  showHRulerMarker(widget);
                }
            }
          else
            {
              if(pos[0] != y)
                {
                  pos[0] = y;
                  showVRulerMarker(widget);
                }
            }
        }
    }
}
/**************************************************************************/
static void  checkRulerUnit(widget) EZ_Widget *widget;
{
  int tick =  EZ_RulerTickSize(widget);
  if(tick <= 0)
    {
      int type = EZ_WidgetType(widget);
      int ntick;
      float inpixel, inmm, ftmp;
      if(type == EZ_WIDGET_HORIZONTAL_RULER)
        {
          inpixel = (float)EZ_XDisplayWidth;
          inmm = (float) (DisplayWidthMM(EZ_Display, EZ_ScreenNum));
        }
      else
        {
          inpixel = (float)EZ_XDisplayHeight;
          inmm = (float) (DisplayHeightMM(EZ_Display, EZ_ScreenNum));
        }
      if(tick == 0) /* metric */
        {
          EZ_RulerBigTick(widget) = 4; /* 1mm a tick */
          ftmp = 2.5 *inpixel/inmm;
          ntick = (int)(ftmp);
          EZ_RulerTickError(widget) = 0.25 *(ftmp - (float)ntick);
        }
      else /* in inches */
        {
          EZ_RulerBigTick(widget) = 8; /* 1/8th in a tick */
          ftmp =3.175 * inpixel/inmm;  /* 1in in pixels */
          ntick = (int) (ftmp);
          EZ_RulerTickError(widget) = 0.125 *(ftmp - (float)ntick);          
        }
      if(ntick == 0) ntick = 1;
      EZ_RulerTickScale(widget) = 1;
      EZ_RulerTickSize(widget) = ntick;
    }
}
/**************************************************************************/
#undef _EZ_WIDGET_RULER_C_












