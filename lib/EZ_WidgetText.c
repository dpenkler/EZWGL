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
 ***              TextWidget                                   ***
 ***                                                           ***
 *****************************************************************/
#define _EZ_WIDGET_TEXT_C_
#include "EZ_Widget.h"

/*********************************************************************
 * 
 *  Functions implemented in this file:
 */
EZ_Widget        *EZ_CreateTextWidget MY_ANSIARGS((EZ_Widget *parent, int editable, int hs,int vs));
void             EZ_DrawWidgetText MY_ANSIARGS((EZ_Widget *widget));
void             EZ_ComputeWidgetTextSize MY_ANSIARGS((EZ_Widget *widget, int *w, int *h));

void             EZ_TextLoadFile MY_ANSIARGS((EZ_Widget *widget,char *fname));
void             EZ_TextLoadAnnotatedFile MY_ANSIARGS((EZ_Widget *widget, char *fname));
void             EZ_TextClear MY_ANSIARGS((EZ_Widget *widget));
int              EZ_TextSaveText  MY_ANSIARGS((EZ_Widget *widget, char *file));
int              EZ_TextSaveAnnotatedText  MY_ANSIARGS((EZ_Widget *widget, char *file));
int              EZ_TextSetMarker  MY_ANSIARGS((EZ_Widget *widget));
int              EZ_TextGetPosition  MY_ANSIARGS((EZ_Widget *widget));
void             EZ_TextGotoPosition  MY_ANSIARGS((EZ_Widget *widget, int pos));
char             *EZ_TextGetRegionString  MY_ANSIARGS((EZ_Widget *widget));
char             *EZ_TextGetBufferString  MY_ANSIARGS((EZ_Widget *widget));

int              EZ_TextBufferModified MY_ANSIARGS((EZ_Widget *widget));

void             EZ_GetTextWidgetComponents  MY_ANSIARGS((EZ_Widget *widget,
							  EZ_Widget **itxt,
							  EZ_Widget **hscr,
							  EZ_Widget **vscr ));



void             EZ_TextNextLine MY_ANSIARGS((EZ_Widget *widget));
void             EZ_TextPreviousLine MY_ANSIARGS((EZ_Widget *widget));
void             EZ_TextBackwardChar  MY_ANSIARGS((EZ_Widget *widget));
void             EZ_TextForwardChar  MY_ANSIARGS((EZ_Widget *widget));
void             EZ_TextPreviousPage  MY_ANSIARGS((EZ_Widget *widget));
void             EZ_TextForwardWord  MY_ANSIARGS((EZ_Widget *widget));
void             EZ_TextBackwardWord  MY_ANSIARGS((EZ_Widget *widget));
void             EZ_TextNextPage  MY_ANSIARGS((EZ_Widget *widget));
void             EZ_TextEndOfBuffer  MY_ANSIARGS((EZ_Widget *widget));
void             EZ_TextBeginningOfBuffer  MY_ANSIARGS((EZ_Widget *widget));
void             EZ_TextEndOfLine  MY_ANSIARGS((EZ_Widget *widget));
void             EZ_TextBeginningOfLine  MY_ANSIARGS((EZ_Widget *widget));
void             EZ_TextInsertChar  MY_ANSIARGS((EZ_Widget *widget, char c));
void             EZ_TextDeletePreviousChar  MY_ANSIARGS((EZ_Widget *widget));
void             EZ_TextDeleteNextChar  MY_ANSIARGS((EZ_Widget *widget));
void             EZ_TextInsertNewLine  MY_ANSIARGS((EZ_Widget *widget));
void             EZ_TextKillEndOfLine  MY_ANSIARGS((EZ_Widget *widget));
void             EZ_TextKillRegion  MY_ANSIARGS((EZ_Widget *widget));
void             EZ_TextPasteSelection  MY_ANSIARGS((EZ_Widget *widget));
void             EZ_TextReDisplay  MY_ANSIARGS((EZ_Widget *widget));
void             EZ_TextInsertString  MY_ANSIARGS((EZ_Widget *widget, char *str));
void             EZ_TextInsertAnnotatedString MY_ANSIARGS((EZ_Widget *widget, char *str));

void             EZ_TextInsertStringWithProperty  MY_ANSIARGS((EZ_Widget *widget, char *str, 
							       EZ_TextProperty *prop));

