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
 *
 *  Display items
 *
 *****************************************************************/
#define _EZ_ITEM_C_

#include "EZ_Widget.h"
#include "EZ_WidgetConfig.h"

/*****************************************************************
 *
 * public functions
 */
EZ_Item *EZ_AllocateNewDisplayItem MY_ANSIARGS((void));
EZ_Item *EZ_DuplicateItem MY_ANSIARGS((EZ_Item *item));

void    EZ_ComputeItemSize MY_ANSIARGS((
					EZ_Item *item,
					int *w_ret, int *h_ret));
void    EZ_DisplayOneItem MY_ANSIARGS((EZ_Widget *widget,
				       Drawable  drawable,
				       int       x0, int y0,
				       EZ_Item   *item,
				       int       Rx, int Ry, int  Rw, int Rh));
void    EZ_UpdateOneItem MY_ANSIARGS((
				      EZ_Widget *widget,
				      Drawable  drawable,
				      EZ_Item   *item,
				      int       Rx, int Ry, int  Rw, int Rh));

void    EZ_HighlightOneItem MY_ANSIARGS((
					 EZ_Widget *widget,
					 Drawable  drawable,
					 EZ_Item   *item,
					 unsigned long fg,  unsigned long bg, 
					 int       Rx, int Ry, int  Rw, int Rh, int flag));
void    EZ_DestroyItem MY_ANSIARGS((EZ_Item *item));
void    EZ_DestroyItemFinal MY_ANSIARGS((EZ_Item *item));
int     EZ_GetItemType MY_ANSIARGS((EZ_Item *item));
void    EZ_GetItemXYOffset MY_ANSIARGS((EZ_Item *item, int *xoff, int *yoff));
void    EZ_GetItemBwAndXYPading MY_ANSIARGS((EZ_Item *item, int *bw, int *xpad, int *ypad));
int     EZ_ScaleItem MY_ANSIARGS((EZ_Item *item));


void    EZ_GetItemClientData MY_ANSIARGS((EZ_Item *item,
					  int *idata_ret,
					  void **pdata_ret));
int     EZ_GetItemIntData MY_ANSIARGS((EZ_Item *item));
void    *EZ_GetItemPtrData MY_ANSIARGS((EZ_Item *item));

void     EZ_SetItemClientData MY_ANSIARGS((EZ_Item *item,
					  int idata, void *pdata ));
void     EZ_SetItemIntData MY_ANSIARGS((EZ_Item *item, int i));
void     EZ_SetItemPtrData MY_ANSIARGS((EZ_Item *item, void *p));


void    EZ_ComputeItemArrayGeometry MY_ANSIARGS((EZ_Item ***array,
						 int nrows,
						 int ncolumns,
						 int *w_ret, int *h_ret,
						 int *w_det_ret, int *h_det_ret));
void    EZ_DisplayOneRowOfItems MY_ANSIARGS((
					     EZ_Widget *widget,
					     Drawable drawable,
					     EZ_Item **row,
					     int nelts,
					     int x0,int y0,int hsep,
					     int *w_sizes,
					     int Rx,int Ry,int Rw,int Rh,
					     unsigned long fg, unsigned long bg, 
					     int hmode, int selmode
					     ));
void    EZ_UpdateOneRow MY_ANSIARGS((
				     EZ_Widget *widget,
				     Drawable drawable,
				     EZ_Item **row,
				     int nelts,
				     int cx,int cy,int cw, int ch));
void    EZ_HighlightOneRow MY_ANSIARGS((
					EZ_Widget *widget,
					Drawable drawable,
					EZ_Item **row,
					int nelts,
					unsigned long fg, unsigned long bg,
					int cx,int cy,int cw, int ch, int hmode));

int     EZ_PickAnItem MY_ANSIARGS(( EZ_Item *item, int x, int y));

void    EZ_ItemConfigureSentienl MY_ANSIARGS(( EZ_Item *item, int dirty));

void    EZ_AddItemCallBack MY_ANSIARGS((EZ_Item *item, int which,
					EZ_CallBack callback, void *cdata,
					int where));
void    EZ_AddItemCallBackInternal MY_ANSIARGS((EZ_Item *item, int which,
						EZ_CallBack callback, void *cdata,
						int where));
void    EZ_RemoveItemCallBack MY_ANSIARGS((EZ_Item *item,  int which,
					   EZ_CallBack callback, void *cdata));
void    EZ_RemoveAllItemCallBacks MY_ANSIARGS((EZ_Item *item, int which));

EZ_Widget *EZ_GetItemHostWidget MY_ANSIARGS((EZ_Item *item));

void   EZ_UpdateDisplayItem  MY_ANSIARGS((EZ_Item *item));

int  EZ_FillItemBackground MY_ANSIARGS((EZ_Widget *wptr, EZ_Item *item, Drawable d, int x0, int y0,
					int w0, int h0, int x1, int y1,
					unsigned long *bpv));
/*****************************************************************/
int     EZ_GetItemUnknownData MY_ANSIARGS((EZ_Item *item, int idx,
					   EZ_UnknownDataType  *ret));
void    EZ_SetItemUnknownData  MY_ANSIARGS((EZ_Item *item, int idx,
					    EZ_UnknownDataType data));
EZ_UnknownDataType *EZ_GetItemUnknownDataPtr MY_ANSIARGS((EZ_Item *item));
/*****************************************************************/
void    EZ_SetItemPosition MY_ANSIARGS((EZ_Item *item, int x, int y));
void    EZ_SetItemWidth MY_ANSIARGS((EZ_Item *item, int width));
void    EZ_SetItemHeight MY_ANSIARGS((EZ_Item *item, int height));
void    EZ_SetItemDimension MY_ANSIARGS((EZ_Item *item, int w, int h));
int     EZ_GetItemWidth MY_ANSIARGS((EZ_Item *item));
int     EZ_GetItemHeight MY_ANSIARGS((EZ_Item *item));
void    EZ_GetItemDimension MY_ANSIARGS((EZ_Item *item, int *w, int *h));
void    EZ_GetItemADimension MY_ANSIARGS((EZ_Item *item, int *w, int *h));
void    EZ_GetItemPosition MY_ANSIARGS((EZ_Item *item, int *x, int *y));
void    EZ_GetItemAPosition MY_ANSIARGS((EZ_Item *item, int *x, int *y));

void EZ_SetItemAttributes MY_ANSIARGS((EZ_Item *item, int option, int i1, int i2, 
				       void *p1, void *p2, 
				       unsigned long ul1, unsigned long ul2,
				       int i3, int i4));
/*****************************************************************/
static  EZ_CallBackStruct **EZ_GetItemCallbackType  MY_ANSIARGS((EZ_Item *item, int which));
/*****************************************************************/
/* void EZ_ConfigureItem MY_VARARGS(void *,  the_args) */

/*****************************************************************
 *
 * Global Data
 */
EZ_DpyItemHandle   *EZ_DpyItemHandlers[EZ_TOTAL_NUMBER_OF_ITEMS];

/*****************************************************************/
extern int  EZ_CounterUT;

