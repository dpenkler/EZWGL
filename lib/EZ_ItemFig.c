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
 * Fig item.
 *
 *****************************************************************/
#define _EZ_ITEM_FIG_C_
#include "EZ_Widget.h"
/*****************************************************************
 *
 * public functions
 */
GC     EZ_GetGC MY_ANSIARGS((unsigned long mask, XGCValues *gcv));
void   EZ_FreeGC MY_ANSIARGS((GC gc));
void   EZ_FreezeFigItem MY_ANSIARGS((EZ_Item *item));
void   EZ_UnFreezeFigItem MY_ANSIARGS((EZ_Item *item));

EZ_Item    *EZ_CreateFigItem MY_ANSIARGS((void));

void          EZ_FigItemDeletePiece MY_ANSIARGS((EZ_Item *item, EZ_FigPiece *fpiece, int upd));
void          EZ_FigItemDeleteAllPieces MY_ANSIARGS((EZ_Item *item, int upd));
void          EZ_FigItemHidePiece MY_ANSIARGS((EZ_Item *item, EZ_FigPiece *fpiece, int upd));
void          EZ_FigItemShowPiece MY_ANSIARGS((EZ_Item *item, EZ_FigPiece *fpiece, int upd));

void          EZ_ScaleFigPiece MY_ANSIARGS((EZ_FigPiece *fpiece, float sx, float sy));
void          EZ_GetFigPieceBoundingBox MY_ANSIARGS((EZ_FigPiece *fpiece,
						     int *x1, int *y1, int *x2, int *y2));

EZ_FigPiece  *EZ_FigItemAddPiece MY_ANSIARGS((EZ_Item *fitem, int ptype,
					      GC theGC,
					      int x, int y, int width, int height, int other,
					      int npoints, XPoint *xpts, int mode));

EZ_FigPiece  *EZ_FigItemAddString MY_ANSIARGS((EZ_Item *fitem, GC theGC,
					    char *str, int slen, int justification, int x, int y));
EZ_FigPiece  *EZ_FigItemAddPixmap MY_ANSIARGS((EZ_Item *fitem, GC theGC,
				       EZ_Bitmap *bmp, int sx, int sy, int sw, int sh, int dx, int dy));

EZ_FigPiece  *EZ_FigItemAddPoints MY_ANSIARGS((EZ_Item *fitem, GC theGC,
				       XPoint *pts, int npts));

EZ_FigPiece  *EZ_FigItemAddLines  MY_ANSIARGS((EZ_Item *fitem, GC theGC,
				      XPoint *pts, int npts, int mode));

EZ_FigPiece  *EZ_FigItemAddPolygon MY_ANSIARGS((EZ_Item *fitem, GC theGC,
				      XPoint *pts, int npts, int mode));

EZ_FigPiece  *EZ_FigItemAddRectangle MY_ANSIARGS((EZ_Item *fitem, GC theGC,
					 int x, int y, int w, int h));

EZ_FigPiece  *EZ_FigItemAddFilledRectangle MY_ANSIARGS((EZ_Item *fitem, GC theGC,
					       int x, int y, int w, int h));

EZ_FigPiece  *EZ_FigItemAddArc MY_ANSIARGS((EZ_Item *fitem, GC theGC,
				   int x, int y, int w, int h, int a1, int a2));

EZ_FigPiece  *EZ_FigItemAddFilledArc MY_ANSIARGS((EZ_Item *fitem, GC theGC,
					 int x, int y, int w, int h, int a1, int a2));

EZ_FigPiece  *EZ_FigItemAddRectangles MY_ANSIARGS((EZ_Item *fitem, GC theGC,
					  XRectangle *rects, int nrects));

EZ_FigPiece  *EZ_FigItemAddFilledRectangles MY_ANSIARGS((EZ_Item *fitem, GC theGC,
						XRectangle *rects, int nrects));

EZ_FigPiece  *EZ_FigItemAddArcs MY_ANSIARGS((EZ_Item *fitem, GC theGC,
				    XArc *arcs, int narcs));

EZ_FigPiece  *EZ_FigItemAddFilledArcs MY_ANSIARGS((EZ_Item *fitem, GC theGC,
					  XArc *arcs, int narcs));

EZ_FigPiece  *EZ_FigItemAddSegments MY_ANSIARGS((EZ_Item *fitem, GC theGC,
						 XSegment *segs, int nsegs));

void  EZ_DuplicatePartList MY_ANSIARGS((EZ_Item *item, EZ_FigPiece **np, EZ_FigPiece *op));

void  EZ_ScaleFigItem MY_ANSIARGS((EZ_Item *item, float sx, float sy, int mode ));

Pixmap EZ_MakePixmapFromFigItem MY_ANSIARGS((EZ_Item *fitem, int *width, int *height));
/*****************************************************************/
static void EZ_ComputeFigItemSize MY_ANSIARGS((EZ_Item *item, int *w, int *h));
static void EZ_DrawFigItem MY_ANSIARGS((EZ_Widget *widget,
					Drawable  drawable,
					int       x0, int y0,
					EZ_Item   *item,
					int       Rx, int Ry, int Rw, int Rh));
static void EZ_FreeFigItemPrivateData MY_ANSIARGS((EZ_Item *item));
static void EZ_UpdateFigItem MY_ANSIARGS((EZ_Widget *widget,
					  Drawable  drawable,
					  EZ_Item   *item,
					  int       Rx, int Ry, int Rw, int Rh));
static void EZ_HighlightFigItem MY_ANSIARGS((EZ_Widget *widget,
					     Drawable  drawable,
					     EZ_Item   *item,
					     unsigned long fg, unsigned long bg,
					     int       Rx, int Ry, int Rw, int Rh, int hmode));
static int  EZ_PickAFigItem MY_ANSIARGS(( EZ_Item *item, int x, int y));

static int xyIsOnOrInArc MY_ANSIARGS((int x,int y,int x0,int y0,int w,int h,int a1,int a2, int mode));
static void EZ_FigItemPostscript MY_ANSIARGS(( EZ_Item *item, FILE *fp, void *psinfo));
/************************************************************************************/
static void EZ_DrawFigItemWork MY_ANSIARGS((EZ_Widget *widget,
					    Drawable  drawable,
					    int       x0, int y0,
					    EZ_Item   *item,
					    int       Rx, int Ry, int Rw, int Rh, int which, int s,
					    unsigned long fg, unsigned long bg, int hmode));

static int PointLineDist MY_ANSIARGS((int x, int y, int x1, int y1, int x2, int y2, int width));
static int IntersectPolygonEdge MY_ANSIARGS((int x, int y, int x1, int y1, int x2, int y2));
static void EZ_IncrementGCRef MY_ANSIARGS((GC gc, int *lw, XFontStruct **xft));

/************************************************************************************/
static EZ_DpyItemHandle FigItemHandle =
{
  EZ_ComputeFigItemSize,
  EZ_DrawFigItem,
  EZ_UpdateFigItem,
  EZ_HighlightFigItem,
  EZ_FreeFigItemPrivateData,
  EZ_PickAFigItem,
  EZ_FigItemPostscript,
};
/*****************************************************************
 *
 *  Functions dealing with LabelItem
 */
EZ_Item *EZ_CreateFigItem()
{
  EZ_Item *item;
  
  item = (EZ_Item *)EZ_AllocateNewDisplayItem();
  if(item)
    {
      EZ_DpyItemHandlers[EZ_FIG_ITEM] = &FigItemHandle;
      EZ_SetItemTypeAndNames(item, EZ_FIG_ITEM);
      EZ_FigItemPartList(item) = NULL;
      EZ_FigItemSx(item)=1.0;
      EZ_FigItemSy(item)=1.0;
    }
  return(item);
}
/********************************************************************************/
static void DestroyFigItemPiece(act) EZ_FigPiece *act;
{
  if(act)
    {
      if(EZ_FigPieceData(act)) 
	{
	  if(EZ_FigPieceType(act) == EZ_FIG_PIXMAP)
	    EZ_FreeBitmap((EZ_Bitmap *)EZ_FigPieceData(act));
	  else my_free(EZ_FigPieceData(act));
	}
      EZ_FreeGC(EZ_FigPieceGC(act));
      my_free(act);
    }
}
static void EZ_FreeFigItemPrivateData(item)
     EZ_Item *item;
{
  if(item && EZ_ItemType(item) == EZ_ITEM_FIG)
    {
      EZ_FigPiece *next, *act = EZ_FigItemPartList(item);
      while(act)
	{
	  next = act->next;
	  DestroyFigItemPiece(act);
	  act = next;
	}
      EZ_FigItemPartList(item) = NULL;
      if(EZ_FigItemSSChartJunk(item))
        {
          (void)my_free(EZ_FigItemSSChartJunk(item));
          EZ_FigItemSSChartJunk(item) = NULL;
        }
    }
}
/********************************************************************************/

