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
 ***              Terminal Emulator                            ***
 ***                                                           ***
 *****************************************************************/
#define _EZ_WIDGET_TERM_C_
#include "EZ_Widget.h"

/*********************************************************************
 * 
 *  Functions implemented in this file:
 */
EZ_Widget        *EZ_CreateTerm MY_ANSIARGS((EZ_Widget *parent, int scrol));
void             EZ_DrawWidgetTerm MY_ANSIARGS((EZ_Widget *widget));
void             EZ_ComputeWidgetTermSize MY_ANSIARGS((EZ_Widget *widget, int *w, int *h));
void             EZ_GetTermWidgetComponents MY_ANSIARGS((EZ_Widget *widget, EZ_Widget **iterm,
                                                         EZ_Widget **vscro));
/*********************************************************************
 * 
 *  Local Variables.
 */
static EZ_WidgetHandle EZ_TermHandle =
{
  EZ_ComputeWidgetTermSize,
  EZ_DrawWidgetTerm,
  EZ_FreeUnknownData,
  EZ_UnkownEventHandle,
};

/*********************************************************************/
EZ_Widget  *EZ_CreateTerm(parent, vscrollbar)
     EZ_Widget  *parent;    /* parent widget    */
     int vscrollbar;
{
  EZ_Widget  *wptr, *iTerm, *vscroll = NULL;

  wptr = EZ_CreateNewWidget(parent);
  /*--------------------------------------------------
   * Register the handling functions for ...
   *  has to be done after  EZ_CreateNewWidget.
   *-------------------------------------------------*/
  EZ_WidgetHandlingFunctions[EZ_WIDGET_TERM] = &EZ_TermHandle;
  
  EZ_WidgetStacking(wptr) = EZ_VERTICAL;
  EZ_SetWidgetTypeAndNames(wptr, EZ_WIDGET_TERM);
  EZ_WidgetBorderStyle(wptr) = EZ_BORDER_NONE;
  EZ_WidgetBorderWidth(wptr) = 0;
  EZ_WidgetFillMode(wptr) = EZ_FILL_VERTICALLY;
  EZ_WidgetPadX(wptr) = 0;
  EZ_WidgetPadY(wptr) = 0;
  EZ_WidgetSepX(wptr)= 0;
  EZ_WidgetSepY(wptr)= 0;

  EZ_SetRowColConstrains(wptr, EZ_ROW,    0, 64, 1000, 0);
  EZ_SetRowColConstrains(wptr, EZ_COLUMN, 0, 64, 1000, 0);
  EZ_SetRowColConstrains(wptr, EZ_COLUMN, 1, 0,  0,    0);

  iTerm = EZ_TermITerm(wptr) = EZ_CreateITerm(wptr);
  EZ_WidgetGroupLeader(iTerm) = wptr;
  EZ_WidgetGBX(iTerm) = 0;
  EZ_WidgetGBY(iTerm) = 0;
  EZ_WidgetGBW(iTerm) = 1;
  EZ_WidgetGBH(iTerm) = 1;

  EZ_SetWidgetExpandFlag(iTerm);

  if(vscrollbar)
    {
      vscroll = EZ_TermVScroll(wptr) = EZ_CreateScrollbar(wptr,EZ_WIDGET_VERTICAL_SCROLLBAR);
      EZ_SetWidgetExpandFlag(vscroll);
      EZ_ScrollObject(vscroll) = iTerm;
      EZ_WidgetGroupLeader(vscroll) = wptr;
      EZ_WidgetGBX(vscroll) = 1;
      EZ_WidgetGBY(vscroll) = 0;
      EZ_WidgetGBW(vscroll) = 1;
      EZ_WidgetGBH(vscroll) = 1;
    }

  EZ_ITermVScroll(iTerm) = vscroll;  
  EZ_ITermTerm(iTerm) = wptr;

  return(wptr);
}
/********************************************************************
 *
 *  Figure out the dimension of an internal list box.
 */
void EZ_ComputeWidgetTermSize(widget, w, h)
     EZ_Widget *widget;
     int             *w, *h;
{
  int  cw, ch;

  if(!(EZ_GetWidgetSizeComputedFlag(widget)))
    EZ_SetWidgetSizeComputedFlag(widget);
      
  cw = EZ_WidgetPadX(widget) + EZ_WidgetBorderWidth(widget);
  ch = EZ_WidgetPadY(widget) + EZ_WidgetBorderWidth(widget);
  cw = cw + cw;
  ch = ch + ch;

  *w = cw;
  *h = ch;
}

/****************************************************************************
 *
 *  Draw a Text widget
 */

void  EZ_DrawWidgetTerm(wptr)
     EZ_Widget *wptr;
{

  if(EZ_WidgetNumChildren(wptr) > 1)
    {
      int             w, h;
      Pixmap          pixmap;
      Window          win;
      GC              gc;
      unsigned long   bgpv;

      win = EZ_WidgetWindow(wptr);
      w   = EZ_WidgetWidth(wptr);
      h   = EZ_WidgetHeight(wptr);
  
      pixmap = XCreatePixmap(EZ_DisplayForWidgets, win, w, h, EZ_DepthForWidgets);    
      if(EZ_GetBackgroundGC(wptr, &gc, &bgpv, 0, 0))
	{
	  int ox, oy;
	  EZ_GetWidgetTileOrigin(wptr, &ox, &oy);      
	  XSetTSOrigin(EZ_Display, gc, ox, oy);	  
	}
      XFillRectangle(EZ_DisplayForWidgets, pixmap, gc, 0,0, w, h); 
      EZ_DrawRectBorder(wptr, pixmap);           
      XCopyArea(EZ_DisplayForWidgets,pixmap,win, EZ_WRITABLEGC,0,0,w,h,0,0); 
      XFreePixmap(EZ_DisplayForWidgets, pixmap); 
    }
}
/*****************************************************************************/
void EZ_GetTermWidgetComponents(term, iterm, scroll)
     EZ_Widget *term, **iterm, **scroll;
{
  if(term && EZ_WidgetType(term) == EZ_WIDGET_TERM)
    {
      if(iterm) *iterm = EZ_TermITerm(term);
      if(scroll) *scroll = EZ_TermVScroll(term);
    }
}
/*****************************************************************************/
#undef _EZ_WIDGET_TERM_C_