void             EZ_TextSetRegionFont  MY_ANSIARGS((EZ_Widget *widget, char *fontname));
void             EZ_TextSetRegionForeground  MY_ANSIARGS((EZ_Widget *widget, char *clr));
void             EZ_TextSetRegionBackground  MY_ANSIARGS((EZ_Widget *widget, char *clr));
void             EZ_TextSetRegionUnderline  MY_ANSIARGS((EZ_Widget *widget, int udl));
void             EZ_TextSetRegionProperty  MY_ANSIARGS((EZ_Widget *widget, EZ_TextProperty *prop));

void             EZ_TextReplaceRegionWithPixmap  MY_ANSIARGS((EZ_Widget *widget, char *file));
int              EZ_GetTextSpace  MY_ANSIARGS((EZ_Widget *widget));
/*********************************************************************
 * 
 *  Local Variables.
 */
static EZ_WidgetHandle EZ_TextHandle =
{
  EZ_ComputeWidgetTextSize,
  EZ_DrawWidgetText,
  EZ_FreeUnknownData,
  EZ_UnkownEventHandle,
};

/*********************************************************************/
EZ_Widget  *EZ_CreateTextWidget(parent, editable, hscrollbar, vscrollbar)
     EZ_Widget  *parent;    /* parent widget    */
     int hscrollbar, vscrollbar, editable;
{
  EZ_Widget  *wptr, *iText, *hscroll = NULL, *vscroll = NULL;


  wptr = EZ_CreateNewWidget(parent);
  /*--------------------------------------------------
   * Register the handling functions for ...
   *  has to be done after  EZ_CreateNewWidget.
   *-------------------------------------------------*/
  EZ_WidgetHandlingFunctions[EZ_WIDGET_TEXT] = &EZ_TextHandle;
  
  EZ_WidgetStacking(wptr) = EZ_VERTICAL;
  EZ_SetWidgetTypeAndNames(wptr, EZ_WIDGET_TEXT);
  EZ_WidgetBorderStyle(wptr) = EZ_BORDER_NONE;
  EZ_WidgetBorderWidth(wptr) = 0;
  EZ_WidgetFillMode(wptr) = EZ_FILL_VERTICALLY;
  EZ_WidgetPadX(wptr) = 0;
  EZ_WidgetPadY(wptr) = 0;
  EZ_WidgetSepX(wptr)= 0;
  EZ_WidgetSepY(wptr)= 0;

  EZ_SetRowColConstrains(wptr, EZ_ROW,    0, 16, 1000, 0);
  EZ_SetRowColConstrains(wptr, EZ_ROW,    1, 0,  0,    0);
  EZ_SetRowColConstrains(wptr, EZ_COLUMN, 0, 16, 1000, 0);
  EZ_SetRowColConstrains(wptr, EZ_COLUMN, 1, 0,  0,    0);

  iText = EZ_TextIText(wptr) = EZ_CreateIText(wptr,editable);
  EZ_WidgetGroupLeader(iText) = wptr;
  EZ_WidgetGBX(iText) = 0;
  EZ_WidgetGBY(iText) = 0;
  EZ_WidgetGBW(iText) = 1;
  EZ_WidgetGBH(iText) = 1;

  EZ_SetWidgetExpandFlag(iText);

  if(vscrollbar)
    {
      vscroll = EZ_TextVScroll(wptr) = EZ_CreateScrollbar(wptr,EZ_WIDGET_VERTICAL_SCROLLBAR);
      EZ_SetWidgetExpandFlag(vscroll);
      EZ_ScrollObject(vscroll) = iText;
      EZ_WidgetGroupLeader(vscroll) = wptr;
      EZ_WidgetGBX(vscroll) = 1;
      EZ_WidgetGBY(vscroll) = 0;
      EZ_WidgetGBW(vscroll) = 1;
      EZ_WidgetGBH(vscroll) = 1;
    }
  if(hscrollbar)  
    {
      hscroll = EZ_TextHScroll(wptr) = EZ_CreateScrollbar(wptr,EZ_WIDGET_HORIZONTAL_SCROLLBAR);
      EZ_SetWidgetExpandFlag(hscroll);
      EZ_ScrollObject(hscroll) = iText;
      EZ_WidgetGroupLeader(hscroll) = wptr;
      EZ_WidgetGBX(hscroll) = 0;
      EZ_WidgetGBY(hscroll) = 1;
      EZ_WidgetGBW(hscroll) = 1;
      EZ_WidgetGBH(hscroll) = 1;
    }
  EZ_ITextHScroll(iText) = hscroll;
  EZ_ITextVScroll(iText) = vscroll;  
  EZ_ITextTextWidget(iText) = wptr;

  return(wptr);
}
/********************************************************************
 *
 *  Figure out the dimension of an internal list box.
 */
