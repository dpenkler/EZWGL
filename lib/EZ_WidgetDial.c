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
 ***              Widget Dial                                  ***
 ***                                                           ***
 *****************************************************************/
#define _EZ_WIDGET_DIAL_C_

#include "EZ_Widget.h"

/*********************************************************************
 * 
 *  Functions implemented in this file:
 */

EZ_Widget   *EZ_CreateDial MY_ANSIARGS((EZ_Widget *parent));
float        EZ_GetDialValue MY_ANSIARGS((EZ_Widget *dial));
void         EZ_SetDialValue  MY_ANSIARGS((EZ_Widget *dial, float value));


void         EZ_ComputeDialSize  MY_ANSIARGS((EZ_Widget *widget, int *w, int *h));
void         EZ_DrawDial  MY_ANSIARGS((EZ_Widget *widget));
void         EZ_FreeDialData  MY_ANSIARGS((EZ_Widget *widget));
void         EZ_DialEventHandle  MY_ANSIARGS((EZ_Widget *widget, XEvent *event));

/*********************************************************************
 * 
 *  Local functions:
 */

/*********************************************************************
 * 
 *  Local Variables.
 */
static EZ_WidgetHandle EZ_DialHandle =
{
  EZ_ComputeDialSize,
  EZ_DrawDial,
  EZ_FreeDialData,
  EZ_DialEventHandle,
};

/*********************************************************************/

EZ_Widget  *EZ_CreateDial(parent)
     EZ_Widget  *parent;     /* parent widget    */
{
  EZ_Widget  *wptr;

  wptr = EZ_CreateNewWidget(parent);
  EZ_WidgetHandlingFunctions[EZ_WIDGET_DIAL] = &EZ_DialHandle;
  EZ_SetWidgetTypeAndNames(wptr, EZ_WIDGET_DIAL);

  EZ_WidgetFont(wptr) = EZ_GetFontFromId(EZ_TINY_FONT);
  EZ_WidgetSepY(wptr) = 8;

  EZ_DialForeground(wptr)  = ~0;
  EZ_DialFrom(wptr) = 0.0;
  EZ_DialTo(wptr)   = 10.0;
  EZ_DialResolution(wptr) = 0.1;

  EZ_DialNTicks(wptr)    = 10;    
  EZ_DialTickSize(wptr)  = 8;
  EZ_DialSpan(wptr)      = 270.0;
  EZ_DialShowValue(wptr) = 10;
  EZ_DialStyle(wptr)     = 0;
  EZ_DialVSet(wptr)     = 0;

  EZ_SetWidgetFocusableFlag(wptr);  

  return(wptr);
}
/********************************************************************/
void EZ_FreeDialData(widget) EZ_Widget *widget;
{
  if(EZ_DialLabel(widget))
    { (void) my_free((void *) EZ_DialLabel(widget));  EZ_DialLabel(widget) = NULL;}
}
/********************************************************************
 *
 *  Figure out the dimension of a horizontal slider
 */
#define ONEPI              (3.141592653589793231)
#define HALFPI             (ONEPI * 0.5)
#define THREEHALFPI        (ONEPI * 1.5)
#define TWOPI              (ONEPI * 2.0)
#define DEGREE2RADIAN(aa)  ((aa) * ONEPI/180.0)
#define RADIAN2DEGREE(aa)  ((aa) * 180.0/ONEPI)