#define bbox(x_,y_, x1_,y1_,x2_,y2_) \
{\
  if(x_ < x1_) x1_ = x_;\
  if(y_ < y1_) y1_ = y_;\
  if(x_ > x2_) x2_ = x_;\
  if(y_ > y2_) y2_ = y_;\
}
static void computeFPieceBBox(act, rx1,ry1,rx2,ry2) 
     EZ_FigPiece *act; int *rx1, *ry1, *rx2, *ry2;
{
  if(act && EZ_FigPieceClean(act) == 0)
    {
      int type = act->figType;
      int x1,y1,x2,y2;
      switch(type)
	{
	case EZ_FIG_POINTS:
	case EZ_FIG_POLYLINE:
	case EZ_FIG_POLYGON:
	  {
	    EZ_FigPiece *ae = act;
	    int npoints;
	    if(ae && (npoints = EZ_FigPieceNPoints(ae)) > 0)
	      {
		if(EZ_FigPieceClean(ae) == 0)
		  {
		    int i, lwidth, x,y, x1,y1,x2,y2;
		    XPoint *data = EZ_FigPieceData(ae);
		    x = data[0].x; y = data[0].y;
		    x1=x2=x; y1=y2=y;
		    for(i = 1; i < npoints; i++)
		      {
			x += data[i].x; y += data[i].y;
			bbox(x,y,x1,y1,x2,y2);
		      }
		    lwidth = EZ_FigPieceLineWidth(ae);
		    ae->x1 = x1 - lwidth; ae->y1 = y1 - lwidth;
		    ae->x2 = x2 + lwidth; ae->y2 = y2 + lwidth;
		  }
	      }
	    else
	      {
		ae->x1 = 0; ae->y1 = 0;
		ae->x2 = 0; ae->y2 = 0;
	      }
	  }
	break;
	case EZ_FIG_SEGMENTS:
	  {
	    EZ_FigPiece *ae = act;
	    int nsegs;
	    if(ae && (nsegs = EZ_FigPieceNPoints(ae)) > 0)
	      {
		if(EZ_FigPieceClean(ae) == 0)
		  {
		    int i, lwidth, x1,y1,x2,y2;
		    XSegment *data = (XSegment *)EZ_FigPieceData(ae);
		    x1 = y1 = (1 << 30); x2 = y2 = - x1;
		    for(i = 0; i < nsegs; i++)
		      {
			int xx1 = data[i].x1;
			int yy1 = data[i].y1;
			int xx2 = data[i].x2;
			int yy2 = data[i].y2;
			bbox(xx1,yy1,x1,y1,x2,y2);
			bbox(xx2,yy2,x1,y1,x2,y2);
		      }
		    lwidth = EZ_FigPieceLineWidth(ae);
		    ae->x1 = x1 - lwidth; ae->y1 = y1 - lwidth;
		    ae->x2 = x2 + lwidth; ae->y2 = y2 + lwidth;
		  }
	      }
	    else
	      {
		ae->x1 = 0; ae->y1 = 0;
		ae->x2 = 0; ae->y2 = 0;
	      }
	  }
	break;
	case EZ_FIG_RECTANGLE:
	case EZ_FIG_FILLED_RECTANGLE:
	case EZ_FIG_ARC:
	case EZ_FIG_FILLED_ARC:
	  {
	    int lwidth,x1,y1,x2,y2;
	    x1 = EZ_FigPieceX(act);
	    y1 = EZ_FigPieceY(act);
	    /*if(x1 > 0) x1 = 0;
	      if(y1 > 0) y1 = 0;*/

	    x2 = EZ_FigPieceX(act) + EZ_FigPieceWidth(act);
	    y2 = EZ_FigPieceY(act) + EZ_FigPieceHeight(act);      
	    lwidth = EZ_FigPieceLineWidth(act);
	    act->x1 = x1 - lwidth; act->y1 = y1 - lwidth;
	    act->x2 = x2 + lwidth; act->y2 = y2 + lwidth;
	  }			
	break;
	case EZ_FIG_RECTANGLES:
	case EZ_FIG_FILLED_RECTANGLES:
	  {
	    int nrects = EZ_FigPieceNPoints(act);
	    if(nrects > 0)
	      {
		int i, lwidth,x1,y1,x2,y2, xx1,yy1,xx2,yy2;
		XRectangle *rects = (XRectangle *)EZ_FigPieceData(act);
			
		xx1 = yy1 = (1<<30); xx2 = yy2 = -xx1;
		for(i = 0; i < nrects; i++)
		  {
		    x1 = rects[i].x; y1 = rects[i].y;
		    /*if(x1 > 0) x1 = 0; if(y1>0) y1 = 0;*/
		    x2 = rects[i].x + rects[i].width;
		    y2 = rects[i].y + rects[i].height;
		    if(x1 < xx1) xx1 = x1;
		    if(y1 < yy1) yy1 = y1;
		    if(x2 > xx2) xx2 = x2;
		    if(y2 > yy2) yy2 = y2;
		  }
		lwidth = EZ_FigPieceLineWidth(act);
		act->x1 = xx1 - lwidth; act->y1 = yy1 - lwidth;
		act->x2 = xx2 + lwidth; act->y2 = yy2 + lwidth;
	      }
	    else { act->x1 =0; act->y1 =0; act->x2 =0; act->y2 =0;}
	  }
	break;
	case EZ_FIG_ARCS:
	case EZ_FIG_FILLED_ARCS:
	  {
	    int narcs = EZ_FigPieceNPoints(act);
	    if(narcs > 0)
	      {
		int i,lwidth,x1,y1,x2,y2, xx1,yy1,xx2,yy2;
		XArc *arcs = (XArc *)EZ_FigPieceData(act);
			
		xx1 = yy1 = (1<< 30); xx2 = yy2 = -xx1;			
		for(i = 0; i < narcs; i++)
		  {
		    x1 = arcs[i].x; y1 = arcs[i].y;
		    /* if(x1 > 0) x1 = 0; if(y1>0) y1 = 0;*/
		    x2 = arcs[i].x + arcs[i].width;
		    y2 = arcs[i].y + arcs[i].height;
		    if(x1 < xx1) xx1 = x1;
		    if(y1 < yy1) yy1 = y1;
		    if(x2 > xx2) xx2 = x2;
		    if(y2 > yy2) yy2 = y2;
		  }
		lwidth = EZ_FigPieceLineWidth(act);
		act->x1 = xx1 - lwidth; act->y1 = yy1 - lwidth;
		act->x2 = xx2 + lwidth; act->y2 = yy2 + lwidth;
	      }
	    else { act->x1 =0; act->y1 =0; act->x2 =0; act->y2 =0;}
	  }
	break;

	case EZ_FIG_STRING:
	  {
	    int nlines, twidth, theight;
	    XFontStruct *fs = EZ_FigPieceXFont(act);
	    EZ_FigureLabelExtent(fs,
				 (char *)EZ_FigPieceData(act),
				 (int)EZ_FigPieceStrLength(act),
				 1024, &nlines, &twidth);
	    theight = (fs->ascent + fs->descent);
			
	    EZ_FigPieceStrNLines(act) = nlines;
	    EZ_FigPieceStrWidth(act) = twidth;
	    EZ_FigPieceStrHeight(act) = theight;
	    x1 = EZ_FigPieceX(act);
	    y1 = EZ_FigPieceY(act);
	    /*if(x1 > 0) x1 = 0;
	      if(y1 > 0) y1 = 0; */
	    x2 = EZ_FigPieceX(act) + twidth;
	    y2 = EZ_FigPieceY(act) + theight * nlines;
	    act->x1 = x1; act->y1 = y1;
	    act->x2 = x2; act->y2 = y2;
	  }
	break;
	case EZ_FIG_PIXMAP:
	  {
	    EZ_Bitmap *bitmap = (EZ_Bitmap *)EZ_FigPieceData(act);
	    if(bitmap)
	      {
		int x1,y1,x2,y2, w, h;
		x1 = EZ_FigPieceX(act);
		y1 = EZ_FigPieceY(act);
		w = EZ_FigPiecePixmapW(act);
		h = EZ_FigPiecePixmapH(act);
		x2 = x1 + w;
		y2 = y1 + h;
		/*if(x1 > 0) x1 = 0;
		  if(y1 > 0) y1 = 0;*/
		act->x1 = x1; act->y1 = y1;
		act->x2 = x2; act->y2 = y2;
	      } 
	    else
	      {
		act->x1 = 0; act->y1 = 0;
		act->x2 = 0; act->y2 = 0;
	      }
	  }
	break;
	default:
	  break;
	}
      EZ_FigPieceClean(act) = 1;
    }
  if(act)
    {
      *rx1 = act->x1; *ry1 = act->y1;
      *rx2 = act->x2; *ry2 = act->y2;
    }
  else
    {
      *rx1 = 0; *ry1 = 0; *rx2 = 0; *ry2 = 0;
    }
}

/*******************************************************************************/
void EZ_GetFigPieceBoundingBox(fpiece, x1, y1,x2,y2)
     EZ_FigPiece *fpiece;
     int *x1, *y1, *x2, *y2;
{
  computeFPieceBBox(fpiece, x1,y1,x2,y2);
}

