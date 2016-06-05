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
***              GradientBar                                  ***
***                                                           ***
*****************************************************************/
#define _EZ_WIDGET_GBAR_C_
#include "EZ_Widget.h"

/*********************************************************************
 * 
 *  Functions implemented in this file:
 */
EZ_Widget   *EZ_CreateGradientBar  MY_ANSIARGS((EZ_Widget *parent)); 
void         EZ_SetGradientBarValue  MY_ANSIARGS((EZ_Widget *widget, float value));
float        EZ_GetGradientBarValue  MY_ANSIARGS((EZ_Widget *widget));
void         EZ_SetGradientBarRange MY_ANSIARGS((EZ_Widget *widget, float f, float t));
void         EZ_SetGradientBarColors MY_ANSIARGS((EZ_Widget *widget, char **clrs, int cnt));
void         EZ_GetGradientBarColor  MY_ANSIARGS((EZ_Widget *widget, int *r, int *b, int *g));


Pixmap EZ_MakeGradientPixmap MY_ANSIARGS((int n, float x0, float x1, float y0, float y1,
                                          float vts[][2], float rgbs[][3], int kind));

/*********************************************************************/
void         EZ_GBarEventHandle  MY_ANSIARGS((EZ_Widget *widget, XEvent *event));
void         EZ_ComputeWidgetGBarSize  MY_ANSIARGS((EZ_Widget *widget, int *w, int *h));
void         EZ_DrawWidgetGBar   MY_ANSIARGS((EZ_Widget *widget));
void         EZ_FreeWidgetGBarData  MY_ANSIARGS((EZ_Widget *widget));
/*********************************************************************
 * 
 *  Local Variables.
 */
static EZ_WidgetHandle EZ_GBarHandle =
{
  EZ_ComputeWidgetGBarSize,
  EZ_DrawWidgetGBar,
  EZ_FreeWidgetGBarData,
  EZ_GBarEventHandle,
};

/*********************************************************************/

EZ_Widget  *EZ_CreateGradientBar(parent)
     EZ_Widget  *parent;     /* parent widget    */
{
  EZ_Widget  *wptr;
  
  wptr = EZ_CreateNewWidget(parent);
  EZ_WidgetHandlingFunctions[EZ_WIDGET_GRADIENT_BAR] = &EZ_GBarHandle;
  EZ_SetWidgetTypeAndNames(wptr, EZ_WIDGET_GRADIENT_BAR);
  EZ_WidgetBorderStyle(wptr) = EZ_BORDER_DOWN;
  EZ_WidgetBorderWidth(wptr) = 2;
  EZ_WidgetPadB(wptr) = 2; 
  
  EZ_GBarOrientation(wptr) = EZ_HORIZONTAL;
  EZ_GBarOrientationOld(wptr) = 123;
  EZ_GBarColors(wptr)[0] = EZ_ColorArray[GREEN];
  EZ_GBarColors(wptr)[1] = EZ_ColorArray[YELLOW];
  EZ_GBarNColors(wptr) = 2;
  EZ_GBarRange(wptr)[0] = 0.0;
  EZ_GBarRange(wptr)[1] = 1.0;
  EZ_GBarValue(wptr) = 0.5;
  EZ_GBarPixmap(wptr) = None;
  EZ_GBarPWidth(wptr)= 0;             
  EZ_GBarPHeight(wptr)= 0;
  EZ_GBarStyle(wptr) = 0;
  EZ_GBarEditable(wptr) = 1;
  EZ_GBarResolution(wptr) = -1.0;
  EZ_GBarBWidthSet(wptr)= 4;             
  
  EZ_SetWidgetFocusableFlag(wptr);  

  return(wptr);
}

