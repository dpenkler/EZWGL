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
 ***       GC for Widgets                                      ***
 ***                                                           ***
 ***  Initialize the GCs which are used in drawing widgets.    ***
 ***  0-8:   Different shades of grey.                         ***
 ***   # 3   is used for deactived widget                      *** 
 ***         0 for black and 8 for white.                      ***
 ***  9:     DarkRed                                           ***
 ***  10-13: Fonts:  Normal, Thin, Bold, Italic                ***
 ***  14:    A Writable GC.                                    ***
 *****************************************************************/
#define  _EZ_WIDGET_GC_C_

#include "EZ_Widget.h"

/*********************************************************************
 * 
 *  Functions implemented in this file:
 */
void  EZ_InitializeWidgetGC MY_ANSIARGS((void));
void  EZ_DestroyWidgetGCsandFonts MY_ANSIARGS((void));
GC    EZ_CreateGCFor3DCanvas MY_ANSIARGS((void));
void  EZ_GetMovingDashedLineGC MY_ANSIARGS((GC *gcp_ret, int which));

/*********************************************************************/
/* #define TOTAL_NUM_GCS  DARKRED+21 */
GC        EZ_GCList[TOTAL_NUM_GCS];       /* list of GC's for widgets */
/*********************************************************************/

void  EZ_InitializeWidgetGC()
{
  unsigned long valuemask; 
  XGCValues     values;
  register int  i;
  Window        win = EZ_DummyWindow;

  /*-------------------------------------------
   * Read Only GCs. From GRAY0(0) to DARKRED(9)
   * (well, font may be changed)
   *------------------------------------------*/
  for(i = GRAY0; i <= DARKRED; i++)
    {
      values.foreground = EZ_ColorArray[i];
      values.background = EZ_ColorArray[0];
      values.graphics_exposures = False; 
      values.cap_style = CapButt;
      values.join_style = ( i == GRAY0 ? JoinMiter : JoinRound);
      values.line_width = (i == DARKRED || i == GRAY0 ? 2 : 1);
      valuemask = GCGraphicsExposures|GCCapStyle|GCJoinStyle|GCLineWidth|
	GCForeground|GCBackground;
      EZ_GCList[i] = XCreateGC(EZ_DisplayForWidgets, win, valuemask, &values);  
    }
  EZ_KbdFocusHighlightColor = EZ_ColorArray[0];

  /*-------------------------------------------------------------------
   *  GCs for render text with the default fonts.
   *------------------------------------------------------------------*/
  EZ_InitFontList();   /* make sure that the default fonts are loaded */

  for(i = 1; i <= 4; i++)
    {
      values.foreground = EZ_DefaultForeground;
      values.background = EZ_ColorArrayForWidgets[WHITE];
      values.graphics_exposures = False; 
      values.cap_style = CapRound;
      values.join_style = JoinRound;
      values.line_width = 0;
      values.font = (EZ_GetFontFromId(i))->fid;
      valuemask = GCGraphicsExposures|GCCapStyle|GCJoinStyle|GCLineWidth|
	GCForeground|GCBackground|GCFont;
      EZ_GCList[i+DARKRED] = XCreateGC(EZ_DisplayForWidgets, win, valuemask, &values);  
    }

  /*-------------------------------------------------------------------
   * Writable GCs. 
   *------------------------------------------------------------------*/  
  for(i = 0; i < 4; i++)
    {
      values.foreground = EZ_ColorArrayForWidgets[GRAY2+i];
      values.background = EZ_ColorArrayForWidgets[BLACK];
      valuemask = GCGraphicsExposures|GCCapStyle|GCJoinStyle|GCLineWidth|
	GCForeground|GCBackground;

      EZ_GCList[i+5+DARKRED] = XCreateGC(EZ_DisplayForWidgets, win, valuemask, &values);
    }

  /*-------------------------------------------------------------------
   * an Xor GC. 
   *------------------------------------------------------------------*/
  {
    valuemask = GCFunction|GCLineWidth|GCFillStyle|GCForeground|
      GCBackground|GCSubwindowMode|GCPlaneMask|GCFont;
    values.function   = GXxor;
    values.line_width = 2;
    values.plane_mask = 0x55555555; /* AllPlanes;*/
    values.stipple    = None;
    values.fill_style = /*FillOpaqueStippled;*/ FillSolid;
    values.foreground = (1 << EZ_Depth) - 1;
    values.background = EZ_ColorArray[0];
    values.subwindow_mode = IncludeInferiors;
    values.font = (EZ_GetFontFromId(EZ_HUGE_FONT))->fid;
    EZ_GCList[9+DARKRED] = XCreateGC(EZ_DisplayForWidgets, win, valuemask, &values);  
  }
  /*-------------------------------------------------------------------
   * a writable GC for drawing figures in FigItem
   *------------------------------------------------------------------*/
  {
    valuemask = GCGraphicsExposures|GCCapStyle|GCJoinStyle|GCLineWidth|
      GCForeground|GCBackground;
    values.foreground = EZ_ColorArray[0];
    values.background = EZ_ColorArray[8];
    values.graphics_exposures = False; 
    values.cap_style = CapButt;
    values.join_style = JoinRound;
    values.line_width = 1;
    EZ_GCList[10+DARKRED] = XCreateGC(EZ_DisplayForWidgets, win, valuemask, &values);  
  }
  /*-------------------------------------------------------------------
   *  Drag GC. 
   *------------------------------------------------------------------*/
  {
    valuemask = GCGraphicsExposures|GCCapStyle|GCJoinStyle|GCLineWidth|
      GCForeground|GCBackground|GCPlaneMask|GCFunction;
    values.foreground = 0xaaaaaaaa;
    values.background = 0;
    values.graphics_exposures = False; 
    values.cap_style = CapButt;
    values.join_style = JoinRound;
    values.function   = GXxor;
    values.plane_mask = AllPlanes;
    values.line_width = 1;
    EZ_GCList[11+DARKRED] = XCreateGC(EZ_DisplayForWidgets, win, valuemask, &values);  
  }
  /*-------------------------------------------------------------------
   * a Dashed line GC. 
   *------------------------------------------------------------------*/
  {
    valuemask = GCGraphicsExposures|GCCapStyle|GCJoinStyle|GCLineWidth|
      GCForeground|GCBackground|GCLineStyle|GCSubwindowMode|GCDashList;
    values.foreground = EZ_ColorArray[8];
    values.background = EZ_ColorArray[0];
    values.graphics_exposures = False; 
    values.line_style = LineDoubleDash;
    values.cap_style = CapButt;
    values.join_style = JoinRound;
    values.line_width = 1;
    values.dashes = 4;
    values.subwindow_mode = IncludeInferiors;
    EZ_GCList[12+DARKRED] = XCreateGC(EZ_DisplayForWidgets, win, valuemask, &values); 
  }
  /*-------------------------------------------------------------------
   * A GC for Drawing DND item outline
   *------------------------------------------------------------------*/
  {
    valuemask = GCFunction|GCLineWidth|GCForeground|
      GCBackground|GCSubwindowMode|GCPlaneMask|GCFont;
    values.function   = GXxor;
    values.line_width = 4;
    values.plane_mask = 0xaaaaaaaa; /*AllPlanes;*/
    values.stipple    = None;
    values.foreground = (1 << EZ_Depth) - 1;
    values.background = EZ_ColorArrayForWidgets[0];
    values.subwindow_mode = IncludeInferiors;
    values.font = (EZ_GetFontFromId(EZ_HUGE_FONT))->fid;
    EZ_GCList[13+DARKRED] = XCreateGC(EZ_DisplayForWidgets, win, valuemask, &values);  
  }
  /* another writable GC */
  {
    valuemask = GCGraphicsExposures|GCCapStyle|GCJoinStyle|GCLineWidth|
      GCForeground|GCBackground;
    values.line_width = 2;
    values.foreground = EZ_ColorArray[0];
    values.background = EZ_ColorArray[0];
    values.graphics_exposures = False; 
    values.cap_style = CapButt;
    values.join_style = JoinRound;
    EZ_GCList[14+DARKRED] = XCreateGC(EZ_DisplayForWidgets, win, valuemask, &values);  
  }

  /* tile GCs for rendering tiled background and borders */
  {
    valuemask = GCGraphicsExposures|GCCapStyle|GCJoinStyle|GCLineWidth|
	GCForeground|GCBackground|GCFillStyle;
    values.foreground = EZ_ColorArray[0];
    values.background = EZ_ColorArray[8];
    values.graphics_exposures = False; 
    values.cap_style = CapButt;
    values.join_style = JoinRound;
    values.line_width = 1;
    values.fill_style = FillTiled;
    for(i = 0; i < 6; i++)
      EZ_GCList[DARKRED+15+i] = XCreateGC(EZ_DisplayForWidgets, win, valuemask, &values);  
  }

  /*-------------------------------------------------------------------
   * A GC for highlighting FIG items
   *------------------------------------------------------------------*/
  {
    static unsigned char somebits[] = { 0x01, 0x04, 0x01, 0x04}; 
    Pixmap stipple = XCreateBitmapFromData(EZ_Display,
					   RootWindow(EZ_Display, EZ_ScreenNum),
					   somebits, 4,4);
    valuemask = GCForeground|GCBackground|GCFillStyle|GCStipple;
    values.fill_style = FillOpaqueStippled;
    values.stipple    = stipple;
    values.foreground = 0;
    values.background = 0xffffffff;
    EZ_GCList[21+DARKRED] = XCreateGC(EZ_Display, win, valuemask, &values);  
  }
}
/******************************************************************************/
void  EZ_GetMovingDashedLineGC(gc_ret, which)
     GC *gc_ret; int which;
{
  static int offset1 = 0, offset2 = 0;
  unsigned long valuemask = GCDashOffset|GCBackground|GCForeground;
  unsigned long fg, bg;
  XGCValues values;
  int offset;
  if(which)
    {
      if(offset1 == 64)  offset1 = 0;
      else  offset1 += 2;
      offset = offset1;
      fg = EZ_ColorArray[RED];
      bg = EZ_ColorArray[GREEN];
    }
  else
    {
      if(offset2 == 64)  offset2 = 0;
      else  offset2 += 2;
      offset = offset2;
      fg = EZ_ColorArray[8];
      bg = EZ_ColorArray[0];
    }
  values.background = bg;
  values.foreground = fg;
  values.dash_offset= offset;
  *gc_ret = EZ_GCList[12+DARKRED];
  XChangeGC(EZ_Display, *gc_ret, valuemask, &values);
}