EZ_Item *EZ_AllocateNewDisplayItem()
{
  EZ_Item *item;
  
  item = (EZ_Item *)my_malloc(sizeof(EZ_Item), _DISPLAY_ITEM_);
  (void) memset(item,0, sizeof(EZ_Item));
  if(item)
    {
      EZ_ItemBlocked(item)   = 0;
      EZ_ItemType(item)      = 0;
      EZ_ItemParentWidget(item)    = (EZ_Widget *)NULL;
      EZ_ItemCallback(item)  = (EZ_CallBackStruct *)NULL;
      EZ_ItemDestroyCallback(item)  = (EZ_CallBackStruct *)NULL;

      EZ_ItemDirItemInfo(item) = NULL;
      
      EZ_ItemOriginX(item)   = 0;
      EZ_ItemOriginY(item)   = 0;    
      EZ_ItemWidth(item)     = 2;    
      EZ_ItemHeight(item)    = 2;    
      EZ_ItemAOriginX(item)  = 0;
      EZ_ItemAOriginY(item)  = 0;    
      EZ_ItemAWidth(item)    = 2;    
      EZ_ItemAHeight(item)   = 2;    
      EZ_ItemDirty(item)     = 1;
      EZ_ItemVisible(item)   = 1;
      EZ_ItemBorderType(item) =  EZ_BORDER_NONE;
      EZ_ItemBorderWidth(item)  = 0;
      EZ_ItemPadX(item) = 0;
      EZ_ItemPadY(item) = 0;
      EZ_ItemAnchor(item) = EZ_CENTER;

      EZ_ItemForeground(item) = EZ_DefaultForeground;
      EZ_ItemBackground(item) = 0;
      EZ_ItemFont(item) = EZ_GetFontFromId(EZ_LABEL_FONT);
      EZ_ItemBitmap(item)    = (EZ_Bitmap *)NULL;     

      EZ_ItemDnDDragCursor(item) = (Cursor)NULL;
      EZ_ItemDnDDataEncoders(item) = (EZ_DnDDataEncoder *)NULL;
      EZ_ItemDnDDataDecoders(item) = (EZ_DnDDataDecoder *)NULL;
      EZ_ItemDnDTargetHelp(item) = (char *)NULL;

      EZ_InsertItemToHashTable(item);

      /* application data */
      EZ_ItemIntData(item) = 0;
      EZ_ItemPtrData(item) = (void *)NULL;

      EZ_ItemNPValues(item) = 0;
      EZ_ItemPValues(item) =  NULL;
    }
  else EZ_OutOfMemory("EZ_AllocateNewDisplayItem");
  
  return(item);
}
/*****************************************************************
 *
 *  Figure out the dimension of a common item.
 */
void EZ_ComputeItemSize(item, w_ret, h_ret)
     EZ_Item *item;
     int *w_ret, *h_ret;
{
  if(item)
    {
      int type = EZ_ItemType(item);
      
      (EZ_DpyItemHandlers[type]->computeSize)(item, w_ret, h_ret);
    }
  else
    {
      *w_ret = 0;
      *h_ret = 0;
    }
}
/*****************************************************************
 *
 * Decide whether or not (x,y) is over item.
 */

int EZ_PickAnItem(item,x,y)
     EZ_Item *item;
     int x,y;
{
  int ans = 0;

  if(item)
    {
      int type = EZ_ItemType(item);
      ans = (EZ_DpyItemHandlers[type]->pick)(item, x, y);
    }
  return(ans);
}

/*****************************************************************
 *
 *  Display an item in a drawable related to widget at (x0,y0)
 *  widget is used to figure out the default background !
 */
void EZ_DisplayOneItem(widget, drawable, x0, y0, item, Rx, Ry, Rw,Rh)    
     EZ_Widget *widget;
     Drawable  drawable;
     EZ_Item   *item;
     int       x0, y0;
     int       Rx, Ry, Rw,Rh; /* confined region in drawable */
{
  if(item)
    {
      int type = EZ_ItemType(item);
      
      (EZ_DpyItemHandlers[type]->draw)(widget, drawable, x0, y0, item, Rx, Ry, Rw,Rh);
    }
}

/******************************************************************
 *
 *  create a pixmap, draw the only item in it and copy it to drawable.
 *  Assuming the geometry has been computed.
 */
void EZ_UpdateOneItem(widget, drawable, item,cx,cy,cw,ch)    
     EZ_Widget *widget;
     Drawable  drawable;
     EZ_Item   *item;
     int cx,cy,cw,ch;     /* confined region in drawable */
{
  if(item && EZ_ItemVisible(item))
    {
      int type = EZ_ItemType(item);
      
      (EZ_DpyItemHandlers[type]->update)(widget, drawable, item, cx, cy, cw, ch);
    }
}

/***********************************************************************
 *
 * For highlighting, only fg and bg color are reset for highlighting.
 */
void EZ_HighlightOneItem(widget, drawable, item, fg, bg, cx,cy,cw,ch, flag)    
     EZ_Widget *widget;
     Drawable  drawable;
     EZ_Item   *item;
     unsigned long fg,bg; /* ~0 ~0 for default setting */
     int cx,cy,cw,ch;     /* confined region in drawable */
     int flag;            /* method to draw a highligted item */
{
  if(item)
    {
      int type = EZ_ItemType(item);
      
      (EZ_DpyItemHandlers[type]->highlight)(widget, drawable, item, fg, bg, 
					    cx, cy, cw, ch, flag);
    }
}

/*********************************************************************/  
void  EZ_GetItemXYOffset(item, xoff, yoff)
     EZ_Item *item;
     int *xoff, *yoff;
{
  if(item)
    {
      int w = EZ_ItemWidth(item);
      int h = EZ_ItemHeight(item);
      int aw = EZ_ItemAWidth(item);
      int ah = EZ_ItemAHeight(item);      
      int dw = (aw - w);
      int dh = (ah - h);
      int x, y;
      
      switch(EZ_ItemAnchor(item))
	{
	case EZ_LEFT:
	  x = 0;
	  y = dh/2;
	  break;
	case EZ_RIGHT:
	  x =  dw;
	  y = dh/2;
	  break;
	case EZ_TOP:
	  y = 0;
	  x = dw/2;
	  break;
	case EZ_BOTTOM:
	  y = dh;
	  x = dw/2;
	  break;
	case EZ_TOP_LEFT:
	  y = 0;
	  x = 0;
	  break;
	case EZ_TOP_RIGHT:
	  y = 0;
	  x = dw;
	  break;
	case EZ_BOTTOM_LEFT:
	  y = dh;
	  x = 0;
	  break;
	case EZ_BOTTOM_RIGHT:
	  y = dh;
	  x = dw;
	  break;
	case EZ_CENTER:
	  x = dw/2;
	  y = dh/2;
	  break;
	default:
	  x = 0; y = 0;
	  break;	
	}
      *xoff = x; *yoff = y;
    }
}

/*********************************************************************/  
     
void  EZ_GetItemBwAndXYPading(item, bw, xpad, ypad)
     EZ_Item *item;
     int *bw, *xpad, *ypad;
{
  if(item)
    {
      int bd = (int)EZ_ItemBorderWidth(item); 
      int xx = (int)EZ_ItemPadX(item);
      int yy = (int)EZ_ItemPadY(item);

      *bw = bd;
      *xpad = bd+xx;
      *ypad = bd+yy;
    }
  else
    {
      *bw = 0;
      *xpad = 0;
      *ypad = 0;
    }
}

/*****************************************************************
 *
 *  given an array of LabelItems, compute the geometry.
 */
void EZ_ComputeItemArrayGeometry(array, nrows, ncolumns, w_ret, h_ret, w_det_ret, h_det_ret)
     EZ_Item ***array;
     int nrows;          /* n rows, include the first row */
     int ncolumns;
     int *w_ret, *h_ret; /* total with and height */
     int *w_det_ret, *h_det_ret;  /* width and height of columns and rows */
{
  int xsize[256];
  int i,j, w, h, hh, totalH = 0, totalW = 0, parity;
  EZ_Item **row;

  for(i = 0; i < ncolumns; i++) xsize[i] = 0;

  /* pass I: compute the geometry of all items, set y-size */

  for(parity = 0, j = 0; j < nrows; j++, parity = !parity)
    {
      row = array[j];
      hh = 0;
      for(i = 0 ; i < ncolumns; i++)
	{
	  EZ_Item *item = row[i];
	  if(item)
	    {
	      EZ_ComputeItemSize(item, &w, &h);
	      EZ_ItemRowParity(item) =  parity;
	      if(hh < h) hh = h;
	      if(xsize[i] < w) xsize[i] = w;
	    }
	}
      for(i = 0 ; i < ncolumns; i++)
	if(row[i]) EZ_ItemAHeight(row[i]) = hh;
      h_det_ret[j] = hh;
      totalH += hh;
    }
  
  /* pass II: set x-size */
  for(j = 0; j < nrows; j++)
    {
      row = array[j];
      for(i = 0 ; i < ncolumns; i++)
	if(row[i]) EZ_ItemAWidth(row[i]) = xsize[i];
    }
  for(i = 0 ; i < ncolumns; i++)
    {
      w_det_ret[i] = xsize[i];
      totalW += xsize[i];
    }
  
  *w_ret = totalW;
  *h_ret = totalH;
}
/*********************************************************************/
void EZ_DisplayOneRowOfItems(widget, drawable, row, nelts, x0,y0, hsep, w_sizes, Rx,Ry,Rw,Rh,
			     fg, bg, hmode, selmode)
     EZ_Widget *widget;
     Drawable drawable;
     EZ_Item **row;
     int nelts;
     int x0,y0,hsep;
     int Rx,Ry,Rw,Rh, *w_sizes;
     unsigned long fg, bg;
     int selmode, hmode;
{
  int i;
  int x = x0, y = y0;
  EZ_Item *item;

  for( i = 0; i < nelts; i++)
    {
      item = row[i];
      if(item) 
	{
	  if(selmode == 0) /* selection is set to one item a time */
	    {
	      int hi = EZ_ItemHighlighted(item);
	      EZ_DisplayOneItem(widget, drawable, x, y, item, Rx,Ry,Rw,Rh);
	      if(hi) EZ_HighlightOneItem(widget, drawable, item, fg, bg, Rx,Ry,Rw,Rh,hmode);
	    }
	  else EZ_DisplayOneItem(widget, drawable, x, y, item, Rx,Ry,Rw,Rh);
	}
      x += w_sizes[i] + hsep;
    }
}

