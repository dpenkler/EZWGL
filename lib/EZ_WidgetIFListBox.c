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
 ***              Widget Internal Fancy List Box               ***
 ***                                                           ***
 *****************************************************************/
#define _EZ_WIDGET_IFLIST_BOX_C_
#include "EZ_Widget.h"


/*********************************************************************
 * 
 *  Functions implemented in this file:
 */
EZ_Widget        *EZ_CreateIFListBox MY_ANSIARGS((EZ_Widget *parent, int ncols, int seltype));
void             EZ_DrawWidgetIFListBox MY_ANSIARGS((EZ_Widget *widget));
void             EZ_ComputeWidgetIFListBoxSize MY_ANSIARGS((EZ_Widget *widget, int *w, int *h));
void             EZ_FreeWidgetIFListBoxData MY_ANSIARGS((EZ_Widget *widget));
void             EZ_IFListBoxEventHandle MY_ANSIARGS((EZ_Widget *widget, XEvent *event));
void             EZ_IFListBoxHandleBtnEventA MY_ANSIARGS((EZ_Widget *widget, XEvent *event));
void             EZ_IFListBoxHandleBtnEventB MY_ANSIARGS((EZ_Widget *widget, XEvent *event));
void             EZ_SetIFListBoxData MY_ANSIARGS((EZ_Widget *widget, EZ_Item **items, int row, int col));
void             EZ_InsertIFListBoxRow  MY_ANSIARGS((EZ_Widget *widget, EZ_Item **items, int nitems, int where));
void             EZ_DeleteIFListBoxRow  MY_ANSIARGS((EZ_Widget *widget, int idx));
void             EZ_DeleteIFListBoxSomeRows MY_ANSIARGS((EZ_Widget *widget, int *idces, int cnt));
void             EZ_ReplaceIFListBoxItem  MY_ANSIARGS((EZ_Widget *widget, EZ_Item *old, EZ_Item *newItem));
void             EZ_ReplaceIFListBoxItemUsingIdx MY_ANSIARGS((EZ_Widget *widget, int row, int col, EZ_Item *newItem));
void             EZ_ClearIFListBox  MY_ANSIARGS((EZ_Widget *widget));
void             EZ_SetIFListBoxHeader  MY_ANSIARGS((EZ_Widget *widget, EZ_Item **elts, int nelts));

EZ_Item          **EZ_GetIFListBoxSelection  MY_ANSIARGS((EZ_Widget *widget));
EZ_Item          *EZ_GetIFListBoxItemUsingIdx  MY_ANSIARGS((EZ_Widget *widget, int row, int col));

int              EZ_FindPointerOnRowIFL  MY_ANSIARGS((EZ_Item ***array, int nrow, int ncol, int x, int y,
						      int Rx, int Ry, int RW, int RH));
void             EZ_FindPointerOnRowAndColIFL  MY_ANSIARGS((EZ_Item ***array, int nrow, int ncol, int x, int y,
							    int *rowR, int *colR, int Rx, int Ry, int RW, int RH));

void             EZ_FlashRowOutline MY_ANSIARGS((EZ_Widget *widget, EZ_Item **row, int nelts));
void             EZ_DnDOutlineRow MY_ANSIARGS((EZ_Widget *widget, EZ_Item **row, int nelts,
					       int Rx, int Ry, int Rw, int Rh));

void             EZ_IFListBoxSelectItemUsingIdx MY_ANSIARGS((EZ_Widget *widget, int Srow, int Scol, int *location));
void             EZ_IFListBoxSelectItem MY_ANSIARGS((EZ_Widget *widget, EZ_Item *item, int *location));
void             EZ_IFListBoxAllocNewStorage MY_ANSIARGS((EZ_Widget *widget,int rows, int cols));
/*********************************************************************/
int              *EZ_IFListBoxGetHighlightedRowsA MY_ANSIARGS((EZ_Widget *widget, int *cnt));
int              EZ_IFListBoxGetHighlightedRows MY_ANSIARGS((EZ_Widget *widget, int *ary, int siz));
EZ_Item          **EZ_IFListBoxGetHighlightedItemsA MY_ANSIARGS((EZ_Widget *widget, int *cnt));
int              EZ_IFListBoxGetHighlightedItems MY_ANSIARGS((EZ_Widget *widget, EZ_Item **arg,int siz));
int              EZ_IFListBoxGetHithlightedItemsAndIndices MY_ANSIARGS((EZ_Widget *wgt, EZ_Item ***itm_ret,
									int **rowIdx_ret, int **colIdx_ret));
/*********************************************************************
 * 
 * Local functions.
 */
static void   update_ifl_scrollbars MY_ANSIARGS((EZ_Widget *widget));
static void   IFTimerCallBack MY_ANSIARGS((EZ_Timer *timer, void *data));
static void   iflistbox_vscroll_update_selection MY_ANSIARGS((EZ_Widget *widget,
							      int sline, int nsline, int mode));
static void   iflistbox_hscroll_update_selection MY_ANSIARGS((EZ_Widget *widget,
							      int sline, int col, int ncol, int tt));
static void   EZ_IFListBoxSetSelection MY_ANSIARGS((EZ_Widget *widget));

static void   EZ_IFListBoxGrowStorage MY_ANSIARGS((EZ_Widget *widget, int count));
static void   EZ_IFListBoxFindItem MY_ANSIARGS((EZ_Widget *widget, EZ_Item *item, int *loc));
static void   EZ_IFListBoxClearHighlight MY_ANSIARGS((EZ_Widget *widget,int bx,int by,int bw,int bh,int smode));
static void   EZ_IFBHighlightItemsInRectangle MY_ANSIARGS((EZ_Widget *widget, int bx,int by,int bw,int bh));
static void   EZ_IFBHighlightRowsInRectangle MY_ANSIARGS((EZ_Widget *widget, int bx,int by,int bw,int bh));

static int  ifboxGetNumberOfHiglightedRows MY_ANSIARGS((EZ_Widget *widget));
static int  ifboxGetNumberOfHiglightedItems MY_ANSIARGS((EZ_Widget *widget));
static void ifbDnDOneRow MY_ANSIARGS((EZ_Widget *widget, int sline, XEvent *xe, int Rx, int Ry, int Rw, int Rh));
static void ifbDnDNRows MY_ANSIARGS((EZ_Widget *widget, int nlines, XEvent *xe, int Rx, int Ry, int Rw, int Rh));

static void ifbDnDOneItem MY_ANSIARGS((EZ_Widget *widget, EZ_Item *item, XEvent *xe, 
				       int Rx, int Ry, int Rw, int Rh));
static void ifbDnDNItems MY_ANSIARGS((EZ_Widget *widget, int nitems, XEvent *xe, 
				      int Rx, int Ry, int Rw, int Rh));
/*********************************************************************
 * 
 *  Local Variables.
 */
static EZ_WidgetHandle EZ_IFListBoxHandle =
{
  EZ_ComputeWidgetIFListBoxSize,
  EZ_DrawWidgetIFListBox,
  EZ_FreeWidgetIFListBoxData,
  EZ_IFListBoxEventHandle,
};

/*********************************************************************/

EZ_Widget  *EZ_CreateIFListBox(parent, ncolumns, selectionType)
     EZ_Widget  *parent;     /* parent widget    */
     int ncolumns;
     int selectionType;
{
  EZ_Widget  *wptr;

  wptr = EZ_CreateNewWidget(parent);
  EZ_WidgetHandlingFunctions[EZ_WIDGET_IFANCY_LIST_BOX] = &EZ_IFListBoxHandle;

  EZ_SetWidgetTypeAndNames(wptr,EZ_WIDGET_IFANCY_LIST_BOX);
  EZ_WidgetBorderStyle(wptr)  = EZ_BORDER_DOWN;
  EZ_WidgetBorderWidth(wptr)  = 2;
  EZ_WidgetPadX(wptr) = 1;
  EZ_WidgetPadY(wptr) = 1;
  EZ_WidgetPadB(wptr) = 2;

  EZ_IFListBoxColumns(wptr)  = (ncolumns < 1 ?  1 : (ncolumns > 100? 100: ncolumns));
  EZ_IFListBoxRows(wptr)     = 0;
  EZ_IFListBoxSpace(wptr)    = 0;
  EZ_IFListBoxSelectionIDX(wptr)  = -1;
  EZ_IFListBoxSelectionCol(wptr)  = -1;

  EZ_IFListBoxHighlightBG(wptr)   = ~0;
  EZ_IFListBoxHighlightFG(wptr)   = ~0;
  EZ_IFListBoxTextBG(wptr)        = ~0;
  EZ_IFListBoxXSize(wptr)         = 1;
  EZ_IFListBoxXPageSize(wptr)     = 1; 
  EZ_IFListBoxXStart(wptr)        = 0; 
  EZ_IFListBoxYSize(wptr)         = 1; 
  EZ_IFListBoxYPageSize(wptr)     = 1; 
  EZ_IFListBoxYStart(wptr)        = 0; 
  EZ_IFListBoxDirty(wptr)         = 1;
  EZ_IFListBoxDisplayed(wptr)     = 0;

  EZ_IFListBoxFListBox(wptr) = (EZ_Widget *)NULL;
  EZ_IFListBoxHScroll(wptr) = (EZ_Widget *)NULL;
  EZ_IFListBoxVScroll(wptr) = (EZ_Widget *)NULL;

  EZ_IFListBoxSelectionType(wptr)  = (selectionType != 0);
  
  EZ_CreateTimer(0,330000,-1, (EZ_CallBack)IFTimerCallBack, wptr, 0);

  EZ_IFListBoxHSizes(wptr) = (int *)NULL;
  EZ_IFListBoxVSizes(wptr) = (int *)NULL;
  EZ_IFListBoxData(wptr) = (EZ_Item ***)NULL;
  EZ_IFListBoxSpace(wptr) = 0;

  EZ_IFListBoxAllocNewStorage(wptr, 64, ncolumns);  

  EZ_SetWidgetFocusableFlag(wptr);  

  EZ_Insert2DnDWidgetList(wptr);
  EZ_WidgetDnDHandler(wptr) = (EZ_DnDMsgHandler)EZ_IFListBoxDnDHandle;
  
  return(wptr);
}

void  EZ_FreeWidgetIFListBoxData(widget)
     EZ_Widget *widget;
{
  int          i, j;
  int          ncolumns = EZ_IFListBoxColumns(widget);
  int          nrows = EZ_IFListBoxSpace(widget);
  EZ_Item      ***data= EZ_IFListBoxData(widget);

  for(i = 0; i < nrows; i++)
    {
      EZ_Item **items = data[i];
      for(j = 0; j < ncolumns; j++)
	{
	  if(items[j]) EZ_DestroyItem(items[j]);
	}
      (void) my_free((char *)items);
    }
  (void) my_free((char *)data);
  (void) my_free((char *)EZ_IFListBoxHSizes(widget));
  (void) my_free((char *)EZ_IFListBoxVSizes(widget));
}
 
/********************************************************************
 *
 *  Figure out the dimension of an internal list box.
 */
void EZ_ComputeWidgetIFListBoxSize(widget, w, h)
     EZ_Widget *widget;
     int             *w, *h;
{
  int cw, ch;

  if(!(EZ_GetWidgetSizeComputedFlag(widget)))
    { 
      EZ_SetWidgetSizeComputedFlag(widget);
    }
  cw = EZ_WidgetPadX(widget) + EZ_WidgetBorderWidth(widget) + EZ_WidgetPadB(widget);
  ch = EZ_WidgetPadY(widget) + EZ_WidgetBorderWidth(widget) + EZ_WidgetPadB(widget);
  cw = cw + cw + 30;
  ch = ch + ch + 30;

  EZ_IFListBoxXPageSize(widget)     = 30; 
  EZ_IFListBoxYPageSize(widget)     = 30; 

  *w = cw;
  *h = ch;
}

/****************************************************************************
 *
 *  Draw a IFListBox widget
 */