void EZ_ComputeDialSize(widget, w, h)
     EZ_Widget *widget;
     int             *w, *h;
{
  int cw, ch, hadj, bwp, ticksize, width, height, twidth, showv, nticks, tsize;
  float span2, ftmp, hspan, wspan, sss;

  if(!(EZ_GetWidgetSizeComputedFlag(widget)))
    { EZ_SetWidgetSizeComputedFlag(widget); }

  ftmp = EZ_DialSpan(widget);
  /*
  if(ftmp < 180.0)
    { ftmp = EZ_DialSpan(widget) = 180.0;}
    */
  ftmp = (ftmp -180.0) * 0.5;
  EZ_DialOffset(widget) = -ftmp;
  span2 = DEGREE2RADIAN(ftmp);

  wspan = 2.0;
  sss = sin(span2);
  hspan = (1.0 + sss);

  showv = EZ_DialShowValue(widget);
  nticks = EZ_DialNTicks(widget);
  tsize = EZ_DialTickSize(widget);
  if(tsize == 0) tsize = 16;
  else tsize += tsize;
  ticksize = tsize;
  
  EZ_SetPrintFFormat(EZ_DialTo(widget), EZ_DialFrom(widget),EZ_DialResolution(widget),
                     (float) EZ_DialSpan(widget), EZ_DialFormat(widget));
  if(showv)
    {
      char str[32];
      int a, b;
      sprintf(str, EZ_DialFormat(widget), EZ_DialFrom(widget));
      a = XTextWidth(EZ_WidgetFont(widget), str,strlen(str));
      sprintf(str, EZ_DialFormat(widget), EZ_DialTo(widget));
      b = XTextWidth(EZ_WidgetFont(widget), str,strlen(str));
      twidth = EZ_DialTWidth(widget) = MINV(a,b);
      twidth += twidth;
    }
  else twidth = EZ_DialTWidth(widget) = 0;
  
  EZ_DialAscent(widget) = EZ_WidgetFont(widget)->ascent;
  EZ_DialTHeight(widget) = EZ_DialAscent(widget)+EZ_WidgetFont(widget)->descent;
  
  ticksize <<= 1;
  twidth += ticksize;
  width = (int)( (float)twidth * wspan);
  height =(int)( (float)twidth * hspan);
  
  hadj=EZ_DialHAdjustment(widget) = tsize;

  if(EZ_DialLabelLength(widget)>0)
    {
      EZ_DialLabelWidth(widget) = XTextWidth(EZ_WidgetFont(widget),
                                             EZ_DialLabel(widget),
                                             EZ_DialLabelLength(widget));
      EZ_DialLabelHeight(widget) = EZ_DialTHeight(widget) + EZ_WidgetSepY(widget);
      if(EZ_DialStyle(widget)) EZ_DialLabelHeight(widget) += EZ_DialTickSize(widget);
    }
  else
    {
      EZ_DialLabelWidth(widget) = 0; 
      EZ_DialLabelHeight(widget) = 0;     
    }
  bwp = EZ_WidgetPadB(widget) + EZ_WidgetBorderWidth(widget);
  cw = bwp + EZ_WidgetPadX(widget);
  ch = bwp + EZ_WidgetPadY(widget);
  EZ_DialARatio(widget) = (float)width/(float)height;
  EZ_DialFactor(widget) = (EZ_DialTo(widget)-EZ_DialFrom(widget))/(float) EZ_DialSpan(widget);

  /*-------------------------------
   *  minimal size of the X window
   *------------------------------*/
  if(width < EZ_DialLabelWidth(widget)) width = EZ_DialLabelWidth(widget);
  ch = ch + ch + height + hadj + EZ_DialLabelHeight(widget);
  cw = cw + cw + width;
  
        
  *w = cw;
  *h = ch;
}	  
/**************************************************************************/

