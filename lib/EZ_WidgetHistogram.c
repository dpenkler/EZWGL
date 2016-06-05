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
***              histogram                                    ***
***                                                           ***
*****************************************************************/
#define _EZ_WIDGET_HISTOGRAM_C_
#include "EZ_Widget.h"

/*********************************************************************
 * 
 *  Functions implemented in this file:
 */
EZ_Widget   *EZ_CreateHistogram  MY_ANSIARGS((EZ_Widget *parent)); 
void         EZ_SetHistogramRange MY_ANSIARGS((EZ_Widget *widget, float f, float t));
void         EZ_SetHistogramValues  MY_ANSIARGS((EZ_Widget *widget, float *values, int n));
void         EZ_SetHistogramValue  MY_ANSIARGS((EZ_Widget *widget, float value));
float        EZ_GetHistogramValue  MY_ANSIARGS((EZ_Widget *widget, int n));
int          EZ_GetHistogramSelection  MY_ANSIARGS((EZ_Widget *widget, float *vret));
void         EZ_SetHistogramColors  MY_ANSIARGS((EZ_Widget *widget, unsigned long *pvs, int n));
/*********************************************************************/
void         EZ_HgramEventHandle  MY_ANSIARGS((EZ_Widget *widget, XEvent *event));
void         EZ_ComputeWidgetHgramSize  MY_ANSIARGS((EZ_Widget *widget, int *w, int *h));
void         EZ_DrawWidgetHgram   MY_ANSIARGS((EZ_Widget *widget));
void         EZ_FreeWidgetHgramData  MY_ANSIARGS((EZ_Widget *widget));
/*********************************************************************
 * 
 *  Local Variables.
 */
static EZ_WidgetHandle EZ_HgramHandle =
{
  EZ_ComputeWidgetHgramSize,
  EZ_DrawWidgetHgram,
  EZ_FreeWidgetHgramData,
  EZ_HgramEventHandle,
};

/*********************************************************************/

EZ_Widget  *EZ_CreateHistogram(parent)
     EZ_Widget  *parent;     /* parent widget    */
{
  EZ_Widget  *wptr;
  int i;
  unsigned long *clr;
  float  *fptr;
  
  wptr = EZ_CreateNewWidget(parent);
  EZ_WidgetHandlingFunctions[EZ_WIDGET_HISTOGRAM] = &EZ_HgramHandle;
  EZ_SetWidgetTypeAndNames(wptr, EZ_WIDGET_HISTOGRAM);
  EZ_WidgetBorderStyle(wptr) = EZ_BORDER_DOWN;
  EZ_WidgetBorderWidth(wptr) = 2;
  EZ_WidgetPadB(wptr) = 0; 
  EZ_WidgetSepX(wptr) = 0;
  EZ_WidgetSepY(wptr) = 0;
  EZ_WidgetPadX(wptr) = 0;
  EZ_WidgetPadY(wptr) = 0;

  EZ_HgramOrientation(wptr) = EZ_VERTICAL;
  EZ_HgramOrientationOld(wptr) = 123;
  EZ_HgramColors(wptr) = clr =
    (unsigned long *)my_malloc((256) * sizeof(unsigned long), _HOSTOGRAM_);
  for(i = 0; i < 256; i++) clr[i] = EZ_DefaultForeground;
  EZ_HgramNColors(wptr) = 0;
  EZ_HgramNColorCells(wptr) = 256;
  EZ_HgramRange(wptr)[0] = 0.0;
  EZ_HgramRange(wptr)[1] = 100.0;
  EZ_HgramValues(wptr) = fptr = (float *) my_malloc( 10 * sizeof(float), _HOSTOGRAM_);
  for(i = 0; i < 10; i++) fptr[i] =(float)( (rand()>> 5) %100);
  EZ_HgramNValues(wptr) = 10;

  EZ_HgramNBins(wptr) = 10;
  EZ_HgramCBin(wptr) = 0;
  EZ_HgramBinWidth(wptr) = 1;

  return(wptr);
}