/***********************************************************************/
void   EZ_FreeWidgetGBarData(widget)    EZ_Widget *widget;
{
  if(EZ_GBarPixmap(widget))
    {
      XFreePixmap(EZ_Display,EZ_GBarPixmap(widget));
      EZ_GBarPixmap(widget) = None;
    }
}
/***********************************************************************/
void EZ_ComputeWidgetGBarSize(widget, w, h)
    EZ_Widget *widget;
     int             *w, *h;
{
  int  length, height, bw, bw2, badjust=0, style;
  
  EZ_SetWidgetSizeComputedFlag(widget);
  EZ_WidgetPadX(widget) = 0;
  EZ_WidgetPadY(widget) = 0;
  EZ_WidgetSepX(widget) = 0;
  EZ_WidgetSepY(widget) = 0;
      
  bw = EZ_WidgetBorderWidth(widget) + EZ_WidgetPadB(widget);
  bw2 = bw+bw;
  style = EZ_GBarStyle(widget);
  if(style) 
    { 
      if(EZ_GBarBWidthSet(widget))
        {badjust = EZ_GBarBWidth(widget) = EZ_GBarBWidthSet(widget);}
      else
        {badjust = EZ_GBarBWidth(widget) = 4;}
    }
  else
    { badjust = EZ_GBarBWidth(widget) = 0; }

  if(EZ_GBarOrientation(widget) <= EZ_HORIZONTAL_CENTER)
    {
      height = 4;  length = 8 + (badjust << 3); 
    }
  else
    {
      length = 4; height = 8 + (badjust << 3); 
    }
  *w = bw2 + length;
  *h = bw2 + height;
}
/***********************************************************************
 *
 *  Draw A GBar.
 */
