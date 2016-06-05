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
 ***              Widget List Box                              ***
 ***                                                           ***
 *****************************************************************/
#define _EZ_WIDGET_LIST_BOX_C_
#include "EZ_Widget.h"

/*********************************************************************
 * 
 *  Functions implemented in this file:
 */
EZ_Widget        *EZ_CreateListBox MY_ANSIARGS((EZ_Widget *parent, int hsc, int vsc));
void             EZ_DrawWidgetListBox MY_ANSIARGS((EZ_Widget *widget));
void             EZ_ComputeWidgetListBoxSize MY_ANSIARGS((EZ_Widget *widget, int *w, int *h));

void             EZ_ClearListBox MY_ANSIARGS((EZ_Widget *widget));
void             EZ_SetListBoxItems MY_ANSIARGS((EZ_Widget *widget, char **items, int nitems));
void             EZ_AppendListBoxItem MY_ANSIARGS((EZ_Widget *widget, char *item));
void             EZ_InsertListBoxItem MY_ANSIARGS((EZ_Widget *widget, char *str, int idx));
void             EZ_DeleteListBoxItem MY_ANSIARGS((EZ_Widget *widget, int idx));
void             EZ_DeleteListBoxString MY_ANSIARGS((EZ_Widget *widget, char *str));
void             EZ_GetListBoxData MY_ANSIARGS((EZ_Widget *widget, listboxentry **dat, int *cnt));
char             *EZ_GetListBoxSelectedItem MY_ANSIARGS((EZ_Widget *widget));
char             *EZ_GetListBoxSelection MY_ANSIARGS((EZ_Widget *widget));
int              EZ_GetListBoxSelectionLineNumber MY_ANSIARGS((EZ_Widget *widget));
void             EZ_ModifyListBoxItem MY_ANSIARGS((EZ_Widget *widget, char *item, int idx));
void             EZ_ListBoxSelectItem MY_ANSIARGS((EZ_Widget *widget, char *str));
void             EZ_ListBoxSelectItemUsingIdx MY_ANSIARGS((EZ_Widget *widget, int idx));
void             EZ_GetListBoxState  MY_ANSIARGS((EZ_Widget *widget, int *xoff, int *fl)); 
void             EZ_SetListBoxState  MY_ANSIARGS((EZ_Widget *widget, int xoff, int fl)); 
void             EZ_GetListBoxWidgetComponents MY_ANSIARGS((EZ_Widget *widget, EZ_Widget **ilb, 
						      EZ_Widget **hsc,  EZ_Widget **vsc));

void             EZ_GetListBoxVState MY_ANSIARGS((EZ_Widget *widget, int *total, int *page, int *first));
void             EZ_GetListBoxHState MY_ANSIARGS((EZ_Widget *widget, int *total, int *page, int *first));
void             EZ_SetListBoxVState MY_ANSIARGS((EZ_Widget *widget, int begin));
void             EZ_SetListBoxHState MY_ANSIARGS((EZ_Widget *widget, int begin));
void             EZ_SetListBoxHScrollbar MY_ANSIARGS((EZ_Widget *widget, EZ_Widget *scr));
void             EZ_SetListBoxVScrollbar MY_ANSIARGS((EZ_Widget *widget, EZ_Widget *scr));

void             EZ_SetListBoxRowBg MY_ANSIARGS((EZ_Widget *widget, int mode,
                                                 char *bg1, char *bg2));

void          EZ_InsertListBoxColorItem MY_ANSIARGS((EZ_Widget *widget, char *item, 
                                                         unsigned long color, int where));
void          EZ_ColorListBoxItemUsingIdx MY_ANSIARGS((EZ_Widget *widget, int idx ,
                                                        unsigned long clr));
void          EZ_ColorListBoxItem MY_ANSIARGS((EZ_Widget *widget, char *item,
                                                unsigned long clr));

/*********************************************************************
 * 
 *  Local Variables.
 */
static EZ_WidgetHandle EZ_ListBoxHandle =
{
  EZ_ComputeWidgetListBoxSize,
  EZ_DrawWidgetListBox,
  EZ_FreeUnknownData,
  EZ_UnkownEventHandle,
};

