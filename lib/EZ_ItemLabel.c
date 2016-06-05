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
 *  Label item.
 *
 *****************************************************************/
#define _EZ_ITEM_LABEL_C_
#include "EZ_Widget.h"

/*****************************************************************
 *
 * public functions
 */
EZ_Item    *EZ_CreateLabelItem MY_ANSIARGS((char *str, EZ_TextProperty *prop));
void        EZ_LabelItemSetProperty MY_ANSIARGS((EZ_Item *item, EZ_TextProperty *prop));
void        EZ_GetLabelItemStringInfo MY_ANSIARGS((EZ_Item *item, char **str, int *len));
int         EZ_GetLabelItemTextGeometry MY_ANSIARGS((EZ_Item *item, int *x, int *y, int *w, int *h));
EZ_Bitmap   *EZ_GetLabelItemPixmap MY_ANSIARGS((EZ_Item *item));
/*****************************************************************/

static void EZ_ComputeLabelItemSize MY_ANSIARGS((EZ_Item *item, int *w, int *h));
static void EZ_DrawLabelItem MY_ANSIARGS((EZ_Widget *widget,
					  Drawable  drawable,
					  int       x0, int y0,
					  EZ_Item   *item,
					  int       Rx, int Ry, int Rw, int Rh));
static void EZ_FreeLabelItemPrivateData MY_ANSIARGS((EZ_Item *item));
static void EZ_UpdateLabelItem MY_ANSIARGS((EZ_Widget *widget,
					    Drawable  drawable,
					    EZ_Item   *item,
					    int       Rx, int Ry, int Rw, int Rh));
static void EZ_HighlightLabelItem MY_ANSIARGS((EZ_Widget *widget,
					       Drawable  drawable,
					       EZ_Item   *item,
					       unsigned long fg, unsigned long bg,
					       int       Rx, int Ry, int Rw, int Rh, int hmode));
static int  EZ_PickALabelItem MY_ANSIARGS(( EZ_Item *item, int x, int y));
static void EZ_LabelItemPostscript MY_ANSIARGS(( EZ_Item *item, FILE *fp, void *psinfo));
/************************************************************************************/
static EZ_DpyItemHandle LabelItemHandle =
{
  EZ_ComputeLabelItemSize,
  EZ_DrawLabelItem,
  EZ_UpdateLabelItem,
  EZ_HighlightLabelItem,
  EZ_FreeLabelItemPrivateData,
  EZ_PickALabelItem,
  EZ_LabelItemPostscript,
};

/*****************************************************************
 *
 *  Functions dealing with LabelItem
 */
