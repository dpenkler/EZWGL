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
 ***              Widget Working Area                          ***
 ***                                                           ***
 *****************************************************************/
#define _EZ_WIDGET_WORK_AREA_C_

#include "EZ_Widget.h"

/*********************************************************************
 * 
 *  Functions implemented in this file:
 */
EZ_Widget        *EZ_CreateWorkArea MY_ANSIARGS((EZ_Widget *parent, int hsc, int vsc));
void             EZ_DrawWidgetWorkArea MY_ANSIARGS((EZ_Widget *widget));

EZ_Item          *EZ_GetWorkAreaSelection MY_ANSIARGS((EZ_Widget *widget));
void              EZ_SetWorkAreaGeometryManager MY_ANSIARGS((EZ_Widget *widget,
							     EZ_WorkAreaGeometryManager managet,
							     void *pdata  ));


void              EZ_WorkAreaInsertItem MY_ANSIARGS((EZ_Widget *widget, EZ_Item *item));
void              EZ_WorkAreaInsertItems MY_ANSIARGS((EZ_Widget *widget, EZ_Item **item, int n));
void              EZ_WorkAreaDeleteItem MY_ANSIARGS((EZ_Widget *widget, EZ_Item *item));
void              EZ_WorkAreaDeleteItemO MY_ANSIARGS((EZ_Widget *widget, EZ_Item *item, int o));
void              EZ_WorkAreaClear MY_ANSIARGS((EZ_Widget *widget));

void              EZ_WorkAreaRearrange MY_ANSIARGS((EZ_Widget *widget));

void              EZ_WorkAreaInsertItemBefore MY_ANSIARGS((EZ_Widget *widget, EZ_Item *item, EZ_Item *newItem));
void              EZ_WorkAreaInsertItemAfter  MY_ANSIARGS((EZ_Widget *widget, EZ_Item *item, EZ_Item *newItem));
void              EZ_WorkAreaInsertItemUsingIdx MY_ANSIARGS((EZ_Widget *widget, EZ_Item *item, int idx));
void              EZ_WorkAreaDeleteItemUsingIdx MY_ANSIARGS((EZ_Widget *widget, int idx));
void              EZ_WorkAreaDeleteItemUsingIdxO MY_ANSIARGS((EZ_Widget *widget, int idx, int dp));
void              EZ_WorkAreaDeleteSomeItemsUsingIndices  MY_ANSIARGS((EZ_Widget *widget, int *ids, int cnt));
void              EZ_GetWorkAreaWidgetComponents MY_ANSIARGS((EZ_Widget *widget, EZ_Widget **iwa, 
							EZ_Widget **hsc, EZ_Widget **vsc));

void              EZ_WorkAreaSelectItem MY_ANSIARGS((EZ_Widget *widget, EZ_Item *item, int *location));
void              EZ_WorkAreaSelectItemUsingIdx MY_ANSIARGS((EZ_Widget *widget, int idx, int *location));
EZ_Item           *EZ_WorkAreaFindItemAtXY MY_ANSIARGS((EZ_Widget *wa, int x, int y));

void              EZ_GetWorkAreaXYOffset MY_ANSIARGS((EZ_Widget *widget, int *x, int *y));
void              EZ_SetWorkAreaXYOffset MY_ANSIARGS((EZ_Widget *widget, int x, int y));
int               EZ_GetWorkAreaNumberOfItems MY_ANSIARGS((EZ_Widget *widget));
EZ_Item           *EZ_GetWorkAreaItemUsingIdx MY_ANSIARGS((EZ_Widget *wa, int x));
EZ_Item           **EZ_GetWorkAreaData MY_ANSIARGS((EZ_Widget *wa));

EZ_Item           **EZ_WorkAreaGetHighlightedItemsA MY_ANSIARGS((EZ_Widget *widget, int *n_ret));
int               EZ_WorkAreaGetHighlightedItems MY_ANSIARGS((EZ_Widget *widget,  EZ_Item ** a, int siz));