void  EZ_DrawWidgetGBar(wptr)
     EZ_Widget *wptr;
{
  int       sx, sy, dx, dy, w, h, padb, padb2, bw, sbw, bwpad, bwpad2;
  int       pw, ph, horiz, vw,vh, bwidth, bwidth2, style = EZ_GBarStyle(wptr);
  Pixmap    pixmap;
  Window    win;
  GC        gc;
  unsigned long bgpv;
  float    value, *range;
  
  win = EZ_WidgetWindow(wptr);
  w   = EZ_WidgetWidth(wptr);
  h   = EZ_WidgetHeight(wptr);
  padb = EZ_WidgetPadB(wptr);
  bw = EZ_WidgetBorderWidth(wptr);
  padb2 = padb + padb;

  pixmap = XCreatePixmap(EZ_Display, win, w, h, EZ_Depth); 
  EZ_FixFocusHighlightBorder(wptr, pixmap, w, h, padb);

  if(EZ_GetBackgroundGC(wptr, &gc, &bgpv, 0, 0))
    {
      int ox, oy;
      EZ_GetWidgetTileOrigin(wptr, &ox, &oy);      
      XSetTSOrigin(EZ_Display, gc, ox, oy);
    }
  XFillRectangle(EZ_DisplayForWidgets, pixmap, gc, padb,padb, w-padb2, h-padb2); 

  horiz = (EZ_GBarOrientation(wptr) <= EZ_HORIZONTAL_CENTER);  
  bwpad = bw + padb;
  bwpad2 = bwpad + bwpad;

  /* size of the gbar */
  pw = w - bwpad2;
  ph = h - bwpad2;
  bwidth = EZ_GBarBWidth(wptr);
  bwidth2 = bwidth>>1;
  range = EZ_GBarRange(wptr);
  value = EZ_GBarValue(wptr);
  /* recreate gbar if necessary */
  if(pw != EZ_GBarPWidth(wptr) || ph != EZ_GBarPHeight(wptr) || EZ_GBarPixmap(wptr) == None ||
     EZ_GBarOrientation(wptr) != EZ_GBarOrientationOld(wptr))
    {
      int i, j, ir, ig, ib, nclrs = EZ_GBarNColors(wptr), nclrs2 = nclrs+nclrs;
      float rgbs[30][3], vts[30][2], r,g,b, fpw, fph;
      EZ_GBarPWidth(wptr) = pw;
      EZ_GBarPHeight(wptr) = ph;
      EZ_GBarOrientationOld(wptr) = EZ_GBarOrientation(wptr);
      if(EZ_GBarPixmap(wptr))
        { XFreePixmap(EZ_Display,EZ_GBarPixmap(wptr)); EZ_GBarPixmap(wptr) = None;}

      fpw = (float)((pw + 7) & ~3);  fph = (float)((ph + 7) & ~3);

      if(horiz) /* horizontal */
        {
          float aa = 0.0, dd = (fpw)/(float)(nclrs-1);

          for(i = 0; i < nclrs2; i += 2)
            {
              vts[i][0] = aa;
              vts[i][1] = fph;
              vts[i+1][0] = aa;
              vts[i+1][1] = 0.0;
              aa += dd;
            }
          EZ_GBarFactor(wptr) = (float)(pw-bwidth)/(range[1] - range[0]);
        }
      else
        {
          float aa = 0.0, dd = (fph)/(float)(nclrs-1);
          for(i = 0; i < nclrs2; i+=2)
            {
              vts[i][0] = fpw;
              vts[i][1] = aa;
              vts[i+1][0] = 0.0;
              vts[i+1][1] = aa;
              aa += dd;
            }
          EZ_GBarFactor(wptr) = (float)(ph-bwidth)/(range[1] - range[0]);
        }
#define OneOver255 0.003921568627450980392156862745
      for(j=i = 0; j < nclrs; j++, i+=2)
        {
          EZ_PixelValue2RGB(EZ_GBarColors(wptr)[j], &ir,&ig, &ib);
          r = (float)ir*OneOver255; g = (float)ig*OneOver255; b = (float)ib*OneOver255;
          rgbs[i][0] = r; rgbs[i][1] = g; rgbs[i][2] = b;
          rgbs[i+1][0] = r; rgbs[i+1][1] = g; rgbs[i+1][2] = b;
        }
      EZ_GBarPixmap(wptr) = EZ_MakeGradientPixmap(nclrs+nclrs, 
                                                  0.0, fpw, 0.0, fph, 
                                                  vts, rgbs, EZ_QUAD_STRIP);
    }
  
  if(value < range[0]) value = range[0];
  if(value > range[1]) value = range[1];
  EZ_GBarValue(wptr) = value = EZ_FixSliderValue(value, EZ_GBarResolution(wptr));
  
  dx=dy=bwpad;    
  sbw = bw? bw : 1;
  if(style == 0)
    {
      if(horiz)
        {
          sx=sy=0;
          vw = (int) (EZ_GBarFactor(wptr)  * ( value  -range[0]));
          EZ_GBarPosition(wptr) = bwpad+vw + bwidth2;
          vh = ph;
        }
      else
        {
          vh =  (int) (EZ_GBarFactor(wptr)  * ( value -range[0]));
          EZ_GBarPosition(wptr) =ph - vh - bwidth2 + bwpad;
          vw = pw;
          sx=0;
          sy = ph - vh;
          dy += sy;
        }
      XCopyArea(EZ_Display,EZ_GBarPixmap(wptr), pixmap, gc,sx,sy,vw,vh,dx,dy);   
    }
  else
    {
      int tmp = EZ_GBarBWidth(wptr);
      
      XCopyArea(EZ_Display,EZ_GBarPixmap(wptr), pixmap, gc,0,0,pw,ph,bwpad,bwpad);   
      /* draw the slider bar */
      if(horiz)
        {
          vw = (int) (EZ_GBarFactor(wptr)  * (value  -range[0]));
          EZ_GBarPosition(wptr) = bwpad+vw + bwidth2;
          dx = bwpad + vw;
          dy = bwpad;

          if(style == 2)
            XFillRectangle(EZ_Display, pixmap, EZ_DRAGGC, 
                           dx,dy,tmp,ph);
          else if(style >= 3)
            {
              if(style > 3)
                {
                  if(EZ_WidgetBGPixmapB(wptr) != NULL)
                    {
                      EZ_Bitmap *bitmap = EZ_WidgetBGPixmapB(wptr);
                      gc = EZ_NTILEGC2;
                      XSetTile(EZ_Display,EZ_NTILEGC2,bitmap->pixmap);
                      XSetTSOrigin(EZ_Display, EZ_NTILEGC2, dx, dy);
                    }
                  else if(EZ_WidgetParentBGPixmapB(wptr) != NULL)
                    {
                      EZ_Bitmap *bitmap = EZ_WidgetParentBGPixmapB(wptr);
                      gc = EZ_NTILEGC2;
                      XSetTile(EZ_Display,EZ_NTILEGC2,bitmap->pixmap);
                      XSetTSOrigin(EZ_Display, EZ_NTILEGC2, dx, dy);
                    }
                  else
                    {
                      int tile;
                      tile  = EZ_GetBackgroundGC(wptr,&gc, &bgpv, 0, 0);
                      if(tile) 
                        {
                          int ox, oy;
                          EZ_GetWidgetTileOrigin(wptr, &ox, &oy);      
                          XSetTSOrigin(EZ_Display, gc, ox, oy);
                        }
                    }  
                  XFillRectangle(EZ_Display, pixmap, gc, dx,dy, tmp, ph);
                }
              EZ_DrawRectBorderWithSize(wptr, pixmap, 
                                        dx,
                                        dy,
                                        tmp,
                                        ph,
                                        sbw,
                                        EZ_BORDER_UP, dx, dy, 1);
            }
          else
            XDrawRectangle(EZ_Display, pixmap, EZ_DRAGGC, 
                           dx+1,dy+1,tmp,ph-2);

        }
      else /* vertical */
        {
          vh =  (int) (EZ_GBarFactor(wptr)  * ( value -range[0]));
          EZ_GBarPosition(wptr) =ph - vh - bwidth2 + bwpad;
          dx = bwpad;
          dy = bwpad + ph-vh -bwidth;
          if(style == 2)
            XFillRectangle(EZ_Display, pixmap, EZ_DRAGGC, 
                           dx,dy,pw, tmp);
          else if(style >= 3)
            {
              if(style > 3)
                {
                  if(EZ_WidgetBGPixmapB(wptr) != NULL)
                    {
                      EZ_Bitmap *bitmap = EZ_WidgetBGPixmapB(wptr);
                      gc = EZ_NTILEGC2;
                      XSetTile(EZ_Display,EZ_NTILEGC2,bitmap->pixmap);
                      XSetTSOrigin(EZ_Display, EZ_NTILEGC2, dx, dy);
                    }
                  else if(EZ_WidgetParentBGPixmapB(wptr) != NULL)
                    {
                      EZ_Bitmap *bitmap = EZ_WidgetParentBGPixmapB(wptr);
                      gc = EZ_NTILEGC2;
                      XSetTile(EZ_Display,EZ_NTILEGC2,bitmap->pixmap);
                      XSetTSOrigin(EZ_Display, EZ_NTILEGC2, dx, dy);
                    }
                  else
                    {
                      int tile;
                      tile  = EZ_GetBackgroundGC(wptr,&gc, &bgpv, 0, 0);
                      if(tile) 
                        {
                          int ox, oy;
                          EZ_GetWidgetTileOrigin(wptr, &ox, &oy);      
                          XSetTSOrigin(EZ_Display, gc, ox, oy);
                        }
                    }  
                  XFillRectangle(EZ_Display, pixmap, gc, dx,dy, pw, tmp);
                }
              EZ_DrawRectBorderWithSize(wptr, pixmap, 
                                        dx,
                                        dy,
                                        pw,
                                        tmp,
                                        sbw,
                                        EZ_BORDER_UP, dx, dy, 1);
            }
          else
            XDrawRectangle(EZ_Display, pixmap, EZ_DRAGGC, 
                           dx+1,dy+1,pw-2, tmp);
        }
    }
  EZ_DrawRectBorder(wptr, pixmap);     
  XCopyArea(EZ_Display,pixmap,win, EZ_WRITABLEGC,0,0,w,h,0,0); 
  XFreePixmap(EZ_Display, pixmap); 
}