void  EZ_DrawDial(wptr)
     EZ_Widget *wptr;
{
  int       i, bw,w, h,x,y,z,xx,yy,xoff, yoff, padb, padb2, tmp, tmp1, tmp2;
  int       cx, cy, ticksize, nticks, ticks2, padx, pady, shadow,shadowx,shadowy;
  int       angle1, angle2, width, height, hadj, tile, radius, style,tox,toy;
  int       labelHeight, showv, nocv,islast;
  float     aspan, ftmp,aratio, dt, astart, fangle, vstart, dv, value, vend;
  XPoint    handle[5];

  float     sint[100],cost[100];
  char      str[32];

  Pixmap    pixmap;
  Window    win;
  GC        gc;
  unsigned long   bgpv;
  unsigned long valuemask;
  XGCValues gcvalues;

  win = EZ_WidgetWindow(wptr);
  w   = EZ_WidgetWidth(wptr);
  h   = EZ_WidgetHeight(wptr);
  bw  = EZ_WidgetBorderWidth(wptr);
  padb = EZ_WidgetPadB(wptr);
  padb2 = padb + padb;

  pixmap = XCreatePixmap(EZ_Display, win, w, h, EZ_Depth); 
  EZ_FixFocusHighlightBorder(wptr, pixmap, w, h, padb);
  tile = EZ_GetBackgroundGC(wptr, &gc, &bgpv, 1, 0);
  if(tile)
    {
      EZ_GetWidgetTileOrigin(wptr, &tox, &toy);      
      XSetTSOrigin(EZ_Display, gc, tox, toy);
    }
  XFillRectangle(EZ_Display, pixmap, gc, padb,padb, w-padb2, h-padb2); 

  if(EZ_DialVSet(wptr))
    {
      ftmp = ((EZ_DialValue(wptr) - EZ_DialFrom(wptr)) *(float) EZ_DialSpan(wptr))/
        (EZ_DialTo(wptr) - EZ_DialFrom(wptr));
      EZ_DialAngle(wptr) =(EZ_DialSpan(wptr) + EZ_DialOffset(wptr)) - ftmp;
      EZ_DialVSet(wptr)= 0;
    }

  labelHeight = EZ_DialLabelHeight(wptr);
  showv = EZ_DialShowValue(wptr);
  nocv =(showv ? (EZ_DialNoCValue(wptr)): 1);
  style = EZ_DialStyle(wptr);
  nticks = EZ_DialNTicks(wptr);
  ticks2 = nticks + nticks;
  ticksize = EZ_DialTickSize(wptr);
  padx = EZ_WidgetPadX(wptr);
  pady = EZ_WidgetPadY(wptr);
  if(nticks && showv>1) showv = nticks;
  
  aspan = EZ_DialSpan(wptr);
  angle1 = (int) ((180.0 - aspan)* 32.0);
  angle2 = (int) (aspan * 64.0);

  islast=(aspan > 330.0);
  tmp1 = ticks2?ticks2:(showv? showv: 1);
  ftmp = DEGREE2RADIAN(aspan);
  dt = -(ftmp)/(float)tmp1;
  tmp = (angle1 + angle2)>>6;
  astart = DEGREE2RADIAN(tmp);
  fangle = astart;
  for(i = 0; i <= tmp1; i++, fangle += dt)
    { cost[i] = cos(fangle);  sint[i] = sin(fangle);}
  vstart = EZ_DialFrom(wptr);
  vend = EZ_DialTo(wptr);
  tmp1 = showv? showv : 1;
  dv = (vend - vstart)/(float)tmp1;

  hadj = EZ_DialHAdjustment(wptr);
  xx = bw +padb + padx;
  yy = bw +padb + pady;
  width = w - (xx<<1);
  height= h - (yy<<1) - hadj - labelHeight;
  ftmp = (float)width/(float)height;
  aratio = EZ_DialARatio(wptr);
  if(ftmp > aratio) /* too wide */
    {
      ftmp = (float)height*aratio;
      tmp  = (int)ftmp;
      xoff = (width - tmp)>>1;
      yoff = 0;
      width = tmp;
    } 
  else /* too high */
    {
      ftmp = (float)width / aratio;
      tmp  = (int)ftmp;
      yoff = (height - tmp)>>1;
      xoff = 0;
    }
  radius = width >> 1;
  EZ_DialRadius(wptr) = radius;
  xx += xoff;
  yy += yoff + labelHeight;
  cx = xx + radius;
  cy = yy + radius;
  EZ_DialCenterX(wptr) = cx;
  EZ_DialCenterY(wptr) = cy;
  if(style)
    {
      tmp = EZ_DialTWidth(wptr);
      radius -= tmp;
      width -= (tmp+tmp);
      xx += tmp;
      yy += tmp;
    }
  height = width;  

  /* handle coordinates */
  {
    float sina, cosa;
    int sx,sy;
    fangle = (float) EZ_DialAngle(wptr);
    fangle = DEGREE2RADIAN(fangle);
    sina = sin(fangle);
    cosa = cos(fangle);
    x = (float)radius * cosa;
    y = (float)radius * sina;
    x = cx + x;
    y = cy - y;
    if(EZ_DialSpan(wptr)>=180.0)
      {
        ftmp = -width * 0.040;
        sx = (int)(ftmp * cosa);
        sy = (int)(ftmp * sina);
        handle[0].x=x;       handle[0].y=y;
        handle[1].x=cx-sy+sx-x; handle[1].y=cy-sx-sy-y;
        handle[2].x=sy-sx;   handle[2].y=sy+sx;
        handle[3].x=sx+sy;   handle[3].y=sx-sy;
      }
    else
      {
        if(x > cx) {sx =2; sy=2;} else {sx = -2; sy=2;}
        handle[0].x=x;    handle[0].y=y;
        handle[1].x=sx;   handle[1].y=sy;
        handle[2].x=cx-x; handle[2].y=cy-y;
        handle[3].x=-sx;   handle[3].y=-sy;
      }
    /* set dial value */
    astart = EZ_DialSpan(wptr) + EZ_DialOffset(wptr);
    fangle = astart - EZ_DialAngle(wptr);
    EZ_DialValue(wptr) = EZ_DialFrom(wptr) + fangle *  EZ_DialFactor(wptr) ;  
  }

  if(EZ_GetWidgetDisabledFlag(wptr) )
    {
      GC  bgc, lgc, ngc;
      if(EZ_GetDarkBrightNormalBDGC(wptr, &bgc, &lgc, &ngc))
        {
          XSetTSOrigin(EZ_Display, bgc, tox, toy);
          XSetTSOrigin(EZ_Display, lgc, tox, toy);
          XSetTSOrigin(EZ_Display, ngc, tox, toy);
        }
      XSetFont(EZ_Display, bgc, EZ_WidgetFont(wptr)->fid);
      XSetFont(EZ_Display, lgc, EZ_WidgetFont(wptr)->fid);
      XSetFont(EZ_Display, ngc, EZ_WidgetFont(wptr)->fid);
      if(EZ_DialLabelLength(wptr)>0)
        {
          x = (w - EZ_DialLabelWidth(wptr))>>1;
          y = (bw+padb+pady)+EZ_DialAscent(wptr) + yoff;
          XDrawString(EZ_Display, pixmap, bgc, x+1, y+1, EZ_DialLabel(wptr), EZ_DialLabelLength(wptr));
          XDrawString(EZ_Display, pixmap, lgc, x-1, y-1, EZ_DialLabel(wptr), EZ_DialLabelLength(wptr));
          XDrawString(EZ_Display, pixmap, ngc, x,   y,   EZ_DialLabel(wptr), EZ_DialLabelLength(wptr));
        }
      XDrawArc(EZ_Display, pixmap, bgc, xx+1, yy+1, width-2, height-2, angle1, angle2);
      XDrawArc(EZ_Display, pixmap, lgc, xx-1, yy-1, width+2, height+2, angle1, angle2);
      XDrawArc(EZ_Display, pixmap, ngc, xx, yy, width, height, angle1, angle2);
      
      tmp = radius;
      if(style == 0)
        {
          tmp1 = tmp - ticksize;
          tmp2 = tmp1 +(ticksize >>1);
        }
      else
        {
          tmp1 = tmp + ticksize;
          tmp2 = tmp1 -(ticksize >>1);
        }
      x = cx;
      y = cy;
      if(style) z=0; else z = EZ_DialTHeight(wptr);
      if(ticks2 > 0)
        {
          for(value=vstart, i = 0; i <= ticks2; i++)
            {
              int x1,y1,x2,y2;
              if( (i&1) ==0)
                {
                  x1 = x + (int) (tmp1 * cost[i]);
                  x2 = x + (int) (tmp  * cost[i]);
                  y1 = y - (int) (tmp1 * sint[i]);
                  y2 = y - (int) (tmp * sint[i]); 
                  if(showv>1 && i<=ticks2-islast)
                    {
                      int tx, ty;
                      (void) sprintf(str, EZ_DialFormat(wptr), value); 
                      value += dv;
                      tx = x1;
                      ty = y1+z;
                      if(x1 == x2) tx -= (EZ_DialTWidth(wptr)>>1);
                      else if(x1 < x2) tx -= (EZ_DialTWidth(wptr));
                      XDrawString(EZ_Display, pixmap, bgc, tx+1, ty+1, str, strlen(str));	  
                      XDrawString(EZ_Display, pixmap, lgc, tx-1, ty-1, str, strlen(str));	  
                      XDrawString(EZ_Display, pixmap, ngc, tx, ty, str, strlen(str));	  
                    }
                }
              else
                {
                  x1 = x + (int) (tmp2 * cost[i]);
                  x2 = x + (int) (tmp  * cost[i]);
                  y1 = y - (int) (tmp2 * sint[i]);
                  y2 = y - (int) (tmp * sint[i]);
                }
              XDrawLine(EZ_Display, pixmap, lgc, x1+1,y1+1,x2+1,y2+1);
              XDrawLine(EZ_Display, pixmap, bgc, x1-1,y1-1,x2-1,y2-1);
              XDrawLine(EZ_Display, pixmap, ngc, x1,y1,x2,y2);
            }
        }
      else if(showv > 1)
        {
          if(ticksize== 0) {if(style == 0) tmp1 -= 8; else tmp1 += 4;}
          for(value=vstart, i = 0; i <= showv-islast; i++)
            {
              int x1,y1,x2,y2;
              int tx, ty;
              x1 = x + (int) (tmp1 * cost[i]);
              x2 = x + (int) (tmp  * cost[i]);
              y1 = y - (int) (tmp1 * sint[i]);
              y2 = y - (int) (tmp * sint[i]); 
              (void) sprintf(str, EZ_DialFormat(wptr), value); 
              value += dv;
              tx = x1;
              ty = y1+z;
              if(x1 == x2) tx -= (EZ_DialTWidth(wptr)>>1);
              else if(x1 < x2) tx -= (EZ_DialTWidth(wptr));
              XDrawString(EZ_Display, pixmap, bgc, tx+1, ty+1, str, strlen(str));	  
              XDrawString(EZ_Display, pixmap, lgc, tx-1, ty-1, str, strlen(str));	  
              XDrawString(EZ_Display, pixmap, ngc, tx, ty, str, strlen(str));	  
            }
        }
      handle[0].x += 2; handle[0].y += 2;

      XFillPolygon(EZ_Display, pixmap, bgc,handle,4, Nonconvex,CoordModePrevious);
      handle[0].x -= 4; handle[0].y -= 4;
      XFillPolygon(EZ_Display, pixmap, lgc,handle,4, Nonconvex,CoordModePrevious);
      handle[0].x += 2; handle[0].y += 2;
      XFillPolygon(EZ_Display, pixmap, ngc,handle,4, Nonconvex,CoordModePrevious);
    }
  else /* enabled */
    {
      valuemask = GCForeground|GCFont|GCLineWidth;
      gcvalues.font = EZ_WidgetFont(wptr)->fid;
      gcvalues.line_width = 2;
      gc = EZ_WRITABLEGC;

      shadow = EZ_WidgetLabelShadow(wptr);
      shadowx = shadowy = shadow;
      if(shadow &0xf0)
        {
          shadowx = (shadow & 0xf0) >> 4;
          shadowy = (shadow & 0x0f);
        }
      if(shadowx & 8) shadowx= (8-shadowx);
      if(shadowy & 8) shadowy= (8-shadowy);
      if(1) 
        {
          width -= 4; height -= 4;
          xx += 2; yy += 2;
          gcvalues.foreground = EZ_ColorArray[EZ_DARK_GRAY2];
          XChangeGC(EZ_Display, gc, valuemask, &gcvalues);
          if(shadow && EZ_DialLabelLength(wptr)>0)
            {
              x = (w - EZ_DialLabelWidth(wptr))>>1;
              y = (bw+padb+pady)+EZ_DialAscent(wptr) + yoff;
              XDrawString(EZ_Display, pixmap, gc, x+shadowx, y+shadowy,
                          EZ_DialLabel(wptr), EZ_DialLabelLength(wptr));
            }
          x = xx;
          y = yy;
          XDrawArc(EZ_Display, pixmap, gc, x, y, width, height, angle1, angle2);
          valuemask = GCLineWidth;
          gcvalues.line_width = 0;
          XChangeGC(EZ_Display, gc, valuemask, &gcvalues);

          if(shadow)
            {
              if(EZ_DialSpan(wptr) >= 180.0)          
                {
                  shadowx *= 3; if(shadowx > 6) shadowx = 6;
                  shadowy *= 3; if(shadowy > 6) shadowy = 6;
                }
              handle[0].x += shadowx;
              handle[0].y += shadowy;
              XFillPolygon(EZ_Display, pixmap, gc,handle,4, Nonconvex,CoordModePrevious);
              handle[0].x -= shadowx;
              handle[0].y -= shadowy;
            }
          width += 4; height += 4;
          xx -= 2; yy -= 2;
        }  
      valuemask = GCForeground|GCFont|GCLineWidth;
      gcvalues.line_width = 2;
      gcvalues.foreground = EZ_WidgetForeground(wptr);
      XChangeGC(EZ_Display, gc, valuemask, &gcvalues);
      XDrawArc(EZ_Display, pixmap, gc, xx, yy, width, height, angle1, angle2);
      valuemask=GCLineWidth;
      gcvalues.line_width = 0;
      XChangeGC(EZ_Display, gc, valuemask, &gcvalues);  

      if(EZ_DialLabelLength(wptr) >0)
        {
          x = (w - EZ_DialLabelWidth(wptr))>>1;
          y = (bw+padb+pady)+EZ_DialAscent(wptr) + yoff;
          XDrawString(EZ_Display, pixmap, gc, x, y,
                      EZ_DialLabel(wptr), EZ_DialLabelLength(wptr));
        }
      tmp = radius;
      if(style == 0)
        {
          tmp1 = tmp - ticksize;
          tmp2 = tmp1 +(ticksize >>1);
        }
      else
        {
          tmp1 = tmp + ticksize;
          tmp2 = tmp1 -(ticksize >>1);
        }
      x = cx;
      y = cy;
      if(style) z=0; else z = EZ_DialTHeight(wptr);
      if(nticks > 0)
        {
          for(value=vstart, i = 0; ticks2 && i <= ticks2; i++)
            {
              int x1,y1,x2,y2;
              if( (i&1) ==0)
                {
                  x1 = x + (int) (tmp1 * cost[i]);
                  x2 = x + (int) (tmp  * cost[i]);
                  y1 = y - (int) (tmp1 * sint[i]);
                  y2 = y - (int) (tmp * sint[i]); 
                  if(showv>1 && i <=ticks2-islast)
                    {
                      int tx, ty;
                      (void) sprintf(str, EZ_DialFormat(wptr), value); 
                      value += dv;
                      tx = x1;
                      ty = y1+z;
                      if(x1 == x2) tx -= (EZ_DialTWidth(wptr)>>1);
                      else if(x1 < x2) tx -= (EZ_DialTWidth(wptr));
                      XDrawString(EZ_Display, pixmap, gc, tx, ty, str, strlen(str));	  
                    }
                }
              else
                {
                  x1 = x + (int) (tmp2 * cost[i]);
                  x2 = x + (int) (tmp  * cost[i]);
                  y1 = y - (int) (tmp2 * sint[i]);
                  y2 = y - (int) (tmp * sint[i]);
                }
              XDrawLine(EZ_Display, pixmap, gc, x1,y1,x2,y2);
            }
        }
      else if(showv > 1)
        {
          if(ticksize== 0) {if(style == 0) tmp1 -= 8; else tmp1 += 4;}
          for(value=vstart, i = 0; i <= showv-islast; i++)
            {
              int x1,y1,x2,y2;
              int tx, ty;
              x1 = x + (int) (tmp1 * cost[i]);
              x2 = x + (int) (tmp  * cost[i]);
              y1 = y - (int) (tmp1 * sint[i]);
              y2 = y - (int) (tmp * sint[i]); 

              (void) sprintf(str, EZ_DialFormat(wptr), value); 
              value += dv;
              tx = x1;
              ty = y1+z;
              if(x1 == x2) tx -= (EZ_DialTWidth(wptr)>>1);
              else if(x1 < x2) tx -= (EZ_DialTWidth(wptr));
              XDrawString(EZ_Display, pixmap, gc, tx, ty, str, strlen(str));	  
            }
        }
      /* display current dial value */
      if(nocv == 0)
        {
          if(EZ_DialTForeground(wptr))
            XSetForeground(EZ_Display, gc, EZ_DialTForeground(wptr));	
          tmp = EZ_DialTHeight(wptr);
          x = cx - (EZ_DialTWidth(wptr))/2;
          if(EZ_DialSpan(wptr)>=210.0)
            {
              int itmp = (int) (radius * sint[0]);
              y = cy - itmp/2 + tmp;
            }
          else if(EZ_DialSpan(wptr) < 150.0)
            {
              int itmp = (int) (radius * sint[0]);
              y = cy - itmp +tmp/2;
            }
          else y = cy - (radius-tmp)/2 + EZ_DialAscent(wptr);
          
          (void) sprintf(str, EZ_DialFormat(wptr), EZ_DialValue(wptr)); 
          XDrawString(EZ_Display, pixmap, gc, x, y, str, strlen(str));
        }
      /* draw the handle */
      if(EZ_DialForeground(wptr) != ~0)
        {
          bgpv = EZ_DialForeground(wptr);	
          XSetForeground(EZ_Display, gc, bgpv);
        }
      else if(EZ_WidgetBGPixmapB(wptr) != NULL)
        {
          EZ_Bitmap *bitmap = EZ_WidgetBGPixmapB(wptr);
          gc = EZ_NTILEGC2;
          XSetTile(EZ_Display,EZ_NTILEGC2,bitmap->pixmap);
          XSetTSOrigin(EZ_Display, EZ_NTILEGC2, cx, cy);
        }
      else if(EZ_WidgetParentBGPixmapB(wptr) != NULL)
        {
          EZ_Bitmap *bitmap = EZ_WidgetParentBGPixmapB(wptr);
          gc = EZ_NTILEGC2;
          XSetTile(EZ_Display,EZ_NTILEGC2,bitmap->pixmap);
          XSetTSOrigin(EZ_Display, EZ_NTILEGC2, cx, cy);
        }
      XFillPolygon(EZ_Display, pixmap, gc,handle,4, Nonconvex,CoordModePrevious);
    }
  EZ_DrawRectBorder(wptr, pixmap);     
  XCopyArea(EZ_Display,pixmap,win, EZ_WRITABLEGC,0,0,w,h,0,0); 
  XFreePixmap(EZ_Display, pixmap);   

}
/************************************************************/