EZ_Item *EZ_CreateLabelItem(str, prop)
     char      *str;
     EZ_TextProp *prop;
{
  EZ_Item *item;
  
  item = (EZ_Item *)EZ_AllocateNewDisplayItem();
  if(item)
    {
      EZ_DpyItemHandlers[EZ_LABEL_ITEM] = &LabelItemHandle;
      EZ_SetItemTypeAndNames(item, EZ_LABEL_ITEM);

      EZ_LabelItemLabelPosition(item) = EZ_LEFT;      

      EZ_LabelItemPad(item)       = 2;
      EZ_LabelItemNLines(item)    = 1;
      EZ_LabelItemOrientation(item)  = EZ_HORIZONTAL;
      EZ_LabelItemJustification(item) = EZ_LEFT;

      EZ_LabelItemStringWidth(item) = 0;
      EZ_LabelItemStringHeight(item)= 1;

      EZ_LabelItemString(item)    = EZ_AllocCopyString(str);     
      EZ_LabelItemStringLength(item) = (str == NULL ? 0 : strlen(str));
      EZ_LabelItemStringLineLength(item) = EZ_DEFAULT_LABEL_LINE_LENGTH;

      if(prop)
	{
	  int itmp;
	  if(EZ_TextPropertyFGSet(prop))
	    EZ_ItemForeground(item) = EZ_TextPropertyForeground(prop);
	  if(EZ_TextPropertyBGSet(prop)) 
	    EZ_ItemBackground(item) = EZ_TextPropertyBackground(prop);
	  itmp = EZ_TextPropertyFont(prop);
	  if( itmp != -1) EZ_ItemFont(item) = EZ_GetFontFromId( itmp);

	  if(EZ_TextPropertyBitmap(prop))
	    EZ_ItemBitmap(item) = EZ_TextPropertyBitmap(prop); 
	}
    }
  return(item);
}
/********************************************************************************/
void EZ_LabelItemSetProperty(item, prop) 
     EZ_Item *item;
     EZ_TextProp *prop;
{
  if(item)
    {
      XFontStruct *ofont = EZ_ItemFont(item);
      EZ_Bitmap *obitmap = EZ_ItemBitmap(item); 
      if(prop)
	{
	  int itmp;
	  if(EZ_TextPropertyFGSet(prop))
	    EZ_ItemForeground(item) = EZ_TextPropertyForeground(prop);
	  if(EZ_TextPropertyBGSet(prop)) 
	    EZ_ItemBackground(item) = EZ_TextPropertyBackground(prop);
	  itmp = EZ_TextPropertyFont(prop);
	  if( itmp != -1) EZ_ItemFont(item) = EZ_GetFontFromId( itmp);
	  if(EZ_TextPropertyBitmap(prop))
	    EZ_ItemBitmap(item) = EZ_TextPropertyBitmap(prop); 
	}
      else
	{
	  EZ_ItemForeground(item) = EZ_DefaultForeground;
	  EZ_ItemBackground(item) = 0;
	  EZ_ItemFont(item) = EZ_GetFontFromId(EZ_LABEL_FONT);
	  EZ_ItemBitmap(item) = (EZ_Bitmap *)NULL;
	}
      if(EZ_ItemDirty(item) == 0)
	{
	  XFontStruct *font = EZ_ItemFont(item);
	  EZ_Bitmap *bitmap = EZ_ItemBitmap(item); 
	  int dirty = 0;

	  if(ofont != font)
	    {
	      if(ofont->ascent != font->ascent ||
		 ofont->descent != font->descent) dirty = 1;
	    }
	  if(bitmap != obitmap)
	    {
	      if(bitmap == (EZ_Bitmap *)NULL || obitmap == (EZ_Bitmap *)NULL)
		dirty = 1;
	      else
		if(bitmap->width != obitmap->width || bitmap->height != obitmap->height)
		  dirty = 1;
	    }
	  EZ_ItemConfigureSentienl(item,dirty);
	}
    }
}
/*****************************************************************************/
static void EZ_ComputeLabelItemSize(item, w_ret, h_ret)
     EZ_Item *item;
     int *w_ret, *h_ret;
{
  if(item)
    {
      if(EZ_ItemDirty(item))
	{
	  int pw, ph, tw, th, orientation, itmpX, itmpY;
	  int nlines;
	  EZ_Bitmap *bitmap = EZ_ItemPixmap(item);
	  XFontStruct *fontstruct = EZ_ItemFont(item);
	  orientation = EZ_LabelItemOrientation(item);
      
	  EZ_FigureLabelExtent(fontstruct,
			       EZ_LabelItemString(item),
			       (int)EZ_LabelItemStringLength(item),
			       (int)EZ_LabelItemStringLineLength(item),
			       &nlines, &tw);
	  th = nlines * (fontstruct->ascent + fontstruct->descent);
	  EZ_LabelItemStringHeight(item) = (fontstruct->ascent + fontstruct->descent);
	  EZ_LabelItemNLines(item)  = nlines;
	  EZ_LabelItemStringWidth(item) = tw;
	  
	  if(bitmap != (EZ_Bitmap *)NULL)
	    {
	      /* there is a pixmap */
	      pw = EZ_BitmapWidth(bitmap);
	      ph = EZ_BitmapHeight(bitmap);
	      
	      if(orientation == EZ_HORIZONTAL)
		{
		  tw = pw + tw + (int) EZ_LabelItemPad(item);
		  th = (ph > th ? ph : th);
		}
	      else
		{
		  tw = (tw > pw ? tw: pw);
		  th = ph + th + (int) EZ_LabelItemPad(item);
		}
	    }
	  /* add two pixels for drawing selection border */
	  itmpX = (int)EZ_ItemBorderWidth(item);
	  itmpY = itmpX + (int) EZ_ItemPadY(item) + 1;
	  itmpX = itmpX + (int) EZ_ItemPadX(item) + 1;
	  tw += (itmpX << 1);
	  th += (itmpY << 1);

	  EZ_ItemWidth(item) = tw; 
	  EZ_ItemHeight(item) = th;
	  EZ_ItemAWidth(item) = tw; 
	  EZ_ItemAHeight(item) = th;
	  EZ_ItemDirty(item) = 0;
	}
      *w_ret = (int)EZ_ItemWidth(item);
      *h_ret = (int)EZ_ItemHeight(item);
    }
  else
    {
      *w_ret = 0;
      *h_ret = 0;
    }
}
/************************************************************************/