/*********************************************************************************/
void EZ_GBarEventHandle(widget, event)
     EZ_Widget *widget;    XEvent *event;
{
  if(event->type == Expose) { EZ_DrawWidget(widget);  return; }
  if(event->type == ButtonPress &&  event->xbutton.button == EZ_Btn3)
    { EZ_HandleDnDEvents(widget, event); return; }

  /* behave like a slider */
  if(EZ_GBarEditable(widget))
    {
      int horiz = (EZ_GBarOrientation(widget) <= EZ_HORIZONTAL_CENTER);
      int wadj, bwidth = EZ_GBarBWidth(widget);
      int bwpad = EZ_WidgetBorderWidth(widget) + EZ_WidgetPadB(widget);
      int w = EZ_WidgetWidth(widget);
      int h = EZ_WidgetHeight(widget);
      int     xy0, xy1, xy;
      float factor = EZ_GBarFactor(widget);
      
      xy0 = bwpad;
      if(horiz) {xy1 = w - bwpad;} else { xy1 = h - bwpad;}
      if(bwidth < 4) {  wadj = 4; } else { wadj = bwidth;}
      switch(event->type)
        {
        case ButtonPress:
          {
            int button, x,y, onbutton, inwindow;

            XEvent  xevent;

            if(!horiz) { factor = -factor;}
            button = event->xbutton.button;
            x = event->xbutton.x;
            y = event->xbutton.y;
            onbutton = 0;
            inwindow = 0;
            
            xy = horiz? x : y;
            if(xy >= EZ_GBarPosition(widget) - wadj &&
               xy <= EZ_GBarPosition(widget) + wadj)  onbutton = 1;
            else if(xy > xy0 && xy < xy1) 
              {
                inwindow = 1;
                if(button == EZ_Btn2) /* move slider to pointer position */
                  {
                    int yy = xy - EZ_GBarPosition(widget);
                    if(yy) 
                      {
                        float delta = (float)yy / factor;
                        EZ_GBarValue(widget) += delta;
                        EZ_DrawWidget(widget);
                      }
                    onbutton = 1;
                  }
              }
            if(onbutton) /* sliding ... */
              {
                int wox, woy;
                Window junkwin;
                XTranslateCoordinates(EZ_Display, EZ_WidgetWindow(widget),
                                      RootWindow(EZ_Display, EZ_ScreenNum),
                                      0, 0,
                                      &wox,&woy, &junkwin);
                while(1)
                  {
                    XNextEvent(EZ_Display, &xevent);
                    if(EZ_FilterEvent(&xevent)) EZ_InvokePrivateEventHandler(&xevent);
                    if(xevent.type == Expose || xevent.type == FocusIn || xevent.type == FocusOut) 
                      EZ_WidgetDispatchEvent(&xevent);
                    while(XPending(EZ_Display))
                      {
                        if(xevent.type == ButtonRelease && xevent.xbutton.button == button) break;
                        XNextEvent(EZ_Display, &xevent);
                        if(EZ_FilterEvent(&xevent)) EZ_InvokePrivateEventHandler(&xevent);
                        if(xevent.type == Expose || xevent.type == FocusIn || xevent.type == FocusOut) 
                          EZ_WidgetDispatchEvent(&xevent); 
                      }
                    if(xevent.type == ButtonRelease && xevent.xbutton.button == button)
                      {
                        EZ_ExecuteWidgetCallBacks(widget);
                        break;
                      }
                    else 
                      {
                        int             rx,ry,x,y;
                        unsigned int    mask;
                        Window          root,win;
                        XQueryPointer(EZ_Display, RootWindow(EZ_Display, EZ_ScreenNum),
                                      &root,
                                      &win, 
                                      &rx, &ry, 
                                      &x,&y, 
                                      &mask );
                        if(horiz) xy = rx - wox; else xy = ry - woy;
                        if(xy > xy0 && xy < xy1) 
                          {
                            int yy = xy - EZ_GBarPosition(widget);
                            if(yy)
                              {
                                float delta = (float)yy / factor;
                                EZ_GBarValue(widget) += delta;
                                EZ_DrawWidget(widget);
                                {
                                  EZ_CallBackStruct *callbacks = EZ_WidgetMotionCallBack(widget);
                                  if(callbacks) {EZ_HandleMotionCallBacks(widget,callbacks);}
                                  else { EZ_ExecuteWidgetCallBacks(widget);}
                                }
                              }
                          }
                      }
                  }  /* while */
              }
            else if(inwindow)
              {
                /* must be Button1 and the press is not on slider */
                /* slide toward pointer */
                int   first_time = 1, done = 0;
                int   pos = EZ_GBarPosition(widget);
                int   m_unit,iincr =(xy > pos)? 1: -1;
                if(event->xbutton.state & ShiftMask) m_unit =  iincr * 4;
                else if(event->xbutton.state & ControlMask)  m_unit = iincr * 2;
                else m_unit = iincr;
                    
                while(!done) /* wait for a matching ButtonRelease */
                  { 
                    float vl = EZ_GBarValue(widget);
                    
                    if((iincr > 0 && pos < xy1) || (iincr < 0 && pos > xy0))
                      {
                        EZ_GBarValue(widget) += (float) m_unit / factor;
                        EZ_DrawWidget(widget); 
                        if(vl != EZ_GBarValue(widget))
                          { EZ_ExecuteWidgetCallBacks(widget);}
                      }
                    if(first_time)
                      {
                        first_time = 0;
                        XFlush(EZ_Display);
                        EZ_SitForALittleBit(300000); 
                      }
                    else EZ_SitForALittleBit(30000); 

                    while(XPending(EZ_Display))
                      {
                        XNextEvent(EZ_Display, &xevent);
                        if(EZ_FilterEvent(&xevent))  EZ_InvokePrivateEventHandler(&xevent);
                        if(xevent.type == ButtonRelease && xevent.xbutton.button == button)
                          {  { EZ_ExecuteWidgetCallBacks(widget);} done = 1; break;}
                      }
                  }	
              }
          }
          break;
        case KeyPress:
          {
            char              tmpbuffer[8];
            int               buffersize = 8, move_unit=0;
            KeySym            keysym;
            XComposeStatus    compose; 
            int count = XLookupString(&(event->xkey), tmpbuffer, buffersize, &keysym, &compose);
            tmpbuffer[count] = '\0';
            switch(keysym)
              {
              case XK_Down: case XK_n: case XK_j: case XK_N: case XK_J: 
              case XK_Left: case XK_b:  case XK_h: case XK_B:  case XK_H:  
#ifdef XK_KP_Down
              case XK_KP_Left:
              case XK_KP_Down:
#endif
                move_unit = -1; 
                break;
              case XK_Right: case XK_f: case XK_l: case XK_F: case XK_L:  
              case XK_Up: case XK_k:  case XK_p: case XK_K:  case XK_P: 
#ifdef XK_KP_Up
              case XK_KP_Right:
              case XK_KP_Up:
#endif
                move_unit = 1;
                break;
              default:
                break;
              }
            if(move_unit)
              {
                int pos = EZ_GBarPosition(widget) ;
                if(event->xkey.state & ShiftMask)   move_unit *= 4;
                if(event->xkey.state & ControlMask) move_unit *= 2;
                if((move_unit > 0 && pos <= xy1) || (move_unit < 0 && pos >= xy0))
                  {
                    float vl = EZ_GBarValue(widget);
                    EZ_GBarValue(widget) += (float) move_unit / factor;
                    EZ_DrawWidget(widget); 
                    if(vl != EZ_GBarValue(widget))
                      { EZ_ExecuteWidgetCallBacks(widget);}
                  }
              }
          }
          {
            XEvent tmpevent; 
            while(XCheckTypedEvent(EZ_Display, KeyPress, &tmpevent))
              if(EZ_FilterEvent(&tmpevent))
                EZ_InvokePrivateEventHandler(&tmpevent);
          }
          break;
        default: 
          break;
        }
    }
}
/*******************************************************************/