/*********************************************************************
 * 
 * Local functions.
 */

/*********************************************************************
 * 
 *  Local Variables.
 */
static EZ_WidgetHandle EZ_WorkAreaHandle =
{
  EZ_ComputeWidgetUnknownSize,
  EZ_DrawWidgetWorkArea,
  EZ_FreeUnknownData,
  EZ_UnkownEventHandle,
};

/*********************************************************************/

EZ_Widget  *EZ_CreateWorkArea(parent, hscro, vscro)
     EZ_Widget  *parent;     /* parent widget    */
     int         hscro, vscro;
{
  EZ_Widget  *wptr, *iwa, *hscroll=NULL, *vscroll=NULL;

  wptr = EZ_CreateNewWidget(parent);
  EZ_WidgetHandlingFunctions[EZ_WIDGET_WORK_AREA] = &EZ_WorkAreaHandle;
  EZ_SetWidgetTypeAndNames(wptr, EZ_WIDGET_WORK_AREA);  

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

  iwa = EZ_WorkAreaIWorkArea(wptr) = EZ_CreateIWorkArea(wptr);
  EZ_WidgetGroupLeader(iwa) = wptr;
  EZ_WidgetGBX(iwa) = 0;
  EZ_WidgetGBY(iwa) = 0;
  EZ_WidgetGBW(iwa) = 1;
  EZ_WidgetGBH(iwa) = 1;

  EZ_SetWidgetExpandFlag(iwa);
  if(vscro)
    {
      vscroll = EZ_WorkAreaVScroll(wptr) = EZ_CreateScrollbar(wptr,EZ_WIDGET_VERTICAL_SCROLLBAR);
      EZ_SetWidgetExpandFlag(vscroll);
      EZ_ScrollObject(vscroll) = iwa;
      EZ_WidgetGroupLeader(vscroll) = wptr;
      EZ_WidgetGBX(vscroll) = 1;
      EZ_WidgetGBY(vscroll) = 0;
      EZ_WidgetGBW(vscroll) = 1;
      EZ_WidgetGBH(vscroll) = 1;
    }
  if(hscro)
    {
      hscroll = EZ_WorkAreaHScroll(wptr) = EZ_CreateScrollbar(wptr,EZ_WIDGET_HORIZONTAL_SCROLLBAR);
      EZ_SetWidgetExpandFlag(hscroll);
      EZ_ScrollObject(hscroll) = iwa;
      EZ_WidgetGroupLeader(hscroll) = wptr;
      EZ_WidgetGBX(hscroll) = 0;
      EZ_WidgetGBY(hscroll) = 1;
      EZ_WidgetGBW(hscroll) = 1;
      EZ_WidgetGBH(hscroll) = 1;
    }
  EZ_IWorkAreaHScroll(iwa) = hscroll;
  EZ_IWorkAreaVScroll(iwa) = vscroll;  
  EZ_IWorkAreaWorkArea(iwa) = wptr;   

  return(wptr);
}
/************************************************************************************************/

void   EZ_DrawWidgetWorkArea(wptr)
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
/*****************************************************************************/
 
void  EZ_SetWorkAreaGeometryManager(widget, manager, GMdata)
     EZ_Widget *widget;
     EZ_WorkAreaGeometryManager manager;
     void *GMdata;
{
  if(widget)
    {
      int type = EZ_WidgetType(widget);
      EZ_Widget *twidget = NULL;

      if(type == EZ_WIDGET_IWORK_AREA) twidget = widget;
      else if(type == EZ_WIDGET_WORK_AREA) twidget = EZ_WorkAreaIWorkArea(widget);
      
      if(twidget)
	{
	  EZ_IWorkAreaGeometryManager(twidget) = (EZ_WorkAreaGeometryManager)manager;
	  EZ_IWorkAreaGMData(twidget) = GMdata;
	}
    }
}