/******************************************************************************/
void  EZ_DestroyWidgetGCsandFonts()
{
  int i;
  for(i = 0; i < TOTAL_NUM_GCS; i++)
    {
      XFreeGC(EZ_Display, EZ_GCList[i]);
      EZ_GCList[i] = (GC)NULL;
    }
  EZ_DestroyFontList();
}
/******************************************************************************
 *
 *  Each 3DCanvas has its own GC. This function is invoked once whenever a
 *  3DCanvas is created.
 */
GC EZ_CreateGCFor3DCanvas()
{
  unsigned long valuemask; 
  XGCValues values;
  GC   gc;

  values.foreground = WhitePixel(EZ_Display, EZ_ScreenNum);
  values.background = BlackPixel(EZ_Display, EZ_ScreenNum);
  values.graphics_exposures = False; 
  values.cap_style = CapRound;
  values.join_style = JoinRound;
  values.line_width = 0;
  valuemask = GCGraphicsExposures|GCCapStyle|GCJoinStyle|GCLineWidth|
    GCForeground|GCBackground;
  gc = XCreateGC(EZ_Display, EZ_DummyWindow, valuemask, &values);
  return(gc);
}
/******************************************************************************/
#undef TOTAL_NUM_GCS

#undef _EZ_WIDGET_GC_C_