void EZ_ComputeWidgetTextSize(widget, w, h)
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

void  EZ_DrawWidgetText(wptr)
     EZ_Widget *wptr;
{

  if(  EZ_WidgetNumChildren(wptr) > 2 ||
     (EZ_TextHScroll(wptr) == NULL && EZ_WidgetNumChildren(wptr) > 1))
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
      
      XCopyArea(EZ_DisplayForWidgets,pixmap,win, EZ_WRITABLEGC,0,0,w,h,0,0); 
      XFreePixmap(EZ_DisplayForWidgets, pixmap); 
    }
}

/*****************************************************************************/
void  EZ_SetTextHScrollbar(widget, scroll)
     EZ_Widget *widget, *scroll;
{
  if(widget && scroll && EZ_WidgetType(scroll) == EZ_WIDGET_HORIZONTAL_SCROLLBAR)
    {
      int type = EZ_WidgetType(widget); 
      if(type  == EZ_WIDGET_TEXT)
	{
	  EZ_TextHScroll(widget) =
	    EZ_ITextHScroll(EZ_TextIText(widget)) = scroll;
	}
      else if(type  == EZ_WIDGET_ITEXT)
	{
	  EZ_ITextHScroll(widget) = scroll;
	}
    }
}
void  EZ_SetTextVScrollbar(widget, scroll)
     EZ_Widget *widget, *scroll;
{
  if(widget && scroll && EZ_WidgetType(scroll) == EZ_WIDGET_VERTICAL_SCROLLBAR)
    {
      int type = EZ_WidgetType(widget); 
      if(type  == EZ_WIDGET_TEXT)
	{
	  EZ_TextVScroll(widget) =
	    EZ_ITextVScroll(EZ_TextIText(widget)) = scroll;
	}
      else if(type  == EZ_WIDGET_ITEXT)
	{
	  EZ_ITextVScroll(widget) = scroll;
	}      
    }
}
/*****************************************************************************/
void  EZ_GetTextVState(widget, total, page, first)
     EZ_Widget *widget;
     int *total, *page, *first;
{
  if(widget != (EZ_Widget *)NULL)
    {
      int type = EZ_WidgetType(widget);
      EZ_Widget *t = NULL;
      if(type  == EZ_WIDGET_TEXT)
	{
	  t = EZ_TextIText(widget);
	}
      else if(type == EZ_WIDGET_ITEXT)
	t = widget;
      if(t)
	{
	  *total = EZ_ITextNLines(t);
	  *page = EZ_ITextPageSize(t);
	  *first = EZ_ITextFirstLine(t);
	}
    }
}
/*****************************************************************************/
void  EZ_GetTextHState(widget, total, page, first)
     EZ_Widget *widget;
     int *total, *page, *first;
{
  if(widget != (EZ_Widget *)NULL)
    {
      int type = EZ_WidgetType(widget);
      EZ_Widget *t = NULL;
      if(type  == EZ_WIDGET_TEXT)
	{
	  t = EZ_TextIText(widget);
	}
      else if(type == EZ_WIDGET_ITEXT)
	t = widget;
      if(t)
	{
	  *total = EZ_ITextMaxHSize(t);
	  *page = EZ_ITextHPageSize(t);
	  *first = EZ_ITextHStart(t);
	}
    }
}
/*****************************************************************************/
void  EZ_SetTextVState(widget, first)
     EZ_Widget *widget;
     int first;
{
  if(widget != (EZ_Widget *)NULL)
    {
      int type = EZ_WidgetType(widget);
      EZ_Widget *t = NULL;
      if(type  == EZ_WIDGET_TEXT)
	{
	  t = EZ_TextIText(widget);
	}
      else if(type == EZ_WIDGET_ITEXT)
	t = widget;
      if(t)
	{
	  EZ_ITextFirstLine(t) = first;
	}
    }
}
/*****************************************************************************/
void  EZ_SetTextHState(widget, first)
     EZ_Widget *widget;
     int  first;
{
  if(widget != (EZ_Widget *)NULL)
    {
      int type = EZ_WidgetType(widget);
      EZ_Widget *t = NULL;
      if(type  == EZ_WIDGET_TEXT)
	{
	  t = EZ_TextIText(widget);
	}
      else if(type == EZ_WIDGET_ITEXT)
	t = widget;
      if(t)
	{
	  EZ_ITextHStart(t) = first;
	}
    }
}