static void EZ_DrawLabelItem(widget, drawable, x0, y0, item, Rx, Ry, Rw,Rh)    
     EZ_Widget *widget;
     Drawable  drawable;
     EZ_Item   *item;
     int       x0, y0;
     int       Rx, Ry, Rw,Rh; /* confined region in drawable */
{
  if(item)
    {
      int x,y, xoff,yoff, bdX, bdY, bdwidth;
      int w = EZ_ItemWidth(item);
      int h = EZ_ItemHeight(item);
      int aw = EZ_ItemAWidth(item);
      int ah = EZ_ItemAHeight(item);      
      int orientation, utile = 0;

      EZ_Bitmap *bitmap;
      XFontStruct *xfontstruct;
      int pw, ph;
      GC  gc;
      unsigned long bgpv;

      /* remember the current origin, and which widget it belongs to */
      EZ_ItemOriginX(item) = x0;
      EZ_ItemOriginY(item) = y0;
      EZ_ItemParentWidget(item) = widget;

      /* do a simple cliping.  Note: we still need to traverse
       * the whole tree to set the position of all nodes. The
       * geometry info is needed all over the place.
       */
      if(x0 > Rw + Rx || x0 + aw < Rx || y0 + ah < Ry || y0 > Rh +Ry)
	{
	  EZ_ItemVisible(item) = 0;	  
	  return;
	}
      EZ_ItemVisible(item) = 1;	  
      
      EZ_GetItemXYOffset(item, &xoff, &yoff);
      EZ_GetItemBwAndXYPading(item, &bdwidth,&bdX, &bdY);

      orientation = EZ_LabelItemOrientation(item);
      bitmap  = EZ_ItemPixmap(item);
      xfontstruct = EZ_ItemFont(item);
      /* fill the background */
      gc = EZ_WRITABLEGC;      
      if(EZ_ItemBackground(item) != 0)
	{
	  bgpv = EZ_ItemBackground(item);      
	  XSetForeground(EZ_Display, gc, bgpv);
	  XFillRectangle(EZ_Display, drawable, gc, x0, y0, aw,ah);
	}
      else utile = EZ_FillItemBackground(widget,item, drawable, x0, y0, aw, ah, x0, y0, &bgpv);
      /* render the label */
      if(EZ_ItemForeground(item) != EZ_DefaultForeground ||
	 EZ_ItemFont(item) != EZ_GetFontFromId(EZ_LABEL_FONT))
	{
	  XSetFont(EZ_Display, gc, EZ_ItemFont(item)->fid);
	  XSetForeground(EZ_Display, gc, EZ_ItemForeground(item));
	}
      else 
	{
	  XSetFont(EZ_Display, gc, EZ_GetFontFromId(EZ_LABEL_FONT)->fid);
	  XSetForeground(EZ_Display, gc, EZ_DefaultForeground);
	}
      if(orientation == EZ_HORIZONTAL) 
	{
	  xoff += bdX;
	  if(bitmap)
	    {
	      pw = EZ_BitmapWidth(bitmap);
	      ph = EZ_BitmapHeight(bitmap);
	      x = x0 + xoff + 1; /* selection border */
	      y = y0 + yoff +(h-ph)/2;

	      if(bitmap->shape != (Pixmap)NULL)
		{
		  XSetClipMask(EZ_Display, gc, bitmap->shape);
		  XSetClipOrigin(EZ_Display, gc,x,y);
		}

	      XSetBackground(EZ_Display, gc, bgpv);
	      EZ_RenderPixmapLabelWithGC(bitmap, drawable, gc, x,y);
	      if(bitmap->shape != (Pixmap)NULL) XSetClipMask(EZ_Display, gc, None);
	      pw += EZ_LabelItemPad(item);
	    }
	  else {pw = 1; ph = 0; }
	  if(EZ_LabelItemStringLength(item)> 0)
	    {
              int shadow = EZ_LabelItemShadow(item);
	      x = xoff + x0 + pw;
	      y = yoff + y0 + (h - (int)EZ_LabelItemStringHeight(item) * EZ_LabelItemNLines(item))/2
		+ xfontstruct->ascent;

	      EZ_RenderLabelText(drawable,gc,x,y,
				 (int)EZ_LabelItemStringHeight(item),			     
				 EZ_LabelItemString(item),
				 (int)EZ_LabelItemStringLength(item),
				 (int)EZ_LabelItemStringLineLength(item),
				 (int)EZ_LabelItemNLines(item),
				 (int)(EZ_ItemWidth(item) - pw),
				 (int) EZ_LabelItemJustification(item),
				 xfontstruct, shadow);
	    }
	}
      else  /* vertical */
	{
	  yoff += bdY;
	  if(bitmap)
	    {
	      pw = EZ_BitmapWidth(bitmap);
	      ph = EZ_BitmapHeight(bitmap);
	      x = xoff + x0 + (w-pw)/2;
	      y = yoff + y0 + 1; /* selection border */
	      if(bitmap->shape != (Pixmap)NULL)
		{
		  XSetClipMask(EZ_Display, gc, bitmap->shape);
		  XSetClipOrigin(EZ_Display, gc,x,y);
		}
	      XSetBackground(EZ_Display, gc, bgpv);
	      EZ_RenderPixmapLabelWithGC(bitmap, drawable, gc, x,y);
	      if(bitmap->shape != (Pixmap)NULL) XSetClipMask(EZ_Display, gc, None);
	      ph += EZ_LabelItemPad(item);
	    }
	  else {pw = 0; ph = 1;/* selection border */ }
	  if(EZ_LabelItemStringLength(item)> 0)
	    {
              int shadow = EZ_LabelItemShadow(item);
	      y = yoff + y0 + ph + xfontstruct->ascent;
	      x = xoff + x0 + (w - (int)EZ_LabelItemStringWidth(item))/2;
	      
	      EZ_RenderLabelText(drawable,gc,x,y,
				 (int)EZ_LabelItemStringHeight(item),			     
				 EZ_LabelItemString(item),
				 (int)EZ_LabelItemStringLength(item),
				 (int)EZ_LabelItemStringLineLength(item),
				 (int)EZ_LabelItemNLines(item),
				 (int)EZ_LabelItemStringWidth(item),
				 (int) EZ_LabelItemJustification(item),
				 xfontstruct,shadow);
	    }
	}
      /* how about border */
      if(bdwidth > 0)
        EZ_DrawRectBorderWithSize(widget, drawable, x0,y0,aw,ah,
                                  bdwidth, (int)EZ_ItemBorderType(item),0,0, 2);
    }
}
/*****************************************************************************/