/***********************************************************************/
void EZ_FreeWidgetHgramData(widget)    EZ_Widget *widget;
{
  my_free(EZ_HgramColors(widget));
  EZ_HgramColors(widget) = NULL;
  if(EZ_HgramValues(widget))
    my_free((char *) EZ_HgramValues(widget));
}
/***********************************************************************/
void EZ_ComputeWidgetHgramSize(widget, w, h)
    EZ_Widget *widget;
     int             *w, *h;
{
  int  sep, width, height, bw, bw2, nbins, binwidth;
  int padx, pady, vert;

  EZ_SetWidgetSizeComputedFlag(widget);

  padx = EZ_WidgetPadX(widget);
  pady = EZ_WidgetPadY(widget);
      
  bw = EZ_WidgetBorderWidth(widget);
  bw2 = bw+bw;
  nbins = EZ_HgramNBins(widget);
  binwidth = EZ_HgramBinWidth(widget);
  vert = EZ_HgramOrientation(widget);
  vert = (vert >= EZ_VERTICAL);
  
  width = height = 0;

  if(nbins < 0) nbins = 8;
  if(vert)
    {
      sep = EZ_WidgetSepX(widget);
      width = nbins * (binwidth + sep) - sep;
      height = 8;
    }
  else
    {
      sep = EZ_WidgetSepY(widget);
      height = nbins * (binwidth + sep) - sep;
      width = 8;
    }
  padx = padx+padx;
  pady = pady+pady;
  *w = bw2 + padx + width;
  *h = bw2 + pady + height;
}

/***********************************************************************/
static void checkHistogramStorage(wptr, nbins)
     EZ_Widget *wptr; int nbins;
{
  if(EZ_HgramNValues(wptr) < nbins)
    {
      float *n, *old = EZ_HgramValues(wptr);
      n = (float *)my_malloc( (nbins+2) * sizeof(float), _HOSTOGRAM_);
      memset(n, 0, (nbins+2) * sizeof(float));
      memcpy(n, old, EZ_HgramNValues(wptr) * sizeof(float));
      if(old) my_free((char *)old);
      EZ_HgramValues(wptr) = n;
      EZ_HgramCBin(wptr) = 0;
      EZ_HgramNValues(wptr) = nbins;      
    }
}
/***********************************************************************
 *
 *  Draw A Hgram.
 */