void EZ_DialEventHandle(widget, event)
     EZ_Widget *widget; XEvent *event;
{
  int   btn, onbtn;
  float direc;
  if(event->type == Expose)   EZ_DrawWidget(widget);      
  if( EZ_GetWidgetDisabledFlag(widget) )  return;
  switch(event->type)
    {
    case ButtonPress:
      btn = event->xbutton.button;
      onbtn =0;
      direc = 0.0;
      if(btn != EZ_Btn3)
	{
          int     x,y,cx,cy, tx, ty, wox, woy, radius;
          float   dangle, ftmp;
          float   angle1, angle2;
          Window  junkwin;
          XEvent  xevent;
          x = event->xbutton.x;
          y = event->xbutton.y;
          angle1 = EZ_DialOffset(widget);
          angle2 = angle1 + EZ_DialSpan(widget);
          cx = EZ_DialCenterX(widget);
          cy = EZ_DialCenterY(widget);
          tx = x-cx; ty = cy-y;
          radius = EZ_DialRadius(widget);
          if(tx*tx + ty*ty >= radius *radius) return;
          if(abs(tx)>3 || abs(ty) > 3)
            {
              dangle = atan2((double)ty, (double)tx);
              if(dangle <= -HALFPI) dangle += TWOPI;
              ftmp = RADIAN2DEGREE(dangle);
              if(ftmp < angle1 || ftmp > angle2) return;
              direc = ftmp -  EZ_DialAngle(widget);
              if(fabs(direc) < 5.0) onbtn =1;
              else if(btn == EZ_Btn2 && ftmp != EZ_DialAngle(widget)) 
                {
                  EZ_DialAngle(widget) = ftmp;
                  EZ_DrawWidget(widget);
                  onbtn = 1;
                }
            }
          else return;

          XTranslateCoordinates(EZ_Display, EZ_WidgetWindow(widget),
                                RootWindow(EZ_Display, EZ_ScreenNum),
                                0, 0, &wox,&woy, &junkwin);
          if(onbtn)
            {
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
                  if(xevent.type == ButtonRelease && xevent.xbutton.button == btn)
                    {
                      { EZ_ExecuteWidgetCallBacks(widget);}
                      break;
                    }
                  /* not over yet */
                  {
                    int             rx,ry;
                    unsigned int    mask;
                    Window          root,win;
                    XQueryPointer(EZ_Display, RootWindow(EZ_Display, EZ_ScreenNum),
                                  &root, &win,  &rx, &ry, &x,&y, 
                                  &mask );
                    tx = rx-wox-cx; ty = cy-(ry-woy);
                    if(abs(tx)>3 || abs(ty) > 3)
                      {
                        dangle = atan2((double)ty, (double)tx);
                        if(dangle <= -HALFPI) dangle += TWOPI;
                        ftmp = RADIAN2DEGREE(dangle);
                        if(ftmp < angle1) ftmp = angle1;
                        if(ftmp > angle2) ftmp = angle2;
                        if(ftmp != EZ_DialAngle(widget)) 
                          {
                            EZ_CallBackStruct *callbacks = EZ_WidgetMotionCallBack(widget);
                            EZ_DialAngle(widget) = ftmp;
                            EZ_DrawWidget(widget);
                            if(callbacks) {EZ_HandleMotionCallBacks(widget,callbacks);}
                            else { EZ_ExecuteWidgetCallBacks(widget);}
                          }
                      }
                  }
                }
            }
          else /* not on button, scroll */
            {
              float angle, kacel = EZ_DialSpan(widget)/360.0;
              int done=0, first_time = 1;
              if(direc < 0.0) kacel = -kacel;
              
              while(!done)
                {
                  if(first_time)
                    {
                      first_time = 0;
                      XFlush(EZ_Display);
                      EZ_SitForALittleBit(100000); 
                    }
                  else EZ_SitForALittleBit(10000); 
                  angle = EZ_DialAngle(widget) + kacel;
                  if(angle < angle1) angle = angle1;
                  if(angle > angle2) angle = angle2;
                  if(angle != EZ_DialAngle(widget)) 
                    {
                      EZ_DialAngle(widget) = angle;
                      EZ_DrawWidget(widget);
                      { EZ_ExecuteWidgetCallBacks(widget);}
                    }
                  while(XPending(EZ_Display))
                    {
                      XNextEvent(EZ_Display, &xevent);
                      if(EZ_FilterEvent(&xevent)) EZ_InvokePrivateEventHandler(&xevent);
                      if(xevent.type == ButtonRelease && xevent.xbutton.button == btn) 
                        {
                          done = 1;
                          break;
                        }
                    }
                }
            }
        }
      else if(event->xbutton.button == EZ_Btn3)
	{
          /*
            #include "EZ_DnDHandler1.c"
	  EZ_DnDEventHandler1(widget, event);
          */
          EZ_HandleDnDEvents(widget, event);
	}
      break;
    case KeyPress:
      {
	char              tmpbuffer[8];
	int               buffersize = 8;
	KeySym            keysym;
	XComposeStatus    compose; 
	int count = XLookupString(&(event->xkey), tmpbuffer, buffersize, &keysym, &compose);
        float  angle,angle1, angle2, kacel, acel;
        kacel = EZ_DialSpan(widget)/360.0;
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
            angle1 = EZ_DialOffset(widget);
            angle2 = angle1 + EZ_DialSpan(widget);
            acel = - kacel;
	    if(event->xkey.state & ShiftMask) acel += acel;
	    if(event->xkey.state & Mod1Mask)  acel = acel * 4.0;
            angle = EZ_DialAngle(widget) + acel;
            if(angle < angle1) angle = angle1;
            if(angle > angle2) angle = angle2;
            if(angle != EZ_DialAngle(widget)) 
              {
                EZ_DialAngle(widget) = angle;
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
            angle1 = EZ_DialOffset(widget);
            angle2 = angle1 + EZ_DialSpan(widget);
            acel =  kacel;
	    if(event->xkey.state & ShiftMask) acel += acel;
	    if(event->xkey.state & Mod1Mask)  acel = acel * 4.0;
            angle = EZ_DialAngle(widget) + acel;
            if(angle < angle1) angle = angle1;
            if(angle > angle2) angle = angle2;            
            if(angle != EZ_DialAngle(widget)) 
              {
                EZ_DialAngle(widget) = angle;
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
    default:
      break;
    }
}
/************************************************************/
float EZ_GetDialValue(wptr) EZ_Widget *wptr;
{
  if(wptr && EZ_WidgetType(wptr) == EZ_WIDGET_DIAL)
    return(EZ_DialValue(wptr));
  return(0);
}

void EZ_SetDialValue(wptr, value) EZ_Widget *wptr; float value;
{
  if(wptr && EZ_WidgetType(wptr) == EZ_WIDGET_DIAL)
    {
      float mn, mx, tmp; 
      mn = EZ_DialFrom(wptr);
      mx = EZ_DialTo(wptr);
      if(mx < mn) {tmp=mx; mx=mn; mn=tmp;}
      if(value < mn) value = mn;
      else if(value>mx) value = mx;

      if(EZ_DialValue(wptr) != value)
        {
          EZ_DialValue(wptr) = value;
          EZ_DialVSet(wptr) = 1;
        }
      if(EZ_WidgetMapped(wptr)) EZ_DrawWidget(wptr);
    }
}

/************************************************************/     
#undef _EZ_WIDGET_DIAL_C_






