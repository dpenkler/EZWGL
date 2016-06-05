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
 ***          Widget Internal Terminal Emulator                ***
 ***                                                           ***
 *****************************************************************/
#define _EZ_WIDGET_ITERM_C_

#include "EZ_Widget.h"

/*********************************************************************
 * 
 *  Functions implemented in this file:
 */
void EZ_TermTouchScreen MY_ANSIARGS((EZ_Widget *widget));
void EZ_TermPowerOn     MY_ANSIARGS((EZ_Widget *widget));
void EZ_GetTermComponents MY_ANSIARGS((EZ_Widget *term,
                                       EZ_Widget **iterm,
                                       EZ_Widget **vscroll));


EZ_Widget *EZ_CreateITerm MY_ANSIARGS((EZ_Widget *parent));
void      EZ_update_iterm_scrollbars MY_ANSIARGS((EZ_Widget *widget));

void      EZ_DrawWidgetITerm MY_ANSIARGS((EZ_Widget *widget));
void      EZ_ComputeWidgetITermSize MY_ANSIARGS((EZ_Widget *widget, int *w, int *h));
void      EZ_FreeWidgetITermData MY_ANSIARGS((EZ_Widget *widget));

/*********************************************************************
 * 
 *  Local Variables.
 */
EXTERN void  EZ_ITermEventHandle MY_ANSIARGS((EZ_Widget *widget, XEvent *event));
EXTERN void  EZ_iterm_resize_window MY_ANSIARGS((EZ_Widget *widget));
EXTERN void  scr_refresh  MY_ANSIARGS((EZ_Widget *widget, int mode));
EXTERN void  EZ_ITermReverseVideo   MY_ANSIARGS((EZ_Widget *widget, int mode));
EXTERN void  scr_touch  MY_ANSIARGS((EZ_Widget *widget));
EXTERN void  scr_poweron  MY_ANSIARGS((EZ_Widget *widget));


static EZ_WidgetHandle EZ_ITermHandle =
{
  EZ_ComputeWidgetITermSize,
  EZ_DrawWidgetITerm,
  EZ_FreeWidgetITermData,
  EZ_ITermEventHandle,
};
/*********************************************************************/
EXTERN void EZ_InitITermScreen MY_ANSIARGS((EZ_Widget *wptr));
EXTERN void EZ_DestroyITermScreen MY_ANSIARGS((EZ_Widget *wptr));
EXTERN void EZ_InitITermCommand MY_ANSIARGS((EZ_Widget *wptr));
EXTERN void EZ_DestroyITermCommand MY_ANSIARGS((EZ_Widget *wptr));
EXTERN void EZ_ITermTimerCb MY_ANSIARGS((EZ_Timer *timer, void *pdata));
EXTERN void EZ_ITermBTimerCb MY_ANSIARGS((EZ_Timer *timer, void *pdata));
/*********************************************************************/

EZ_Widget  *EZ_CreateITerm(parent)
     EZ_Widget  *parent;     /* parent widget    */
{
  EZ_Widget  *wptr;

  wptr = EZ_CreateNewWidget(parent);
  /*--------------------------------------------------
   * Register the handling functions for ...
   *  has to be done after  EZ_CreateNewWiget.
   *-------------------------------------------------*/
  EZ_WidgetHandlingFunctions[EZ_WIDGET_ITERM] = &EZ_ITermHandle;
  EZ_SetWidgetTypeAndNames(wptr,EZ_WIDGET_ITERM);
  EZ_SetWidgetFocusableFlag(wptr);

  EZ_WidgetBorderStyle(wptr)  = EZ_BORDER_DOWN;
  EZ_WidgetBorderWidth(wptr)  = 2;
  EZ_WidgetPadB(wptr) = 2;
  EZ_WidgetFont(wptr) = EZ_GetFontFromId(EZ_TEXT_FONT);
  EZ_WidgetCursor(wptr) = EZ_GetCursor(EZ_C_XTERM);

  EZ_ITermCursorColor(wptr) = 0;
  EZ_ITermTextBG(wptr) = ~0;
  EZ_ITermDirty(wptr) = 1;
  EZ_ITermONRow(wptr) = -1;
  EZ_ITermONCol(wptr) = -1;
  EZ_ITermSaveLines(wptr) = 64;
  EZ_ITermFixedFont(wptr) = 1;

  EZ_ITermTerm(wptr) = (EZ_Widget *)NULL;
  EZ_ITermVScroll(wptr) = (EZ_Widget *)NULL;

  EZ_InitITermCommand(wptr);  
  EZ_InitITermScreen(wptr);
  EZ_ITermChildPid(wptr) = -1;
  EZ_ITermTimer(wptr) = EZ_CreateTimer(0, 1000, -1, 
                                       (EZ_CallBack)EZ_ITermTimerCb, 
                                       (void *)wptr, 0);
  EZ_ITermTimer(wptr) = EZ_CreateTimer(1, 0, -1, 
                                       (EZ_CallBack)EZ_ITermBTimerCb, 
                                       (void *)wptr, 0);
  return(wptr);
}