void EZ_DisplayHeaderRowIFLB(widget, drawable, row, nelts, x0,y0, hsep, w_sizes, Rx,Ry,Rw,Rh,
			     fg, bg, hmode, selmode)
     EZ_Widget *widget;
     Drawable drawable;
     EZ_Item **row;
     int nelts;
     int x0,y0,hsep;
     int Rx,Ry,Rw,Rh, *w_sizes;
     unsigned long fg, bg;
     int selmode, hmode;
{
  int i, total;
  int x = x0, y = y0;
  EZ_Item *item;

  total = nelts;
  for( i = 0; i < total; i++)
    {
      item = row[i];
      if(item) 
	{
	  EZ_ItemAWidth(item) += hsep;
	  if(selmode == 0) /* selection is set to one item a time */
	    {
	      int hi = EZ_ItemHighlighted(item);
	      EZ_DisplayOneItem(widget, drawable, x, y, item, Rx,Ry,Rw,Rh);
	      if(hi) EZ_HighlightOneItem(widget, drawable, item, fg, bg, Rx,Ry,Rw,Rh,hmode);
	    }
	  else EZ_DisplayOneItem(widget, drawable, x, y, item, Rx,Ry,Rw,Rh);
	  EZ_ItemAWidth(item) -= hsep;
	}
      x += w_sizes[i] + hsep;
    }
}

/*********************************************************************
 *
 * Redisplay a row of items.
 */
void EZ_UpdateOneRow(widget, drawable, row, nelts, cx,cy,cw,ch)
     EZ_Widget *widget;
     Drawable drawable;
     EZ_Item **row;
     int nelts;
     int cx,cy,cw,ch;     /* confined area to draw */
{
  if(row && widget)
    {
      EZ_Item *item;
      int i, h = -1, x0,y0 = -1, w = -1, tx = -999999;
      Pixmap pixmap;

      for(i = 0 ; i < nelts; i++)
	{
	  item = row[i];
	  if(item)
	    {
	      if(tx == -999999) tx = EZ_ItemOriginX(item);
	      w = EZ_ItemOriginX(item) + EZ_ItemAWidth(item);
	      if(h < 0)
		{
		  h = EZ_ItemAHeight(item);
		  y0 = EZ_ItemOriginY(item);
		}
	    }
	}
      if(h > 0)
	{
	  int x_r,y_r,w_r,h_r;

	  x0 = tx;
	  w -= x0;
	  EZ_IntersectRectangles(cx,cy,cw,ch, 
				 x0,y0,w,h,
				 &x_r,&y_r, &w_r, &h_r);
	  if(w_r > 0)
	    {
	      pixmap = XCreatePixmap(EZ_Display, drawable,cw,h,EZ_Depth);
	      EZ_FillItemBackground(widget, row[0], pixmap, x0, y0, cw, h, 0, 0, NULL);
	      XCopyArea(EZ_Display, pixmap, drawable, EZ_WRITABLEGC,0,y_r-y0,w_r,h_r, x_r,y_r);
	      XFreePixmap(EZ_Display,pixmap);
      
	      for(i = 0 ; i < nelts; i++)
		{
		  item = row[i];
		  if(item) EZ_UpdateOneItem(widget, drawable, item,cx,cy,cw,ch);
		}
	    }
	}
    }
}
/*********************************************************************/
void EZ_HighlightOneRow(widget, drawable, row, nelts, fg, bg,cx,cy,cw,ch, hmode)
     EZ_Widget *widget;
     Drawable drawable;
     EZ_Item **row;
     int nelts;
     unsigned long fg, bg; /* ~0 ~0 for default */
     int cx,cy,cw,ch;
     int hmode;            /* highlight mode, not currently used */
{
  if(row && widget)
    {
      EZ_Item *item;
      int i, w=-1, h=-1, x0,y0=0, tx = -999999;
      Pixmap pixmap;
      GC gc;

      for(i = 0 ; i < nelts; i++)
	{
	  item = row[i];
	  if(item)
	    {
	      if(tx == -999999) tx = EZ_ItemOriginX(item);
	      w = EZ_ItemOriginX(item) + EZ_ItemAWidth(item);
	      if(h < 0)
		{
		  h = EZ_ItemAHeight(item);
		  y0 = EZ_ItemOriginY(item);
		}
	    }
	}
      if(h > 0)
	{
	  int x_r,y_r,w_r,h_r;
	  
	  x0 = tx;
	  w -= x0;
	  EZ_IntersectRectangles(cx,cy,cw,ch, 
				 x0,y0,w,h,
				 &x_r,&y_r, &w_r, &h_r);
	  if(w_r > 0)
	    {
	      pixmap = XCreatePixmap(EZ_Display, drawable,cw,h,EZ_Depth);
	      gc = EZ_WRITABLEGC;
	      if(bg == ~0) EZ_GetBrightBDpv(widget, &bg);
	      XSetForeground(EZ_Display, gc, bg);
	      XFillRectangle(EZ_Display, pixmap, gc, 0, 0, cw,h);
	      XCopyArea(EZ_Display, pixmap, drawable, EZ_WRITABLEGC,0,y_r-y0,w_r,h_r, x_r,y_r);
	      XFreePixmap(EZ_Display,pixmap);
      
	      for(i = 0 ; i < nelts; i++)
		{
		  item = row[i];  /* highlightmode must be 0 or items may be partially highlighted */
		  if(item) EZ_HighlightOneItem(widget, drawable, item, fg, bg, cx,cy,cw,ch, 0);
		}
	    }
	}
    }
}
/*********************************************************************/
void EZ_UpdateDisplayItem(item)
     EZ_Item *item;
{
  if(EZ_LookupItemFromHT(item) != NULL)  
    {
      EZ_ItemDirty(item) = 0;
      EZ_ItemConfigureSentienl(item,1);
    }
}
/*********************************************************************/