/*********************************************************************/
EZ_Widget  *EZ_CreateListBox(parent, hscrollbar, vscrollbar)
     EZ_Widget  *parent;     /* parent widget    */
     int hscrollbar, vscrollbar;
{
  EZ_Widget  *wptr, *ilistbox, *hscroll=NULL, *vscroll=NULL;

  wptr = EZ_CreateNewWidget(parent);
  /*--------------------------------------------------
   * Register the handling functions for ...
   *  has to be done after  EZ_CreateNewWidget.
   *-------------------------------------------------*/
  EZ_WidgetHandlingFunctions[EZ_WIDGET_LIST_BOX] = &EZ_ListBoxHandle;

  EZ_SetWidgetTypeAndNames(wptr,EZ_WIDGET_LIST_BOX);
  EZ_WidgetStacking(wptr) = EZ_VERTICAL;
  EZ_WidgetBorderStyle(wptr)  = EZ_BORDER_NONE;
  EZ_WidgetBorderWidth(wptr)  = 0;
  EZ_WidgetFillMode(wptr) = EZ_FILL_VERTICALLY;

  EZ_WidgetPadX(wptr) = 0;
  EZ_WidgetPadY(wptr) = 0;
  EZ_WidgetSepX(wptr)= 0;
  EZ_WidgetSepY(wptr)= 0;

  EZ_SetRowColConstrains(wptr, EZ_ROW,    0, 64, 1000, 0);
  EZ_SetRowColConstrains(wptr, EZ_ROW,    1, 0,  0,    0);
  EZ_SetRowColConstrains(wptr, EZ_COLUMN, 0, 64, 1000, 0);
  EZ_SetRowColConstrains(wptr, EZ_COLUMN, 1, 0,  0,    0);

  ilistbox = EZ_ListBoxIListBox(wptr) = EZ_CreateIListBox(wptr);
  EZ_WidgetGroupLeader(ilistbox) = wptr;
  EZ_WidgetGBX(ilistbox) = 0;
  EZ_WidgetGBY(ilistbox) = 0;
  EZ_WidgetGBW(ilistbox) = 1;
  EZ_WidgetGBH(ilistbox) = 1;
  EZ_SetWidgetExpandFlag(ilistbox);
  if(vscrollbar)
    {
      vscroll = EZ_ListBoxVScroll(wptr) = 
	EZ_CreateScrollbar(wptr,EZ_WIDGET_VERTICAL_SCROLLBAR);
      EZ_SetWidgetExpandFlag(vscroll);
      EZ_ScrollObject(vscroll) = ilistbox;
      EZ_WidgetGroupLeader(vscroll) = wptr;
      EZ_WidgetGBX(vscroll) = 1;
      EZ_WidgetGBY(vscroll) = 0;
      EZ_WidgetGBW(vscroll) = 1;
      EZ_WidgetGBH(vscroll) = 1;
    }
  if(hscrollbar)
    {
      hscroll = EZ_ListBoxHScroll(wptr) = 
	EZ_CreateScrollbar(wptr,EZ_WIDGET_HORIZONTAL_SCROLLBAR);
      EZ_SetWidgetExpandFlag(hscroll);
      EZ_ScrollObject(hscroll) = ilistbox;
      EZ_WidgetGroupLeader(hscroll) = wptr;
      EZ_WidgetGBX(hscroll) = 0;
      EZ_WidgetGBY(hscroll) = 1;
      EZ_WidgetGBW(hscroll) = 1;
      EZ_WidgetGBH(hscroll) = 1;
    }
  EZ_IListBoxHScroll(ilistbox) = hscroll;
  EZ_IListBoxVScroll(ilistbox) = vscroll;  
  EZ_IListBoxListBox(ilistbox) = wptr;   

  return(wptr);
}

/********************************************************************
 *
 *  Figure out the dimension of an internal list box.
 */
