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
 ***              Widget FancyListBox                          ***
 ***                                                           ***
 *****************************************************************/
#define _EZ_WIDGET_FLIST_BOX_C_
#include "EZ_Widget.h"

/*********************************************************************
 * 
 *  Functions implemented in this file:
 */
EZ_Widget  *EZ_CreateFancyListBox MY_ANSIARGS((EZ_Widget *parent,
					       int hsc, int vsc, int ncol, int selctype));
void       EZ_DrawWidgetFancyListBox MY_ANSIARGS((EZ_Widget *widget));
void       EZ_GetFancyListBoxWidgetComponents MY_ANSIARGS((EZ_Widget *widget,
							   EZ_Widget **ifl,
							   EZ_Widget **hsc,
							   EZ_Widget **vsc));

EZ_Item    **EZ_GetFancyListBoxSelection MY_ANSIARGS((EZ_Widget *widget));
EZ_Item    *EZ_GetFancyListBoxItemUsingIdx  MY_ANSIARGS((EZ_Widget *widget, int row, int col));
void       EZ_GetFancyListBoxSelectionIdx  MY_ANSIARGS((EZ_Widget *widget, int *row, int *col));
void       EZ_SetFancyListBoxData  MY_ANSIARGS((EZ_Widget *widget, EZ_Item **data, int nrow, int ncol));
void       EZ_FancyListBoxClear  MY_ANSIARGS((EZ_Widget *widget));
void       EZ_GetFancyListBoxXYOffset  MY_ANSIARGS((EZ_Widget *widget, int *xoff, int *yoff));
void       EZ_SetFancyListBoxXYOffset  MY_ANSIARGS((EZ_Widget *widget, int xoff, int yoff));
void       EZ_SetFancyListBoxHeader  MY_ANSIARGS((EZ_Widget *widget, EZ_Item **header, int nelts));

void       EZ_FancyListBoxInsertRow  MY_ANSIARGS((EZ_Widget *widget, EZ_Item **items, int nitems, int where));
void       EZ_FancyListBoxDeleteRow  MY_ANSIARGS((EZ_Widget *widget, int idx));
void       EZ_FancyListBoxDeleteSomeRows MY_ANSIARGS((EZ_Widget *widget, int *idces, int cnt));
void       EZ_FancyListBoxReplaceItem  MY_ANSIARGS((EZ_Widget *widget, EZ_Item *old, EZ_Item *newItem));
void       EZ_FancyListBoxReplaceItemUsingIdx  MY_ANSIARGS((EZ_Widget *widget, int row, int col, EZ_Item *newItem));
void       EZ_FancyListBoxSelectItemUsingIdx MY_ANSIARGS((EZ_Widget *widget, int Srow, int Scol, int *location));
void       EZ_FancyListBoxSelectItem MY_ANSIARGS((EZ_Widget *widget, EZ_Item *item, int *location));
int        EZ_GetFancyListBoxNumberOfRows  MY_ANSIARGS((EZ_Widget *widget));
int        EZ_GetFancyListBoxNumberOfColumns  MY_ANSIARGS((EZ_Widget *widget));
EZ_Item    ***EZ_GetFancyListBoxData  MY_ANSIARGS((EZ_Widget *widget));

EZ_Item    **EZ_FancyListBoxFindRowAtXY MY_ANSIARGS((EZ_Widget *fl, int x, int y, int *r_ret));
EZ_Item    *EZ_FancyListBoxFindItemAtXY MY_ANSIARGS((EZ_Widget *fl, int x, int y, int *r_ret, int *c_ret));

int              *EZ_FancyListBoxGetHighlightedRowsA MY_ANSIARGS((EZ_Widget *widget, int *cnt));
int              EZ_FancyListBoxGetHighlightedRows MY_ANSIARGS((EZ_Widget *widget, int *ary, int siz));
EZ_Item          **EZ_FancyListBoxGetHighlightedItemsA MY_ANSIARGS((EZ_Widget *widget, int *cnt));
int              EZ_FancyListBoxGetHighlightedItems MY_ANSIARGS((EZ_Widget *widget, EZ_Item **arg,int siz));
int              EZ_FancyListBoxGetHithlightedItemsAndIndices MY_ANSIARGS((EZ_Widget *wgt, EZ_Item ***itm_ret,
									int **rowIdx_ret, int **colIdx_ret));