/******************************************************************************************/
EZ_Item *EZ_GetWorkAreaSelection(widget)
     EZ_Widget *widget;
{
  if(widget)
    {
      int type = EZ_WidgetType(widget);
      EZ_Widget *twidget = NULL;
      
      if(type == EZ_WIDGET_IWORK_AREA) twidget = widget;
      else  if(type == EZ_WIDGET_WORK_AREA) twidget = EZ_WorkAreaIWorkArea(widget);
      
      if(twidget) return(EZ_IWorkAreaSelection(twidget));
    }
  return(NULL);
}
/******************************************************************************************/
void EZ_WorkAreaInsertItem(widget, item)
     EZ_Widget *widget;
     EZ_Item *item;
{
  if(widget)
    {
      int type = EZ_WidgetType(widget);
      EZ_Widget *twidget = NULL;
      
      if(type == EZ_WIDGET_IWORK_AREA) twidget = widget;
      else  if(type == EZ_WIDGET_WORK_AREA) twidget = EZ_WorkAreaIWorkArea(widget);
      
      if(twidget) 
	EZ_IWorkAreaInsertItem(twidget, item);
    }
}

void EZ_WorkAreaInsertItems(widget, item, n)
     EZ_Widget *widget;
     EZ_Item **item; int n;
{
  if(widget)
    {
      int type = EZ_WidgetType(widget);
      EZ_Widget *twidget = NULL;
      
      if(type == EZ_WIDGET_IWORK_AREA) twidget = widget;
      else  if(type == EZ_WIDGET_WORK_AREA) twidget = EZ_WorkAreaIWorkArea(widget);
      
      if(twidget) 
	EZ_IWorkAreaInsertItems(twidget, item, n);
    }
}

  
void  EZ_WorkAreaDeleteItem(widget, item)
     EZ_Widget *widget;
     EZ_Item *item;
{
  if(widget)
    {
      int type = EZ_WidgetType(widget);
      EZ_Widget *twidget = NULL;
      
      if(type == EZ_WIDGET_IWORK_AREA) twidget = widget;
      else  if(type == EZ_WIDGET_WORK_AREA) twidget = EZ_WorkAreaIWorkArea(widget);
      
      if(twidget) 
	EZ_IWorkAreaDeleteItem(twidget, item);
    }
}

void  EZ_WorkAreaDeleteItemO(widget, item, dp)
     EZ_Widget *widget;
     EZ_Item *item;int dp;
{
  if(widget)
    {
      int type = EZ_WidgetType(widget);
      EZ_Widget *twidget = NULL;
      
      if(type == EZ_WIDGET_IWORK_AREA) twidget = widget;
      else  if(type == EZ_WIDGET_WORK_AREA) twidget = EZ_WorkAreaIWorkArea(widget);
      
      if(twidget) 
	EZ_IWorkAreaDeleteItemO(twidget, item, dp);
    }
}



void  EZ_WorkAreaClear(widget)
     EZ_Widget *widget;
{
  if(widget)
    {
      int type = EZ_WidgetType(widget);
      EZ_Widget *twidget = NULL;
      
      if(type == EZ_WIDGET_IWORK_AREA) twidget = widget;
      else  if(type == EZ_WIDGET_WORK_AREA) twidget = EZ_WorkAreaIWorkArea(widget);
      
      if(twidget) 
	EZ_IWorkAreaDeleteAllItems(twidget);
    }
}