/*****************************************************************************
 *
 *     Editing Commands.
 *
 *****************************************************************************/
void EZ_TextLoadFile(widget, file)
     EZ_Widget *widget; char *file;
{
  EZ_Widget *t = NULL;
  if(widget)
    {
      if(EZ_WidgetType(widget) == EZ_WIDGET_TEXT)
	t = EZ_TextIText(widget);
      else if(EZ_WidgetType(widget) == EZ_WIDGET_ITEXT)
	t = widget;
      if(t)
	{
	  EZ_FreezeWidget(widget);
	  EZ_ITextLoadFile(t, file);
	  EZ_UnFreezeWidget(widget);
	}
    }
}
void  EZ_TextLoadAnnotatedFile(widget, file)
     EZ_Widget *widget;
     char      *file;
{
  EZ_Widget *t = NULL;
  if(widget)
    {
      if(EZ_WidgetType(widget) == EZ_WIDGET_TEXT)
	t = EZ_TextIText(widget);
      else if(EZ_WidgetType(widget) == EZ_WIDGET_ITEXT)
	t = widget;
      if(t)
	{
	  EZ_FreezeWidget(widget);
	  EZ_ITextLoadAnnotatedFile(t, file);
	  EZ_UnFreezeWidget(widget);
	}
    }
}
/*****************************************************************************/
void EZ_TextClear(widget)
     EZ_Widget *widget;
{
  EZ_Widget *t = NULL;
  if(widget)
    {
      if(EZ_WidgetType(widget) == EZ_WIDGET_TEXT)
	t = EZ_TextIText(widget);
      else if(EZ_WidgetType(widget) == EZ_WIDGET_ITEXT)
	t = widget;
      if(t)
	{
	  EZ_ITextClear(t);
	}
    }
}
/*****************************************************************************/
void EZ_TextNextLine(widget)
     EZ_Widget *widget;
{
  EZ_Widget *t = NULL;
  if(widget)
    {
      if(EZ_WidgetType(widget) == EZ_WIDGET_TEXT)
	t = EZ_TextIText(widget);
      else if(EZ_WidgetType(widget) == EZ_WIDGET_ITEXT)
	t = widget;
      if(t)
	{
	  EZ_ITextNextLine(t);
	}
    }
}
/*****************************************************************************/
void EZ_TextPreviousLine(widget)
     EZ_Widget *widget;
{
  EZ_Widget *t = NULL;
  if(widget)
    {
      if(EZ_WidgetType(widget) == EZ_WIDGET_TEXT)
	t = EZ_TextIText(widget);
      else if(EZ_WidgetType(widget) == EZ_WIDGET_ITEXT)
	t = widget;
      if(t)
	{
	  EZ_ITextPreviousLine(t);
	}
    }
}
/*****************************************************************************/
void EZ_TextBackwardChar(widget)
     EZ_Widget *widget;
{
  EZ_Widget *t = NULL;
  if(widget)
    {
      if(EZ_WidgetType(widget) == EZ_WIDGET_TEXT)
	t = EZ_TextIText(widget);
      else if(EZ_WidgetType(widget) == EZ_WIDGET_ITEXT)
	t = widget;
      if(t)
	{
	  EZ_ITextBackwardChar(t);
	}
    }
}
/*****************************************************************************/
void EZ_TextForwardChar(widget)
     EZ_Widget *widget;
{
  EZ_Widget *t = NULL;
  if(widget)
    {
      if(EZ_WidgetType(widget) == EZ_WIDGET_TEXT)
	t = EZ_TextIText(widget);
      else if(EZ_WidgetType(widget) == EZ_WIDGET_ITEXT)
	t = widget;
      if(t)
	{
	  EZ_ITextForwardChar(t);
	}
    }
}
/*****************************************************************************/
void EZ_TextForwardWord(widget)
     EZ_Widget *widget;
{
  EZ_Widget *t = NULL;
  if(widget)
    {
      if(EZ_WidgetType(widget) == EZ_WIDGET_TEXT)
	t = EZ_TextIText(widget);
      else if(EZ_WidgetType(widget) == EZ_WIDGET_ITEXT)
	t = widget;
      if(t)
	{
	  EZ_ITextForwardWord(t);
	}
    }
}
/*****************************************************************************/
void EZ_TextBackwardWord(widget)
     EZ_Widget *widget;
{
  EZ_Widget *t = NULL;
  if(widget)
    {
      if(EZ_WidgetType(widget) == EZ_WIDGET_TEXT)
	t = EZ_TextIText(widget);
      else if(EZ_WidgetType(widget) == EZ_WIDGET_ITEXT)
	t = widget;
      if(t)
	{
	  EZ_ITextBackwardWord(t);
	}
    }
}
/*****************************************************************************/
void EZ_TextPreviousPage(widget)
     EZ_Widget *widget;
{
  EZ_Widget *t = NULL;
  if(widget)
    {
      if(EZ_WidgetType(widget) == EZ_WIDGET_TEXT)
	t = EZ_TextIText(widget);
      else if(EZ_WidgetType(widget) == EZ_WIDGET_ITEXT)
	t = widget;
      if(t)
	{
	  EZ_ITextPreviousPage(t);
	}
    }
}
/*****************************************************************************/
void EZ_TextNextPage(widget)
     EZ_Widget *widget;
{
  EZ_Widget *t = NULL;
  if(widget)
    {
      if(EZ_WidgetType(widget) == EZ_WIDGET_TEXT)
	t = EZ_TextIText(widget);
      else if(EZ_WidgetType(widget) == EZ_WIDGET_ITEXT)
	t = widget;
      if(t)
	{
	  EZ_ITextNextPage(t);
	}
    }
}
/*****************************************************************************/
void EZ_TextEndOfBuffer(widget)
     EZ_Widget *widget;
{
  EZ_Widget *t = NULL;
  if(widget)
    {
      if(EZ_WidgetType(widget) == EZ_WIDGET_TEXT)
	t = EZ_TextIText(widget);
      else if(EZ_WidgetType(widget) == EZ_WIDGET_ITEXT)
	t = widget;
      if(t)
	{
	  EZ_ITextEndOfBuffer(t);
	}
    }
}
/*****************************************************************************/
void EZ_TextBeginningOfBuffer(widget)
     EZ_Widget *widget;
{
  EZ_Widget *t = NULL;
  if(widget)
    {
      if(EZ_WidgetType(widget) == EZ_WIDGET_TEXT)
	t = EZ_TextIText(widget);
      else if(EZ_WidgetType(widget) == EZ_WIDGET_ITEXT)
	t = widget;
      if(t)
	{
	  EZ_ITextBeginningOfBuffer(t);
	}
    }
}
/*****************************************************************************/
void EZ_TextEndOfLine(widget)
     EZ_Widget *widget;
{
  EZ_Widget *t = NULL;
  if(widget)
    {
      if(EZ_WidgetType(widget) == EZ_WIDGET_TEXT)
	t = EZ_TextIText(widget);
      else if(EZ_WidgetType(widget) == EZ_WIDGET_ITEXT)
	t = widget;
      if(t)
	{
	  EZ_ITextEndOfLine(t);
	}
    }
}
/*****************************************************************************/
void EZ_TextBeginningOfLine(widget)
     EZ_Widget *widget;
{
  EZ_Widget *t = NULL;
  if(widget)
    {
      if(EZ_WidgetType(widget) == EZ_WIDGET_TEXT)
	t = EZ_TextIText(widget);
      else if(EZ_WidgetType(widget) == EZ_WIDGET_ITEXT)
	t = widget;
      if(t)
	{
	  EZ_ITextBeginningOfLine(t);
	}
    }
}
/*****************************************************************************/
void EZ_TextInsertChar(widget,theChar)
     EZ_Widget *widget;
     char      theChar;
{
  EZ_Widget *t = NULL;
  if(widget)
    {
      if(EZ_WidgetType(widget) == EZ_WIDGET_TEXT)
	t = EZ_TextIText(widget);
      else if(EZ_WidgetType(widget) == EZ_WIDGET_ITEXT)
	t = widget;
      if(t)
	{
	  EZ_ITextInsertChar(t, theChar);
	}
    }
}
void EZ_TextDeletePreviousChar(widget)
     EZ_Widget *widget;
{
  if(widget && EZ_WidgetType(widget) == EZ_WIDGET_TEXT)
    {
      EZ_ITextDeletePreviousChar(EZ_TextIText(widget));
    }
}
/*****************************************************************************/
void EZ_TextDeleteNextChar(widget)
     EZ_Widget *widget;
{
  EZ_Widget *t = NULL;
  if(widget)
    {
      if(EZ_WidgetType(widget) == EZ_WIDGET_TEXT)
	t = EZ_TextIText(widget);
      else if(EZ_WidgetType(widget) == EZ_WIDGET_ITEXT)
	t = widget;
      if(t)
	{
	  EZ_ITextDeleteNextChar(t);
	}
    }
}
/*****************************************************************************/
void EZ_TextInsertNewLine(widget)
     EZ_Widget *widget;
{
  EZ_Widget *t = NULL;
  if(widget)
    {
      if(EZ_WidgetType(widget) == EZ_WIDGET_TEXT)
	t = EZ_TextIText(widget);
      else if(EZ_WidgetType(widget) == EZ_WIDGET_ITEXT)
	t = widget;
      if(t)
	{
	  EZ_ITextInsertNewLine(t,1);
	}
    }
}
/*****************************************************************************/
void EZ_TextKillEndOfLine(widget)
     EZ_Widget *widget;
{
  EZ_Widget *t = NULL;
  if(widget)
    {
      if(EZ_WidgetType(widget) == EZ_WIDGET_TEXT)
	t = EZ_TextIText(widget);
      else if(EZ_WidgetType(widget) == EZ_WIDGET_ITEXT)
	t = widget;
      if(t)
	{
	  EZ_ITextKillEndOfLine(t);
	}
    }
}
/*****************************************************************************/
int EZ_TextSetMarker(widget)
     EZ_Widget *widget;
{
  EZ_Widget *t = NULL;
  if(widget)
    {
      if(EZ_WidgetType(widget) == EZ_WIDGET_TEXT)
	t = EZ_TextIText(widget);
      else if(EZ_WidgetType(widget) == EZ_WIDGET_ITEXT)
	t = widget;
      if(t)
	{
	  return(EZ_ITextSetMarker(t));
	}
    }
  return(0);
}
/*****************************************************************************/
void EZ_TextKillRegion(widget)
     EZ_Widget *widget;
{
  EZ_Widget *t = NULL;
  if(widget)
    {
      if(EZ_WidgetType(widget) == EZ_WIDGET_TEXT)
	t = EZ_TextIText(widget);
      else if(EZ_WidgetType(widget) == EZ_WIDGET_ITEXT)
	t = widget;
      if(t)
	{
	  EZ_ITextKillRegion(t);
	}
    }
}
/*****************************************************************************/
void EZ_TextPasteSelection(widget)
     EZ_Widget *widget;
{
  EZ_Widget *t = NULL;
  if(widget)
    {
      if(EZ_WidgetType(widget) == EZ_WIDGET_TEXT)
	t = EZ_TextIText(widget);
      else if(EZ_WidgetType(widget) == EZ_WIDGET_ITEXT)
	t = widget;
      if(t)
	{
	  EZ_ITextPasteSelection(t);
	}
    }
}
/*****************************************************************************/
void EZ_TextReDisplay(widget)
     EZ_Widget *widget;
{
  EZ_Widget *t = NULL;
  if(widget)
    {
      if(EZ_WidgetType(widget) == EZ_WIDGET_TEXT)
	t = EZ_TextIText(widget);
      else if(EZ_WidgetType(widget) == EZ_WIDGET_ITEXT)
	t = widget;
      if(t)
	{
	  EZ_ITextReDisplay(t);
	}
    }
}
/*****************************************************************************/
void EZ_TextInsertString(widget,str)
     EZ_Widget *widget;
     char      *str;
{
  EZ_Widget *t = NULL;
  if(widget)
    {
      if(EZ_WidgetType(widget) == EZ_WIDGET_TEXT)
	t = EZ_TextIText(widget);
      else if(EZ_WidgetType(widget) == EZ_WIDGET_ITEXT)
	t = widget;
      if(t)
	{
	  EZ_ITextInsertString(t,str);
	}
    }
}
void EZ_TextInsertAnnotatedString(widget,str)
     EZ_Widget *widget;
     char      *str;
{
  EZ_Widget *t = NULL;
  if(widget)
    {
      if(EZ_WidgetType(widget) == EZ_WIDGET_TEXT)
	t = EZ_TextIText(widget);
      else if(EZ_WidgetType(widget) == EZ_WIDGET_ITEXT)
	t = widget;
      if(t)
	{
	  EZ_ITextInsertAnnotatedString(t,str);
	}
    }
}
/*****************************************************************************/
void EZ_TextInsertStringWithProperty(widget,str, prop)
     EZ_Widget *widget;
     char      *str;
     TextProp  *prop;
{
  EZ_Widget *t = NULL;
  if(widget)
    {
      if(EZ_WidgetType(widget) == EZ_WIDGET_TEXT)
	t = EZ_TextIText(widget);
      else if(EZ_WidgetType(widget) == EZ_WIDGET_ITEXT)
	t = widget;
      if(t)
	{
	  EZ_ITextInsertStringWithProperty(t,str, prop);
	}
    }
}
/*****************************************************************************/
void  EZ_TextSetRegionFont(widget, font_name)
     EZ_Widget *widget;
     char      *font_name;
{
  EZ_Widget *t = NULL;
  if(widget)
    {
      if(EZ_WidgetType(widget) == EZ_WIDGET_TEXT)
	t = EZ_TextIText(widget);
      else if(EZ_WidgetType(widget) == EZ_WIDGET_ITEXT)
	t = widget;
      if(t)
	{
	  EZ_ITextSetRegionFont(t, font_name);
	}
    }
}
/*****************************************************************************/
void  EZ_TextSetRegionUnderline(widget, underline)
     EZ_Widget *widget;
     int       underline;
{
  EZ_Widget *t = NULL;
  if(widget)
    {
      if(EZ_WidgetType(widget) == EZ_WIDGET_TEXT)
	t = EZ_TextIText(widget);
      else if(EZ_WidgetType(widget) == EZ_WIDGET_ITEXT)
	t = widget;
      if(t)
	{
	  EZ_ITextSetRegionUnderline(t, underline);
	}
    }
}
/*****************************************************************************/
void EZ_TextSetRegionForeground(widget, color_name)
     EZ_Widget *widget;
     char      *color_name;
{
  EZ_Widget *t = NULL;
  if(widget)
    {
      if(EZ_WidgetType(widget) == EZ_WIDGET_TEXT)
	t = EZ_TextIText(widget);
      else if(EZ_WidgetType(widget) == EZ_WIDGET_ITEXT)
	t = widget;
      if(t)
	{
	  EZ_ITextSetRegionForeground(t, color_name);
	}
    }
}
/*****************************************************************************/
void EZ_TextSetRegionBackground(widget, color_name)
     EZ_Widget *widget;
     char      *color_name;
{
  EZ_Widget *t = NULL;
  if(widget)
    {
      if(EZ_WidgetType(widget) == EZ_WIDGET_TEXT)
	t = EZ_TextIText(widget);
      else if(EZ_WidgetType(widget) == EZ_WIDGET_ITEXT)
	t = widget;
      if(t)
	{
	  EZ_ITextSetRegionBackground(t, color_name);
	}
    }
}
/*****************************************************************************/
void EZ_TextSetRegionProperty(widget, tprop)
     EZ_Widget *widget;
     TextProp  *tprop;
{
  EZ_Widget *t = NULL;
  if(widget)
    {
      if(EZ_WidgetType(widget) == EZ_WIDGET_TEXT)
	t = EZ_TextIText(widget);
      else if(EZ_WidgetType(widget) == EZ_WIDGET_ITEXT)
	t = widget;
      if(t)
	{
	  EZ_ITextSetRegionProperty(t, tprop);
	}
    }
}
/*****************************************************************************/