void    EZ_SetFancyListBoxRowBg MY_ANSIARGS((EZ_Widget *widget, int mode, char *bg1, char *bg2));

/*********************************************************************
 * 
 *  Local Variables.
 */
static EZ_WidgetHandle EZ_FancyListBoxHandle =
{ 
  EZ_ComputeWidgetUnknownSize,
  EZ_DrawWidgetFancyListBox,
  EZ_FreeUnknownData,
  EZ_UnkownEventHandle,
};

/*********************************************************************/
EZ_Widget  *EZ_CreateFancyListBox(parent, hscrollbar, vscrollbar, ncolumns, stype)
     EZ_Widget  *parent;     /* parent widget    */
     int hscrollbar, vscrollbar;
     int ncolumns, stype;
{
  EZ_Widget  *wptr, *iflistbox, *hscroll=NULL, *vscroll=NULL;

  wptr = EZ_CreateNewWidget(parent);
  EZ_WidgetHandlingFunctions[EZ_WIDGET_FANCY_LIST_BOX] = &EZ_FancyListBoxHandle;
  EZ_SetWidgetTypeAndNames(wptr,EZ_WIDGET_FANCY_LIST_BOX);

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

  iflistbox = EZ_FListBoxIFListBox(wptr) = 
    EZ_CreateIFListBox(wptr, ncolumns, stype);
  EZ_WidgetGroupLeader(iflistbox) = wptr;
  EZ_WidgetGBX(iflistbox) = 0;
  EZ_WidgetGBY(iflistbox) = 0;
  EZ_WidgetGBW(iflistbox) = 1;
  EZ_WidgetGBH(iflistbox) = 1;

  EZ_SetWidgetExpandFlag(iflistbox);

  if(vscrollbar)
    {
      vscroll = EZ_FListBoxVScroll(wptr) = EZ_CreateScrollbar(wptr,EZ_WIDGET_VERTICAL_SCROLLBAR);
      EZ_SetWidgetExpandFlag(vscroll);
      EZ_ScrollObject(vscroll) = iflistbox;
      EZ_WidgetGroupLeader(vscroll) = wptr;
      EZ_WidgetGBX(vscroll) = 1;
      EZ_WidgetGBY(vscroll) = 0;
      EZ_WidgetGBW(vscroll) = 1;
      EZ_WidgetGBH(vscroll) = 1;
    }
  if(hscrollbar)
    {
      hscroll = EZ_FListBoxHScroll(wptr) = EZ_CreateScrollbar(wptr,EZ_WIDGET_HORIZONTAL_SCROLLBAR);
      EZ_SetWidgetExpandFlag(hscroll);
      EZ_ScrollObject(hscroll) = iflistbox;
      EZ_WidgetGroupLeader(hscroll) = wptr;
      EZ_WidgetGBX(hscroll) = 0;
      EZ_WidgetGBY(hscroll) = 1;
      EZ_WidgetGBW(hscroll) = 1;
      EZ_WidgetGBH(hscroll) = 1;
    }
  EZ_IFListBoxHScroll(iflistbox) = hscroll;
  EZ_IFListBoxVScroll(iflistbox) = vscroll;  
  EZ_IFListBoxFListBox(iflistbox) = wptr;   

  return(wptr);
}

/*******************************************************************
 *
 *  Draw A Label Widget.
 */
void  EZ_DrawWidgetFancyListBox(wptr)
     EZ_Widget *wptr;
{
  if( EZ_WidgetNumChildren(wptr)> 2 ||
      (EZ_FListBoxHScroll(wptr) == NULL &&  EZ_WidgetNumChildren(wptr) > 1))
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
      pixmap = XCreatePixmap(EZ_Display, win, w, h, EZ_Depth);    
      if(EZ_GetBackgroundGC(wptr, &gc, &bgpv, 0, 0))
	{
	  int ox, oy;
	  EZ_GetWidgetTileOrigin(wptr, &ox, &oy);      
	  XSetTSOrigin(EZ_Display, gc, ox, oy);
	}
      XFillRectangle(EZ_Display, pixmap, gc, 0,0, w, h); 
      
      EZ_DrawRectBorder(wptr, pixmap);     
      XCopyArea(EZ_Display,pixmap,win, EZ_WRITABLEGC,0,0,w,h,0,0); 
      XFreePixmap(EZ_Display, pixmap); 
    }
}