void  EZ_DrawWidgetHgram(wptr)
     EZ_Widget *wptr;
{
  int       w, h, bw, bw2, pw, ph, orien, incr, binwidth, binsep;
  int       nbins, vert, padx, pady, padx2, pady2, ncolors,itmp;
  int       xx, yy, i, j, offset = 0;
  float     *range, *values, ftmp, minvalue, maxvalue, value,factor;
  Pixmap    pixmap;
  Window    win;
  GC        gc;
  unsigned long bgpv;
  
  win = EZ_WidgetWindow(wptr);
  w   = EZ_WidgetWidth(wptr);
  h   = EZ_WidgetHeight(wptr);
  bw = EZ_WidgetBorderWidth(wptr);
  padx = EZ_WidgetPadX(wptr);
  pady = EZ_WidgetPadY(wptr);
  bw2 = bw+bw;
  padx2 = padx+padx;
  pady2 = pady+pady;
  binsep = 0;

  orien =EZ_HgramOrientation(wptr); 
  vert = (orien >= EZ_VERTICAL);
  nbins = EZ_HgramNBins(wptr);
  pw = w - (bw2 + padx2);
  ph = h - (bw2 + pady2);
  incr = 1;

  if(vert) { itmp = pw; binsep = EZ_WidgetSepX(wptr);}
  else  { itmp = ph;  binsep = EZ_WidgetSepY(wptr);}
  if(nbins > 0)
    {
      if(itmp > nbins)
        {
          int jtmp = itmp/nbins;
          if(jtmp > binsep) binwidth = jtmp - binsep;
          else { binwidth = jtmp; binsep = 0;}
          offset = (itmp - nbins * jtmp)>>1;
        }
      else
        {
          binwidth = 1;
          incr = nbins/itmp;
        }
    }
  else
    {
      binsep = 0;
      binwidth = 1;
      nbins = itmp-1;
    }
  /* make sure storage is ok */
  checkHistogramStorage(wptr, nbins);
  
  ncolors = EZ_HgramNColors(wptr);

  pixmap = XCreatePixmap(EZ_Display, win, w, h, EZ_Depth); 
  if(EZ_GetBackgroundGC(wptr, &gc, &bgpv, 0, 0))
    {
      int ox, oy;
      EZ_GetWidgetTileOrigin(wptr, &ox, &oy);      
      XSetTSOrigin(EZ_Display, gc, ox, oy);
    }
  XFillRectangle(EZ_DisplayForWidgets, pixmap, gc, 0,0, w, h); 

  range = EZ_HgramRange(wptr);
  minvalue = range[0];
  maxvalue = range[1];
  values = EZ_HgramValues(wptr);

  /* to convert to screen coor */
  if(vert) { factor = (float) ph/(maxvalue - minvalue);}
  else { factor =(float)pw/(maxvalue - minvalue);}

  gc = EZ_WRITABLEGC;

  if(ncolors > 0)
    {
      unsigned long *clrs = EZ_HgramColors(wptr);
      if(vert)
        {
          int tpadx = bw+padx;
          int tpady = bw+pady;
          int iicc = (binwidth+binsep);
          for(xx=offset+tpadx, j=0, i = 0; i < nbins; i += incr, j++, xx += iicc)
            {
              if(j >= ncolors) j = 0;
              XSetForeground(EZ_Display, gc,clrs[j]);
              value = values[i]; 
              if(value < minvalue)value=minvalue;
              else if(value >maxvalue) value=maxvalue;
              ftmp = (value - minvalue) * factor;
              itmp = (int)ftmp;
              XFillRectangle(EZ_Display, pixmap, gc, xx, tpady+ph-itmp, binwidth, itmp);
            }
        }
      else
        {
          int tpadx = bw+padx;
          int tpady = bw+pady;
          int iicc = (binwidth+binsep);
          for(yy=iicc+offset, j=0, i = 0; i < nbins; i += incr, j++, yy += iicc)
            {
              if(j >= ncolors) j = 0;
              XSetForeground(EZ_Display, gc,clrs[j]);
              value = values[i]; 
              if(value < minvalue)value=minvalue;
              else if(value >maxvalue) value=maxvalue;
              ftmp = (value - minvalue) * factor;
              itmp = (int)ftmp;
              XFillRectangle(EZ_Display, pixmap, gc, tpadx, tpady+ph-yy, itmp,binwidth);
            }
        }

    }
  else
    {
      XSetForeground(EZ_Display, gc, EZ_WidgetForeground(wptr));
      if(vert)
        {
          int tpadx = bw+padx;
          int tpady = bw+pady;
          int iicc = (binwidth+binsep);
          for(xx=tpadx+offset, i = 0; i < nbins; i += incr, xx += iicc)
            {
              value = values[i]; 
              if(value < minvalue)value=minvalue;
              else if(value >maxvalue) value=maxvalue;
              ftmp = (value - minvalue) * factor;
              itmp = (int)ftmp;
              XFillRectangle(EZ_Display, pixmap, gc, xx, tpady+ph-itmp, binwidth, itmp);
            }
        }
      else
        {
          int tpadx = bw+padx;
          int tpady = bw+pady;
          int iicc = (binwidth+binsep);
          for(yy=iicc+offset, i = 0; i < nbins; i += incr, yy += iicc)
            {
              value = values[i]; 
              if(value < minvalue)value=minvalue;
              else if(value >maxvalue) value=maxvalue;
              ftmp = (value - minvalue) * factor;
              itmp = (int)ftmp;
              XFillRectangle(EZ_Display, pixmap, gc, tpadx, tpady+ph-yy, itmp,binwidth);
            }
        }
    }
  EZ_HgramNBinsUsed(wptr) = nbins;
  EZ_HgramBinWidthUsed(wptr) = binwidth;
  EZ_HgramBinSep(wptr) = binsep;
  EZ_DrawRectBorder(wptr, pixmap);     
  XCopyArea(EZ_Display,pixmap,win, EZ_WRITABLEGC,0,0,w,h,0,0); 
  XFreePixmap(EZ_Display, pixmap); 
}