int EZ_FillItemBackground(wptr, item, drawable, x0, y0, w0, h0, x1, y1, bpv_ret)
     EZ_Widget *wptr; Drawable drawable;  int x0, y0,w0, h0; /* rectangle in wptr'win */
     int x1, y1; /* UL corner of drawable to draw */
     unsigned long *bpv_ret;
     EZ_Item *item;
{
  int type = (int) EZ_WidgetType(wptr);
  unsigned long bgpv = 0;
  int tile = 0;
  GC  gc = EZ_WRITABLEGC;
  
  switch(type)
    {
    case EZ_WIDGET_SPREAD_SHEET:
      bgpv = EZ_SSheetTextBG(wptr);
      if(bgpv == ~0)tile = EZ_GetBackgroundGC(wptr, &gc, &bgpv, 0, 0);
      if(tile)  
	{
	  int ox, oy;
	  EZ_GetWidgetTileOrigin(wptr, &ox, &oy);      
	  XSetTSOrigin(EZ_Display, gc, ox+x1-x0, oy+y1-y0);
	}
      else
	{
	  if(bgpv == ~0) EZ_GetBackgroundPV(wptr, &bgpv, 0, 0);
	  XSetForeground(EZ_Display, gc, bgpv);      
	}
      XFillRectangle(EZ_Display, drawable, gc, x1, y1, w0, h0);  
      if(bpv_ret) *bpv_ret = bgpv;
      return(tile);
      break;
    case EZ_WIDGET_IWORK_AREA:
      bgpv = EZ_IWorkAreaTextBG(wptr);
      if(bgpv == ~0)tile = EZ_GetBackgroundGC(wptr, &gc, &bgpv, 0, 0);
      if(tile)  
	{
	  int ox, oy;
	  EZ_GetWidgetTileOrigin(wptr, &ox, &oy);      
	  XSetTSOrigin(EZ_Display, gc, ox+x1-x0, oy+y1-y0);
	}
      else
	{
	  if(bgpv == ~0) EZ_GetBackgroundPV(wptr, &bgpv, 0, 0);
	  XSetForeground(EZ_Display, gc, bgpv);      
	}
      XFillRectangle(EZ_Display, drawable, gc, x1, y1, w0, h0);  
      if(bpv_ret) *bpv_ret = bgpv;
      return(tile);
      break;
    case EZ_WIDGET_ITREE:
      if(item && EZ_ITreeWRowBgSet(wptr))
	{
	  int rowp = EZ_ItemRowParity(item);
	  unsigned long *bgs = EZ_ITreeWRowBg(wptr);
	  bgpv = bgs[rowp];
	  if(bgpv)
	    {
	      XSetForeground(EZ_Display, gc, bgpv);      
	      XFillRectangle(EZ_Display, drawable, gc, x1, y1, w0, h0);  
	      if(bpv_ret) *bpv_ret = bgpv;
	    }
	  else
	    {
	      GC darkgc, brightgc, normalgc;
	      tile = EZ_GetDarkBrightNormalBDGC(wptr, &darkgc, &brightgc, &normalgc);
	      if(rowp)
		{
		  if(tile)
		    {
                      int x, y;
                      EZ_GetWidgetTileOrigin(wptr, &x, &y);      
		      XSetTSOrigin(EZ_Display, normalgc, x+x1-x0, y+y1-y0);
		    }
		  XFillRectangle(EZ_Display, drawable, normalgc, x1, y1, w0, h0);  
		}
	      else
		{
		  if(tile)
		    {
                      int x, y;
                      EZ_GetWidgetTileOrigin(wptr, &x, &y);      
		      XSetTSOrigin(EZ_Display, brightgc, x+x1-x0, y+y1-y0);
		    }
		  XFillRectangle(EZ_Display, drawable, brightgc, x1, y1, w0, h0);  
		}
	    }
          return(0);
	}
      bgpv = EZ_ITreeWTextBG(wptr);
      if(bgpv == ~0)tile = EZ_GetBackgroundGC(wptr, &gc, &bgpv, 0, 0);
      if(tile)  
	{
	  int ox, oy;
	  EZ_GetWidgetTileOrigin(wptr, &ox, &oy);      
	  XSetTSOrigin(EZ_Display, gc, ox+x1-x0, oy+y1-y0);
	}
      else
	{
	  EZ_GetBackgroundPV(wptr, &bgpv, 0, 0);
	  XSetForeground(EZ_Display, gc, bgpv);      
	}
      XFillRectangle(EZ_Display, drawable, gc, x1, y1, w0, h0);  
      if(bpv_ret) *bpv_ret = bgpv;
      return(tile);      
      break;
    case EZ_WIDGET_IFANCY_LIST_BOX:
      if(item && EZ_IFListBoxSelectionType(wptr) && EZ_IFListBoxRowBGSet(wptr))
	{
	  int rowp = EZ_ItemRowParity(item);
	  unsigned long *bgs = EZ_IFListBoxRowBG(wptr);
	  bgpv = bgs[rowp];
	  if(bgpv)
	    {
	      XSetForeground(EZ_Display, gc, bgpv);      
	      XFillRectangle(EZ_Display, drawable, gc, x1, y1, w0, h0);  
	      if(bpv_ret) *bpv_ret = bgpv;
	    }
	  else
	    {
	      GC darkgc, brightgc, normalgc;
	      tile = EZ_GetDarkBrightNormalBDGC(wptr, &darkgc, &brightgc, &normalgc);
	      if(rowp)
		{
		  if(tile)
		    {
                      int x, y;
                      EZ_GetWidgetTileOrigin(wptr, &x, &y);      
		      XSetTSOrigin(EZ_Display, normalgc, x+x1-x0, y+y1-y0);
		    }
		  XFillRectangle(EZ_Display, drawable, normalgc, x1, y1, w0, h0);  
		}
	      else
		{
		  if(tile)
		    {
                      int x, y;
                      EZ_GetWidgetTileOrigin(wptr, &x, &y);      
		      XSetTSOrigin(EZ_Display, brightgc, x+x1-x0, y+y1-y0);
		    }
		  XFillRectangle(EZ_Display, drawable, brightgc, x1, y1, w0, h0);  
		}
	    }
	  return(0);
	}
      /* default */
      bgpv = EZ_IFListBoxTextBG(wptr);
      if(bgpv == ~0) tile = EZ_GetBackgroundGC(wptr, &gc, &bgpv, 0, 0);
      if(tile)  
	{
	  int ox, oy;
	  EZ_GetWidgetTileOrigin(wptr, &ox, &oy);      
	  XSetTSOrigin(EZ_Display, gc, ox+x1-x0, oy+y1-y0);
	}
      else 
	{
	  EZ_GetBackgroundPV(wptr, &bgpv, 0, 0);
	  XSetForeground(EZ_Display, gc, bgpv);      
	}
      XFillRectangle(EZ_Display, drawable, gc, x1, y1, w0, h0);  
      if(bpv_ret) *bpv_ret = bgpv;
      return(tile);      
      break;
    default:
      break;
    }
  return(0);
}
/*********************************************************************/
      