void  EZ_WorkAreaRearrange(widget)
     EZ_Widget *widget;
{
  if(widget)
    {
      int type = EZ_WidgetType(widget);
      EZ_Widget *twidget = NULL;
      
      if(type == EZ_WIDGET_IWORK_AREA) twidget = widget;
      else  if(type == EZ_WIDGET_WORK_AREA) twidget = EZ_WorkAreaIWorkArea(widget);
      
      if(twidget)
	{
	  EZ_IWorkAreaDirty(twidget) = 1;
	  if(EZ_WidgetMapped(twidget)) EZ_DrawWidget(twidget);
	}
    }
}
/***************************************************************************************/
void EZ_WorkAreaInsertItemBefore(widget, item, newItem)
     EZ_Widget *widget;
     EZ_Item *item, *newItem;
{
  if(widget)
    {
      int type = EZ_WidgetType(widget);
      EZ_Widget *twidget = NULL;
      
      if(type == EZ_WIDGET_IWORK_AREA) twidget = widget;
      else  if(type == EZ_WIDGET_WORK_AREA) twidget = EZ_WorkAreaIWorkArea(widget);
      
      if(twidget) 
	EZ_IWorkAreaInsertItemBeforeAfter(twidget,item, newItem, 0);
    }
}
/***************************************************************************************/
void EZ_WorkAreaInsertItemAfter(widget, item, newItem)
     EZ_Widget *widget;
     EZ_Item *item, *newItem;
{
  if(widget)
    {
      int type = EZ_WidgetType(widget);
      EZ_Widget *twidget = NULL;
      
      if(type == EZ_WIDGET_IWORK_AREA) twidget = widget;
      else  if(type == EZ_WIDGET_WORK_AREA) twidget = EZ_WorkAreaIWorkArea(widget);
      
      if(twidget) 
	EZ_IWorkAreaInsertItemBeforeAfter(twidget,item, newItem, 1);
    }
}
/***************************************************************************************/
void EZ_WorkAreaInsertItemUsingIdx(widget, item, idx)
     EZ_Widget *widget;
     EZ_Item *item;
     int idx;
{
  if(widget)
    {
      int type = EZ_WidgetType(widget);
      EZ_Widget *twidget = NULL;
      
      if(type == EZ_WIDGET_IWORK_AREA) twidget = widget;
      else  if(type == EZ_WIDGET_WORK_AREA) twidget = EZ_WorkAreaIWorkArea(widget);
      
      if(twidget) 
	EZ_IWorkAreaInsertItemUsingIdx(twidget, idx, item);
    }
}

/***************************************************************************************/
void EZ_WorkAreaDeleteItemUsingIdx(widget, idx)
     EZ_Widget *widget;
     int idx;
{
  if(widget)
    {
      int type = EZ_WidgetType(widget);
      EZ_Widget *twidget = NULL;
      
      if(type == EZ_WIDGET_IWORK_AREA) twidget = widget;
      else  if(type == EZ_WIDGET_WORK_AREA) twidget = EZ_WorkAreaIWorkArea(widget);
      
      if(twidget) 
	EZ_IWorkAreaDeleteItemUsingIdx(twidget, idx);
    }
}

void EZ_WorkAreaDeleteItemUsingIdxO(widget, idx, dp)
     EZ_Widget *widget;
     int idx,dp;
{
  if(widget)
    {
      int type = EZ_WidgetType(widget);
      EZ_Widget *twidget = NULL;
      
      if(type == EZ_WIDGET_IWORK_AREA) twidget = widget;
      else  if(type == EZ_WIDGET_WORK_AREA) twidget = EZ_WorkAreaIWorkArea(widget);
      
      if(twidget) 
	EZ_IWorkAreaDeleteItemUsingIdxO(twidget, idx,dp);
    }
}


void EZ_WorkAreaDeleteSomeItemsUsingIndices(widget, indices, cnt)
     EZ_Widget *widget;
     int *indices, cnt;
{
  if(widget)
    {
      int type = EZ_WidgetType(widget);
      EZ_Widget *twidget = NULL;
      
      if(type == EZ_WIDGET_IWORK_AREA) twidget = widget;
      else  if(type == EZ_WIDGET_WORK_AREA) twidget = EZ_WorkAreaIWorkArea(widget);
      
      if(twidget) EZ_IWorkAreaDeleteSomeItemsUsingIndices(twidget, indices, cnt);
    }
}     