/************************************************************************************/
void  EZ_GetFancyListBoxWidgetComponents(widget, iflistbox_ret, h_ret, v_ret)
     EZ_Widget *widget;
     EZ_Widget **iflistbox_ret, **h_ret, **v_ret;
{
  if(widget)
    {
      if( EZ_WidgetType(widget) == EZ_WIDGET_FANCY_LIST_BOX)
        {
          if(iflistbox_ret) *iflistbox_ret = EZ_FListBoxIFListBox(widget);
          if(h_ret)*h_ret = EZ_FListBoxHScroll(widget);
          if(v_ret) *v_ret = EZ_FListBoxVScroll(widget);
        }
      else if( EZ_WidgetType(widget) == EZ_WIDGET_IFANCY_LIST_BOX)
        {
          if(iflistbox_ret) *iflistbox_ret = widget;
          if(h_ret)*h_ret = EZ_IFListBoxHScroll(widget);
          if(v_ret) *v_ret = EZ_IFListBoxVScroll(widget);
        }
    }
}

/************************************************************************************/
EZ_Item  **EZ_GetFancyListBoxSelection(widget)
     EZ_Widget *widget;
{
  if(widget)
    {
      EZ_Widget *ifl;
      switch(EZ_WidgetType(widget))
	{
	case EZ_WIDGET_FANCY_LIST_BOX:
	  ifl = EZ_FListBoxIFListBox(widget);
	  break; 
	case EZ_WIDGET_IFANCY_LIST_BOX:
	  ifl = widget;
	  break;
	default:
	  ifl = NULL;
	  break;
	}
      if(ifl) return(EZ_GetIFListBoxSelection(ifl));
    }
  return(NULL);
}

/************************************************************************************/
void  EZ_SetFancyListBoxData(widget, data, nrows, ncols)
     EZ_Widget *widget;
     EZ_Item   **data;
     int nrows, ncols;
{
  if(widget)
    {
      EZ_Widget *ifl;
      switch(EZ_WidgetType(widget))
	{
	case EZ_WIDGET_FANCY_LIST_BOX:
	  ifl = EZ_FListBoxIFListBox(widget);
	  break; 
	  case EZ_WIDGET_IFANCY_LIST_BOX:
	  ifl = widget;
	default:
	  ifl = NULL;
	  break;
	}
      if(ifl) EZ_SetIFListBoxData(ifl, data, nrows, ncols);
    }
}

/************************************************************************************/
void  EZ_FancyListBoxClear(widget)
     EZ_Widget *widget;
{
  if(widget)
    {
      EZ_Widget *ifl;
      switch(EZ_WidgetType(widget))
	{
	case EZ_WIDGET_FANCY_LIST_BOX:
	  ifl = EZ_FListBoxIFListBox(widget);
	  break; 
	  case EZ_WIDGET_IFANCY_LIST_BOX:
	  ifl = widget;
	default:
	  ifl = NULL;
	  break;
	}
      if(ifl) EZ_ClearIFListBox(ifl);
    }
}
/************************************************************************************/
void  EZ_GetFancyListBoxXYOffset(widget, xoff,yoff)
     EZ_Widget *widget; int *xoff, *yoff;
{
  if(widget)
    {
      EZ_Widget *ifl;
      switch(EZ_WidgetType(widget))
	{
	case EZ_WIDGET_FANCY_LIST_BOX:
	  ifl = EZ_FListBoxIFListBox(widget);
	  break; 
	  case EZ_WIDGET_IFANCY_LIST_BOX:
	  ifl = widget;
	default:
	  ifl = NULL;
	  break;
	}
      if(ifl)
	{
	  if(xoff) *xoff = EZ_IFListBoxXStart(ifl);
	  if(yoff) *yoff = EZ_IFListBoxYStart(ifl);
	}
    }
}
void  EZ_SetFancyListBoxXYOffset(widget, xoff,yoff)
     EZ_Widget *widget; int xoff, yoff;
{
  if(widget)
    {
      EZ_Widget *ifl;
      switch(EZ_WidgetType(widget))
	{
	case EZ_WIDGET_FANCY_LIST_BOX:
	  ifl = EZ_FListBoxIFListBox(widget);
	  break; 
	  case EZ_WIDGET_IFANCY_LIST_BOX:
	  ifl = widget;
	default:
	  ifl = NULL;
	  break;
	}
      if(ifl)
	{
	  EZ_IFListBoxXStart(ifl) = xoff;
	  EZ_IFListBoxYStart(ifl) = yoff;
	  EZ_IFListBoxDirty(ifl) = 1;
	  if(EZ_WidgetMapped(widget)) EZ_DisplayWidget(widget);
	}
    }
}
/************************************************************************************/
void  EZ_SetFancyListBoxHeader(widget, items, nitems)
     EZ_Widget *widget;
     EZ_Item **items;
     int    nitems;
{
  if(widget)
    {
      EZ_Widget *ifl;
      switch(EZ_WidgetType(widget))
	{
	case EZ_WIDGET_FANCY_LIST_BOX:
	  ifl = EZ_FListBoxIFListBox(widget);
	  break; 
	  case EZ_WIDGET_IFANCY_LIST_BOX:
	  ifl = widget;
	default:
	  ifl = NULL;
	  break;
	}
      if(ifl)  EZ_SetIFListBoxHeader(ifl, items, nitems);
    }
}
/************************************************************************************/
extern void  EZ_GetIFListBoxSelectionIdx MY_ANSIARGS((EZ_Widget *wid, int *row, int *col));