void  EZ_SetGradientBarValue(widget, value)
     EZ_Widget *widget; float value;
{
  if(widget && EZ_WidgetType(widget) == EZ_WIDGET_GRADIENT_BAR)
    {
      EZ_GBarValue(widget) = value;
      if(EZ_WidgetMapped(widget)) EZ_DrawWidget(widget);
    }
}
float EZ_GetGradientBarValue(widget)
     EZ_Widget *widget;
{
  if(widget && EZ_WidgetType(widget) == EZ_WIDGET_GRADIENT_BAR)
    {
      return(EZ_GBarValue(widget));
    }
  return(0.0);
}
void  EZ_SetGradientBarRange (widget, frm, to)
     EZ_Widget *widget; float frm, to;
{
  if(widget && EZ_WidgetType(widget) == EZ_WIDGET_GRADIENT_BAR)
    {
      float *range = EZ_GBarRange(widget);
      range[0] = frm; range[1] = to;
      if(EZ_WidgetMapped(widget)) EZ_DrawWidget(widget);
    }
}
void EZ_SetGradientBarColors(widget, colors, ncolors)
     EZ_Widget *widget; char **colors; int ncolors;
{
  if(widget && EZ_WidgetType(widget) == EZ_WIDGET_GRADIENT_BAR)
    {
      unsigned long *pvs = EZ_GBarColors(widget);
      int i, cnt = (ncolors < 12 ? ncolors: 12);
      for(i = 0; i < cnt; i++)
        if(colors && colors[i]) pvs[i] = EZ_AllocateColorFromName(colors[i]);          
      EZ_GBarNColors(widget) = cnt;
    }
}
/**********************************************************************************/

