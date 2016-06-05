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
 ***              Widget H and V Slider                        ***
 ***                                                           ***
 *****************************************************************/
#define _EZ_WIDGET_SLIDER_C_

#include "EZ_Widget.h"

/*********************************************************************
 * 
 *  Functions implemented in this file:
 */

void         EZ_SetSliderValue  MY_ANSIARGS((EZ_Widget *widget, float value));
float        EZ_GetSliderValue  MY_ANSIARGS((EZ_Widget *widget));
void         EZ_SetSliderRange MY_ANSIARGS((EZ_Widget *widget, float f, float t));

void         EZ_ComputeWidgetHSliderSize  MY_ANSIARGS((EZ_Widget *widget, int *w, int *h));
void         EZ_ComputeWidgetVSliderSize  MY_ANSIARGS((EZ_Widget *widget, int *w, int *h));
void         EZ_DrawWidgetHorizontalSlider  MY_ANSIARGS((EZ_Widget *widget));
void         EZ_DrawWidgetVerticalSlider MY_ANSIARGS((EZ_Widget *widget));
void         EZ_FreeWidgetSliderData  MY_ANSIARGS((EZ_Widget *widget));
EZ_Widget   *EZ_CreateSlider  MY_ANSIARGS((EZ_Widget *parent, char *lab, 
					   float mnv, float mxv, float initv, int type));
void         EZ_SliderEventHandle  MY_ANSIARGS((EZ_Widget *widget, XEvent *event));

void         EZ_SetPrintFFormat MY_ANSIARGS((float from, float to, float resolution,
                                             float objlength, char *ret));
float         EZ_FixSliderValue MY_ANSIARGS((float v, float res));
/*********************************************************************
 * 
 *  Local functions:
 */
static void         EZ_InitialHSliderInternalData  MY_ANSIARGS((EZ_Widget *widget, int w, int h));
static void         EZ_InitialVSliderInternalData  MY_ANSIARGS((EZ_Widget *widget, int w, int h));

/*********************************************************************
 * 
 *  Local Variables.
 */
static EZ_WidgetHandle EZ_HSliderHandle =
{
  EZ_ComputeWidgetHSliderSize,
  EZ_DrawWidgetHorizontalSlider,
  EZ_FreeWidgetSliderData,
  EZ_SliderEventHandle,
};

static EZ_WidgetHandle EZ_VSliderHandle =
{
  EZ_ComputeWidgetVSliderSize,
  EZ_DrawWidgetVerticalSlider,
  EZ_FreeWidgetSliderData,
  EZ_SliderEventHandle,
};

/*********************************************************************/

EZ_Widget  *EZ_CreateSlider(parent,label, minvalue, maxvalue, init_v, type)
     EZ_Widget  *parent;     /* parent widget    */
     char             *label;
     float            minvalue, maxvalue, init_v;
     int              type;
{
  EZ_Widget  *wptr;

  wptr = EZ_CreateNewWidget(parent);
  EZ_SetWidgetTypeAndNames(wptr, type);
  /*--------------------------------------------
   * This is a hack. For HSlider, we really
   * don't want the width be modified when
   * a fill mode is set on its parent.
   * For VSlider, we don't want the width be
   * modified.
   *------------------------------------------*/
  if(type == EZ_WIDGET_HORIZONTAL_SLIDER) 
    {
      /*--------------------------------------------------
       * Register the handling functions for ...
       *  has to be done after  EZ_CreateNewWiget.
       *-------------------------------------------------*/
      EZ_WidgetHandlingFunctions[EZ_WIDGET_HORIZONTAL_SLIDER] = &EZ_HSliderHandle;
      EZ_SetWidgetHeightSetFlag(wptr); 
    }
  else
    {
      /*--------------------------------------------------
       * Register the handling functions for ...
       *  has to be done after  EZ_CreateNewWiget.
       *-------------------------------------------------*/
      EZ_WidgetHandlingFunctions[EZ_WIDGET_VERTICAL_SLIDER] = &EZ_VSliderHandle;
      EZ_SetWidgetWidthSetFlag(wptr);
    }

  EZ_SliderLabel(wptr) = EZ_AllocCopyString(label);      
  EZ_SliderLabelLength(wptr) = (label != (char *)NULL)? strlen(label): 0;

  EZ_SliderFont(wptr) = EZ_GetFontFromId(EZ_SLIDER_FONT);
  EZ_WidgetBorderStyle(wptr)  = EZ_BORDER_NONE;
  EZ_WidgetPadB(wptr) = 2;  

  EZ_SliderWidth(wptr) = 16;           /* default size of slider */
  EZ_SliderLength(wptr) = 100;
  EZ_SliderSliderLength(wptr) = 32;

  EZ_SliderResolution(wptr) = -1.0;

  EZ_SliderBorderWidth(wptr) = 2;
  EZ_SliderBorderStyle(wptr) = EZ_BORDER_UP;

  EZ_SliderMinValue(wptr) = minvalue;
  EZ_SliderMaxValue(wptr) = maxvalue;
  EZ_SliderValue(wptr) = init_v;

  EZ_SliderDisplayValue(wptr) = 1;

  EZ_SetWidgetFocusableFlag(wptr);  

  return(wptr);
}

void   EZ_FreeWidgetSliderData(widget)
     EZ_Widget *widget;
{
  if(EZ_SliderLabel(widget) != (char *)NULL)
    (void) my_free( (char *) EZ_SliderLabel(widget));
}

/********************************************************************
 *
 *  Figure out the dimension of a horizontal slider
 */
void EZ_ComputeWidgetHSliderSize(widget, w, h)
    EZ_Widget *widget;
     int             *w, *h;
{
  int length, height, cw, ch,bs,ss, tmp;
  int dvalue,nticks,ticksize,style,nocvalue;

  dvalue = (int)EZ_SliderDisplayValue(widget);
  bs = (EZ_SliderLabelLength(widget) != 0) ? 2 : 0;
  ss = (dvalue != 0) ? 2 : 0;
  
  nticks = (int) EZ_SliderNTicks(widget);
  ticksize = (nticks )? (int) EZ_SliderTickSize(widget): 0;
  nocvalue = (int)EZ_SliderNoCValue(widget);
  
  if(!(EZ_GetWidgetSizeComputedFlag(widget)))
    { 
      /* 6.3.96 */
      /* make sure slider button is big enough */
      style = (int) EZ_SliderStyle(widget);
      tmp = ( (EZ_SliderBorderWidth(widget))<< 1) + 4;
      if(style < 4)
        {
          if(EZ_SliderWidth(widget) < tmp) EZ_SliderWidth(widget) =  tmp;
          if(EZ_SliderSliderLength(widget) < tmp + tmp) EZ_SliderSliderLength(widget) = tmp + tmp;
        }
      else
        {
          int jtmp = (tmp-3) << 2;
          if(EZ_SliderWidth(widget) < jtmp) EZ_SliderWidth(widget) = jtmp;
          EZ_SliderSliderLength(widget) = (EZ_SliderBorderWidth(widget) +EZ_SliderWidth(widget))>>1;
        }

      if( EZ_SliderLabelLength(widget) != 0)
	{
	  EZ_SliderTWidth(widget) = XTextWidth(EZ_SliderFont(widget),
					       EZ_SliderLabel(widget),
					       EZ_SliderLabelLength(widget));
	  EZ_SliderTHeight(widget) = EZ_SliderFont(widget)->ascent +
	    EZ_SliderFont(widget)->descent;
	}
      else
        {
          EZ_SliderTWidth(widget) = 0;
          EZ_SliderTHeight(widget) = 0;
        }
      EZ_SliderFontAscent(widget) = EZ_SliderFont(widget)->ascent;
      
      if(dvalue)
	{
	  char tmp[32];
	  int  itmp1,itmp2, itmp3;

          EZ_SetPrintFFormat(EZ_SliderMaxValue(widget), EZ_SliderMinValue(widget),
                             EZ_SliderResolution(widget), (float)EZ_SliderLength(widget),
                             EZ_SliderFormat(widget));
	  (void)sprintf(tmp, EZ_SliderFormat(widget), EZ_SliderMaxValue(widget));
	  itmp1 = EZ_SliderVWidth(widget) = XTextWidth(EZ_SliderFont(widget),
						       tmp,
						       strlen(tmp));
	  (void)sprintf(tmp, EZ_SliderFormat(widget), EZ_SliderMinValue(widget));
	  itmp2 = EZ_SliderVWidtha(widget) = XTextWidth(EZ_SliderFont(widget),
							tmp,
							strlen(tmp));

          EZ_SliderVHeight(widget) = EZ_SliderFont(widget)->ascent +
            EZ_SliderFont(widget)->descent;
	  EZ_SliderFontAscent(widget) = EZ_SliderFont(widget)->ascent;

	  if(style < 4)
            {
              itmp3 = (MAXV(itmp1,itmp2)>>1) + 1;
              if(EZ_SliderSliderLength(widget) < itmp3)
                EZ_SliderSliderLength(widget) = itmp3;
            }
	}
      else
        {
          EZ_SliderVWidth(widget) = 0;
          EZ_SliderVWidtha(widget) = 0;
          EZ_SliderVHeight(widget) = 0;
        }
      EZ_SetWidgetSizeComputedFlag(widget);
    }

  length = EZ_SliderTWidth(widget) + 
    ((EZ_SliderVWidth(widget) + EZ_SliderVWidtha(widget)) << 1)
    + EZ_SliderSliderLength(widget) + 4;
  height = EZ_SliderTHeight(widget) + (EZ_SliderVHeight(widget)) + bs + (ss<<1); 
  EZ_SliderLength(widget) = length; 
  
  if(dvalue>1 && nocvalue==0) height += EZ_SliderVHeight(widget);
  cw = EZ_WidgetPadX(widget) + EZ_WidgetBorderWidth(widget) + EZ_WidgetPadB(widget) 
    + EZ_SliderBorderWidth(widget) ;
  ch = EZ_WidgetPadY(widget) + EZ_WidgetBorderWidth(widget) + EZ_WidgetPadB(widget)
    + EZ_SliderBorderWidth(widget);

  /*-------------------------------
   *  minimal size of the X window
   *------------------------------*/
  ch = ch + ch + EZ_SliderWidth(widget) + height + ticksize;
  cw = cw + cw + EZ_SliderLength(widget);
  *w = cw;
  *h = ch;

  EZ_SetWidgetSizeMayBeChangedFlag(widget);
  /* EZ_InitialHSliderInternalData(widget,cw,ch);*/
}	  

/*******************************************************************************
 *
 *  Initialize H slider internal data.
 */