/********************************************************************/
void  EZ_FreeWidgetITermData(widget)
     EZ_Widget *widget;
{
  EZ_CancelTimer(EZ_ITermBTimer(widget));
  EZ_CancelTimer(EZ_ITermTimer(widget));
  EZ_DestroyITermScreen(widget);
  EZ_DestroyITermCommand(widget);
  if(EZ_ITermLPixmap(widget)) 
    XFreePixmap(EZ_Display, EZ_ITermLPixmap(widget));
  EZ_ITermLPixmap(widget) = None;
}
/********************************************************************
 *
 *  Figure out the dimension
 */
void EZ_ComputeWidgetITermSize(widget, w, h)
     EZ_Widget *widget;
     int             *w, *h;
{
  int  width, height, cw, ch, padb;

  if(!(EZ_GetWidgetSizeComputedFlag(widget)))
    {
      /* determine if font has a fixed width */
      int m = (XTextWidth(EZ_WidgetFont(widget), "MM/@MMMM", 8)) >> 3;
      int i = (XTextWidth(EZ_WidgetFont(widget), "iiiiiiii", 8)) >> 3;
      if(m == i) 
        {
          EZ_ITermFWidth(widget) = m;
          EZ_ITermFixedFont(widget) = 1;
        }
      else
        {
          EZ_ITermFWidth(widget) = EZ_WidgetFont(widget)->max_bounds.width;
          EZ_ITermFixedFont(widget) = 0;
        }
      EZ_ITermFHeight(widget) = EZ_WidgetFont(widget)->ascent + EZ_WidgetFont(widget)->descent;
      EZ_ITermFAscent(widget) = EZ_WidgetFont(widget)->ascent;
      EZ_SetWidgetSizeComputedFlag(widget);
      EZ_ITermDirty(widget) = 1;
    }
  
  height = EZ_ITermFHeight(widget) * 4;
  width =  EZ_ITermFWidth(widget) * 20;
  cw = EZ_WidgetPadX(widget) + EZ_WidgetBorderWidth(widget);
  ch = EZ_WidgetPadY(widget) + EZ_WidgetBorderWidth(widget);
  cw = cw + cw;
  ch = ch + ch;
  padb = EZ_WidgetPadB(widget)<<1;

  *w = width + cw + padb;
  *h = height + ch + padb;
}

/****************************************************************************
 *
 *  Draw a ITerm widget
 */