/*********************************************************************************/
void EZ_HgramEventHandle(widget, event)
     EZ_Widget *widget;    XEvent *event;
{
  if(event->type == Expose) { EZ_DrawWidget(widget);  return; }
  if(event->type == ButtonPress &&  event->xbutton.button == EZ_Btn3)
    { EZ_HandleDnDEvents(widget, event); return; }

  if(EZ_HgramNBins(widget)>0)
    {
      if(event->type == ButtonPress)
        {
          int hh = EZ_WidgetHeight(widget);
          int bw = EZ_WidgetBorderWidth(widget);
          int padx = EZ_WidgetPadX(widget);
          int pady = EZ_WidgetPadX(widget);
          int binsep = EZ_HgramBinSep(widget);
          int orien =EZ_HgramOrientation(widget); 
          int vert = (orien >= EZ_VERTICAL);
          int nbinsUsed = EZ_HgramNBinsUsed(widget);
          int binWidthUsed = EZ_HgramBinWidthUsed(widget);
          XEvent  xevent;
          int button = event->xbutton.button;
          int x = event->xbutton.x;
          int y = event->xbutton.y;
          int cbin;

          if(vert) cbin = (x - bw -padx)/(binWidthUsed + binsep);
          else  cbin = (hh-y - bw -pady)/(binWidthUsed + binsep);
          if(cbin < 0 || cbin >= nbinsUsed) return;
          
          EZ_HgramCBin(widget) = cbin;
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
                  XQueryPointer(EZ_Display, EZ_WidgetWindow(widget),
                                &root, &win, 
                                &rx, &ry, 
                                &x,&y, 
                                &mask );
                  if(vert) cbin = (x - bw -padx)/(binWidthUsed + binsep);
                  else  cbin = (hh-y - bw -pady)/(binWidthUsed + binsep);
                  if(cbin >= 0 && cbin < nbinsUsed)
                    {
                      int last = EZ_HgramSelection(widget);
                      EZ_HgramCBin(widget) = cbin;
                      if(last != cbin)
                        {
                          EZ_CallBackStruct *callbacks;
                          EZ_HgramSelection(widget) = cbin;
                          callbacks = EZ_WidgetMotionCallBack(widget);
                          if(callbacks) {EZ_HandleMotionCallBacks(widget,callbacks);}
                          else { EZ_ExecuteWidgetCallBacks(widget);}
                        }
                    }
                }
            }   /* while */
        }
    }
}
/**********************************************************************************/
void  EZ_SetHistogramValues(widget, values, n)
     EZ_Widget *widget; float *values; int n;
{
  if(widget && EZ_WidgetType(widget) == EZ_WIDGET_HISTOGRAM)
    {
      int i;
      float *vv;
      checkHistogramStorage(widget, n);
      vv = EZ_HgramValues(widget);
      for(i = 0; i < n; i++)
        vv[i] = values[i]; 
      EZ_HgramNBins(widget) = n;
      if(EZ_WidgetMapped(widget)) EZ_DrawWidget(widget);
    }
}

void  EZ_SetHistogramValue(widget, value)
     EZ_Widget *widget; float value;
{
  if(widget && EZ_WidgetType(widget) == EZ_WIDGET_HISTOGRAM)
    {
      int i, n ;
      float *vv;
      n = EZ_HgramNBins(widget);
      if(n <= 0) n = EZ_HgramNBinsUsed(widget);
      if(n <= 0) n = EZ_HgramNValues(widget);
      checkHistogramStorage(widget, n);
      vv = EZ_HgramValues(widget);
      for(i = n-1; i > 0; i--)
        vv[i] = vv[i-1];
      vv[0] = value;
      if(EZ_WidgetMapped(widget) && EZ_GetWidgetFreezedFlag(widget)==0)
        EZ_DrawWidget(widget);
    }
}

float EZ_GetHistogramValue(widget, n)
     EZ_Widget *widget; int n;
{
  if(widget && EZ_WidgetType(widget) == EZ_WIDGET_HISTOGRAM)
    {
      int nv = EZ_HgramNValues(widget);      
      if( n >= 0 && n < nv)
        {
          float *vv = EZ_HgramValues(widget);
          return(vv[n]);
        }
    }
  return(0.0);
}

int EZ_GetHistogramSelection(widget, vv) EZ_Widget *widget; float *vv;
{
  if(widget && EZ_WidgetType(widget) == EZ_WIDGET_HISTOGRAM)
    {
      int cbin = EZ_HgramCBin(widget);
      int n = EZ_HgramNBins(widget);
      if(vv) { if(cbin >= 0 && cbin < n) *vv = EZ_HgramValues(widget)[cbin];}
      return(cbin);
    }
  return(-1);
}
void EZ_SetHistogramColors(widget, colors, ncolors)
     EZ_Widget *widget; unsigned long *colors; int ncolors;
{
  if(widget && EZ_WidgetType(widget) == EZ_WIDGET_HISTOGRAM)
    {
      if(ncolors > EZ_HgramNColorCells(widget))
        {
          unsigned long *pvs = EZ_HgramColors(widget);
          pvs = (unsigned long *)my_realloc(pvs, (ncolors) * sizeof(unsigned long), _HOSTOGRAM_);
          EZ_HgramColors(widget) = pvs;
          EZ_HgramNColorCells(widget) = ncolors;      
        }
      EZ_HgramNColors(widget) = ncolors;      
      memcpy(EZ_HgramColors(widget), colors, ncolors * sizeof(unsigned long));
    }
}

/**********************************************************************************/
#undef _EZ_WIDGET_HISTOGRAM_C_