void  EZ_ItemConfigureSentienl(item,dirty)
     EZ_Item *item; int dirty;
{
  if(item)
    {
      if(EZ_ItemDirty(item) == 0)
	{
	  if(dirty)
	    {
	      EZ_Widget *parent = EZ_ItemParentWidget(item);	      
	      EZ_ItemDirty(item) = 1;

	      if(parent && EZ_LookupWidgetFromAllHT(parent) == parent)
		{
		  int wtype = EZ_WidgetType(parent);
		  switch(wtype)
		    {
		    case EZ_WIDGET_ITREE:
		      EZ_ITreeWDirty(parent) = 1;		      
		      break;
		    case EZ_WIDGET_IFANCY_LIST_BOX:
		      EZ_IFListBoxDirty(parent) = 1;
		      break;
		    case EZ_WIDGET_IWORK_AREA:
		      EZ_IWorkAreaDirty(parent) = 1;
		      break;
		    default:
		      break;
		    }
		  EZ_ClearWidgetSizeComputedFlag(parent);
		  if(EZ_WidgetMapped(parent)) EZ_DisplayWidget(parent);
		}
	    }
	  else 
	    {
	      EZ_Widget *parent = EZ_ItemParentWidget(item);
	      if(parent && EZ_LookupWidgetFromAllHT(parent) == parent && EZ_WidgetMapped(parent))
		{
		  EZ_Widget *widget = parent;
		  int wtype = EZ_WidgetType(widget);
		  int Rx,Ry,Rw,Rh;
		  int notdone = 1;
		  switch(wtype)
		    {
		    case EZ_WIDGET_ITREE:
		      {
			EZ_TreeNode   *tnode = EZ_ITreeWSelection(widget);
			EZ_Item *sitem = (tnode == NULL? NULL : EZ_TreeNodeItem(tnode));
			if(item == sitem) 
			  {
			    unsigned long bg = EZ_ITreeWHighlightBG(widget);
			    unsigned long fg = EZ_ITreeWHighlightFG(widget); 
			    int bw = EZ_WidgetBorderWidth(widget)+EZ_WidgetPadB(widget);
			    int ww = (int)(EZ_WidgetWidth(widget));
			    int hh = (int)(EZ_WidgetHeight(widget)); 
			    int hmode = EZ_ITreeWHighlightMode(widget);
			    Rx = Ry = bw;
			    Rw =  ww - (bw<<1);
			    Rh =  hh - (bw<<1);			    
			    EZ_HighlightOneItem(widget,EZ_WidgetWindow(widget),item,fg,bg,
						Rx, Ry, Rw, Rh, hmode);
			    notdone = 0;
			  }
		      }
		    break;
		    case EZ_WIDGET_IFANCY_LIST_BOX:
		      {
			int smode = EZ_IFListBoxSelectionType(widget);
			int sline = EZ_IFListBoxSelectionIDX(widget); 
			int special = 0;
			
			if(smode)  /* row mode */
			  {
			    if(sline > 0)
			      {
				int ncols = EZ_IFListBoxColumns(widget);
				EZ_Item **items = EZ_IFListBoxData(widget)[sline];
				int i;
				for(i = 0; i < ncols; i++)
				  if(items[i] == item)
				    {
				      special = 1; break;
				    }
			      }
			  }
			else
			  {
			    if(sline > 0)
			      {
				int col =  EZ_IFListBoxSelectionCol(widget);
				if(col >= 0)
				  {
				    EZ_Item **items = EZ_IFListBoxData(widget)[sline];
				    if(items[col] == item)    special = 1;
				  }
			      }
			  }
			if(special)
			  {
			    int *vsa = EZ_IFListBoxVSizes(widget);
			    int bw = EZ_WidgetBorderWidth(widget)+EZ_WidgetPadB(widget);
			    int yoffset = bw + EZ_WidgetPadY(widget);
			    int ww = EZ_WidgetWidth(widget);
			    int hh = EZ_WidgetHeight(widget);
			    EZ_Item **row =  EZ_IFListBoxData(widget)[sline];
			    unsigned long fg = EZ_IFListBoxHighlightFG(widget);
			    unsigned long bg = EZ_IFListBoxHighlightBG(widget);
			    int  hmode = EZ_IFListBoxHighlightMode(widget);			    

			    Rx = bw;
			    Ry = yoffset + vsa[0];
			    Rw = ww - (bw << 1);
			    Rh = hh - bw - Ry;

			    if(smode)
			      EZ_HighlightOneRow(widget, EZ_WidgetWindow(widget), row, 
						 EZ_IFListBoxColumns(widget),fg, bg,
						 Rx,Ry,Rw,Rh, hmode);
			    else
			      EZ_HighlightOneItem(widget, EZ_WidgetWindow(widget),
						  item, fg, bg, Rx,Ry,Rw,Rh, hmode);
			    notdone = 0;
			  }
		      }
		      break;
		    case EZ_WIDGET_IWORK_AREA:
		      {
			EZ_Item *sitem = EZ_IWorkAreaSelection(widget);	
			if(sitem == item)
			  {
			    unsigned long fg = EZ_IWorkAreaHighlightFG(widget);
			    unsigned long bg = EZ_IWorkAreaHighlightBG(widget);
			    int bw = EZ_WidgetBorderWidth(widget) + EZ_WidgetPadB(widget);
			    int ww = EZ_WidgetWidth(widget);
			    int hh = EZ_WidgetHeight(widget);
			    int hmode = EZ_IWorkAreaHighlightMode(widget);
			    Rx = bw;
			    Ry = bw;
			    Rw = ww - (bw << 1);
			    Rh = hh -(bw << 1);
			    
			    EZ_HighlightOneItem(widget, EZ_WidgetWindow(widget),
						item, fg, bg, Rx,Ry,Rw,Rh, hmode);
			    notdone = 0;
			  }
		      }
		    break;
		    default:
		      break;
		    }
		  if(notdone)
		    EZ_UpdateOneItem(parent, EZ_WidgetWindow(parent), item, 0, 0, 99999,99999);
		}
	    }
	}
    }
}
/**************************************************************************************/     
void EZ_DestroyItem(item)
     EZ_Item *item;
{
  if(EZ_LookupItemFromHT(item) != NULL)
    {
      EZ_RemoveItemFromHT(item);

      if(EZ_ItemBlocked(item) <= 0) EZ_DestroyItemFinal(item);
      else EZ_InsertObjToGarbageList(EZ_GARBAGE_IS_ITEM, (void *)item);
    }
}
void EZ_DestroyItemFinal(item)
     EZ_Item *item;
{
  /* destroy regular callbacks */
  int type = EZ_ItemType(item);
  EZ_CallBackStruct *next, *funcList =  EZ_ItemCallbacks(item);

  while(funcList)
    {
      next = funcList->next;
      (void)my_free(funcList);      
      funcList = next;
    }
  EZ_ItemCallbacks(item) = NULL;


  /* free DnD Data Handlers */
  if(EZ_ItemDnDDataEncoders(item))
    {
      EZ_DnDDataEncoder *tmp = EZ_ItemDnDDataEncoders(item);
      EZ_ItemDnDDataEncoders(item) = (EZ_DnDDataEncoder *)NULL;
      while(tmp)
	{
	  EZ_DnDDataEncoder *next = tmp->next;
	  (void)my_free((char *)tmp);
	  tmp = next;
	}
    }
  if(EZ_ItemDnDDataDecoders(item))
    {
      EZ_DnDDataDecoder *tmp = EZ_ItemDnDDataDecoders(item);
      EZ_ItemDnDDataDecoders(item) = (EZ_DnDDataDecoder *)NULL;
      while(tmp)
	{
	  EZ_DnDDataDecoder *next = tmp->next;
	  (void)my_free((char *)tmp);
	  tmp = next;
	}
    }
  /* clean up dirTreeNodeItem private data */
  {
    EZ_DirTreeNodeInfo *dinfo = EZ_ItemDirItemInfo(item);
    if(dinfo)
      {
	if(dinfo->id) (void) my_free((char *) dinfo->id);
	if(dinfo->pattern) (void) my_free((char *) dinfo->pattern);
	(void) my_free((char *)dinfo);
      }
  }
  (EZ_DpyItemHandlers[type]->freePrivateData)(item);
  /* call the user-defined destruction callbacks  */
  {
    EZ_CallBackStruct *next, *funcList =  EZ_ItemDestroyCallback(item);
	
    if(funcList)
      {
	EZ_ItemDestroyCallback(item) = NULL;
	while(funcList)
	  {
	    if(funcList->callback) (funcList->callback)(item, funcList->data);
	    next = funcList->next;
	    (void)my_free((char *)funcList);
	    funcList = next;	    
	  }
      }
  }
  /* free the spaces allocated for client data */
  if(EZ_ItemPValues(item) != NULL) (void) my_free((char *)EZ_ItemPValues(item));
  if(EZ_ItemResourceString(item))  (void) my_free((char *)EZ_ItemResourceString(item));
  /* finally, release item */
  (void) my_free((char *)item);
}
/**************************************************************************************/
void EZ_GetItemClientData(item,idata_ret,pdata_ret)
     EZ_Item *item;
     void **pdata_ret;
     int *idata_ret;
{
  if(item)
    {
      if(pdata_ret) *pdata_ret = EZ_ItemPtrData(item);
      if(idata_ret) *idata_ret = EZ_ItemIntData(item);
    }
}
int  EZ_GetItemIntData(item)
     EZ_Item *item;
{
  if(item)
    return(EZ_ItemIntData(item));
  return(0);
}

void  *EZ_GetItemPtrData(item)
     EZ_Item *item;
{
  if(item)
    return((void *)EZ_ItemPtrData(item));
  return(NULL);
}

void EZ_SetItemClientData(item,idata,pdata)
     EZ_Item *item;
     void *pdata;
     int idata;
{
  if(item)
    {
      EZ_ItemPtrData(item) = pdata;
      EZ_ItemIntData(item) = idata;
    }
}
void  EZ_SetItemIntData(item, i)
     EZ_Item *item; int i;
{
  if(item)
    EZ_ItemIntData(item) = i;
}