void  EZ_GetFancyListBoxSelectionIdx(widget, row, col)
     EZ_Widget *widget;
     int *row, *col;
{
  if(widget)
    {
      EZ_Widget *twidget = NULL;
      switch( EZ_WidgetType(widget))
	{
	case EZ_WIDGET_FANCY_LIST_BOX:
	  twidget = EZ_FListBoxIFListBox(widget);
	  break;
	case EZ_WIDGET_IFANCY_LIST_BOX:
	  twidget = widget;
	  break;
	default:
	  break;
	}
      if(twidget)
	{
	  EZ_GetIFListBoxSelectionIdx(twidget, row, col);
	  return;
	}
    }
  if(row) *row = -1;
  if(col) *col = -1;
}
/***************************************************************/
void EZ_FancyListBoxInsertRow(widget, items, nitems, where)
     EZ_Widget *widget;
     EZ_Item **items;
     int nitems, where;
{
  if(widget)
    {
      EZ_Widget *twidget = NULL;
      switch( EZ_WidgetType(widget))
	{
	case EZ_WIDGET_FANCY_LIST_BOX:
	  twidget = EZ_FListBoxIFListBox(widget);
	  break;
	case EZ_WIDGET_IFANCY_LIST_BOX:
	  twidget = widget;
	  break;
	default:
	  break;
	}
      if(twidget)
	EZ_InsertIFListBoxRow(twidget, items, nitems, where);
    }
}
/***************************************************************/
void EZ_FancyListBoxDeleteRow(widget, idx)
     EZ_Widget *widget;
     int idx;
{
  if(widget)
    {
      EZ_Widget *twidget = NULL;
      switch( EZ_WidgetType(widget))
	{
	case EZ_WIDGET_FANCY_LIST_BOX:
	  twidget = EZ_FListBoxIFListBox(widget);
	  break;
	case EZ_WIDGET_IFANCY_LIST_BOX:
	  twidget = widget;
	  break;
	default:
	  break;
	}
      if(twidget)
	EZ_DeleteIFListBoxRow(twidget,idx);
    }
}

void EZ_FancyListBoxDeleteSomeRows(widget, indices, cnt)
     EZ_Widget *widget;
     int *indices, cnt;
{
  if(widget)
    {
      EZ_Widget *twidget = NULL;
      switch( EZ_WidgetType(widget))
	{
	case EZ_WIDGET_FANCY_LIST_BOX:
	  twidget = EZ_FListBoxIFListBox(widget);
	  break;
	case EZ_WIDGET_IFANCY_LIST_BOX:
	  twidget = widget;
	  break;
	default:
	  break;
	}
      if(twidget) EZ_DeleteIFListBoxSomeRows(twidget,indices, cnt);
    }
}