void EZ_ComputeWidgetListBoxSize(widget, w, h)
     EZ_Widget *widget;
     int             *w, *h;
{
  int cw, ch;

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
 *  Draw a ListBox widget
 */

void  EZ_DrawWidgetListBox(wptr)
     EZ_Widget *wptr;
{
  if( EZ_WidgetNumChildren(wptr)> 2 ||
     (EZ_ListBoxHScroll(wptr) == NULL &&  EZ_WidgetNumChildren(wptr) > 1))
    {
      int             w, h;
      Pixmap          pixmap;
      Window          win;
      GC              gc;
      unsigned long   bgpv;

      win = EZ_WidgetWindow(wptr);
      w   = EZ_WidgetWidth(wptr);
      h   = EZ_WidgetHeight(wptr);
  
      /*-----------------------------------------------------------
       *  Create a pixmap, draw into this pixmap in background and
       *  copy to the button window when finished.
       *----------------------------------------------------------*/
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

/********************************************************************************/
void  EZ_ClearListBox(widget)
     EZ_Widget *widget;
{
  if(widget != (EZ_Widget *)NULL)
    {
      int type = EZ_WidgetType(widget);
      EZ_Widget *t = NULL;
      if(type == EZ_WIDGET_LIST_BOX)
	t = EZ_ListBoxIListBox(widget);
      else if(type  == EZ_WIDGET_ILIST_BOX)
	t = widget;
      if(t) EZ_ClearIListBox(t);
    }
}
/********************************************************************************/
void  EZ_SetListBoxItems(widget, items, nitems)
     EZ_Widget *widget;
     char      **items;
     int       nitems;
{
  if(widget != (EZ_Widget *)NULL)
    {
      int type = EZ_WidgetType(widget);
      EZ_Widget *t = NULL;
      if(type  == EZ_WIDGET_LIST_BOX)
	t = EZ_ListBoxIListBox(widget);
      else if(type == EZ_WIDGET_ILIST_BOX)
	t = widget;
      if(t) EZ_SetIListBoxData(t, items, nitems);
    }
}
/********************************************************************************/
void  EZ_AppendListBoxItem(widget, str)
     EZ_Widget *widget;
     char      *str;     
{
  if(widget != (EZ_Widget *)NULL)
    {
      int type = EZ_WidgetType(widget);
      EZ_Widget *t= NULL;
      if(type == EZ_WIDGET_LIST_BOX)
	t = EZ_ListBoxIListBox(widget);
      else if(type == EZ_WIDGET_ILIST_BOX)
	t = widget;
      if(t) EZ_InsertIListBoxItem(t, str, -1);
    }
}
void  EZ_InsertListBoxItem(widget, str,where)
     EZ_Widget *widget;
     char      *str;     
     int where;
{
  EZ_InsertListBoxColorItem(widget, str, ~0, where);
}

void  EZ_InsertListBoxColorItem(widget, str, clr, where)
     EZ_Widget *widget;
     char      *str;     
     int where;
     unsigned long clr;
{
  if(widget != (EZ_Widget *)NULL)
    {
      int type = EZ_WidgetType(widget);
      EZ_Widget *t= NULL;
      if(type == EZ_WIDGET_LIST_BOX)
	t = EZ_ListBoxIListBox(widget);
      else if(type == EZ_WIDGET_ILIST_BOX)
	t = widget;
      if(t) EZ_InsertIListBoxColorItem(t, str, clr, where);
    }
}
/********************************************************************************/
extern char *EZ_GetIListBoxSelectedItem();
extern void  EZ_GetIListBoxData();

char *EZ_GetListBoxSelectedItem(widget)
     EZ_Widget *widget;
{
  if(widget != (EZ_Widget *)NULL)
    {
      int type = EZ_WidgetType(widget);
      EZ_Widget *t = NULL;
      if(type == EZ_WIDGET_LIST_BOX)
	t = EZ_ListBoxIListBox(widget);
      else if(type == EZ_WIDGET_ILIST_BOX)
	t = widget;
      if(t) return(EZ_GetIListBoxSelectedItem(t));
    }
  return( (char *)NULL);
}
extern char *EZ_GetIListBoxSelection MY_ANSIARGS((EZ_Widget *widget));

char *EZ_GetListBoxSelection(widget)
     EZ_Widget *widget;
{
  if(widget != (EZ_Widget *)NULL)
    {
      int type = EZ_WidgetType(widget);
      EZ_Widget *t = NULL;
      if(type == EZ_WIDGET_LIST_BOX)
	t = EZ_ListBoxIListBox(widget);
      else if(type == EZ_WIDGET_ILIST_BOX)
	t = widget;
      return(EZ_GetIListBoxSelection(t));
    }
  return(NULL);
}


void EZ_GetListBoxData(widget, data_ret, count_ret)
     EZ_Widget *widget;
     listboxentry **data_ret;
     int *count_ret;
{
  if(widget != (EZ_Widget *)NULL)
    {
      int type = EZ_WidgetType(widget);
      EZ_Widget *t = NULL;
      if(type == EZ_WIDGET_LIST_BOX)
	t = EZ_ListBoxIListBox(widget);
      else if(type == EZ_WIDGET_ILIST_BOX)
	t = widget;
      EZ_GetIListBoxData(t, data_ret, count_ret);
      return;
    }  
  *data_ret = NULL;
  *count_ret = 0;
}  
/********************************************************************************/
void  EZ_SetListBoxHScrollbar(widget, scroll)
     EZ_Widget *widget, *scroll;
{
  if(widget && scroll && EZ_WidgetType(scroll) == EZ_WIDGET_HORIZONTAL_SCROLLBAR)
    {
      int type = EZ_WidgetType(widget);
      EZ_Widget *t = NULL;
      if(type == EZ_WIDGET_LIST_BOX)
	{
	  t = EZ_ListBoxIListBox(widget);
	  EZ_ListBoxHScroll(widget) =
	    EZ_IListBoxHScroll(t) = scroll;
	}
      else if(type == EZ_WIDGET_ILIST_BOX)
	{
	  EZ_IListBoxHScroll(widget) = scroll;
	}
    }
}
/********************************************************************************/
void  EZ_SetListBoxVScrollbar(widget, scroll)
     EZ_Widget *widget, *scroll;
{
  if(widget && scroll && EZ_WidgetType(scroll) == EZ_WIDGET_VERTICAL_SCROLLBAR)
    {
      int type = EZ_WidgetType(widget); 
      if(type  == EZ_WIDGET_LIST_BOX)
	{
	  EZ_ListBoxVScroll(widget) =
	    EZ_IListBoxVScroll(EZ_ListBoxIListBox(widget)) = scroll;
	}
      else if(type  == EZ_WIDGET_LIST_BOX)
	{
	  EZ_IListBoxVScroll(widget) = scroll;
	}
    }
}
/********************************************************************************/
void EZ_GetListBoxVState(widget, total, page, first)
     EZ_Widget *widget;
     int *total, *page, *first;
{
  if(widget != (EZ_Widget *)NULL)
    {
      int type = EZ_WidgetType(widget);
      EZ_Widget *t= NULL;
      if(type == EZ_WIDGET_LIST_BOX)
	t = EZ_ListBoxIListBox(widget);
      else if(type == EZ_WIDGET_ILIST_BOX)
	t = widget;
      if(t)
	{
	  *total = EZ_IListBoxNLines(t);
	  *page = EZ_IListBoxPageSize(t);
	  *first = EZ_IListBoxFirstLine(t);
	}
    }
}
/********************************************************************************/
void EZ_GetListBoxHState(widget, total, page, first)
     EZ_Widget *widget;
     int *total, *page, *first;
{
  if(widget != (EZ_Widget *)NULL)
    {
      int type = EZ_WidgetType(widget);
      EZ_Widget *t= NULL;
      if(type == EZ_WIDGET_LIST_BOX)
	t = EZ_ListBoxIListBox(widget);
      else if(type == EZ_WIDGET_ILIST_BOX)
	t = widget;
      if(t)
	{
	  *total = EZ_IListBoxMaxHSize(t);
	  *page = EZ_IListBoxHPageSize(t);
	  *first = EZ_IListBoxHStart(t);
	}
    }
}

/********************************************************************************/
void EZ_SetListBoxVState(widget,first)
     EZ_Widget *widget;
     int  first;
{
  if(widget != (EZ_Widget *)NULL)
    {
      int type = EZ_WidgetType(widget);
      EZ_Widget *t= NULL;
      if(type == EZ_WIDGET_LIST_BOX)
	t = EZ_ListBoxIListBox(widget);
      else if(type == EZ_WIDGET_ILIST_BOX)
	t = widget;
      if(t)
	{
	  EZ_IListBoxFirstLine(t) = first;
	}
    }
}
/********************************************************************************/
void EZ_SetListBoxHState(widget,first)
     EZ_Widget *widget;
     int first;
{
  if(widget != (EZ_Widget *)NULL)
    {
      int type = EZ_WidgetType(widget);
      EZ_Widget *t= NULL;
      if(type == EZ_WIDGET_LIST_BOX)
	t = EZ_ListBoxIListBox(widget);
      else if(type == EZ_WIDGET_ILIST_BOX)
	t = widget;
      if(t)
	{
	  EZ_IListBoxHStart(t) = first;
	}
    }
}
/********************************************************************************/
void  EZ_GetListBoxWidgetComponents(widget, ilbx_ret, h_ret, v_ret)
     EZ_Widget *widget;
     EZ_Widget **ilbx_ret, **h_ret, **v_ret;
{
  if(widget)
    {
      if( EZ_WidgetType(widget) == EZ_WIDGET_LIST_BOX)
        {
          if(ilbx_ret) *ilbx_ret = EZ_ListBoxIListBox(widget);
          if(h_ret) *h_ret = EZ_ListBoxHScroll(widget);
          if(v_ret) *v_ret = EZ_ListBoxVScroll(widget);
        }
      else if( EZ_WidgetType(widget) == EZ_WIDGET_ILIST_BOX)
        {
          if(ilbx_ret) *ilbx_ret = widget;
          if(h_ret) *h_ret = EZ_IListBoxHScroll(widget);
          if(v_ret) *v_ret = EZ_IListBoxVScroll(widget);
        }
    }
}
/********************************************************************************/
void  EZ_DeleteListBoxItem(widget, idx)
     EZ_Widget *widget; int idx;
{
  if(widget)
    {
      EZ_Widget *t = NULL;
      switch( EZ_WidgetType(widget))
	{
	case EZ_WIDGET_LIST_BOX:
	  t = EZ_ListBoxIListBox(widget);
	  break;
	case EZ_WIDGET_ILIST_BOX:
	  t = widget;
	  break;
	default:
	  break;
	}
      if(t) EZ_DeleteIListBoxItem(t, idx);
    }
}

void  EZ_DeleteListBoxString(widget, str)
     EZ_Widget *widget; char *str;
{
  if(widget)
    {
      EZ_Widget *t = NULL;
      switch( EZ_WidgetType(widget))
	{
	case EZ_WIDGET_LIST_BOX:
	  t = EZ_ListBoxIListBox(widget);
	  break;
	case EZ_WIDGET_ILIST_BOX:
	  t = widget;
	  break;
	default:
	  break;
	}
      if(t) EZ_DeleteIListBoxString(t, str);
    }
}
/**************************************************************************/
int  EZ_GetListBoxSelectionLineNumber(widget)
     EZ_Widget *widget;
{
  if(widget)
    {
      EZ_Widget *t = NULL;
      switch( EZ_WidgetType(widget))
	{
	case EZ_WIDGET_LIST_BOX:
	  t = EZ_ListBoxIListBox(widget);
	  break;
	case EZ_WIDGET_ILIST_BOX:
	  t = widget;
	  break;
	default:
	  break;
	}
      if(t) return(EZ_IListBoxSLine(t));
    }  
  return(-1);
}
/**************************************************************************/

/* contributed by  Antti H{yrynen <hydra@pato.vaala.fi> */

void EZ_ModifyListBoxItem(widget, newitem, itemn)
     EZ_Widget *widget;
     int itemn;
     char *newitem;
{
  if (widget != (EZ_Widget *)NULL && itemn >= 0)
    {
      int type = EZ_WidgetType(widget);
      EZ_Widget *t = NULL;
      if (type == EZ_WIDGET_LIST_BOX)
        t = EZ_ListBoxIListBox(widget);
      else if(type == EZ_WIDGET_ILIST_BOX)
        t = widget;
      if (t) EZ_ModifyIListBoxItem(t, newitem, itemn);
    }
}

void EZ_ListBoxSelectItem(widget, str)
     EZ_Widget *widget; char *str;
{
  if(widget)
    {
      EZ_Widget *t = NULL;
      switch( EZ_WidgetType(widget))
	{
	case EZ_WIDGET_LIST_BOX:
	  t = EZ_ListBoxIListBox(widget);
	  break;
	case EZ_WIDGET_ILIST_BOX:
	  t = widget;
	  break;
	default:
	  break;
	}
      if(t) EZ_IListBoxSelectItem(t, str);
    }    
}


void EZ_ListBoxSelectItemUsingIdx(widget, idx)
     EZ_Widget *widget; int idx;
{
  if(widget)
    {
      EZ_Widget *t = NULL;
      switch( EZ_WidgetType(widget))
	{
	case EZ_WIDGET_LIST_BOX:
	  t = EZ_ListBoxIListBox(widget);
	  break;
	case EZ_WIDGET_ILIST_BOX:
	  t = widget;
	  break;
	default:
	  break;
	}
      if(t) EZ_IListBoxSelectItemUsingIdx(t, idx);
    }  
}

void EZ_GetListBoxState(widget, xoff, fl)
     EZ_Widget *widget; int *xoff; int *fl;
{
  if(widget)
    {
      EZ_Widget *t = NULL;
      switch( EZ_WidgetType(widget))
	{
	case EZ_WIDGET_LIST_BOX:
	  t = EZ_ListBoxIListBox(widget);
	  break;
	case EZ_WIDGET_ILIST_BOX:
	  t = widget;
	  break;
	default:
	  break;
	}
      if(t)
	{
	  if(xoff) *xoff = EZ_IListBoxHStart(t);
	  if(fl)   *fl = EZ_IListBoxFirstLine(t);
	}
    }  
}

void EZ_SetListBoxState(widget, xoff, fl)
     EZ_Widget *widget; int xoff; int fl;
{
  if(widget)
    {
      EZ_Widget *t = NULL;
      switch( EZ_WidgetType(widget))
	{
	case EZ_WIDGET_LIST_BOX:
	  t = EZ_ListBoxIListBox(widget);
	  break;
	case EZ_WIDGET_ILIST_BOX:
	  t = widget;
	  break;
	default:
	  break;
	}
      if(t)
	{
	  EZ_IListBoxHStart(t) = xoff;
	  EZ_IListBoxFirstLine(t) = fl;
	  if(EZ_WidgetMapped(widget)) EZ_DisplayWidget(widget);
	}
    }  
}
/*****************************************************************************/
void EZ_SetListBoxRowBg(widget, mode, bg1, bg2)
     EZ_Widget *widget; char *bg1, *bg2; int mode;
{
  if(widget)
    {
      EZ_Widget *twidget = NULL;
      switch( EZ_WidgetType(widget))
	{
	case EZ_WIDGET_LIST_BOX:
	  twidget = EZ_ListBoxIListBox(widget);
	  break;
	case EZ_WIDGET_ILIST_BOX:
	  twidget = widget;
	  break;
	default:
	  break;
	}
      if(twidget)
	{
	  unsigned long *bg = EZ_IListBoxRowBG(twidget);
	  EZ_IListBoxRowBGSet(twidget) = mode;
	  if(bg1) bg[0]= EZ_AllocateColorFromName(bg1);
	  else  bg[0] = 0;
	  if(bg2) bg[1]= EZ_AllocateColorFromName(bg2);
	  else bg[1] = 0;
	}
    }
}
/*****************************************************************************/
void  EZ_ColorListBoxItemUsingIdx(widget, idx, clr)
     EZ_Widget *widget; int idx; unsigned long clr;
{
  if(widget)
    {
      EZ_Widget *twidget = NULL;
      switch( EZ_WidgetType(widget))
	{
	case EZ_WIDGET_LIST_BOX:
	  twidget = EZ_ListBoxIListBox(widget);
	  break;
	case EZ_WIDGET_ILIST_BOX:
	  twidget = widget;
	  break;
	default:
	  break;
	}
      if(twidget)  EZ_ColorIListBoxItemUsingIdx(twidget, idx, clr);
    }
}

void  EZ_ColorListBoxItem(widget, str, clr)
     EZ_Widget *widget; char *str; unsigned long clr;
{
  if(widget)
    {
      EZ_Widget *twidget = NULL;
      switch( EZ_WidgetType(widget))
	{
	case EZ_WIDGET_LIST_BOX:
	  twidget = EZ_ListBoxIListBox(widget);
	  break;
	case EZ_WIDGET_ILIST_BOX:
	  twidget = widget;
	  break;
	default:
	  break;
	}
      if(twidget)  EZ_ColorIListBoxItem(twidget, str, clr);
    }
}
/*****************************************************************************/
#undef _EZ_WIDGET_LIST_BOX_C_