void  EZ_TextReplaceRegionWithPixmap(widget, file)
     EZ_Widget *widget;
     char      *file;
{
  EZ_Widget *t = NULL;
  if(widget)
    {
      if(EZ_WidgetType(widget) == EZ_WIDGET_TEXT)
	t = EZ_TextIText(widget);
      else if(EZ_WidgetType(widget) == EZ_WIDGET_ITEXT)
	t = widget;
      if(t)
	{
	  EZ_ITextReplaceRegionWithBitmap(t, file);
	}
    }
}
/*****************************************************************************/
int  EZ_TextSaveText(widget, fname)
     EZ_Widget *widget;
     char      *fname;
{
  EZ_Widget *t = NULL;
  if(widget)
    {
      if(EZ_WidgetType(widget) == EZ_WIDGET_TEXT)
	t = EZ_TextIText(widget);
      else if(EZ_WidgetType(widget) == EZ_WIDGET_ITEXT)
	t = widget;
      if(t)
	{
	  return(EZ_ITextSaveText(t, fname));
	}
    }
  return(0);
}
/*****************************************************************************/
int EZ_TextSaveAnnotatedText(widget,fname)
     EZ_Widget *widget;
     char      *fname;
{
  EZ_Widget *t = NULL;
  if(widget)
    {
      if(EZ_WidgetType(widget) == EZ_WIDGET_TEXT)
	t = EZ_TextIText(widget);
      else if(EZ_WidgetType(widget) == EZ_WIDGET_ITEXT)
	t = widget;
      if(t)
	{
	  return(EZ_ITextSaveAnnotatedText(t,fname));
	}
    }
  return(0);
}
/*****************************************************************************/