void  EZ_SetItemPtrData(item, p)
     EZ_Item *item; void *p;
{
  if(item)
    EZ_ItemPtrData(item) = p;
}

/**************************************************************************************/     

void EZ_ConfigureItem MY_VARARGS(void *,  the_args)
     MY_VARARGS_DECL
{
  va_list         ap;
  EZ_Item         *item;
  
  item = (EZ_Item *)( MY_VARARGS_START(void *, the_args, ap));
  if(EZ_LookupItemFromHT(item) == NULL) return;
#include "EZ_ItemAttr.h"

}

/**************************************************************************************/
void EZ_SetItemAttributes(item, option, i1, i2, p1, p2, ul1, ul2, i3, i4)
     EZ_Item *item;
     int option, i1, i2, i3, i4;
     unsigned long ul1, ul2;
     void *p1, *p2;
{
  if(item)
    {
      EZ_Bitmap       *bitmap = (EZ_Bitmap *)NULL;
      XFontStruct     *tfont = (XFontStruct *)NULL;
      int             type, dirty = 0, bitmapChanged = 0;
      
      type = EZ_ItemType(item);
      if(option != 0)
	{
	  switch(option)
	    {
            case EZ_LABEL_SHADOW:
              if(EZ_ItemType(item) == EZ_LABEL_ITEM)
                {
                  int sx, sy, sgnx=0, sgny=0;
                  unsigned char uc;
                  if(i1<0) { sgnx=8; i1 = -i1;}
                  if(i2<0) { sgny=8; i2 = -i2;}
                  sx = (i1 & 0xf)|sgnx; 
                  sy = (i2 & 0xf)|sgny;
                  uc = (unsigned char)((sx<<4) | (sy));
                  EZ_LabelItemShadow(item) = uc;
                }
              break;
            case EZ_NAME:
              {
                char *str = (char *)p1;
                if(str && *str) 
                  EZ_ItemIName(item) = EZ_StringToQuark(str);
              }
            break;
            case EZ_CLASS:
              {
                char *str = (char *)p1;
                if(str && *str) 
                  EZ_ItemCName(item) = EZ_StringToQuark(str);
              }
            break;
	    case EZ_ORIENTATION:
	      {
		int value = i1;
		if(type == EZ_LABEL_ITEM)
		  {
		    if(value != EZ_VERTICAL) value = EZ_HORIZONTAL;
		    if(EZ_LabelItemOrientation(item) != value)
		      {
			EZ_LabelItemOrientation(item) = value;
			dirty = 1;
		      }
		  }
		else if(type == EZ_FIG_ITEM)
                  {
		    if(value) { EZ_FigItemSy(item) = -1.0;}
                    else { EZ_FigItemSy(item) = 1.0;}
                    dirty = 1;
                  }
	      }
	      break; 
	    case EZ_JUSTIFICATION:
	      {
		int value = i1;
		if(type == EZ_LABEL_ITEM)
		  {
		    if(value >= EZ_CENTER && value <= EZ_RIGHT)  /* 0, 1, 2 :: CENTER, LEFT, RIGHT */
		      {
			if(EZ_LabelItemJustification(item) != value)
			  {
			    EZ_LabelItemJustification(item) = value;	  
			    dirty = 1;
			  }
		      }
		  }
	      }
	      break;
	    case EZ_LABEL_POSITION:  /* anchor position */
	      {
		int value = i1;
		if(value >= EZ_WIDGET_LABEL_POSITION_BEGIN &&
		   value <= EZ_WIDGET_LABEL_POSITION_END)
		  {
		    if(EZ_ItemAnchor(item) != value)
		      {
			EZ_ItemAnchor(item) = value;	  
			dirty = 1;
		      }
		  }
	      }
	      break; 
	    case  EZ_DND_DRAG_CURSOR:
	      {
		Cursor cursor = (Cursor)ul1;
		EZ_ItemDnDDragCursor(item) = cursor;
	      }
	    break;
	    case  EZ_DND_BUBBLE_HELP:
	      {
		char *ostr, *str = (char *)p1;
		int olen, len = str? strlen(str): 0;
		
		ostr = EZ_ItemDnDTargetHelp(item);
		if(len == 0) /* disable bubble help */
		  {
		    if(ostr) (void)my_free(ostr);
		    EZ_ItemDnDTargetHelp(item) = (char *)NULL;
		  }
		else
		  {
		    if(ostr)
		      {
			olen = strlen(ostr);
			if(olen < len)
			  ostr = EZ_ItemDnDTargetHelp(item) = (char *)my_realloc(ostr, len, _BUBBLE_STRING_);
			if(!ostr) EZ_OutOfMemory("EZ_ConfigureItem:DnDBubbleString");
			(void)strcpy(ostr, str);
		      }
		    else
		      EZ_ItemDnDTargetHelp(item) = EZ_AllocCopyString(str);
		  }
	      }
	    break;
	    case EZ_ATTACH_PTR_DATA:
	      {
		void *ptr =  p1;
		EZ_ItemPtrData(item) = ptr;
	      }
	      break;
	    case EZ_ATTACH_INT_DATA:
	      {
		int ii =  i1;
		EZ_ItemIntData(item) = ii;
	      }
	      break;
	    case EZ_FONT_NAME:
	      {
		char *fontname = (char *)p1;
		if(fontname && *fontname)
		  tfont = EZ_GetFontFromName(fontname);
	      }
	    break;
	    case EZ_FONT_ID:
	      {
		int font_id = i1;
		tfont = EZ_GetFontFromId(font_id);
	      }
	      break;
	    case EZ_FOREGROUND:
	      {
		char *color = (char *)p1;	
                unsigned long pv=0;
		if(color && *color) pv = EZ_AllocateColorFromName(color); 
                if(pv==0) pv= EZ_DefaultForeground;
                EZ_ItemForeground(item) = pv;
		break;
	      }
	      break;
	    case EZ_BACKGROUND:
	      {
		char *color = (char *)p1;	
		if(color && *color) 
		  EZ_ItemBackground(item) = EZ_AllocateColorFromName(color);
		else EZ_ItemBackground(item) = 0;
	      }
	    break;
	    case EZ_IPADX:   case EZ_IPADY:
	      {
		if(type == EZ_LABEL_ITEM)
		  {
		    int pad = i1;
		    int opad = EZ_LabelItemPad(item);
		    if(opad != pad)
		      {
			EZ_LabelItemPad(item) = pad;
			dirty = 1;
		      }
		  }
	      }
	      break;
	    case EZ_PADX: 
	      {
		int pad = i1;
		int opad = EZ_ItemPadX(item);
		if(opad != pad)
		  {
		    EZ_ItemPadX(item) = pad;
		    dirty = 1;
		  }
	      }
	      break;
	    case EZ_PADY: 
	      {
		int pad = i1;
		int opad = EZ_ItemPadY(item);
		if(opad != pad)
		  {
		    EZ_ItemPadY(item) = pad;
		    dirty = 1;
		  }
	      }
	      break;
	    case EZ_X:
	      {
		if(EZ_ItemAOriginX(item) != i1)
		  {
		    EZ_ItemAOriginX(item) = i1;
		    dirty = 1;
		  }
	      }
	    break;
	    case EZ_Y:
	      {
		if(EZ_ItemAOriginY(item) != i1)
		  {
		    EZ_ItemAOriginY(item) = i1;
		    dirty = 1;
		  }
	      }
	    break;
	    case EZ_LOCATION:
	      if(EZ_ItemAOriginX(item) != i1 || EZ_ItemAOriginY(item) != i2)
		{
		  EZ_ItemAOriginX(item) = i1;
		  EZ_ItemAOriginY(item) = i2;
		  dirty = 1;
		}
	      break;
	    case EZ_WIDTH:
	      {
		if(EZ_ItemAWidth(item) != i1)
		  {
		    EZ_ItemAWidth(item) = i1;
		    dirty = 1;
		  }
	      }
	    break;	    
	    case EZ_HEIGHT:
	      {
		if(EZ_ItemAHeight(item) != i1)
		  {
		    EZ_ItemAHeight(item) = i1;
		    dirty = 1;
		  }
	      }
	    break;
	    case EZ_SIZE:
	      if(EZ_ItemAWidth(item) != i1 || EZ_ItemAHeight(item) != i2)
		{
		  EZ_ItemAWidth(item) = i1;
		  EZ_ItemAHeight(item) = i2;
		  dirty = 1;
		}
	      break;
            case EZ_GEOMETRY:
              {
                EZ_ItemAOriginX(item) = i1;
                EZ_ItemAOriginY(item) = i2;
                EZ_ItemAWidth(item) = i3;
                EZ_ItemAHeight(item) = i4;
                dirty = 1;
              }
            break;
	    case EZ_BORDER_TYPE: 
	      {
		int type = i1;
		if(EZ_ItemBorderType(item) != type)
		  {
		    EZ_ItemBorderType(item) = type;
		    dirty = 1;
		  }
	      }
	      break;
	    case EZ_BORDER_WIDTH: 
	      {
		int width = i1;
		if(EZ_ItemBorderWidth(item) != width)
		  {
		    EZ_ItemBorderWidth(item) = width;
		    dirty = 1;
		  }
	      }
	      break;
	    case EZ_CALLBACK:
	      {
		EZ_CallBack callback = (EZ_CallBack)p1;
		void *data = p2;
		{
		  EZ_CallBackStruct **funcList =  &(EZ_ItemCallback(item));
		  EZ_AddCallBackWork(funcList, callback, data, 0, 0);
		}
	      }
	      break;
	    case EZ_DESTROY_CALLBACK:
	      {
		EZ_CallBack tmpfunc = (EZ_CallBack)p1;
		void *data = p2;
		EZ_CallBackStruct **funcList =  &(EZ_ItemDestroyCallback(item));
		EZ_AddCallBackWork(funcList, tmpfunc, data, 0, 0);
	      }
	    break;
	    case EZ_PIXMAP_FILE:
	      {
		char *file = (char *)p1;
		bitmap = (file == (char *)NULL? (EZ_Bitmap *)NULL:
				     EZ_GetBitmapFromFile(file, 1));
                bitmapChanged = 1;
	      }
	    break;
	    case EZ_IMAGE_FILE:
	      {
		char *file = (char *)p1;
		bitmap = (file == (char *)NULL? (EZ_Bitmap *)NULL:
				     EZ_GetImageFromFile(file, 1));
                bitmapChanged = 1;
	      }
	    break;
	    case EZ_PIXMAP_DATA:
	      {
		char **file = (char **)p1;
		bitmap = (file == (char **)NULL? (EZ_Bitmap *)NULL:
				     EZ_GetPixmapFromData(file));
                bitmapChanged = 1;
	      }
	    break;
	    case EZ_BITMAP_DATA:
	      {
		char *data = (char *)p1;
		int width = i1;
		int height = i2;
		if(width > 0 && height > 0)
		  bitmap = EZ_GetBitmapFromData(data, 
						(unsigned int)width,
						(unsigned int)height);
                bitmapChanged = 1;
	      }
	    break;
	    case EZ_RAW_RGB_DATA:
	      {
		char *RGBmap = (char *) p1;
		int width = (int) i1;
		int height = (int) i2;
	    
		bitmap = EZ_GetPixmapFromRawRGB(RGBmap, width, height);
                bitmapChanged = 1;
	      }
	    break;
	    case EZ_X_PIXMAP:
	      {
		Pixmap pixmap = (Pixmap)ul1;
		int    x = i1;
		int    y = i2;
		int width = i3;
		int height = i4;
		bitmap = EZ_GetPixmapFromPixmap(pixmap,x,y,width,height);
                bitmapChanged = 1;
	      }
	    break;
	    case EZ_LABEL_PIXMAP:
	      {
		char *tmp =  (char *)p1;
		bitmap = (EZ_Bitmap *)tmp;
		if(bitmap) bitmap->ref += 1;
                bitmapChanged = 1;
	      }
	    break;
	    case EZ_LABEL_STRING:
	      {
		char *str = (char *)p1;
		if(type == EZ_LABEL_ITEM)
		  {
		    EZ_LabelItemString(item) = (char *)
		      EZ_ReallocCopyString(EZ_LabelItemString(item),str);
		    EZ_LabelItemStringLength(item) = (str == NULL? 0 : strlen(str));
		    dirty = 1;
		  }
	      }
	      break;
	    case EZ_TEXT_LINE_LENGTH:
	      {
		int len  = i1;
		if(type == EZ_LABEL_ITEM && len > 0)
		  {
		    EZ_LabelItemStringLineLength(item) = len;
		    dirty = 1;
		  }
	      }
	    break;
	    case EZ_ITEM_WIDGET_WIDGET:
	      if(type == EZ_WIDGET_ITEM)
		{
		  EZ_Widget *widget = (EZ_Widget *)p1;
		  EZ_Widget *owidget = EZ_WidgetItemWidget(item);
		  if(owidget && owidget != widget) EZ_DestroyWidget(owidget);
		  EZ_WidgetItemWidget(item) = NULL;
		  if(widget)
		    {
		      if(EZ_WidgetParent(widget))  /* cannot do it */
			{
			  EZ_RemoveWidget(widget);  /* remove it from its parent's children list */
			}
		      EZ_WidgetItemWidget(item) = widget;
		    }
		}
	      break;
            case EZ_MARK_CLEAN:
              dirty = (i1 == 0);
              break;
            case EZ_RESOURCE_STRING:
              if(EZ_ParseItemResourceString(item, (char *)p1))
                {
                  dirty = 1;
                  EZ_RecordResourceString( &(EZ_ItemResourceString(item)), (char *)p1);
                }
              break;
	    case 0:
	      break;
	    default:
	      {
                extern int silenceOnConfigError;
                if(silenceOnConfigError == 0)
                  {
                    char *option_name;
                    if(option > 0 && option <= EZ_WIDGET_CONFIGURE_LAST_OPTION)
                      option_name = EZ_ConfigureOptionNames[option];
                    else  option_name = (char *)NULL;
                    fprintf(stderr, "EZ_ConfigureItem, %s is not a valid option for items\n", option_name);
                  }
              }
	      break;
	    }
	}

      if(tfont != (XFontStruct *)NULL || bitmapChanged)
	{
	  XFontStruct *ofont = EZ_ItemFont(item);
	  EZ_Bitmap   *obitmap = EZ_ItemPixmap(item);

	  if(tfont != (XFontStruct *)NULL && tfont != ofont)
	    {
	      EZ_ItemFont(item) = tfont;
	      if(ofont->ascent != tfont->ascent || ofont->descent !=
		 tfont->descent)
		dirty = 1;
	    }
	  if(bitmapChanged)
	    {
	      EZ_ItemPixmap(item) = bitmap;
	      if( (!obitmap && bitmap) || (obitmap && !bitmap) ||
                  (bitmap && obitmap &&
                   ((EZ_BitmapWidth(obitmap) != EZ_BitmapWidth(bitmap)) ||
                    (EZ_BitmapHeight(obitmap) != EZ_BitmapHeight(bitmap)))))
                dirty = 1;
              if(obitmap) EZ_FreeBitmap(obitmap);
	    }
	}
      EZ_ItemConfigureSentienl(item,dirty);
    }
}
/*****************************************************************************/
void  EZ_AddItemCallBack(item, which, callback, data, where)
     EZ_Item *item;
     EZ_CallBack callback;
     void *data;
     int where; /* 0: insert at the front, !=0: insert at the end */
     int which;
{
  if(item)
    { 
      EZ_CallBackStruct **funcList = EZ_GetItemCallbackType(item, which);
      EZ_AddCallBackWork(funcList,callback, data, where, 0);
    }
}
/*****************************************************************************/
void  EZ_AddItemCallBackInternal(item, which, callback, data, where)
     EZ_Item *item;
     EZ_CallBack callback;
     void *data;
     int where; /* 0: insert at the front, !=0: insert at the end */
     int which;
{
  if(item)
    { 
      EZ_CallBackStruct **funcList = EZ_GetItemCallbackType(item, which);
      EZ_AddCallBackWork(funcList,callback, data, where, 1);
    }
}