static void EZ_UpdateLabelItem(widget, drawable, item, cx,cy,cw,ch)    
     EZ_Widget *widget;
     Drawable  drawable;
     EZ_Item   *item;
     int cx,cy,cw,ch;     /* confined region in drawable */
{
  if(item && EZ_ItemVisible(item))
    {
      int xoff, yoff, bdX, bdY, bdwidth;
      int x0 = EZ_ItemOriginX(item);
      int y0 = EZ_ItemOriginY(item);
      int w = EZ_ItemWidth(item);
      int h = EZ_ItemHeight(item);
      int aw = EZ_ItemAWidth(item);
      int ah = EZ_ItemAHeight(item);      
      int utile = 0;
      EZ_Bitmap *bitmap = EZ_ItemPixmap(item);
      XFontStruct *xfontstruct = EZ_ItemFont(item);
      int orientation = EZ_LabelItemOrientation(item);
      int pw, ph;
      GC  gc;
      unsigned long bgpv;
      Pixmap pixmap;

      pixmap = XCreatePixmap(EZ_Display, drawable, aw, ah, EZ_Depth);

      /* fill the background */
      gc = EZ_WRITABLEGC;
      if(EZ_ItemBackground(item) != 0)
	{
	  bgpv = EZ_ItemBackground(item);      
	  XSetForeground(EZ_Display, gc, bgpv);
	  XFillRectangle(EZ_Display, pixmap, gc, 0, 0, aw,ah);
	}
      else utile = EZ_FillItemBackground(widget, item, pixmap, x0, y0, aw, ah, 0, 0, &bgpv);
      
      
      /* render the label */
      if(EZ_ItemForeground(item) != EZ_DefaultForeground ||
	 EZ_ItemFont(item) != EZ_GetFontFromId(EZ_LABEL_FONT))
	{
	  XSetFont(EZ_Display, gc, EZ_ItemFont(item)->fid);
	  XSetForeground(EZ_Display, gc, EZ_ItemForeground(item));
	}
      else 
	{
	  XSetFont(EZ_Display, gc, EZ_GetFontFromId(EZ_LABEL_FONT)->fid);
	  XSetForeground(EZ_Display, gc, EZ_DefaultForeground);
	}
      
      EZ_GetItemXYOffset(item, &xoff, &yoff);
      EZ_GetItemBwAndXYPading(item, &bdwidth,&bdX, &bdY);
      
      if(orientation == EZ_HORIZONTAL) 
	{      
	  xoff += bdX;
	  if(bitmap)
	    {
	      int x,y;
	      pw = EZ_BitmapWidth(bitmap);
	      ph = EZ_BitmapHeight(bitmap);
	      x = xoff + 1; /* selection border */
	      y = yoff + (h-ph)/2;
	      if(bitmap->shape != (Pixmap)NULL)
		{
		  XSetClipMask(EZ_Display, gc, bitmap->shape);
		  XSetClipOrigin(EZ_Display, gc,x,y);
		}
	      XSetBackground(EZ_Display, gc, bgpv);
	      EZ_RenderPixmapLabelWithGC(bitmap, pixmap, gc, x,y);
	      if(bitmap->shape != (Pixmap)NULL) XSetClipMask(EZ_Display, gc, None);
	      pw += EZ_LabelItemPad(item);
	    }
	  else {pw = 1; /* selection border */ ph = 0; }
	  if(EZ_LabelItemStringLength(item)> 0)
	    {
	      int x,y, shadow = EZ_LabelItemShadow(item);
	      y = yoff + (h - (int)EZ_LabelItemStringHeight(item) * EZ_LabelItemNLines(item))/2
		+ xfontstruct->ascent;
	      x =  xoff + pw;
	      EZ_RenderLabelText(pixmap,gc,x,y,
				 (int)EZ_LabelItemStringHeight(item),			     
				 EZ_LabelItemString(item),
				 (int)EZ_LabelItemStringLength(item),
				 (int)EZ_LabelItemStringLineLength(item),
				 (int)EZ_LabelItemNLines(item),
				 (int)EZ_ItemWidth(item) - pw,
				 (int) EZ_LabelItemJustification(item),
				 xfontstruct, shadow);
	    } 
	}
      else /* vertical */
	{
	  yoff += bdY;
	  if(bitmap)
	    {
	      int x,y;
	      pw = EZ_BitmapWidth(bitmap);
	      ph = EZ_BitmapHeight(bitmap);
	      x = xoff + (w-pw)/2; 
	      y = yoff + 1; /* selection border */
	      if(bitmap->shape != (Pixmap)NULL)
		{
		  XSetClipMask(EZ_Display, gc, bitmap->shape);
		  XSetClipOrigin(EZ_Display, gc,x,y);
		}
	      XSetBackground(EZ_Display, gc, bgpv);
	      EZ_RenderPixmapLabelWithGC(bitmap, pixmap, gc, x,y);
	      if(bitmap->shape != (Pixmap)NULL) XSetClipMask(EZ_Display, gc, None);
	      ph += EZ_LabelItemPad(item);
	    }
	  else {pw = 0; ph = 1;/* selection border */ }
	  if(EZ_LabelItemStringLength(item)> 0)
	    {
	      int x,y, shadow = EZ_LabelItemShadow(item);;
	      y =  yoff + ph + xfontstruct->ascent;
	      x =  xoff + (w - EZ_LabelItemStringWidth(item))/2;

	      EZ_RenderLabelText(pixmap,gc,x,y,
				 (int)EZ_LabelItemStringHeight(item),			     
				 EZ_LabelItemString(item),
				 (int)EZ_LabelItemStringLength(item),
				 (int)EZ_LabelItemStringLineLength(item),
				 (int)EZ_LabelItemNLines(item),
				 (int)EZ_LabelItemStringWidth(item),
				 (int) EZ_LabelItemJustification(item),
				 xfontstruct, shadow);
	    } 
	}
      /* how about border */
      if(bdwidth > 0)
        EZ_DrawRectBorderWithSize(widget, pixmap, 0,0,aw,ah,
                                  bdwidth, (int)EZ_ItemBorderType(item),-x0,-y0, 6);
      /* finally copy pixmap to drawable */
      {
	int x_r, y_r, w_r, h_r;

	EZ_IntersectRectangles(cx,cy,cw,ch,    /* the drawing area */
			       x0,y0,aw,ah,
			       &x_r,&y_r, &w_r, &h_r);
	XCopyArea(EZ_Display, pixmap, drawable, EZ_WRITABLEGC,x_r-x0,y_r-y0,w_r,h_r, x_r,y_r);
	XFreePixmap(EZ_Display,pixmap);
      }
    }
}
/********************************************************************************************/
int  EZ_GetLabelItemTextGeometry(item, x_r, y_r,w_r,h_r)
     EZ_Item *item; int *x_r, *y_r, *w_r, *h_r;
{
  if(item && EZ_ItemType(item) == EZ_LABEL_ITEM && EZ_ItemVisible(item)
     && EZ_LabelItemStringLength(item)> 0)
    {
      int x, y, xoff,yoff, bdX, bdY, bdwidth, pw, ph;
      int w = EZ_ItemWidth(item);
      int h = EZ_ItemHeight(item);
      int x0 = EZ_ItemOriginX(item);
      int y0 = EZ_ItemOriginY(item);
      int height = (int)EZ_LabelItemStringHeight(item);
      int theight = height * EZ_LabelItemNLines(item);

      EZ_Bitmap *bitmap = EZ_ItemPixmap(item);
      /*XFontStruct *xfontstruct = EZ_ItemFont(item);*/
      int orientation;
      EZ_GetItemXYOffset(item, &xoff, &yoff);
      EZ_GetItemBwAndXYPading(item, &bdwidth,&bdX, &bdY);
      orientation = EZ_LabelItemOrientation(item);
      if(orientation == EZ_HORIZONTAL) 
	{
	  xoff += bdX;
	  if(bitmap)
	    {
	      pw = EZ_BitmapWidth(bitmap);
	      ph = EZ_BitmapHeight(bitmap);
	      x = x0 + xoff + 1; /* selection border */
	      y = y0 + yoff +(h-ph)/2;
	      pw += EZ_LabelItemPad(item);
	    }
	  else {pw = 1; ph = 0; }
	  x = xoff + x0 + pw;
	  y = yoff + y0 + (h - theight)/2 /* + xfontstruct->ascent*/;

	  if(x_r) *x_r = x;
	  if(y_r) *y_r = y /*- height*/;
	  if(w_r) *w_r = (int)EZ_LabelItemStringWidth(item);
	  if(h_r) *h_r = theight;
	}
      else  /* vertical */
	{
	  yoff += bdY;
	  if(bitmap)
	    {
	      pw = EZ_BitmapWidth(bitmap);
	      ph = EZ_BitmapHeight(bitmap);
	      ph += EZ_LabelItemPad(item);
	    }
	  else {pw = 0; ph = 1;/* selection border */ }
	  y = yoff + y0 + ph /*+ xfontstruct->ascent*/;
	  x = xoff + x0 + (w - (int)EZ_LabelItemStringWidth(item))/2;
	  if(x_r) *x_r = x;
	  if(y_r) *y_r = y /*- height*/;
	  if(w_r) *w_r = (int)EZ_LabelItemStringWidth(item);
	  if(h_r) *h_r = theight;
	}
      /*
      {
	GC gc = EZ_DARKREDGC;
	EZ_Widget *parent = EZ_ItemParentWidget(item);
	Window win = EZ_WidgetWindow(parent);
	XFillRectangle(EZ_Display, win, gc, *x_r, *y_r, *w_r, *h_r);
      }
      */
      return(1);
    } 
  return(0);
}
/********************************************************************************************/