/***************************************************************************************/
void  EZ_GetWorkAreaWidgetComponents(widget, iwret, hsret, vsret)
     EZ_Widget *widget;
     EZ_Widget **iwret, **hsret, **vsret;
{
  if(widget)
    {
      if( EZ_WidgetType(widget)== EZ_WIDGET_WORK_AREA)
        {
          if(iwret) *iwret = EZ_WorkAreaIWorkArea(widget);
          if(hsret) *hsret = EZ_WorkAreaHScroll(widget);
          if(vsret) *vsret = EZ_WorkAreaVScroll(widget);
        }
      else if( EZ_WidgetType(widget)== EZ_WIDGET_IWORK_AREA)
        {
          if(iwret) *iwret = widget;
          if(hsret) *hsret = EZ_IWorkAreaHScroll(widget);
          if(vsret) *vsret = EZ_IWorkAreaVScroll(widget);
        }
    }
}


void EZ_WorkAreaSelectItem(widget, item, location)
     EZ_Widget *widget; EZ_Item *item; int *location;
{
  if(widget)
    {
      int type = EZ_WidgetType(widget);
      EZ_Widget *twidget = NULL;
      
      if(type == EZ_WIDGET_IWORK_AREA) twidget = widget;
      else  if(type == EZ_WIDGET_WORK_AREA) twidget = EZ_WorkAreaIWorkArea(widget);
      
      if(twidget) EZ_IWorkAreaSelectItem(twidget, item, location);
    }
}
void EZ_WorkAreaSelectItemUsingIdx(widget, idx, location)
     EZ_Widget *widget; int idx, *location;
{
  if(widget)
    {
      int type = EZ_WidgetType(widget);
      EZ_Widget *twidget = NULL;
      
      if(type == EZ_WIDGET_IWORK_AREA) twidget = widget;
      else  if(type == EZ_WIDGET_WORK_AREA) twidget = EZ_WorkAreaIWorkArea(widget);
      
      if(twidget) EZ_IWorkAreaSelectItemUsingIdx(twidget, idx, location);
    }  
}
/***************************************************************************************/
void EZ_GetWorkAreaXYOffset(widget, x, y)
     EZ_Widget *widget; int *x, *y;
{
  if(widget)
    {
      int type = EZ_WidgetType(widget);
      EZ_Widget *twidget = NULL;
      
      if(type == EZ_WIDGET_IWORK_AREA) twidget = widget;
      else  if(type == EZ_WIDGET_WORK_AREA) twidget = EZ_WorkAreaIWorkArea(widget);
      
      if(twidget) 
	{
	  if(x) *x = EZ_IWorkAreaXStart(twidget);
	  if(y) *y = EZ_IWorkAreaYStart(twidget);
	}
    }
}