void  EZ_DrawWidgetITerm(wptr)
     EZ_Widget *wptr;
{
  int             w, h, padb, padb2, tile;
  Pixmap          pixmap;
  Window          win;
  GC              gc;
  unsigned long   bgpv;

  win = EZ_WidgetWindow(wptr);
  w   = EZ_WidgetWidth(wptr);
  h   = EZ_WidgetHeight(wptr);
  padb = EZ_WidgetPadB(wptr);
  padb2 = padb + padb;

  pixmap = XCreatePixmap(EZ_DisplayForWidgets, win, w, h, EZ_Depth); 
  EZ_FixFocusHighlightBorder(wptr, pixmap, w, h, padb);

  if(EZ_ITermRVideoPending(wptr) || EZ_ITermRVideo(wptr))
    {
      bgpv = EZ_WidgetForeground(wptr);	
      XSetForeground(EZ_Display, EZ_WRITABLEGC, bgpv);
      gc = EZ_WRITABLEGC;
    }
  else if(EZ_ITermTextBG(wptr)  != ~0)
    {
      bgpv = EZ_ITermTextBG(wptr);	
      XSetForeground(EZ_Display, EZ_WRITABLEGC, bgpv);
      gc = EZ_WRITABLEGC;
    }
  else
    {
      tile = EZ_GetBackgroundGC(wptr, &gc,&bgpv, 1, 0);
      if(tile)
        {
          int ox, oy;
          EZ_GetWidgetTileOrigin(wptr, &ox, &oy);      
          XSetTSOrigin(EZ_Display, gc, ox, oy);
        }  
    }

  if(EZ_ITermChildPid(wptr) < 0)
    {
      XFillRectangle(EZ_Display, pixmap, gc, padb,padb, w-padb2, h-padb2); 
      EZ_DrawRectBorder(wptr, pixmap);
      XCopyArea(EZ_Display,pixmap,win, EZ_WRITABLEGC,0,0,w,h,0,0);  
    }
  else
    {
      int bw = EZ_WidgetBorderWidth(wptr);
      int xoff=bw+padb;
      int yoff=bw+padb;
      int padx = EZ_WidgetPadX(wptr);
      int pady = EZ_WidgetPadY(wptr);
      int xxx, yyy;
      if(EZ_ITermDirty(wptr))
        {
          unsigned int colorfgbg, fgc, i;
          screen_t *scr = EZ_ITermScreen(wptr);
          fgc = EZ_WidgetForeground(wptr);
          if(fgc != scr->pixColors[arbColor])
            {
              int r,g,b;
              scr->pixColors[itfgColor] = fgc;
              scr->pixColors[arbColor] = fgc;
              EZ_PixelValue2RGB(fgc, &r, &g, &b);
              r = (r * 13)/10; if(r>=255) r=255;
              g = (g * 13)/10; if(g>=255) g=255;
              b = (b * 13)/10; if(b>=255) b=255;
              scr->pixColors[arbHColor] = EZ_AllocateColorFromRGB(r,g,b);
            }
          if((scr->cmasks & (1<<colorBD))==0)
            scr->pixColors[colorBD] = scr->pixColors[itfgColor];
          if((scr->cmasks & (1<<colorUL))==0)
            scr->pixColors[colorUL] = scr->pixColors[itfgColor];
          colorfgbg = DEFAULT_RSTYLE;
          for(i = minColor; i <= maxColor; i++)
            {
              if(scr->pixColors[itfgColor] == scr->pixColors[i]
                 && scr->pixColors[itfgColor] == scr->pixColors[colorBD])
                colorfgbg = SET_FGCOLOR(colorfgbg, i);
              if(scr->pixColors[itbgColor] == scr->pixColors[i])
                colorfgbg = SET_BGCOLOR(colorfgbg, i);
            }
          EZ_ITermColorFgBg(wptr) = colorfgbg;
          EZ_iterm_resize_window(wptr);
          EZ_ITermDirty(wptr) = 0;
        }
      xxx = EZ_ITermXDiff(wptr)+padx;
      yyy = EZ_ITermYDiff(wptr)+pady;

      if(padx) XFillRectangle(EZ_Display, pixmap, gc, xoff, yoff, padx, h-yoff);
      if(pady) XFillRectangle(EZ_Display, pixmap, gc, xoff, yoff, w-xoff, pady);
      if(xxx)  XFillRectangle(EZ_Display, pixmap, gc, w-xoff-xxx, yoff, xxx, h-yoff);
      if(yyy)  XFillRectangle(EZ_Display, pixmap, gc, xoff, h-yoff-yyy, w-yoff, yyy);

      EZ_DrawRectBorder(wptr, pixmap);
      if(padx) XCopyArea(EZ_Display,pixmap,win, EZ_WRITABLEGC, xoff, yoff, padx, h-yoff, xoff, yoff);
      if(pady) XCopyArea(EZ_Display,pixmap,win, EZ_WRITABLEGC, xoff, yoff, w-xoff, pady, xoff, yoff);
      if(xxx)  XCopyArea(EZ_Display,pixmap,win, EZ_WRITABLEGC, w-xoff-xxx, yoff, xxx, h-yoff,
                         w-xoff-xxx, yoff);
      if(yyy)  XCopyArea(EZ_Display,pixmap,win, EZ_WRITABLEGC, xoff, h-yoff-yyy, w-yoff, yyy,
                         xoff, h-yoff-yyy);
      XCopyArea(EZ_Display,pixmap,win, EZ_WRITABLEGC,0,0,xoff,h,0,0);        
      XCopyArea(EZ_Display,pixmap,win, EZ_WRITABLEGC,w-xoff,0,xoff,h,w-xoff,0);        
      XCopyArea(EZ_Display,pixmap,win, EZ_WRITABLEGC,xoff,0,w-(xoff+xoff),yoff,xoff,0);        
      XCopyArea(EZ_Display,pixmap,win, EZ_WRITABLEGC,xoff,(h-yoff),w-(xoff+xoff),yoff,xoff,h-yoff);
      if(EZ_ITermRVideoPending(wptr))
        {
          EZ_ITermRVideoPending(wptr) = 0;
          EZ_ITermReverseVideo(wptr, 1);
        }
      else scr_refresh(wptr, 1);
    }
  XFreePixmap(EZ_Display, pixmap);
  EZ_update_iterm_scrollbars(wptr);
}
/*************************************************************************/
void EZ_update_iterm_scrollbars(wptr)
     EZ_Widget *wptr;
{
  if(EZ_LookupWidgetFromAllHT(EZ_ITermVScroll(wptr)) != (EZ_Widget *)NULL)
    {
      int total = EZ_ITermNScrolled(wptr);
      int nrows = EZ_ITermNRow(wptr);
      int start = EZ_ITermViewStart(wptr);
      start = total - start;
      total += nrows;
      EZ_UpdateScrollbarI(EZ_ITermVScroll(wptr),
                          total,
                          nrows,
                          start);
    }
  else
    {
      EZ_Widget *p = EZ_ITermTerm(wptr);
      EZ_ITermVScroll(wptr) = NULL;
      if(p) EZ_TermVScroll(p) = NULL;
    }
}
/***********************************************************************************/
void EZ_TermTouchScreen(widget) EZ_Widget *widget;
{
  if(widget)
    {
      EZ_Widget *twidget = NULL;
      if(EZ_WidgetType(widget) == EZ_WIDGET_TERM)
        twidget = EZ_TermITerm(widget);
      else  if(EZ_WidgetType(widget) == EZ_WIDGET_ITERM)
        twidget = widget;
      if(twidget) scr_touch(twidget);
    }
}

void EZ_TermPowerOn(widget) EZ_Widget *widget;
{
  if(widget)
    {
      EZ_Widget *twidget = NULL;
      if(EZ_WidgetType(widget) == EZ_WIDGET_TERM)
        twidget = EZ_TermITerm(widget);
      else  if(EZ_WidgetType(widget) == EZ_WIDGET_ITERM)
        twidget = widget;
      if(twidget) scr_poweron(twidget);
    }
}
void EZ_GetTermComponents(term, iterm, vscroll)
     EZ_Widget *term, **iterm, **vscroll;
{
  if(term)
    {
      if(EZ_WidgetType(term) == EZ_WIDGET_TERM)
        {
          if(iterm)   *iterm = EZ_TermITerm(term);
          if(vscroll) *vscroll = EZ_TermVScroll(term);
        }
      else  if(EZ_WidgetType(term) == EZ_WIDGET_ITERM)
        {
          if(iterm)   *iterm = term;
          if(vscroll) *vscroll = EZ_ITermVScroll(term);
        }
    }
}
/***********************************************************************************/
#undef _EZ_WIDGET_ITERM_C_