static void EZ_HighlightLabelItem(widget, drawable, item, fg, bg, cx,cy,cw,ch, hmode)    
     EZ_Widget *widget;
     Drawable  drawable;
     EZ_Item   *item;
     unsigned long fg,bg; /* ~0 ~0 for default setting */
     int cx,cy,cw,ch;     /* confined region in drawable */
     int hmode;           /* method to highlight item */
{
  if(item)
    {
      int xoff, yoff, bdwidth, bdX, bdY;
      int x0 = EZ_ItemOriginX(item);
      int y0 = EZ_ItemOriginY(item);      
      int w = EZ_ItemWidth(item);
      int h = EZ_ItemHeight(item);
      int aw = EZ_ItemAWidth(item);
      int ah = EZ_ItemAHeight(item);      
      EZ_Bitmap *bitmap = EZ_ItemPixmap(item);
      XFontStruct *xfontstruct = EZ_ItemFont(item);
      int orientation = EZ_LabelItemOrientation(item);
      int pw, ph, utile = 0;
      GC  gc, gcb;
      Pixmap pixmap;
      
      pixmap = XCreatePixmap(EZ_Display, drawable, aw, ah, EZ_Depth);

      if(bg == ~0) EZ_GetBrightBDpv(widget, &bg);
      if(fg == ~0)
	{
	  if(EZ_ItemForeground(item)) fg = EZ_ItemForeground(item);
	  else fg = EZ_DefaultForeground;
	}
      gcb = EZ_WRITABLEGC2;
      XSetForeground(EZ_Display, gcb, bg);
      utile = EZ_FillItemBackground(widget, item, pixmap, x0, y0, aw, ah, 0, 0, NULL);
      if(hmode < 2) XFillRectangle(EZ_Display, pixmap, gcb, 0, 0, aw,ah);

      /* render the label */
      gc = EZ_WRITABLEGC;
      XSetForeground(EZ_Display, gc,fg);
      XSetFont(EZ_Display, gc, xfontstruct->fid);

      EZ_GetItemXYOffset(item, &xoff, &yoff);
      EZ_GetItemBwAndXYPading(item, &bdwidth,&bdX, &bdY);

      if(orientation == EZ_HORIZONTAL) 
	{
	  xoff += bdX;

	  if(bitmap)
	    {
	      int x,y;
	      pw = EZ_BitmapWidth(bitmap);
	      ph = EZ_BitmapHeight(bitmap);
	      XSetBackground(EZ_Display, gc, bg);
	      x = xoff + 1; /* selection border */
	      y = yoff + (h-ph)/2;
	      if(bitmap->shape != (Pixmap)NULL)
		{
		  if(hmode >= 2)  XFillRectangle(EZ_Display, pixmap, gcb, x, y, pw, ph);
		  XSetClipMask(EZ_Display, gc, bitmap->shape);
		  XSetClipOrigin(EZ_Display, gc,x,y);
		}
	      EZ_RenderPixmapLabelWithGC(bitmap, pixmap, gc, x,y);
	      if(bitmap->shape != (Pixmap)NULL) XSetClipMask(EZ_Display, gc, None);
	      pw += EZ_LabelItemPad(item);
	    }
	  else {pw = 1; /* selection border */ ph = 0; }
	  if(EZ_LabelItemStringLength(item)> 0)
	    {
	      int x,y, tmpw, tmph,shadow = EZ_LabelItemShadow(item);
	      y = yoff+(h - (int)EZ_LabelItemStringHeight(item) * EZ_LabelItemNLines(item))/2;
	      x = xoff + pw;

	      tmpw = (int)EZ_LabelItemStringWidth(item);
	      tmph = (int)EZ_LabelItemStringHeight(item) *
		(int)EZ_LabelItemNLines(item);
	      if(hmode >= 2) XFillRectangle(EZ_Display, pixmap, gcb, x, y, tmpw, tmph);

	      y += xfontstruct->ascent;
	      EZ_RenderLabelText(pixmap,gc,x,y,
				 (int)EZ_LabelItemStringHeight(item),			     
				 EZ_LabelItemString(item),
				 (int)EZ_LabelItemStringLength(item),
				 (int)EZ_LabelItemStringLineLength(item),
				 (int)EZ_LabelItemNLines(item),
				 (int)EZ_LabelItemStringWidth(item),
				 (int) EZ_LabelItemJustification(item),
				 xfontstruct,shadow);
	    }
	}
      else /* vertical */
	{
	  yoff += bdY;

	  if(bitmap)
	    {
	      int x,y;
	      pw = EZ_BitmapWidth(bitmap);
	      ph = EZ_BitmapHeight(bitmap);
	      XSetBackground(EZ_Display, gc, bg);
	      x = xoff + (w-pw)/2;
	      y = yoff + 1; /* selection border */
	      if(bitmap->shape != (Pixmap)NULL)
		{
		  if(hmode >= 2)  XFillRectangle(EZ_Display, pixmap, gcb, x, y, pw, ph);
		  XSetClipMask(EZ_Display, gc, bitmap->shape);
		  XSetClipOrigin(EZ_Display, gc,x,y);
		}
	      EZ_RenderPixmapLabelWithGC(bitmap, pixmap, gc, x,y);
	      if(bitmap->shape != (Pixmap)NULL) XSetClipMask(EZ_Display, gc, None);
	      ph += EZ_LabelItemPad(item);
	    }
	  else {pw = 0; ph = 1; }
	  if(EZ_LabelItemStringLength(item)> 0)
	    {
	      int x,y,tmpw,tmph, shadow = EZ_LabelItemShadow(item);
	      y = yoff + ph;
	      x = xoff + (w - EZ_LabelItemStringWidth(item))/2;
	      tmpw = (int)EZ_LabelItemStringWidth(item);
	      tmph = (int)EZ_LabelItemStringHeight(item) *
		(int)EZ_LabelItemNLines(item);
	      if(hmode >= 2)	XFillRectangle(EZ_Display, pixmap, gcb, x, y, tmpw, tmph);
	      
	      y += xfontstruct->ascent;
	      EZ_RenderLabelText(pixmap,gc,x,y,
				 (int)EZ_LabelItemStringHeight(item),			     
				 EZ_LabelItemString(item),
				 (int)EZ_LabelItemStringLength(item),
				 (int)EZ_LabelItemStringLineLength(item),
				 (int)EZ_LabelItemNLines(item),
				 (int)EZ_LabelItemStringWidth(item),
				 (int) EZ_LabelItemJustification(item),
				 xfontstruct,shadow);
	    }
	}
      /* how about border */
      if(bdwidth > 0)
        EZ_DrawRectBorderWithSize(widget, pixmap, 0,0,aw,ah,
                                  bdwidth, (int)EZ_ItemBorderType(item),-x0,-y0,6);
      
      gc = EZ_WRITABLEGC;
      /* finally copy pixmap to drawable */
      {
	int x0 = EZ_ItemOriginX(item);
	int y0 = EZ_ItemOriginY(item);
	int x_r, y_r, w_r, h_r;

	EZ_IntersectRectangles(cx,cy,cw,ch,    /* the drawing area */
			       x0,y0,aw,ah,
			       &x_r,&y_r, &w_r, &h_r);
	XCopyArea(EZ_Display, pixmap, drawable, gc,x_r-x0,y_r-y0,w_r,h_r, x_r,y_r);
	XFreePixmap(EZ_Display,pixmap);
      }
    }
}
/**********************************************************************************************/
static int  EZ_PickALabelItem(item,x,y)
     EZ_Item *item;
     int x,y;
{
  int x0,y0,w0,h0;

  x0 = EZ_ItemOriginX(item);
  y0 = EZ_ItemOriginY(item);
  w0 = EZ_ItemAWidth(item);
  h0 = EZ_ItemAHeight(item);

  if(x >= x0 && x <= x0 + w0 && y >= y0 && y <= y0 + h0) return(1);

  return(0);
}
/**********************************************************************************************/
static void EZ_FreeLabelItemPrivateData(item)
     EZ_Item *item;
{
  if(item)
    {
      if(EZ_LabelItemString(item))
	(void)my_free((char *) EZ_LabelItemString(item));
    }
}

/**********************************************************************************************/
void EZ_GetLabelItemStringInfo(item, str_ret, length_ret)
     EZ_Item *item;
     char **str_ret;
     int *length_ret;
{
  char *str = NULL;
  int len = 0;
  if(item && item == EZ_LookupItemFromHT(item))  
    {
      switch(EZ_ItemType(item))
	{
	case EZ_LABEL_ITEM:
	  str = EZ_LabelItemString(item);
	  len = EZ_LabelItemStringLength(item);
	  break;
	default:
	  break;
	}
    }
  if(str_ret)  *str_ret = str;
  if(length_ret) *length_ret = len;
}

EZ_Bitmap   *EZ_GetLabelItemPixmap (item) EZ_Item *item;
{
  if(item && item == EZ_LookupItemFromHT(item))  
    return(EZ_ItemPixmap(item));
  return(NULL);
}
/**************************************************************************************/     
static void EZ_LabelItemPostscript(item, fp, psinfo)
     EZ_Item *item; FILE *fp; void *psinfo;
{
  
}
/**************************************************************************************/     
#undef _EZ_ITEM_LABEL_C_