void EZ_SetWorkAreaXYOffset(widget, x, y)
     EZ_Widget *widget; int x, y;
{
  if(widget)
    {
      int type = EZ_WidgetType(widget);
      EZ_Widget *twidget = NULL;
      
      if(type == EZ_WIDGET_IWORK_AREA) twidget = widget;
      else  if(type == EZ_WIDGET_WORK_AREA) twidget = EZ_WorkAreaIWorkArea(widget);
      
      if(twidget) 
	{
	  EZ_IWorkAreaXStart(twidget) = x;
	  EZ_IWorkAreaYStart(twidget) = y;
	  EZ_IWorkAreaDirty(twidget) = 1;
	  if(EZ_WidgetMapped(widget)) EZ_DisplayWidget(widget);
	}
    }
}
/***************************************************************************************/
EZ_Item *EZ_WorkAreaFindItemAtXY(widget, x, y)
     EZ_Widget *widget; int x; int y;
{
  if(widget)
    {
      int type = EZ_WidgetType(widget);
      EZ_Widget *twidget = NULL;
      
      if(type == EZ_WIDGET_IWORK_AREA) twidget = widget;
      else  if(type == EZ_WIDGET_WORK_AREA) twidget = EZ_WorkAreaIWorkArea(widget);
      
      if(twidget) 
	{
	  int nitems = EZ_IWorkAreaNItems(twidget);
	  if(nitems > 0)
	    {
	      EZ_Item **data = EZ_IWorkAreaData(twidget);
	      return(EZ_FindPointerOnItemIWA(data, nitems, NULL,x,y));
	    }
	}
    }
  return(NULL);
}
/***************************************************************************************/
int  EZ_GetWorkAreaNumberOfItems(widget) EZ_Widget *widget;
{
  if(widget)
    {
      int type = EZ_WidgetType(widget);
      EZ_Widget *twidget = NULL;
      
      if(type == EZ_WIDGET_IWORK_AREA) twidget = widget;
      else  if(type == EZ_WIDGET_WORK_AREA) twidget = EZ_WorkAreaIWorkArea(widget);
      
      if(twidget) return(EZ_IWorkAreaNItems(twidget));
    }
  return(0);
}

EZ_Item *EZ_GetWorkAreaItemUsingIdx(widget, idx) EZ_Widget *widget; int idx;
{
  if(widget)
    {
      int type = EZ_WidgetType(widget);
      EZ_Widget *twidget = NULL;
      
      if(type == EZ_WIDGET_IWORK_AREA) twidget = widget;
      else  if(type == EZ_WIDGET_WORK_AREA) twidget = EZ_WorkAreaIWorkArea(widget);
      
      if(twidget)
	{
	  int bd = EZ_IWorkAreaNItems(twidget);
	  if(idx >= 0 && idx < bd)
	    {
	      EZ_Item **data = EZ_IWorkAreaData(twidget);
	      return(data[idx]);
	    }
	}
    }
  return(NULL);
}

EZ_Item **EZ_GetWorkAreaData(widget) EZ_Widget *widget;
{
  if(widget)
    {
      int type = EZ_WidgetType(widget);
      EZ_Widget *twidget = NULL;
      
      if(type == EZ_WIDGET_IWORK_AREA) twidget = widget;
      else  if(type == EZ_WIDGET_WORK_AREA) twidget = EZ_WorkAreaIWorkArea(widget);
      
      if(twidget) return(EZ_IWorkAreaData(twidget));
    }
  return(NULL);
}

/***************************************************************************************/
EZ_Item **EZ_WorkAreaGetHighlightedItemsA(widget, cnt_ret)
     EZ_Widget *widget; int *cnt_ret;
{
  if(widget)
    {
      int type = EZ_WidgetType(widget);
      EZ_Widget *twidget = NULL;
      if(type == EZ_WIDGET_IWORK_AREA) twidget = widget;
      else  if(type == EZ_WIDGET_WORK_AREA) twidget = EZ_WorkAreaIWorkArea(widget);
      if(twidget) return(EZ_IWorkAreaGetHighlightedItemsA(twidget, cnt_ret));
    }
  return(NULL);
}
/***************************************************************************************/
int  EZ_WorkAreaGetHighlightedItems(widget, ary, siz)
     EZ_Widget *widget; EZ_Item **ary; int siz;
{
  if(widget)
    {
      int type = EZ_WidgetType(widget);
      EZ_Widget *twidget = NULL;
      if(type == EZ_WIDGET_IWORK_AREA) twidget = widget;
      else  if(type == EZ_WIDGET_WORK_AREA) twidget = EZ_WorkAreaIWorkArea(widget);
      if(twidget) return(EZ_WorkAreaGetHighlightedItems(twidget, ary, siz));
    }
  return(0);
}
/***************************************************************************************/

#undef _EZ_WIDGET_WORK_AREA_C_