int  EZ_TextGetPosition(widget)
     EZ_Widget *widget;
{
  EZ_Widget *t = NULL;
  if(widget)
    {
      if(EZ_WidgetType(widget) == EZ_WIDGET_TEXT)
	t = EZ_TextIText(widget);
      else if(EZ_WidgetType(widget) == EZ_WIDGET_ITEXT)
	t = widget;
      if(t)
	{
	  return(EZ_ITextGetPosition(t));
	}
    }
  return(0);
}

void EZ_TextGotoPosition(widget, pos)
     EZ_Widget *widget; int pos;
{
  EZ_Widget *t = NULL;
  if(widget)
    {
      if(EZ_WidgetType(widget) == EZ_WIDGET_TEXT)
	t = EZ_TextIText(widget);
      else if(EZ_WidgetType(widget) == EZ_WIDGET_ITEXT)
	t = widget;
      if(t)
	{
	  EZ_ITextGotoPosition(t, pos);
	}
    }
}

char *EZ_TextGetRegionString(widget)
     EZ_Widget *widget; 
{
  EZ_Widget *t = NULL;
  if(widget)
    {
      if(EZ_WidgetType(widget) == EZ_WIDGET_TEXT)
	t = EZ_TextIText(widget);
      else if(EZ_WidgetType(widget) == EZ_WIDGET_ITEXT)
	t = widget;
      if(t)
	{
	  return ( EZ_ITextGetRegionString(t));
	}
    }
  return(NULL);
}
char *EZ_TextGetBufferString(widget)
     EZ_Widget *widget; 
{
  EZ_Widget *t = NULL;
  if(widget)
    {
      if(EZ_WidgetType(widget) == EZ_WIDGET_TEXT)
	t = EZ_TextIText(widget);
      else if(EZ_WidgetType(widget) == EZ_WIDGET_ITEXT)
	t = widget;
      if(t)
	{
	  return ( EZ_ITextGetBufferString(t));
	}
    }
  return(NULL);
}
/***********************************************************/
int EZ_TextBufferModified(widget) EZ_Widget *widget;
{
  EZ_Widget *t = NULL;
  if(widget)
    {
      if(EZ_WidgetType(widget) == EZ_WIDGET_TEXT) t = EZ_TextIText(widget);
      else if(EZ_WidgetType(widget) == EZ_WIDGET_ITEXT)	t = widget;
      if(t) return ( (int) EZ_ITextModified(t));
    }
  return(0);
}
/***********************************************************/
void  EZ_GetTextWidgetComponents(wptr, it_ret, h_ret, v_ret)
     EZ_Widget *wptr, **it_ret, **h_ret, **v_ret;
{
  if(wptr)
    {
      if( EZ_WidgetType(wptr) == EZ_WIDGET_TEXT)
        {
          if(it_ret) *it_ret = EZ_TextIText(wptr);
          if(h_ret) *h_ret = EZ_TextHScroll(wptr);
          if(v_ret) *v_ret = EZ_TextVScroll(wptr);
        }
      else if(EZ_WidgetType(wptr) == EZ_WIDGET_ITEXT)
        {
          if(it_ret) *it_ret = wptr;
          if(h_ret) *h_ret = EZ_ITextHScroll(wptr);
          if(v_ret) *v_ret = EZ_ITextVScroll(wptr);
        }
    }
}
int EZ_GetTextSpace(wptr) EZ_Widget *wptr;
{
  if(wptr)
    {
      int type = EZ_WidgetType(wptr);
      if(type == EZ_WIDGET_TEXT)
        return(EZ_ITextVSpacing(EZ_TextIText(wptr)));
      else if(type == EZ_WIDGET_ITEXT)
        return(EZ_ITextVSpacing(wptr));
    }
  return(0);
}
/***********************************************************/
#undef _EZ_WIDGET_TEXT_C_
