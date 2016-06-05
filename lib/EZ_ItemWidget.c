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
 *  Widget item.
 *
 *****************************************************************/
#define _EZ_ITEM_WIDGET_C_
#include "EZ_Widget.h"

/*****************************************************************
 *
 * public functions
 */
EZ_Item    *EZ_CreateWidgetItem MY_ANSIARGS((EZ_Widget *widget));
EZ_Widget  *EZ_GetWidgetItemWidget MY_ANSIARGS((EZ_Item *item));
void        EZ_SetWidgetItemWidget MY_ANSIARGS((EZ_Item *item, EZ_Widget *widget));
void        EZ_PostscriptWidget MY_ANSIARGS((EZ_Widget *widget, FILE *fp));
void        EZ_SaveWidget2Postscript MY_ANSIARGS((EZ_Widget *widget, int landscape, int dpi, 
						  float scale, FILE *fp));
void        EZ_SaveWidget2PPM  MY_ANSIARGS((EZ_Widget *widget, FILE *fp));
/*****************************************************************/

static void EZ_ComputeWidgetItemSize MY_ANSIARGS((EZ_Item *item, int *w, int *h));
static void EZ_DrawWidgetItem MY_ANSIARGS((
					  EZ_Widget *widget,
					  Drawable  drawable,
					  int       x0, int y0,
					  EZ_Item   *item,
					  int       Rx, int Ry, int Rw, int Rh));
static void EZ_UpdateWidgetItem  MY_ANSIARGS((
					    EZ_Widget *widget,
					    Drawable  drawable,
					    EZ_Item   *item,
					    int       Rx, int Ry, int Rw, int Rh));
static void EZ_HighlightWidgetItem MY_ANSIARGS((
					       EZ_Widget *widget,
					       Drawable  drawable,
					       EZ_Item   *item,
					       unsigned long fg, unsigned long bg,
					       int       Rx, int Ry, int Rw, int Rh, int flag));
static void EZ_FreeWidgetItemPrivateData MY_ANSIARGS((EZ_Item *item));
static int  EZ_PickAWidgetItem MY_ANSIARGS(( EZ_Item *item, int x, int y));
static void EZ_WidgetItemPostscript MY_ANSIARGS(( EZ_Item *item, FILE *fp, void *psinfo));
/*****************************************************************************/
static EZ_DpyItemHandle WidgetItemHandle =
{
  EZ_ComputeWidgetItemSize,
  EZ_DrawWidgetItem,
  EZ_UpdateWidgetItem,
  EZ_HighlightWidgetItem,
  EZ_FreeWidgetItemPrivateData,
  EZ_PickAWidgetItem,
  EZ_WidgetItemPostscript,
};

/*****************************************************************
 *
 *  Functions dealing with WidgetItem
 */
EZ_Item *EZ_CreateWidgetItem(widget)
     EZ_Widget *widget;
{
  EZ_Item *item;
  
  item = (EZ_Item *)EZ_AllocateNewDisplayItem();
  if(item)
    {
      EZ_DpyItemHandlers[EZ_WIDGET_ITEM] = &WidgetItemHandle;
      EZ_SetItemTypeAndNames(item, EZ_WIDGET_ITEM);

      EZ_WidgetItemDummyWin(item) = EZ_DummyWindow;
      EZ_WidgetItemWidget(item) = NULL;
      if(widget)
	{
	  if(EZ_WidgetParent(widget) != (EZ_Widget *)NULL)
	    {
	      fprintf(stderr, "EZ_CreateWidgetItem(), widget is not a toplevel widget\n");
	      return(NULL);
	    }
	  if(EZ_WidgetParent(widget))  /* cannot do it */
	    {
	      EZ_RemoveWidget(widget);  /* remove it from its parent's children list */
	    }
	  EZ_WidgetItemWidget(item) = widget;
	}
    }
  return(item);
}
/********************************************************************************/

