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
***              Status Meter                                 ***
***                                                           ***
*****************************************************************/
#define _EZ_WIDGET_SMETER_C_
#include "EZ_Widget.h"

/*********************************************************************
 * 
 *  Functions implemented in this file:
 */
EZ_Widget   *EZ_CreateStatusMeter  MY_ANSIARGS((EZ_Widget *parent)); 
void         EZ_SetStatusMeterValue  MY_ANSIARGS((EZ_Widget *widget, float value));
void         EZ_SetStatusMeterRange MY_ANSIARGS((EZ_Widget *widget, float f, float t));
float        EZ_GetStatusMeterValue  MY_ANSIARGS((EZ_Widget *widget));
void         EZ_StopStatusMeter MY_ANSIARGS((EZ_Widget *widget));
void         EZ_StartStatusMeter MY_ANSIARGS((EZ_Widget *widget));
void         EZ_SetStatusMeterForeground MY_ANSIARGS((EZ_Widget *widget, char *clr));
void         EZ_SetStatusMeterBackground MY_ANSIARGS((EZ_Widget *widget, char *clr));

void         EZ_SetStatusMeterForegroundN MY_ANSIARGS((EZ_Widget *widget, int idx, char *clr));
void         EZ_SetStatusMeterValueN  MY_ANSIARGS((EZ_Widget *widget, int idx, float value));
void         EZ_SetStatusMeterRangeN MY_ANSIARGS((EZ_Widget *widget, int idx, float f, float t));
float        EZ_GetStatusMeterValueN  MY_ANSIARGS((EZ_Widget *widget, int idx));

/*********************************************************************/
void         EZ_SMeterEventHandle  MY_ANSIARGS((EZ_Widget *widget, XEvent *event));
void         EZ_ComputeWidgetSMeterSize  MY_ANSIARGS((EZ_Widget *widget, int *w, int *h));
void         EZ_DrawWidgetSMeter   MY_ANSIARGS((EZ_Widget *widget));
void         EZ_FreeWidgetSMeterData  MY_ANSIARGS((EZ_Widget *widget));
/*********************************************************************
 * 
 *  Local Variables.
 */
static EZ_WidgetHandle EZ_SMeterHandle =
{
  EZ_ComputeWidgetSMeterSize,
  EZ_DrawWidgetSMeter,
  EZ_FreeWidgetSMeterData,
  EZ_SMeterEventHandle,
};

/*********************************************************************/

EZ_Widget  *EZ_CreateStatusMeter(parent)
     EZ_Widget  *parent;     /* parent widget    */
{
  EZ_Widget  *wptr;
  unsigned long *clrs;
  int i;
  float    *f;

  wptr = EZ_CreateNewWidget(parent);
  EZ_WidgetHandlingFunctions[EZ_WIDGET_STATUS_METER] = &EZ_SMeterHandle;
  EZ_SetWidgetTypeAndNames(wptr, EZ_WIDGET_STATUS_METER);
  EZ_WidgetBorderStyle(wptr) = EZ_BORDER_DOWN;
  EZ_WidgetBorderWidth(wptr) = 2;
  EZ_WidgetPadX(wptr) = 0;
  EZ_WidgetPadY(wptr) = 0;
  EZ_WidgetSepX(wptr) = 0;
  EZ_WidgetSepY(wptr) = 0;
  EZ_WidgetFont(wptr) = EZ_GetFontFromId(EZ_LABEL_FONT);  
  clrs = EZ_SMeterFColor(wptr);
  clrs[0] = EZ_ColorArray[BLUE];
  clrs[1] = EZ_ColorArray[YELLOW];
  clrs[2] = EZ_ColorArray[RED];
  clrs[3] = EZ_ColorArray[GREEN];
  clrs[4] = EZ_ColorArray[MAGENTA];
  EZ_SMeterBColor(wptr) = ~0;
  f = EZ_SMeterValues(wptr);
  for(i = 0; i < 15; i++) f[i] = 0.0;
  f = EZ_SMeterRange(wptr);
  f[0] = f[1] = 0.0;
  EZ_SMeterNControls(wptr) = 1;
  EZ_SMeterPosition(wptr) = 0;
  EZ_SMeterOrientation(wptr) = EZ_HORIZONTAL;
  EZ_SMeterStyle(wptr) = 0;
  EZ_SMeterShowValue(wptr) = 0;
  EZ_SMeterStoped(wptr) = 0;
  EZ_SMeterBarWidth(wptr)  = 8;
  EZ_SMeterBarSep(wptr) = 2;
  
  return(wptr);
}