static void EZ_InitialHSliderInternalData(widget,w,h)
     EZ_Widget *widget;
     int             w,h;
{
  int cw, ch, padb, sl, tmp,bs,ss, bw2, nticks, ticksize, dvalue,style, nocvalue;

  padb = EZ_WidgetPadB(widget);
  dvalue = (int)EZ_SliderDisplayValue(widget);
  nocvalue=(int)EZ_SliderNoCValue(widget);
  bs = (EZ_SliderLabelLength(widget) != 0) ? 2 : 0;
  ss = (dvalue != 0) ? 2 : 0;
  nticks = (int)EZ_SliderNTicks(widget);
  ticksize = (nticks) ? (int)EZ_SliderTickSize(widget): 0;
  
  bw2 = EZ_WidgetBorderWidth(widget) + EZ_SliderBorderWidth(widget);
  cw = EZ_WidgetPadX(widget) + bw2 + padb;
  ch = EZ_WidgetPadY(widget) + bw2 + padb;

  sl = EZ_SliderLength(widget) = w - (cw <<1);
  
  style = EZ_SliderStyle(widget);
  /*
  if(!EZ_SliderWidthSet(widget))
    {
      if(style < 4)
        {
          EZ_SliderWidth(widget) = ( h - ((ch <<1) + bs + ticksize + (ss<<1) + EZ_SliderTHeight(widget)) );
          if(nocvalue == 0) EZ_SliderWidth(widget)  -= EZ_SliderVHeight(widget);
          if(dvalue > 1) EZ_SliderWidth(widget) -= EZ_SliderVHeight(widget);
        }
    }
    */
  /*-----------------------------------------------
   * min scrn coor of slider and the slider window
   *----------------------------------------------*/
  EZ_SliderPositionMin(widget) = cw + (EZ_SliderSliderLength(widget)>>1);
  EZ_SliderMinx(widget) = cw;
  EZ_SliderMaxx(widget) = cw + EZ_SliderLength(widget);
  EZ_SliderMiny(widget) = ch + EZ_SliderTHeight(widget) + EZ_SliderVHeight(widget) + bs + ss;
  if(nocvalue) EZ_SliderMiny(widget) -=  EZ_SliderVHeight(widget);
  EZ_SliderMaxy(widget) = EZ_SliderMiny(widget) + EZ_SliderWidth(widget);
  if(style > 1)
    {
      EZ_SliderMiny(widget) -= EZ_SliderBorderWidth(widget);
      EZ_SliderMaxy(widget) -= EZ_SliderBorderWidth(widget);
    }
  /*--------------------------
   * compute scaling factor
   *--------------------------*/
  if( (tmp = (sl - EZ_SliderSliderLength(widget))) <= 0) tmp = 1;
  EZ_SliderFactor(widget) = (EZ_SliderMaxValue(widget) - EZ_SliderMinValue(widget))/
      (float)tmp;
  /*-------------------------------------------
   * Set initial slider position 
   *------------------------------------------*/
  EZ_SliderPosition(widget) = (int)
    (EZ_SliderPositionMin(widget) +
     (EZ_SliderValue(widget) -EZ_SliderMinValue(widget))/EZ_SliderFactor(widget));
}

/***************************************************************
 *
 *  Figure out the precision needed to display the slider value
 */

void EZ_SetPrintFFormat(from, to, resolution, olength, fret)
     float from, to, resolution, olength; char *fret;
{
  double fa,fb, ftmp;
  int    maxdigits, resolutiondigits, digits, itmp;

  fa = fabs(from);
  fb = fabs(to);
  if(fa == 0.0) fa = 1.0;
  if(fb == 0.0) fb = 1.0;
  if(fa > fb) { ftmp = fa; fa = fb; fb = ftmp;}
  maxdigits = floor(log10(fa)); 
  digits = floor(log10(fb));
  maxdigits = (abs(digits) > abs(maxdigits)? digits: maxdigits);


  if(resolution > 0.0)
    resolutiondigits = floor(log10(resolution)) + 1;
  else
    {
      ftmp = olength;
      fa = fabs(from - to)/ftmp;
      resolutiondigits = floor(log10(fa));
    }
  digits = (maxdigits - resolutiondigits) + 1;   
  /* this is the number of digits needed */

  itmp = digits - maxdigits;  /* digits for fractional part */
  if(itmp < 0) itmp  = 0;
  (void) sprintf(fret, "%%.%df", itmp);
}  

/**************************************************************************/