static void EZ_ComputeWidgetItemSize(item, w_ret, h_ret)
     EZ_Item *item;
     int *w_ret, *h_ret;
{
  if(item)
    {
      if(EZ_ItemDirty(item))
	{
	  int tw, th, itmpX, itmpY;
	  EZ_Widget *widget = EZ_WidgetItemWidget(item);
	  EZ_ComputeWidgetWindowSize(widget, &tw, &th, NULL);

	  itmpX = (int)EZ_ItemBorderWidth(item);
	  itmpY = itmpX + (int) EZ_ItemPadY(item) + 2;
	  itmpX = itmpX + (int) EZ_ItemPadX(item) + 2;
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

static void EZ_DrawWidgetItem(widget, drawable, x0, y0, item, Rx, Ry, Rw,Rh)    
     EZ_Widget *widget;
     Drawable  drawable;
     EZ_Item   *item;
     int       x0, y0;
     int       Rx, Ry, Rw,Rh; /* confined region in drawable */
{
  if(item)
    {
      EZ_Widget *wptr = EZ_WidgetItemWidget(item);
      if(wptr)
	{
	  int x,y, xoff,yoff, bdX, bdY, bdwidth;
	  int aw = EZ_ItemAWidth(item);
	  int ah = EZ_ItemAHeight(item);      
	  
	  /* remember the current origin, and which widget it belongs to */
	  EZ_ItemOriginX(item) = x0;
	  EZ_ItemOriginY(item) = y0;
	  EZ_ItemParentWidget(item) = widget;

	  /* Widget type item must always be treated as visible !! */
	  EZ_ItemVisible(item) = 1;	  
      
	  /* fill the background */
	  if(EZ_ItemBackground(item) != 0) 
	    {
	      unsigned long bgpv = EZ_ItemBackground(item);
	      GC gc = EZ_WRITABLEGC;
	      XSetForeground(EZ_Display, gc, bgpv);
	      XFillRectangle(EZ_Display, drawable, gc, 0, 0, aw,ah);
	    }
	  else  EZ_FillItemBackground(widget, item, drawable, x0, y0, aw, ah, x0, y0, NULL);
	  
	  /* now position the widget: Need to set its parent, 
	   * to prevent it from being considered as a toplevel widget!
	   */
	  EZ_WidgetParent(wptr) = widget;
	  EZ_SetDoNotPropagateFlag(wptr);

	  EZ_GetItemXYOffset(item, &xoff, &yoff);
	  EZ_GetItemBwAndXYPading(item, &bdwidth,&bdX, &bdY);

	  if(EZ_GetWidgetExpandFlag(wptr))
	    {
	      int tx = bdX + 2;
	      int ty = bdY + 2;
	      x = x0 + tx;
	      y = y0 + ty;
	      EZ_WidgetWidth(wptr)  = aw - (tx + tx);
	      EZ_WidgetHeight(wptr) = ah - (ty + ty);
	    }
	  else
	    {
	      x = x0 + xoff + bdX + 2;
	      y = y0 + yoff + bdY + 2;
	    }
	  EZ_WidgetOriginX(wptr) = x;
	  EZ_WidgetOriginY(wptr) = y; 
	  
	  EZ_SetWidgetXYSetFlag(wptr);

	  if(EZ_WidgetMapped(wptr)) EZ_WidgetMapped(wptr) = 4;
	  EZ_DisplayWidget(wptr);

	  /* EZ_WidgetParent(wptr) = (EZ_Widget *)NULL; */
	  
	  /* how about border */
	  if(bdwidth > 0)
	    EZ_DrawRectBorderWithSize(widget, drawable, x0,y0,aw,ah,
				      bdwidth, (int)EZ_ItemBorderType(item),0,0,2);
	}
    }
}
/*****************************************************************************/

static void EZ_UpdateWidgetItem(widget, drawable, item, cx,cy,cw,ch)    
     EZ_Widget *widget;
     Drawable  drawable;
     EZ_Item   *item;
     int cx,cy,cw,ch;     /* confined region in drawable */
{
  if(item)
    {
      int bdwidth;
      int aw = EZ_ItemAWidth(item);
      int ah = EZ_ItemAHeight(item);      
      int x0 = EZ_ItemOriginX(item);
      int y0 = EZ_ItemOriginY(item);

      GC  gc;
      unsigned long bgpv;
      Pixmap pixmap;

      pixmap = XCreatePixmap(EZ_Display, drawable, aw, ah, EZ_Depth);

      /* fill the background */
      if(EZ_ItemBackground(item) != 0) 
	{
	  bgpv = EZ_ItemBackground(item);
	  gc = EZ_WRITABLEGC;
	  XSetForeground(EZ_Display, gc, bgpv);
	  XFillRectangle(EZ_Display, pixmap, gc, 0, 0, aw,ah);
	}
      else  EZ_FillItemBackground(widget, item, pixmap, x0, y0, aw, ah, 0, 0,NULL);

      bdwidth = EZ_ItemBorderWidth(item);
      /* how about border */
      if(bdwidth > 0)
	EZ_DrawRectBorderWithSize(widget, pixmap, 0,0,aw,ah,
				  bdwidth, (int)EZ_ItemBorderType(item),0,0,2);

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

static void EZ_HighlightWidgetItem(widget, drawable, item, fg, bg, cx,cy,cw,ch, flag)    
     EZ_Widget *widget;
     Drawable  drawable;
     EZ_Item   *item;
     unsigned long fg,bg; /* ~0 ~0 for default setting */
     int cx,cy,cw,ch;     /* confined region in drawable */
     int flag;
{
  if(item)
    {
      int bdwidth;
      int aw = EZ_ItemAWidth(item);
      int ah = EZ_ItemAHeight(item);      
      GC  gc;
      Pixmap pixmap;
      EZ_Widget *wptr = EZ_WidgetItemWidget(item);

      /* raise the widget window */
      {
	if(wptr == EZ_LookupWidgetFromAllHTUL((unsigned long)wptr) && EZ_WidgetWindow(wptr) &&
	   EZ_WidgetMapped(wptr))
	  XRaiseWindow(EZ_Display, EZ_WidgetWindow(wptr));
      }
      
      pixmap = XCreatePixmap(EZ_Display, drawable, aw, ah, EZ_Depth);

      gc = EZ_WRITABLEGC;
      if(bg == ~0) EZ_GetDarkBDpv(widget, &bg);
      XSetForeground(EZ_Display, gc, bg);
      XFillRectangle(EZ_Display, pixmap, gc, 0, 0, aw,ah);

      /* how about border */
      bdwidth = EZ_ItemBorderWidth(item);
      if(bdwidth > 0)
	EZ_DrawRectBorderWithSize(widget, pixmap, 0,0,aw,ah,
				  bdwidth, (int)EZ_ItemBorderType(item),0,0,2);

      /* finally copy pixmap to drawable */
      {
	int x_r, y_r, w_r, h_r;
	int x0 = EZ_ItemOriginX(item);
	int y0 = EZ_ItemOriginY(item);
	EZ_IntersectRectangles(cx,cy,cw,ch,    /* the drawing area */
			       x0,y0,aw,ah,
			       &x_r,&y_r, &w_r, &h_r);
	XCopyArea(EZ_Display, pixmap, drawable, EZ_WRITABLEGC,x_r-x0,y_r-y0,w_r,h_r, x_r,y_r);
	XFreePixmap(EZ_Display,pixmap);
      }
      
    }
}
/**********************************************************************************************/
static void EZ_FreeWidgetItemPrivateData(item)
     EZ_Item *item;
{
  if(item)
    {
      EZ_Widget *widget = EZ_WidgetItemWidget(item);
      EZ_DestroyWidget(widget);
    }
}

/**********************************************************************************************/
static int EZ_PickAWidgetItem(item,x,y)
     EZ_Item *item;
     int x,y;
{
  int x0,y0,w0,h0;

  x0 = EZ_ItemOriginX(item) -1;
  y0 = EZ_ItemOriginY(item) -1;
  w0 = EZ_ItemAWidth(item) + 2;
  h0 = EZ_ItemAHeight(item) + 2;
  
  if(x >= x0 && x <= x0 + w0 && y >= y0 && y <= y0 + h0) return(1);

  return(0);
}
/**********************************************************************************************/
EZ_Widget  *EZ_GetWidgetItemWidget(item)
     EZ_Item *item;
{
  if(item && EZ_ItemType(item) == EZ_WIDGET_ITEM)
    return(EZ_WidgetItemWidget(item));
  return(NULL);
}
void EZ_SetWidgetItemWidget(item, widget)
     EZ_Item *item; EZ_Widget *widget;
{
  if(item && EZ_ItemType(item) == EZ_WIDGET_ITEM)
    {
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
}
/**********************************************************************************************/
static XImage *ReadWidget2Ximage(widget,w,h) EZ_Widget *widget; int w, h;
{

  XImage *image = NULL;
  if(EZ_WidgetType(widget) == EZ_WIDGET_NW_LABEL)
    {
      int x0 = EZ_WidgetOriginX(widget);
      int y0 = EZ_WidgetOriginY(widget);
      EZ_Widget *parent = EZ_WidgetParent(widget);

      while(parent)
	{
	  if(EZ_WidgetType(parent) == EZ_WIDGET_NW_LABEL)
	    {
	      x0 += EZ_WidgetOriginX(parent);
	      y0 += EZ_WidgetOriginY(parent);
	    }
	  else  break;
	  parent = EZ_WidgetParent(parent);
	}
      if(parent) image = EZ_ReadDrawable2XImage(EZ_WidgetWindow(parent), x0, y0, w, h);
    }
  else  image = EZ_ReadDrawable2XImage(EZ_WidgetWindow(widget), 0, 0, w, h);
  return(image);
}
/**********************************************************************************************/
void EZ_PostscriptWidget(widget, fp)
     EZ_Widget *widget; FILE *fp; 
{
  if(widget && EZ_WidgetMaped(widget))
    {
      int    w,h;
      XImage *image;

      w = EZ_WidgetWidth(widget);
      h = EZ_WidgetHeight(widget);
      image = ReadWidget2Ximage(widget,w,h);

      if(image != (XImage *)NULL)
	{
	  fprintf(fp, "gsave\n");
	  EZ_PSXImage(image, w, h, fp);
	  XDestroyImage(image);
	  fprintf(fp, "\ngrestore\n");
	}
    }
}
/**********************************************************************************************/
void EZ_SaveWidget2Postscript(widget, landscape, dpi, scale, fp)
     EZ_Widget *widget; int landscape, dpi;
     float scale; FILE *fp;
{
  if(widget && EZ_WidgetMaped(widget))
    {
      int    w,h;
      XImage *image;

      w = EZ_WidgetWidth(widget);
      h = EZ_WidgetHeight(widget);
      image = ReadWidget2Ximage(widget,w,h);

      if(image != (XImage *)NULL)
	{
	  float fwidth, fheight, factor, llx, lly;
	  int width, height, devpix;
	  if(landscape) { width = h; height = w; }
	  else  { width = w; height = h; }

	  devpix = (int)( dpi/72.0 + 0.5);
	  factor = 72.0/dpi * devpix;
	  fwidth = scale * width * factor;
	  fheight = scale * height * factor;
	  
	  llx = 0.5 * (612 - fwidth);
	  lly = 0.5 * (762 - fheight);

	  fprintf(fp, "%%!PS-Adobe-2.0 EPSF-2.0\n%%%%Creator: EZWGL\n%%%%Pages: 1\n" );
	  fprintf(fp, "%%%%BoundingBox: %d %d %d %d\n", (int)llx, (int)lly,
		  (int)(llx + fwidth + 0.5), (int)(lly + fheight + 0.5) );
	  fprintf(fp, "%g %g translate\n", llx, lly );
	  fprintf(fp, "%g %g scale\n", fwidth, fheight);
	  if(landscape)
	    fprintf(fp, "0.5 0.5 translate\n90 rotate\n-0.5 -0.5 translate\n" );
	  EZ_PSXImage(image, w, h, fp);
	  XDestroyImage(image);
	  fprintf(fp, "\nshowpage\n");
	}
    }
}
void EZ_SaveWidget2PPM(widget, fp) EZ_Widget *widget; FILE *fp;
{
  if(widget && EZ_WidgetMaped(widget))
    {
      int    w,h;
      XImage *image;
      w = EZ_WidgetWidth(widget);
      h = EZ_WidgetHeight(widget);
      image = ReadWidget2Ximage(widget,w,h);

      if(image != (XImage *)NULL)
	{
	  EZ_PPMXImage(image, w, h, fp);
	  XDestroyImage(image);
	}
    }
}
/**********************************************************************************************/
static void EZ_WidgetItemPostscript(item, fp, psinfo)
     EZ_Item *item; FILE *fp; void *psinfo;
{

}
/**********************************************************************************************/
#undef _EZ_ITEM_WIDGET_C_