/***********************************************************************/
void   EZ_FreeWidgetSMeterData(widget)    EZ_Widget *widget;
{

}
/***********************************************************************/
void EZ_SMeterEventHandle(widget, event)
     EZ_Widget *widget;    XEvent *event;
{
  if(widget)
    {
      if(event->type == Expose)	EZ_DrawWidget(widget);      
      else if(event->type == ButtonPress)
	{
	  if(event->xbutton.button == EZ_Btn1)
	    { EZ_ExecuteWidgetCallBacks(widget);}
	  else if(event->xbutton.button == EZ_Btn3)
	    {
	      /*
		#include "EZ_DnDHandler1.c"
		EZ_DnDEventHandler1(widget, event);
		*/
	      EZ_HandleDnDEvents(widget, event);
	    }
	}
    }
}
/***********************************************************************/
void EZ_ComputeWidgetSMeterSize(widget, w, h)
    EZ_Widget *widget;
     int             *w, *h;
{
  int      length, height, cw, ch;

  EZ_SetWidgetSizeComputedFlag(widget);
      
  cw = EZ_WidgetPadX(widget) + EZ_WidgetBorderWidth(widget) + EZ_WidgetPadB(widget);
  ch = EZ_WidgetPadY(widget) + EZ_WidgetBorderWidth(widget) + EZ_WidgetPadB(widget);

  if(EZ_SMeterOrientation(widget) <= EZ_HORIZONTAL_CENTER)
    { length = 32; height = 8;   }
  else
    { length = 8; height = 32;   }
  *w = cw + cw + length;
  *h = ch + ch + height;
}

/*******************************************************************
 *
 *  Draw A SMeter.
 */