void  EZ_DrawWidgetHorizontalSlider(wptr)
     EZ_Widget *wptr;
{
  int             w, h,x,y,xx,yy, ww, hh, cw,ch,bs,ss, padb, padb2, tile;
  Pixmap          pixmap;
  Window          win;
  GC              gc, gc1;
  int             offset = 0, sliderStyle, tox = 0, toy = 0, highlight, swapBg;
  int             bw, nticks,ticksize,tick1,tick2,dvalue, nocvalue;
  unsigned long   bgpv;

  win = EZ_WidgetWindow(wptr);
  w   = EZ_WidgetWidth(wptr);
  h   = EZ_WidgetHeight(wptr);
  padb = EZ_WidgetPadB(wptr);
  padb2 = padb + padb;
  sliderStyle = EZ_SliderStyle(wptr);
  swapBg = EZ_SliderHighlight(wptr);
  highlight = EZ_GetWidgetHighlightFlag(wptr);
  dvalue = (int) EZ_SliderDisplayValue(wptr);
  nocvalue=(int)EZ_SliderNoCValue(wptr);
  bs = (EZ_SliderLabelLength(wptr) != 0)? 2 : 0;
  ss = (dvalue != 0) ? 2 : 0;

  if(EZ_WidgetSizeMayBeChangedFlag(wptr))
    {
      EZ_InitialHSliderInternalData(wptr,w,h);
      EZ_ClearWidgetSizeMayBeChangedFlag(wptr);
    }
  /*-----------------------------------------------------------
   *  Create a pixmap, draw into this pixmap in background and
   *  copy to the button window when finished.
   *----------------------------------------------------------*/
  pixmap = XCreatePixmap(EZ_Display, win, w, h, EZ_Depth);    
  EZ_FixFocusHighlightBorder(wptr, pixmap, w, h, padb);

  tile = EZ_GetBackgroundGC(wptr, &gc, &bgpv,0, 0);
  if(tile)
    {
      EZ_GetWidgetTileOrigin(wptr, &tox, &toy);      
      XSetTSOrigin(EZ_Display, gc, tox, toy);
    }
  XFillRectangle(EZ_Display, pixmap, gc, padb,padb, w-padb2, h-padb2); 

  bw = EZ_WidgetBorderWidth(wptr);
  cw = bw + EZ_WidgetPadX(wptr) + padb;
  ch = bw + EZ_WidgetPadY(wptr) + padb;

  nticks = (int) EZ_SliderNTicks(wptr);
  ticksize = (nticks ? EZ_SliderTickSize(wptr): 0);
  if(nticks)
    {
      tick1 =(ticksize-4);
      tick2= (tick1>>1);
    }
  else tick1=tick2=ticksize=0;
  /*--------------------------------------------------------
   *  Draw label and values.
   *-------------------------------------------------------*/
  if( 1 /*dvalue || nticks ||  EZ_SliderLabelLength(wptr) != 0 */)
    {
      if( EZ_GetWidgetDisabledFlag(wptr) )
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
          
	  if(EZ_SliderLabelLength(wptr) != 0)
	    {
	      x = ( (w  - EZ_SliderTWidth(wptr)) >> 1);
	      y =  EZ_SliderFontAscent(wptr) + ch;
	      XDrawString(EZ_Display, pixmap, bgc,
			  x+1, y+1,
			  EZ_SliderLabel(wptr),
			  EZ_SliderLabelLength(wptr));
	      XDrawString(EZ_Display, pixmap, lgc,
			  x-1, y-1,
			  EZ_SliderLabel(wptr),
			  EZ_SliderLabelLength(wptr));
	      XDrawString(EZ_Display, pixmap, ngc,
			  x, y,
			  EZ_SliderLabel(wptr),
			  EZ_SliderLabelLength(wptr));
	    }
	  if(dvalue)
	    {
	      char tmp[32];
              int xx;
              /* display slider value */
              if(nocvalue == 0)
                {
                  (void) sprintf(tmp, EZ_SliderFormat(wptr),EZ_SliderValue(wptr)); 
                  x =  EZ_SliderPosition(wptr) - (EZ_SliderVWidth(wptr)>>1);
                  y =  EZ_SliderTHeight(wptr) + EZ_SliderFontAscent(wptr)  + ch + bs;
                  /*y =  h - (ch + EZ_SliderVHeight(wptr) - EZ_SliderFontAscent(wptr)) + ss;*/
                  XDrawString(EZ_Display, pixmap, bgc, x+1, y+1,  tmp,  strlen(tmp));	  
                  XDrawString(EZ_Display, pixmap, lgc, x-1, y-1,  tmp,  strlen(tmp));	  
                  XDrawString(EZ_Display, pixmap, ngc, x, y,  tmp,  strlen(tmp));	  
                }

              /* display tick values */
              if(dvalue >= 2 )
                {
                  int nnn, i, count, vwidth, vwidtha, tswidth;
                  float frange = (EZ_SliderMaxValue(wptr) - EZ_SliderMinValue(wptr));
                  float irange = frange / EZ_SliderFactor(wptr);
                  float dd, dtmp, dv, dvtmp;
                  nnn = (nticks > dvalue? nticks: dvalue);

                  vwidth =EZ_SliderVWidth(wptr);
                  vwidtha =EZ_SliderVWidtha(wptr);
                  if(vwidth < vwidtha) vwidth = vwidtha;
                  vwidth += 4;
                  tswidth = (int)irange;
                  count = tswidth/vwidth;     
                  if(count < nnn)  nnn = count;
                  if(nticks) nticks = nnn;

                  dtmp =  irange/(float)nnn;
                  dvtmp = (EZ_SliderMaxValue(wptr)-EZ_SliderMinValue(wptr))/nnn;

                  x =  EZ_SliderPositionMin(wptr)  - (EZ_SliderVWidtha(wptr)>>1);
                  /*y =  EZ_SliderTHeight(wptr) + EZ_SliderFontAscent(wptr)  + ch + bs;*/
                  y =  h - (ch + EZ_SliderVHeight(wptr) - EZ_SliderFontAscent(wptr)) + ss;
                  
                  for(dd=0.0, dv=0.0, i = 0; i <=nnn; i++, dd+=dtmp, dv+=dvtmp)
                    {
                      xx = x + (int)(dd);
                      (void) sprintf(tmp, EZ_SliderFormat(wptr),EZ_SliderMinValue(wptr) + dv);
                      XDrawString(EZ_Display, pixmap, bgc, xx+1, y+1, tmp, strlen(tmp));
                      XDrawString(EZ_Display, pixmap, lgc, xx-1, y-1, tmp, strlen(tmp));
		      XDrawString(EZ_Display, pixmap, ngc, x, y,  tmp,  strlen(tmp));	  
		    }
                }
            }
          if(nticks > 0)
            {
              int i, voff;
              float dd,dtmp, irange=(EZ_SliderMaxValue(wptr) - EZ_SliderMinValue(wptr))/ EZ_SliderFactor(wptr);
              nticks += nticks;
              dtmp =  irange/(float)nticks;
              voff = (dvalue > 1)?  EZ_SliderVHeight(wptr): 0;
              yy = h -(ch + voff + ss + ss + tick1);
              x =  EZ_SliderPositionMin(wptr);
              for(dd=0.0, i = 0; i <= nticks; i++, dd+=dtmp)
                {
                  int hhh = (i&1)? tick2: tick1;
                  xx = x + (int)(dd);
                  XDrawLine(EZ_Display,pixmap,bgc, xx+1, yy+1,xx+1,yy+hhh+1);
                  XDrawLine(EZ_Display,pixmap,lgc, xx-1, yy-1,xx-1,yy+hhh-1);
                  XDrawLine(EZ_Display,pixmap,ngc, xx, yy,xx,yy+hhh);
                }
            }
        }
      else /* enabled */
	{
	  if(EZ_SliderForeground(wptr) != EZ_DefaultForeground || 
	     EZ_SliderFont(wptr) != EZ_GetFontFromId(EZ_BUTTON_FONT))
	    {
	      XSetFont(EZ_Display, EZ_WRITABLEGC, EZ_SliderFont(wptr)->fid);
	      XSetForeground(EZ_Display, EZ_WRITABLEGC, EZ_SliderForeground(wptr));	
	      gc = EZ_WRITABLEGC;
	    }
	  else  gc = EZ_BUTTON_TEXTGC;
	  if(EZ_SliderLabelLength(wptr) != 0)
	    {
              int shadow = EZ_WidgetLabelShadow(wptr);
	      x = ( (w  - EZ_SliderTWidth(wptr)) >> 1);
	      y =  EZ_SliderFontAscent(wptr) + ch;
              if(shadow)
                {
                  int shadowx, shadowy;
                  if(shadow &0xf0)
                    {
                      shadowx = (shadow & 0xf0) >> 4;
                      shadowy = (shadow & 0x0f);
                    }
                  else shadowx = shadowy = shadow;
                  if(shadowx & 8) shadowx= (8-shadowx);
                  if(shadowy & 8) shadowy= (8-shadowy);
                  XSetFont(EZ_Display, EZ_SHADOWGC, EZ_SliderFont(wptr)->fid);
                  XDrawString(EZ_Display, pixmap, EZ_SHADOWGC,
                              x+shadowx, y+shadowy,
                              EZ_SliderLabel(wptr),
                              EZ_SliderLabelLength(wptr));
                }
	      XDrawString(EZ_Display, pixmap, gc, 
			  x, y,
			  EZ_SliderLabel(wptr),
			  EZ_SliderLabelLength(wptr));
	    }
	  if(dvalue)
	    {
	      char tmp[32];
              int xx;
              /* display slider value */
              if(nocvalue == 0)
                {
                  (void) sprintf(tmp, EZ_SliderFormat(wptr),EZ_SliderValue(wptr)); 
                  x =  EZ_SliderPosition(wptr) - (EZ_SliderVWidth(wptr)>>1);
                  y =  EZ_SliderTHeight(wptr) + EZ_SliderFontAscent(wptr)  + ch + bs;
                  /*y =  h - (ch + EZ_SliderVHeight(wptr) - EZ_SliderFontAscent(wptr)) + ss;*/
                  XDrawString(EZ_Display, pixmap, gc,  x, y, tmp, strlen(tmp));	  
                }

              /* display tick values */
              if(dvalue >= 2 )
                {
                  int nnn, i, count, vwidth, vwidtha, tswidth;
                  float frange = (EZ_SliderMaxValue(wptr) - EZ_SliderMinValue(wptr));
                  float irange = frange / EZ_SliderFactor(wptr);
                  float dd, dtmp, dv, dvtmp;
                  nnn = (nticks > dvalue? nticks: dvalue);

                  vwidth =EZ_SliderVWidth(wptr);
                  vwidtha =EZ_SliderVWidtha(wptr);
                  if(vwidth < vwidtha) vwidth = vwidtha;
                  vwidth += 4;
                  tswidth = (int)irange;
                  count = tswidth/vwidth;     
                  if(count < nnn)  nnn = count;
                  if(nticks) nticks = nnn;

                  dtmp =  irange/(float)nnn;
                  dvtmp = (EZ_SliderMaxValue(wptr)-EZ_SliderMinValue(wptr))/nnn;

                  x =  EZ_SliderPositionMin(wptr)  - (EZ_SliderVWidtha(wptr)>>1);
                  /* y =  h - (ch + EZ_SliderVHeight(wptr) - EZ_SliderFontAscent(wptr)) + ss;*/
                  y = EZ_SliderMaxy(wptr) + ticksize + (EZ_SliderBorderWidth(wptr))*2 + ss +
                    EZ_SliderFontAscent(wptr);

                  for(dd=0.0, dv=0.0, i = 0; i <=nnn; i++, dd+=dtmp, dv+=dvtmp)
                    {
                      xx = x + (int)(dd);
                      (void) sprintf(tmp, EZ_SliderFormat(wptr),EZ_SliderMinValue(wptr) + dv);
                      XDrawString(EZ_Display, pixmap, gc, xx, y, tmp, strlen(tmp));
                    }
                }
            }
          if(nticks > 0)
            {
              int i, voff;
              float dd,dtmp,irange = (EZ_SliderMaxValue(wptr) - EZ_SliderMinValue(wptr))/ EZ_SliderFactor(wptr);
              nticks += nticks;
              dtmp =  irange/(float)nticks;
              voff = (dvalue > 1)?  EZ_SliderVHeight(wptr): 0;
              yy = EZ_SliderMaxy(wptr) + (EZ_SliderBorderWidth(wptr))*2 + ss ;
              x =  EZ_SliderPositionMin(wptr);
              if(EZ_SliderForeground(wptr) != EZ_DefaultForeground || 
                 EZ_SliderFont(wptr) != EZ_GetFontFromId(EZ_BUTTON_FONT))
                {
                  XSetFont(EZ_Display, EZ_WRITABLEGC, EZ_SliderFont(wptr)->fid);
                  XSetForeground(EZ_Display, EZ_WRITABLEGC, EZ_SliderForeground(wptr));	
                  gc = EZ_WRITABLEGC;
                }
              else  gc = EZ_BUTTON_TEXTGC;

              for(dd=0.0, i = 0; i <= nticks; i++, dd+=dtmp)
                {
                  int hhh = (i&1)? tick2: tick1;
                  xx = x + (int)(dd);
                  XDrawLine(EZ_Display,pixmap,gc, xx, yy,xx,yy+hhh);
                }
              /* XDrawLine(EZ_Display,pixmap,gc, x+1, yy+1,xx+1,yy+1);*/
            }
        }
    }
  /*---------------------------------------------------------
   *     Draw the slider.
   *--------------------------------------------------------*/
  x = cw;
  y = ch + bs + ss + EZ_SliderTHeight(wptr);
  if(nocvalue == 0) y += EZ_SliderVHeight(wptr);
  
  ww = EZ_SliderLength(wptr);
  hh = EZ_SliderWidth(wptr);

  switch(sliderStyle)
    {
    case 0:
    case 1:
      {
	EZ_DrawRectBorderWithSize(wptr, pixmap, x,y,
				  ww + (EZ_SliderBorderWidth(wptr)<< 1),
				  hh + (EZ_SliderBorderWidth(wptr)<< 1),
				  EZ_SliderBorderWidth(wptr),
				  EZ_BORDER_DOWN,0,0,0);
  
	x =  EZ_SliderPosition(wptr) - (EZ_SliderSliderLength(wptr)>>1);
	y += EZ_SliderBorderWidth(wptr);

	if(EZ_WidgetBGPixmapB(wptr) != NULL)
	  {
	    EZ_Bitmap *bitmap = EZ_WidgetBGPixmapB(wptr);
	    gc = EZ_NTILEGC2;
	    if(highlight) XSetTile(EZ_Display,EZ_NTILEGC2,bitmap->htpixmap);
            else XSetTile(EZ_Display,EZ_NTILEGC2,bitmap->pixmap);
	    XSetTSOrigin(EZ_Display, EZ_NTILEGC2, x, y);
	  }
	else if(EZ_WidgetParentBGPixmapB(wptr) != NULL)
	  {
	    EZ_Bitmap *bitmap = EZ_WidgetParentBGPixmapB(wptr);
	    gc = EZ_NTILEGC2;
	    if(highlight) XSetTile(EZ_Display,EZ_NTILEGC2,bitmap->htpixmap);
	    else XSetTile(EZ_Display,EZ_NTILEGC2,bitmap->pixmap);
	    XSetTSOrigin(EZ_Display, EZ_NTILEGC2, x, y);
	  }
	else
	  {
            int tile;
            if(swapBg) { EZ_InstallHighlightColor(wptr); } /*4/29/99*/
            tile = EZ_GetBackgroundGC(wptr,&gc, &bgpv, 0, highlight);
	    if(tile) 
	      {
		int ox, oy;
		EZ_GetWidgetTileOrigin(wptr, &ox, &oy);      
		XSetTSOrigin(EZ_Display, gc, ox, oy);
	      }
	  }  
	XFillRectangle(EZ_Display, pixmap, gc,
		       x, y, EZ_SliderSliderLength(wptr), hh);
	if(EZ_SliderBorderStyle(wptr) == EZ_BORDER_DOWN) offset = 1;
	yy = y + offset +EZ_SliderBorderWidth(wptr);

	if(sliderStyle == 1)
	  {
	    int iw = EZ_SliderSliderLength(wptr) - (EZ_SliderBorderWidth(wptr)<<1);
	    int ih = hh -  (EZ_SliderBorderWidth(wptr)<<1);
	    int ix = x + EZ_SliderBorderWidth(wptr) + offset;
	    iw = (iw>>1) -2;
	    EZ_OverlayDragablePixmap(wptr, pixmap, ix, yy, iw, ih);
	    EZ_OverlayDragablePixmap(wptr, pixmap, ix+iw+4, yy, iw, ih);
	  }
	xx = EZ_SliderPosition(wptr) + offset - 1;
	EZ_SetupUpBdGC(wptr, &gc1, &gc1, &gc1, &gc1,&gc, &gc, x, y, 1);
	XDrawLine(EZ_Display,pixmap, gc,
		  xx, yy,
		  xx, yy+ EZ_SliderWidth(wptr) - (EZ_SliderBorderWidth(wptr)<<1));
	XDrawLine(EZ_Display,pixmap, gc1,
		  xx+2, yy,
		  xx+2, yy+ EZ_SliderWidth(wptr) - (EZ_SliderBorderWidth(wptr)<<1));
	EZ_DrawRectBorderWithSize(wptr, pixmap, 
				  x,
				  y,
				  EZ_SliderSliderLength(wptr),
				  hh,
				  EZ_SliderBorderWidth(wptr),
				  EZ_SliderBorderStyle(wptr),x,y,1);
      }
    break;
    case 2:
    case 3:
      {
        int sbw = EZ_SliderBorderWidth(wptr);
	xx = cw;
	yy = y + (hh>>1) -2;

	EZ_SetupUpBdGC(wptr, &gc1, &gc1, &gc1, &gc1,&gc, &gc, xx, yy, 1);
	XFillRectangle(EZ_Display, pixmap, gc, xx+sbw,yy, ww, 4);
	EZ_DrawRectBorderWithSize(wptr, pixmap, xx+sbw,yy,
				  ww, 4, 1, EZ_BORDER_DOWN,0,0,0);
	x =  EZ_SliderPosition(wptr) - (EZ_SliderSliderLength(wptr)>>1);

        if(swapBg) { EZ_InstallHighlightColor(wptr); } /*4/29/99*/
	if(EZ_WidgetBGPixmapB(wptr) != NULL)
	  {
	    EZ_Bitmap *bitmap = EZ_WidgetBGPixmapB(wptr);
	    gc = EZ_NTILEGC2;
	    if(highlight) XSetTile(EZ_Display,EZ_NTILEGC2,bitmap->htpixmap);
	    else XSetTile(EZ_Display,EZ_NTILEGC2,bitmap->pixmap);
	    XSetTSOrigin(EZ_Display, EZ_NTILEGC2, x, y);
	  }
	else if(EZ_WidgetParentBGPixmapB(wptr) != NULL)
	  {
	    EZ_Bitmap *bitmap = EZ_WidgetParentBGPixmapB(wptr);
	    gc = EZ_NTILEGC2;
	    if(highlight) XSetTile(EZ_Display,EZ_NTILEGC2,bitmap->htpixmap);
	    else XSetTile(EZ_Display,EZ_NTILEGC2,bitmap->pixmap);
	    XSetTSOrigin(EZ_Display, EZ_NTILEGC2, x, y);
	  }
	else
	  {
	    int tile;
            if(swapBg) { EZ_InstallHighlightColor(wptr); } /*4/29/99*/
            tile  = EZ_GetBackgroundGC(wptr,&gc, &bgpv, 0, highlight);
	    if(tile) 
	      {
		int ox, oy;
		EZ_GetWidgetTileOrigin(wptr, &ox, &oy);      
		XSetTSOrigin(EZ_Display, gc, ox, oy);
	      }
	  }  
	XFillRectangle(EZ_Display, pixmap, gc,
		       x, y, EZ_SliderSliderLength(wptr), hh);
	yy = y + sbw;
	if(sliderStyle == 3)
	  {
	    int iw = EZ_SliderSliderLength(wptr) - (sbw+sbw);
	    int ih = hh -  (sbw+sbw);
	    int ix = x + sbw;
	    iw = (iw>>1) -2;
	    EZ_OverlayDragablePixmap(wptr, pixmap, ix, yy, iw, ih);
	    EZ_OverlayDragablePixmap(wptr, pixmap, ix+iw+4, yy, iw, ih);
	  }
	xx = EZ_SliderPosition(wptr) - 1;
	EZ_SetupUpBdGC(wptr, &gc1, &gc1, &gc1, &gc1,&gc, &gc, x, y, 1);
	XDrawLine(EZ_Display,pixmap, gc,
		  xx, yy,
		  xx, yy+ EZ_SliderWidth(wptr) - (sbw+sbw));
	XDrawLine(EZ_Display,pixmap, gc1,
		  xx+2, yy,
		  xx+2, yy+ EZ_SliderWidth(wptr) - (sbw+sbw));
	EZ_DrawRectBorderWithSize(wptr, pixmap, 
				  x,
				  y,
				  EZ_SliderSliderLength(wptr),
				  hh,
				  sbw,
				  EZ_BORDER_UP,x,y,1);
      }
    break;
    case 4:
    case 5:
    case 6:
    case 7:
      {
        int sbw = EZ_SliderBorderWidth(wptr);
        int btnBorder = EZ_HSLIDER_BORDER;
        if(sliderStyle > 5) btnBorder = EZ_BORDER_UP;
	xx = cw;
	yy = y + (hh>>1) -2;
        hh += sbw;

	EZ_SetupUpBdGC(wptr, &gc1, &gc1, &gc1, &gc1,&gc, &gc, xx, yy, 1);
	XFillRectangle(EZ_Display, pixmap, gc, xx+sbw,yy, ww, 4);
	EZ_DrawRectBorderWithSize(wptr, pixmap, xx+sbw,yy,
				  ww, 4, 1, EZ_BORDER_DOWN,0,0,0);
	x =  EZ_SliderPosition(wptr) - (EZ_SliderSliderLength(wptr)>>1);

        if(swapBg) { EZ_InstallHighlightColor(wptr); } /*4/29/99*/
	if(EZ_WidgetBGPixmapB(wptr) != NULL)
	  {
	    EZ_Bitmap *bitmap = EZ_WidgetBGPixmapB(wptr);
	    gc = EZ_NTILEGC2;
	    if(highlight) XSetTile(EZ_Display,EZ_NTILEGC2,bitmap->htpixmap);
	    else XSetTile(EZ_Display,EZ_NTILEGC2,bitmap->pixmap);
	    XSetTSOrigin(EZ_Display, EZ_NTILEGC2, x, y);
	  }
	else if(EZ_WidgetParentBGPixmapB(wptr) != NULL)
	  {
	    EZ_Bitmap *bitmap = EZ_WidgetParentBGPixmapB(wptr);
	    gc = EZ_NTILEGC2;
	    if(highlight) XSetTile(EZ_Display,EZ_NTILEGC2,bitmap->htpixmap);
	    else XSetTile(EZ_Display,EZ_NTILEGC2,bitmap->pixmap);
	    XSetTSOrigin(EZ_Display, EZ_NTILEGC2, x, y);
	  }
	else
	  {
	    int tile;
            if(swapBg) { EZ_InstallHighlightColor(wptr); } /*4/29/99*/
            tile  = EZ_GetBackgroundGC(wptr,&gc, &bgpv, 0, highlight);
	    if(tile) 
	      {
		int ox, oy;
		EZ_GetWidgetTileOrigin(wptr, &ox, &oy);      
		XSetTSOrigin(EZ_Display, gc, ox, oy);
	      }
	  }  
	XFillRectangle(EZ_Display, pixmap, gc,
		       x, y, EZ_SliderSliderLength(wptr), hh);
	yy = y + sbw;
	if(sliderStyle & 1)
	  {
	    int iw = EZ_SliderSliderLength(wptr) - (sbw+sbw);
	    int ih = hh -  (sbw+sbw);
	    int ix = x + sbw;
	    EZ_OverlayDragablePixmap(wptr, pixmap, ix, yy, iw, ih);
	  }
	EZ_DrawRectBorderWithSize(wptr, pixmap, 
				  x,
				  y,
				  EZ_SliderSliderLength(wptr),
				  hh,
				  sbw,
				  btnBorder,x,y, 1);
      }
    default:
      break;
    }
  if(swapBg) { EZ_UninstallHighlightColor(wptr); } /*4/29/99*/
  EZ_DrawRectBorder(wptr, pixmap);
  XCopyArea(EZ_Display,pixmap,win, EZ_WRITABLEGC,0,0,w,h,0,0); 
  XFreePixmap(EZ_Display, pixmap);                             
}