/***************************************************************/
void EZ_FancyListBoxReplaceItem(widget, olditem, newitem)
     EZ_Widget *widget;
     EZ_Item *olditem, *newitem;
{
  if(widget)
    {
      EZ_Widget *twidget = NULL;
      switch( EZ_WidgetType(widget))
	{
	case EZ_WIDGET_FANCY_LIST_BOX:
	  twidget = EZ_FListBoxIFListBox(widget);
	  break;
	case EZ_WIDGET_IFANCY_LIST_BOX:
	  twidget = widget;
	  break;
	default:
	  break;
	}
      if(twidget)
	EZ_ReplaceIFListBoxItem(twidget, olditem, newitem);
    }  
}
/***************************************************************/
void EZ_FancyListBoxReplaceItemUsingIdx(widget, rowI, colI, newitem)
     EZ_Widget *widget;
     EZ_Item   *newitem;
     int rowI, colI;
{
  if(widget)
    {
      EZ_Widget *twidget = NULL;
      switch( EZ_WidgetType(widget))
	{
	case EZ_WIDGET_FANCY_LIST_BOX:
	  twidget = EZ_FListBoxIFListBox(widget);
	  break;
	case EZ_WIDGET_IFANCY_LIST_BOX:
	  twidget = widget;
	  break;
	default:
	  break;
	}
      if(twidget)
	EZ_ReplaceIFListBoxItemUsingIdx(twidget, rowI, colI, newitem);
    }  
}
/***************************************************************/
EZ_Item  *EZ_GetFancyListBoxItemUsingIdx(widget, rowI,colI)
     EZ_Widget *widget;
     int rowI, colI;
{
  if(widget)
    {
      EZ_Widget *twidget = NULL;
      switch( EZ_WidgetType(widget))
	{
	case EZ_WIDGET_FANCY_LIST_BOX:
	  twidget = EZ_FListBoxIFListBox(widget);
	  break;
	case EZ_WIDGET_IFANCY_LIST_BOX:
	  twidget = widget;
	  break;
	default:
	  break;
	}
      if(twidget)
	return(EZ_GetIFListBoxItemUsingIdx(twidget, rowI, colI));
    }  
  return(NULL);
}
/********************************************************************/
void EZ_FancyListBoxSelectItemUsingIdx(widget, row, col, location)
     EZ_Widget *widget;
     int row, col, *location;
{
  if(widget)
    {
      EZ_Widget *twidget = NULL;
      switch( EZ_WidgetType(widget))
	{
	case EZ_WIDGET_FANCY_LIST_BOX:
	  twidget = EZ_FListBoxIFListBox(widget);
	  break;
	case EZ_WIDGET_IFANCY_LIST_BOX:
	  twidget = widget;
	  break;
	default:
	  break;
	}
      if(twidget)EZ_IFListBoxSelectItemUsingIdx(twidget, row, col, location);
    }   
} 

/********************************************************************/
void EZ_FancyListBoxSelectItem(widget, item, location)
     EZ_Widget *widget;
     EZ_Item *item;
     int *location;
{
  if(widget)
    {
      EZ_Widget *twidget = NULL;
      switch( EZ_WidgetType(widget))
	{
	case EZ_WIDGET_FANCY_LIST_BOX:
	  twidget = EZ_FListBoxIFListBox(widget);
	  break;
	case EZ_WIDGET_IFANCY_LIST_BOX:
	  twidget = widget;
	  break;
	default:
	  break;
	}
      if(twidget)EZ_IFListBoxSelectItem(twidget, item, location);
    }   
} 

int EZ_GetFancyListBoxNumberOfRows(widget)
     EZ_Widget *widget;
{
  if(widget)
    {
      EZ_Widget *twidget = NULL;
      switch( EZ_WidgetType(widget))
	{
	case EZ_WIDGET_FANCY_LIST_BOX:
	  twidget = EZ_FListBoxIFListBox(widget);
	  break;
	case EZ_WIDGET_IFANCY_LIST_BOX:
	  twidget = widget;
	  break;
	default:
	  break;
	}
      if(twidget) return(EZ_IFListBoxRows(twidget)); 
    }
  return(-1);
}
int EZ_GetFancyListBoxNumberOfColumns(widget)
     EZ_Widget *widget;
{
  if(widget)
    {
      EZ_Widget *twidget = NULL;
      switch( EZ_WidgetType(widget))
	{
	case EZ_WIDGET_FANCY_LIST_BOX:
	  twidget = EZ_FListBoxIFListBox(widget);
	  break;
	case EZ_WIDGET_IFANCY_LIST_BOX:
	  twidget = widget;
	  break;
	default:
	  break;
	}
      if(twidget) return(EZ_IFListBoxColumns(twidget));
    }
  return(-1);
}