Pixmap EZ_MakeGradientPixmap(n,pw0, pw, ph0, ph,vts,rgbs, kind)
     int n, kind; 
     float vts[][2], rgbs[][3];
     float pw0, pw, ph0, ph;
{
  int i, width, height;
  Pixmap  pixmap;
  EZ_Widget *canvas;
  width = (int)(pw - pw0);
  height = (int)(ph - ph0);
  canvas = EZ_CreateWidget(EZ_WIDGET_3D_CANVAS, NULL,
                           EZ_SIZE,   width,  height,
                           EZ_OVERRIDE_REDIRECT, 1,
                           EZ_LOCATION, (-width-10), 0,
                           0);
  EZ_DisplayWidget(canvas);         
  
  EZ_RGBMode();                 /* we are using RGB mode  */
  EZ_SetBackBuffer(EZ_PIXMAP);  /* use a pixmap backbuffer*/
  EZ_ShadeModel(EZ_SMOOTH);     /* turn on smooth shading */
  EZ_LoadIdentity();            /* clear the top mtx 2 id */     
  EZ_Ortho2(pw0, pw, ph0, ph);

  EZ_DrawBuffer(EZ_BACK);  
  /* now draw the bar */
  EZ_Clear(EZ_COLOR_BUFFER_BIT); /* clear the frame buffer */
  EZ_Begin(kind);

  for(i = 0; i < n; i++)
    {
      EZ_Color3fv(rgbs[i]);
      EZ_Vertex2fv(vts[i]);
    }
  EZ_End();  

  pixmap = XCreatePixmap(EZ_Display, EZ_DummyWindow, pw, ph, EZ_Depth); 
  XCopyArea(EZ_Display, EZ_Pixmap3,  pixmap, EZ_MainGC, 0, 0, width, height, 0, 0);
  EZ_DestroyWidget(canvas); 
  return(pixmap);
}
/**********************************************************************************/
void EZ_GetGradientBarColor(wptr, rr, gr, br)
     EZ_Widget *wptr; int *rr, *br, *gr;
{
  int nclrs = EZ_GBarNColors(wptr);
  if(nclrs == 1)
    {
      EZ_PixelValue2RGB(EZ_GBarColors(wptr)[0], rr,gr, br);
    }
  else
    {
      float dv, df, gv, factor, value = EZ_GBarValue(wptr);
      float *range = EZ_GBarRange(wptr);
      int i, r0,g0,b0,r1,g1,b1;
      dv = (range[1] - range[0])/(float)(nclrs - 1);
      df = range[0];
      for(i = 0; i < nclrs-1; i++)
        {
          if(value >= df && value <= df +dv) break;
          df += dv;
        }
      EZ_PixelValue2RGB(EZ_GBarColors(wptr)[i], &r0,&g0, &b0);
      EZ_PixelValue2RGB(EZ_GBarColors(wptr)[i+1], &r1,&g1, &b1);
      factor = (value - df)/dv;
      gv = 0.5+(float)r0 + factor * (float)(r1-r0);
      *rr = (int)gv;
      gv = 0.5+(float)g0 + factor * (float)(g1-g0);
      *gr = (int)gv;
      gv = 0.5+(float)b0 + factor * (float)(b1-b0);
      *br = (int)gv;      
    }
}      
/**********************************************************************************/
#undef _EZ_WIDGET_GBAR_C_