/********************************************************************
 *
 *  Figure out the dimension of a vertical slider
 */
void EZ_ComputeWidgetVSliderSize(widget, w, h)
    EZ_Widget *widget;
     int             *w, *h;
{
  int length, width, cw, ch,itmp1,itmp2, bs,ss, tmp, dvalue, nticks, ticksize,style;
  int  itmp3, nocvalue;

  dvalue = (int)EZ_SliderDisplayValue(widget);
  nocvalue = (int)EZ_SliderNoCValue(widget);
  bs = (EZ_SliderLabelLength(widget) != 0)? 2 : 0;

  ss = (dvalue != 0) ? 2 : 0;
  nticks = (int) EZ_SliderNTicks(widget);
  ticksize = (nticks )? (int) EZ_SliderTickSize(widget): 0;
  
  if(!(EZ_GetWidgetSizeComputedFlag(widget)))
    { 
      style = (int) EZ_SliderStyle(widget);
      tmp = ( (EZ_SliderBorderWidth(widget))<< 1) + 4;      
      if(style < 4)
        {
          if(EZ_SliderWidth(widget) < tmp) EZ_SliderWidth(widget) =  tmp;
          if(EZ_SliderSliderLength(widget) < tmp + tmp) EZ_SliderSliderLength(widget) = tmp + tmp;
        }
      else
        {
          int jtmp = (tmp-3) << 2;
          if(EZ_SliderWidth(widget) < jtmp) EZ_SliderWidth(widget) = jtmp;
          EZ_SliderSliderLength(widget) = (EZ_SliderBorderWidth(widget) +EZ_SliderWidth(widget))>>1;
        }

      if( EZ_SliderLabelLength(widget) != 0)
	{
	  EZ_SliderTWidth(widget) = XTextWidth(EZ_SliderFont(widget),
					       EZ_SliderLabel(widget),
					       EZ_SliderLabelLength(widget));
	  EZ_SliderTHeight(widget) = EZ_SliderFont(widget)->ascent +
	    EZ_SliderFont(widget)->descent;
	  EZ_SliderFontAscent(widget) = EZ_SliderFont(widget)->ascent;
	}
      else
        {
	  EZ_SliderTWidth(widget) = 0;
	  EZ_SliderTHeight(widget) = 0;
        }
      EZ_SliderFontAscent(widget) = EZ_SliderFont(widget)->ascent;

      if(dvalue)
	{
	  char tmp[32];

          EZ_SetPrintFFormat(EZ_SliderMaxValue(widget), EZ_SliderMinValue(widget),
                             EZ_SliderResolution(widget), (float)EZ_SliderLength(widget),
                             EZ_SliderFormat(widget));
          
	  (void)sprintf(tmp, EZ_SliderFormat(widget), EZ_SliderMaxValue(widget));
	  EZ_SliderVWidth(widget) = XTextWidth(EZ_SliderFont(widget),
					       tmp,
					       strlen(tmp));
	  (void)sprintf(tmp, EZ_SliderFormat(widget), EZ_SliderMinValue(widget));
	  EZ_SliderVWidtha(widget) = XTextWidth(EZ_SliderFont(widget),
						tmp,
						strlen(tmp));

	  EZ_SliderVHeight(widget) = EZ_SliderFont(widget)->ascent +
	    EZ_SliderFont(widget)->descent;
	  EZ_SliderFontAscent(widget) = EZ_SliderFont(widget)->ascent;
	}
      else
        {
          EZ_SliderVWidth(widget) = 0;
          EZ_SliderVWidtha(widget) = 0;
          EZ_SliderVHeight(widget) = 0;
        }
      EZ_SetWidgetSizeComputedFlag(widget);
    }
  itmp1 = EZ_SliderTWidth(widget);
  itmp3 = MAXV(EZ_SliderVWidth(widget), EZ_SliderVWidtha(widget));
  itmp2 = EZ_SliderWidth(widget)+ ((EZ_SliderBorderWidth(widget) + ss)<<1);
  if(dvalue > 1) itmp2 += itmp3;
  if(nocvalue == 0) itmp2 += itmp3;
  width = MAXV(itmp1,itmp2);
  length = EZ_SliderTHeight(widget) + EZ_SliderSliderLength(widget) + 4 +
    (EZ_SliderVHeight(widget)<<1);
  
  EZ_SliderLength(widget) = length;
  
  cw = EZ_WidgetPadX(widget) + EZ_WidgetBorderWidth(widget) + EZ_WidgetPadB(widget)
    + EZ_SliderBorderWidth(widget) ;
  ch = EZ_WidgetPadY(widget) + EZ_WidgetBorderWidth(widget) + EZ_WidgetPadB(widget)
    + EZ_SliderBorderWidth(widget);

  /*----------------------------
   *  Dimension of the X window
   *----------------------------*/
  ch = ch + ch + EZ_SliderLength(widget) +  EZ_SliderTHeight(widget) + bs;
  cw = cw + cw + width + ticksize;

  *w = cw;
  *h = ch;

  /* EZ_InitialVSliderInternalData(widget,cw,ch);*/
  EZ_SetWidgetSizeMayBeChangedFlag(widget);
}

/**********************************************************************************
 *
 *  Initialize the internal data for a V slider.
 */
static void  EZ_InitialVSliderInternalData(widget,w,h)
     EZ_Widget *widget;
     int             w,h;
{
  int bw, cw, ch, sl, tmp,itmp1,itmp2, bs,ss,padb, dvalue, nticks, ticksize, style, nocvalue;

  bs = (EZ_SliderLabelLength(widget) != 0)? 2 : 0;
  dvalue = (int)EZ_SliderDisplayValue(widget);
  nocvalue = (int)EZ_SliderNoCValue(widget);
  ss = (dvalue != 0) ? 2 : 0;
  nticks = (int)EZ_SliderNTicks(widget);
  ticksize = (nticks) ? (int)EZ_SliderTickSize(widget): 0;
  style = EZ_SliderStyle(widget);

  padb = EZ_WidgetPadB(widget);
  bw = EZ_WidgetBorderWidth(widget);
  cw = padb+ EZ_WidgetPadX(widget) + bw + EZ_SliderBorderWidth(widget) ;
  ch = padb +EZ_WidgetPadY(widget) + bw + EZ_SliderBorderWidth(widget) ;

  sl = EZ_SliderLength(widget) = h - (ch <<1) - bs - EZ_SliderTHeight(widget);
  itmp1 = EZ_SliderVWidth(widget);
  itmp2 = EZ_SliderVWidtha(widget);
  tmp = MAXV(itmp1,itmp2);
  /*
  if(!EZ_SliderWidthSet(widget))
    {
      if(style < 4)
        {
          if(bs == 0)
            {
              EZ_SliderWidth(widget) = w -( (cw + ss) << 1) - ticksize;
              if(nocvalue == 0) EZ_SliderWidth(widget) -= tmp;
              if(dvalue > 1) EZ_SliderWidth(widget) -= tmp;
            }
        }
    }
    */
 /*-----------------------------------------------
   * min scrn coor of slider and the slider window
   *----------------------------------------------*/
  EZ_SliderPositionMin(widget) = ch + EZ_SliderTHeight(widget) +
    (EZ_SliderSliderLength(widget)>>1) + bs;
  EZ_SliderMinx(widget) = cw + EZ_SliderBorderWidth(widget);
  EZ_SliderMaxx(widget) = cw +EZ_SliderWidth(widget) + EZ_SliderBorderWidth(widget);
  if(nocvalue == 0)
    {
      EZ_SliderMinx(widget) += tmp;
      EZ_SliderMaxx(widget) += tmp;
    }
  if(style > 1) EZ_SliderMaxx(widget) -=  EZ_SliderBorderWidth(widget);

  EZ_SliderMiny(widget) = EZ_SliderPositionMin(widget) -  (EZ_SliderSliderLength(widget)>>1);
  EZ_SliderMaxy(widget) = h - ch;
  
  /*--------------------------
   * compute scaling factor
   *--------------------------*/
  if( (tmp = (sl - EZ_SliderSliderLength(widget))) <= 0) tmp = 1;
  EZ_SliderFactor(widget) = (EZ_SliderMaxValue(widget) - EZ_SliderMinValue(widget))/
    (float)tmp;

  /*-------------------------------------------
   * Set initial slider position
   *------------------------------------------*/
  EZ_SliderPosition(widget) = (int)
    (EZ_SliderPositionMin(widget) +
     (EZ_SliderValue(widget) -EZ_SliderMinValue(widget))/EZ_SliderFactor(widget));
}	  