/********************************************************************/
EZ_Item  ***EZ_GetFancyListBoxData(widget) EZ_Widget *widget;
{
  if(widget)
    {
      EZ_Widget *twidget = NULL;
      switch( EZ_WidgetType(widget))
	{
	case EZ_WIDGET_FANCY_LIST_BOX:
	  twidget = EZ_FListBoxIFListBox(widget);
	  break;
	case EZ_WIDGET_IFANCY_LIST_BOX:
	  twidget = widget;
	  break;
	default:
	  break;
	}
      if(twidget) return(EZ_IFListBoxData(twidget));
    }
  return(NULL);
}
/********************************************************************/

EZ_Item **EZ_FancyListBoxFindRowAtXY(widget, x,y, r_ret)
     EZ_Widget *widget; int x; int y; int *r_ret; 
{
  if(widget)
    {
      EZ_Widget *twidget = NULL;
      switch( EZ_WidgetType(widget))
	{
	case EZ_WIDGET_FANCY_LIST_BOX:
	  twidget = EZ_FListBoxIFListBox(widget);
	  break;
	case EZ_WIDGET_IFANCY_LIST_BOX:
	  twidget = widget;
	  break;
	default:
	  break;
	}
      widget = twidget;
      if(widget)
	{
	  int numcols = EZ_IFListBoxColumns(widget);
	  int nrows = EZ_IFListBoxRows(widget);
	  EZ_Item ***data =EZ_IFListBoxData(widget);
	  int bw = EZ_WidgetBorderWidth(widget) + EZ_WidgetPadB(widget);
	  int yoffset = bw + EZ_WidgetPadY(widget);
	  int Rx, Ry, ww, Rw, Rh, hh, rowN;
	  int *vsa = EZ_IFListBoxVSizes(widget);
	  
	  Rx = bw;
	  Ry = yoffset + vsa[0];
	  ww = EZ_WidgetWidth(widget);
	  Rw = ww - (bw << 1);
	  hh = EZ_WidgetHeight(widget);
	  Rh = hh - bw - Ry;
	  rowN = EZ_FindPointerOnRowIFL(data, nrows, numcols, x, y, Rx, Ry, Rw, Rh);
	  if(rowN >= 0)
	    {
	      if(r_ret) *r_ret = rowN;
	      return(data[rowN]);
	    }
	}
    }
  return(NULL);
}

EZ_Item *EZ_FancyListBoxFindItemAtXY(widget, x,y, r_ret, c_ret)
     EZ_Widget *widget; int x; int y; int *r_ret; int *c_ret;
{
  if(widget)
    {
      EZ_Widget *twidget = NULL;
      switch( EZ_WidgetType(widget))
	{
	case EZ_WIDGET_FANCY_LIST_BOX:
	  twidget = EZ_FListBoxIFListBox(widget);
	  break;
	case EZ_WIDGET_IFANCY_LIST_BOX:
	  twidget = widget;
	  break;
	default:
	  break;
	}
      widget = twidget;
      if(widget)
	{
	  int numcols = EZ_IFListBoxColumns(widget);
	  int nrows = EZ_IFListBoxRows(widget);
	  EZ_Item ***data =EZ_IFListBoxData(widget);
	  int bw = EZ_WidgetBorderWidth(widget) + EZ_WidgetPadB(widget);
	  int yoffset = bw + EZ_WidgetPadY(widget);
	  int Rx, Ry, ww, Rw, Rh, hh, rowN, colN;
	  int *vsa = EZ_IFListBoxVSizes(widget);
	  
	  Rx = bw;
	  Ry = yoffset + vsa[0];
	  ww = EZ_WidgetWidth(widget);
	  Rw = ww - (bw << 1);
	  hh = EZ_WidgetHeight(widget);
	  Rh = hh - bw - Ry;
	  EZ_FindPointerOnRowAndColIFL(data, nrows, numcols, x, y, &rowN, &colN, Rx, Ry, Rw, Rh);
	  if(rowN >= 0 && colN >= 0)
	    {
	      EZ_Item **row = data[rowN];
	      if(r_ret) *r_ret = rowN;
	      if(c_ret) *c_ret = colN;
	      return(row[colN]);
	    }
	}
    }
  return(NULL);
}

/********************************************************************/