/*****************************************************************************/
void  EZ_RemoveItemCallBack(item,which,callback, data)
     EZ_Item *item;
     int which;
     EZ_CallBack callback;
     void *data;
{
  if(item)
    { 
      EZ_CallBackStruct **funcList = EZ_GetItemCallbackType(item, which);
      EZ_RemoveCallBackWork(funcList,callback, data);
    }
}
/*****************************************************************************/
void  EZ_RemoveAllItemCallBacks(item, which)
     EZ_Item *item;
     int which;
{
  if(item)
    { 
      EZ_CallBackStruct **funcList = EZ_GetItemCallbackType(item, which);
      EZ_RemoveAllCallBacksWork(funcList);
    }
}
/*****************************************************************************/
static  EZ_CallBackStruct  **EZ_GetItemCallbackType(item, which)
     EZ_Item *item;
     int which;
{
  EZ_CallBackStruct **funcList = NULL;
  switch(which)
    {
    case EZ_CALLBACK:
      funcList = &(EZ_ItemCallbacks(item)); 
      break;
    case EZ_DESTROY_CALLBACK:
      funcList = &(EZ_ItemDestroyCallback(item)); 
      break;
    default:
      break;
    }
  return(funcList);
}
/*****************************************************************************/
void    EZ_SetItemPosition(item, x, y)
     EZ_Item *item;
     int x,y;
{
  if(item)
    {
      EZ_ItemAOriginX(item)   = x;
      EZ_ItemAOriginY(item)   = y;      
    }
}