/**************************************************************************/

void  EZ_DrawWidgetVerticalSlider(wptr)
     EZ_Widget *wptr;
{
  int             w, h,x,y,xx,yy, ww, hh, ch,cw,bs,ss, padb,padb2, tile;
  Pixmap          pixmap;
  Window          win;
  GC              gc, gc1;
  int             offset = 0, sliderStyle, tox=0,toy=0, highlight, swapBg;
  int             sbw, bw, nticks,ticksize,tick1,tick2,dvalue, nocvalue;
  unsigned long   bgpv;

  win = EZ_WidgetWindow(wptr);
  w   = EZ_WidgetWidth(wptr);
  h   = EZ_WidgetHeight(wptr);
  padb = EZ_WidgetPadB(wptr);
  padb2 = padb + padb;
  sliderStyle = EZ_SliderStyle(wptr);
  highlight = EZ_GetWidgetHighlightFlag(wptr);
  swapBg = EZ_SliderHighlight(wptr);
  dvalue = (int) EZ_SliderDisplayValue(wptr);
  nocvalue = (int)EZ_SliderNoCValue(wptr);
  bs = (EZ_SliderLabelLength(wptr) != 0)? 2 : 0;
  ss = (dvalue != 0) ? 2 : 0;

  if(EZ_WidgetSizeMayBeChangedFlag(wptr))
    {
      EZ_InitialVSliderInternalData(wptr,w,h);
      EZ_ClearWidgetSizeMayBeChangedFlag(wptr);
    }
  /*-----------------------------------------------------------
   *  Create a pixmap, draw into this pixmap in background and
   *  copy to the button window when finished.
   *----------------------------------------------------------*/
  pixmap = XCreatePixmap(EZ_Display, win, w, h, EZ_Depth);    
  EZ_FixFocusHighlightBorder(wptr, pixmap, w, h, padb);

  tile = EZ_GetBackgroundGC(wptr, &gc, &bgpv, 0, 0);  
  if(tile)
    {
      EZ_GetWidgetTileOrigin(wptr, &tox, &toy);      
      XSetTSOrigin(EZ_Display, gc, tox, toy);
    }
  XFillRectangle(EZ_Display, pixmap, gc, padb,padb, w-padb2, h-padb2); 

  bw = EZ_WidgetBorderWidth(wptr);
  cw = bw + EZ_WidgetPadX(wptr) + padb;  
  ch = bw + EZ_WidgetPadY(wptr) + padb;  
  sbw = EZ_SliderBorderWidth(wptr);

  nticks = (int) EZ_SliderNTicks(wptr);
  ticksize = (nticks) ? (int)EZ_SliderTickSize(wptr): 0;
  if(nticks)
    {
      tick1 =(ticksize-4);
      tick2= (tick1>>1);
    }
  else tick1=tick2=0;
  /*--------------------------------------------------------
   *  Draw label and values.
   *-------------------------------------------------------*/
  if(1/*dvalue || EZ_SliderLabelLength(wptr) != 0*/)
    {
      if( EZ_GetWidgetDisabledFlag(wptr) )
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

	  if(EZ_SliderLabelLength(wptr) != 0)
	    {
	      x = ((w - EZ_SliderTWidth(wptr)) >> 1);
	      y = EZ_SliderFontAscent(wptr) + ch;
	  
	      XDrawString(EZ_Display, pixmap, bgc,
			  x+1, y+1,  EZ_SliderLabel(wptr), EZ_SliderLabelLength(wptr));
	      XDrawString(EZ_Display, pixmap, lgc,
			  x-1, y-1,  EZ_SliderLabel(wptr), EZ_SliderLabelLength(wptr));
	      XDrawString(EZ_Display, pixmap, ngc,
			  x, y,  EZ_SliderLabel(wptr), EZ_SliderLabelLength(wptr));
	    }
	  if(dvalue)
	    {
	      char tmp[32];
              /* display slider value */
              if(nocvalue == 0)
                {
                  (void) sprintf(tmp, EZ_SliderFormat(wptr),EZ_SliderValue(wptr)); 
                  x =  EZ_SliderMinx(wptr) - EZ_SliderVWidtha(wptr) - ss - sbw;
                  /*x =  EZ_SliderMaxx(wptr) + (ss<1) + bw */
                  y =  EZ_SliderPosition(wptr) + ( EZ_SliderFontAscent(wptr)>>1);
                  XDrawString(EZ_Display, pixmap, bgc,  x+1, y+1, tmp,  strlen(tmp));	  
                  XDrawString(EZ_Display, pixmap, lgc,  x-1, y-1, tmp,  strlen(tmp));	  
                  XDrawString(EZ_Display, pixmap, ngc,  x, y, tmp,  strlen(tmp));	  
                }
              
              /* display tick values */
              if(dvalue >= 2 )
                {	      
                  int nnn, i, count, vwidth, vwidtha, tswidth, vheight;
                  float frange = (EZ_SliderMaxValue(wptr) - EZ_SliderMinValue(wptr));
                  float irange = frange / EZ_SliderFactor(wptr);
                  float dd, dtmp, dv, dvtmp;
                  nnn = (nticks > dvalue? nticks: dvalue);
                  vheight = EZ_SliderVHeight(wptr) + 4;
                  vwidth =EZ_SliderVWidth(wptr);
                  vwidtha =EZ_SliderVWidtha(wptr);
                  if(vwidth < vwidtha) vwidth = vwidtha;
                  tswidth = (int)irange;
                  count = tswidth/vheight;     
                  if(count < nnn)  nnn = count;
                  if(nticks) nticks = nnn;

                  dtmp =  irange/(float)nnn;
                  dvtmp = (EZ_SliderMaxValue(wptr)-EZ_SliderMinValue(wptr))/nnn;
                  x =  EZ_SliderMaxx(wptr) + (ss<1) + bw + sbw + ticksize;
                  y =  EZ_SliderPositionMin(wptr) + ( EZ_SliderFontAscent(wptr)>>1);                  
                  for(dd=0.0, dv=0.0, i = 0; i <=nnn; i++, dd+=dtmp, dv+=dvtmp)
                    {
                      yy = y + (int)(dd);
                      (void) sprintf(tmp, EZ_SliderFormat(wptr),EZ_SliderMinValue(wptr) + dv);
                      XDrawString(EZ_Display, pixmap, bgc, x+1, yy+1, tmp, strlen(tmp));
                      XDrawString(EZ_Display, pixmap, lgc, x-1, yy-1, tmp, strlen(tmp));
                      XDrawString(EZ_Display, pixmap, ngc, x, yy, tmp, strlen(tmp));
                    }
                }
	    }
          if(nticks > 0)
            {
              int i;
              float dd, dtmp, irange=(EZ_SliderMaxValue(wptr) - EZ_SliderMinValue(wptr))/ EZ_SliderFactor(wptr);
              nticks += nticks;
              dtmp =  irange/(float)nticks;
              y =  EZ_SliderPositionMin(wptr);
              xx =  EZ_SliderMaxx(wptr) + ss + sbw;
              for(dd=0.0, i = 0; i <= nticks; i++, dd+=dtmp)
                {
                  int hhh = (i&1)? tick2: tick1;
                  yy = y + (int)(dd);
                  XDrawLine(EZ_Display,pixmap,bgc, xx+1, yy+1,xx+hhh+1,yy+1);
                  XDrawLine(EZ_Display,pixmap,lgc, xx-1, yy-1,xx+hhh-1,yy-1);
                  XDrawLine(EZ_Display,pixmap,gc, xx, yy,xx+hhh,yy);
                }
            }
	}
      else /* enabled */
	{
	  if(EZ_SliderForeground(wptr) != EZ_DefaultForeground || 
	     EZ_SliderFont(wptr) != EZ_GetFontFromId(EZ_BUTTON_FONT))
	    {
	      XSetFont(EZ_Display, EZ_WRITABLEGC, EZ_SliderFont(wptr)->fid);
	      XSetForeground(EZ_Display, EZ_WRITABLEGC, EZ_SliderForeground(wptr));	
	      gc = EZ_WRITABLEGC;
	    }
	  else  gc = EZ_BUTTON_TEXTGC;

	  if(EZ_SliderLabelLength(wptr) != 0)
	    {
              int shadow = EZ_WidgetLabelShadow(wptr);
	      x = ((w - EZ_SliderTWidth(wptr)) >> 1);
	      y = EZ_SliderFontAscent(wptr) + ch;
              if(shadow)
                {
                  int shadowx, shadowy;
                  if(shadow &0xf0)
                    {
                      shadowx = (shadow & 0xf0) >> 4;
                      shadowy = (shadow & 0x0f);
                    }
                  else shadowx = shadowy = shadow;
                  if(shadowx & 8) shadowx= (8-shadowx);
                  if(shadowy & 8) shadowy= (8-shadowy);
                  XSetFont(EZ_Display, EZ_SHADOWGC, EZ_SliderFont(wptr)->fid);
                  XDrawString(EZ_Display, pixmap, EZ_SHADOWGC,
                              x+shadowx, y+shadowy,
                              EZ_SliderLabel(wptr),
                              EZ_SliderLabelLength(wptr));
                }	  

	      XDrawString(EZ_Display, pixmap, gc, 
			  x, y,
			  EZ_SliderLabel(wptr),
			  EZ_SliderLabelLength(wptr));
	    }
	  if(dvalue)
	    {
	      char tmp[32];
              /* display slider value */
              if(nocvalue == 0)
                {
                  (void) sprintf(tmp, EZ_SliderFormat(wptr),EZ_SliderValue(wptr)); 
                  x =  EZ_SliderMinx(wptr) - EZ_SliderVWidtha(wptr) - ss - sbw;
                  /*x =  EZ_SliderMaxx(wptr) + (ss<1) + bw */
                  y =  EZ_SliderPosition(wptr) + ( EZ_SliderFontAscent(wptr)>>1);
                  XDrawString(EZ_Display, pixmap, gc,  x, y, tmp,  strlen(tmp));	  
                }
              /* display tick values */
              if(dvalue >= 2 )
                {	      
                  int nnn, i, count, vwidth, vwidtha, tswidth, vheight;
                  float frange = (EZ_SliderMaxValue(wptr) - EZ_SliderMinValue(wptr));
                  float irange = frange / EZ_SliderFactor(wptr);
                  float dd, dtmp, dv, dvtmp;
                  nnn = (nticks > dvalue? nticks: dvalue);
                  vheight = EZ_SliderVHeight(wptr) + 4;
                  vwidth =EZ_SliderVWidth(wptr);
                  vwidtha =EZ_SliderVWidtha(wptr);
                  if(vwidth < vwidtha) vwidth = vwidtha;
                  tswidth = (int)irange;
                  count = tswidth/vheight;     
                  if(count < nnn)  nnn = count;
                  if(nticks) nticks = nnn;

                  dtmp =  irange/(float)nnn;
                  dvtmp = (EZ_SliderMaxValue(wptr)-EZ_SliderMinValue(wptr))/nnn;
                  x =  EZ_SliderMaxx(wptr) + (ss<1) + bw + sbw + ticksize;
                  y =  EZ_SliderPositionMin(wptr) + ( EZ_SliderFontAscent(wptr)>>1);                  
                  for(dd=0.0, dv=0.0, i = 0; i <=nnn; i++, dd+=dtmp, dv+=dvtmp)
                    {
                      yy = y + (int)(dd);
                      (void) sprintf(tmp, EZ_SliderFormat(wptr),EZ_SliderMinValue(wptr) + dv);
                      XDrawString(EZ_Display, pixmap, gc, x, yy, tmp, strlen(tmp));
                    }
                }
	    }
          if(nticks > 0)
            {
              int i;
              float dd, dtmp, irange=(EZ_SliderMaxValue(wptr) - EZ_SliderMinValue(wptr))/ EZ_SliderFactor(wptr);
              nticks += nticks;
              dtmp =  irange/(float)nticks;
              y =  EZ_SliderPositionMin(wptr);
              xx =  EZ_SliderMaxx(wptr) + ss + sbw;
              for(dd=0.0, i = 0; i <= nticks; i++, dd+=dtmp)
                {
                  int hhh = (i&1)? tick2: tick1;
                  yy = y + (int)(dd);
                  XDrawLine(EZ_Display,pixmap,gc, xx, yy,xx+hhh,yy);
                }
            }
	}
    }
  /*---------------------------------------------------------
   *     Draw the slider.
   *--------------------------------------------------------*/


  x = EZ_SliderMinx(wptr) - EZ_SliderBorderWidth(wptr);
  y = EZ_SliderMiny(wptr) - EZ_SliderBorderWidth(wptr);

  hh = EZ_SliderLength(wptr);
  ww = EZ_SliderWidth(wptr);

  switch(sliderStyle)
    {
    case 0:
    case 1:
      EZ_DrawRectBorderWithSize(wptr, pixmap, x,y,
				ww + (sbw<< 1),
				hh + (sbw<< 1),
				EZ_SliderBorderWidth(wptr),
				EZ_BORDER_DOWN,0,0,0);
  
      x += EZ_SliderBorderWidth(wptr);
      y =  EZ_SliderPosition(wptr) - (EZ_SliderSliderLength(wptr)>>1); 

      if(EZ_WidgetBGPixmapB(wptr) != NULL)
	{
	  EZ_Bitmap *bitmap = EZ_WidgetBGPixmapB(wptr);
	  gc = EZ_NTILEGC2;
	  if(highlight) XSetTile(EZ_Display,EZ_NTILEGC2,bitmap->htpixmap);
	  else XSetTile(EZ_Display,EZ_NTILEGC2,bitmap->pixmap);
	  XSetTSOrigin(EZ_Display, EZ_NTILEGC2, x, y);
	}
      else if(EZ_WidgetParentBGPixmapB(wptr) != NULL)
	{
	  EZ_Bitmap *bitmap = EZ_WidgetParentBGPixmapB(wptr);
	  gc = EZ_NTILEGC2;
	  if(highlight) XSetTile(EZ_Display,EZ_NTILEGC2,bitmap->htpixmap);
	  else XSetTile(EZ_Display,EZ_NTILEGC2,bitmap->pixmap);
	  XSetTSOrigin(EZ_Display, EZ_NTILEGC2, x, y);
	}
      else
	{
          int tile;
          if(swapBg) { EZ_InstallHighlightColor(wptr); } /*4/29/99*/
	  tile = EZ_GetBackgroundGC(wptr,&gc, &bgpv, 0, highlight);
	  if(tile) 
	    {
	      int ox, oy;
	      EZ_GetWidgetTileOrigin(wptr, &ox, &oy);      
	      XSetTSOrigin(EZ_Display, gc, ox, oy);
	    }
	}  
      XFillRectangle(EZ_Display, pixmap, gc,
		     x, y,
		     ww,EZ_SliderSliderLength(wptr));

      if(EZ_SliderBorderStyle(wptr) == EZ_BORDER_DOWN) offset = 1;

      xx = x + offset +sbw;
      if(sliderStyle == 1)
	{
	  int iw = ww - (sbw<<1);
	  int ih = EZ_SliderSliderLength(wptr) -  (sbw<<1);
	  yy = y + sbw + offset;
	  ih = ih/2 -2;
	  EZ_OverlayDragablePixmap(wptr, pixmap, xx, yy, iw, ih);      
	  EZ_OverlayDragablePixmap(wptr, pixmap, xx, yy+ih+4, iw, ih);      
	}
      yy =  EZ_SliderPosition(wptr) + offset - 1;
      EZ_SetupUpBdGC(wptr, &gc1, &gc1, &gc1, &gc1,&gc, &gc, x, y, 1);
      XDrawLine(EZ_Display,pixmap, gc,
		xx, yy,
		xx+ EZ_SliderWidth(wptr) - (sbw<<1), 
		yy);
      XDrawLine(EZ_Display,pixmap, gc1,
		xx, yy+2,
		xx+ EZ_SliderWidth(wptr) - (sbw<<1), 
		yy+2);
      EZ_DrawRectBorderWithSize(wptr, pixmap, 
				x,
				y,
				ww,
				EZ_SliderSliderLength(wptr),
                                sbw,
				EZ_SliderBorderStyle(wptr),0,0,1);
      break;
    case 2:
    case 3:
      {
	int ix = x + (ww>>1) - 2;
	int iy = y;
	EZ_SetupUpBdGC(wptr, &gc1, &gc1, &gc1, &gc1,&gc, &gc, ix, iy, 1);
	XFillRectangle(EZ_Display, pixmap, gc, ix,iy +sbw, 4, hh);
	EZ_DrawRectBorderWithSize(wptr, pixmap, ix,iy +sbw, 4,hh,
				  1, EZ_BORDER_DOWN,0,0,0);
  
	y =  EZ_SliderPosition(wptr) - (EZ_SliderSliderLength(wptr)>>1); 

	if(EZ_WidgetBGPixmapB(wptr) != NULL)
	  {
	    EZ_Bitmap *bitmap = EZ_WidgetBGPixmapB(wptr);
	    gc = EZ_NTILEGC2;
            if(highlight) XSetTile(EZ_Display,EZ_NTILEGC2,bitmap->htpixmap);
	    else XSetTile(EZ_Display,EZ_NTILEGC2,bitmap->pixmap);
	    XSetTSOrigin(EZ_Display, EZ_NTILEGC2, x, y);
	  }
	else if(EZ_WidgetParentBGPixmapB(wptr) != NULL)
	  {
	    EZ_Bitmap *bitmap = EZ_WidgetParentBGPixmapB(wptr);
	    gc = EZ_NTILEGC2;
            if(highlight) XSetTile(EZ_Display,EZ_NTILEGC2,bitmap->htpixmap);
            else XSetTile(EZ_Display,EZ_NTILEGC2,bitmap->pixmap);
	    XSetTSOrigin(EZ_Display, EZ_NTILEGC2, x, y);
	  }
	else
	  {
            int tile;
            if(swapBg) { EZ_InstallHighlightColor(wptr); } /*4/29/99*/
	    tile = EZ_GetBackgroundGC(wptr,&gc, &bgpv, 0, highlight);
	    if(tile) 
	      {
		int ox, oy;
		EZ_GetWidgetTileOrigin(wptr, &ox, &oy);      
		XSetTSOrigin(EZ_Display, gc, ox, oy);
	      }
	  }  
	XFillRectangle(EZ_Display, pixmap, gc,
		       x, y,
		     ww,EZ_SliderSliderLength(wptr));

	xx = x + sbw;
	if(sliderStyle == 3)
	  {
	    int iw = ww - (sbw+sbw);
	    int ih = EZ_SliderSliderLength(wptr) -  (sbw+sbw);
	    yy = y + sbw + offset;
	    ih = ih/2 -2;
	    EZ_OverlayDragablePixmap(wptr, pixmap, xx, yy, iw, ih);      
	    EZ_OverlayDragablePixmap(wptr, pixmap, xx, yy+ih+4, iw, ih);      
	  }
	yy =  EZ_SliderPosition(wptr) - 1;
	EZ_SetupUpBdGC(wptr, &gc1, &gc1, &gc1, &gc1,&gc, &gc, x, y, 1);
	XDrawLine(EZ_Display,pixmap, gc,
		  xx, yy,
		  xx+ EZ_SliderWidth(wptr) - (sbw+sbw),
		  yy);
	XDrawLine(EZ_Display,pixmap, gc1,
		  xx, yy+2,
		  xx+ EZ_SliderWidth(wptr) - (sbw+sbw),
		  yy+2);
	EZ_DrawRectBorderWithSize(wptr, pixmap, 
				  x,
				  y,
				  ww,
				  EZ_SliderSliderLength(wptr),
				  sbw,
				  EZ_BORDER_UP,0,0,1);
      }
    break;
    case 4:
    case 5:
    case 6:
    case 7:
      {
	int ix = x + (ww>>1) - 2;
	int iy = y;
        int btnBorder = EZ_VSLIDER_BORDER;
        if(sliderStyle > 5) { btnBorder = EZ_BORDER_UP;}
        ww += sbw;
       
	EZ_SetupUpBdGC(wptr, &gc1, &gc1, &gc1, &gc1,&gc, &gc, ix, iy, 1);
	XFillRectangle(EZ_Display, pixmap, gc, ix,iy +sbw, 4, hh);
	EZ_DrawRectBorderWithSize(wptr, pixmap, ix,iy +sbw, 4,hh,
				  1, EZ_BORDER_DOWN,0,0,0);
        
	y =  EZ_SliderPosition(wptr) - (EZ_SliderSliderLength(wptr)>>1); 

	if(EZ_WidgetBGPixmapB(wptr) != NULL)
	  {
	    EZ_Bitmap *bitmap = EZ_WidgetBGPixmapB(wptr);
	    gc = EZ_NTILEGC2;
            if(highlight) XSetTile(EZ_Display,EZ_NTILEGC2,bitmap->htpixmap);
	    else XSetTile(EZ_Display,EZ_NTILEGC2,bitmap->pixmap);
	    XSetTSOrigin(EZ_Display, EZ_NTILEGC2, x, y);
	  }
	else if(EZ_WidgetParentBGPixmapB(wptr) != NULL)
	  {
	    EZ_Bitmap *bitmap = EZ_WidgetParentBGPixmapB(wptr);
	    gc = EZ_NTILEGC2;
            if(highlight) XSetTile(EZ_Display,EZ_NTILEGC2,bitmap->htpixmap);
            else XSetTile(EZ_Display,EZ_NTILEGC2,bitmap->pixmap);
	    XSetTSOrigin(EZ_Display, EZ_NTILEGC2, x, y);
	  }
	else
	  {
            int tile;
            if(swapBg) { EZ_InstallHighlightColor(wptr); } /*4/29/99*/
	    tile = EZ_GetBackgroundGC(wptr,&gc, &bgpv, 0, highlight);
	    if(tile) 
	      {
		int ox, oy;
		EZ_GetWidgetTileOrigin(wptr, &ox, &oy);      
		XSetTSOrigin(EZ_Display, gc, ox, oy);
	      }
	  }  
	XFillRectangle(EZ_Display, pixmap, gc,
		       x, y,
		     ww,EZ_SliderSliderLength(wptr));

	xx = x + sbw;
	if(sliderStyle & 1)
	  {
	    int iw = ww - (sbw+sbw);
	    int ih = EZ_SliderSliderLength(wptr) -  (sbw+sbw);
	    yy = y + sbw + offset;
	    EZ_OverlayDragablePixmap(wptr, pixmap, xx, yy, iw, ih);      
	  }
	EZ_DrawRectBorderWithSize(wptr, pixmap, 
				  x,
				  y,
				  ww,
				  EZ_SliderSliderLength(wptr),
				  sbw,
				  btnBorder,x,y,1);

      }
      break;
    default:
      break;
    }
  if(swapBg) { EZ_UninstallHighlightColor(wptr); } /*4/29/99*/
  EZ_DrawRectBorder(wptr, pixmap);
  XCopyArea(EZ_Display,pixmap,win, EZ_WRITABLEGC,0,0,w,h,0,0); 
  XFreePixmap(EZ_Display, pixmap);                             
}