int *EZ_FancyListBoxGetHighlightedRowsA(widget, cnt_ret)
     EZ_Widget *widget; int *cnt_ret;
{
  if(widget)
    {
      EZ_Widget *twidget = NULL;
      switch( EZ_WidgetType(widget))
	{
	case EZ_WIDGET_FANCY_LIST_BOX:
	  twidget = EZ_FListBoxIFListBox(widget);
	  break;
	case EZ_WIDGET_IFANCY_LIST_BOX:
	  twidget = widget;
	  break;
	default:
	  break;
	}
      if(twidget) return(EZ_IFListBoxGetHighlightedRowsA(twidget, cnt_ret));
    }
  return(NULL);
}
/*************************************************************/
int  EZ_FancyListBoxGetHighlightedRows(widget, ary, siz)
     EZ_Widget *widget; int *ary, siz;
{
  if(widget)
    {
      EZ_Widget *twidget = NULL;
      switch( EZ_WidgetType(widget))
	{
	case EZ_WIDGET_FANCY_LIST_BOX:
	  twidget = EZ_FListBoxIFListBox(widget);
	  break;
	case EZ_WIDGET_IFANCY_LIST_BOX:
	  twidget = widget;
	  break;
	default:
	  break;
	}
      if(twidget) return(EZ_IFListBoxGetHighlightedRows(twidget, ary, siz));
    }
  return(0);
}
/*************************************************************/
EZ_Item **EZ_FancyListBoxGetHighlightedItemsA(widget, cnt_ret)
     EZ_Widget *widget; int *cnt_ret;
{
  if(widget)
    {
      EZ_Widget *twidget = NULL;
      switch( EZ_WidgetType(widget))
	{
	case EZ_WIDGET_FANCY_LIST_BOX:
	  twidget = EZ_FListBoxIFListBox(widget);
	  break;
	case EZ_WIDGET_IFANCY_LIST_BOX:
	  twidget = widget;
	  break;
	default:
	  break;
	}
      if(twidget) return(EZ_IFListBoxGetHighlightedItemsA(twidget, cnt_ret));
    }
  return(NULL);
}
/*************************************************************/
int EZ_FancyListBoxGetHighlightedItems(widget, ary, siz)
     EZ_Widget *widget; EZ_Item **ary; int siz;
{
  if(widget)
    {
      EZ_Widget *twidget = NULL;
      switch( EZ_WidgetType(widget))
	{
	case EZ_WIDGET_FANCY_LIST_BOX:
	  twidget = EZ_FListBoxIFListBox(widget);
	  break;
	case EZ_WIDGET_IFANCY_LIST_BOX:
	  twidget = widget;
	  break;
	default:
	  break;
	}
      if(twidget) return(EZ_IFListBoxGetHighlightedItems(twidget, ary,siz));
    }
  return(0);
}
/*************************************************************/
int EZ_FancyListBoxGetHithlightedItemsAndIndices(widget, itm_ret, r_ret, c_ret)
     EZ_Widget *widget; EZ_Item ***itm_ret; int **r_ret, **c_ret;
{
  if(widget)
    {
      EZ_Widget *twidget = NULL;
      switch( EZ_WidgetType(widget))
	{
	case EZ_WIDGET_FANCY_LIST_BOX:
	  twidget = EZ_FListBoxIFListBox(widget);
	  break;
	case EZ_WIDGET_IFANCY_LIST_BOX:
	  twidget = widget;
	  break;
	default:
	  break;
	}
      if(twidget)
	return(EZ_FancyListBoxGetHithlightedItemsAndIndices(twidget, itm_ret, r_ret, c_ret));
    }
  return(0);  
}
/********************************************************************/

void EZ_SetFancyListBoxRowBg(widget, mode, bg1, bg2)
     EZ_Widget *widget; char *bg1, *bg2; int mode;
{
  if(widget)
    {
      EZ_Widget *twidget = NULL;
      switch( EZ_WidgetType(widget))
	{
	case EZ_WIDGET_FANCY_LIST_BOX:
	  twidget = EZ_FListBoxIFListBox(widget);
	  break;
	case EZ_WIDGET_IFANCY_LIST_BOX:
	  twidget = widget;
	  break;
	default:
	  break;
	}
      if(twidget)
	{
	  unsigned long *bg = EZ_IFListBoxRowBG(twidget);
	  EZ_IFListBoxRowBGSet(twidget) = mode;
	  if(bg1) bg[0]= EZ_AllocateColorFromName(bg1);
	  else  bg[0] = 0;
	  if(bg2) bg[1]= EZ_AllocateColorFromName(bg2);
	  else bg[1] = 0;
	}
    }
}
/********************************************************************/
#undef _EZ_WIDGET_FLIST_BOX_C_