void  EZ_DrawWidgetSMeter(wptr)
     EZ_Widget *wptr;
{
  int       w, h,x,y, padb, padb2, bwpdx, bwpdy;
  Pixmap    pixmap;
  Window    win;
  GC        gc;
  unsigned  long   bgpv, *pv = EZ_SMeterFColor(wptr);
  int  i,j, ncontrols = EZ_SMeterNControls(wptr);
  float stotal, total, *fptr, *ff = EZ_SMeterValues(wptr);

  win = EZ_WidgetWindow(wptr);
  w   = EZ_WidgetWidth(wptr);
  h   = EZ_WidgetHeight(wptr);
  padb = EZ_WidgetPadB(wptr);
  padb2 = padb + padb;

  pixmap = XCreatePixmap(EZ_Display, win, w, h, EZ_Depth); 
  EZ_FixFocusHighlightBorder(wptr, pixmap, w, h, padb);

  if(EZ_SMeterBColor(wptr) != ~0)
    {
      gc = EZ_WRITABLEGC;
      XSetForeground(EZ_Display, gc, EZ_SMeterBColor(wptr));
    }
  else
    {
      if(EZ_GetBackgroundGC(wptr, &gc, &bgpv, 0, 0))
	{
	  int ox, oy;
	  EZ_GetWidgetTileOrigin(wptr, &ox, &oy);      
	  XSetTSOrigin(EZ_Display, gc, ox, oy);
	}
    }
  XFillRectangle(EZ_DisplayForWidgets, pixmap, gc, padb,padb, w-padb2, h-padb2); 
  if(EZ_SMeterStoped(wptr)) goto finish;
  
  bwpdx = EZ_WidgetPadX(wptr) + EZ_WidgetBorderWidth(wptr) + padb;
  bwpdy = EZ_WidgetPadY(wptr) + EZ_WidgetBorderWidth(wptr) + padb;

  fptr = EZ_SMeterRange(wptr);
  total = fptr[1] - fptr[0];
  if(total < 0.0) total = - total;
  stotal = 0.0;
  for(i = 0, j=0; i < ncontrols; i++, j += 3)
    {
      float ftmp = ff[j+1] - ff[j];
      stotal += (ftmp >= 0.0 ? ftmp : -ftmp);
    }
  if(total < stotal) total = stotal;

  gc = EZ_WRITABLEGC;
  if(total == 0.0)
    {
      if(EZ_SMeterOrientation(wptr)<= EZ_HORIZONTAL_CENTER)
	{
	  int ww, hh, ss, pos;
	  hh = h - (bwpdy +bwpdy);
	  ww = w - (bwpdx +bwpdx);
	  if(ww > 60) ss = 16;
	  else ss = ww >> 2;
	  pos = EZ_SMeterPosition(wptr);
	  if(pos >= 0)
	    {
	      if(pos + ss < ww) EZ_SMeterPosition(wptr) += 5;
	      else EZ_SMeterPosition(wptr) = - pos;
	    }
	  else
	    {
	      if(pos < 0) EZ_SMeterPosition(wptr) += 5;
	      else EZ_SMeterPosition(wptr) = -pos;
	    }
	  y = bwpdy;
	  x = (pos> 0 ? pos : -pos);
	  if(x < 0) x = 0; else if(x > ww -ss) x = ww-ss;
	  x += bwpdx; 
	  XSetForeground(EZ_Display, gc, pv[0]);
	  XFillRectangle(EZ_Display, pixmap, gc, x,y, ss, hh);
	}
      else
	{
	  int ww, hh, ss, pos;
	  hh = h - (bwpdy +bwpdy);
	  ww = w - (bwpdx +bwpdx);
	  if(hh > 60) ss = 16;
	  else ss = hh >> 2;
	  pos = EZ_SMeterPosition(wptr);
	  if(pos >= 0)
	    {
	      if(pos < hh) EZ_SMeterPosition(wptr) += 5;
	      else EZ_SMeterPosition(wptr) = - pos;
	    }
	  else
	    {
	      if(pos - ss < 0) EZ_SMeterPosition(wptr) += 5;
	      else EZ_SMeterPosition(wptr) = -pos;
	    }
	  x = bwpdx;
	  y = hh - (pos> 0 ? pos : -pos);
	  if(y < 0) y = 0;
	  else if(y > hh -ss) y = h -ss;
	  y += bwpdy; 
	  
	  XSetForeground(EZ_Display, gc, pv[0]);
	  XFillRectangle(EZ_Display, pixmap, gc, x,y, ww, ss);
	}
    }
  else 
    {
      if(EZ_SMeterOrientation(wptr)<= EZ_HORIZONTAL_CENTER)
	{
	  float flen = (float)( w - bwpdx - bwpdx);
	  float ftmp, factor = flen / total;
	  float ftotal = 0.0;
	  int ww, hh;
	  x = bwpdx; 
	  y = bwpdy;
	  hh = h - (bwpdy +bwpdy);

	  if(EZ_SMeterStyle(wptr) == 0)
	    {
	      for(i = 0, j=0; i < ncontrols; i++, j += 3) 
		{
		  if(ff[j] != ff[j+2])
		    {
		      ftmp =  ff[j+2] - ff[j];
		      if(ftmp < 0.0) ftmp = -ftmp;
		      ftotal += ftmp;
		      ww = (int) ( factor * ftmp + 0.5);
		      if(ww > 0)
			{
			  XSetForeground(EZ_Display, gc, pv[i]);
			  XFillRectangle(EZ_Display, pixmap, gc, x,y, ww, hh);
			  x += ww;
			}
		    }
		}
	    }
	  else 
	    {
	      int k, nbars, BAR_WIDTH, BAR_SEP;
	      BAR_WIDTH = (int)EZ_SMeterBarWidth(wptr);
	      BAR_SEP = (int)EZ_SMeterBarSep(wptr);
	      for(i = 0, j=0; i < ncontrols; i++, j += 3) 
		{
		  if(ff[j] != ff[j+2])
		    {
		      ftmp =  ff[j+2] - ff[j];
		      if(ftmp < 0.0) ftmp = -ftmp;
		      ftotal += ftmp;
		      ww = (int) ( factor * ftmp + 0.5);
		      if(ww > 0)
			{
			  XSetForeground(EZ_Display, gc, pv[i]);
			  nbars = (ww + BAR_WIDTH + BAR_SEP-1)/(BAR_WIDTH + BAR_SEP);
			  for(k = 0; k < nbars; k++)
			    {
			      XFillRectangle(EZ_Display, pixmap, gc, x,y, BAR_WIDTH, hh);
			      x += BAR_WIDTH + BAR_SEP;
			    }
			}
		    }	      
		}
	    }
	  if(EZ_SMeterShowValue(wptr))
	    {
	      char str[8]; int tlen, ascent,dscent;
	      XFontStruct *fnt = EZ_WidgetFont(wptr);
	      gc = EZ_WRITABLEGC;
	      XSetFont(EZ_Display, gc, fnt->fid);
	      XSetFunction(EZ_Display, gc, GXinvert);
	      sprintf(str, "%3d%%", (int)(ftotal * 100.0/total));
	      tlen = XTextWidth(fnt, str, 3);
	      ascent = fnt->ascent;
	      dscent = fnt->descent;
	      x = ((w - tlen) >> 1);
	      y = (h - ascent -dscent)/2 + ascent;
	      XDrawString(EZ_Display, pixmap, gc, x, y, str, 4);
	      XSetFunction(EZ_Display, gc, GXcopy);
	    }
	}
      else
	{
	  float flen = (float)( h - bwpdy - bwpdy);
	  float ftmp, factor = flen / total;
	  float ftotal = 0.0;
	  int ww, hh;
	  ww = w - (bwpdx +bwpdx);
	  y = h - bwpdy;
	  x = bwpdx; 
	  if(EZ_SMeterStyle(wptr) == 0)
	    {
	      for(i = 0, j=0; i < ncontrols; i++, j += 3) 
		{
		  if(ff[j] != ff[j+2])
		    {
		      ftmp =  ff[j+2] - ff[j];
		      if(ftmp < 0.0) ftmp = -ftmp;
		      ftotal += ftmp;
		      hh = (int) ( factor * ftmp + 0.5);
		      if(hh > 0)
			{
			  XSetForeground(EZ_Display, gc, pv[i]);
			  XFillRectangle(EZ_Display, pixmap, gc, x,y-hh, ww, hh);
			  y -= hh;
			}
		    }
		}
	    }
	  else
	    {
	      int k, nbars, BAR_WIDTH, BAR_SEP;
	      BAR_WIDTH = (int)EZ_SMeterBarWidth(wptr);
	      BAR_SEP = (int)EZ_SMeterBarSep(wptr);
	      for(i = 0, j=0; i < ncontrols; i++, j += 3) 
		{
		  if(ff[j] != ff[j+2])
		    {
		      ftmp =  ff[j+2] - ff[j];
		      if(ftmp < 0.0) ftmp = -ftmp;
		      ftotal += ftmp;
		      hh = (int) ( factor * ftmp + 0.5);
		      if(hh > 0)
			{
			  XSetForeground(EZ_Display, gc, pv[i]);
			  nbars = (hh + BAR_WIDTH + BAR_SEP-1)/(BAR_WIDTH + BAR_SEP);
			  for(k = 0; k < nbars; k++)
			    {
			      XFillRectangle(EZ_Display, pixmap, gc, x,y, ww, BAR_WIDTH);
			      y -= BAR_WIDTH + BAR_SEP;
			    }
			}
		    }
		}
	    }
	  if(EZ_SMeterShowValue(wptr))
	    {
	      char str[8]; int tlen, ascent,dscent;
	      XFontStruct *fnt = EZ_WidgetFont(wptr);
	      if( ww > 2 * (fnt->max_bounds. width))
		{
		  gc = EZ_WRITABLEGC;
		  XSetFunction(EZ_Display, gc, GXinvert);
		  XSetFont(EZ_Display, gc, fnt->fid);
		  sprintf(str, "%3d%%", (int)(ftotal * 100.0/total));
		  tlen = XTextWidth(fnt, str, 3);
		  ascent = fnt->ascent;
		  dscent = fnt->descent;
		  x = bwpdx + (w - tlen)/2;
		  y = (h - ascent -dscent)/2 + ascent;
		  XDrawString(EZ_Display, pixmap, gc, x, y, str, 4);
		  XSetFunction(EZ_Display, gc, GXcopy);
		}
	    }
	}
    }
 finish:
  EZ_DrawRectBorder(wptr, pixmap);     
  XCopyArea(EZ_Display,pixmap,win, EZ_WRITABLEGC,0,0,w,h,0,0); 
  XFreePixmap(EZ_Display, pixmap); 
}
/*********************************************************************************/
void  EZ_SetStatusMeterValue(widget, value)
     EZ_Widget *widget; float value;
{
  if(widget && EZ_WidgetType(widget) == EZ_WIDGET_STATUS_METER)
    {
      float *ff = EZ_SMeterValues(widget);
      ff[2] = value;
      if(EZ_SMeterStoped(widget) == 0)  EZ_DrawWidget(widget);
    }
}
void  EZ_SetStatusMeterRange (widget, frm, to)
     EZ_Widget *widget; float frm, to;
{
  if(widget && EZ_WidgetType(widget) == EZ_WIDGET_STATUS_METER)
    {
      float *ff = EZ_SMeterRange(widget);
      ff[0] = frm; ff[1] = to;
      if(EZ_SMeterStoped(widget) == 0)  EZ_DrawWidget(widget);
    }
}
float EZ_GetStatusMeterValue(widget)
     EZ_Widget *widget;
{
  if(widget && EZ_WidgetType(widget) == EZ_WIDGET_STATUS_METER)
    {
      float *ff = EZ_SMeterValues(widget);
      return(ff[2]);
    }
  return(0.0);
}
/*****************************************************/
void  EZ_SetStatusMeterValueN(widget, idx, value)
     EZ_Widget *widget; float value; int idx;
{
  if(widget && EZ_WidgetType(widget) == EZ_WIDGET_STATUS_METER)
    {
      if(idx >= 0 && idx < 5)
	{
	  float *ff = EZ_SMeterValues(widget);
	  ff[3*idx + 2] = value;
	  idx++;
	  if(EZ_SMeterNControls(widget) <= idx) EZ_SMeterNControls(widget) = idx+1;
	  if(EZ_SMeterStoped(widget) == 0)  EZ_DrawWidget(widget);
	}
    }
}
void  EZ_SetStatusMeterRangeN(widget, idx, frm, to)
     EZ_Widget *widget; float frm, to; int idx;
{
  if(widget && EZ_WidgetType(widget) == EZ_WIDGET_STATUS_METER)
    {
      if(idx >= 0 && idx < 5)
	{
	  float *ff = EZ_SMeterValues(widget);
	  ff[3*idx] = frm; ff[3*idx +1] = to;
	  idx++;
	  if(EZ_SMeterNControls(widget) <= idx) EZ_SMeterNControls(widget) = idx+1;
	  if(EZ_SMeterStoped(widget) == 0)  EZ_DrawWidget(widget);
	}
    }
}
float EZ_GetStatusMeterValueN(widget, idx)
     EZ_Widget *widget; int idx;
{
  if(widget && EZ_WidgetType(widget) == EZ_WIDGET_STATUS_METER)
    {
      if(idx >= 0 && idx < 5)
	{
	  float *ff = EZ_SMeterValues(widget);
	  return(ff[3*idx + 2]);
	}
    }
  return(0.0);
}