/**************************************************************************/
void  EZ_SliderEventHandle(widget, event)
     EZ_Widget *widget;
     XEvent          *event;
{
  int button;
  if(widget == (EZ_Widget *)NULL) return;
  if(event->type == Expose) EZ_DrawWidget(widget);      
  if( EZ_GetWidgetDisabledFlag(widget) ) return;
  if(event->type == ButtonPress && event->xbutton.button == EZ_Btn3)
    {
      EZ_HandleDnDEvents(widget, event);
      return;
    }

  switch(event->type)
    {
    case EnterNotify:
      break;
    case LeaveNotify:
      EZ_DrawWidget(widget);
      break;
    case ButtonPress:
      button = event->xbutton.button;

      switch(EZ_WidgetType(widget))
	{
	case EZ_WIDGET_HORIZONTAL_SLIDER:
	  {
	    int     x,y, onbutton, inwindow, halflength, newpos;
	    XEvent  xevent;

	    x = event->xbutton.x;
	    y = event->xbutton.y;
	    onbutton = 0;
	    inwindow = 0;

	    if(y >= EZ_SliderMiny(widget) && y <= EZ_SliderMaxy(widget))
	      {
		/*--------------------------------------------------
		 * First, check if the button press is on the button
		 *--------------------------------------------------*/
		if(x >= EZ_SliderPosition(widget) - (EZ_SliderSliderLength(widget)>>1) &&
		   x <= EZ_SliderPosition(widget) +(EZ_SliderSliderLength(widget)>>1))
		  {
		    onbutton = 1;
		    /*----------------------------------------------
		     * Press is on the button. Update button 
		     * appearence immediately. But don't update
		     * the slider position yet.
		     *---------------------------------------------*/
		    if(EZ_DepresionDisabled == 0)
		      EZ_SliderBorderStyle(widget) = EZ_BORDER_DOWN;
		  }
		else if( x >= EZ_SliderMinx(widget) && x <= EZ_SliderMaxx(widget))
		  {
		    inwindow = 1;

		    /*---------------------------------------------------
		     * press is in the slider window. If Button2, Update
		     * slider position and appearance immediately
		     *-------------------------------------------------*/
		    if(button == EZ_Btn2)
		      {
			halflength = (EZ_SliderSliderLength(widget)>>1);
			if( x < EZ_SliderPositionMin(widget))
			  newpos =  EZ_SliderPositionMin(widget);
			else if( x > EZ_SliderMaxx(widget) - halflength)
			  newpos =  EZ_SliderMaxx(widget) - halflength;
			else  newpos = x;
			EZ_SliderPosition(widget) = newpos;
			EZ_SliderValue(widget) = EZ_SliderMinValue(widget) +
			  EZ_SliderFactor(widget) * (newpos - EZ_SliderPositionMin(widget));
                        EZ_SliderValue(widget) = 
                          EZ_FixSliderValue(EZ_SliderValue(widget), EZ_SliderResolution(widget));
			if(EZ_DepresionDisabled == 0)
			  EZ_SliderBorderStyle(widget) = EZ_BORDER_DOWN;
			onbutton = 1;
		      }
		  }
		if(onbutton)
		  {
		    int wox, woy;
		    Window junkwin;
		    XTranslateCoordinates(EZ_Display, EZ_WidgetWindow(widget),
					  RootWindow(EZ_Display, EZ_ScreenNum),
					  0, 0,
					  &wox,&woy, &junkwin);
		    /*---------------------------------------------
		     *  Now wait for a ButtonRelease event. Process
		     *  all pointer motion event.
		     *--------------------------------------------*/
		    EZ_SetWidgetHighlightFlag(widget);
                    EZ_SliderHighlight(widget) = 1;
		    EZ_DrawWidget(widget);		    
		    while(1)
		      {
			XNextEvent(EZ_Display, &xevent);
			if(EZ_FilterEvent(&xevent))
			  EZ_InvokePrivateEventHandler(&xevent);
			if(xevent.type == Expose || xevent.type == FocusIn || xevent.type == FocusOut) 
			  EZ_WidgetDispatchEvent(&xevent);
			while(XPending(EZ_Display))
			  {
			    if(xevent.type == ButtonRelease && xevent.xbutton.button == button) break;
			    XNextEvent(EZ_Display, &xevent);
			    if(EZ_FilterEvent(&xevent))
			      EZ_InvokePrivateEventHandler(&xevent);
			    if(xevent.type == Expose || xevent.type == FocusIn || xevent.type == FocusOut) 
			      EZ_WidgetDispatchEvent(&xevent); 
			  }
			if(xevent.type == ButtonRelease && xevent.xbutton.button == button)
			  {
			    if(EZ_SliderBorderStyle(widget) == EZ_BORDER_DOWN ||
			       EZ_GetWidgetHighlightFlag(widget))
			      {
				EZ_ClearWidgetHighlightFlag(widget);
                                EZ_SliderHighlight(widget) = 0;
				EZ_SliderBorderStyle(widget) = EZ_BORDER_UP; 
				EZ_DrawWidget(widget); 
			      }
			    { EZ_ExecuteWidgetCallBacks(widget);}
			    break;
			  }
			else 
			  {
			    float ff;
			    int             rx,ry,x,y;
			    unsigned int    mask;
			    Window          root,win;
			    XQueryPointer(EZ_Display, RootWindow(EZ_Display, EZ_ScreenNum),
					  &root,
					  &win, 
					  &rx, &ry, 
					  &x,&y, 
					  &mask );
			    x = rx - wox;
			    halflength = (EZ_SliderSliderLength(widget)>>1);
			    if( x < EZ_SliderPositionMin(widget))
			      newpos =  EZ_SliderPositionMin(widget);
			    else if( x > EZ_SliderMaxx(widget) - halflength)
			      newpos =  EZ_SliderMaxx(widget) - halflength;
			    else  newpos = x;
			    EZ_SliderPosition(widget) = newpos;
			    ff = EZ_SliderValue(widget);
			    EZ_SliderValue(widget) = EZ_SliderMinValue(widget) +
			      EZ_SliderFactor(widget) * (newpos - EZ_SliderPositionMin(widget));
                            EZ_SliderValue(widget) = 
                              EZ_FixSliderValue(EZ_SliderValue(widget), EZ_SliderResolution(widget));
			    EZ_DrawWidget(widget); 
			    if(ff != EZ_SliderValue(widget))
                              {
                                EZ_CallBackStruct *callbacks = EZ_WidgetMotionCallBack(widget);
                                if(callbacks) {EZ_HandleMotionCallBacks(widget,callbacks);}
                                else { EZ_ExecuteWidgetCallBacks(widget);}
                              }
			  }
		      }
		  }
		else if(inwindow)
		  {
		    /* must be Button1 and the press is not on slider */
		    /* slide toward pointer */
		    int   first_time = 1, done = 0;
		    int   pos = EZ_SliderPosition(widget);
		    int   m_unit, iincr =(x > pos)? 1: -1;
		    int   halflength = (EZ_SliderSliderLength(widget)>>1);
		    int   minP = EZ_SliderPositionMin(widget);
		    int   maxP = EZ_SliderMaxx(widget) - halflength;
		    float factor = EZ_SliderFactor(widget);
		    float minF = EZ_SliderMinValue(widget);
		    float ff;
		    if(event->xbutton.state & ShiftMask) m_unit =  iincr * 4;
		    else if(event->xbutton.state & ControlMask)  m_unit = iincr * 2;
		    else m_unit = iincr;

		    while(!done) /* wait for a matching ButtonRelease */
		      { 
			int pos = EZ_SliderPosition(widget) ;
			if((iincr > 0 && pos < maxP) || (iincr < 0 && pos > minP))
			  {
			    EZ_SliderPosition(widget) += m_unit;
			    if(EZ_SliderPosition(widget) > maxP)
			      EZ_SliderPosition(widget) = maxP;
			    else if(EZ_SliderPosition(widget) < minP)
			      EZ_SliderPosition(widget) = minP;
			    ff = EZ_SliderValue(widget);
			    EZ_SliderValue(widget) = minF +
			      (EZ_SliderPosition(widget) - minP) * factor;
                            EZ_SliderValue(widget) = 
                              EZ_FixSliderValue(EZ_SliderValue(widget), EZ_SliderResolution(widget));
			    EZ_DrawWidget(widget); 
			    if(ff != EZ_SliderValue(widget))
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
			    if(EZ_FilterEvent(&xevent))
			      EZ_InvokePrivateEventHandler(&xevent);
			    if(xevent.type == ButtonRelease &&
			       xevent.xbutton.button == button)
			      {
				done = 1;
				break;
			      }
			  }
		      }		    
		  }
	      }
	  }
	  break;
	case EZ_WIDGET_VERTICAL_SLIDER:
	  {
	    int     x,y, onbutton, inwindow, halflength, newpos;
	    XEvent  xevent;

	    x = event->xbutton.x;
	    y = event->xbutton.y;
	    onbutton = 0;
	    inwindow = 0;

	    if(x >= EZ_SliderMinx(widget) && x <= EZ_SliderMaxx(widget))
	      {
		/*--------------------------------------------------
		 * First, check if the button press is on the button
		 *--------------------------------------------------*/
		if(y >= EZ_SliderPosition(widget) - (EZ_SliderSliderLength(widget)>>1) &&
		   y <= EZ_SliderPosition(widget) +(EZ_SliderSliderLength(widget)>>1))
		  {
		    onbutton = 1;
		    /*----------------------------------------------
		     * Press is on the button. Update button 
		     * appearence immediately. But don't update
		     * the slider position yet.
		     *---------------------------------------------*/
		    if(EZ_DepresionDisabled == 0)
		      EZ_SliderBorderStyle(widget) = EZ_BORDER_DOWN;
		  }
		else if( y >= EZ_SliderMiny(widget) && y <= EZ_SliderMaxy(widget))
		  {
		    inwindow = 1;
		    /*---------------------------------------------
		     * press is in the slider window. Update
		     * slider position and appearance immediately
		     *--------------------------------------------*/
		    if(button == EZ_Btn2)
		      {
			halflength = (EZ_SliderSliderLength(widget)>>1);
			if(y < EZ_SliderPositionMin(widget))
			  newpos =  EZ_SliderPositionMin(widget);
			else if( y > EZ_SliderMaxy(widget) - halflength)
			  newpos =  EZ_SliderMaxy(widget) - halflength;
			else  newpos = y;
			EZ_SliderPosition(widget) = newpos;
			EZ_SliderValue(widget) = EZ_SliderMinValue(widget) +
			  EZ_SliderFactor(widget) * (newpos - EZ_SliderPositionMin(widget));
                        EZ_SliderValue(widget) = 
                          EZ_FixSliderValue(EZ_SliderValue(widget), EZ_SliderResolution(widget));
			if(EZ_DepresionDisabled == 0)
			  EZ_SliderBorderStyle(widget) = EZ_BORDER_DOWN;
			onbutton = 1;
		      }
		  }
		if(onbutton)
		  {
		    int wox,woy;
		    Window junkwin;
		    XTranslateCoordinates(EZ_Display, EZ_WidgetWindow(widget),
					  RootWindow(EZ_Display, EZ_ScreenNum),
					  0, 0,
					  &wox,&woy, &junkwin);
		    /*---------------------------------------------
		     *  Now wait for a ButtonRelease event. Process
		     *  all pointer motion event.
		     *--------------------------------------------*/
		    EZ_SetWidgetHighlightFlag(widget);
                    EZ_SliderHighlight(widget) = 1;
		    EZ_DrawWidget(widget);		    
		    while(1)
		      {
			XNextEvent(EZ_Display, &xevent);
			if(EZ_FilterEvent(&xevent))
			  EZ_InvokePrivateEventHandler(&xevent);
			if(xevent.type == Expose || xevent.type == FocusIn || xevent.type == FocusOut) 
			  EZ_WidgetDispatchEvent(&xevent); 
			while(XPending(EZ_Display))
			  {
			    if(xevent.type == ButtonRelease && xevent.xbutton.button == button) break;
			    XNextEvent(EZ_Display, &xevent);
			    if(EZ_FilterEvent(&xevent))
			      EZ_InvokePrivateEventHandler(&xevent);
			    if(xevent.type == Expose || xevent.type == FocusIn || xevent.type == FocusOut) 
			      EZ_WidgetDispatchEvent(&xevent); 
			  }
			if(xevent.type == ButtonRelease && xevent.xbutton.button == button)
			  {
			    if(EZ_SliderBorderStyle(widget) == EZ_BORDER_DOWN ||
			       EZ_GetWidgetHighlightFlag(widget))
			      {
				EZ_ClearWidgetHighlightFlag(widget);
                                EZ_SliderHighlight(widget) = 0;
				EZ_SliderBorderStyle(widget) = EZ_BORDER_UP; 
				EZ_DrawWidget(widget); 
			      }
			    { EZ_ExecuteWidgetCallBacks(widget);}
			    break;
			  }
			else
			  {
			    float ff;
			    int             rx,ry,x,y;
			    unsigned int    mask;
			    Window          root,win;
			    XQueryPointer(EZ_Display, RootWindow(EZ_Display, EZ_ScreenNum),
					  &root,
					  &win, 
					  &rx, &ry, 
					  &x,&y, 
					  &mask );
			    
			    y = ry - woy;
			    halflength = (EZ_SliderSliderLength(widget)>>1);
			    if( y < EZ_SliderPositionMin(widget))
			      newpos =  EZ_SliderPositionMin(widget);
			    else if( y > EZ_SliderMaxy(widget) - halflength)
			      newpos =  EZ_SliderMaxy(widget) - halflength;
			    else  newpos = y;
			    EZ_SliderPosition(widget) = newpos;
			    ff = EZ_SliderValue(widget);
			    EZ_SliderValue(widget) = EZ_SliderMinValue(widget) +
			      EZ_SliderFactor(widget) * (newpos - EZ_SliderPositionMin(widget));
                            EZ_SliderValue(widget) = 
                              EZ_FixSliderValue(EZ_SliderValue(widget), EZ_SliderResolution(widget));
			    EZ_DrawWidget(widget); 	
			    if(ff != EZ_SliderValue(widget))
                              {
                                EZ_CallBackStruct *callbacks = EZ_WidgetMotionCallBack(widget);
                                if(callbacks) {EZ_HandleMotionCallBacks(widget,callbacks);}
                                else { EZ_ExecuteWidgetCallBacks(widget);}
                              }
			  }
		      }
		  }
		else if(inwindow)
		  {
		    /* must be Button1 and the press is not on slider */
		    /* slide toward pointer */
		    int   first_time = 1, done = 0;
		    int   pos = EZ_SliderPosition(widget);
		    int   m_unit,iincr =(y > pos)? 1: -1;
		    int   halflength = (EZ_SliderSliderLength(widget)>>1);
		    int   minP = EZ_SliderPositionMin(widget);
		    int   maxP = EZ_SliderMaxy(widget) - halflength;
		    float factor = EZ_SliderFactor(widget);
		    float minF = EZ_SliderMinValue(widget);
		    float ff;
		    if(event->xbutton.state & ShiftMask) m_unit =  iincr * 4;
		    else if(event->xbutton.state & ControlMask)  m_unit = iincr * 2;
		    else m_unit = iincr;

		    while(!done) /* wait for a matching ButtonRelease */
		      { 
			int pos = EZ_SliderPosition(widget);

			if((iincr > 0 && pos < maxP) || (iincr < 0 && pos > minP))
			  {
			    EZ_SliderPosition(widget) += m_unit;
			    if(EZ_SliderPosition(widget) > maxP)
			      EZ_SliderPosition(widget) = maxP;
			    else if(EZ_SliderPosition(widget) < minP)
			      EZ_SliderPosition(widget) = minP;
			    ff = EZ_SliderValue(widget);
			    EZ_SliderValue(widget) = minF +
			      (EZ_SliderPosition(widget) - minP) * factor;
                            EZ_SliderValue(widget) = 
                              EZ_FixSliderValue(EZ_SliderValue(widget), EZ_SliderResolution(widget));
                            EZ_DrawWidget(widget); 
			    if(ff != EZ_SliderValue(widget))
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
			    if(EZ_FilterEvent(&xevent))
			      EZ_InvokePrivateEventHandler(&xevent);
			    if(xevent.type == ButtonRelease &&
			       xevent.xbutton.button == button)
			      {
				done = 1;
				break;
			      }
			  }
		      }		    
		  }
	      }
	  }
	  break;
	default:
	  break;
	}
      break;
    case MotionNotify:
      switch(EZ_WidgetType(widget))
	{
	case EZ_WIDGET_HORIZONTAL_SLIDER:
	  {
	    int  x,y;

	    x = event->xbutton.x;
	    y = event->xbutton.y;

	    if( (y >= EZ_SliderMiny(widget) && y <= EZ_SliderMaxy(widget)) &&
	       (x >= EZ_SliderPosition(widget) - (EZ_SliderSliderLength(widget)>>1) &&
		     x <= EZ_SliderPosition(widget) +(EZ_SliderSliderLength(widget)>>1)) )
	      {
                if(EZ_SliderHighlight(widget) == 0)
                  {
                    EZ_SetWidgetHighlightFlag(widget);
                    EZ_SliderHighlight(widget) = 1;
                    EZ_DrawWidget(widget);	
                  }
	      }
	    else if(EZ_GetWidgetHighlightFlag(widget))
	      {
		EZ_ClearWidgetHighlightFlag(widget);
                EZ_SliderHighlight(widget) = 0;
		EZ_DrawWidget(widget);
	      }
	  }
	  break;
	case EZ_WIDGET_VERTICAL_SLIDER:
	  {
	    int  x,y;
	    
	    x = event->xbutton.x;
	    y = event->xbutton.y;

	    if( (x >= EZ_SliderMinx(widget) && x <= EZ_SliderMaxx(widget)) &&
	       (y >= EZ_SliderPosition(widget) - (EZ_SliderSliderLength(widget)>>1) &&
		y <= EZ_SliderPosition(widget) +(EZ_SliderSliderLength(widget)>>1)) )
	      {
                if(EZ_SliderHighlight(widget) == 0)
                  {
                    EZ_SetWidgetHighlightFlag(widget);
                    EZ_SliderHighlight(widget) = 1;
                    EZ_DrawWidget(widget);	
                  }
	      }
	    else if(EZ_GetWidgetHighlightFlag(widget))
	      {
		EZ_ClearWidgetHighlightFlag(widget);
                EZ_SliderHighlight(widget) = 0;
		EZ_DrawWidget(widget);
	      }
	  }
	  break;
	default:
	  break;
	}
      break;
    case KeyPress:
      {
	char              tmpbuffer[8];
	int               buffersize = 8, move_unit;
	KeySym            keysym;
	XComposeStatus    compose; 
	int count = XLookupString(&(event->xkey), tmpbuffer, buffersize, &keysym, &compose);
	tmpbuffer[count] = '\0';

	if(EZ_WidgetType(widget) == EZ_WIDGET_VERTICAL_SLIDER)
	  {
	    switch(keysym)
	      {
	      case XK_Down: case XK_n: case XK_j: case XK_N: case XK_J: 
#ifdef XK_KP_Down
	      case XK_KP_Down:
#endif
		move_unit = 1;
		break;
	      case XK_Up: case XK_k:  case XK_p: case XK_K:  case XK_P: 
#ifdef XK_KP_Up
	      case XK_KP_Up:
#endif
		move_unit = -1;
		break;
	      default:
		move_unit = 0;
		break;
	      }
	    if(move_unit)
	      {
		int   halflength = (EZ_SliderSliderLength(widget)>>1);
		int   minP = EZ_SliderPositionMin(widget);
		int   maxP = EZ_SliderMaxy(widget) - halflength;
		float factor = EZ_SliderFactor(widget);
		float minF = EZ_SliderMinValue(widget);
		float ff;
		int pos = EZ_SliderPosition(widget) ;
		if(event->xkey.state & ShiftMask)   move_unit *= 4;
		if(event->xkey.state & ControlMask) move_unit *= 2;

		if((move_unit > 0 && pos < maxP) || (move_unit < 0 && pos > minP))
		  {
		    EZ_SliderPosition(widget) += move_unit;
		    if(EZ_SliderPosition(widget) > maxP)
		      EZ_SliderPosition(widget) = maxP;
		    else if(EZ_SliderPosition(widget) < minP)
		      EZ_SliderPosition(widget) = minP;
		    ff = EZ_SliderValue(widget);
		    EZ_SliderValue(widget) = minF +
		      (EZ_SliderPosition(widget) - minP) * factor;
                    EZ_SliderValue(widget) = 
                      EZ_FixSliderValue(EZ_SliderValue(widget), EZ_SliderResolution(widget));
		    EZ_DrawWidget(widget); 
		    if(ff != EZ_SliderValue(widget))
		      { EZ_ExecuteWidgetCallBacks(widget);}
		  }
	      }
	  }
	else if(EZ_WidgetType(widget) == EZ_WIDGET_HORIZONTAL_SLIDER)
	  {
	    switch(keysym)
	      {
	      case XK_Left: case XK_b:  case XK_h: case XK_B:  case XK_H:  
#ifdef XK_KP_Left
	      case XK_KP_Left:
#endif
		move_unit = -1;
		break;
	      case XK_Right: case XK_f: case XK_l: case XK_F: case XK_L:  
#ifdef XK_KP_Right
	      case XK_KP_Right:
#endif
		move_unit = 1;
		break;
	      default:
		move_unit = 0;
		break;
	      }
	    if(move_unit)
	      {
		int   halflength = (EZ_SliderSliderLength(widget)>>1);
		int   minP = EZ_SliderPositionMin(widget);
		int   maxP = EZ_SliderMaxx(widget) - halflength;
		float factor = EZ_SliderFactor(widget);
		float minF = EZ_SliderMinValue(widget);
		float ff;
		int pos = EZ_SliderPosition(widget) ;
		if(event->xkey.state & ShiftMask)   move_unit *= 4;
		if(event->xkey.state & ControlMask) move_unit *= 2;

		if((move_unit >0 && pos < maxP) || (move_unit < 0 && pos > minP))
		  {
		    EZ_SliderPosition(widget) += move_unit;
		    if(EZ_SliderPosition(widget) > maxP)
		      EZ_SliderPosition(widget) = maxP;
		    else if(EZ_SliderPosition(widget) < minP)
		      EZ_SliderPosition(widget) = minP;

		    ff = EZ_SliderValue(widget);
		    EZ_SliderValue(widget) = minF +
		      (EZ_SliderPosition(widget) - minP) * factor;
                    EZ_SliderValue(widget) = 
                      EZ_FixSliderValue(EZ_SliderValue(widget), EZ_SliderResolution(widget));
		    EZ_DrawWidget(widget); 
		    if(ff != EZ_SliderValue(widget))
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
      }
      break;
    default:
      break;
    }
}

/************************************************************
 *
 */
float  EZ_GetSliderValue(widget)
     EZ_Widget *widget;
{
  if(widget == (EZ_Widget *)NULL ||
     (EZ_WidgetType(widget)!= EZ_WIDGET_VERTICAL_SLIDER &&
      EZ_WidgetType(widget)!= EZ_WIDGET_HORIZONTAL_SLIDER))
    {
      (void)fprintf(stderr, "In EZ_GetSliderValue(), wrong type of widget\n");
      return(0.0);
    }
  return(EZ_SliderValue(widget));
}

/************************************************************/
void  EZ_SetSliderValue(widget, value)
     EZ_Widget *widget; 
     float     value;
{
  if(widget != (EZ_Widget *)NULL &&
     (EZ_WidgetType(widget) == EZ_WIDGET_VERTICAL_SLIDER ||
      EZ_WidgetType(widget) == EZ_WIDGET_HORIZONTAL_SLIDER))
    {
      EZ_SliderValue(widget) = value;
      if(EZ_WidgetMapped(widget)) 
	{
	  EZ_SetWidgetSizeMayBeChangedFlag(widget);
	  EZ_DrawWidget(widget);
	}
    }
}
/************************************************************/

float EZ_FixSliderValue(v, reso)
     float v, reso;
{
  if(reso > 0.0)
    {
      float f = v;
      float R = reso;
      float R2 = 0.5 * R;
      float r = (float)fmod( (double) f, (double) R);
      
      if( r < 0.0)  r += R;
      if( r > R2)
	v += (R - r);
      else
	v -= r;
    }
  return(v);
}
/************************************************************/
void EZ_SetSliderRange(widget, from, to)
     EZ_Widget *widget; float from, to;
{
  if(widget && (EZ_WidgetType(widget) == EZ_WIDGET_HORIZONTAL_SLIDER ||
     EZ_WidgetType(widget) == EZ_WIDGET_VERTICAL_SLIDER ))
    {
      EZ_SliderMinValue(widget) = (float)from;
      EZ_SliderMaxValue(widget) = (float)to;
      if(EZ_WidgetMapped(widget)) EZ_DisplayWidget(widget);
    }
}
/************************************************************/
#undef _EZ_WIDGET_SLIDER_C_