void  EZ_DrawWidgetIFListBox(wptr)
     EZ_Widget *wptr;
{
  int             w, h, bw, padb,padb2;
  Pixmap          pixmap;
  Window          win;
  GC              gc;
  unsigned long   bgpv;

  win = EZ_WidgetWindow(wptr);
  w   = EZ_WidgetWidth(wptr);
  h   = EZ_WidgetHeight(wptr);
  bw = EZ_WidgetBorderWidth(wptr);
  padb = EZ_WidgetPadB(wptr);
  padb2 = padb + padb;
  /*-----------------------------------------------------------
   *  Create a pixmap, draw into this pixmap.
   *----------------------------------------------------------*/
  pixmap = XCreatePixmap(EZ_Display, win, w, h, EZ_Depth);    
  /*  */
  {
    if(EZ_IFListBoxTextBG(wptr) != ~0)
      {
	XSetForeground(EZ_Display, EZ_WRITABLEGC, EZ_IFListBoxTextBG(wptr));	
	gc = EZ_WRITABLEGC;
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
  }
  XFillRectangle(EZ_DisplayForWidgets, pixmap, gc, padb,padb, w -padb2, h-padb2); 
  
  /* display the lists */
  if(EZ_IFListBoxRows(wptr) > 0)
    {
      EZ_Item ***data = EZ_IFListBoxData(wptr);
      EZ_Item **row;
      int nitems = EZ_IFListBoxRows(wptr); 
      int ncolumns = EZ_IFListBoxColumns(wptr);
      int Wsize, Hsize, itmp;
      int bww =  EZ_WidgetBorderWidth(wptr) + EZ_WidgetPadB(wptr); 
      int xoff = bww + EZ_WidgetPadX(wptr);
      int yoff = bww + EZ_WidgetPadY(wptr);
      int vspacing = EZ_WidgetSepY(wptr);
      int x, y, i;
      int Rx = bww;
      int Ry = yoff;
      int Rw = w -(bww << 1);
      int Rh = h -(bww + yoff);
      int hmode = EZ_IFListBoxHighlightMode(wptr);
      int selmode = EZ_IFListBoxSelectionType(wptr);
      int *hsa = EZ_IFListBoxHSizes(wptr);
      int *vsa = EZ_IFListBoxVSizes(wptr);
      unsigned long fg = EZ_IFListBoxHighlightFG(wptr);
      unsigned long bg = EZ_IFListBoxHighlightBG(wptr);

      if(EZ_IFListBoxDirty(wptr))
	{
	  EZ_ComputeItemArrayGeometry(data, nitems, ncolumns, &Wsize, &Hsize, hsa, vsa); 
	  itmp = Wsize + (ncolumns -1) * (int) EZ_WidgetSepX(wptr);
	  EZ_IFListBoxXSize(wptr) = (itmp + (xoff << 1));
	  itmp = Hsize + (nitems -1) * (int) EZ_WidgetSepY(wptr);
	  EZ_IFListBoxYSize(wptr) = (itmp + (yoff << 1));
	  EZ_IFListBoxDirty(wptr)= 0;
	  if(EZ_IFListBoxXStart(wptr) + EZ_IFListBoxXSize(wptr) <= 0)
	    {
	      int itmp = Rw -  EZ_IFListBoxXSize(wptr);
	      EZ_IFListBoxXStart(wptr) = (itmp > 0? 0: itmp);
	    }
	  if(EZ_IFListBoxYStart(wptr) + EZ_IFListBoxYSize(wptr) <= 0)
	    {
	      int itmp = Rh -  EZ_IFListBoxYSize(wptr);
	      EZ_IFListBoxYStart(wptr) = itmp > 0 ? 0: itmp;
	    }
	}
      EZ_IFListBoxXPageSize(wptr) = w;
      EZ_IFListBoxYPageSize(wptr) = h;
      
      x = EZ_IFListBoxXStart(wptr) + xoff;
      y = yoff;
      itmp = (int) EZ_WidgetSepX(wptr);
      
      /* check for header */
      row = data[0];
      if(row[0]) 
	{
	  y += vsa[0];
	  Ry +=  vsa[0];
	  Rh -=  vsa[0];  
	  /* EZ_IFListBoxYPageSize(wptr) -= vsa[0]; */
	}
      y += EZ_IFListBoxYStart(wptr);
      for(i = 1; i < nitems; i++)
	{
	  EZ_Item *titem;
	  row = data[i];
	  EZ_DisplayOneRowOfItems(wptr, pixmap, row, ncolumns, x,y, itmp, hsa,
				  Rx,Ry,Rw,Rh,fg,bg,hmode, selmode);
	  if(selmode)
	    {
	      titem = row[0];
	      if(titem && EZ_ItemHighlighted(titem))
		EZ_HighlightOneRow(wptr, pixmap, row,ncolumns,fg, bg, Rx,Ry,Rw,Rh, hmode);
	    }
	  y +=  vsa[i] + vspacing;
	}

      /* update the header !!! */
      row = data[0];
      if(row[0]) 
	{
	  x = EZ_IFListBoxXStart(wptr) + xoff;
	  y = yoff;
	  EZ_DisplayHeaderRowIFLB(wptr, pixmap, row, ncolumns, x,y, itmp, hsa,
				  Rx, yoff, Rw,Rh,fg,bg,hmode,selmode);
	  /*
	  EZ_DisplayOneRowOfItems(wptr, pixmap, row, ncolumns, x,y, itmp, hsa,
				  Rx, yoff, Rw,Rh,fg,bg,hmode,selmode);
				  */
	}
    }
  EZ_FixFocusHighlightBorder(wptr, pixmap, w, h, padb);

  EZ_DrawRectBorder(wptr, pixmap);
  XCopyArea(EZ_Display,pixmap,win, EZ_WRITABLEGC,0,0,w,h,0,0); 
  XFreePixmap(EZ_DisplayForWidgets, pixmap); 
  
  update_ifl_scrollbars(wptr);

  if(EZ_IFListBoxDisplayed(wptr) == 0)
    {
      int srow = EZ_IFListBoxSelectionIDX(wptr);
      EZ_IFListBoxDisplayed(wptr) = 1;
      
      if( srow > 0)
	{
	  int smode = EZ_IFListBoxSelectionType(wptr);
	  iflistbox_vscroll_update_selection(wptr, srow, srow, smode);
	  if(smode == 0) 
	    {
	      int scol = EZ_IFListBoxSelectionCol(wptr);
	      iflistbox_hscroll_update_selection(wptr, srow, scol, scol, 0);
	    }
	  EZ_HandleMotionCallBacks(wptr,EZ_IFListBoxMotionCallBack(wptr));
	  /* EZ_IFListBoxSetSelection(wptr);*/
	}
    }
}
/*************************************************************************/
static void update_ifl_scrollbars(wptr)
     EZ_Widget *wptr;
{
  if(EZ_LookupWidgetFromAllHT(EZ_IFListBoxHScroll(wptr)) != (EZ_Widget *)NULL)
    {
      EZ_UpdateScrollbarI(EZ_IFListBoxHScroll(wptr),
			  EZ_IFListBoxXSize(wptr),
			  EZ_IFListBoxXPageSize(wptr),
			  EZ_IFListBoxXStart(wptr));
    }
  else
    {
      EZ_Widget *p = EZ_IFListBoxFListBox(wptr);
      EZ_IFListBoxHScroll(wptr) = NULL;
      if(p) EZ_FListBoxHScroll(p) = NULL;
    }

  if(EZ_LookupWidgetFromAllHT(EZ_IFListBoxVScroll(wptr)) != (EZ_Widget *)NULL)
    {
      EZ_UpdateScrollbarI(EZ_IFListBoxVScroll(wptr),
			  EZ_IFListBoxYSize(wptr),
			  EZ_IFListBoxYPageSize(wptr),
			  -EZ_IFListBoxYStart(wptr));
    }
  else
    {
      EZ_Widget *p = EZ_IFListBoxFListBox(wptr);
      EZ_IFListBoxVScroll(wptr) = NULL;
      if(p) EZ_FListBoxVScroll(p) = NULL;
    }
}
/*************************************************************************
 *
 *  Event handling
 */
void  EZ_IFListBoxEventHandle(widget, event)
     EZ_Widget *widget;
     XEvent          *event;
{
  int       smode, *vsa;

  if(event->type == Expose && event->xexpose.count == 0)   EZ_DrawWidget(widget);      
  if( EZ_GetWidgetDisabledFlag(widget) )  return;

  vsa = EZ_IFListBoxVSizes(widget);
  smode = EZ_IFListBoxSelectionType(widget); /* 0 for picking an item, 1 for picking a row */

  switch(event->type)
    {
    case SelectionRequest:
      EZ_HandleSelectionRequest(widget,event);
      break;
    case ButtonPress:
      smode = EZ_IFListBoxSelectionType(widget);
      if(smode) EZ_IFListBoxHandleBtnEventA(widget, event); /* row mode */
      else   EZ_IFListBoxHandleBtnEventB(widget, event);
      break;
    case EnterNotify:
      break;
    case LeaveNotify:
      break;
    case KeyPress:
      if(EZ_IFListBoxRows(widget) > 0)
	{
	  if(EZ_IFListBoxSelectionIDX(widget) < 0)
	    EZ_IFListBoxSelectionIDX(widget) = 0;
	  if(EZ_IFListBoxSelectionCol(widget) < 0)
	    EZ_IFListBoxSelectionCol(widget) = 0;
	  {
#define TEMP_BUFFER_SIZE    32
	    int               count, move_unit = 1;
	    KeySym            keysym;
	    XComposeStatus    compose; 
	    char              tmpbuffer[TEMP_BUFFER_SIZE];
	    int               buffersize = TEMP_BUFFER_SIZE;
#undef TEMP_BUFFER_SIZE
	    if(event->xkey.state & ShiftMask) move_unit = 2;
	    if(event->xkey.state & ControlMask)     move_unit = 4;
	
	    count = XLookupString(&(event->xkey), tmpbuffer, buffersize, &keysym, &compose);
	    tmpbuffer[count] = '\0'; 
	
	    switch(keysym)
	      {
	      case XK_Up: case XK_k:  case XK_p: case XK_K:  case XK_P: 
#ifdef XK_KP_Up
	      case XK_KP_Up:
#endif
		{
		  int sline = EZ_IFListBoxSelectionIDX(widget);
		  int tmp = sline - move_unit;
		  if(tmp <= 0 ) break;
		  
		  if(tmp != sline)
		    {
		      iflistbox_vscroll_update_selection(widget, sline, tmp, smode);
		      if(smode == 0)
			{
			  int sline = EZ_IFListBoxSelectionIDX(widget); 
			  int col =  EZ_IFListBoxSelectionCol(widget); 
			  iflistbox_hscroll_update_selection(widget, sline,col, col, 0);
			}
		      EZ_HandleMotionCallBacks(widget,EZ_IFListBoxMotionCallBack(widget));
		      EZ_IFListBoxSetSelection(widget);
		    }
		}
	      break;
	      case XK_Down: case XK_n: case XK_j: case XK_N: case XK_J:  
#ifdef XK_KP_Down
	      case XK_KP_Down:
#endif
		{
		  int sline = EZ_IFListBoxSelectionIDX(widget);
		  int tmp = sline + move_unit;
		  int nrows = EZ_IFListBoxRows(widget);
		  if(tmp >= nrows) break;
		  
		  if(tmp != sline)
		    {
		      iflistbox_vscroll_update_selection(widget, sline, tmp, smode);
		      if(smode == 0)
			{
			  int sline = EZ_IFListBoxSelectionIDX(widget); 
			  int col =  EZ_IFListBoxSelectionCol(widget); 
			  iflistbox_hscroll_update_selection(widget, sline,col, col, 0);
			}
		      EZ_HandleMotionCallBacks(widget,EZ_IFListBoxMotionCallBack(widget));
		      EZ_IFListBoxSetSelection(widget); 
		    }
		}
	      break;
	      case XK_Left: case XK_b:  case XK_h: case XK_B:  case XK_H:  
#ifdef XK_KP_Left
	      case XK_KP_Left:
#endif
		{
		  if(smode)
		    {
		      if(EZ_IFListBoxXSize(widget) > EZ_IFListBoxXPageSize(widget))
			{
			  int aaa = (move_unit << 3);
			  int itmp = EZ_IFListBoxXStart(widget) + aaa;
			  if(itmp > 0) itmp = 0;
			  
			  if(EZ_IFListBoxXStart(widget) != itmp)
			    {
			      EZ_IFListBoxXStart(widget) = itmp;
			      if(EZ_WidgetMapped(widget)) EZ_DrawWidget(widget);
			    }
			}
		    }
		  else
		    {
		      int sline = EZ_IFListBoxSelectionIDX(widget); 
		      int col =  EZ_IFListBoxSelectionCol(widget); 
		      int itmp = col -1;
		      itmp = itmp >= 0 ? itmp : 0;
	      
		      if(col != itmp && sline >= 0)
			{
			  iflistbox_hscroll_update_selection(widget, sline,col, itmp, -1); 
			  EZ_HandleMotionCallBacks(widget,EZ_IFListBoxMotionCallBack(widget));
			  EZ_IFListBoxSetSelection(widget);
			}
		    }
		}
	      break;
	      case XK_Right: case XK_f: case XK_l: case XK_F: case XK_L:  
#ifdef XK_KP_Right
	      case XK_KP_Right: 
#endif
		{
		  if(smode)
		    {
		      if(EZ_IFListBoxXSize(widget) > EZ_IFListBoxXPageSize(widget))
			{
			  int bw = EZ_WidgetBorderWidth(widget);
			  int w = EZ_WidgetWidth(widget) - (bw << 1);
			  int tw = EZ_IFListBoxXSize(widget);
			  int aaa = -(move_unit << 3);
			  int itmp = EZ_IFListBoxXStart(widget) + aaa;
			  if(itmp < w - tw) itmp = w -tw;
		  
			  if(EZ_IFListBoxXStart(widget) != itmp)
			    {
			      EZ_IFListBoxXStart(widget) = itmp;
			      if(EZ_WidgetMapped(widget)) EZ_DrawWidget(widget);
			    }
			}
		    }
		  else
		    {
		      int sline = EZ_IFListBoxSelectionIDX(widget); 
		      int col =  EZ_IFListBoxSelectionCol(widget); 
		      int columns = EZ_IFListBoxColumns(widget) -1;
		      int itmp = col +1;
		      itmp = itmp <= columns ? itmp : columns;

		      if(col != itmp && sline > 0)
			{
			  iflistbox_hscroll_update_selection(widget, sline, col, itmp, 1); 	      
			  EZ_HandleMotionCallBacks(widget,EZ_IFListBoxMotionCallBack(widget));
			  EZ_IFListBoxSetSelection(widget);
			}

		    }
		}
	      break;
	      case XK_Return: case XK_Linefeed: case XK_space: 
#ifdef XK_KP_Enter
	      case XK_KP_Enter:
#endif
		{ 
		  EZ_Item *item;
		  int row = EZ_IFListBoxSelectionIDX(widget);
		  int ok = 0;
		  if(smode == 0)
		    {
		      int col = EZ_IFListBoxSelectionCol(widget);
		      if(row > 0 && col >= 0)
			{
			  EZ_Item ***data = EZ_IFListBoxData(widget);
			  item = data[row][col];
			  if(item){ EZ_HandleItemCallBacks(item); ok = 1;}
			}
		    }
		  else
		    {
		      EZ_Item *item, **items = EZ_IFListBoxData(widget)[row];
		      int i, ncols =  EZ_IFListBoxColumns(widget);
		      for(i = 0; i < ncols; i++)
			{
			  item = items[i];
			  if(item) {EZ_HandleItemCallBacks(item); ok = 1;}
			}
		    }
		  /* exec the call backs */
		  if(ok)
		  {
		    /* EZ_Widget *flistbox = EZ_IFListBoxFListBox(widget); */
		    EZ_ExecuteWidgetCallBacks(widget);
		    /* if(flistbox) {EZ_ExecuteWidgetCallBacks(flistbox);} */
		  }
		}
	      break;
	      default:
		break;
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
/***********************************************************************************/
static int find_Pointer_Item_linear();
int EZ_FindPointerOnRowIFL(array, nrows, ncolumns,x, y,Rx,Ry,Rw,Rh)
     EZ_Item ***array;
     int nrows, ncolumns;
     int x,y;
     int Rx,Ry,Rw,Rh;
{
  EZ_Item ***ptr, **row, *item;
  int len, len1, find = -1;
  int idx, j, y0=0, ck = 0;

  if(y <= Ry || y >= Ry + Rh) 
    {return (-1); }

  /* do a binary search */
  len = nrows/2;
  len1 = nrows - len;

  idx = len;
  ptr = array;

  while(1)
    {
      row = ptr[idx];
      item = row[0];

      if(item)
	{
	  y0 = EZ_ItemOriginY(item);
	}
      else
	{
	  for(j = 1; j < ncolumns; j++)
	    {
	      item = row[j];
	      if(item) 
		{ 
		  y0 = EZ_ItemOriginY(item);
		  break;
		}
	    }
	}
      if(item)
	{
	  int h = EZ_ItemAHeight(item);
	  if(y < y0)
	    {
	      int itmp = len >> 1;
	      len1 = len - itmp; 
	      len = itmp;
	      idx -= len1;
	    }
	  else if(y > y0+h)
	    {
	      len = len1 >> 1;
	      len1 = len1 - len; 
	      if(len) idx += len;
	      else idx += len1;
	    }
	  else
	    {
	      if( y >= y0 && y <= y0+h) { find = idx; break; }
	    }
	}
      else  /* an empty row: fails, do a linear search */
	{
	  return(find_Pointer_Item_linear(array, nrows, ncolumns,x, y,Rx,Ry,Rw,Rh));
	  break;
	}
      if(len1 == 0) {find = idx; ck = 1; break;}
      if(idx >= nrows) break;
    }
  if(find == 0) find = -1;
  else if(ck)
    {
      row = ptr[find];
      if((item = row[0]) == NULL)
	{
	  for(j = 1; j < ncolumns; j++)
	    {
	      item = row[j];
	      if(item) break;
	    }
	}
      if(item)
	{
	  int h = EZ_ItemAHeight(item);
	  int y0 = EZ_ItemOriginY(item);
	  if( y < y0 || y > y0+h) { find = -1;}
	}
    }
  return(find);
}

static int find_Pointer_Item_linear(array, nrows, ncolumns,x, y,Rx,Ry,Rw,Rh)
     EZ_Item ***array;
     int nrows, ncolumns;
     int x,y;
     int Rx,Ry,Rw,Rh;
{
  EZ_Item ***ptr, **row, *item;
  int i,j, y0=0;

  if(y <= Ry ) y = Ry+1;
  ptr = array;
  for(i = 1; i < nrows;  i++)
    {
      row = ptr[i];
      item = NULL;

      for(j = 0; j < ncolumns; j++)
	{
	  item = row[j];
	  if(item) break;
	}
      if(item)
	{
	  int h = EZ_ItemAHeight(item);
	  y0 = EZ_ItemOriginY(item);
	  if( y >= y0 && y <= y0+h) return(i);
	} 
    }
  return(-1);
}

/***********************************************************************************/

void EZ_FindPointerOnRowAndColIFL(array, nrows, ncolumns,x, y, rowN, colN,
				Rx,Ry,Rw,Rh)
     EZ_Item ***array;
     int nrows, ncolumns;
     int x,y,*rowN, *colN;
     int Rx,Ry,Rw,Rh;
{
  int rowNumber;
  int find =  -1;

  if(y <= Ry ) { *rowN = -1; *colN =  -1; return; }
  rowNumber = EZ_FindPointerOnRowIFL(array, nrows, ncolumns,x, y, Rx,Ry,Rw,Rh);
  
  if(rowNumber > 0)
    {
      EZ_Item **row, *item;
      int i;

      row = array[rowNumber];
      for(i = 0; i  < ncolumns; i++)
	{
	  item = row[i];
	  if(item)
	    {
	      int x0 = EZ_ItemOriginX(item);
	      int w =  EZ_ItemAWidth(item);
	      if( x >= x0 && x <= x0 + w) 
		{
		  find = i;
		  break;
		}
	    }
	}
    }
  *rowN = rowNumber;
  *colN = find;
}
/**************************************************************************/
static void IFTimerCallBack(timer, cdata)
     EZ_Timer *timer; 
     void *cdata;
{
  EZ_Widget *widget = (EZ_Widget *)cdata;
  if(widget && EZ_LookupWidgetFromAllHT(widget) == widget &&
     EZ_WidgetType(widget) == EZ_WIDGET_IFANCY_LIST_BOX)
    {
      int  hmode = EZ_IFListBoxHighlightMode(widget) & 1;
      if(hmode == 0 && EZ_WidgetWindow(widget) && EZ_WidgetMapped(widget) &&
	 EZ_IFListBoxRows(widget) > 0)
	{
	  int smode = EZ_IFListBoxSelectionType(widget);
	  int yoffset = EZ_WidgetBorderWidth(widget) + EZ_WidgetPadB(widget);
	  int *vsa = EZ_IFListBoxVSizes(widget);
	  yoffset += (vsa[0] + EZ_WidgetPadY(widget));
	  
	  if(smode)
	    {
	      int sline = EZ_IFListBoxSelectionIDX(widget);
              if(sline > 0)
                {
                  EZ_Item **row = (EZ_IFListBoxData(widget))[sline];
		  int nelts = EZ_IFListBoxColumns(widget);
		  EZ_FlashRowOutline(widget, row, nelts);
		}
	    }
	  else if( ((int)EZ_IFListBoxHighlightMode(widget) & 1) == 0)
	    {
	      int sline = EZ_IFListBoxSelectionIDX(widget);
	      int scol =  EZ_IFListBoxSelectionCol(widget);
	      if(sline > 0 && scol >= 0)
		{
		  EZ_Item ***data = EZ_IFListBoxData(widget);
		  EZ_Item *sel = data[sline][scol];
		  if(sel)
		    {
		      int Rx = EZ_WidgetBorderWidth(widget)+1+ EZ_WidgetPadB(widget);
		      int Ry = yoffset;
		      int Rw = EZ_WidgetWidth(widget) - (Rx+Rx);
		      int Rh = EZ_WidgetHeight(widget) -(Rx+Ry);

		      EZ_FlashItemOutline(widget,sel, Rx,Ry,Rw,Rh);
		    }
		}
	    }
	}
    }
  else EZ_CancelTimer(timer);
}
/***********************************************************************************************/     
void EZ_FlashRowOutline(widget, row, nelts)
     EZ_Widget *widget;
     EZ_Item **row;
     int nelts;
{
  if(row)
    {
      int yy=0, hh=0;
      int tw = 0, tx = -999999;
      EZ_Item *item;
      int i, ncols = nelts;

      for(i = 0; i< ncols; i++)
	{
	  item = row[i];
	  if(item)
	    {
	      if(hh == 0)
		{
		  yy = EZ_ItemOriginY(item);
		  hh = EZ_ItemAHeight(item);
		}
	      if(tx == -999999) tx = EZ_ItemOriginX(item);
	      tw =  EZ_ItemOriginX(item) + EZ_ItemAWidth(item);
	    }
	}
      if(hh > 1) /* we are ok */
	{
	  int *vsa = EZ_IFListBoxVSizes(widget);
	  int bw = EZ_WidgetBorderWidth(widget) + EZ_WidgetPadB(widget);
	  int x0 = bw;
	  int y0 = yy;
	  int top, bottom;
	  int width =  EZ_WidgetWidth(widget) - bw;
	  int ww;
	  XPoint points[2];
	  Window win = EZ_WidgetWindow(widget);
	  GC gc;
	  EZ_GetMovingDashedLineGC(&gc, 0);

	  if(tx > bw) x0 = tx;
	  if(tw < width ) { ww = tw;}
	  else ww = width;
	  ww -= (x0+1);
	  hh -= 1;
	  if(vsa[0] > 0) top = bw + vsa[0] + EZ_WidgetPadY(widget);
	  else top = bw;
	  bottom = EZ_WidgetHeight(widget) - bw;
	  if(tx > bw)
	    {
	      int tmp = y0+hh;
	      points[0].x = x0;     points[0].y = y0;
	      points[1].x = x0;   points[1].y = (tmp < bottom? tmp: bottom);
	      XDrawLines(EZ_Display, win, gc, points, 2, CoordModeOrigin);
	    }
	  if(yy >= top && yy < bottom)
	    {
	      points[0].x = x0;     points[0].y = y0;
	      points[1].x = x0+ww;   points[1].y = y0;
	      XDrawLines(EZ_Display, win, gc, points, 2, CoordModeOrigin);
	    }
	  if(yy + hh >= top && yy +hh < bottom)
	    {
	      points[0].x = x0+ww;     points[0].y = y0 + hh;
	      points[1].x = x0;   points[1].y = y0 + hh;
	      XDrawLines(EZ_Display, win, gc, points, 2, CoordModeOrigin);
	    }
	  if(tw < width)
	    {
	      int tmp = y0 +hh;
	      points[0].x = x0+ww;     points[0].y = y0;
	      points[1].x = x0+ww;   points[1].y = (tmp < bottom? tmp: bottom);
	      XDrawLines(EZ_Display, win, gc, points, 2, CoordModeOrigin);	      
	    }
	}
    }
}
/***********************************************************************************************/     
static void   iflistbox_vscroll_update_selection(widget, sline, nsline, smode)
     EZ_Widget *widget;
     int sline, nsline, smode;
{
  EZ_Item ***data = EZ_IFListBoxData(widget);
  EZ_Item **row, *item;
  int y0=0, h0=0;
  int redraw = 0;
  int mapped = EZ_WidgetMapped(widget);
  
  row = data[nsline];
  item = row[0];
  if(item)
    {
      y0 = EZ_ItemOriginY(item);
      h0 = EZ_ItemAHeight(item);
    }
  else
    {
      int i, ncolumns = EZ_IFListBoxColumns(widget);
      for(i = 1; i < ncolumns; i++)
	{
	  item = row[i];
	  if(item)
	    {
	      y0 = EZ_ItemOriginY(item);
	      h0 = EZ_ItemAHeight(item);
	      break;
	    }
	}
    }
  if(item)  /* do the scrolling */
    {
      int yoff, bw,Ry,Rh;
      int *vsa = EZ_IFListBoxVSizes(widget);

      bw = EZ_WidgetBorderWidth(widget) + EZ_WidgetPadB(widget);
      yoff = bw + EZ_WidgetPadY(widget);
      Rh = EZ_WidgetHeight(widget) - yoff;
      Ry =  yoff + vsa[0];

      if(y0 < Ry || y0 + h0 > Rh)
	{
	  if(y0 < Ry)
	    {
	      int itmp  =  Ry - y0  + EZ_IFListBoxYStart(widget);
	      itmp = (itmp < 0 ? itmp: 0); 
	      if(itmp != EZ_IFListBoxYStart(widget))
		{
		  EZ_IFListBoxYStart(widget) = itmp;
		  redraw = 1;
		}
	    }
	  else
	    {
	      int th = Rh;
	      int dif = th - EZ_IFListBoxYSize(widget);
	      if(dif < 0)
		{
		  int itmp =  EZ_IFListBoxYStart(widget) - h0 + th - y0; 
		  itmp = (itmp > dif ? itmp: dif);
		  if(EZ_IFListBoxYStart(widget) != itmp)
		    {
		      EZ_IFListBoxYStart(widget) = itmp;
		      redraw = 1; 
		    }
		}
	    }
	}
    }


  /* */
    {
      EZ_Item **row, *item, ***data = EZ_IFListBoxData(widget);
      int bw, yoffset, Rx,Ry,Rw,Rh;
      int rubberband = EZ_IFListBoxRubberBand(widget);
      
      bw = EZ_WidgetBorderWidth(widget) + EZ_WidgetPadB(widget);
      yoffset = bw + EZ_WidgetPadY(widget);

      Rx = bw;
      Ry = yoffset + (EZ_IFListBoxVSizes(widget))[0];
      Rw = EZ_WidgetWidth(widget) - (bw << 1);
      Rh = EZ_WidgetHeight(widget) - bw - Ry;

      if(smode)
	{
	  /* clear selection */
	  if(sline > 0)
	    {
	      row =  EZ_IFListBoxData(widget)[sline];
	      item = row[0];
	      if(item && EZ_ItemHighlighted(item))  EZ_ItemHighlighted(item) = 0;
	      if(redraw == 0 && mapped)
		EZ_UpdateOneRow(widget, EZ_WidgetWindow(widget),
				row,
				EZ_IFListBoxColumns(widget),
				Rx,Ry,Rw,Rh);
	    }
	  if(rubberband && mapped) EZ_IFListBoxClearHighlight(widget, Rx,Ry,Rw,Rh,smode);
	  
	  /* make new selection */
	  {
	    unsigned long fg = EZ_IFListBoxHighlightFG(widget);
	    unsigned long bg = EZ_IFListBoxHighlightBG(widget);
	    int  hmode = EZ_IFListBoxHighlightMode(widget);
	    row =  EZ_IFListBoxData(widget)[nsline];
	    item = row[0];
	    if(item && EZ_ItemHighlighted(item) == 0)
	      EZ_ItemHighlighted(item) = 1;
	    if(redraw == 0 && mapped)
	      EZ_HighlightOneRow(widget, EZ_WidgetWindow(widget), row, 
				 EZ_IFListBoxColumns(widget),fg, bg,
				 Rx,Ry,Rw,Rh,hmode);
	  }
	}
      else /* item mode */
	{
	  int col = EZ_IFListBoxSelectionCol(widget);
	  if(sline > 0 && col >= 0)
	    {
	      row = data[sline];
	      item = row[col];
	      if(item)
		{
		  EZ_ItemHighlighted(item) = 0;
		  if(redraw == 0 && mapped)
		    EZ_UpdateOneItem(widget,EZ_WidgetWindow(widget),item, Rx,Ry,Rw,Rh);
		}
	      if(rubberband && mapped) EZ_IFListBoxClearHighlight(widget, Rx,Ry,Rw,Rh,smode);
	      row = data[nsline];
	      item = row[col];
	      if(item)
		{
		  unsigned long fg = EZ_IFListBoxHighlightFG(widget);
		  unsigned long bg = EZ_IFListBoxHighlightBG(widget);
		  int  hmode = EZ_IFListBoxHighlightMode(widget);
		  EZ_ItemHighlighted(item) = 1;
		  if(redraw == 0 && mapped)
		    EZ_HighlightOneItem(widget, EZ_WidgetWindow(widget),item, fg, bg, Rx,Ry,Rw,Rh, hmode);
		}
	    }
	}
    }
  EZ_IFListBoxSelectionIDX(widget) = nsline;
  if(redraw && mapped) EZ_DrawWidget(widget); 
}
/*****************************************************************************/
static void iflistbox_hscroll_update_selection(widget, sline, col, ncol, flag)
     EZ_Widget *widget;
     int sline, col, ncol, flag;
{
  EZ_Item ***data;
  EZ_Item **row, *item;
  int redraw = 0;
  int bw, xoff, Rw, x0, w0;
  int mapped = EZ_WidgetMapped(widget);
  int smode = EZ_IFListBoxSelectionType(widget);

  if(sline <= 0) return;

  data  = EZ_IFListBoxData(widget);
  bw = EZ_WidgetBorderWidth(widget) + EZ_WidgetPadB(widget);
  xoff = bw + EZ_WidgetPadX(widget);
  Rw = EZ_WidgetWidth(widget) - xoff;

  row = data[sline];
  item = row[ncol];

  if(item == NULL && flag != 0)
    {
      int ncolums = EZ_IFListBoxColumns(widget);
      int tmp = ncol + flag;
      while(tmp >= 0 && tmp < ncolums)
	{
	    if(row[tmp] != NULL) 
	      {
		item = row[tmp];
		ncol = tmp;
		break;
	      }
	    tmp += flag;
	}
      if(item == NULL) return;
    }

  if(item)
    {
      x0 = EZ_ItemOriginX(item);
      w0 = EZ_ItemAWidth(item);

      if(x0 < xoff  ||  x0 + w0 > Rw)
	{
	  if(x0 < xoff)
	    {
	      int itmp = EZ_IFListBoxXStart(widget)  + xoff - x0;
	      itmp = (itmp < 0? itmp: 0);
	      if(itmp != EZ_IFListBoxXStart(widget))
		{
		  EZ_IFListBoxXStart(widget) = itmp;
		  redraw = 1;
		}	      
	    }
	  else 
	    {
	      int th = EZ_IFListBoxXSize(widget);
	      int diff = Rw - th;
	      if(diff < 0)
		{
		  int itmp = EZ_IFListBoxXStart(widget) + Rw -x0 -w0;
		  itmp = (itmp > diff ? itmp: diff);
		  if(EZ_IFListBoxXSize(widget) != itmp)
		    {
		      EZ_IFListBoxXStart(widget) = itmp;
		      redraw = 1;
		    }	      
		}
	    }
	}
    }

  /*  if(redraw == 0)  */
    {
      int bw, xoffset, yoffset;
      int Rx,Ry,Rw,Rh;
      int rubberband = EZ_IFListBoxRubberBand(widget);			  

      bw = EZ_WidgetBorderWidth(widget)  + EZ_WidgetPadB(widget);
      xoffset = bw + EZ_WidgetPadX(widget);
      yoffset = bw + EZ_WidgetPadY(widget);
      
      Rx = bw;
      Ry = yoffset + (EZ_IFListBoxVSizes(widget))[0];
      Rw = EZ_WidgetWidth(widget) - (bw << 1);
      Rh = EZ_WidgetHeight(widget) - bw - Ry;


      row = data[sline];
      if(col >= 0)
	{
	  item = row[col];
	  if(item)
	    {
	      EZ_ItemHighlighted(item) = 0;
	      if(mapped)EZ_UpdateOneItem(widget,EZ_WidgetWindow(widget),item, Rx,Ry,Rw,Rh);
	    }
	}
      if(rubberband) EZ_IFListBoxClearHighlight(widget, Rx,Ry,Rw,Rh,smode);
      item = row[ncol];
      if(item)
	{
	  unsigned long fg = EZ_IFListBoxHighlightFG(widget);
	  unsigned long bg = EZ_IFListBoxHighlightBG(widget);
	  int  hmode = EZ_IFListBoxHighlightMode(widget);
	  EZ_ItemHighlighted(item) = 1;
	  if(mapped)
	    EZ_HighlightOneItem(widget, EZ_WidgetWindow(widget),item, fg, bg, Rx,Ry,Rw,Rh, hmode);
	}
    }
  EZ_IFListBoxSelectionCol(widget) = ncol;
  if(redraw && mapped) EZ_DrawWidget(widget);
}
/***************************************************************************************/
extern ITextLine TextWidgetKillBuffer;

static void EZ_IFListBoxSetSelection(widget)
     EZ_Widget *widget;
{
  int smode = EZ_IFListBoxSelectionType(widget);
  int sline = EZ_IFListBoxSelectionIDX(widget);

  if(smode)
    {
      if(sline > 0)
	{
	  EZ_Item ***data = EZ_IFListBoxData(widget);
	  EZ_Item **row = data[sline];
	  EZ_Item *item;
	  int cols = EZ_IFListBoxColumns(widget);
	  int i, total = 0;
	  for(i = 0; i < cols; i++)
	    {
	      item = row[i];
	      if(item)
		{
		  char *str; 
		  int length;
		  EZ_GetLabelItemStringInfo(item, &str, &length);
		  if(length > 0)
		    {
		      EZ_GrowITextKillBuffer(&TextWidgetKillBuffer,(total+length+8));
		      if(i>0 && total > 0)
			{
			  char *t = TextWidgetKillBuffer.data + total;
			  *t = ' ';
			  *(t+1) = 0;
			  total++;
			}
		      (void) strncpy(TextWidgetKillBuffer.data + total, str, length);
		      total += length;
		      (TextWidgetKillBuffer.data)[total] = 0;
		    }
		}
	    }
	  if(total > 0)
	    {
	      TextWidgetKillBuffer.length = total;
	      TextWidgetKillBuffer.width = 0;
	      EZ_SetSelectionOwner(widget);
	    }
	}
    }
  else
    {
      if(sline > 0)
	{
	  int col = EZ_IFListBoxSelectionCol(widget);
	  if(col >= 0)
	    {
	      EZ_Item ***data = EZ_IFListBoxData(widget);
	      EZ_Item *item = data[sline][col];
	      if(item)
		{
		  char *str;
		  int length;
		  EZ_GetLabelItemStringInfo(item, &str, &length);
		  if(length > 0)
		    {
		      EZ_GrowITextKillBuffer(&TextWidgetKillBuffer,(length+8));
		      (void) strncpy(TextWidgetKillBuffer.data, str, length);
		      (TextWidgetKillBuffer.data)[length] = 0;

		      TextWidgetKillBuffer.length = length;
		      TextWidgetKillBuffer.width = 0;
		      EZ_SetSelectionOwner(widget);
		    }
		}
	    }
	}
    }
}
/****************************************************************************/
void  EZ_IFListBoxAllocNewStorage(wptr, nrows, ncolumns)
     EZ_Widget *wptr;
     int nrows, ncolumns;
{
  EZ_Item  ***data, **row;
  int      *iptr, *jptr, i, j;
  int aval = EZ_IFListBoxSpace(wptr);
  int col = EZ_IFListBoxColumns(wptr);

  if(nrows <= 16) nrows = 32;
  /* check if current setting is ok */
  if(aval > nrows + 2 && col == ncolumns)
    { /* ok */
      int count = EZ_IFListBoxRows(wptr);
      data = EZ_IFListBoxData(wptr);
      for(i = 1; i < count; i++)
	{
	  row = data[i];
	  for(j = 0; j < ncolumns; j++)
	    {
	      if(row[j]){ EZ_DestroyItem(row[j]); row[j] = NULL;}
	    }
	}
      (EZ_IFListBoxVSizes(wptr))[0] = 0;
    }
  else
    {
      /* get rid of the old ones */
      EZ_Item **header;
      int headerH;

      data = EZ_IFListBoxData(wptr);
      if(aval > 0)
	{
	  header = data[0];
	  headerH =  (EZ_IFListBoxVSizes(wptr))[0];
	}
      else { header = NULL; headerH = 0; }
      
      for(i = 1; i < aval; i++)
	{
	  row = data[i];
	  for(j = 0; j < col; j++)
	    {
	      if(row[j]) EZ_DestroyItem(row[j]); 
	    }
	  (void)my_free((char *)row);
	}
      if(data)   (void) my_free( (char *)data);
      if(EZ_IFListBoxHSizes(wptr))
	(void) my_free( (char *) EZ_IFListBoxHSizes(wptr));
      if(EZ_IFListBoxVSizes(wptr))
	(void) my_free( (char *) EZ_IFListBoxVSizes(wptr));
      
      data = (EZ_Item ***)  my_malloc((nrows + 16) * sizeof(EZ_Item **), _IFLISTBOX_DATA_);
      iptr = (int *)my_malloc( ncolumns * sizeof(int),  _IFLISTBOX_DATA_);
      jptr = (int *)my_malloc( (nrows + 16) * sizeof(int),  _IFLISTBOX_DATA_);

      if(jptr == NULL || iptr== NULL || data == NULL)  EZ_OutOfMemory("EZ_CreateIFListBox");
      EZ_IFListBoxHSizes(wptr) = iptr;
      EZ_IFListBoxVSizes(wptr) = jptr;
      EZ_IFListBoxData(wptr) = data;
      for(i = 0; i < ncolumns; i++)   iptr[i] = 0;

      for(i = 0; i < nrows + 16; i++)
	{
	  EZ_Item **rowptr;
	  jptr[i] = 0;
	  rowptr = data[i] = (EZ_Item **)my_malloc(ncolumns * sizeof(EZ_Item *), _IFLISTBOX_DATA_);
	  if(rowptr == NULL) EZ_OutOfMemory("EZ_CreateIFListBox");
	  {
	    int j; 
	    for(j = 0; j < ncolumns; j++) rowptr[j] = (EZ_Item *)NULL;
	  }
	}
      if(header)
	{
	  int i, mcol = (ncolumns < col ? ncolumns: col);
	  row = data[0];
	  for(i = 0; i < mcol; i++) row[i] = header[i];
	  for(i = mcol; i < col; i++)  if(row[i]) EZ_DestroyItem(row[i]);
	  jptr[0] = headerH;
	  (void)my_free((char *)header);
	}
      EZ_IFListBoxSpace(wptr) = nrows + 16;
    }
  EZ_IFListBoxColumns(wptr) = ncolumns;
  EZ_IFListBoxRows(wptr) = 1;  
  EZ_IFListBoxSelectionIDX(wptr) = -1;
  EZ_IFListBoxSelectionCol(wptr) = -1;
}
/******************************************************************************************/
static void  EZ_IFListBoxGrowStorage(widget, unit)
     EZ_Widget *widget;
     int unit;
{
  int nrows = EZ_IFListBoxRows(widget);
  int aval = EZ_IFListBoxSpace(widget);

  if(nrows + unit + 4 > aval)
    {
      EZ_Item ***data = EZ_IFListBoxData(widget);
      int *vsa = EZ_IFListBoxVSizes(widget);
      int i,j, new_aval = aval + unit + 16;
      int col = EZ_IFListBoxColumns(widget);

      data = (EZ_Item ***)my_realloc((void *)data, (new_aval)*sizeof(EZ_Item **),
				     _IFLISTBOX_DATA_);
      vsa = (int *) my_realloc((void *)vsa, new_aval * sizeof(int),_IFLISTBOX_DATA_);
      if(data == NULL || vsa == NULL) EZ_OutOfMemory("EZ_IFListBoxGrowBuffer");
      for(i = aval; i < new_aval; i++)
	{
	  EZ_Item **t = (EZ_Item **)my_malloc(col * sizeof(EZ_Item *), _IFLISTBOX_DATA_);
	  if( t == NULL) EZ_OutOfMemory("EZ_IFListBoxGrowBuffer");
	  for(j = 0; j < col; j++) t[j] = (EZ_Item *)NULL;
	  data[i] = t;
	  vsa[i] = 0;
	}
      EZ_IFListBoxData(widget) = data; 
      EZ_IFListBoxVSizes(widget) = vsa;
      EZ_IFListBoxSpace(widget) = new_aval;
    }
}
/******************************************************************************************/

void EZ_SetIFListBoxData(widget, data, nrows, ncols)
     EZ_Widget *widget;
     EZ_Item   **data;
     int nrows, ncols;
{
  int i,j;
  EZ_Item ***des, **row, **src;

  EZ_IFListBoxAllocNewStorage(widget, nrows, ncols);

  src = (EZ_Item **)data;
  des = EZ_IFListBoxData(widget);
  for(i = 0; i < nrows; i++)
    {
      row = des[i+1];
      for(j = 0; j < ncols; j++)
	row[j] = *src++;
    }
  EZ_IFListBoxColumns(widget) = ncols;
  EZ_IFListBoxRows(widget) = nrows + 1;    
  EZ_IFListBoxDirty(widget) = 1;      
  EZ_IFListBoxSelectionIDX(widget) = -1;
  EZ_IFListBoxSelectionCol(widget) = -1;

  if(EZ_WidgetMapped(widget)) EZ_DrawWidget(widget);
}
/******************************************************************************************/
void  EZ_InsertIFListBoxRow(widget, items, nitems, where)
     EZ_Widget *widget;
     EZ_Item **items;
     int where, nitems;
{
  EZ_IFListBoxGrowStorage(widget, 1);
  {
    int numcols = EZ_IFListBoxColumns(widget);
    int nrows = EZ_IFListBoxRows(widget);
    int i, idx;
    int ncols = (numcols < nitems? numcols: nitems);
    EZ_Item ***data =EZ_IFListBoxData(widget);
    EZ_Item **save = data[nrows];

    if(where < 1 || where >= nrows) /* insert at the end */
      idx = nrows;
    else
      {
	int sline = EZ_IFListBoxSelectionIDX(widget);
	for(i = nrows; i > where; i--)
	  data[i] = data[i-1];
	idx = where;
	if(sline >= idx) EZ_IFListBoxSelectionIDX(widget) = sline +1;
      }
    {
      int i;
      
      for(i = 0; i < numcols; i++)
	{
	  if(i < ncols)	save[i] = items[i];
	  else save[i] = NULL;
	}
      EZ_IFListBoxData(widget)[idx] = save;
    }

    EZ_IFListBoxRows(widget) = nrows + 1;
    EZ_IFListBoxDirty(widget) = 1;
  }
  if(EZ_WidgetMapped(widget) && EZ_GetWidgetFreezedFlag(widget)==0)
    { EZ_DrawWidget(widget); }
}
/**************************************************/
void  EZ_DeleteIFListBoxRow(widget,idx)
     EZ_Widget *widget;
     int idx;
{
  int ncols = EZ_IFListBoxColumns(widget);
  int nrows = EZ_IFListBoxRows(widget);
  int i;
  EZ_Item ***data =EZ_IFListBoxData(widget);

  if(idx < 1 || idx >= nrows)  return;
  else
    {
      int sline = EZ_IFListBoxSelectionIDX(widget);
      EZ_Item **save = data[idx];
      for(i = idx; i <= nrows; i++)
	data[i] = data[i+1];
      for(i = 0; i < ncols; i++)
	if(save[i]) { EZ_DestroyItem(save[i]); save[i] = NULL; }

      /* bug: deleted row got lost in the dark. 
       * thanks to "Elliot Turner <turnere@MimeStar.com>"
       * for finding it.
       */
      data[nrows+1] = save;

      EZ_IFListBoxRows(widget) = nrows - 1;
      EZ_IFListBoxDirty(widget) = 1;
      if(sline == idx)
	EZ_IFListBoxSelectionIDX(widget) = -1;
      else  if(sline > idx)
	EZ_IFListBoxSelectionIDX(widget) -= 1;
      if(EZ_WidgetMapped(widget)) EZ_DrawWidget(widget);
    }
}
void  EZ_DeleteIFListBoxSomeRows(widget,indices, cnt)
     EZ_Widget *widget;
     int *indices, cnt;
{
  int ncols = EZ_IFListBoxColumns(widget);
  int nrows = EZ_IFListBoxRows(widget);
  int i,j, idx, sline, deleted = 0;
  EZ_Item ***save, ***data =EZ_IFListBoxData(widget);

  save  = (EZ_Item ***)my_malloc( (nrows+1) * sizeof(EZ_Item **), _MISC_DATA_);

  sline = EZ_IFListBoxSelectionIDX(widget);
  for(i = 0; i < cnt; i++)
    {
      idx = indices[i];
      if(idx >= 1 && idx < nrows && data[idx] != NULL)
	{
	  EZ_Item **tmp = data[idx];
	  save[deleted++] = tmp;
	  data[idx] = NULL;
	  for(j = 0; j < ncols; j++)
	    if(tmp[j]) { EZ_DestroyItem(tmp[j]); tmp[j] = NULL; }
	  if(sline > 0)
	    {
	      if(sline == idx) sline = -1;
	      else if(sline > idx) sline -= 1;
	    }
	}
    }
  if(deleted)
    {
      int i, j, k;
      
      EZ_IFListBoxDirty(widget) = 1;
      i=1; j = 1;
      while(j < nrows)
	{
	  while(data[i] != NULL) i++;
	  j = (j > i? j: i);
	  while( j < nrows && data[j] == NULL) j++;	  
	  if(j < nrows)
	    {
	      data[i] = data[j];
	      data[j] = NULL;
	      j++;
	      i++;
	    }
	}
      
      for(k = 0, j = i; j <= nrows && k < deleted;i++)
	{
	  if(data[i] == NULL)
	    data[i] = save[k++];
	}
      k = nrows - deleted;
      EZ_IFListBoxRows(widget) = k;
      EZ_IFListBoxSelectionIDX(widget) = sline;
      if(EZ_WidgetMapped(widget)) EZ_DrawWidget(widget);
    }
  (void)my_free((char *)save);
}
/******************************************************************************************/
void  EZ_ClearIFListBox(widget)
     EZ_Widget *widget;
{
  EZ_Item  ***data, **row;
  int cols = EZ_IFListBoxColumns(widget);
  int i, j, nrows = EZ_IFListBoxRows(widget);

  data = EZ_IFListBoxData(widget);
  for(i = 1; i < nrows; i++)
    {
      row = data[i];
      for(j = 0; j < cols; j++)
	{
	  if(row[j]){ EZ_DestroyItem(row[j]); row[j] = NULL;}
	}
    }
  EZ_IFListBoxRows(widget) = 1;
  EZ_IFListBoxDirty(widget) = 1;      
  EZ_IFListBoxSelectionIDX(widget) = -1;
  EZ_IFListBoxSelectionCol(widget) = -1;
  EZ_IFListBoxXStart(widget) = 0;
  EZ_IFListBoxYStart(widget) = 0;
  if(EZ_WidgetMapped(widget)) EZ_DisplayWidget(widget);
}

/******************************************************************************************/
EZ_Item  **EZ_GetIFListBoxSelection(widget)
     EZ_Widget *widget;
{
  int smode = EZ_IFListBoxSelectionType(widget);

  if(smode)
    {
      int sline = EZ_IFListBoxSelectionIDX(widget);
      if(sline > 0)
	{
	  EZ_Item ***data = EZ_IFListBoxData(widget);
	  return(data[sline]);
	}
    }
  else
    {
      int sline = EZ_IFListBoxSelectionIDX(widget);
      if(sline > 0)
	{
	  int col = EZ_IFListBoxSelectionCol(widget);
	  if(col >= 0)
	    {
	      EZ_Item ***data = EZ_IFListBoxData(widget);	      
	      EZ_Item **row = data[sline];
	      return(row + col);
	    }
	}
    }
  return(NULL);
}
/******************************************************************************************/
void EZ_SetIFListBoxHeader(widget, items, nitems)
     EZ_Widget *widget;
     EZ_Item **items;
     int      nitems;
{
  int i, col, ncol = EZ_IFListBoxColumns(widget);
  EZ_Item **data = EZ_IFListBoxData(widget)[0];
  
  col = (ncol < nitems? ncol: nitems);
  for(i = 0; i < col; i++)
    {
      EZ_Item *item = data[i];
      if(item) EZ_DestroyItem(item);
      data[i] = (items == NULL ? NULL : items[i]);
    }
  EZ_IFListBoxDirty(widget) = 1;
  if(EZ_WidgetMapped(widget)) EZ_DrawWidget(widget);
}
/******************************************************************************************/
void EZ_DnDOutlineRow(widget, row, nelts, cx,cy,cw,ch)
     EZ_Widget *widget;
     EZ_Item **row;
     int nelts;
     int cx,cy,cw,ch;
{
  if(row && widget)
    {
      EZ_Item *item;
      int i, h, x0,y0=0;

      h = 0;
      for(i = 0 ; i < nelts; i++)
	{
	  item = row[i];
	  if(item)
	    {
	      h = EZ_ItemAHeight(item);
	      y0 = EZ_ItemOriginY(item);
	      break;
	    }
	}
      if(h > 0)
	{
	  int x_r,y_r,w_r,h_r;
	  
	  x0 = 0;
	  EZ_IntersectRectangles(cx,cy,cw,ch, 
				 x0,y0,999999,h,
				 &x_r,&y_r, &w_r, &h_r);
	  if(w_r > 0)
	    {
	      XPoint points[5];
	      Window win = EZ_WidgetWindow(widget);
	      GC gc = EZ_DNDXORGC; 
	      
	      points[0].x = x_r;       points[0].y = y_r;
	      points[1].x = x_r+w_r;   points[1].y = y_r;
	      points[2].x = x_r+w_r;   points[2].y = y_r+h_r;
	      points[3].x = x_r;       points[3].y = y_r+h_r;
	      points[4].x = x_r;       points[4].y = y_r;
	      XDrawLines(EZ_Display, win, gc, points, 5, CoordModeOrigin);
	    }
	}
    }
}
/*********************************************************************/
void  EZ_GetIFListBoxSelectionIdx(widget, row_ret, col_ret)
     EZ_Widget *widget;
     int *row_ret, *col_ret;
{
  if(widget)
    {
      int smode = EZ_IFListBoxSelectionType(widget);
      int sline =  EZ_IFListBoxSelectionIDX(widget); 
      int nrows = EZ_IFListBoxRows(widget); 
      if(smode)
	{
	  if(sline > 0 && sline < nrows)
	    {
	      if(row_ret) *row_ret = sline;
	      if(col_ret) *col_ret = 0;
	      return;
	    }
	}
      else
	{
	  if(sline > 0 && sline < nrows)
	    {
	      int col = EZ_IFListBoxSelectionCol(widget);
	      if(col >= 0 && col < (int) EZ_IFListBoxColumns(widget))
		{
		  if(row_ret) *row_ret = sline;
		  if(col_ret) *col_ret = col;		  
		  return;
		}
	    }
	}
    }
  if(row_ret) *row_ret = -1;
  if(col_ret) *col_ret = -1;		    
}
/***********************************************************/	      
void EZ_ReplaceIFListBoxItem(widget, olditem, newitem)
     EZ_Widget *widget;
     EZ_Item *olditem, *newitem;
{
  if(widget)
    {
      int nrows = EZ_IFListBoxRows(widget); 
      int ncols = EZ_IFListBoxColumns(widget);
      EZ_Item ***data = EZ_IFListBoxData(widget);
      int i,j;
      for(i = 0; i < nrows; i++)
	{
	  EZ_Item **row = data[i];
	  for(j = 0; j < ncols; j++)
	    {
	      if(row[j] == olditem)
		{
		  if(olditem) EZ_DestroyItem(olditem);
		  row[j] = newitem;
		  EZ_IFListBoxDirty(widget) = 1;
		  if(EZ_WidgetMapped(widget)) EZ_DrawWidget(widget);
		  return;
		}
	    }
	}
    }
}
/***********************************************************/	      
void EZ_ReplaceIFListBoxItemUsingIdx(widget, rowI, colI, newitem)
     EZ_Widget *widget;
     int rowI, colI;
     EZ_Item *newitem;
{
  if(widget)
    {
      int nrows = EZ_IFListBoxRows(widget); 
      int ncols = EZ_IFListBoxColumns(widget);
      EZ_Item ***data = EZ_IFListBoxData(widget);
      if(rowI > 0 && rowI < nrows && colI >= 0  && colI < ncols)
	{
	  EZ_Item **row = data[rowI];
	  if(row[colI]) EZ_DestroyItem(row[colI]);
	  row[colI] = newitem;
	  EZ_IFListBoxDirty(widget) = 1;
	  if(EZ_WidgetMapped(widget)) EZ_DrawWidget(widget);
	}
    }
}
/***********************************************************/	      
EZ_Item  *EZ_GetIFListBoxItemUsingIdx(widget, rowI, colI)
     EZ_Widget *widget;
     int rowI, colI;
{
  if(widget)
    {
      int nrows = EZ_IFListBoxRows(widget); 
      int ncols = EZ_IFListBoxColumns(widget);
      EZ_Item ***data = EZ_IFListBoxData(widget);
      if(rowI > 0 && rowI < nrows && colI >= 0  && colI < ncols)
	{
	  EZ_Item **row = data[rowI];
	  return(row[colI]);
	}
    }
  return(NULL);
}
/*************************************************************/
static void EZ_IFListBoxFindItem(widget, item, loc)
     EZ_Widget *widget; EZ_Item *item; int *loc;
{
  if(item == NULL) { *loc = -1; return;}
  if(widget)
    {
      int i, j, nrows = EZ_IFListBoxRows(widget); 
      int ncols = EZ_IFListBoxColumns(widget);
      EZ_Item ***data = EZ_IFListBoxData(widget);
      for(i = 0; i < nrows; i++)
	{
	  EZ_Item **row = data[i];
	  for(j = 0; j < ncols; j++)
	    if(row[j] == item) { loc[0] = i; loc[1] = j; return;}
	}
    }
  *loc = -1;
}
/*************************************************************/
void EZ_IFListBoxSelectItem(widget, item, location)
     EZ_Widget *widget;
     EZ_Item *item;
     int *location;
{
  if(widget)
    {
      int ridx = -1, cidx = 0;
      if(item != NULL)
	{
	  int pos[2];  pos[0] = -1;
	  EZ_IFListBoxFindItem(widget, item, pos);
	  ridx = pos[0]; cidx = pos[1];
	}
      EZ_IFListBoxSelectItemUsingIdx(widget, ridx, cidx, location);
    }
}
/*************************************************************/

void EZ_IFListBoxSelectItemUsingIdx(widget, Srow, Scol, location)
     EZ_Widget *widget;
     int Srow, Scol;
     int *location;
{
  if(widget)
    {
      int bw, xoffset, yoffset, ww,hh,Rx,Ry,Rw,Rh, *vsa;
      int rubberband = EZ_IFListBoxRubberBand(widget);
      int smode = EZ_IFListBoxSelectionType(widget);
      int mapped = EZ_WidgetMapped(widget);
      EZ_Item *item, **row, ***data = EZ_IFListBoxData(widget);

      vsa = EZ_IFListBoxVSizes(widget);
      bw = EZ_WidgetBorderWidth(widget) + EZ_WidgetPadB(widget);
      xoffset = bw + EZ_WidgetPadX(widget);
      yoffset = bw + EZ_WidgetPadY(widget);
      Rx = bw;
      Ry = yoffset + vsa[0];
      ww = EZ_WidgetWidth(widget);
      Rw = ww - (bw << 1);
      hh = EZ_WidgetHeight(widget);
      Rh = hh - bw - Ry;

      /* first, remove current highlight and deselect */
      if(rubberband == 0)
	{
	  int sline = EZ_IFListBoxSelectionIDX(widget); 
	  int scol = EZ_IFListBoxSelectionCol(widget);
	  if(sline >= 0)
	    {
	      if(smode) /* row mode */
		{
		  row = data[sline];
		  item = row[0];
		  if(item && EZ_ItemHighlighted(item))
		    {
		      EZ_ItemHighlighted(item) = 0;
		      if(mapped) EZ_UpdateOneRow(widget, EZ_WidgetWindow(widget),
						 row, EZ_IFListBoxColumns(widget),
						 Rx,Ry,Rw,Rh);
		    }
		}
	      else /* item mode */
		{
		  row = data[sline];
		  item = row[scol];
		  if(item && EZ_ItemHighlighted(item))
		    {
		      EZ_ItemHighlighted(item) = 0;
		      if(mapped) EZ_UpdateOneItem(widget,EZ_WidgetWindow(widget),item, Rx,Ry,Rw,Rh);
		    }
		}
	    }
	}
      else  /* rubberband */
        {
	  EZ_IFListBoxClearHighlight(widget, Rx,Ry,Rw,Rh,smode);
	}
      EZ_IFListBoxSelectionIDX(widget) = -1;

      /* select new item */
      if(Srow > 0)
	{
	  int row = Srow;
	  int col = Scol;
	  int  nrows = EZ_IFListBoxRows(widget); 
	  int  ncols = EZ_IFListBoxColumns(widget);
	  int doit = 0;
	  
	  doit = (row > 0 && row < nrows);
	  if(doit && smode == 0) doit = (col >= 0 && col < ncols);
	  
	  if(doit)
	    {
	      int osline = EZ_IFListBoxSelectionIDX(widget);
	      int ocol =  EZ_IFListBoxSelectionCol(widget); 
	      EZ_Item ***data = EZ_IFListBoxData(widget);  
	      EZ_Item **tmprow = data[row];
	      EZ_Item *item = tmprow[col];

	      EZ_IFListBoxSelectionIDX(widget) = row;
	      if(smode == 0)
		{
		  if(item) EZ_ItemHighlighted(item) = 1;
		  EZ_IFListBoxSelectionCol(widget) = col;
		}
	      else
		{
		  EZ_Item *titem = tmprow[0];
		  if(titem) EZ_ItemHighlighted(titem) = 1;
		}
	      if(EZ_WidgetMapped(widget))
		{
		  if(location)
		    {
		      int x = EZ_ItemOriginX(item);
		      int y = EZ_ItemOriginY(item);
		      EZ_IFListBoxXStart(widget) += location[0] - x;
		      EZ_IFListBoxYStart(widget) += location[1] - y;
		      EZ_DrawWidget(widget);
		      iflistbox_vscroll_update_selection(widget, row, row, smode);
		      if(smode == 0) iflistbox_hscroll_update_selection(widget, row, col, col, 0);
		      EZ_HandleMotionCallBacks(widget,EZ_IFListBoxMotionCallBack(widget));
		    }
		  else
		    {
		      if(smode)
			{
			  if(row != osline)
			    {
			      iflistbox_vscroll_update_selection(widget, osline, row, smode);
			      EZ_HandleMotionCallBacks(widget,EZ_IFListBoxMotionCallBack(widget));
			      /* EZ_IFListBoxSetSelection(widget);*/
			    }
			}
		      else
			{
			  if( (row != osline || col != ocol))
			    {
			      iflistbox_vscroll_update_selection(widget, osline, row, smode);
			      iflistbox_hscroll_update_selection(widget, row, ocol, col, 0);
			      EZ_HandleMotionCallBacks(widget,EZ_IFListBoxMotionCallBack(widget));
			      /* EZ_IFListBoxSetSelection(widget);*/
			    }
			}
		    }
		}
	    }
	}
    }
}
/*************************************************************
 *
 * handle button Event in ROW mode
 */
void  EZ_IFListBoxHandleBtnEventA(widget, event) EZ_Widget *widget; XEvent *event;
{
  XEvent    xevent;
  int       x, y, bw, Rx,Ry,Rw,Rh, xoffset, yoffset, done,  smode;
  int       moved = 0;
  int       sline;
  int       *vsa,ww,hh;
  EZ_Item   **nsitems = NULL, *nsitem = NULL;


  int       rubberband, hmode;
  unsigned long fg = EZ_IFListBoxHighlightFG(widget);
  unsigned long bg = EZ_IFListBoxHighlightBG(widget);

  /* first, get focus if we don;t have it */
  rubberband = EZ_IFListBoxRubberBand(widget);
  hmode = EZ_IFListBoxHighlightMode(widget);
  smode = EZ_IFListBoxSelectionType(widget);
  vsa = EZ_IFListBoxVSizes(widget);

  if(event->xbutton.button == EZ_Btn1) 	  /* get keyboard focus */
    { 
      Window fw; int rt;
      EZ_Widget *tmp;
      XGetInputFocus(EZ_Display, &fw, &rt);
      tmp = EZ_LookupWidgetFromMappedHT(fw);
      if(tmp != widget) EZ_SetFocusTo(widget);
    }
  if(EZ_IFListBoxRows(widget) <= 0) {return;}
  if(event->xbutton.button == EZ_Btn1)
    {
      Time  time_stamp = event->xbutton.time;   
      int   elapsedtime = EZ_LastButton1ClickTime - time_stamp; 
      EZ_LastButton1ClickTime = time_stamp;

      bw = EZ_WidgetBorderWidth(widget) + EZ_WidgetPadB(widget);
      xoffset = bw + EZ_WidgetPadX(widget);
      yoffset = bw + EZ_WidgetPadY(widget);

      Rx = bw;
      Ry = yoffset + vsa[0];
      ww = EZ_WidgetWidth(widget);
      Rw = ww - (bw << 1);
      hh = EZ_WidgetHeight(widget);
      Rh = hh - bw - Ry;

      x = event->xbutton.x;
      y = event->xbutton.y;
      if(rubberband == 0)   /* rubberband disabled */
	{
	  int scol = -1;
	  sline = -1;
	  EZ_FindPointerOnRowAndColIFL(EZ_IFListBoxData(widget), 
				       EZ_IFListBoxRows(widget), 
				       EZ_IFListBoxColumns(widget), 
				       x,y, &sline, &scol,
				       Rx,Ry,Rw,Rh);
	  
	  if(sline >=0 && sline != EZ_IFListBoxSelectionIDX(widget))
	    {
	      int tline =  EZ_IFListBoxSelectionIDX(widget); 
	      if(tline > 0)
		{
		  EZ_Item **row = EZ_IFListBoxData(widget)[tline];
		  EZ_Item *item = row[0];
		  if(item && EZ_ItemHighlighted(item)) EZ_ItemHighlighted(item) = 0;
		  EZ_UpdateOneRow(widget, EZ_WidgetWindow(widget), row,
				  
				  EZ_IFListBoxColumns(widget),
				  Rx,Ry,Rw,Rh);
		}
	      EZ_IFListBoxSelectionIDX(widget) = sline;
	      {
		EZ_Item **row =  EZ_IFListBoxData(widget)[sline];
		EZ_Item *item = row[0];
		if(item && EZ_ItemHighlighted(item)== 0) EZ_ItemHighlighted(item) = 1;
		EZ_HighlightOneRow(widget, EZ_WidgetWindow(widget), row, 
				   EZ_IFListBoxColumns(widget),fg, bg,
				   Rx,Ry,Rw,Rh,hmode);
	      }
	      EZ_HandleMotionCallBacks(widget,EZ_IFListBoxMotionCallBack(widget));
	      EZ_IFListBoxSetSelection(widget);
	    }
	  /*
	   * a double click cause the execuation of the call back function
	   */
	  if(sline >= 0 && ABSV(elapsedtime) < DOUBLE_CLICK_TIME_INTERVAL)
	    { 
	      EZ_Item *item, **items = EZ_IFListBoxData(widget)[sline];
	      int i, ncols =  EZ_IFListBoxColumns(widget);
	      for(i = 0; i < ncols; i++)
		{
		  item = items[i];
		  if(item) {EZ_HandleItemCallBacks(item);}
		}
	      EZ_ExecuteWidgetCallBacks(widget);
	      return;
	    }

	  if(sline > 0)
	    {
	      int i, ncols =  EZ_IFListBoxColumns(widget);
	      nsitems = EZ_IFListBoxData(widget)[sline];
	      for(i = 0; i < ncols; i++)
		{
		  nsitem = nsitems[i];
		  if(nsitem) break;
		}
	    }
	  if(nsitem)
	    {
	      int save_row = -1;
	      done = 0;
	      moved = 0;
	      while(!done)
		{
		  do {
		    XNextEvent(EZ_Display, &xevent); 	      
		    if(EZ_FilterEvent(&xevent))  EZ_InvokePrivateEventHandler(&xevent);
		    if(xevent.type == MotionNotify)
		      {
			moved = 1; 
			x = xevent.xmotion.x;
			y = xevent.xmotion.y;
		      }
		    else if(xevent.type == ButtonRelease)
		      {
			if(xevent.xbutton.button == EZ_Btn1) { done = 1; break;}
		      }
		    else if(xevent.type == Expose || xevent.type == FocusIn || xevent.type == FocusOut)
		      EZ_WidgetDispatchEvent(&xevent);
		  } while(XPending(EZ_Display) && !done) ;
		      
		  if(!done)
		    {
		      if(moved)
			{ 
			  int tmp = EZ_FindPointerOnRowIFL(EZ_IFListBoxData(widget), 
							   EZ_IFListBoxRows(widget), 
							   EZ_IFListBoxColumns(widget), 
							   x,y, Rx,Ry,Rw,Rh);
			  if(tmp != save_row && tmp != EZ_IFListBoxSelectionIDX(widget))
			    {
			      if(save_row >= 0)
				EZ_UpdateOneRow(widget, EZ_WidgetWindow(widget),
						EZ_IFListBoxData(widget)[save_row],
						(int)EZ_IFListBoxColumns(widget),
						Rx,Ry,Rw,Rh);
			      if(tmp >= 0) EZ_FlashRowOutline(widget, 
							      EZ_IFListBoxData(widget)[tmp],
							      (int)EZ_IFListBoxColumns(widget));
			      save_row = tmp;
			    }
			}
		    }
		  else /* we are done */
		    {
		      if(save_row >= 0) /* cannot be the current selection */
			{
			  int tline = EZ_IFListBoxSelectionIDX(widget);
			  if(tline > 0)
			    {
			      EZ_Item **row = EZ_IFListBoxData(widget)[tline];
			      EZ_Item *item = row[0];
			      if(item && EZ_ItemHighlighted(item)) EZ_ItemHighlighted(item) = 0;
			      EZ_UpdateOneRow(widget, EZ_WidgetWindow(widget), row,
					      
					      EZ_IFListBoxColumns(widget),
					      Rx,Ry,Rw,Rh);
			    }
			  EZ_IFListBoxSelectionIDX(widget) = save_row;
			  {
			    EZ_Item **row =  EZ_IFListBoxData(widget)[save_row];
			    EZ_Item *item = row[0];
			    if(item && EZ_ItemHighlighted(item) == 0) EZ_ItemHighlighted(item) = 1;
			    EZ_HighlightOneRow(widget, EZ_WidgetWindow(widget), row, 
					       EZ_IFListBoxColumns(widget),fg, bg,
					       Rx,Ry,Rw,Rh, hmode);
			  }
			  EZ_HandleMotionCallBacks(widget,EZ_IFListBoxMotionCallBack(widget));
			  EZ_IFListBoxSetSelection(widget);
			}
		    }
		} /* while */
	    }
	  else /* not on a row, scroll */
	    {
	      done = 0;
	      xevent.type = 0;
	      while(!done)
		{
		  if(XPending(EZ_Display))
		    {
		      while(XPending(EZ_Display))
			{
			  if(xevent.type == MotionNotify)
			    {
			      x = xevent.xmotion.x;
			      y = xevent.xmotion.y;
			    }
			  else if(xevent.type == ButtonRelease)
			    {
			      if( xevent.xbutton.button == EZ_Btn1) break;
			    }
			  else if(xevent.type == Expose || xevent.type == FocusIn || xevent.type == FocusOut)
			    EZ_WidgetDispatchEvent(&xevent);
			  XNextEvent(EZ_Display, &xevent);
			  if(EZ_FilterEvent(&xevent))  EZ_InvokePrivateEventHandler(&xevent);
			}
		    }
		  else { EZ_SitForALittleBit(50000); }
		  if(xevent.type == ButtonRelease && xevent.xbutton.button == EZ_Btn1) {done = 1; break;}
		  if( x < xoffset || x > ww - xoffset || y <= yoffset + vsa[0] || y > hh - yoffset)
		    {
		      if(y <= yoffset + vsa[0])
			{
			  /* scroll down */
			  if(EZ_IFListBoxYStart(widget) < 0)
			    {
			      int tmp = EZ_IFListBoxYStart(widget) + 10;
			      EZ_IFListBoxYStart(widget) = (tmp < 0 ? tmp: 0);
			      EZ_DrawWidget(widget);
			    }
			}
		      else if( y > hh - yoffset)
			{
			  /* scroll up */ 
			  int  th = EZ_IFListBoxYSize(widget);
			  if(th > hh)
			    {
			      int dif = hh - th;
			      int tmp = EZ_IFListBoxYStart(widget) - 10;
			      EZ_IFListBoxYStart(widget) = (tmp > dif ? tmp: dif);
			      EZ_DrawWidget(widget);				  
			    }
			}
		      else if(x < xoffset)
			{
			  /* scroll right */
			  if(EZ_IFListBoxXStart(widget) < 0)
			    {
			      int tmp = EZ_IFListBoxXStart(widget) + 10;
			      EZ_IFListBoxXStart(widget) = (tmp < 0 ? tmp: 0);
			      EZ_DrawWidget(widget);
			    }
			}
		      else if( x > ww - xoffset)
			{
			  /* scroll left */ 
			  int  tw = EZ_IFListBoxXSize(widget);
			  if(tw > ww)
			    {
			      int dif = ww - tw;
			      int tmp = EZ_IFListBoxXStart(widget) - 10;
			      EZ_IFListBoxXStart(widget) = (tmp > dif ? tmp: dif);
			      EZ_DrawWidget(widget);				  
			    }
			}
		    }
		} /* done scrolling */		  		  
	    }
	}
      else /* with rubberband enabled */
	{
	  int save_row = -1, scol = -1;
	  int tline =  EZ_IFListBoxSelectionIDX(widget); 
	  sline = -1;
	  EZ_FindPointerOnRowAndColIFL(EZ_IFListBoxData(widget), 
				       EZ_IFListBoxRows(widget), 
				       EZ_IFListBoxColumns(widget), 
				       x,y, &sline, &scol,
				       Rx,Ry,Rw,Rh);
	  if(sline >= 0 && scol >= 0) /* pointer is on an item */
	    {
	      EZ_IFListBoxClearHighlight(widget, Rx,Ry,Rw, Rh, smode);
	      EZ_IFListBoxSelectionIDX(widget) = sline;
	      {
		EZ_Item *item, **row =  EZ_IFListBoxData(widget)[sline];

		item = row[0];
		if(item)
		  {
		    EZ_ItemHighlighted(item) = 1;
		    EZ_HighlightOneRow(widget, EZ_WidgetWindow(widget), row, 
				       EZ_IFListBoxColumns(widget),fg, bg,
				       Rx,Ry,Rw,Rh,hmode);
		  }
	      }
	      if(sline != tline)EZ_HandleMotionCallBacks(widget,EZ_IFListBoxMotionCallBack(widget));
	      /* EZ_IFListBoxSetSelection(widget);*/
	    }
	  /*
	   * a double click cause the execuation of the call back function
	   */
	  if(sline >= 0 && ABSV(elapsedtime) < DOUBLE_CLICK_TIME_INTERVAL)
	    { 
	      EZ_Item *item, **items = EZ_IFListBoxData(widget)[sline];
	      int i, ncols =  EZ_IFListBoxColumns(widget);
	      for(i = 0; i < ncols; i++)
		{
		  item = items[i];
		  if(item) {EZ_HandleItemCallBacks(item);}
		}
	      EZ_ExecuteWidgetCallBacks(widget);
	      return;
	    }
	   
	  if(sline >= 0)
	    {
	      if(rubberband > 1) /* select all lines the pointer touched */
		{
		  done = 0;
		  moved = 0;
		  while(!done)
		    {
		      do {
			XNextEvent(EZ_Display, &xevent); 	      
			if(EZ_FilterEvent(&xevent)) EZ_InvokePrivateEventHandler(&xevent);
			if(xevent.type == MotionNotify)
			  {
			    moved = 1; 
			    x = xevent.xmotion.x;
			    y = xevent.xmotion.y;
			  }
			else if(xevent.type == ButtonRelease)
			  {
			    if(xevent.xbutton.button == EZ_Btn1) { done = 1; break;}
			  }
			else if(xevent.type == Expose || xevent.type == FocusIn || xevent.type == FocusOut)
			  EZ_WidgetDispatchEvent(&xevent);
		      } while(XPending(EZ_Display) && !done) ;
		      
		      if(!done)
			{
			  if(moved)
			    { 
			      int tmp = EZ_FindPointerOnRowIFL(EZ_IFListBoxData(widget), 
							       EZ_IFListBoxRows(widget), 
							       EZ_IFListBoxColumns(widget), 
							       x,y, Rx,Ry,Rw,Rh);
			      if(tmp >= 0 )
				{
				  EZ_Item *item, **row, ***data = EZ_IFListBoxData(widget);
				  row = data[tmp];
				  item = row[0];
				  if(item && EZ_ItemHighlighted(item) == 0)
				    {
				      EZ_ItemHighlighted(item) = 1;
				      EZ_HighlightOneRow(widget, EZ_WidgetWindow(widget), row, 
							 EZ_IFListBoxColumns(widget),fg, bg,
							 Rx,Ry,Rw,Rh,hmode);	
				      if(save_row == -1) save_row = tmp;
				    }			  
				}
			    }
			}
		      else /* done */
			{
			  if(save_row >= 0) /* cannot be the current selection */
			    {
			      int tline = EZ_IFListBoxSelectionIDX(widget);
			      if(tline != save_row)
				{
				  EZ_Item **row;
				  EZ_IFListBoxSelectionIDX(widget) = save_row;
				  row =  EZ_IFListBoxData(widget)[save_row];
				  EZ_HighlightOneRow(widget, EZ_WidgetWindow(widget), row, 
						     EZ_IFListBoxColumns(widget),fg, bg,
						     Rx,Ry,Rw,Rh, hmode);
				  EZ_HandleMotionCallBacks(widget,EZ_IFListBoxMotionCallBack(widget));
				  /* EZ_IFListBoxSetSelection(widget); */
				}
			    }
			}
		    }
		}
	      else  /* rubberbad == 1 */
		{
		  int save_row = -1;
		  done = 0;
		  moved = 0;
		  while(!done)
		    {
		      do {
			XNextEvent(EZ_Display, &xevent); 	      
			if(EZ_FilterEvent(&xevent)) EZ_InvokePrivateEventHandler(&xevent);
			if(xevent.type == MotionNotify)
			  {
			    moved = 1; 
			    x = xevent.xmotion.x;
			    y = xevent.xmotion.y;
			  }
			else if(xevent.type == ButtonRelease)
			  {
			    if(xevent.xbutton.button == EZ_Btn1) { done = 1; break;}
			  }
			else if(xevent.type == Expose || xevent.type == FocusIn || xevent.type == FocusOut)
			  EZ_WidgetDispatchEvent(&xevent);
		      } while(XPending(EZ_Display) && !done) ;
		      
		      if(!done)
			{
			  if(moved)
			    { 
			      int tmp = EZ_FindPointerOnRowIFL(EZ_IFListBoxData(widget), 
							       EZ_IFListBoxRows(widget), 
							       EZ_IFListBoxColumns(widget), 
							       x,y, Rx,Ry,Rw,Rh);
			      if(tmp != save_row && tmp != EZ_IFListBoxSelectionIDX(widget))
				{
				  if(save_row >= 0)
				    EZ_UpdateOneRow(widget, EZ_WidgetWindow(widget),
						    EZ_IFListBoxData(widget)[save_row],
						    (int)EZ_IFListBoxColumns(widget),
						    Rx,Ry,Rw,Rh);
				  if(tmp >= 0) EZ_FlashRowOutline(widget, 
								  EZ_IFListBoxData(widget)[tmp],
								  (int)EZ_IFListBoxColumns(widget));
				  save_row = tmp;
				}
			    }
			}
		      else /* we are done */
			{
			  if(save_row >= 0) /* cannot be the current selection */
			    {
			      int tline = EZ_IFListBoxSelectionIDX(widget);
			      if(tline > 0)
				{
				  EZ_Item **row = EZ_IFListBoxData(widget)[tline];
				  EZ_Item *item = row[0];
				  if(item && EZ_ItemHighlighted(item)) EZ_ItemHighlighted(item) = 0;
				  EZ_UpdateOneRow(widget, EZ_WidgetWindow(widget), row,
					      
						  EZ_IFListBoxColumns(widget),
						  Rx,Ry,Rw,Rh);
				}
			      EZ_IFListBoxSelectionIDX(widget) = save_row;
			      {
				EZ_Item **row =  EZ_IFListBoxData(widget)[save_row];
				EZ_Item *item = row[0];
				if(item && EZ_ItemHighlighted(item) == 0) EZ_ItemHighlighted(item) = 1;
				EZ_HighlightOneRow(widget, EZ_WidgetWindow(widget), row, 
						   EZ_IFListBoxColumns(widget),fg, bg,
						   Rx,Ry,Rw,Rh, hmode);
			      }
			      EZ_HandleMotionCallBacks(widget,EZ_IFListBoxMotionCallBack(widget));
			      EZ_IFListBoxSetSelection(widget);
			    }
			}
		    } /* while */
		}
	    }
	  else if(rubberband) /* pointer clicked on the background, do rubberband */
	    {
	      int x0, y0, x1, y1, rx, ry, rw,rh, dflag = 0, moved = 0;
	      Window win = EZ_WidgetWindow(widget);
	      GC gc = EZ_XORGC;
	      x0 = x;      y0 = y;
	      rx = ry = rw = rh = 0;
		  
	      while(1)
		{
		  XNextEvent(EZ_Display, &xevent);
		  if(EZ_FilterEvent(&xevent))  EZ_InvokePrivateEventHandler(&xevent);
		  if(xevent.type == MotionNotify)
		    {
		      if(moved == 0)
			{
			  moved = 1;
			  EZ_IFListBoxClearHighlight(widget, Rx,Ry,Rw, Rh,smode);
			  EZ_IFListBoxSelectionIDX(widget) = -1;
			  EZ_IFListBoxSelectionCol(widget) = -1;
			}
		      if(dflag) 
			{
			  XDrawRectangle(EZ_Display, win, gc, rx,ry,rw, rh);
			  dflag = 0;
			}
		      while (XCheckTypedEvent(EZ_Display, MotionNotify, &xevent));
		      x1 = xevent.xmotion.x;
		      y1 = xevent.xmotion.y;
		      rw = x1 - x0; if(rw<0) rw = -rw;
		      rh = y1 - y0; if(rh<0) rh = -rh;
		      rx = x0 < x1 ? x0: x1;
		      ry = y0 < y1 ? y0: y1;
		      if(moved) EZ_IFBHighlightRowsInRectangle(widget, rx,ry,rw,rh);
		      XDrawRectangle(EZ_Display, win, gc, rx,ry,rw, rh);
		      dflag = 1;
		    }
		  else if(xevent.type == ButtonRelease &&  xevent.xbutton.button == EZ_Btn1)
		    {
		      if(dflag) 
			XDrawRectangle(EZ_Display, win, gc, rx,ry,rw, rh);		      
		      break;
		    }
		  else if(xevent.type == Expose || xevent.type == FocusIn || xevent.type == FocusOut)
		    EZ_WidgetDispatchEvent(&xevent);
		}
	      if(moved) /* select a row */
		{
		  EZ_Item ***data, **row, *item;
		  int nrows = EZ_IFListBoxRows(widget);
		  int i;
  
		  data = EZ_IFListBoxData(widget);
		  for(i = 1; i < nrows; i++)
		    {
		      row = data[i];
		      item = row[0];
		      if(item && EZ_ItemHighlighted(item))
			{
			  EZ_IFListBoxSelectionIDX(widget) = i;
			  EZ_IFListBoxSelectionCol(widget) = 0;
			  EZ_HandleMotionCallBacks(widget,EZ_IFListBoxMotionCallBack(widget));
			  break;
			}
		    }
		}
	    }
	}
    }
  else if(event->xbutton.button == EZ_Btn3)
    {
      EZ_Item ***data, **row, *item;
      int     nrows, ncols;
      Window  widgetwin = EZ_WidgetWindow(widget);

      bw = EZ_WidgetBorderWidth(widget) + EZ_WidgetPadB(widget);
      xoffset = bw + EZ_WidgetPadX(widget);
      yoffset = bw + EZ_WidgetPadY(widget);

      Rx = bw;
      Ry = yoffset + vsa[0];
      ww = EZ_WidgetWidth(widget);
      Rw = ww - (bw << 1);
      hh = EZ_WidgetHeight(widget);
      Rh = hh - bw - Ry;
      x = event->xbutton.x;
      y = event->xbutton.y;

      data = EZ_IFListBoxData(widget);
      nrows = EZ_IFListBoxRows(widget);
      ncols = EZ_IFListBoxColumns(widget);
      sline = EZ_FindPointerOnRowIFL(data, nrows, ncols, x,y, Rx,Ry,Rw,Rh);

      if(sline >= 0)
	{
	  int tline = EZ_IFListBoxSelectionIDX(widget);
	  if(rubberband == 0)
	    {
	      if(sline != tline)
		{
		  if(tline > 0)
		    {
		      row = data[tline];
		      item = row[0];
		      if(item && EZ_ItemHighlighted(item)) EZ_ItemHighlighted(item) = 0;
		      EZ_UpdateOneRow(widget, widgetwin,  row,
				      EZ_IFListBoxColumns(widget),
				      Rx,Ry,Rw,Rh);
		    }
		  EZ_IFListBoxSelectionIDX(widget) = sline;
		  {
		    row =  data[sline];
		    item = row[0];
		    if(item && EZ_ItemHighlighted(item) == 0) EZ_ItemHighlighted(item) = 1;
		    EZ_HighlightOneRow(widget, widgetwin, row, 
				       EZ_IFListBoxColumns(widget),fg, bg,
				       Rx,Ry,Rw,Rh, hmode);
		  }
		  EZ_HandleMotionCallBacks(widget,EZ_IFListBoxMotionCallBack(widget));
		}
	      ifbDnDOneRow(widget, sline, event, Rx,Ry,Rw,Rh);
	    }
	  else 
	    {
	      row = data[sline];
	      item = row[0];
	      if(item)
		{
		  if(EZ_ItemHighlighted(item) == 0) /* drag a new row */
		    {
		      EZ_IFListBoxClearHighlight(widget, Rx,Ry,Rw, Rh,smode);
		      EZ_IFListBoxSelectionIDX(widget) = sline;
		      EZ_ItemHighlighted(item) = 1;
		      item = row[0];
		      if(item && EZ_ItemHighlighted(item) == 0) EZ_ItemHighlighted(item) = 1;
		      EZ_HighlightOneRow(widget, widgetwin, row, 
					 EZ_IFListBoxColumns(widget),fg, bg,
					 Rx,Ry,Rw,Rh, hmode); 
		      ifbDnDOneRow(widget, sline, event, Rx,Ry,Rw,Rh);      
		    }
		  else  /* on a highlighted row */
		    {
		      int nhrows = ifboxGetNumberOfHiglightedRows(widget);
		      if(nhrows == 1) ifbDnDOneRow(widget, sline, event, Rx,Ry,Rw,Rh);      
		      else ifbDnDNRows(widget, nhrows, event, Rx,Ry,Rw,Rh);      
		    }
		}
	    }
	}
      else if(rubberband) /* click on the background, do rubberband */
	{
	  int x0, y0, x1, y1, rx, ry, rw,rh, dflag = 0, moved = 0;
	  Window win = EZ_WidgetWindow(widget);
	  GC gc = EZ_XORGC;
	  x0 = x;      y0 = y;
	  rx = ry = rw = rh = 0;
		  
	  while(1)
	    {
	      XNextEvent(EZ_Display, &xevent);
	      if(EZ_FilterEvent(&xevent))  EZ_InvokePrivateEventHandler(&xevent);
	      if(xevent.type == MotionNotify)
		{
		  if(moved == 0)
		    {
		      moved = 1;
		      EZ_IFListBoxClearHighlight(widget, Rx,Ry,Rw, Rh,smode);
		      EZ_IFListBoxSelectionIDX(widget) = -1;
		      EZ_IFListBoxSelectionCol(widget) = -1;
		    }
		  if(dflag) 
		    {
		      XDrawRectangle(EZ_Display, win, gc, rx,ry,rw, rh);
		      dflag = 0;
		    }
		  while (XCheckTypedEvent(EZ_Display, MotionNotify, &xevent));
		  x1 = xevent.xmotion.x;
		  y1 = xevent.xmotion.y;
		  rw = x1 - x0; if(rw<0) rw = -rw;
		  rh = y1 - y0; if(rh<0) rh = -rh;
		  rx = x0 < x1 ? x0: x1;
		  ry = y0 < y1 ? y0: y1;
		  if(moved) EZ_IFBHighlightRowsInRectangle(widget, rx,ry,rw,rh);
		  XDrawRectangle(EZ_Display, win, gc, rx,ry,rw, rh);
		  dflag = 1;
		}
	      else if(xevent.type == ButtonRelease &&  xevent.xbutton.button == EZ_Btn3)
		{
		  if(dflag) 
		    XDrawRectangle(EZ_Display, win, gc, rx,ry,rw, rh);		      
		  break;
		}
	      else if(xevent.type == Expose || xevent.type == FocusIn || xevent.type == FocusOut)
		EZ_WidgetDispatchEvent(&xevent);
	    }
	  if(moved) /* select a row */
	    {
	      EZ_Item ***data, **row, *item;
	      int nrows = EZ_IFListBoxRows(widget);
	      int i;
  
	      data = EZ_IFListBoxData(widget);
	      for(i = 1; i < nrows; i++)
		{
		  row = data[i];
		  item = row[0];
		  if(item && EZ_ItemHighlighted(item))
		    {
		      EZ_IFListBoxSelectionIDX(widget) = i;
		      EZ_IFListBoxSelectionCol(widget) = 0;
		      EZ_HandleMotionCallBacks(widget,EZ_IFListBoxMotionCallBack(widget));
		      break;
		    }
		}
	    }
	}
    }
}
/*************************************************************
 *
 * handle button Event in ITEM mode
 */
void  EZ_IFListBoxHandleBtnEventB(widget, event) EZ_Widget *widget; XEvent *event;
{
  XEvent    xevent;
  int       x, y, bw, Rx,Ry,Rw,Rh, xoffset, yoffset, done,  smode;
  int       oldx, oldy, iww, ihh, moved = 0;
  int       *vsa,ww,hh;
  EZ_Item   *nsitem = NULL;
  int       rubberband, hmode;
  unsigned long fg = EZ_IFListBoxHighlightFG(widget);
  unsigned long bg = EZ_IFListBoxHighlightBG(widget);
  Window   widgetwin = EZ_WidgetWindow(widget);

  /* first, get focus if we don;t have it */
  rubberband = EZ_IFListBoxRubberBand(widget);
  hmode = EZ_IFListBoxHighlightMode(widget);
  smode = EZ_IFListBoxSelectionType(widget);
  vsa = EZ_IFListBoxVSizes(widget);

  if(event->xbutton.button == EZ_Btn1) 	  /* get keyboard focus */
    { 
      Window fw; int rt;
      EZ_Widget *tmp;
      XGetInputFocus(EZ_Display, &fw, &rt);
      tmp = EZ_LookupWidgetFromMappedHT(fw);
      if(tmp != widget) EZ_SetFocusTo(widget);
    }
  if(EZ_IFListBoxRows(widget) <= 0) {return;}
  if(event->xbutton.button == EZ_Btn1)
    {
      Time  time_stamp = event->xbutton.time;   
      int   elapsedtime = EZ_LastButton1ClickTime - time_stamp; 
      EZ_LastButton1ClickTime = time_stamp;

      bw = EZ_WidgetBorderWidth(widget) + EZ_WidgetPadB(widget);
      xoffset = bw + EZ_WidgetPadX(widget);
      yoffset = bw + EZ_WidgetPadY(widget);

      Rx = bw;
      Ry = yoffset + vsa[0];
      ww = EZ_WidgetWidth(widget);
      Rw = ww - (bw << 1);
      hh = EZ_WidgetHeight(widget);
      Rh = hh - bw - Ry;

      x = event->xbutton.x;
      y = event->xbutton.y;
      if(rubberband == 0)   /* no rubberband */
	{ 
	  int rowN, colN;
	  EZ_FindPointerOnRowAndColIFL(EZ_IFListBoxData(widget), 
				       EZ_IFListBoxRows(widget), 
				       EZ_IFListBoxColumns(widget), 
				       x,y, &rowN, &colN,
				       Rx,Ry,Rw,Rh);
	      
	  if(rowN > 0 && colN >= 0)
	    {
	      EZ_Item ***data, **row, *sitem = NULL;
	      int sline = EZ_IFListBoxSelectionIDX(widget);
	      int col =  EZ_IFListBoxSelectionCol(widget);
	      data = EZ_IFListBoxData(widget);
	      if(sline > 0 && col >= 0)
		{
		  row = data[sline];
		  sitem = row[col];
		}
	      if(sline != rowN || col != colN)
		{
		  EZ_IFListBoxSelectionIDX(widget) = rowN;
		  EZ_IFListBoxSelectionCol(widget) = colN;
		  if(sitem)
		    {
		      EZ_ItemHighlighted(sitem) = 0;
		      EZ_UpdateOneItem(widget,EZ_WidgetWindow(widget),sitem, Rx,Ry,Rw,Rh);
		    }
		  {
		    row = data[rowN];
		    sitem = row[colN];
		    if(sitem)
		      {
			EZ_ItemHighlighted(sitem) = 1;
			EZ_HighlightOneItem(widget, EZ_WidgetWindow(widget),
					    sitem, fg, bg, Rx,Ry,Rw,Rh, hmode);
		      }
		  }
		  EZ_HandleMotionCallBacks(widget,EZ_IFListBoxMotionCallBack(widget));
		  EZ_IFListBoxSetSelection(widget);
		}
	      /*
	       * a double click cause the execuation of the call back function
	       */
	      if(sitem && ABSV(elapsedtime) < DOUBLE_CLICK_TIME_INTERVAL)
		{ 
		  /* EZ_Widget *fl = EZ_IFListBoxFListBox(widget);*/
		  EZ_HandleItemCallBacks(sitem);
		  EZ_ExecuteWidgetCallBacks(widget);
		  /* if(fl) {EZ_ExecuteWidgetCallBacks(fl);}*/
		  return;
		}
	      nsitem = sitem;
	      /* to make gcc happy */
	      { oldx = oldy = 1; iww = ihh = 1; }
		  
	      if(nsitem)
		{
		  int save_row = -1, save_col = -1;
		  done = 0;
		  moved = 0;
		  while(!done)
		    {
		      do {
			XNextEvent(EZ_Display, &xevent); 	      
			if(EZ_FilterEvent(&xevent)) EZ_InvokePrivateEventHandler(&xevent);
			if(xevent.type == MotionNotify)
			  {
			    moved = 1; 
			    x = xevent.xmotion.x;
			    y = xevent.xmotion.y;
			  }
			else if(xevent.type == ButtonRelease)
			  {
			    if(xevent.xbutton.button == EZ_Btn1) { done = 1; break;}
			  }
			else if(xevent.type == Expose || xevent.type == FocusIn || xevent.type == FocusOut) 
			  EZ_WidgetDispatchEvent(&xevent);
		      } while(XPending(EZ_Display) && !done) ;
			  
		      if(!done)
			{
			  if(moved)
			    { 
			      int rowN, colN;
			      EZ_FindPointerOnRowAndColIFL(EZ_IFListBoxData(widget), 
							   EZ_IFListBoxRows(widget), 
							   EZ_IFListBoxColumns(widget), 
							   x,y, &rowN, &colN,
							   Rx,Ry,Rw,Rh);
			      if( (rowN != save_row || colN != save_col) &&
				  (rowN != (int) EZ_IFListBoxSelectionIDX(widget) ||
				   colN != (int)EZ_IFListBoxSelectionCol(widget)))
				{
				  if(save_row > 0 && save_col >= 0)
				    {
				      EZ_Item ***data, **row, *item = NULL;
				      data = EZ_IFListBoxData(widget);
				      row = data[save_row];
				      item = row[save_col];
				      EZ_UpdateOneItem(widget,EZ_WidgetWindow(widget),item, Rx,Ry,Rw,Rh);
				    }
				  if(rowN > 0 && colN >= 0)
				    {
				      EZ_Item ***data, **row, *item = NULL;
				      data = EZ_IFListBoxData(widget);
				      row = data[rowN];
				      item = row[colN];
				      EZ_FlashItemOutline(widget,item, Rx,Ry,Rw,Rh);
				    }
				  save_row = rowN; save_col = colN;
				}
			    }
			}
		      else /* we are done */
			{
			  if(save_row > 0 && save_col >= 0) /* cannot be the current selection */
			    {
			      int rowN = (int)EZ_IFListBoxSelectionIDX(widget);
			      int colN = (int)EZ_IFListBoxSelectionCol(widget);
				  
			      if(rowN > 0 && colN >= 0)
				{
				  EZ_Item ***data, **row, *item = NULL;
				  data = EZ_IFListBoxData(widget);
				  row = data[rowN];
				  item = row[colN];
				  if(item)
				    {
				      EZ_ItemHighlighted(item) = 0;
				      EZ_UpdateOneItem(widget,EZ_WidgetWindow(widget),item, Rx,Ry,Rw,Rh);
				    }
				}
			      EZ_IFListBoxSelectionIDX(widget) = save_row;
			      EZ_IFListBoxSelectionCol(widget) = save_col;
			      {
				EZ_Item **row =  EZ_IFListBoxData(widget)[save_row];
				EZ_Item *item = row[save_col];
				if(item)
				  {
				    EZ_ItemHighlighted(item) = 1;
				    EZ_HighlightOneItem(widget,EZ_WidgetWindow(widget),item,fg,bg,
							Rx, Ry, Rw, Rh, hmode);
				  }
			      }
			      EZ_HandleMotionCallBacks(widget,EZ_IFListBoxMotionCallBack(widget));
			      EZ_IFListBoxSetSelection(widget);
			    }
			}
		    } /* while */
		} 
	    }
	  else /* click is on the background */
	    {
	      done = 0;
	      xevent.type = 0;
	      while(!done)
		{
		  if(XPending(EZ_Display))
		    {
		      while(XPending(EZ_Display))
			{
			  if(xevent.type == MotionNotify)
			    {
			      x = xevent.xmotion.x;
			      y = xevent.xmotion.y;
			    }
			  else if(xevent.type == ButtonRelease)
			    {
			      if( xevent.xbutton.button == EZ_Btn1) break;
			    }
			  else if(xevent.type == Expose || xevent.type == FocusIn || xevent.type == FocusOut)  
			    EZ_WidgetDispatchEvent(&xevent);
			  XNextEvent(EZ_Display, &xevent);
			  if(EZ_FilterEvent(&xevent)) EZ_InvokePrivateEventHandler(&xevent);
			}
		    }
		  else { EZ_SitForALittleBit(50000); }
		  if(xevent.type == ButtonRelease && xevent.xbutton.button == EZ_Btn1) {done = 1; break;}
		  if( x < xoffset || x > ww - xoffset || y <= yoffset + vsa[0] || y > hh - yoffset)
		    {
		      if(y <= yoffset + vsa[0])
			{
			  /* scroll down */
			  if(EZ_IFListBoxYStart(widget) < 0)
			    {
			      int tmp = EZ_IFListBoxYStart(widget) + 10;
			      EZ_IFListBoxYStart(widget) = (tmp < 0 ? tmp: 0);
			      EZ_DrawWidget(widget);
			    }
			}
		      else if( y > hh - yoffset)
			{
			  /* scroll up */ 
			  int  th = EZ_IFListBoxYSize(widget);
			  if(th > hh)
			    {
			      int dif = hh - th;
			      int tmp = EZ_IFListBoxYStart(widget) - 10;
			      int nOff = (tmp > dif ? tmp: dif);
			      if( EZ_IFListBoxYStart(widget) != nOff)
				{
				  EZ_IFListBoxYStart(widget) = nOff;
				  EZ_DrawWidget(widget);		
				}		  
			    }
			}
		      else if(x < xoffset)
			{
			  /* scroll right */
			  if(EZ_IFListBoxXStart(widget) < 0)
			    {
			      int tmp = EZ_IFListBoxXStart(widget) + 10;
			      EZ_IFListBoxXStart(widget) = (tmp < 0 ? tmp: 0);
			      EZ_DrawWidget(widget);
			    }
			}
		      else if( x > ww - xoffset)
			{
			  /* scroll left */ 
			  int  tw = EZ_IFListBoxXSize(widget);
			  if(tw > ww)
			    {
			      int dif = ww - tw;
			      int tmp = EZ_IFListBoxXStart(widget) - 10;
			      int nOff = (tmp > dif ? tmp: dif);
			      if(EZ_IFListBoxXStart(widget) !=  nOff)
				{
				  EZ_IFListBoxXStart(widget) = nOff;
				  EZ_DrawWidget(widget);		
				}		  
			    }
			}
		    }
		} /* done scrolling */		  		  
	    }
	}
      else /* do rubberband */
	{
	  int rowN, colN;
	  EZ_FindPointerOnRowAndColIFL(EZ_IFListBoxData(widget), 
				       EZ_IFListBoxRows(widget), 
				       EZ_IFListBoxColumns(widget), 
				       x,y, &rowN, &colN,
				       Rx,Ry,Rw,Rh);
	      
	  if(rowN > 0 && colN >= 0) /* click on an item */
	    {
	      EZ_Item ***data, **row, *sitem = NULL, *ositem = NULL;
	      int sline = EZ_IFListBoxSelectionIDX(widget);
	      int col =  EZ_IFListBoxSelectionCol(widget);
	      data = EZ_IFListBoxData(widget);
	      if(sline > 0 && col >= 0)
		{
		  row = data[sline];
		  ositem = row[col];
		}
	      EZ_IFListBoxClearHighlight(widget, Rx,Ry,Rw, Rh,smode);
	      EZ_IFListBoxSelectionIDX(widget) = rowN;
	      EZ_IFListBoxSelectionCol(widget) = colN;
	      row = data[rowN];
	      sitem = row[colN];
	      if(sitem)
		{
		  EZ_ItemHighlighted(sitem) = 1;
		  EZ_HighlightOneItem(widget, EZ_WidgetWindow(widget),
				      sitem, fg, bg, Rx,Ry,Rw,Rh, hmode);
		}
	      if(ositem != sitem)
		EZ_HandleMotionCallBacks(widget,EZ_IFListBoxMotionCallBack(widget));
	      /* EZ_IFListBoxSetSelection(widget);*/

	      /*
	       * a double click cause the execuation of the call back function
	       */
	      if(sitem && ABSV(elapsedtime) < DOUBLE_CLICK_TIME_INTERVAL)
		{ 
		  /* EZ_Widget *fl = EZ_IFListBoxFListBox(widget);*/
		  EZ_HandleItemCallBacks(sitem);
		  EZ_ExecuteWidgetCallBacks(widget);
		  /* if(fl) {EZ_ExecuteWidgetCallBacks(fl);}*/
		  return;
		}
	      nsitem = sitem;
	      /* to make gcc happy */
	      { oldx = oldy = 1; iww = ihh = 1; }
		  
	      if(nsitem)
		{
		  if(rubberband > 1)
		    {
		      done = 0;
		      moved = 0;
		      while(!done)
			{
			  do {
			    XNextEvent(EZ_Display, &xevent); 	      
			    if(EZ_FilterEvent(&xevent)) EZ_InvokePrivateEventHandler(&xevent);
			    if(xevent.type == MotionNotify)
			      {
				moved = 1; 
				x = xevent.xmotion.x;
				y = xevent.xmotion.y;
			      }
			    else if(xevent.type == ButtonRelease)
			      {
				if(xevent.xbutton.button == EZ_Btn1) { done = 1; break;}
			      }
			    else if(xevent.type == Expose || xevent.type == FocusIn || xevent.type == FocusOut) 
			      EZ_WidgetDispatchEvent(&xevent);
			  } while(XPending(EZ_Display) && !done) ;
			  
			  if(!done)
			    {
			      if(moved)
				{ 
				  int rowN, colN;
				  EZ_FindPointerOnRowAndColIFL(EZ_IFListBoxData(widget), 
							       EZ_IFListBoxRows(widget), 
							       EZ_IFListBoxColumns(widget), 
							       x,y, &rowN, &colN,
							       Rx,Ry,Rw,Rh);
				  if( rowN > 0 && colN >= 0 )
				    {
				      EZ_Item ***data, **row, *item = NULL;
				      data = EZ_IFListBoxData(widget);
				      row = data[rowN];
				      item = row[colN];
				      if(item && EZ_ItemHighlighted(item) == 0)
					{
					  EZ_ItemHighlighted(item) = 1;
					  EZ_HighlightOneItem(widget, EZ_WidgetWindow(widget),
							      item, fg, bg, Rx,Ry,Rw,Rh, hmode);
					}
				    }

				}
			    }
			} /* while */
		    }
		  else /* rubberband = 1 */
		    {
		      int save_row = -1, save_col = -1;
		      done = 0;
		      moved = 0;
		      while(!done)
			{
			  do {
			    XNextEvent(EZ_Display, &xevent); 	      
			    if(EZ_FilterEvent(&xevent)) EZ_InvokePrivateEventHandler(&xevent);
			    if(xevent.type == MotionNotify)
			      {
				moved = 1; 
				x = xevent.xmotion.x;
				y = xevent.xmotion.y;
			      }
			    else if(xevent.type == ButtonRelease)
			      {
				if(xevent.xbutton.button == EZ_Btn1) { done = 1; break;}
			      }
			    else if(xevent.type == Expose || xevent.type == FocusIn || xevent.type == FocusOut) 
			      EZ_WidgetDispatchEvent(&xevent);
			  } while(XPending(EZ_Display) && !done) ;
			  
			  if(!done)
			    {
			      if(moved)
				{ 
				  int rowN, colN;
				  EZ_FindPointerOnRowAndColIFL(EZ_IFListBoxData(widget), 
							       EZ_IFListBoxRows(widget), 
							       EZ_IFListBoxColumns(widget), 
							       x,y, &rowN, &colN,
							       Rx,Ry,Rw,Rh);
				  if( (rowN != save_row || colN != save_col) &&
				      (rowN != (int) EZ_IFListBoxSelectionIDX(widget) ||
				       colN != (int)EZ_IFListBoxSelectionCol(widget)))
				    {
				      if(save_row > 0 && save_col >= 0)
					{
					  EZ_Item ***data, **row, *item = NULL;
					  data = EZ_IFListBoxData(widget);
					  row = data[save_row];
					  item = row[save_col];
					  EZ_UpdateOneItem(widget,EZ_WidgetWindow(widget),item, Rx,Ry,Rw,Rh);
					}
				      if(rowN > 0 && colN >= 0)
					{
					  EZ_Item ***data, **row, *item = NULL;
					  data = EZ_IFListBoxData(widget);
					  row = data[rowN];
					  item = row[colN];
					  EZ_FlashItemOutline(widget,item, Rx,Ry,Rw,Rh);
					}
				      save_row = rowN; save_col = colN;
				    }
				}
			    }
			  else /* we are done */
			    {
			      if(save_row > 0 && save_col >= 0) /* cannot be the current selection */
				{
				  int rowN = (int)EZ_IFListBoxSelectionIDX(widget);
				  int colN = (int)EZ_IFListBoxSelectionCol(widget);
				  
				  if(rowN > 0 && colN >= 0)
				    {
				      EZ_Item ***data, **row, *item = NULL;
				      data = EZ_IFListBoxData(widget);
				      row = data[rowN];
				      item = row[colN];
				      if(item)
					{
					  EZ_ItemHighlighted(item) = 0;
					  EZ_UpdateOneItem(widget,EZ_WidgetWindow(widget),item, Rx,Ry,Rw,Rh);
					}
				    }
				  EZ_IFListBoxSelectionIDX(widget) = save_row;
				  EZ_IFListBoxSelectionCol(widget) = save_col;
				  {
				    EZ_Item **row =  EZ_IFListBoxData(widget)[save_row];
				    EZ_Item *item = row[save_col];
				    if(item)
				      {
					EZ_ItemHighlighted(item) = 1;
					EZ_HighlightOneItem(widget,EZ_WidgetWindow(widget),item,fg,bg,
							    Rx, Ry, Rw, Rh, hmode);
				      }
				  }
				  EZ_HandleMotionCallBacks(widget,EZ_IFListBoxMotionCallBack(widget));
				  EZ_IFListBoxSetSelection(widget);
				}
			    }
			} /* while */
		    } 
		}
	    }
	  else /* click is on the background, do rubberband */
	    {
	      int x0, y0, x1, y1, rx, ry, rw,rh, dflag = 0, moved = 0;
	      Window win = EZ_WidgetWindow(widget);
	      GC gc = EZ_XORGC;
	      x0 = x;      y0 = y;
	      rx = ry = rw = rh = 0;
		  
	      while(1)
		{
		  XNextEvent(EZ_Display, &xevent);
		  if(EZ_FilterEvent(&xevent))  EZ_InvokePrivateEventHandler(&xevent);
		  if(xevent.type == MotionNotify)
		    {
		      if(moved == 0)
			{
			  moved = 1;
			  EZ_IFListBoxClearHighlight(widget, Rx,Ry,Rw, Rh,smode);
			  EZ_IFListBoxSelectionIDX(widget) = -1;
			}
		      if(dflag) 
			{
			  XDrawRectangle(EZ_Display, win, gc, rx,ry,rw, rh);
			  dflag = 0;
			}
		      while (XCheckTypedEvent(EZ_Display, MotionNotify, &xevent));
		      x1 = xevent.xmotion.x;
		      y1 = xevent.xmotion.y;
		      rw = x1 - x0; if(rw<0) rw = -rw;
		      rh = y1 - y0; if(rh<0) rh = -rh;
		      rx = x0 < x1 ? x0: x1;
		      ry = y0 < y1 ? y0: y1;
		      if(moved) EZ_IFBHighlightItemsInRectangle(widget, rx,ry,rw,rh);
		      XDrawRectangle(EZ_Display, win, gc, rx,ry,rw, rh);
		      dflag = 1;
		    }
		  else if(xevent.type == ButtonRelease &&  xevent.xbutton.button == EZ_Btn1)
		    {
		      if(dflag) 
			XDrawRectangle(EZ_Display, win, gc, rx,ry,rw, rh);		      
		      break;
		    }
		  else if(xevent.type == Expose || xevent.type == FocusIn || xevent.type == FocusOut)
		    EZ_WidgetDispatchEvent(&xevent);
		}
	      if(moved) /* select a item */
		{
		  EZ_Item ***data, **row, *item;
		  int nrows = EZ_IFListBoxRows(widget);
		  int i,j, ncols =  EZ_IFListBoxColumns(widget);
  
		  data = EZ_IFListBoxData(widget);
		  for(i = 1; i < nrows; i++)
		    {
		      row = data[i];
		      for(j = 0; j < ncols; j++)
			{
			  item = row[j];
			  if(item && EZ_ItemHighlighted(item))
			    {
			      EZ_IFListBoxSelectionIDX(widget) = i;
			      EZ_IFListBoxSelectionCol(widget) = j;
			      EZ_HandleMotionCallBacks(widget,EZ_IFListBoxMotionCallBack(widget));
			      i = j = 9999999;
			    }
			}
		    }
		}
	    }
	}
    }
  else if(event->xbutton.button == EZ_Btn3)
    {
      bw = EZ_WidgetBorderWidth(widget) + EZ_WidgetPadB(widget);
      xoffset = bw + EZ_WidgetPadX(widget);
      yoffset = bw + EZ_WidgetPadY(widget);

      Rx = bw;
      Ry = yoffset + vsa[0];
      ww = EZ_WidgetWidth(widget);
      Rw = ww - (bw << 1);
      hh = EZ_WidgetHeight(widget);
      Rh = hh - bw - Ry;

      x = event->xbutton.x;
      y = event->xbutton.y;

      /*  */
      {
	int rowN, colN;
	int sline = EZ_IFListBoxSelectionIDX(widget);
	int col =  EZ_IFListBoxSelectionCol(widget);
	EZ_Item ***data, **row, *sitem = NULL;
	data = EZ_IFListBoxData(widget);
	EZ_FindPointerOnRowAndColIFL(data,
				     EZ_IFListBoxRows(widget), 
				     EZ_IFListBoxColumns(widget), 
				     x,y, &rowN, &colN,
				     Rx,Ry,Rw,Rh);
	if(rowN > 0 && colN >= 0) /* picked an item */
	  {
	    if(rubberband == 0) /* no rubberband */
	      {
		if(sline > 0 && col >= 0)
		  {
		    row = data[sline];
		    sitem = row[col];
		  }
		if(sline != rowN || col != colN)
		  {
		    EZ_IFListBoxSelectionIDX(widget) = rowN;
		    EZ_IFListBoxSelectionCol(widget) = colN;
		    if(sitem)
		      {
			EZ_ItemHighlighted(sitem) = 0;
			EZ_UpdateOneItem(widget,widgetwin,sitem, Rx,Ry,Rw,Rh);
		      }
		    {
		      row = data[rowN];
		      sitem = row[colN];
		      if(sitem)
			{
			  EZ_ItemHighlighted(sitem) = 1;
			  EZ_HighlightOneItem(widget, widgetwin,
					      sitem, fg, bg, Rx,Ry,Rw,Rh, hmode);
			}
		    }
		    EZ_HandleMotionCallBacks(widget,EZ_IFListBoxMotionCallBack(widget));
		    EZ_IFListBoxSetSelection(widget);
		  }
		nsitem = sitem;
		ifbDnDOneItem(widget, nsitem, event, Rx,Ry,Rw,Rh);
	      }
	    else
	      {
		row = data[rowN];
		sitem = row[colN];
		if(sitem)
		  {
		    if(EZ_ItemHighlighted(sitem) == 0) /* a new item */
		      {
			EZ_IFListBoxClearHighlight(widget, Rx,Ry,Rw, Rh,smode);
			EZ_IFListBoxSelectionIDX(widget) = rowN;
			EZ_IFListBoxSelectionCol(widget) = colN;
			EZ_ItemHighlighted(sitem) = 1;
			EZ_HighlightOneItem(widget, widgetwin,
					    sitem, fg, bg, Rx,Ry,Rw,Rh, hmode);
			ifbDnDOneItem(widget, sitem, event, Rx,Ry,Rw,Rh);
		      }
		    else
		      {
			int nhitems = ifboxGetNumberOfHiglightedItems(widget);
			if(nhitems == 1) ifbDnDOneItem(widget, sitem, event, Rx,Ry,Rw,Rh);
			else ifbDnDNItems(widget, nhitems, event, Rx,Ry,Rw,Rh);
		      }
		  }
	      }
	  }
	else if(rubberband) /* click is on the background, do rubberband */
	  {
	    int x0, y0, x1, y1, rx, ry, rw,rh, dflag = 0, moved = 0;
	    Window win = EZ_WidgetWindow(widget);
	    GC gc = EZ_XORGC;
	    x0 = x;      y0 = y;
	    rx = ry = rw = rh = 0;
		  
	    while(1)
	      {
		XNextEvent(EZ_Display, &xevent);
		if(EZ_FilterEvent(&xevent))  EZ_InvokePrivateEventHandler(&xevent);
		if(xevent.type == MotionNotify)
		  {
		    if(moved == 0)
		      {
			moved = 1;
			EZ_IFListBoxClearHighlight(widget, Rx,Ry,Rw, Rh,smode);
			EZ_IFListBoxSelectionIDX(widget) = -1;
		      }
		    if(dflag) 
		      {
			XDrawRectangle(EZ_Display, win, gc, rx,ry,rw, rh);
			dflag = 0;
		      }
		    while (XCheckTypedEvent(EZ_Display, MotionNotify, &xevent));
		    x1 = xevent.xmotion.x;
		    y1 = xevent.xmotion.y;
		    rw = x1 - x0; if(rw<0) rw = -rw;
		    rh = y1 - y0; if(rh<0) rh = -rh;
		    rx = x0 < x1 ? x0: x1;
		    ry = y0 < y1 ? y0: y1;
		    if(moved) EZ_IFBHighlightItemsInRectangle(widget, rx,ry,rw,rh);
		    XDrawRectangle(EZ_Display, win, gc, rx,ry,rw, rh);
		    dflag = 1;
		  }
		else if(xevent.type == ButtonRelease &&  xevent.xbutton.button == EZ_Btn3)
		  {
		    if(dflag) 
		      XDrawRectangle(EZ_Display, win, gc, rx,ry,rw, rh);		      
		    break;
		  }
		else if(xevent.type == Expose || xevent.type == FocusIn || xevent.type == FocusOut)
		  EZ_WidgetDispatchEvent(&xevent);
	      }
	    if(moved) /* select a item */
	      {
		EZ_Item ***data, **row, *item;
		int nrows = EZ_IFListBoxRows(widget);
		int i,j, ncols =  EZ_IFListBoxColumns(widget);
  
		data = EZ_IFListBoxData(widget);
		for(i = 1; i < nrows; i++)
		  {
		    row = data[i];
		    for(j = 0; j < ncols; j++)
		      {
			item = row[j];
			if(item && EZ_ItemHighlighted(item))
			  {
			    EZ_IFListBoxSelectionIDX(widget) = i;
			    EZ_IFListBoxSelectionCol(widget) = j;
			    EZ_HandleMotionCallBacks(widget,EZ_IFListBoxMotionCallBack(widget));
			    i = j = 9999999;
			  }
		      }
		  }
	      }
	  }
      }
    }
}

/*************************************************************
 *
 * remove highlight, row mode
 */
static void   EZ_IFListBoxClearHighlight(widget, Rx,Ry,Rw,Rh,smode)
     EZ_Widget *widget; int Rx,Ry,Rw,Rh; int smode;
{
  EZ_Item ***data, **row, *item;
  int nrows = EZ_IFListBoxRows(widget);
  int ncols =  EZ_IFListBoxColumns(widget);  
  Window   win = EZ_WidgetWindow(widget);
  int i,j;
  data = EZ_IFListBoxData(widget);
  
  if(smode)
    {
      for(i = 1; i < nrows; i++)
	{
	  row = data[i];
	  item = row[0];
	  if(item)
	    {
	      int hi = EZ_ItemHighlighted(item);
	      if(hi)
		{
		  EZ_ItemHighlighted(item) = 0;
		  EZ_UpdateOneRow(widget, win, row, ncols, Rx,Ry,Rw,Rh);
		}
	    }
	}
    }
  else
    {

      for(i = 1; i < nrows; i++)
	{
	  row = data[i];
	  for(j = 0; j < ncols; j++)
	    {
	      item = row[j];
	      if(item)
		{
		  int hi = EZ_ItemHighlighted(item);
		  if(hi)
		    {
		      EZ_ItemHighlighted(item) = 0;
		      EZ_UpdateOneItem(widget,EZ_WidgetWindow(widget),item, Rx,Ry,Rw,Rh);
		    }
		}
	    }
	}
    }
}
/*****************************************************************************************/

static void  EZ_IFBHighlightItemsInRectangle(widget, rx,ry,rw,rh)
     EZ_Widget *widget; int rx,ry,rw,rh;
{
  EZ_Item ***data, **row, *item;
  int i,j, bw, xoffset, yoffset, ww,hh,Rx,Ry,Rw,Rh, nrows, ncols, hmode;
  int *vsa = EZ_IFListBoxVSizes(widget);
  Window   win = EZ_WidgetWindow(widget);

  unsigned long fg = EZ_IFListBoxHighlightFG(widget);
  unsigned long bg = EZ_IFListBoxHighlightBG(widget);

  hmode = EZ_IFListBoxHighlightMode(widget);
  nrows = EZ_IFListBoxRows(widget);
  ncols =  EZ_IFListBoxColumns(widget);
  
  data = EZ_IFListBoxData(widget);
  bw = EZ_WidgetBorderWidth(widget) + EZ_WidgetPadB(widget);
  xoffset = bw + EZ_WidgetPadX(widget);
  yoffset = bw + EZ_WidgetPadY(widget);

  Rx = bw;
  Ry = yoffset + vsa[0];
  ww = EZ_WidgetWidth(widget);
  Rw = ww - (bw << 1);
  hh = EZ_WidgetHeight(widget);
  Rh = hh - bw - Ry;

  for(i = 1; i < nrows; i++)
    {
      row = data[i];
      for(j = 0; j < ncols; j++)
	{
	  item = row[j];
	  if(item)
	    {
	      int x0 = EZ_ItemOriginX(item);
	      int y0 = EZ_ItemOriginY(item);
	      int w0 = EZ_ItemAWidth(item);
	      int h0 = EZ_ItemAHeight(item);
	      int cx = x0 + (w0>>1);
	      int cy = y0 + (h0>>1);
	      if(cx >= rx && cx <= rx + rw && cy >= ry && cy <= ry + rh)
		{
		  int hi = EZ_ItemHighlighted(item);
		  if(hi == 0)
		    {
		      EZ_ItemHighlighted(item) = 1;
		      EZ_HighlightOneItem(widget, win, item, fg, bg, Rx,Ry,Rw,Rh, hmode);
		    }
		}
	      else
		{
		  int hi = EZ_ItemHighlighted(item);
		  if(hi != 0)		  
		    {
		      EZ_ItemHighlighted(item) = 0;
		      EZ_UpdateOneItem(widget,EZ_WidgetWindow(widget),item, Rx,Ry,Rw,Rh);
		    }
		}
	    }
	}
    }
}

static void  EZ_IFBHighlightRowsInRectangle(widget, rx,ry,rw,rh)
     EZ_Widget *widget; int rx,ry,rw,rh;
{
  EZ_Item ***data, **row, *item;
  int i, bw, xoffset, yoffset, ww,hh,Rx,Ry,Rw,Rh, nrows, ncols, hmode;
  int *vsa = EZ_IFListBoxVSizes(widget);
  Window   win = EZ_WidgetWindow(widget);

  unsigned long fg = EZ_IFListBoxHighlightFG(widget);
  unsigned long bg = EZ_IFListBoxHighlightBG(widget);

  hmode = EZ_IFListBoxHighlightMode(widget);
  nrows = EZ_IFListBoxRows(widget);
  ncols =  EZ_IFListBoxColumns(widget);
  
  data = EZ_IFListBoxData(widget);
  bw = EZ_WidgetBorderWidth(widget) + EZ_WidgetPadB(widget);
  xoffset = bw + EZ_WidgetPadX(widget);
  yoffset = bw + EZ_WidgetPadY(widget);

  Rx = bw;
  Ry = yoffset + vsa[0];
  ww = EZ_WidgetWidth(widget);
  Rw = ww - (bw << 1);
  hh = EZ_WidgetHeight(widget);
  Rh = hh - bw - Ry;

  for(i = 1; i < nrows; i++)
    {
      row = data[i];
      if((item = row[0]) != NULL)
	{
	  int x0 = EZ_ItemOriginX(item);
	  int y0 = EZ_ItemOriginY(item);
	  int w0 = EZ_ItemAWidth(item);
	  int h0 = EZ_ItemAHeight(item);
	  int cx = x0 + (w0>>1);
	  int cy = y0 + (h0>>1);
	  if(cx >= rx && cx <= rx + rw && cy >= ry && cy <= ry + rh)
	    {
	      int hi = EZ_ItemHighlighted(item);
	      if(hi == 0)
		{
		  EZ_ItemHighlighted(item) = 1;
		  EZ_HighlightOneRow(widget, win, row, ncols, fg, bg,
				     Rx,Ry,Rw,Rh, hmode); 
		}
	    }
	  else
	    {
	      int hi = EZ_ItemHighlighted(item);
	      if(hi != 0)		  
		{
		  EZ_ItemHighlighted(item) = 0;
		  EZ_UpdateOneRow(widget, win, row, ncols, Rx,Ry,Rw,Rh);
		}
	    }
	}
    }
}
/*******************************************************************************************/
static void ifbDnDNRows(widget, numrows, event, Rx,Ry,Rw,Rh) 
     EZ_Widget *widget; XEvent *event; int Rx,Ry,Rw,Rh; int numrows;
{
  int dndType = EZ_DND_OBJ_IS_WIDGET;
  void *dndObj = NULL;
  
  if(EZ_WidgetIsDnDSrc(widget)) dndObj = widget;
  
  if(dndObj != NULL) /* do dnd */
    {
      int       x, y, done, dcnt = 0;
      int       oldx=0, oldy=0, dx,dy, rootx, rooty,Sx,Sy, moved = 0;
      int       iww=1, ihh=1, snapshot = 0, grabed, tx,ty;
      int       wox,woy;
      XEvent    xevent;
      Window ptWin = event->xbutton.window;
      Window oldPtWin = ptWin;
      Window widgetwin = ptWin;
      Window root, child;
      unsigned int mask;
      unsigned int actionType = (event->xbutton.state) & (ShiftMask | ControlMask | Mod1Mask);
      int dragCancelled = 0;
      int helpRequested = 0;

      unsigned long serial = 0L;
      int newEvent = 0;
      done = 0;
      moved = 0;
      grabed = 0;
      x = event->xbutton.x;
      y = event->xbutton.y;

      while(!done)
	{
	  while(XPending(EZ_Display) && !done)
	    {
	      XNextEvent(EZ_Display, &xevent); 
	      if(EZ_FilterEvent(&xevent)) EZ_InvokePrivateEventHandler(&xevent);
	      newEvent = 1;
	      
	      if(xevent.type == MotionNotify)
		{
		  moved = 1;
		  serial = xevent.xmotion.serial;
		  ptWin = xevent.xmotion.window; 
		  if(xevent.xmotion.window == widgetwin)
		    {
		      if(xevent.xmotion.x < 0 ||  xevent.xmotion.y < 0 ||
			 xevent.xmotion.x >= EZ_WidgetWidth(widget) ||  
			 xevent.xmotion.y >= EZ_WidgetHeight(widget))
			ptWin = 0L;
		    }
		  if(helpRequested)
		    {
		      EZ_DnDSendCancelHelpMsg();
		      EZ_DnDInfo.ptWin = (unsigned long)ptWin;
		      helpRequested = 0;
		    }
		}
	      else if(xevent.type == ButtonRelease)
		{
		  if(xevent.xbutton.button == EZ_Btn3) 
		    {
		      ptWin = xevent.xbutton.window; 
		      if(xevent.xbutton.window == widgetwin)
			{
			  if(xevent.xbutton.x < 0 ||  xevent.xbutton.y < 0 ||
			     xevent.xbutton.x >= EZ_WidgetWidth(widget) ||  
			     xevent.xbutton.y >= EZ_WidgetHeight(widget))
			    ptWin = 0L;
			}
		      done = 1; 
		      break;
		    }
		}
	      else if(xevent.type == KeyPress)
		{
		  KeySym            keysym;
		  XComposeStatus    compose; 
		  char              tmpbuffer[4];
		  int               count;
		  count = XLookupString(&(xevent.xkey), tmpbuffer, 4, &keysym, &compose);
		  switch(keysym)
		    {
		    case XK_F1: 
#ifdef XK_KP_F1
		    case XK_KP_F1:  /* help */
#endif
		      if(grabed && helpRequested == 0 && ptWin != 0)
			{
			  EZ_DnDSendRequestHelpMsg();
			  EZ_DnDInfo.ptWin = (unsigned long)ptWin;
			  helpRequested = 1;
			}
		      break;
		    case XK_Escape:   /* cancel */
		      {
			dragCancelled = 1;
			done = 1;
		      }
		    break;
		    default:
		      break;
		    }
		}
	      else if(xevent.type == Expose || xevent.type == FocusIn || xevent.type == FocusOut)  
		EZ_WidgetDispatchEvent(&xevent);
	      if(oldPtWin != ptWin)
		{
		  if(grabed && oldPtWin != 0)
		    {
		      unsigned long twin = EZ_DnDInfo.ptWin;
		      EZ_DnDInfo.ptWin = (unsigned long)(oldPtWin);
		      EZ_DnDSendLeaveWindowMsg();
		      EZ_DnDInfo.ptWin = twin;
		    }
		  oldPtWin = ptWin;
		}
	    }
	  if(newEvent == 0)
	    {
	      EZ_CheckDnDTimer();
	      EZ_SitForALittleBit(1000);
	    }
			  
	  if(!done && newEvent) /* not done yet */
	    {
	      newEvent = 0;
	      if(moved)
		{
		  if(grabed == 0)  /* pointer moved, start drag */
		    {
		      XTranslateCoordinates(EZ_Display, widgetwin,
					    RootWindow(EZ_Display, EZ_ScreenNum),
					    0, 0,
					    &wox,&woy, &child);

		      oldx = wox + x;
		      oldy = woy + y;
		      Sx = oldx;
		      Sy = oldy;
				
		      EZ_InitDrag(serial, dndType, widgetwin,
				  widget, dndObj, Sx,Sy, iww, ihh, oldx, oldy, 0, 0);      
		      
		      if(EZ_DnDInfo.id == serial)
			{
			  EZ_CallDragInitProcedure();
			  if(EZ_DnDInfo.cursor == None)
			    EZ_DnDInfo.cursor =  EZ_WidgetDnDDragCursor(widget);
			  if(EZ_DnDInfo.cursor != None || EZ_DnDInfo.dragIcon != NULL)
			    snapshot = EZ_DND_DRAG_ICON_CURSOR;
			  else 
			    {
			      snapshot = EZ_DND_DRAG_ICON_SPECIAL;
			      EZ_IFListBoxMakeNRowOutLines(widget, numrows,wox,woy);
			    }
			  {
			    EZ_DnDInfo.actionType = actionType;
			    EZ_DnDInfo.px = oldx;
			    EZ_DnDInfo.py = oldy;
			    EZ_DnDInfo.dragIconType = snapshot;
			    {
			      char *smsg; int smsg_len;
			      EZ_GenerateDragStartedMessage(&smsg, &smsg_len);
			      EZ_BroadcastDnDMessage(EZ_DND_DRAG_START,smsg, smsg_len, 0);
			    }
			    /*EZ_BroadcastDnDMessage(EZ_DND_DRAG_START,NULL, 0, 0);*/
			  }
			  if(EZ_PointerGrabed == 0)  /* grab the pointer ! */
			    {
			      if(XGrabPointer(EZ_Display,widgetwin, True,
					      ButtonReleaseMask|ButtonPressMask|PointerMotionMask,
					      GrabModeAsync, GrabModeAsync, None,
					      EZ_DnDInfo.cursor, CurrentTime)
				 == GrabSuccess) EZ_PointerGrabed = 1;
			    }	
			  grabed = 1;
			}
		    }
		  if(grabed)
		    {
		      XQueryPointer(EZ_Display, RootWindow(EZ_Display, EZ_ScreenNum),
				    &root,       /* root return win */
				    &child,      /* child ret win   */
				    &rootx, &rooty,   /* x, y in root    */
				    &tx,&ty,            /* x, y in win     */
				    &mask);  
		      dx = rootx - oldx;  dy = rooty - oldy;
		      oldx = rootx;   oldy = rooty;
				  
		      if(dx | dy)
			{
			  if(snapshot == EZ_DND_DRAG_ICON_SPECIAL)
			    {
			      EZ_MoveNItemOutlines(dx,dy, 0, dcnt,1);
			      dcnt++;
			      EZ_MoveNItemOutlines(dx,dy, 1, dcnt,1);
			      XFlush(EZ_Display);
			    }
			  else if(EZ_DnDInfo.dragIcon != NULL) EZ_ShowDragIcon(rootx, rooty);
			  EZ_DnDInfo.px = oldx;
			  EZ_DnDInfo.py = oldy;
			  EZ_DnDInfo.ptWin = (unsigned long)ptWin;
			  EZ_BroadcastDnDMessage(EZ_DND_DRAG_MOTION, NULL, 0, 0);
			}
		    }
		}/* moved */
	    } /* not done */
	  else if(done) /* done */
	    {
	      if(EZ_PointerGrabed)
		{
		  XUngrabPointer(EZ_Display,CurrentTime);
		  EZ_PointerGrabed = 0;
		}
	      if(grabed) /* dragged */
		{
		  char *message;
		  int length;
		  Window commWin;
		  
		  if(snapshot == EZ_DND_DRAG_ICON_SPECIAL)
		    EZ_MoveNItemOutlines(0,0, 0, 1,1);
		  else if(EZ_DnDInfo.dragIcon != NULL) EZ_HideWidget(EZ_DnDInfo.dragIcon);
		  if(ptWin != 0) commWin = (Window) EZ_WindowIsDnDTarget(ptWin);
		  else commWin = 0L;
		  EZ_FinishDrag(commWin, ptWin, oldx, oldy); /* prepare for drop */

		  if(commWin != 0L)
		    {
		      if(helpRequested)
			{
			  EZ_DnDSendCancelHelpMsg();
			  helpRequested = 0;
			}
				      
		      if(dragCancelled == 0)
			{
			  EZ_DnDInfo.actionType = actionType;
			  EZ_GenerateDragIsDropedMessage(&message, &length);
			  EZ_SendDnDMessage(EZ_DND_DRAG_DROPPED, message, length, 1);
			}
		      else  /* cancelled */
			{
			  EZ_SendDnDMessage(EZ_DND_DRAG_CANCELLED, NULL, 0, 0);
			  EZ_AbortDrag();
			}
		    }	
		  else if(ptWin == RootWindow(EZ_Display, EZ_ScreenNum) &&
			  dragCancelled == 0)
		    {
		      /* drop to the root window, handle this internally */
		      EZ_DnDDataEncoder  *encoder = EZ_FindSpecialEncoder();
		      if(encoder)
			{
			  char *msg; int mlen, needFree, ok = EZ_DND_FAILURE;
					  
			  EZ_DnDInfo.srcStatus = EZ_DRAG_CONVERTING;
			  ok = (encoder->encoder) (encoder->object,
						   encoder->data,
						   &msg, &mlen, &needFree);
			  if(ok != EZ_DND_FAILURE)
			    {
			      if((encoder->callback).callback)
				{
				  EZ_DnDInfo.srcStatus = EZ_DRAG_EXECUTING_CALLBACK;
				  ((encoder->callback).callback)(encoder->object, 
								 (encoder->callback).data);
				}
			      EZ_DnDInfo.id = 0; /* forget about this drag */   
			    }
			  else EZ_AbortDrag();
			}
		      else  EZ_AbortDrag();
		    }
		  else    EZ_AbortDrag();
		}
	    }
	} /* while */
    }  /* item is a drag src */
}
/*******************************************************************************************
 *
 * Do dnd on one row (sline)
 */
static void ifbDnDOneRow(widget, sline, event, Rx,Ry,Rw,Rh) 
     EZ_Widget *widget; XEvent *event; int Rx,Ry,Rw,Rh; int sline;
{
  int i, ncols, dndType = 0;
  void  *dndObj = NULL;
  int rubberband = EZ_IFListBoxRubberBand(widget);
  EZ_Item **nsitems, *nsitem = NULL;

  ncols =  EZ_IFListBoxColumns(widget);
  nsitems = EZ_IFListBoxData(widget)[sline];
  
  if(rubberband)
    {
      nsitem = nsitems[0];
      if(nsitem && EZ_ItemIsDnDSrc(nsitem))
	{
	  dndType = EZ_DND_OBJ_IS_ITEM;
	  dndObj = nsitem;
	}
      else if(EZ_WidgetIsDnDSrc(widget))
	{
	  dndType = EZ_DND_OBJ_IS_WIDGET;
	  dndObj = widget;
	}
      if(nsitem == NULL)
	for(i = 0; i < ncols; i++)
	  {
	    nsitem = nsitems[i];
	    if(nsitem) break;
	  }
    }
  else
    {
      for(i = 0; i < ncols; i++)
	{
	  nsitem = nsitems[i];
	  if(nsitem) break;
	}
      if(nsitem && EZ_ItemIsDnDSrc(nsitem))
	{
	  dndType = EZ_DND_OBJ_IS_ITEM;
	  dndObj = nsitem;
	}
    }
  
  if(nsitem && dndObj != NULL) /* do dnd */
    {
      int       x, y, done;
      int       Ox=0, Oy=0, oldx=0, oldy=0, dx,dy, rootx, rooty,tx,ty,Sx,Sy, moved = 0;
      int       ix0, iy0, iww=1, ihh=1, iarea, snapshot = 0, grabed;
      int       wox,woy;
      XEvent    xevent;
      Window ptWin = event->xbutton.window;
      Window oldPtWin = ptWin;
      Window widgetwin = ptWin;
      Window root, child;
      unsigned int mask;
      unsigned int actionType = (event->xbutton.state) & (ShiftMask | ControlMask | Mod1Mask);
      int dragCancelled = 0;
      int helpRequested = 0;

      unsigned long serial = 0L;
      int newEvent = 0;
      done = 0;
      moved = 0;
      grabed = 0;
      x = event->xbutton.x;
      y = event->xbutton.y;

      while(!done)
	{
	  while(XPending(EZ_Display) && !done)
	    {
	      XNextEvent(EZ_Display, &xevent); 
	      if(EZ_FilterEvent(&xevent)) EZ_InvokePrivateEventHandler(&xevent);
	      newEvent = 1;
	      
	      if(xevent.type == MotionNotify)
		{
		  moved = 1;
		  serial = xevent.xmotion.serial;
		  ptWin = xevent.xmotion.window; 
		  if(xevent.xmotion.window == widgetwin)
		    {
		      if(xevent.xmotion.x < 0 ||  xevent.xmotion.y < 0 ||
			 xevent.xmotion.x >= EZ_WidgetWidth(widget) ||  
			 xevent.xmotion.y >= EZ_WidgetHeight(widget))
			ptWin = 0L;
		    }
		  if(helpRequested)
		    {
		      EZ_DnDSendCancelHelpMsg();
		      EZ_DnDInfo.ptWin = (unsigned long)ptWin;
		      helpRequested = 0;
		    }
		}
	      else if(xevent.type == ButtonRelease)
		{
		  if(xevent.xbutton.button == EZ_Btn3) 
		    {
		      ptWin = xevent.xbutton.window; 
		      if(xevent.xbutton.window == widgetwin)
			{
			  if(xevent.xbutton.x < 0 ||  xevent.xbutton.y < 0 ||
			     xevent.xbutton.x >= EZ_WidgetWidth(widget) ||  
			     xevent.xbutton.y >= EZ_WidgetHeight(widget))
			    ptWin = 0L;
			}
		      done = 1; 
		      break;
		    }
		}
	      else if(xevent.type == KeyPress)
		{
		  KeySym            keysym;
		  XComposeStatus    compose; 
		  char              tmpbuffer[4];
		  int               count;
		  count = XLookupString(&(xevent.xkey), tmpbuffer, 4, &keysym, &compose);
		  switch(keysym)
		    {
		    case XK_F1: 
#ifdef XK_KP_F1
		    case XK_KP_F1:  /* help */
#endif
		      if(grabed && helpRequested == 0 && ptWin != 0)
			{
			  EZ_DnDSendRequestHelpMsg();
			  EZ_DnDInfo.ptWin = (unsigned long)ptWin;
			  helpRequested = 1;
			}
		      break;
		    case XK_Escape:   /* cancel */
		      {
			dragCancelled = 1;
			done = 1;
		      }
		    break;
		    default:
		      break;
		    }
		}
	      else if(xevent.type == Expose || xevent.type == FocusIn || xevent.type == FocusOut)  
		EZ_WidgetDispatchEvent(&xevent);
	      if(oldPtWin != ptWin)
		{
		  if(grabed && oldPtWin != 0)
		    {
		      unsigned long twin = EZ_DnDInfo.ptWin;
		      EZ_DnDInfo.ptWin = (unsigned long)(oldPtWin);
		      EZ_DnDSendLeaveWindowMsg();
		      EZ_DnDInfo.ptWin = twin;
		    }
		  oldPtWin = ptWin;
		}
	    }
	  if(newEvent == 0)
	    {
	      EZ_CheckDnDTimer();
	      EZ_SitForALittleBit(1000);
	    }
			  
	  if(!done && newEvent) /* not done yet */
	    {
	      newEvent = 0;
	      if(moved)
		{
		  if(grabed == 0)  /* pointer moved, start drag */
		    {
		      XTranslateCoordinates(EZ_Display, widgetwin,
					    RootWindow(EZ_Display, EZ_ScreenNum),
					    0, 0,
					    &wox,&woy, &child);
		      ix0 = Rx + 1;
		      iy0 = EZ_ItemOriginY(nsitem);
		      ihh = EZ_ItemAHeight(nsitem);
		      iww = Rw - 2;
		      iarea  = iww * ihh;
				      
		      tx = ix0 - x;
		      ty = iy0 - y;
		      oldx = wox + x;
		      oldy = woy + y;
		      Sx = Ox = oldx + tx;
		      Sy = Oy = oldy + ty;
				
		      EZ_InitDrag(serial, dndType, widgetwin,
				  widget, dndObj, Sx,Sy, iww, ihh, oldx, oldy, tx, ty);      
		      
		      if(EZ_DnDInfo.id == serial)
			{
			  EZ_CallDragInitProcedure();
			  if(EZ_DnDInfo.cursor == None)
			    EZ_DnDInfo.cursor =  EZ_ItemDnDDragCursor(nsitem);
			  if(EZ_DnDInfo.cursor != None || EZ_DnDInfo.dragIcon != NULL)
			    snapshot = EZ_DND_DRAG_ICON_CURSOR;
			  else if(iarea < 40000)
			    snapshot = EZ_MakeSnapShot(widget,1,ix0,iy0,iww,ihh);
			  else snapshot = EZ_DND_DRAG_ICON_OUTLINE;
			  {
			    EZ_DnDInfo.actionType = actionType;
			    EZ_DnDInfo.px = oldx;
			    EZ_DnDInfo.py = oldy;
			    EZ_DnDInfo.dragIconType = snapshot;
			    {
			      char *smsg; int smsg_len;
			      EZ_GenerateDragStartedMessage(&smsg, &smsg_len);
			      EZ_BroadcastDnDMessage(EZ_DND_DRAG_START,smsg, smsg_len, 0);
			    }
			    /*EZ_BroadcastDnDMessage(EZ_DND_DRAG_START,NULL, 0, 0);*/
			  }
			  if(EZ_PointerGrabed == 0)  /* grab the pointer ! */
			    {
			      if(XGrabPointer(EZ_Display,widgetwin, True,
					      ButtonReleaseMask|ButtonPressMask|PointerMotionMask,
					      GrabModeAsync, GrabModeAsync, None,
					      EZ_DnDInfo.cursor, CurrentTime)
				 == GrabSuccess) EZ_PointerGrabed = 1;
			    }	
			  grabed = 1;
			}
		    }
		  if(grabed)
		    {
		      XQueryPointer(EZ_Display, RootWindow(EZ_Display, EZ_ScreenNum),
				    &root,       /* root return win */
				    &child,      /* child ret win   */
				    &rootx, &rooty,   /* x, y in root    */
				    &tx,&ty,            /* x, y in win     */
				    &mask);  
		      dx = rootx - oldx;  dy = rooty - oldy;
		      oldx = rootx;   oldy = rooty;
				  
		      if(dx | dy)
			{
			  Ox += dx; Oy += dy;
			  if(snapshot != EZ_DND_DRAG_ICON_CURSOR)
			    EZ_MoveSnapShotOrOutline(snapshot, Ox, Oy, iww, ihh, 1);
			  else if(EZ_DnDInfo.dragIcon != NULL) EZ_ShowDragIcon(rootx, rooty);
			  EZ_DnDInfo.px = oldx;
			  EZ_DnDInfo.py = oldy;
			  EZ_DnDInfo.ptWin = (unsigned long)ptWin;
			  EZ_BroadcastDnDMessage(EZ_DND_DRAG_MOTION, NULL, 0, 0);
			  if(snapshot != EZ_DND_DRAG_ICON_CURSOR)
			    EZ_MoveSnapShotOrOutline(snapshot, Ox, Oy, iww, ihh, 0);
			}
		    }
		}/* moved */
	    } /* not done */
	  else if(done) /* done */
	    {
	      if(EZ_PointerGrabed)
		{
		  XUngrabPointer(EZ_Display,CurrentTime);
		  EZ_PointerGrabed = 0;
		}
	      if(grabed) /* dragged */
		{
		  char *message;
		  int length;
		  Window commWin;

		  if(snapshot != EZ_DND_DRAG_ICON_CURSOR)				  
		    EZ_MoveSnapShotOrOutline(snapshot, 0,0,0,0,1);
		  else if(EZ_DnDInfo.dragIcon != NULL) EZ_HideWidget(EZ_DnDInfo.dragIcon);
		  if(ptWin != 0) commWin = (Window) EZ_WindowIsDnDTarget(ptWin);
		  else commWin = 0L;
		  EZ_FinishDrag(commWin, ptWin, oldx, oldy); /* prepare for drop */

		  if(commWin != 0L)
		    {
		      if(helpRequested)
			{
			  EZ_DnDSendCancelHelpMsg();
			  helpRequested = 0;
			}
				      
		      if(dragCancelled == 0)
			{
			  EZ_DnDInfo.actionType = actionType;
			  EZ_GenerateDragIsDropedMessage(&message, &length);
			  EZ_SendDnDMessage(EZ_DND_DRAG_DROPPED, message, length, 1);
			}
		      else  /* cancelled */
			{
			  EZ_SendDnDMessage(EZ_DND_DRAG_CANCELLED, NULL, 0, 0);
			  EZ_AbortDrag();
			}
		    }	
		  else if(ptWin == RootWindow(EZ_Display, EZ_ScreenNum) &&
			  dragCancelled == 0)
		    {
		      /* drop to the root window, handle this internally */
		      EZ_DnDDataEncoder  *encoder = EZ_FindSpecialEncoder();
		      if(encoder)
			{
			  char *msg; int mlen, needFree, ok = EZ_DND_FAILURE;
					  
			  EZ_DnDInfo.srcStatus = EZ_DRAG_CONVERTING;
			  ok = (encoder->encoder) (encoder->object,
						   encoder->data,
						   &msg, &mlen, &needFree);
			  if(ok != EZ_DND_FAILURE)
			    {
			      if((encoder->callback).callback)
				{
				  EZ_DnDInfo.srcStatus = EZ_DRAG_EXECUTING_CALLBACK;
				  ((encoder->callback).callback)(encoder->object, 
								 (encoder->callback).data);
				}
			      EZ_DnDInfo.id = 0; /* forget about this drag */   
			    }
			  else EZ_AbortDrag();
			}
		      else  EZ_AbortDrag();
		    }
		  else    EZ_AbortDrag();
		}
	    }
	} /* while */
    }  /* item is a drag src */
}
/*************************************************************/
static int ifboxGetNumberOfHiglightedRows(widget)
     EZ_Widget *widget;
{
  EZ_Item ***data, **row, *item;
  int counter, i, nrows = EZ_IFListBoxRows(widget); 
  data = EZ_IFListBoxData(widget);  

  for(counter = 0, i = 0; i < nrows; i++)
    {
      row = data[i];
      item = row[0];
      if(item && EZ_ItemHighlighted(item)) counter++;
    }
  return(counter);
}
static int ifboxGetNumberOfHiglightedItems(widget)
     EZ_Widget *widget;
{
  EZ_Item ***data, **row, *item;
  int counter, i, j, nrows = EZ_IFListBoxRows(widget); 
  int ncols =  EZ_IFListBoxColumns(widget);
  data = EZ_IFListBoxData(widget);  

  for(counter = 0, i = 0; i < nrows; i++)
    {
      row = data[i];
      for(j = 0; j < ncols; j++)
	{
	  item = row[j];
	  if(item && EZ_ItemHighlighted(item)) counter++;
	}
    }
  return(counter);
}
/*************************************************************/
static void ifbDnDNItems(widget, nnn, event, Rx,Ry,Rw,Rh) 
     EZ_Widget *widget; XEvent *event; int Rx,Ry,Rw,Rh; int nnn;
{
  Window ptWin = event->xbutton.window;
  Window oldPtWin = ptWin;
  Window widgetwin = ptWin;
  unsigned int actionType = (event->xbutton.state) & (ShiftMask | ControlMask | Mod1Mask);
  int dragCancelled = 0;
  int helpRequested = 0;
  int       oldx=0, oldy=0, dx,dy, rootx, rooty,Sx, Sy, tx,ty;
  int       snapshot = 0;
  Window    root, child;
  unsigned  int    mask;
  int       dndType = 0;
  void      *dndObj = NULL;

  /* to make gcc happy */
  { oldx = oldy = 1; }
  if( EZ_WidgetIsDnDSrc(widget)) { dndObj = widget; dndType = EZ_DND_OBJ_IS_WIDGET;}
  if(dndObj)
    {
      XEvent xevent;
      unsigned long serial = 0L;
      int newEvent = 0, dcnt = 0;
      int x,y, wox, woy, done = 0;
      int moved = 0;
      int grabed = 0;
	  
      x = event->xbutton.x;
      y = event->xbutton.y;
      while(!done)
	{
	  while(XPending(EZ_Display) && !done)
	    {
	      XNextEvent(EZ_Display, &xevent); 
	      if(EZ_FilterEvent(&xevent)) EZ_InvokePrivateEventHandler(&xevent);
	      newEvent = 1;
				  
	      if(xevent.type == MotionNotify) 
		{
		  moved = 1; 
		  serial = xevent.xmotion.serial; 
		  ptWin = xevent.xmotion.window; 
		  if(xevent.xmotion.window == widgetwin)
		    {
		      if(xevent.xmotion.x < 0 ||  xevent.xmotion.y < 0 ||
			 xevent.xmotion.x >= EZ_WidgetWidth(widget) ||  
			 xevent.xmotion.y >= EZ_WidgetHeight(widget))
			ptWin = 0L;
		    }
		  if(helpRequested)
		    {
		      EZ_DnDSendCancelHelpMsg();
		      EZ_DnDInfo.ptWin = (unsigned long)ptWin;
		      helpRequested = 0;
		    }
		}
	      else if(xevent.type == ButtonRelease)
		{
		  if(xevent.xbutton.button == EZ_Btn3) 
		    {
		      ptWin = xevent.xbutton.window; 					
		      if(xevent.xbutton.window == widgetwin)
			{
			  if(xevent.xbutton.x < 0 ||  xevent.xbutton.y < 0 ||
			     xevent.xbutton.x >= EZ_WidgetWidth(widget) ||  
			     xevent.xbutton.y >= EZ_WidgetHeight(widget))
			    ptWin = 0L;
			}
		      done = 1;
		      break;
		    }
		}
	      else if(xevent.type == KeyPress)
		{
		  KeySym            keysym;
		  XComposeStatus    compose; 
		  char              tmpbuffer[4];
		  int               count;
		  count = XLookupString(&(xevent.xkey), tmpbuffer, 4, &keysym, &compose);
		  switch(keysym)
		    {
		    case XK_F1: 
#ifdef XK_KP_F1
		    case XK_KP_F1:  /* help */
#endif
		      if(grabed && helpRequested == 0 && ptWin != 0)
			{
			  EZ_DnDInfo.ptWin = (unsigned long)ptWin;
			  EZ_DnDSendRequestHelpMsg();
			  helpRequested = 1;
			}
		      break;
		    case XK_Escape:   /* cancel */
		      {
			dragCancelled = 1;
			done = 1;
		      }
		    break;
		    default:
		      break;
		    }
		}
	      else if(xevent.type == Expose || xevent.type == FocusIn || xevent.type == FocusOut)  
		EZ_WidgetDispatchEvent(&xevent);

	      if(oldPtWin != ptWin)
		{
		  if(grabed && oldPtWin != 0)
		    {
		      unsigned long twin = EZ_DnDInfo.ptWin;
		      EZ_DnDInfo.ptWin = (unsigned long)(oldPtWin);
		      EZ_DnDSendLeaveWindowMsg();
		      EZ_DnDInfo.ptWin = twin;
		    }
		  oldPtWin = ptWin;
		}
	    }
	  if(newEvent == 0)
	    {
	      EZ_CheckDnDTimer();
	      EZ_SitForALittleBit(1000);
	    }
		      
	  if(!done && newEvent) /* not done yet */			      
	    {
	      newEvent = 0;
	      if(moved)
		{
		  if(grabed == 0)  /* pointer moved, start drag */
		    {
		      XTranslateCoordinates(EZ_Display, widgetwin,
					    RootWindow(EZ_Display, EZ_ScreenNum),
					    0, 0,
					    &wox,&woy, &child);
			  
		      oldx = wox + x;
		      oldy = woy + y;
		      Sx = oldx;
		      Sy = oldy;

		      EZ_InitDrag(serial, dndType, widgetwin,
				  widget, dndObj, Sx,Sy, 100, 100, oldx, oldy, 0, 0);
		      if(EZ_DnDInfo.id == serial)
			{  
			  EZ_CallDragInitProcedure();
			  if(EZ_DnDInfo.cursor == None)
			    EZ_DnDInfo.cursor =  EZ_WidgetDnDDragCursor(widget);
			  if(EZ_DnDInfo.cursor != None || EZ_DnDInfo.dragIcon != NULL)
			    snapshot = EZ_DND_DRAG_ICON_CURSOR;
			  else 
			    {
			      snapshot = EZ_DND_DRAG_ICON_SPECIAL;
			      EZ_IFListBoxMakeNItemOutLines(widget, nnn,wox,woy);
			    }
			  {
			    EZ_DnDInfo.actionType = actionType;
			    EZ_DnDInfo.px = oldx;
			    EZ_DnDInfo.py = oldy;
			    EZ_DnDInfo.dragIconType = snapshot;
			    {
			      char *smsg; int smsg_len;
			      EZ_GenerateDragStartedMessage(&smsg, &smsg_len);
			      EZ_BroadcastDnDMessage(EZ_DND_DRAG_START,smsg, smsg_len, 0);
			    }
			    /*EZ_BroadcastDnDMessage(EZ_DND_DRAG_START,NULL, 0, 0);*/
			  }
			  if(EZ_PointerGrabed == 0)  /* grab the pointer ! */
			    {
			      if(XGrabPointer(EZ_Display,widgetwin, True,
					      ButtonReleaseMask|ButtonPressMask|
					      PointerMotionMask,
					      GrabModeAsync, GrabModeAsync, None, 
					      EZ_DnDInfo.cursor, CurrentTime)
				 == GrabSuccess) EZ_PointerGrabed = 1;
			    }	
			  grabed = 1;
			}
		    }
		  if(grabed)
		    {
		      XQueryPointer(EZ_Display, RootWindow(EZ_Display, EZ_ScreenNum),
				    &root,       /* root return win */
				    &child,      /* child ret win   */
				    &rootx, &rooty,   /* x, y in root    */
				    &tx,&ty,            /* x, y in win     */
				    &mask);  
		      dx = rootx - oldx;  dy = rooty - oldy;
		      oldx = rootx;   oldy = rooty;
		      if(dx | dy)
			{
			  if(snapshot == EZ_DND_DRAG_ICON_SPECIAL)
			    {
			      EZ_MoveNItemOutlines(dx,dy, 0, dcnt,1);
			      dcnt++;
			      EZ_MoveNItemOutlines(dx,dy, 1, dcnt,1);
			      XFlush(EZ_Display);
			    }
			  else if(EZ_DnDInfo.dragIcon != NULL) EZ_ShowDragIcon(rootx, rooty);
			  EZ_DnDInfo.px = oldx;
			  EZ_DnDInfo.py = oldy;
			  EZ_DnDInfo.ptWin = (unsigned long)ptWin;
			  EZ_BroadcastDnDMessage(EZ_DND_DRAG_MOTION, NULL, 0, 0);
			}
		    }
		}/* moved */
	    } /* not done */
	  else if(done)/* done */
	    {
	      if(EZ_PointerGrabed)
		{
		  XUngrabPointer(EZ_Display,CurrentTime);
		  EZ_PointerGrabed = 0;
		}
	      if(grabed) /* dragged */
		{
		  char *message;
		  int length;
		  Window commWin;
		      
		  if(snapshot == EZ_DND_DRAG_ICON_SPECIAL)
		    EZ_MoveNItemOutlines(0,0, 0, 1,1);
		  else if(EZ_DnDInfo.dragIcon != NULL) EZ_HideWidget(EZ_DnDInfo.dragIcon);
		  if(ptWin != 0) commWin = (Window) EZ_WindowIsDnDTarget(ptWin);
		  else commWin = 0L;
		  EZ_FinishDrag(commWin, ptWin, oldx, oldy); /* prepare for drop */
		      
		  if(commWin != 0L)
		    {
		      if(helpRequested)
			{
			  EZ_DnDSendCancelHelpMsg();
			  helpRequested = 0;
			}
					  
		      if(dragCancelled == 0)
			{
			  EZ_DnDInfo.actionType = actionType;
			  EZ_GenerateDragIsDropedMessage(&message, &length);
			  EZ_SendDnDMessage(EZ_DND_DRAG_DROPPED, message, length, 1);
			}
		      else  /* cancelled */
			{
			  EZ_SendDnDMessage(EZ_DND_DRAG_CANCELLED, NULL, 0, 0);
			  EZ_AbortDrag();
			}
		    }
		  else if(ptWin == RootWindow(EZ_Display, EZ_ScreenNum) &&
			  dragCancelled == 0)
		    {
		      /* drop to the root window, handle this internally */
		      EZ_DnDDataEncoder  *encoder = EZ_FindSpecialEncoder();
		      if(encoder)
			{
			  char *msg; int mlen, needFree, ok = EZ_DND_FAILURE;
					      
			  EZ_DnDInfo.srcStatus = EZ_DRAG_CONVERTING;
			  ok = (encoder->encoder) (encoder->object,
						   encoder->data,
						   &msg, &mlen, &needFree);
			  if(ok != EZ_DND_FAILURE)
			    {
			      if((encoder->callback).callback)
				{
				  EZ_DnDInfo.srcStatus = EZ_DRAG_EXECUTING_CALLBACK;
				  ((encoder->callback).callback)(encoder->object, 
								 (encoder->callback).data);
				}
			      EZ_DnDInfo.id = 0; /* forget about this drag */   
			    }
			  else EZ_AbortDrag();
			}
		      else  EZ_AbortDrag();
		    }
		  else EZ_AbortDrag();
		}
	    }
	} /* while */
    }  /* if dndObj */
}
/*************************************************************/
static void ifbDnDOneItem(widget, nsitem, event, Rx,Ry,Rw,Rh) 
     EZ_Widget *widget; XEvent *event; int Rx,Ry,Rw,Rh; EZ_Item *nsitem;
{
  Window ptWin = event->xbutton.window;
  Window oldPtWin = ptWin;
  Window widgetwin = ptWin;
  unsigned int actionType = (event->xbutton.state) & (ShiftMask | ControlMask | Mod1Mask);
  int dragCancelled = 0;
  int helpRequested = 0;
  int       Ox=0, Oy=0, oldx=0, oldy=0, dx,dy, rootx, rooty,tx,ty,Sx,Sy;
  int       ix0, iy0, iww=1, ihh=1, iarea, snapshot = 0;
  Window    root, child;
  unsigned  int    mask;
  int       dndType = 0;
  void      *dndObj = NULL;

  /* to make gcc happy */
  { oldx = oldy = 1; iww = ihh = 1; }
		  
  if(nsitem)
    {
      if( EZ_ItemIsDnDSrc(nsitem)) { dndObj = nsitem; dndType = EZ_DND_OBJ_IS_ITEM;}
      else if( EZ_WidgetIsDnDSrc(widget)) { dndObj = widget; dndType = EZ_DND_OBJ_IS_WIDGET;}
      if(dndObj)
	{
	  XEvent xevent;
	  unsigned long serial = 0L;
	  int newEvent = 0;
	  int x,y, wox, woy, done = 0;
	  int moved = 0;
	  int grabed = 0;
	  
	  x = event->xbutton.x;
	  y = event->xbutton.y;
	  while(!done)
	    {
	      while(XPending(EZ_Display) && !done)
		{
		  XNextEvent(EZ_Display, &xevent); 
		  if(EZ_FilterEvent(&xevent))  EZ_InvokePrivateEventHandler(&xevent);
		  newEvent = 1;
				  
		  if(xevent.type == MotionNotify) 
		    {
		      moved = 1; 
		      serial = xevent.xmotion.serial; 
		      ptWin = xevent.xmotion.window; 
		      if(xevent.xmotion.window == widgetwin)
			{
			  if(xevent.xmotion.x < 0 ||  xevent.xmotion.y < 0 ||
			     xevent.xmotion.x >= EZ_WidgetWidth(widget) ||  
			     xevent.xmotion.y >= EZ_WidgetHeight(widget))
			    ptWin = 0L;
			}
		      if(helpRequested)
			{
			  EZ_DnDSendCancelHelpMsg();
			  EZ_DnDInfo.ptWin = (unsigned long)ptWin;
			  helpRequested = 0;
			}
		    }
		  else if(xevent.type == ButtonRelease)
		    {
		      if(xevent.xbutton.button == EZ_Btn3) 
			{
			  ptWin = xevent.xbutton.window; 					
			  if(xevent.xbutton.window == widgetwin)
			    {
			      if(xevent.xbutton.x < 0 ||  xevent.xbutton.y < 0 ||
				 xevent.xbutton.x >= EZ_WidgetWidth(widget) ||  
				 xevent.xbutton.y >= EZ_WidgetHeight(widget))
				ptWin = 0L;
			    }
			  done = 1;
			  break;
			}
		    }
		  else if(xevent.type == KeyPress)
		    {
		      KeySym            keysym;
		      XComposeStatus    compose; 
		      char              tmpbuffer[4];
		      int               count;
		      count = XLookupString(&(xevent.xkey), tmpbuffer, 4, &keysym, &compose);
		      switch(keysym)
			{
			case XK_F1: 
#ifdef XK_KP_F1
			case XK_KP_F1:  /* help */
#endif
			  if(grabed && helpRequested == 0 && ptWin != 0)
			    {
			      EZ_DnDInfo.ptWin = (unsigned long)ptWin;
			      EZ_DnDSendRequestHelpMsg();
			      helpRequested = 1;
			    }
			  break;
			case XK_Escape:   /* cancel */
			  {
			    dragCancelled = 1;
			    done = 1;
			  }
			break;
			default:
			  break;
			}
		    }
		  else if(xevent.type == Expose || xevent.type == FocusIn || xevent.type == FocusOut)  
		    EZ_WidgetDispatchEvent(&xevent);

		  if(oldPtWin != ptWin)
		    {
		      if(grabed && oldPtWin != 0)
			{
			  unsigned long twin = EZ_DnDInfo.ptWin;
			  EZ_DnDInfo.ptWin = (unsigned long)(oldPtWin);
			  EZ_DnDSendLeaveWindowMsg();
			  EZ_DnDInfo.ptWin = twin;
			}
		      oldPtWin = ptWin;
		    }
		}
	      if(newEvent == 0)
		{
		  EZ_CheckDnDTimer();
		  EZ_SitForALittleBit(1000);
		}
		      
	      if(!done && newEvent) /* not done yet */			      
		{
		  newEvent = 0;
		  if(moved)
		    {
		      if(grabed == 0)  /* pointer moved, start drag */
			{
			  XTranslateCoordinates(EZ_Display, widgetwin,
						RootWindow(EZ_Display, EZ_ScreenNum),
						0, 0,
						&wox,&woy, &child);
			  
			  ix0 = EZ_ItemOriginX(nsitem);
			  iy0 = EZ_ItemOriginY(nsitem);
			  iww = EZ_ItemAWidth(nsitem);
			  ihh = EZ_ItemAHeight(nsitem);
			  iarea  = iww * ihh;
					  
			  tx = ix0 - x;
			  ty = iy0 - y;
			  oldx = wox + x;
			  oldy = woy + y;
			  Sx = Ox = oldx + tx;
			  Sy = Oy = oldy + ty;

			  EZ_InitDrag(serial, dndType, widgetwin,
				      widget, dndObj, Sx,Sy, iww, ihh, oldx, oldy, tx, ty);
			  if(EZ_DnDInfo.id == serial)
			    {  
			      EZ_CallDragInitProcedure();
			      if(EZ_DnDInfo.cursor == None)
				EZ_DnDInfo.cursor =  EZ_ItemDnDDragCursor(nsitem);
			      if(EZ_DnDInfo.cursor != None || EZ_DnDInfo.dragIcon != NULL)
				snapshot = EZ_DND_DRAG_ICON_CURSOR;
			      else if(iarea < 40000)
				snapshot = EZ_MakeSnapShot(widget,1,ix0,iy0,iww,ihh);
			      else snapshot = EZ_DND_DRAG_ICON_OUTLINE;
			      {
				EZ_DnDInfo.actionType = actionType;
				EZ_DnDInfo.px = oldx;
				EZ_DnDInfo.py = oldy;
				EZ_DnDInfo.dragIconType = snapshot;
				{
				  char *smsg; int smsg_len;
				  EZ_GenerateDragStartedMessage(&smsg, &smsg_len);
				  EZ_BroadcastDnDMessage(EZ_DND_DRAG_START,smsg, smsg_len, 0);
				}
				/*EZ_BroadcastDnDMessage(EZ_DND_DRAG_START,NULL, 0, 0);*/
			      }
			      if(EZ_PointerGrabed == 0)  /* grab the pointer ! */
				{
				  if(XGrabPointer(EZ_Display,widgetwin, True,
						  ButtonReleaseMask|ButtonPressMask|
						  PointerMotionMask,
						  GrabModeAsync, GrabModeAsync, None, 
						  EZ_DnDInfo.cursor, CurrentTime)
				     == GrabSuccess) EZ_PointerGrabed = 1;
				}	
			      grabed = 1;
			    }
			}
		      if(grabed)
			{
			  XQueryPointer(EZ_Display, RootWindow(EZ_Display, EZ_ScreenNum),
					&root,       /* root return win */
					&child,      /* child ret win   */
					&rootx, &rooty,   /* x, y in root    */
					&tx,&ty,            /* x, y in win     */
					&mask);  
			  dx = rootx - oldx;  dy = rooty - oldy;
			  oldx = rootx;   oldy = rooty;
			  if(dx | dy)
			    {
			      Ox += dx; Oy += dy;
			      if(snapshot != EZ_DND_DRAG_ICON_CURSOR)
				EZ_MoveSnapShotOrOutline(snapshot, Ox, Oy, iww, ihh, 1);
			      else if(EZ_DnDInfo.dragIcon != NULL) EZ_ShowDragIcon(rootx, rooty);
			      EZ_DnDInfo.px = oldx;
			      EZ_DnDInfo.py = oldy;
			      EZ_DnDInfo.ptWin = (unsigned long)ptWin;
			      EZ_BroadcastDnDMessage(EZ_DND_DRAG_MOTION, NULL, 0, 0);
			      if(snapshot != EZ_DND_DRAG_ICON_CURSOR)
				EZ_MoveSnapShotOrOutline(snapshot, Ox, Oy, iww, ihh, 0);
			    }
			}
		    }/* moved */
		} /* not done */
	      else if(done)/* done */
		{
		  if(EZ_PointerGrabed)
		    {
		      XUngrabPointer(EZ_Display,CurrentTime);
		      EZ_PointerGrabed = 0;
		    }
		  if(grabed) /* dragged */
		    {
		      char *message;
		      int length;
		      Window commWin;
		      
		      if(snapshot != EZ_DND_DRAG_ICON_CURSOR)				      
			EZ_MoveSnapShotOrOutline(snapshot, 0,0,0,0,1);
		      else if(EZ_DnDInfo.dragIcon != NULL) EZ_HideWidget(EZ_DnDInfo.dragIcon);
		      if(ptWin != 0) commWin = (Window) EZ_WindowIsDnDTarget(ptWin);
		      else commWin = 0L;
		      EZ_FinishDrag(commWin, ptWin, oldx, oldy); /* prepare for drop */
		      
		      if(commWin != 0L)
			{
			  if(helpRequested)
			    {
			      EZ_DnDSendCancelHelpMsg();
			      helpRequested = 0;
			    }
					  
			  if(dragCancelled == 0)
			    {
			      EZ_DnDInfo.actionType = actionType;
			      EZ_GenerateDragIsDropedMessage(&message, &length);
			      EZ_SendDnDMessage(EZ_DND_DRAG_DROPPED, message, length, 1);
			    }
			  else  /* cancelled */
			    {
			      EZ_SendDnDMessage(EZ_DND_DRAG_CANCELLED, NULL, 0, 0);
			      EZ_AbortDrag();
			    }
			}
		      else if(ptWin == RootWindow(EZ_Display, EZ_ScreenNum) &&
			      dragCancelled == 0)
			{
			  /* drop to the root window, handle this internally */
			  EZ_DnDDataEncoder  *encoder = EZ_FindSpecialEncoder();
			  if(encoder)
			    {
			      char *msg; int mlen, needFree, ok = EZ_DND_FAILURE;
					      
			      EZ_DnDInfo.srcStatus = EZ_DRAG_CONVERTING;
			      ok = (encoder->encoder) (encoder->object,
						       encoder->data,
						       &msg, &mlen, &needFree);
			      if(ok != EZ_DND_FAILURE)
				{
				  if((encoder->callback).callback)
				    {
				      EZ_DnDInfo.srcStatus = EZ_DRAG_EXECUTING_CALLBACK;
				      ((encoder->callback).callback)(encoder->object, 
								     (encoder->callback).data);
				    }
				  EZ_DnDInfo.id = 0; /* forget about this drag */   
				}
			      else EZ_AbortDrag();
			    }
			  else  EZ_AbortDrag();
			}
		      else EZ_AbortDrag();
		    }
		}
	    } /* while */
	}  /* if dndObj */
    }
}

/*************************************************************/
int *EZ_IFListBoxGetHighlightedRowsA(widget, cnt_ret)
     EZ_Widget *widget; int *cnt_ret;
{
  int smode = EZ_IFListBoxSelectionType(widget);
  if(smode)
    {
      int cnt = ifboxGetNumberOfHiglightedRows(widget);
      if(cnt>0)
	{
	  EZ_Item ***data, **row, *item;
	  int *ret, i, nrows = EZ_IFListBoxRows(widget); 
	  ret = (int *)malloc( (cnt+1)*sizeof(int));
	  data = EZ_IFListBoxData(widget);  
	  
	  for(cnt = 0, i = 0; i < nrows; i++)
	    {
	      row = data[i];
	      item = row[0];
	      if(item && EZ_ItemHighlighted(item))
		ret[cnt++] = i;
	    }
	  if(cnt_ret) *cnt_ret = cnt;
	  return(ret);
	}
    }
  return(NULL);
}
/*************************************************************/
int  EZ_IFListBoxGetHighlightedRows(widget, ary, siz)
     EZ_Widget *widget; int *ary, siz;
{
  int cnt  = 0;
  int *hrows = EZ_IFListBoxGetHighlightedRowsA(widget, &cnt);
  if(hrows)
    {
      int i, total = cnt <siz ? cnt: siz;
      for(i = 0; i < total; i++) ary[i] = hrows[i];
      (void)free(hrows);
    }
  return(cnt);
}
/*************************************************************/
EZ_Item **EZ_IFListBoxGetHighlightedItemsA(widget, cnt_ret)
     EZ_Widget *widget; int *cnt_ret;
{
  int smode = EZ_IFListBoxSelectionType(widget);
  if(smode)
    {
      int cnt = ifboxGetNumberOfHiglightedRows(widget);
      if(cnt>0)
	{
	  EZ_Item ***data, **row, *item, **ret;
	  int i, kk, nrows = EZ_IFListBoxRows(widget); 
	  ret = (EZ_Item **)malloc( (cnt+1)*sizeof(EZ_Item *));
	  data = EZ_IFListBoxData(widget);  
	  for(kk = 0, i = 0; i < nrows && kk < cnt; i++)
	    {
	      row = data[i];
	      item = row[0];
	      if(item && EZ_ItemHighlighted(item))
		ret[kk++] = item;
	    }
	  if(cnt_ret) *cnt_ret = kk;
	  return(ret);
	}
    }
  else
    {
      int cnt = ifboxGetNumberOfHiglightedItems(widget);
      if(cnt > 0)
	{
	  EZ_Item ***data, **row, *item, **ret;
	  int kk, i, j, nrows = EZ_IFListBoxRows(widget); 
	  int ncols =  EZ_IFListBoxColumns(widget);
	  data = EZ_IFListBoxData(widget);  
	  ret = (EZ_Item **)malloc( (cnt+1)*sizeof(EZ_Item *));
	  for(kk=0,i = 0; i < nrows && kk < cnt; i++)
	    {
	      row = data[i];
	      for(j = 0; j < ncols; j++)
		{
		  item = row[j];
		  if(item && EZ_ItemHighlighted(item)) 
		    ret[kk++] = item;
		}
	    }
	  if(cnt_ret) *cnt_ret = kk;
	  return(ret);
	}
    }
  return(NULL);
}
/*************************************************************/
int EZ_IFListBoxGetHighlightedItems(widget, ary, siz)
     EZ_Widget *widget; EZ_Item **ary; int siz;
{
  int cnt = 0;
  EZ_Item **hitems = EZ_IFListBoxGetHighlightedItemsA(widget, &cnt);
  if(hitems)
    {
      int i, total = cnt < siz? cnt: siz;
      for(i = 0; i < total; i++) ary[i] = hitems[i];
      (void)free(hitems);
    }
  return(cnt);
}
/*************************************************************/
int EZ_IFListBoxGetHithlightedItemsAndIndices(widget, itm_ret, r_ret, c_ret)
     EZ_Widget *widget; EZ_Item ***itm_ret; int **r_ret, **c_ret;
{
  int smode = EZ_IFListBoxSelectionType(widget);
  if(smode)
    {
      int cnt = ifboxGetNumberOfHiglightedRows(widget);
      if(cnt>0)
	{
	  EZ_Item ***data, **row, *item, **iret = NULL;
	  int *row_ret= NULL, *col_ret = NULL;
	  int i, kk, nrows = EZ_IFListBoxRows(widget); 
	  
	  if(itm_ret) { *itm_ret = iret = (EZ_Item **)malloc( (cnt+1)*sizeof(EZ_Item *));}
	  if(r_ret) {*r_ret = row_ret = (int *)malloc( (cnt+1) * sizeof(int));}
	  if(c_ret) {*c_ret = col_ret = (int *)malloc( (cnt+1) * sizeof(int));}
	  data = EZ_IFListBoxData(widget);  

	  for(kk = 0, i = 0; i < nrows && kk < cnt; i++)
	    {
	      row = data[i];
	      item = row[0];
	      if(item && EZ_ItemHighlighted(item))
		{
		  if(iret) iret[kk] = item;
		  if(row_ret) row_ret[kk] = i;
		  if(col_ret) col_ret[kk] = 0;
		  kk++;
		}
	    }
	  return(kk);
	}
    }
  else
    {
      int cnt = ifboxGetNumberOfHiglightedItems(widget);
      if(cnt > 0)
	{
	  EZ_Item ***data, **row, *item, **iret = NULL;
	  int *row_ret=NULL, *col_ret=NULL;
	  int kk, i, j, nrows = EZ_IFListBoxRows(widget); 
	  int ncols =  EZ_IFListBoxColumns(widget);

	  data = EZ_IFListBoxData(widget);  

	  if(itm_ret) { *itm_ret = iret = (EZ_Item **)malloc( (cnt+1)*sizeof(EZ_Item *));}
	  if(r_ret) {*r_ret = row_ret = (int *)malloc( (cnt+1) * sizeof(int));}
	  if(c_ret) {*c_ret = col_ret = (int *)malloc( (cnt+1) * sizeof(int));}
	  data = EZ_IFListBoxData(widget);  

	  for(kk = 0, i = 0; i < nrows && kk < cnt; i++)
	    {
	      row = data[i];
	      for(j = 0; j < ncols; j++)
		{
		  item = row[j];
		  if(item && EZ_ItemHighlighted(item))
		    {
		      if(iret) iret[kk] = item;
		      if(row_ret) row_ret[kk] = i;
		      if(col_ret) col_ret[kk] = j;
		      kk++;
		    }
		}
	    }
	  return(kk);
	}
    }
  return(0);  
}
/*************************************************************/
#undef _EZ_WIDGET_IFLIST_BOX_C_