void EZ_StopStatusMeter(widget)
     EZ_Widget *widget;
{
  if(widget && EZ_WidgetType(widget) == EZ_WIDGET_STATUS_METER)  
    { EZ_SMeterStoped(widget) = 1; }
}
void EZ_StartStatusMeter(widget)
     EZ_Widget *widget;
{
  if(widget && EZ_WidgetType(widget) == EZ_WIDGET_STATUS_METER &&
     EZ_SMeterStoped(widget) == 1)
    {
      EZ_SMeterStoped(widget) = 0;
      EZ_DrawWidget(widget);
    }
}
/****************************************************************/
void EZ_SetStatusMeterBackground(widget, clr) 
     EZ_Widget *widget; char *clr;
{
  if(widget && EZ_WidgetType(widget) == EZ_WIDGET_STATUS_METER)  
    {
      unsigned long  pixel_value = EZ_AllocateColorFromName(clr);
      if(EZ_SMeterBColor(widget) != pixel_value)
	{
	  EZ_SMeterBColor(widget) = pixel_value;
	  if(EZ_SMeterStoped(widget) == 0)  EZ_DrawWidget(widget);
	}
    }
}

void EZ_SetStatusMeterForeground(widget, clr) 
     EZ_Widget *widget; char *clr;
{
  if(widget && EZ_WidgetType(widget) == EZ_WIDGET_STATUS_METER)  
    {
      unsigned long  pixel_value = EZ_AllocateColorFromName(clr);
      if(EZ_SMeterFColor(widget)[0] != pixel_value)
	{
	  EZ_SMeterFColor(widget)[0] = pixel_value;
	  if(EZ_SMeterStoped(widget) == 0)  EZ_DrawWidget(widget);
	}
    }
}
void EZ_SetStatusMeterForegroundN(widget, idx, clr)
     EZ_Widget *widget; char *clr; int idx;
{
  if(widget && EZ_WidgetType(widget) == EZ_WIDGET_STATUS_METER)  
    {
      if(idx >=0 && idx < 5)
	{
	  unsigned long  pixel_value = EZ_AllocateColorFromName(clr);
	  if(EZ_SMeterFColor(widget)[idx] != pixel_value)
	    {
	      EZ_SMeterFColor(widget)[idx] = pixel_value;
	      if(EZ_SMeterStoped(widget) == 0)  EZ_DrawWidget(widget);
	    }
	}
    }
}
/**********************************************************************************/
#undef _EZ_WIDGET_SMETER_C_