void EZ_GetItemPosition(item, x, y)
     EZ_Item *item;
     int *x, *y;
{
  if(item)
    {
      *x = EZ_ItemAOriginX(item);
      *y = EZ_ItemAOriginY(item);
    }
  else {*x = *y = 0; }
}

void EZ_GetItemAPosition(item, x, y)
     EZ_Item *item;
     int *x, *y;
{
  if(item)
    {
      *x = EZ_ItemOriginX(item);
      *y = EZ_ItemOriginY(item);
    }
  else {*x = *y = 0; }
}

/*************************************************************************/
void    EZ_SetItemWidth(item, w)
     EZ_Item *item;
     int w;
{
  if(item)
    {
      EZ_ItemAWidth(item)   = w;
    }
}
/*************************************************************************/     
void    EZ_SetItemHeight( item, h)
     EZ_Item *item;
     int h;
{
  if(item)
    {
      EZ_ItemAHeight(item)   = h;
    }
}
/*************************************************************************/     
void    EZ_SetItemDimension(item, w, h)
     EZ_Item *item;
     int w, h;
{
  if(item)
    {
      EZ_ItemAWidth(item)   = w;
      EZ_ItemAHeight(item)  = h;
    }
}
/*************************************************************************/     
int     EZ_GetItemWidth(item)
     EZ_Item *item;
{
  if(item) return(EZ_ItemAWidth(item));
  return(0);
}
/*************************************************************************/
int     EZ_GetItemHeight(item)
     EZ_Item *item;
{
  if(item) return(EZ_ItemAHeight(item));
  return(0);
}
/*************************************************************************/
void   EZ_GetItemDimension(item, w, h)
     EZ_Item *item;
     int *w, *h;
{
  if(item) 
    {
       if(w) *w = EZ_ItemAWidth(item);
       if(h) *h = EZ_ItemAHeight(item);
    }
}

void   EZ_GetItemADimension(item, w, h)
     EZ_Item *item;
     int *w, *h;
{
  if(item) 
    {
       if(w) *w = EZ_ItemWidth(item);
       if(h) *h = EZ_ItemHeight(item);
    }
}


EZ_Widget *EZ_GetItemHostWidget(item)
     EZ_Item *item;
{
  if(item) return(EZ_ItemParentWidget(item));
  return(NULL);
}

/*************************************************************************/
int EZ_GetItemUnknownData(item, idx, ret)
     EZ_Item *item; int idx; EZ_UnknownDataType *ret;
{
  if(idx >= 0 && EZ_LookupItemFromHT(item))  
    {
      int count = EZ_ItemNPValues(item);
      if(idx < count) { *ret = EZ_ItemPValues(item)[idx]; return(1);}
    }
  return( 0);
}
EZ_UnknownDataType *EZ_GetItemUnknownDataPtr(item)
     EZ_Item *item;
{
  if(EZ_LookupItemFromHT(item))  
     return(EZ_ItemPValues(item));
  return(NULL);
}

void EZ_SetItemUnknownData(item, idx, data)
     EZ_Item *item; EZ_UnknownDataType data; int idx;
{
  if(idx >= 0 && EZ_LookupItemFromHT(item))  
    {
      int count = EZ_ItemNPValues(item);
      if(idx < count) EZ_ItemPValues(item)[idx] = data;
      else
	{
	  EZ_UnknownDataType *ptr = EZ_ItemPValues(item);
	  count = idx + 1;
	  if(ptr) 
	    {
	      EZ_ItemPValues(item) =
		my_realloc(ptr, count * sizeof(EZ_UnknownDataType),_CLIENT_DATA_);
	    }
	  else
	    {
	      EZ_ItemPValues(item) =
		my_malloc(count * sizeof(EZ_UnknownDataType), _CLIENT_DATA_);
	    }
	  EZ_ItemNPValues(item) = count;
	  EZ_ItemPValues(item)[idx] = data;	  
	}
    }
}
/*************************************************************************/
EZ_Item *EZ_DuplicateItem(oitem) EZ_Item *oitem;
{
  EZ_Item *item = NULL;
  if(EZ_LookupItemFromHT(oitem))      
    {
      int type = EZ_ItemType(oitem);
      if(type == EZ_WIDGET_ITEM)
	{
	  (void)fprintf(stderr, "EZ_DuplicateItem: WIDGET_ITEM cannot be duplicated.\n");
	  return(NULL);
	}
      item = (EZ_Item *)my_malloc(sizeof(EZ_Item), _DISPLAY_ITEM_);
      (void) memcpy(item, oitem, sizeof(EZ_Item));
      EZ_ItemBlocked(item)   = 0;
      EZ_ItemParentWidget(item)    = (EZ_Widget *)NULL;
      EZ_ItemCallback(item)  = (EZ_CallBackStruct *)NULL;
      EZ_ItemDestroyCallback(item)  = (EZ_CallBackStruct *)NULL;
      EZ_ItemDirItemInfo(item) = NULL;
      EZ_ItemDirty(item)     = 1;
      EZ_ItemVisible(item)   = 1;
      EZ_ItemHighlighted(item) = 0;
      {
	EZ_Bitmap *bitmap = EZ_ItemBitmap(item);
	if(bitmap) EZ_BitmapRefCount(bitmap) += 1;
      }
      EZ_ItemDnDDataEncoders(item) = (EZ_DnDDataEncoder *)NULL;
      EZ_ItemDnDDataDecoders(item) = (EZ_DnDDataDecoder *)NULL;
      EZ_ItemDnDTargetHelp(item) = (char *)NULL;
      EZ_InsertItemToHashTable(item);
      /* application data */
      EZ_ItemIntData(item) = 0;
      EZ_ItemPtrData(item) = (void *)NULL;
      EZ_ItemNPValues(item) = 0;
      EZ_ItemPValues(item) =  NULL;
      if(type == EZ_LABEL_ITEM)
	EZ_LabelItemString(item) = EZ_AllocCopyString(EZ_LabelItemString(oitem));
      else if(type == EZ_FIG_ITEM)
	EZ_DuplicatePartList(item, &EZ_FigItemPartList(item),EZ_FigItemPartList(oitem));
    }
  return(item);
}
/*************************************************************************/
int EZ_GetItemType(item) EZ_Item *item;
{
  if(item) return(EZ_ItemType(item));
  return(0);
}
/*************************************************************************/
#undef _EZ_ITEM_C_
  