static void EZ_ComputeFigItemSize(item, w_ret, h_ret)
     EZ_Item *item;
     int *w_ret, *h_ret;
{
  if(item)
    {
      if(EZ_ItemDirty(item))
	{
	  EZ_FigPiece *act, *sact = EZ_FigItemPartList(item);
	  int x1,y1,x2,y2, itmpX, itmpY;
	  act = sact;
	  while(act)
	    {
	      computeFPieceBBox(act, &x1,&y1,&x2,&y2);
	      act = act->next;
	    }
	  act = sact;
	  if(act)
	    {
	      x1 = y1 = (1<<30); x2 = y2 = -x1;
	      while(act)	  
		{
		  if(x1 > act->x1) x1 = act->x1;
		  if(y1 > act->y1) y1 = act->y1;
		  if(x2 < act->x2) x2 = act->x2;
		  if(y2 < act->y2) y2 = act->y2;
		  act = act->next;
		}
	    }
	  else { x1 = y1 = x2 = y2 = 0;}

	  itmpX = (int)EZ_ItemBorderWidth(item);
	  itmpY = itmpX + (int) EZ_ItemPadY(item) + 1;
	  itmpX = itmpX + (int) EZ_ItemPadX(item) + 1;
	  itmpX = itmpX + itmpX;
	  itmpY = itmpY + itmpY;
	  if(x1 > 0) x1 = 0;   if(y1 > 0) y1 = 0;
          EZ_FigItemXOffset(item) = x1;
          EZ_FigItemYOffset(item) = y1;
	  EZ_ItemWidth(item) = (x2 - x1) + itmpX;
	  EZ_ItemHeight(item) = (y2 - y1) + itmpY;
	  EZ_ItemAWidth(item) = EZ_ItemWidth(item);
	  EZ_ItemAHeight(item) = EZ_ItemHeight(item);
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
#undef bbox
/************************************************************************/
static void EZ_DrawFigItem(widget, drawable, x0, y0, item, Rx, Ry, Rw,Rh)    
     EZ_Widget *widget;
     Drawable  drawable;
     EZ_Item   *item;
     int       x0, y0;
     int       Rx, Ry, Rw,Rh; /* confined region in drawable */
{
  int bdwidth;
  EZ_DrawFigItemWork(widget, drawable, x0, y0, item, Rx, Ry, Rw,Rh, 0, 1, 0,0,0);
  if(item && (bdwidth=EZ_ItemBorderWidth(item)) > 0)
    {
      int aw = EZ_ItemAWidth(item);
      int ah = EZ_ItemAHeight(item);
      EZ_DrawRectBorderWithSize(widget, drawable, x0,y0,aw,ah,
                                bdwidth, (int)EZ_ItemBorderType(item),0,0, 2);
    }
}


/*****************************************************************************/

static void EZ_UpdateFigItem(widget, drawable, item, cx,cy,cw,ch)    
     EZ_Widget *widget;
     Drawable  drawable;
     EZ_Item   *item;
     int cx,cy,cw,ch;     /* confined region in drawable */
{
  if(item && EZ_ItemVisible(item))
    {
      int wtype = (widget? EZ_WidgetType(widget): 0);
      int x0 = EZ_ItemOriginX(item);
      int y0 = EZ_ItemOriginY(item);
      if(wtype == EZ_WIDGET_IWORK_AREA)
        {
	  int aw = EZ_ItemAWidth(item);
	  int ah = EZ_ItemAHeight(item);
          int bdwidth = EZ_ItemBorderWidth(item);
          EZ_DrawFigItemWork(widget, drawable, x0, y0, item, cx, cy, cw,ch, 0, 1, 0,0,0);
          if(bdwidth > 0)
            EZ_DrawRectBorderWithSize(widget, drawable, x0,y0,aw,ah,
                                      bdwidth, (int)EZ_ItemBorderType(item), 0,0, 6);
        }
      else
	{
	  int aw = EZ_ItemAWidth(item);
	  int ah = EZ_ItemAHeight(item);
          int bdwidth = EZ_ItemBorderWidth(item);
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
	  else EZ_FillItemBackground(widget, item, pixmap, x0, y0, aw, ah, 0, 0, &bgpv);

	  /* render item */
	  EZ_DrawFigItemWork(widget, pixmap, 0, 0, item, cx, cy, cw,ch, 0, 0, 0,0,0);	  
          if(bdwidth > 0)
            EZ_DrawRectBorderWithSize(widget, pixmap, 0, 0,aw,ah,
                                      bdwidth, (int)EZ_ItemBorderType(item),0,0, 0);
            /*EZ_DrawRectBorderWithSize(widget, drawable, x0,y0,aw,ah,
                                      bdwidth, (int)EZ_ItemBorderType(item),-x0,-y0, 6); */
	  { /* copy pixmap to drawable */
	    int x_r, y_r, w_r, h_r;

	    EZ_IntersectRectangles(cx,cy,cw,ch,    /* the drawing area */
				   x0,y0,aw,ah,
				   &x_r,&y_r, &w_r, &h_r);
	    XCopyArea(EZ_Display, pixmap, drawable, EZ_WRITABLEGC,x_r-x0,y_r-y0,w_r,h_r, x_r,y_r);
	    XFreePixmap(EZ_Display,pixmap);
	  }
	}
    }
}
/********************************************************************************************/

static void EZ_HighlightFigItem(widget, drawable, item, fg, bg, cx,cy,cw,ch, hmode)    
     EZ_Widget *widget;
     Drawable  drawable;
     EZ_Item   *item;
     unsigned long fg,bg; /* ~0 ~0 for default setting */
     int cx,cy,cw,ch;     /* confined region in drawable */
     int hmode;           /* method to highlight item */
{
  if(item && EZ_ItemVisible(item))
    {
      int wtype = (widget? EZ_WidgetType(widget): 0);
      int x0 = EZ_ItemOriginX(item);
      int y0 = EZ_ItemOriginY(item);

      if(wtype == EZ_WIDGET_IWORK_AREA || hmode >= 2) 
        {
	  int aw = EZ_ItemAWidth(item);
	  int ah = EZ_ItemAHeight(item);
          int bdwidth = EZ_ItemBorderWidth(item);
          EZ_DrawFigItemWork(widget, drawable, x0, y0, item, cx, cy, cw,ch, 2, 0, fg,bg,hmode);
          if(bdwidth > 0)
            EZ_DrawRectBorderWithSize(widget, drawable, x0,y0,aw,ah,
                                      bdwidth, (int)EZ_ItemBorderType(item), 0,0, 2);
        }
      else
	{
	  int aw = EZ_ItemAWidth(item);
	  int ah = EZ_ItemAHeight(item);      
          int bdwidth = EZ_ItemBorderWidth(item);
	  GC  gcb;
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
	  XFillRectangle(EZ_Display, pixmap, gcb, 0, 0, aw,ah);
	  EZ_DrawFigItemWork(widget, pixmap, 0, 0, item, cx, cy, cw,ch, 0,0, 0,0,hmode);	  	  
          if(bdwidth > 0)
            EZ_DrawRectBorderWithSize(widget, pixmap, 0,0,aw,ah,
                                      bdwidth, (int)EZ_ItemBorderType(item), 0, 0, 0);
            /*EZ_DrawRectBorderWithSize(widget, drawable, x0,y0,aw,ah,
                                      bdwidth, (int)EZ_ItemBorderType(item), -x0,-y0, 6); */
	  {
	    int x_r, y_r, w_r, h_r;
	    EZ_IntersectRectangles(cx,cy,cw,ch,    /* the drawing area */
				   x0,y0,aw,ah,
				   &x_r,&y_r, &w_r, &h_r);
	    XCopyArea(EZ_Display, pixmap, drawable, gcb,x_r-x0,y_r-y0,w_r,h_r, x_r,y_r);
	    XFreePixmap(EZ_Display,pixmap);
	  }
	}
    }
}
/**********************************************************************************************/
static int  EZ_PickAFigItem(item,x,y)
     EZ_Item *item;
     int x,y;
{
  int x0,y0,w0,h0;

  x0 = EZ_ItemOriginX(item);
  y0 = EZ_ItemOriginY(item);
  w0 = EZ_ItemAWidth(item);
  h0 = EZ_ItemAHeight(item);

  if(x >= x0 && x <= x0 + w0 && y >= y0 && y <= y0 + h0)
    {
      if(EZ_ItemBackground(item) != 0 ||
         (EZ_ItemBorderWidth(item) > 0 &&  EZ_ItemBorderType(item) != EZ_BORDER_NONE))
	return(1);
      else
	{
	  int xoff,yoff, bdX, bdY, bdwidth,ddx,ddy;
          int xoffset = EZ_FigItemXOffset(item);
          int yoffset = EZ_FigItemYOffset(item);
	  EZ_FigPiece *act = EZ_FigItemPartList(item);
	  int linewidth = 0;	  

	  EZ_GetItemXYOffset(item, &xoff, &yoff);
	  EZ_GetItemBwAndXYPading(item, &bdwidth,&bdX, &bdY);
	  ddx = x0 + xoff + bdX - xoffset;
	  ddy = y0 + yoff + bdY - yoffset;

	  while(act)
	    {
	      if(EZ_FigPieceHidden(act) == 0)
		{
		  int dx, dy, px,py,type = act->figType;
		  linewidth = EZ_FigPieceLineWidth(act);
	      
		  switch(type)
		    {
		    case EZ_FIG_POINTS:
		      {
			int i, npts = EZ_FigPieceNPoints(act);
			XPoint *xpts =  EZ_FigPieceData(act);
			px = ddx; py = ddy;
			for(i = 0; i < npts; i++)
			  {
			    px += xpts[i].x; py += xpts[i].y;
			    dx = x - px; dy = y - py;
			    if(ABSV(dx) + ABSV(dy) <= linewidth)
			      {
				return(1); 
				break;
			      }
			  }
		      }
		    break;
		    case EZ_FIG_POLYLINE:
		      {
			int width, tx1,ty1,i, npts = EZ_FigPieceNPoints(act);
			XPoint *xpts =  EZ_FigPieceData(act);
			px = ddx+xpts[0].x; py = ddy+xpts[0].y;
			width = (linewidth >> 1) + 1;
			for(i = 1; i < npts; i++)
			  {
			    tx1 = px + xpts[i].x; ty1 = py + xpts[i].y;
			    if(PointLineDist(x,y, px, py, tx1, ty1, width))
			      return(1);
			    px = tx1; py = ty1;
			  }
		      }
		    break;
		    case EZ_FIG_SEGMENTS:
		      {
			int width, tx1,ty1,i, nsegs = EZ_FigPieceNPoints(act);
			XSegment *segs =  (XSegment *)EZ_FigPieceData(act);
			width = (linewidth >> 1) + 1;
			for(i = 0; i < nsegs; i++)
			  {
			    px = segs[i].x1 + ddx;
			    py = segs[i].y1 + ddy;
			    tx1 = segs[i].x2 + ddx;
			    ty1 = segs[i].y2 + ddy;
			    if(PointLineDist(x,y, px, py, tx1, ty1, width))
			      return(1);
			  }
		      }
		    break;

		    case EZ_FIG_POLYGON:
		      {
			int tx1,ty1,i, npts = EZ_FigPieceNPoints(act);
			XPoint *xpts =  EZ_FigPieceData(act);
			int savex, savey, count = 0;
			px = ddx+xpts[0].x; py = ddy+xpts[0].y;		
			savex = px; savey = py;
			for(i = 1; i < npts; i++)
			  {
			    tx1 = px + xpts[i].x; ty1 = py + xpts[i].y;
			    count += IntersectPolygonEdge(x,y,px,py,tx1,ty1);
			    px = tx1; py = ty1;
			  }
			count += IntersectPolygonEdge(x,y,px,py,savex, savey);
			return(count & 1);
		      }
		    break;
		    case EZ_FIG_RECTANGLE:
		      {
			int tx1, ty1, width = (linewidth >> 1) + 1;
			px = ddx + EZ_FigPieceX(act);
			py = ddy + EZ_FigPieceY(act); 
			tx1 = px + EZ_FigPieceWidth(act);
			ty1 = py + EZ_FigPieceHeight(act);
			if(PointLineDist(x,y, px, py, tx1, py, width) ||
			   PointLineDist(x,y, px, py, px, ty1, width) ||
			   PointLineDist(x,y, tx1, py, tx1, ty1, width) ||
			   PointLineDist(x,y, px, ty1, tx1, ty1, width))
			  return(1);
		      }
		    break;
		    case EZ_FIG_RECTANGLES:
		      {
			int i, nrects = EZ_FigPieceNPoints(act);
			XRectangle *rects = (XRectangle *)EZ_FigPieceData(act);
			int tx1, ty1, width = (linewidth >> 1) + 1;
			for(i = 0; i < nrects; i++)
			  {
			    px = ddx + rects[i].x;  
			    py = ddy + rects[i].y;
			    tx1 = px + rects[i].width;
			    ty1 = py + rects[i].height;
			    if(PointLineDist(x,y, px, py, tx1, py, width) ||
			       PointLineDist(x,y, px, py, px, ty1, width) ||
			       PointLineDist(x,y, tx1, py, tx1, ty1, width) ||
			       PointLineDist(x,y, px, ty1, tx1, ty1, width))
			      return(1);
			  }
		      }
		    break;
		    case EZ_FIG_FILLED_RECTANGLE:
		      {
			int tx1, ty1, width = (linewidth >> 1) + 1;
			px = ddx + EZ_FigPieceX(act) - width;
			py = ddy + EZ_FigPieceY(act) - width; 
			tx1 = px + EZ_FigPieceWidth(act) + linewidth;
			ty1 = py + EZ_FigPieceHeight(act) +linewidth;
			if(x >= px && x <= tx1 && y >= py && y <= ty1) return(1);
		      }
		    break;
		    case EZ_FIG_FILLED_RECTANGLES:
		      {
			int i, nrects = EZ_FigPieceNPoints(act);
			XRectangle *rects = (XRectangle *)EZ_FigPieceData(act);
			int tx1, ty1;
			for(i = 0; i < nrects; i++)
			  {
			    px = ddx + rects[i].x;  
			    py = ddy + rects[i].y;
			    tx1 = px + rects[i].width;
			    ty1 = py + rects[i].height;
			    if(x >= px && x <= tx1 && y >= py && y <= ty1) return(1);
			  }
		      }
		    case EZ_FIG_ARC:
		      {
			if(xyIsOnOrInArc(x,y, ddx+EZ_FigPieceX(act),ddy+EZ_FigPieceY(act),
					 EZ_FigPieceWidth(act), EZ_FigPieceHeight(act),
					 EZ_FigPieceAngle1(act), EZ_FigPieceAngle2(act), 0))
			  return(1);
		      }
		    break;
		    case EZ_FIG_FILLED_ARC:
		      {
			if(xyIsOnOrInArc(x,y, ddx+EZ_FigPieceX(act),ddy+EZ_FigPieceY(act),
					 EZ_FigPieceWidth(act), EZ_FigPieceHeight(act),
					 EZ_FigPieceAngle1(act), EZ_FigPieceAngle2(act), 1))
			  return(1);
		      }
		    break;
		    case EZ_FIG_ARCS:
		      {
			int i, narcs = EZ_FigPieceNPoints(act);
			XArc *arcs = (XArc *)EZ_FigPieceData(act);
			for(i = 0; i < narcs; i++)
			  {
			    if(xyIsOnOrInArc(x,y, ddx+arcs[i].x,ddy+arcs[i].y,
					     arcs[i].width, arcs[i].height,
					     arcs[i].angle1,arcs[i].angle2, 0))
			      return(1);
			  }
		      }
		    break;
		    case EZ_FIG_FILLED_ARCS:
		      {
			int i, narcs = EZ_FigPieceNPoints(act);
			XArc *arcs = (XArc *)EZ_FigPieceData(act);
			for(i = 0; i < narcs; i++)
			  {
			    if(xyIsOnOrInArc(x,y, ddx+arcs[i].x,ddy+arcs[i].y,
					     arcs[i].width, arcs[i].height,
					     arcs[i].angle1,arcs[i].angle2, 1))
			      return(1);
			  }
		      }
		    break;
		    case EZ_FIG_PIXMAP:
		      {
			EZ_Bitmap *bitmap = (EZ_Bitmap *)EZ_FigPieceData(act);
			if(bitmap)
			  {
			    int tx1, ty1;
			    px = ddx + EZ_FigPieceX(act);
			    py = ddy + EZ_FigPieceY(act);
			    tx1 = px + EZ_FigPiecePixmapW(act);
			    ty1 = py + EZ_FigPiecePixmapH(act);
			    if(x >= px && x <= tx1 && y >= py && y <= ty1) return(1);
			  }
		      }
		    break;
		    case EZ_FIG_STRING:
		      {
			int tx1, ty1;
			px = ddx + EZ_FigPieceX(act);
			py = ddy + EZ_FigPieceY(act);
			tx1 = px + EZ_FigPieceStrWidth(act);
			ty1 = py + EZ_FigPieceHeight(act) *  EZ_FigPieceStrNLines(act);
			if(x >= px && x <= tx1 && y >= py && y <= ty1) return(1);
		      }
		    break;
		    default:
		      break;
		    }
		}
	      act = act->next;
	    }
	}
    }
  return(0);
}
/**********************************************************************************************/
static EZ_FigPiece *allocateNewFigPiece(x0,y0,w0,h0, type)
     int x0,y0,w0,h0, type;
{
  EZ_FigPiece *fpiece = (EZ_FigPiece *)my_malloc(sizeof(EZ_FigPiece), _FIG_ITEM_DATA_);
  memset(fpiece, 0, sizeof(EZ_FigPiece));
  EZ_FigPieceLineWidth(fpiece) = 0;
  EZ_FigPieceX(fpiece) = x0;
  EZ_FigPieceY(fpiece) = y0;
  EZ_FigPieceWidth(fpiece) = w0;
  EZ_FigPieceHeight(fpiece) = h0;
  EZ_FigPieceType(fpiece) = type;  
  return(fpiece);
}

/**********************************************************************************************/
EZ_FigPiece *EZ_FigItemAddPiece(item, type, theGC, x,y,w,h,other,npts, xpts, mode)
     EZ_Item *item; 
     int type;
     GC theGC;
     int x, y, w,h, other;
     int npts;
     XPoint *xpts;
     int mode;
{
  EZ_FigPiece *fpiece = NULL;
  if(item && EZ_ItemType(item) == EZ_FIG_ITEM)
    {
      int scale = 0, line_width = 0;
      float xsf = EZ_FigItemSx(item);
      float ysf = EZ_FigItemSy(item);
      XFontStruct *xft = NULL;
      
      if(xsf != 1.0 || ysf != 1.0) scale = 1;
      
      fpiece = allocateNewFigPiece(x,y,w,h, type);
      if(theGC == None)
	{
	  theGC = EZ_FIGGC;
	  line_width = 1;
	  xft = EZ_GetFontFromId(EZ_LABEL_FONT);
	}
      else 
	{
	  EZ_IncrementGCRef(theGC, &line_width, &xft);
	}
      EZ_FigPieceGC(fpiece) = theGC;
      EZ_FigPieceXFont(fpiece) = xft;
      EZ_FigPieceLineWidth(fpiece) = line_width;
      
      switch(type)
	{
	case EZ_FIG_POINTS:
	case EZ_FIG_POLYLINE:
	case EZ_FIG_POLYGON:
	  if(npts > 0)
	    {
	      XPoint *mpts = (XPoint *)my_malloc((npts) * sizeof(XPoint),_FIG_ITEM_DATA_);
	      if(mode == CoordModePrevious)
		memcpy(mpts, xpts, npts * sizeof(XPoint));
	      else
		{
		  int i;
		  for(i = npts-1; i > 0; i--)
		    {
		      mpts[i].x = (xpts[i].x - xpts[i-1].x);
		      mpts[i].y = (xpts[i].y - xpts[i-1].y);
		    }
		  mpts[0].x = xpts[0].x;
		  mpts[0].y = xpts[0].y;
		}
	      EZ_FigPieceNPoints(fpiece) = npts;
	      EZ_FigPieceData(fpiece) = mpts;
	    }
	  break;
	case EZ_FIG_ARC:
	case EZ_FIG_FILLED_ARC:
	  {
	    EZ_FigPieceAngle2(fpiece) = npts;
	    EZ_FigPieceAngle1(fpiece) = other;
	  }
	break;
	case EZ_FIG_RECTANGLE:
	case EZ_FIG_FILLED_RECTANGLE:
	  /* nothing need to be set here */
	  break;
	case EZ_FIG_RECTANGLES:
	case EZ_FIG_FILLED_RECTANGLES:
	  {
	    XRectangle *rects = (XRectangle *)my_malloc((npts) * sizeof(XRectangle),_FIG_ITEM_DATA_);
	    memcpy(rects, xpts, (npts) * sizeof(XRectangle));
	    EZ_FigPieceNPoints(fpiece) = npts;
	    EZ_FigPieceData(fpiece) = (XPoint *)rects;	    	    
	  }
	break;
	case EZ_FIG_ARCS:
	case EZ_FIG_FILLED_ARCS:
	  {
	    XArc *arcs = (XArc *)my_malloc((npts) * sizeof(XArc),_FIG_ITEM_DATA_);
	    memcpy(arcs, xpts, (npts) * sizeof(XArc));
	    EZ_FigPieceNPoints(fpiece) = npts;
	    EZ_FigPieceData(fpiece) = (XPoint *)arcs;	    	    
	  }
	break;
	case EZ_FIG_SEGMENTS:
	  {
	    XSegment *segs =  (XSegment *) my_malloc((npts) * sizeof(XSegment),_FIG_ITEM_DATA_);
	    memcpy(segs, xpts, (npts) * sizeof(XSegment));
	    EZ_FigPieceNPoints(fpiece) = npts;
	    EZ_FigPieceData(fpiece) = (XPoint *)segs;	    	    
	  }
	break;
	case EZ_FIG_PIXMAP:
	  {
	    EZ_Bitmap *bitmap = (EZ_Bitmap *)xpts;
	    if(bitmap) 
	      {
		int bw, bh, ox, oy, ow, oh;
		EZ_BitmapRefCount(bitmap) += 1;
		bw = EZ_BitmapWidth(bitmap);
		bh = EZ_BitmapHeight(bitmap);
		ox = other;
		oy = npts;
		if(ox < 0) ox = 0;
		if(oy < 0) oy = 0;
		if(ox >= bw || oy >= bh || w <= 0 || h <= 0) {ow = 0; oh = 0; }
		else
		  {
		    ow = w; oh = h;
		    if(ox + ow > bw) ow = bw - ox;
		    if(oy + oh > bh) oh = bh - oy;
		  }
		EZ_FigPiecePixmapX(fpiece) = ox;
		EZ_FigPiecePixmapY(fpiece) = oy;
		EZ_FigPiecePixmapW(fpiece) = ow;
		EZ_FigPiecePixmapH(fpiece) = oh;
	      }
	    EZ_FigPieceData(fpiece) = (XPoint *)bitmap;	    
	  }
	break;
	case EZ_FIG_STRING:
	  {
	    int slen = npts;
	    char *str = (char *)my_malloc((slen+1) * sizeof(char),_FIG_ITEM_DATA_);
	    strcpy(str, (char *)xpts);
	    EZ_FigPieceStrLength(fpiece) = npts;
	    EZ_FigPieceData(fpiece) = (XPoint *)str;	    
	    EZ_FigPieceStrJustification(fpiece) = other;
	  }
	break;
	default:
	  break;
	}
      if(scale) EZ_ScaleFigPiece(fpiece, xsf, ysf);
      /* insert at the end to keep the rendering order */
      {
	EZ_Widget *parent =  EZ_ItemParentWidget(item);
	EZ_FigPiece **ptr = &(EZ_FigItemPartList(item));
	while(*ptr) ptr = &((*ptr)->next);
	*ptr = fpiece;
	EZ_ItemDirty(item) = 1;

        if(EZ_FigItemFreezed(item)) return(fpiece);
	/* if item is current viewable, update immediately */
	if( parent && EZ_ItemVisible(item))
	  {
	    int jw, jh;
    
	    if(1) /* hack: redisplay only this item */
	      {
		unsigned long bgpv;
		Window win = EZ_WidgetWindow(parent);
		int x0 = EZ_ItemOriginX(item);
		int y0 = EZ_ItemOriginY(item);
		int aw = EZ_ItemAWidth(item);
		int ah = EZ_ItemAHeight(item);
		EZ_FillItemBackground(parent, item, win, x0, y0, aw, ah, x0, y0, &bgpv);
		if(EZ_WidgetType(parent) == EZ_WIDGET_IWORK_AREA) 
		  {
		    int i, nitems = EZ_IWorkAreaNItems(parent); 
		    EZ_Item **data = EZ_IWorkAreaData(parent);    
		    int bw = EZ_WidgetBorderWidth(parent) + EZ_WidgetPadB(parent);
		    int Rx = bw;
		    int Ry = bw;
		    int Rw = EZ_WidgetWidth(parent) - (Rx+Rx);
		    int Rh = EZ_WidgetHeight(parent) -(Rx+Rx);
		    for(i = 0; i < nitems; i++)
		      {
			EZ_Item *sitem = data[i];
			if(sitem && sitem != item)
			  {
			    int xx0 = EZ_ItemOriginX(sitem);
			    int yy0 = EZ_ItemOriginY(sitem);
			    int w0 = EZ_ItemAWidth(sitem);
			    int h0 = EZ_ItemAHeight(sitem);
			    if( xx0 >= x0 + aw || yy0 >= y0 + ah || xx0 + w0 <= x0  || yy0 + h0 < y0) ;
			    else { EZ_DisplayOneItem(parent, win, xx0, yy0, sitem, Rx, Ry, Rw,Rh); }
			  }
		      }
		  }
		EZ_ComputeItemSize(item, &jw, &jh);
		EZ_ItemConfigureSentienl(item,0);
		XSync(EZ_Display, False);
	      }
	    else EZ_UpdateDisplayItem(item); /* force a complete redisplay */
	  }
      }
    }
  return(fpiece);
}
/**********************************************************************************************/
static void fixFigItemPiece(item, upd) EZ_Item *item; int upd;
{
  EZ_Widget *parent;

  if(EZ_FigItemFreezed(item)) return;
  /* if item is current viewable, update immediately */
  parent =  EZ_ItemParentWidget(item);
  if( parent && EZ_ItemVisible(item))
    {
      if(upd <= 0) /* hack: redisplay only this item */
	{
	  unsigned long bgpv;
	  Window win = EZ_WidgetWindow(parent);
	  int x0 = EZ_ItemOriginX(item);
	  int y0 = EZ_ItemOriginY(item);
	  int aw = EZ_ItemAWidth(item);
	  int ah = EZ_ItemAHeight(item);
	  EZ_FillItemBackground(parent, item, win, x0, y0, aw, ah, x0, y0, &bgpv);
	  if(EZ_WidgetType(parent) == EZ_WIDGET_IWORK_AREA) 
	    {
	      int i, nitems = EZ_IWorkAreaNItems(parent); 
	      EZ_Item **data = EZ_IWorkAreaData(parent);    
	      int bw = EZ_WidgetBorderWidth(parent) + EZ_WidgetPadB(parent);
	      int Rx = bw;
	      int Ry = bw;
	      int Rw = EZ_WidgetWidth(parent) - (Rx+Rx);
	      int Rh = EZ_WidgetHeight(parent) -(Rx+Rx);
	      for(i = 0; i < nitems; i++)
		{
		  EZ_Item *sitem = data[i];
		  if(sitem && sitem != item)
		    {
		      int xx0 = EZ_ItemOriginX(sitem);
		      int yy0 = EZ_ItemOriginY(sitem);
		      int w0 = EZ_ItemAWidth(sitem);
		      int h0 = EZ_ItemAHeight(sitem);
		      if( xx0 >= x0 + aw || yy0 >= y0 + ah || xx0 + w0 <= x0  || yy0 + h0 < y0) ;
		      else { EZ_DisplayOneItem(parent, win, xx0, yy0, sitem, Rx, Ry, Rw,Rh); }
		    }
		}
	      if(upd == 0)
		{
		  int jw, jh; 
		  EZ_ComputeItemSize(item, &jw, &jh);
		  EZ_DisplayOneItem(parent, win, x0, y0, item, Rx, Ry, Rw,Rh); 
		}
	      XSync(EZ_Display, False);
	    }
	}
      else if(upd > 0)
	{
	  EZ_UpdateDisplayItem(item); /* force a complete redisplay */
	  XSync(EZ_Display, False);
	}
    }
}
/*****************************************************************************************/
void EZ_FigItemDeletePiece(item, fpiece, upd) EZ_Item *item; EZ_FigPiece *fpiece; int upd;
{
  if(fpiece && item && EZ_ItemType(item) == EZ_FIG_ITEM)
    {
      EZ_FigPiece *ptr = EZ_FigItemPartList(item);
      int deleted = 0;
      if(ptr == fpiece) { EZ_FigItemPartList(item) = fpiece->next; deleted = 1;}
      else
	{
	  while(ptr && deleted == 0)
	    {
	      if(ptr->next == fpiece)
		{
		  ptr->next = fpiece->next;
		  deleted = 1;
		}
	      else
		ptr = ptr->next;
	    }
	}
      if(deleted)
	{
	  DestroyFigItemPiece(fpiece);
	  if(upd) fixFigItemPiece(item, 1);
	}
    }
}

void EZ_FigItemDeleteAllPieces(item, upd) EZ_Item *item; int upd;
{
  if(item && EZ_ItemType(item) == EZ_FIG_ITEM)
    {
      EZ_FigPiece *tmp, *ptr = EZ_FigItemPartList(item);
      EZ_FigItemPartList(item) = NULL;

      while(ptr)
        {
          tmp = ptr->next;
	  DestroyFigItemPiece(ptr);
          ptr = tmp;
        }
      EZ_FigItemXOffset(item) = 0;
      EZ_FigItemYOffset(item) = 0;
      fixFigItemPiece(item, upd);
    }
}
/**********************************************************************************************/
void EZ_FigItemHidePiece(item, fpiece, upd)
     EZ_Item *item; EZ_FigPiece *fpiece; int upd;
{
  if(item && fpiece && EZ_ItemType(item) == EZ_FIG_ITEM)
    {
      EZ_FigPiece *ptr = EZ_FigItemPartList(item);
      int find = 0;
      while(ptr)
	{
	  if(ptr == fpiece)
	    { find = 1; break;}
	  ptr = ptr->next;
	}
      if(find)
	{
	  if(EZ_FigPieceHidden(fpiece) == 0)
	    {
	      EZ_FigPieceHidden(fpiece) = 1;
	      fixFigItemPiece(item, upd);
	    }
	}
    }
}
void EZ_FigItemShowPiece(item, fpiece, upd)
     EZ_Item *item; EZ_FigPiece *fpiece; int upd;
{
  if(item && fpiece && EZ_ItemType(item) == EZ_FIG_ITEM)
    {
      EZ_FigPiece *ptr = EZ_FigItemPartList(item);
      int find = 0;
      while(ptr)
	{
	  if(ptr == fpiece)
	    { find = 1; break;}
	  ptr = ptr->next;
	}
      if(find)
	{
	  if(EZ_FigPieceHidden(fpiece))
	    {
	      EZ_FigPieceHidden(fpiece) = 0;
	      fixFigItemPiece(item, upd);
	    }
	}
    }
}
/**********************************************************************************************
 *
 * fmode:  0  normal draw
 *         1  highlighting 
 */
static void EZ_DrawFigItemWork(widget, drawable, x0, y0, item, Rx, Ry, Rw,Rh, fmode, umode, fg, bg, hmode)
     EZ_Widget *widget;
     Drawable  drawable;
     EZ_Item   *item;
     int       x0, y0;
     int       Rx, Ry, Rw,Rh; /* confined region in drawable */
     int       fmode, hmode, umode;
     unsigned long fg,bg;
{
  if(item)
    {
      int xoffset = EZ_FigItemXOffset(item);
      int yoffset = EZ_FigItemYOffset(item);
      int xoff,yoff, bdX, bdY, bdwidth;
      int aw = EZ_ItemAWidth(item);
      int ah = EZ_ItemAHeight(item);      
      int dx,dy;
      GC  gc = EZ_FIGUPDATEGC;
      GC  lastGc = None;

      /* remember the current origin, and which widget it belongs to */
      EZ_ItemParentWidget(item) = widget;
      if(umode)
	{
	  EZ_ItemOriginX(item) = x0;
	  EZ_ItemOriginY(item) = y0;

	  if(x0 > Rw + Rx || x0 + aw < Rx || y0 + ah < Ry || y0 > Rh +Ry)
	    {
	      EZ_ItemVisible(item) = 0;	  
	      return;
	    }
	  EZ_ItemVisible(item) = 1;	  
	}
      if(fmode) XSetBackground(EZ_Display, EZ_FIGUPDATEGC, bg);

      EZ_GetItemXYOffset(item, &xoff, &yoff);
      EZ_GetItemBwAndXYPading(item, &bdwidth,&bdX, &bdY);
      dx = x0 + xoff + bdX - xoffset;
      dy = y0 + yoff + bdY - yoffset;
      
      if(EZ_ItemBackground(item) != 0)
        {
	  unsigned long bgpv = EZ_ItemBackground(item);      
	  XSetForeground(EZ_Display, EZ_WRITABLEGC, bgpv);
	  XFillRectangle(EZ_Display, drawable, EZ_WRITABLEGC, x0, y0, aw,ah);
        }
      /* render item pieces */
      {
	EZ_FigPiece *act = EZ_FigItemPartList(item);
	while(act)
	  {
	    if(EZ_FigPieceHidden(act) == 0)
	      {
		int type = act->figType;
		gc = EZ_FigPieceGC(act);
                if(fmode)
                  {
                    if(lastGc != gc)
                      {
                        lastGc = gc;
                        XCopyGC(EZ_Display, lastGc,~(GCBackground|GCFillStyle|GCStipple),EZ_FIGUPDATEGC);
                      }
                    gc =  EZ_FIGUPDATEGC;
                  }
		switch(type)
		  {
		  case EZ_FIG_POINTS:
		    {
		      int npts = EZ_FigPieceNPoints(act);
		      XPoint *xpts =  EZ_FigPieceData(act);
		      if(npts > 0 && xpts)
			{
			  xpts[0].x += dx;  xpts[0].y += dy; 
			  XDrawPoints(EZ_Display, drawable, gc, 
				      xpts, npts, CoordModePrevious);
			  xpts[0].x -= dx;  xpts[0].y -= dy; 
			}
		    }
		  break;
		  case EZ_FIG_POLYLINE:
		    {
		      int npts = EZ_FigPieceNPoints(act);
		      XPoint *xpts =  EZ_FigPieceData(act);
		      if(npts > 0 && xpts)
			{
			  xpts[0].x += dx;  xpts[0].y += dy; 
			  XDrawLines(EZ_Display, drawable, gc, 
				     xpts, npts, CoordModePrevious);
			  xpts[0].x -= dx;  xpts[0].y -= dy; 
			}
		    }
		  break;
		  case EZ_FIG_POLYGON:
		    {
		      int npts = EZ_FigPieceNPoints(act);
		      XPoint *xpts =  EZ_FigPieceData(act);
		      if(npts > 0 && xpts)
			{
			  xpts[0].x += dx;  xpts[0].y += dy; 
			  XFillPolygon(EZ_Display, drawable, gc,
				       xpts, npts, Complex, CoordModePrevious);
			  xpts[0].x -= dx;  xpts[0].y -= dy; 
			}
		    }
		  break;
		  case EZ_FIG_RECTANGLE:
		    {
		      int xx0 =  EZ_FigPieceX(act)+dx;
		      int yy0 =  EZ_FigPieceY(act)+dy;
		      XDrawRectangle(EZ_Display, drawable, gc,
				     xx0,yy0,
				     EZ_FigPieceWidth(act), EZ_FigPieceHeight(act));
		    }
		  break;
		  case EZ_FIG_RECTANGLES:
		    {
		      int i, nrects = EZ_FigPieceNPoints(act);
		      XRectangle *rects = (XRectangle *)EZ_FigPieceData(act);
		      for(i = 0; i < nrects; i++)
			{
			  rects[i].x += dx;
			  rects[i].y += dy;
			}
		      XDrawRectangles(EZ_Display, drawable, gc, rects, nrects);
		      for(i = 0; i < nrects; i++)
			{
			  rects[i].x -= dx;
			  rects[i].y -= dy;
			}
		    }
		  break;
		  case EZ_FIG_SEGMENTS:
		    {
		      int i, nsegs = EZ_FigPieceNPoints(act);
		      XSegment *segs = (XSegment *)EZ_FigPieceData(act);
		      for(i = 0; i < nsegs; i++)
			{
			  segs[i].x1 += dx;
			  segs[i].y1 += dy;
			  segs[i].x2 += dx;
			  segs[i].y2 += dy;
			}
		      XDrawSegments(EZ_Display, drawable, gc, segs, nsegs);
		      for(i = 0; i < nsegs; i++)
			{
			  segs[i].x1 -= dx;
			  segs[i].y1 -= dy;
			  segs[i].x2 -= dx;
			  segs[i].y2 -= dy;
			}
		    }
		  break;
		  case EZ_FIG_FILLED_RECTANGLE:
		    {
		      int xx0 =  EZ_FigPieceX(act)+dx;
		      int yy0 =  EZ_FigPieceY(act)+dy;
		      XFillRectangle(EZ_Display, drawable, gc,
				     xx0,yy0,
				     EZ_FigPieceWidth(act), EZ_FigPieceHeight(act));
		    }
		  break;
		  case EZ_FIG_FILLED_RECTANGLES:
		    {
		      int i, nrects = EZ_FigPieceNPoints(act);
		      XRectangle *rects = (XRectangle *)EZ_FigPieceData(act);
		      for(i = 0; i < nrects; i++)
			{
			  rects[i].x += dx;
			  rects[i].y += dy;
			}
		      XFillRectangles(EZ_Display, drawable, gc, rects, nrects);
		      for(i = 0; i < nrects; i++)
			{
			  rects[i].x -= dx;
			  rects[i].y -= dy;
			}
		    }
		  break;
		  case EZ_FIG_ARC:
		    {
		      int xx0 =  EZ_FigPieceX(act)+dx;
		      int yy0 =  EZ_FigPieceY(act)+dy;
		      XDrawArc(EZ_Display, drawable, gc, xx0, yy0,
			       EZ_FigPieceWidth(act), EZ_FigPieceHeight(act),
			       EZ_FigPieceAngle1(act), EZ_FigPieceAngle2(act));
		    }
		  break;
		  case EZ_FIG_FILLED_ARC:
		    {
		      int xx0 =  EZ_FigPieceX(act)+dx;
		      int yy0 =  EZ_FigPieceY(act)+dy;
		      XFillArc(EZ_Display, drawable, gc, xx0, yy0,
			       EZ_FigPieceWidth(act), EZ_FigPieceHeight(act),
			       EZ_FigPieceAngle1(act), EZ_FigPieceAngle2(act));
		    }
		  break;
		  case EZ_FIG_ARCS:
		    {
		      int i, narcs = EZ_FigPieceNPoints(act);
		      XArc *arcs = (XArc *)EZ_FigPieceData(act);
		      for(i = 0; i < narcs; i++)
			{
			  arcs[i].x += dx; arcs[i].y += dy;
			}
		      XDrawArcs(EZ_Display, drawable, gc, arcs, narcs);
		      for(i = 0; i < narcs; i++)
			{
			  arcs[i].x -= dx; arcs[i].y -= dy;
			}			    
		    }
		  break;
		  case EZ_FIG_FILLED_ARCS:
		    {
		      int i, narcs = EZ_FigPieceNPoints(act);
		      XArc *arcs = (XArc *)EZ_FigPieceData(act);
		      for(i = 0; i < narcs; i++)
			{
			  arcs[i].x += dx; arcs[i].y += dy;
			}
		      XFillArcs(EZ_Display, drawable, gc, arcs, narcs);
		      for(i = 0; i < narcs; i++)
			{
			  arcs[i].x -= dx; arcs[i].y -= dy;
			}			    
		    }
		  break;
		  case EZ_FIG_PIXMAP:
		    {
		      EZ_Bitmap *bitmap = (EZ_Bitmap *)EZ_FigPieceData(act);
		      if(bitmap && EZ_FigPiecePixmapW(act) > 0) 
			{
			  int xx0 =  EZ_FigPieceX(act)+dx;
			  int yy0 =  EZ_FigPieceY(act)+dy;
			  int px0 =  EZ_FigPiecePixmapX(act);
			  int py0 =  EZ_FigPiecePixmapY(act);
			  int pw0 =  EZ_FigPiecePixmapW(act);
			  int ph0 =  EZ_FigPiecePixmapH(act);
                          int width = bitmap->width;
                          int height = bitmap->height;
			  if(EZ_BitmapType(bitmap) == EZ_BITMAP_IS_BITMAP)
			    {
			      if(bitmap->htpixmap == None)
				bitmap->htpixmap = XCreatePixmap(EZ_Display, drawable, width, height, EZ_Depth);    
                              if(fmode == 0)
                                {
                                  XCopyPlane(EZ_DisplayForWidgets,
                                             bitmap->pixmap,
                                             bitmap->htpixmap,
                                             gc,
                                             0,0, width, height,
                                             0,0,
                                             1);
                                }
                              else
                                {
                                  GC tgc = EZ_WRITABLEGC;
                                  XSetForeground(EZ_Display, tgc, bg);
                                  XCopyPlane(EZ_DisplayForWidgets,
                                             bitmap->pixmap,
                                             bitmap->htpixmap,
                                             tgc,
                                             0,0, width, height,
                                             0,0,
                                             1);
                                }
                              XSetClipMask(EZ_Display, gc, bitmap->pixmap);
                              XSetClipOrigin(EZ_Display, gc,xx0-px0,yy0-py0);
                              XCopyArea(EZ_DisplayForWidgets,
                                        bitmap->htpixmap,
                                        drawable,
                                        gc,
                                        px0,py0,MINV(pw0, width), MINV(ph0, height),
                                        xx0,yy0);
                              XSetClipMask(EZ_Display, gc, None);
			    }
			  else 
			    {
			      if(bitmap->shape != (Pixmap)NULL)
				{
				  XSetClipMask(EZ_Display, gc, bitmap->shape);
				  XSetClipOrigin(EZ_Display, gc,xx0,yy0);
				}
			      XCopyArea(EZ_DisplayForWidgets,
					bitmap->pixmap,
					drawable,
					gc,
                                        px0,py0,MINV(pw0, width), MINV(ph0, height),
					xx0,yy0);
			      if(bitmap->shape != (Pixmap)NULL) XSetClipMask(EZ_Display, gc, None);
			    }		      
			}
		    }
		  break;
		  case EZ_FIG_STRING:
		    {
		      int xx0 =  EZ_FigPieceX(act)+dx;
		      int yy0 =  EZ_FigPieceY(act)+dy;
		      XFontStruct *fs = EZ_FigPieceXFont(act);
                      EZ_RenderLabelText(drawable, gc, xx0, yy0+fs->ascent, 
					 EZ_FigPieceStrHeight(act), (char *)EZ_FigPieceData(act),
					 EZ_FigPieceStrLength(act), 1024,
					 EZ_FigPieceStrNLines(act), EZ_FigPieceStrWidth(act),
					 EZ_FigPieceStrJustification(act), fs, 0);
		    }
		  break;
		  }
	      }
	    act = act->next;
      	  }
      }
    }
}
/*********************************************************************************************
 *
 * return 1 if (x,y) is "almost on" (x1,y1)--(x2,y2)
 */
static int PointLineDist(x,y,x1,y1,x2,y2, halfw)
     int x, y,  x1,  y1, x2,  y2, halfw;
{
  if(x1 == x2)
    {
      int dx = x - x1;
      dx = ABSV(dx);
      if(dx < halfw)
	{
	  if(y1 <= y2)
	    {
	      if( y >= y1 && y <= y2) return(1);
	    }
	  else
	    {
	      if( y >= y2 && y <= y1) return(1);	      
	    }
	}
      return(0);
    }
  else if(y1 == y2)
    {
      int dy = y - y1;
      dy = ABSV(dy);
      if(dy < halfw)
	{
	  if(x1 <= x2)
	    {
	      if( x >= x1 && x <= x2) return(1);
	    }
	  else
	    {
	      if( x >= x2 && x <= x1) return(1);	      
	    }
	}
      return(0);
    }
  else
    {
      double dx12 = (double)(x2 - x1);
      double dy12 = (double)(y2 - y1);
      double dx1 = (double)(x-x1);
      double dy1 = (double)(y-y1);
      double dx2 = (double)(x-x2);
      double dy2 = (double)(y-y2);
      if( dx12*dx1+dy12*dy1 >= 0.0 && dx12*dx2+dy12*dy2 <= 0.0)
	{
	  double d1 = (dx12*dy1 - dy12*dx1);
	  double d2 = d1 * d1 /(dx12*dx12 + dy12 *dy12);
	  double d3 = (double)halfw;
	  if(d2 < d3 * d3) return(1);
	}
    }
  return(0);
}
/**********************************************************************************************/
static int IntersectPolygonEdge(x,y,x1,y1,x2,y2)
     int x, y, x1, y1, x2, y2;
{
  if(x1 == x2)      /* vertical edge */
    return(0);
  else if(y1 == y2) /* horizontal edge */
    {
      if(y < y1 && (x - x1) *(x - x2) < 0) return(1);
    }
  else
    {
      int dx1 = x - x1;
      int dx2 = x - x2;
      if( dx1 * dx2 < 0)
	{
	  double ddy = (double) (y1 - y2);
	  double ddx = (double) (x1 - x2);
	  double dxx = (double)dx1;
	  double aa = (double)y1 + ddy/ddx * dxx;
	  int iy = (int) aa;
	  if(iy < y) return(1);
	}
      else if(dx1 == 0) return( y1 < y);
      else if(dx2 == 0) return( y2 < y);
    }
  return(0);
}
/**********************************************************************************************/
static int xyIsOnOrInArc(x,y,x0,y0,w,h, a1, a2,mode) int x,y,x0,y0,w,h,a1,a2; int mode;
{
  double A = (double) w;
  double B = (double) h;
  double xx0 = (double)x0;
  double yy0 = (double)y0;
  double xx = (double)x;
  double yy = (double)y;
  double test, lbd = mode == 0? 0.9: 0.0;
  A *= 0.5; B *= 0.5;
  xx0 += A;
  yy0 += B;
		
  xx -= xx0;
  yy -= yy0;
  test = (xx)*(xx)/(A*A) + (yy)*(yy)/(B*B);

  if( test >= lbd && test <= 1.1)
    {
      int  angle1 = a1;
      int  angle2 = a2;
      int  angle;
      double dangle;
      
      /* normalize angles so that both angles are positive */
      angle1 = angle1 % (360*64);
      if(angle1 < 0) angle1 += (360*64);
      angle2 %= (360*64);
      angle2 = angle1 + angle2;
      if(angle2 < 0)
	{
	  angle1 += (360*64);
	  angle2 += (360*64);
	}
      
      if(xx == 0.0)    dangle = (yy > 0.0? PI*0.5 : -PI*0.5);
      else if(yy == 0.0)   dangle = (xx > 0.0 ? 0.0: PI);
      else 
	{
	  dangle = atan2(-yy,xx); 
	  if(dangle < 0.0) dangle += PI + PI;
	}
      angle = (int)(dangle * (180.0 * 64.0/PI));
      angle %= (360*64);

      if(angle1 < angle2)
	{if(angle >= angle1 && angle <= angle2) return(1);}
      else
	{ if(angle >= angle2 && angle <= angle1) return(1);}
    }
  return(0);
}
/**********************************************************************************************/

EZ_FigPiece  *EZ_FigItemAddString(fitem, theGC, str, len, justification, x, y)
     EZ_Item *fitem; char *str; int len;
     int justification, x, y;
     GC theGC;
{
  return EZ_FigItemAddPiece(fitem, EZ_FIG_STRING, theGC, x, y, 0, 0,justification, len, (XPoint *)str, 0);
}

EZ_FigPiece  *EZ_FigItemAddPixmap(fitem, theGC, pixmap, sx, sy,sw,sh,dx,dy)
     EZ_Item *fitem;
     GC theGC;
     EZ_Bitmap *pixmap;
     int sx,sy,sw,sh,dx,dy;
{
  return EZ_FigItemAddPiece(fitem, EZ_FIG_PIXMAP, theGC, dx, dy, sw, sh, sx, sy, (XPoint *)pixmap, 0);
}

EZ_FigPiece  *EZ_FigItemAddPoints(fitem, theGC, pts, npts)
     EZ_Item *fitem;   GC theGC;
     XPoint *pts; int npts;
{
  return EZ_FigItemAddPiece(fitem, EZ_FIG_POINTS, theGC, 0, 0, 0, 0, 0, npts, pts, 0);
}

EZ_FigPiece  *EZ_FigItemAddLines(fitem, theGC, pts, npts, mode)
     EZ_Item *fitem;   GC theGC; int mode;
     XPoint *pts; int npts;
{
  return EZ_FigItemAddPiece(fitem, EZ_FIG_POLYLINE, theGC, 0, 0, 0, 0, 0, npts, pts, mode);
}

EZ_FigPiece  *EZ_FigItemAddPolygon(fitem, theGC, pts, npts, mode)
     EZ_Item *fitem;   GC theGC; int mode;
     XPoint *pts; int npts;
{
  return EZ_FigItemAddPiece(fitem, EZ_FIG_POLYGON, theGC, 0, 0, 0, 0, 0, npts, pts, mode);
}

EZ_FigPiece  *EZ_FigItemAddRectangle(fitem, theGC, x,y,w,h)
     EZ_Item *fitem;   GC theGC;
     int x,y,w,h;
{
  return EZ_FigItemAddPiece(fitem, EZ_FIG_RECTANGLE, theGC, x, y, w, h, 0, 0,NULL,0);
}

EZ_FigPiece  *EZ_FigItemAddFilledRectangle(fitem, theGC, x,y,w,h)
     EZ_Item *fitem;   GC theGC;
     int x,y,w,h;
{
  return  EZ_FigItemAddPiece(fitem, EZ_FIG_FILLED_RECTANGLE, theGC, x, y, w, h, 0, 0,NULL,0);
}

EZ_FigPiece  *EZ_FigItemAddArc(fitem, theGC, x,y,w,h, ang1, ang2)
     EZ_Item *fitem;   GC theGC;
     int x,y,w,h, ang1, ang2;
{
  return EZ_FigItemAddPiece(fitem, EZ_FIG_ARC, theGC, x, y, w, h, ang1, ang2, NULL,0);
}

EZ_FigPiece  *EZ_FigItemAddFilledArc(fitem, theGC, x,y,w,h, ang1, ang2)
     EZ_Item *fitem;   GC theGC;
     int x,y,w,h, ang1, ang2;
{
  return EZ_FigItemAddPiece(fitem, EZ_FIG_FILLED_ARC, theGC, x, y, w, h, ang1, ang2, NULL,0);
}

EZ_FigPiece  *EZ_FigItemAddRectangles(fitem, theGC, rects, nrects)
     EZ_Item *fitem;   GC theGC;
     XRectangle *rects; int nrects;
{
  return  EZ_FigItemAddPiece(fitem, EZ_FIG_RECTANGLES, theGC, 0, 0, 0, 0, 0, nrects,(XPoint *)rects,0);
}

EZ_FigPiece  *EZ_FigItemAddFilledRectangles(fitem, theGC, rects, nrects)
     EZ_Item *fitem;   GC theGC;
     XRectangle *rects; int nrects;
{
    return EZ_FigItemAddPiece(fitem, EZ_FIG_FILLED_RECTANGLES, theGC, 0, 0, 0, 0, 0, nrects,(XPoint *)rects,0);
}

EZ_FigPiece  *EZ_FigItemAddArcs(fitem, theGC, arcs, narcs)
     EZ_Item *fitem;   GC theGC;
     XArc *arcs; int narcs;
{
    return EZ_FigItemAddPiece(fitem, EZ_FIG_ARCS, theGC, 0, 0, 0, 0, 0, narcs,(XPoint *)arcs,0);
}

EZ_FigPiece  *EZ_FigItemAddFilledArcs(fitem, theGC, arcs, narcs)
     EZ_Item *fitem;   GC theGC;
     XArc *arcs; int narcs;
{
    return EZ_FigItemAddPiece(fitem, EZ_FIG_FILLED_ARCS, theGC, 0, 0, 0, 0, 0, narcs,(XPoint *)arcs,0);
}

EZ_FigPiece  *EZ_FigItemAddSegments(fitem, theGC, segs, nsegs)
     EZ_Item *fitem;   GC theGC;
     XSegment *segs; int nsegs;
{
  return EZ_FigItemAddPiece(fitem, EZ_FIG_SEGMENTS, theGC, 0, 0, 0, 0, 0, nsegs,(XPoint *)segs,0);
}
/**********************************************************************************************/
void EZ_DuplicatePartList(item, np, op) EZ_FigPiece **np, *op; EZ_Item *item;
{
  EZ_FigPiece *fpiece;
  int npts;
  if(item && EZ_ItemType(item) == EZ_FIG_ITEM) {
  *np = NULL;
  while(op)
    {
      *np =fpiece= allocateNewFigPiece(EZ_FigPieceX(op), EZ_FigPieceY(op),
				       EZ_FigPieceWidth(op), EZ_FigPieceHeight(op),
				       EZ_FigPieceType(op));
      
      EZ_FigPieceGC(fpiece) = EZ_FigPieceGC(op);
      EZ_FigPieceXFont(fpiece) = EZ_FigPieceXFont(op);
      EZ_FigPieceLineWidth(fpiece) = EZ_FigPieceLineWidth(op);      
      EZ_IncrementGCRef(EZ_FigPieceGC(op), NULL, NULL);
      switch(EZ_FigPieceType(op))
	{
	case EZ_FIG_POINTS:
	case EZ_FIG_POLYLINE:
	case EZ_FIG_POLYGON:
	  { 
	    if( (npts = EZ_FigPieceNPoints(op)) > 0)
	      {
		XPoint *mpts = (XPoint *)my_malloc((npts) * sizeof(XPoint),_FIG_ITEM_DATA_);
		memcpy(mpts, EZ_FigPieceData(op), npts * sizeof(XPoint));
		EZ_FigPieceNPoints(fpiece) = npts;
		EZ_FigPieceData(fpiece) = mpts;
	      }
	  }
	  break;
	case EZ_FIG_ARC:
	case EZ_FIG_FILLED_ARC:
	  {
	    EZ_FigPieceAngle2(fpiece) = EZ_FigPieceAngle2(op);
	    EZ_FigPieceAngle1(fpiece) = EZ_FigPieceAngle1(op);
	  }
	break;
	case EZ_FIG_RECTANGLE:
	case EZ_FIG_FILLED_RECTANGLE:
	  /* nothing need to be set here */
	  break;
	case EZ_FIG_RECTANGLES:
	case EZ_FIG_FILLED_RECTANGLES:
	  if( (npts = EZ_FigPieceNPoints(op)) > 0)
	    {
	      XRectangle *rects = (XRectangle *)my_malloc((npts) * sizeof(XRectangle),_FIG_ITEM_DATA_);
	      memcpy(rects,EZ_FigPieceData(op), (npts) * sizeof(XRectangle));
	      EZ_FigPieceNPoints(fpiece) = npts;
	      EZ_FigPieceData(fpiece) = (XPoint *)rects;	    	    
	    }
	  break;
	case EZ_FIG_ARCS:
	case EZ_FIG_FILLED_ARCS:
	  if( (npts = EZ_FigPieceNPoints(op)) > 0)
	    {
	      XArc *arcs = (XArc *)my_malloc((npts) * sizeof(XArc),_FIG_ITEM_DATA_);
	      memcpy(arcs, EZ_FigPieceData(op), (npts) * sizeof(XArc));
	      EZ_FigPieceNPoints(fpiece) = npts;
	      EZ_FigPieceData(fpiece) = (XPoint *)arcs;	    	    
	    }
	  break;
	case EZ_FIG_SEGMENTS:
	  if( (npts = EZ_FigPieceNPoints(op)) > 0)
	    {
	      XSegment *segs =  (XSegment *) my_malloc((npts) * sizeof(XSegment),_FIG_ITEM_DATA_);
	      memcpy(segs, EZ_FigPieceData(op), (npts) * sizeof(XSegment));
	      EZ_FigPieceNPoints(fpiece) = npts;
	      EZ_FigPieceData(fpiece) = (XPoint *)segs;	    	    
	    }
	  break;
	case EZ_FIG_PIXMAP:
	  {
	    EZ_Bitmap *bitmap = (EZ_Bitmap *)EZ_FigPieceData(op);
	    if(bitmap) 
	      {
		EZ_BitmapRefCount(bitmap) += 1;
		EZ_FigPiecePixmapX(fpiece) = EZ_FigPiecePixmapX(op);
		EZ_FigPiecePixmapY(fpiece) = EZ_FigPiecePixmapY(op);
		EZ_FigPiecePixmapW(fpiece) = EZ_FigPiecePixmapW(op);
		EZ_FigPiecePixmapH(fpiece) = EZ_FigPiecePixmapH(op);
	      }
	    EZ_FigPieceData(fpiece) = (XPoint *)bitmap;	    
	  }
	break;
	case EZ_FIG_STRING:
	  {
	    int npts =EZ_FigPieceStrLength(op);
	    char *str = (char *)my_malloc((npts+1) * sizeof(char),_FIG_ITEM_DATA_);
	    if(EZ_FigPieceData(op)) strcpy(str, (char *)EZ_FigPieceData(op));
	    EZ_FigPieceStrLength(fpiece) = npts;
	    EZ_FigPieceData(fpiece) = (XPoint *)str;	    
	    EZ_FigPieceStrJustification(fpiece) = EZ_FigPieceStrJustification(op);
	  }
	break;
	default:
	  break;
	}
      np = &((*np)->next);
      *np = NULL;
      op = op->next;
    }
  }
}
/**********************************************************************************************/
void  EZ_ScaleFigPiece(fpiece, sx, sy)
     EZ_FigPiece *fpiece; float sx, sy; 
{
  if(fpiece)
    {
      int x0, y0, i, npts, type = EZ_FigPieceType(fpiece);
      float fx0, fy0;
      
      EZ_FigPieceClean(fpiece) = 0;

      /* scale the rectangle first */
        {
          x0 = EZ_FigPieceX(fpiece);
          y0 = EZ_FigPieceY(fpiece);
          fx0 = (float)(x0) * sx;
          fy0 = (float)(y0) * sy;
          EZ_FigPieceX(fpiece) = (int)fx0;
          EZ_FigPieceY(fpiece) = (int)fy0;
          x0 = EZ_FigPieceWidth(fpiece);
          y0 = EZ_FigPieceHeight(fpiece);
          fx0 = (float)(x0) * sx;
          fy0 = (float)(y0) * sy;
	  EZ_FigPieceWidth(fpiece) = (int)fx0;
	  EZ_FigPieceHeight(fpiece) = (int)fy0;
	  if(sx < 0.0)
	    {
	      EZ_FigPieceWidth(fpiece) = - EZ_FigPieceWidth(fpiece);
	      EZ_FigPieceX(fpiece) -= EZ_FigPieceWidth(fpiece);
	    }
	  if(sy < 0.0)
	    {
	      EZ_FigPieceHeight(fpiece) = - EZ_FigPieceHeight(fpiece);
	      EZ_FigPieceY(fpiece) -= EZ_FigPieceHeight(fpiece);
	    }
	}
      switch(type)
	{
        case EZ_FIG_PIXMAP:
        case EZ_FIG_STRING:
          break;
	case EZ_FIG_POINTS:
	case EZ_FIG_POLYLINE:
	case EZ_FIG_POLYGON:
	  if((npts = EZ_FigPieceNPoints(fpiece)) > 0)
	    {
	      XPoint *mpts = EZ_FigPieceData(fpiece);
	      for(i = 0; i < npts; i++)
		{
		  x0 = mpts[i].x;
		  y0 = mpts[i].y;	
		  fx0 = (float)(x0) * sx;
		  fy0 = (float)(y0) * sy;
		  mpts[i].x = (int)fx0;
		  mpts[i].y = (int)fy0;	  
		}
	    }
	  break;
	case EZ_FIG_ARC:
	case EZ_FIG_FILLED_ARC:
	  if(sx < 0.0)
	    {
	      if(sy > 0.0) 
		{
		  EZ_FigPieceAngle2(fpiece) = -EZ_FigPieceAngle2(fpiece);
		  EZ_FigPieceAngle1(fpiece) = 180*64 - EZ_FigPieceAngle1(fpiece);
		}
	      else
		{
		  EZ_FigPieceAngle1(fpiece) = EZ_FigPieceAngle1(fpiece) + 180*64;
		}
	    }
	  else if(sy < 0.0)
	    {
	      EZ_FigPieceAngle2(fpiece) = -EZ_FigPieceAngle2(fpiece);
	      EZ_FigPieceAngle1(fpiece) = - EZ_FigPieceAngle1(fpiece);
	    }
	  break;
	case EZ_FIG_RECTANGLE:
	case EZ_FIG_FILLED_RECTANGLE:
	  break;
	case EZ_FIG_RECTANGLES:
	case EZ_FIG_FILLED_RECTANGLES:
	  if((npts = EZ_FigPieceNPoints(fpiece)) > 0)
	    {
	      XRectangle *rects = (XRectangle *)EZ_FigPieceData(fpiece);
	      for(i = 0; i < npts; i++)
		{
		  x0 = rects[i].x;
		  y0 = rects[i].y;
		  fx0 = (float)(x0) * sx;
		  fy0 = (float)(y0) * sy;
		  rects[i].x = (int)fx0;
		  rects[i].y = (int)fy0;
		  x0 = rects[i].width;
		  y0 = rects[i].height;
		  fx0 = (float)(x0) * sx;
		  fy0 = (float)(y0) * sy;
		  rects[i].width = (int)fx0;
		  rects[i].height = (int)fy0;
		  if(sx < 0.0)
		    {
		      rects[i].width = -rects[i].width;
		      rects[i].x -= rects[i].width;
		    }
		  if(sy < 0.0)
		    {
		      rects[i].height = -rects[i].height;
		      rects[i].y -= rects[i].height;
		    }
		}
	    }
	  break;
	case EZ_FIG_ARCS:
	case EZ_FIG_FILLED_ARCS:
	  if((npts = EZ_FigPieceNPoints(fpiece)) > 0)
	    {
	      XArc *rects = (XArc *)EZ_FigPieceData(fpiece);
	      for(i = 0; i < npts; i++)
		{
		  x0 = rects[i].x;
		  y0 =  rects[i].y;
		  fx0 = (float)(x0) * sx;
		  fy0 = (float)(y0) * sy;
		  rects[i].x = (int)fx0;
		  rects[i].y = (int)fy0;
		  x0 = rects[i].width;
		  y0 = rects[i].height;
		  fx0 = (float)(x0) * sx;
		  fy0 = (float)(y0) * sy;
		  rects[i].width = (int)fx0;
		  rects[i].height = (int)fy0;
		  if(sx < 0.0)
		    {
		      rects[i].width = -rects[i].width;
		      rects[i].x -= rects[i].width;
		    }
		  if(sy < 0.0)
		    {
		      rects[i].height = -rects[i].height;
		      rects[i].y -= rects[i].height;
		    }
		  /* fix the angles */
		  if(sx < 0.0)
		    {
		      if(sy > 0.0) 
			{
			  rects[i].angle2 = -rects[i].angle2;
			  rects[i].angle1 = 180*64 -rects[i].angle1;
			}
		      else
			{
			  rects[i].angle1 = 180*64 +rects[i].angle1;
			}
		    }
		  else if(sy < 0.0)
		    {
		      rects[i].angle2 = -rects[i].angle2;
		      rects[i].angle1 = -rects[i].angle1;
		    }
		}
	    }
	break;
	case EZ_FIG_SEGMENTS:
	  if((npts = EZ_FigPieceNPoints(fpiece)) > 0)
	    {
	      XSegment *segs = (XSegment *)EZ_FigPieceData(fpiece);
	      for(i = 0; i < npts; i++)
		{
		  x0 = segs[i].x1; y0 = segs[i].y1;
		  fx0 = (float)(x0) * sx; fy0 = (float)(y0) * sy;
		  segs[i].x1 = (int)fx0; segs[i].y1 = (int)fy0;
		  x0 = segs[i].x2; y0 = segs[i].y2;
		  fx0 = (float)(x0) * sx; fy0 = (float)(y0) * sy;
		  segs[i].x2 = (int)fx0; segs[i].y2 = (int)fy0;
		}
	    }
	  break;
	default:
	  break;
	}
    }
}


void  EZ_ScaleFigItem(item, sx, sy, mode) EZ_Item *item; float sx, sy; int mode;
{
  if(item && EZ_ItemType(item) == EZ_FIG_ITEM) 
    {
      EZ_FigPiece *fpiece = EZ_FigItemPartList(item);
      EZ_FigItemXOffset(item) = 0;
      EZ_FigItemYOffset(item) = 0;
      EZ_ItemDirty(item) = 1;
      while(fpiece)
        {
          EZ_ScaleFigPiece(fpiece, sx, sy);
          fpiece = fpiece->next;
        } 
      EZ_FigItemSx(item) *= sx;
      EZ_FigItemSy(item) *= sy;
    }
}
/**********************************************************************************************
 *
 *  
 */
typedef struct GCVrec 
{
  XGCValues        *gcvalues;
  GC               gc;
  int              ref;
  struct GCVrec    *next;
} GCVRecord;

static GCVRecord *GCValuesList = NULL;

GC  EZ_GetGC(mask, gcv)
     unsigned long mask; XGCValues *gcv;
{
  GCVRecord *oldgcrec;
  XGCValues *oldgcv, gcvalues;

  if(mask & GCFunction) 
    gcvalues.function = gcv->function;
  else gcvalues.function = GXcopy;	    

  if(mask & GCPlaneMask) 
    gcvalues.plane_mask = gcv->plane_mask;
  else 	gcvalues.plane_mask = (unsigned) ~0;

  if(mask & GCForeground) 
    gcvalues.foreground = gcv->foreground;
  else 	gcvalues.foreground = 0;

  if(mask & GCBackground) 
    gcvalues.background = gcv->background;
  else gcvalues.background = 1;

  if(mask & GCLineWidth) 
    gcvalues.line_width = gcv->line_width;
  else gcvalues.line_width = 0;

  if(mask & GCLineStyle) 
    gcvalues.line_style = gcv->line_style;
  else 	gcvalues.line_style = LineSolid;

  if(mask & GCCapStyle) 
    gcvalues.cap_style = gcv->cap_style;
  else gcvalues.cap_style = CapButt;

  if(mask & GCJoinStyle) 
    gcvalues.join_style = gcv->join_style;
  else gcvalues.join_style = JoinMiter;

  if(mask & GCFillStyle) 
    gcvalues.fill_style = gcv->fill_style;
  else 	gcvalues.fill_style = FillSolid;

  if(mask & GCFillRule) 
    gcvalues.fill_rule = gcv->fill_rule;
  else 	gcvalues.fill_rule = EvenOddRule;

  if(mask & GCArcMode) 
    gcvalues.arc_mode = gcv->arc_mode;
  else gcvalues.arc_mode = ArcPieSlice;

  if(mask & GCTile) 
    gcvalues.tile = gcv->tile;
  else  gcvalues.tile = None;

  if(mask & GCStipple) 
    gcvalues.stipple = gcv->stipple;
  else 	gcvalues.stipple = None;

  if(mask & GCTileStipXOrigin) 
    gcvalues.ts_x_origin = gcv->ts_x_origin;
  else  gcvalues.ts_x_origin = 0;

  if(mask & GCTileStipYOrigin) 
    gcvalues.ts_y_origin = gcv->ts_y_origin;
  else 	gcvalues.ts_y_origin = 0;

  if(mask & GCFont) 
    gcvalues.font = gcv->font;
  else 	gcvalues.font = None;

  if(mask & GCSubwindowMode) 
    gcvalues.subwindow_mode = gcv->subwindow_mode;
  else 	gcvalues.subwindow_mode = ClipByChildren;

  if(mask & GCGraphicsExposures) 
    gcvalues.graphics_exposures = gcv->graphics_exposures;
  else gcvalues.graphics_exposures = True;

  if(mask & GCClipXOrigin) 
    gcvalues.clip_x_origin = gcv->clip_x_origin;
  else 	gcvalues.clip_x_origin = 0;

  if(mask & GCClipYOrigin) 
    gcvalues.clip_y_origin = gcv->clip_y_origin;
  else 	gcvalues.clip_y_origin = 0;

  if(mask & GCClipMask) 
    gcvalues.clip_mask = gcv->clip_mask;
  else 	gcvalues.clip_mask = None;

  if(mask & GCDashOffset) 
    gcvalues.dash_offset = gcv->dash_offset;
  else gcvalues.dash_offset = 0;

  if(mask & GCDashList) 
    gcvalues.dashes = gcv->dashes;
  else 	gcvalues.dashes = 4;

  /*
  {
    int cnt = 0;
    oldgcrec = GCValuesList;  
    while(oldgcrec)
      {
	oldgcrec = oldgcrec->next;
	cnt++;
      }
    printf("GCs = [%d]\n", cnt);
  }
  */
  
  /* search for a match */
  oldgcrec = GCValuesList;  
  while(oldgcrec)
    {
      oldgcv = oldgcrec->gcvalues;
      if(memcmp(oldgcv, &gcvalues, sizeof(XGCValues)) == 0 &&
	 oldgcrec->ref < (1<<30))
	{
	  oldgcrec->ref++;
	  return(oldgcrec->gc);
	}
      oldgcrec = oldgcrec->next;
    }
  /* we dont have it */
  {
    GCVRecord *nrec = (GCVRecord *)my_malloc(sizeof(GCVRecord),_FIG_ITEM_DATA_);
    nrec->gcvalues =  (XGCValues *)my_malloc(sizeof(XGCValues),_FIG_ITEM_DATA_);
    nrec->next = GCValuesList;
    nrec->ref = 1 + (mask & (1<<30));
    GCValuesList = nrec;
    memcpy(nrec->gcvalues, &gcvalues, sizeof(XGCValues));
    nrec->gc = XCreateGC(EZ_Display,  RootWindow(EZ_Display, EZ_ScreenNum),
			 mask & (~(1<<30)),  &gcvalues);
    return(nrec->gc);
  }
}
/**********************************************************************************************/
void EZ_FreeGC(gc) GC gc;
{
  GCVRecord *prev = NULL, *tmp = GCValuesList;  
  while(tmp)
    {
      if(tmp->gc == gc)
	{
	  tmp->ref -= 1;
	  if(tmp->ref <= 0 || tmp->ref >= (1<<30))
	    {
	      /* printf("++++++++ Freed GC: %lx\n", tmp->gc);*/
	      if(prev == NULL) GCValuesList = tmp->next;
	      else prev->next = tmp->next;
	      my_free(tmp->gcvalues);
	      XFreeGC(EZ_Display, tmp->gc);	      
	      my_free(tmp);
	    }
	  break;
	}
      prev = tmp;
      tmp = tmp->next;
    }
}
static void EZ_IncrementGCRef(gc, lw, xft) GC gc; int *lw; XFontStruct **xft;
{
  GCVRecord  *tmp = GCValuesList;  
  while(tmp)
    {
      if(tmp->gc == gc)
	{
	  tmp->ref++;
	  if(lw)   *lw = tmp->gcvalues->line_width;
	  if(xft)  *xft = EZ_GetFontFromXFontId(tmp->gcvalues->font);
	  return;
	  break;
	}
      tmp = tmp->next;
    }
  if(lw) *lw = 0;
  if(xft) *xft = EZ_GetFontFromId(EZ_LABEL_FONT);
}

/**********************************************************************************************/
void EZ_CleanUpFigItemGCs()
{
  GCVRecord *next, *tmp = GCValuesList;
  while(tmp)
    {
      next = tmp->next;
      my_free(tmp->gcvalues);
      XFreeGC(EZ_Display, tmp->gc);
      my_free(tmp);
      tmp = next;
    }
  GCValuesList = NULL;
}
/**********************************************************************************************/
Pixmap EZ_MakePixmapFromFigItem(item, width, height)
     EZ_Item *item; int *width, *height;
{
  if(item && EZ_ItemType(item) == EZ_FIG_ITEM)
    {
      int w=0, h=0;
      EZ_ComputeFigItemSize(item, &w, &h);
      if(w>0 && h>0)
        {
          GC gc = EZ_WRITABLEGC;      
          Pixmap pixmap = XCreatePixmap(EZ_Display, EZ_DummyWindow, w, h, EZ_Depth);
	  XSetForeground(EZ_Display, gc, 0L);
	  XFillRectangle(EZ_Display, pixmap, gc, 0, 0, w,h);
	  XSetForeground(EZ_Display, gc, 1L);
          EZ_DrawFigItemWork(NULL,pixmap, 0, 0, item, 0, 0, w, h, 0, 0, 1L, 0L, 0);
          *width = w;
          *height = h;
          return(pixmap);
        }
    }
  return(None);
}
/**********************************************************************************************/
void EZ_FreezeFigItem(item) EZ_Item *item;
{
  if(item && EZ_ItemType(item) == EZ_FIG_ITEM)
    {EZ_FigItemFreezed(item)=1;}
}

void EZ_UnFreezeFigItem(item) EZ_Item *item;
{
  if(item && EZ_ItemType(item) == EZ_FIG_ITEM && (EZ_FigItemFreezed(item)))
    {
      EZ_FigItemFreezed(item) = 0;
      fixFigItemPiece(item, 1);
    }
}
/**********************************************************************************************/
static void EZ_FigItemPostscript(item, fp, psinfo)
     EZ_Item *item; FILE *fp; void *psinfo;
{

}
/**********************************************************************************************/
#undef _EZ_ITEM_FIG_C_
