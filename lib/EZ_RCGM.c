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
 ***            Row and Column Geometry Manager                ***
 ***                                                           ***
 *****************************************************************/
#define _EZ_RCGM_C_
#include "EZ_Widget.h"

/*********************************************************************
 * 
 *  Functions implemented in this file:
 */
void  EZ_RowComputeGeometry MY_ANSIARGS((EZ_Widget *widget, int *w, int *h, EZ_Widget *dirtyAns));
void  EZ_ColComputeGeometry MY_ANSIARGS((EZ_Widget *widget, int *w, int *h, EZ_Widget *dirtyAns));
void  EZ_RowSetChildrenPositionAndSize MY_ANSIARGS((EZ_Widget *widget, int w, int h));
void  EZ_ColSetChildrenPositionAndSize MY_ANSIARGS((EZ_Widget *widget, int w, int h));
int   EZ_SetRowColMaxSize MY_ANSIARGS((EZ_Widget *widget, int size));
/****************************************************************************/
static void doOneRow MY_ANSIARGS((EZ_Widget *widget, EZ_Widget **twidgets,
                                  int nwidgets, int fillmode, int alignment, int startx,
                                  int starty,int maxh,int sepx));
static void doOneCol MY_ANSIARGS((EZ_Widget *widget, EZ_Widget **twidgets,
                                  int nwidgets, int fillmode, int alignment, int startx,
                                  int starty,int maxw,int sepy));
/****************************************************************************/

int EZ_SetRowColMaxSize(widget, size)
     EZ_Widget *widget;  int size; 
{
  if(widget)
    {
      EZ_GManager *gmanager = EZ_WidgetGManager(widget);
      EZ_RCGMData *rcdata;
      if(gmanager == (EZ_GManager *)NULL)
        {
          EZ_WidgetGManager(widget) = EZ_AllocateGManager(EZ_ROW_GEOM_MANAGER);
          gmanager = EZ_WidgetGManager(widget);  
        }
      EZ_GMType(gmanager) = EZ_ROW_GEOM_MANAGER;
      rcdata = &(EZ_GMRC(EZ_WidgetGManager(widget)));
      EZ_RCGMMaxSize(rcdata) = size;
      return(1);
    }
  return(0);
}
/****************************************************************************/
void  EZ_RowComputeGeometry(widget, w, h, topdirtywgt)
     EZ_Widget *widget; int *w, *h; EZ_Widget *topdirtywgt;
{
  EZ_GManager *gmanager;
  EZ_RCGMData *rcdata;
  EZ_Widget *child;
  int cw = 0, ch = 0;
  int tw, th, ww, hh;
  int maxw=0, maxh, bwpad;
  int nrows = 1, sepx, isepx;
  int i, nchildren, swidth[256], sheight[256];
  int *width = swidth, *height = sheight;

  if(widget == (EZ_Widget *)NULL || EZ_GetWidgetNonActiveFlag(widget) != 0)
    {
      *w = 0; *h = 0;
      return;
    }
  nchildren = EZ_WidgetNumChildren(widget);
  if(nchildren > 256)
    {
      nchildren++;
      width = (int *)my_malloc( nchildren * sizeof(int), _MISC_DATA_);
      height = (int *)my_malloc( nchildren * sizeof(int), _MISC_DATA_);
    }
  nchildren = 0;
  gmanager = EZ_WidgetGManager(widget);
  rcdata =&(EZ_GMRC(gmanager));
  maxw = EZ_RCGMMaxSize(rcdata);
  bwpad = EZ_WidgetBorderWidth(widget) + EZ_WidgetPadX(widget) + EZ_WidgetPadB(widget);
  
  /* pass 1 */
  child = EZ_WidgetChildren(widget);
  cw = 0;
  while(child)
    {
      if(EZ_GetWidgetNonActiveFlag(child) == 0)
        {
          EZ_ComputeWidgetWindowSize(child, &tw, &th, topdirtywgt);
          width[nchildren] = tw;
          height[nchildren] = th;
          cw = MAXV(cw, tw);
          nchildren++;
        }
      child = EZ_WidgetSibling(child);
    }
  if( maxw == 0)
    {
      int whint = EZ_WidgetWidthHint(widget);
      int w0 = EZ_WidgetWidth(widget);
      whint = MAXV(whint, w0);
      if(whint > 0)
        {
          whint -= (bwpad << 1);
          if(whint > 0) maxw = whint;
        }
    }
  if(maxw < cw) { maxw = cw+1;}
  
  /* pass 2 */
  ch = 0; cw = 0;
  ww = 0; hh = 0;
  maxh = 0;
  sepx = EZ_WidgetSepX(widget);
  isepx = 0;
  for(i = 0; i < nchildren; i++)
    {
      if(cw + width[i] + isepx> maxw)
        {
          ww = MAXV(ww, cw);
          hh += ch;
          cw = 0; ch = 0;
          nrows++;
          isepx = 0;
        }
      /* starts a new row */
        {
          cw += width[i] + isepx;
          ch = MAXV(ch, height[i]);
          isepx = sepx;
        }
    }

  ww = MAXV(ww, cw);
  hh += ch;
  hh += (nrows -1) * (EZ_WidgetSepY(widget));

  /* add in paddings and adjustments */
  {
    int itmp = bwpad;
    int jtmp = EZ_WidgetBorderWidth(widget) + EZ_WidgetPadY(widget) + EZ_WidgetPadB(widget) +
      EZ_WidgetAuxY(widget);
    int awtmp = EZ_WidgetAuxXLeft(widget) + EZ_WidgetAuxXRight(widget);
    int ahtmp =  EZ_WidgetHAdjust(widget);
    EZ_FixCheckMarkOffsets(widget, hh, jtmp);

    ww = ww + itmp + itmp + awtmp;
    hh = hh + jtmp + jtmp + ahtmp;
  }
  *w = ww;
  *h = hh;
  if(width != swidth) { my_free(width); my_free(height);}
}
/****************************************************************************/

void  EZ_RowSetChildrenPositionAndSize(widget, w, h)
     EZ_Widget *widget; int w, h;
{
  int             cw, ch, tw, th;
  int             bwpadx, bwpady, sepx,sepy,auxxL,auxxR, auxxT, auxy, auxy2;
  int             alignment, fillmode, hadjust;
  int             numchildren, nwidgets;
  int             maxh, accw, startx, starty;
  EZ_Widget       *children;
  EZ_Widget       *stwidgets[256];
  int             swidth[256], sheight[256];
  int             *width=swidth, *height=sheight;
  EZ_Widget       **twidgets = stwidgets;

  if(widget == (EZ_Widget *)NULL || EZ_GetWidgetNonActiveFlag(widget) != 0)
    return;

  /*----------------------------------------------------------
   * widget has no children, don't need to do anything
   *---------------------------------------------------------*/
  numchildren = EZ_GetNumberOfActiveChildren(widget);
  if(numchildren == 0)  return; 
  numchildren = EZ_WidgetNumChildren(widget);
  if(numchildren > 256)
    {
      numchildren++;
      width = (int *)my_malloc( numchildren * sizeof(int), _MISC_DATA_);
      height = (int *)my_malloc( numchildren * sizeof(int), _MISC_DATA_);
      twidgets = (EZ_Widget **)my_malloc( numchildren * sizeof(EZ_Widget *), _MISC_DATA_);
    }

  bwpadx = EZ_WidgetBorderWidth(widget) + EZ_WidgetPadX(widget) + EZ_WidgetPadB(widget);
  bwpady = EZ_WidgetBorderWidth(widget) + EZ_WidgetPadY(widget) + EZ_WidgetPadB(widget);
  auxxL = EZ_WidgetAuxXLeft(widget);
  auxxR = EZ_WidgetAuxXRight(widget);
  auxxT = auxxL + auxxR;
  auxy = EZ_WidgetAuxY(widget);
  auxy2 = auxy << 1;
  hadjust = EZ_WidgetHAdjust(widget);

  /*------------------------------------------------
   * Reset the position of all descendants
   *-----------------------------------------------*/ 
  cw = EZ_WidgetWidth(widget);      /* the new width  */
  ch = EZ_WidgetHeight(widget);     /* the new height */
  sepx = EZ_WidgetSepX(widget);     /* horizontal space between neighboring children */
  sepy = EZ_WidgetSepY(widget);     /* vertical space between neighboring children   */
  tw = cw - (bwpadx << 1) - auxxT;
  th = ch - (bwpady << 1) - auxy2 - hadjust;

  children = EZ_WidgetChildren(widget);
  nwidgets = 0;
  maxh = 0;
  accw = 0;
  starty = bwpady + auxy + hadjust;
  fillmode = EZ_WidgetFillMode(widget);
  if(fillmode == EZ_FILL_VERTICALLY || fillmode == EZ_FILL_BOTH) fillmode = 1;
  else fillmode = 0;
  alignment = EZ_WidgetAlignment(widget);
  while(children)
    {
      if(EZ_GetWidgetNoGeomFlag(children) == 0 &&
         EZ_WidgetType(children) < EZ_WIDGET_ITOPLEVEL)
        {
          int sw = EZ_WidgetWidth(children);
          int sh = EZ_WidgetHeight(children);
          if(accw + sw > tw && nwidgets > 0)
            {
              /* get a row */
              startx = bwpadx + auxxL;
              doOneRow(widget, twidgets, nwidgets,fillmode, alignment, 
                       startx, starty, maxh, sepx);
              /* reset counter */
              accw = 0;
              nwidgets = 0;
              starty += maxh + sepy;
              maxh = 0;
            }
          /* starts a new row */
            {
              twidgets[nwidgets] = children;
              width[nwidgets] = sw;
              height[nwidgets] = sh;
              maxh = MAXV(sh, maxh);
              twidgets[nwidgets] = children;
              nwidgets++;
              accw += (sepx + sw);
            }
        }
      children = EZ_WidgetSibling(children);
    }
  if(nwidgets > 0) /* the last row */
    {
      startx = bwpadx + auxxL;
      doOneRow(widget,twidgets,nwidgets,fillmode,alignment,startx,starty,maxh,sepx);
    }
  if(width != swidth) { my_free(width); my_free(height); my_free(twidgets);}  
} 
/****************************************************************************/

void  EZ_ColComputeGeometry(widget, w, h, topdirtywgt)
     EZ_Widget *widget; int *w, *h; EZ_Widget *topdirtywgt;
{
  EZ_GManager *gmanager;
  EZ_RCGMData *rcdata;
  EZ_Widget *child;
  int cw = 0, ch = 0;
  int tw, th, ww, hh;
  int maxh=0, maxw, bwpad;
  int ncols = 1, sepy, isepy;
  int i, nchildren=0, swidth[256], sheight[256];
  int *width = swidth, *height = sheight;

  if(widget == (EZ_Widget *)NULL || EZ_GetWidgetNonActiveFlag(widget) != 0)
    {
      *w = 0; *h = 0;
      return;
    }
  nchildren = EZ_WidgetNumChildren(widget);
  if(nchildren > 256)
    {
      nchildren++;
      width = (int *)my_malloc( nchildren * sizeof(int), _MISC_DATA_);
      height = (int *)my_malloc( nchildren * sizeof(int), _MISC_DATA_);
    }
  nchildren = 0;

  gmanager = EZ_WidgetGManager(widget);
  rcdata =&(EZ_GMRC(gmanager));
  maxh = EZ_RCGMMaxSize(rcdata);
  bwpad = EZ_WidgetBorderWidth(widget) + EZ_WidgetPadY(widget) + EZ_WidgetPadB(widget);
  
  /* pass 1 */
  child = EZ_WidgetChildren(widget);
  ch = 0;
  while(child)
    {
      if(EZ_GetWidgetNonActiveFlag(child) == 0)
        {
          EZ_ComputeWidgetWindowSize(child, &tw, &th, topdirtywgt);
          width[nchildren] = tw;
          height[nchildren] = th;
          ch = MAXV(ch, th);
          nchildren++;
        }
      child = EZ_WidgetSibling(child);
    }
  if(maxh == 0)
    {
      int hhint = EZ_WidgetHeightHint(widget);
      int h0= EZ_WidgetHeight(widget);
      hhint = MAXV(hhint, h0);
      if(hhint > 0)
        {
          hhint -= (bwpad << 1);
          if(hhint > 0) maxh = hhint;
        }
    }
  if(maxh < ch) { maxh = ch+1;}
  
  /* pass 2 */
  ch = 0; cw = 0;
  ww = 0; hh = 0;
  maxw = 0;
  sepy = EZ_WidgetSepY(widget);
  isepy = 0;
  for(i = 0; i < nchildren; i++)
    {
      if(ch + height[i] + isepy> maxh)
        {
          hh = MAXV(hh, ch);
          ww += cw;
          cw = 0; ch = 0;
          ncols++;
          isepy = 0;
        }
      /* a new column */
        {
          ch += height[i] + isepy;
          cw = MAXV(cw, width[i]);
          isepy = sepy;
        }
    }
  hh = MAXV(hh, ch);
  ww += cw;
  ww += (ncols -1) * (EZ_WidgetSepX(widget));

  /* add in paddings and adjustments */
  {
    int itmp = EZ_WidgetBorderWidth(widget) + EZ_WidgetPadX(widget) + EZ_WidgetPadB(widget);
    int jtmp = bwpad + EZ_WidgetAuxY(widget);
    int awtmp = EZ_WidgetAuxXLeft(widget) + EZ_WidgetAuxXRight(widget);
    int ahtmp =  EZ_WidgetHAdjust(widget);
    EZ_FixCheckMarkOffsets(widget, hh, jtmp);

    ww = ww + itmp + itmp + awtmp;
    hh = hh + jtmp + jtmp + ahtmp;
  }
  *w = ww;
  *h = hh;
  if(width != swidth) { my_free(width); my_free(height);}  
}
/****************************************************************************/
void  EZ_ColSetChildrenPositionAndSize(widget, w, h)
     EZ_Widget *widget; int w, h;
{
  int             cw, ch, tw, th;
  int             bwpadx, bwpady, sepx,sepy,auxxL,auxxR, auxxT, auxy, auxy2;
  int             alignment, fillmode, hadjust;
  int             numchildren, nwidgets;
  int             maxw, acch, startx, starty;
  EZ_Widget       *children;
  EZ_Widget       *stwidgets[256];
  int             swidth[256], sheight[256];
  int             *width=swidth, *height=sheight;
  EZ_Widget       **twidgets = stwidgets;

  if(widget == (EZ_Widget *)NULL || EZ_GetWidgetNonActiveFlag(widget) != 0)
    return;

  /*----------------------------------------------------------
   * widget has no children, don't need to do anything
   *---------------------------------------------------------*/
  numchildren = EZ_GetNumberOfActiveChildren(widget);
  if(numchildren == 0)  return; 

  numchildren = EZ_WidgetNumChildren(widget);
  if(numchildren > 256)
    {
      numchildren++;
      width = (int *)my_malloc( numchildren * sizeof(int), _MISC_DATA_);
      height = (int *)my_malloc( numchildren * sizeof(int), _MISC_DATA_);
      twidgets = (EZ_Widget **)my_malloc( numchildren * sizeof(EZ_Widget *), _MISC_DATA_);
    }

  bwpadx = EZ_WidgetBorderWidth(widget) + EZ_WidgetPadX(widget) + EZ_WidgetPadB(widget);
  bwpady = EZ_WidgetBorderWidth(widget) + EZ_WidgetPadY(widget) + EZ_WidgetPadB(widget);
  auxxL = EZ_WidgetAuxXLeft(widget);
  auxxR = EZ_WidgetAuxXRight(widget);
  auxxT = auxxL + auxxR;
  auxy = EZ_WidgetAuxY(widget);
  auxy2 = auxy << 1;
  hadjust = EZ_WidgetHAdjust(widget);

  /*------------------------------------------------
   * Reset the position of all descendants
   *-----------------------------------------------*/ 
  cw = EZ_WidgetWidth(widget);      /* the new width  */
  ch = EZ_WidgetHeight(widget);     /* the new height */
  sepx = EZ_WidgetSepX(widget);     /* horizontal space between neighboring children */
  sepy = EZ_WidgetSepY(widget);     /* vertical space between neighboring children   */
  tw = cw - (bwpadx << 1) - auxxT;
  th = ch - (bwpady << 1) - auxy2 - hadjust;

  children = EZ_WidgetChildren(widget);
  nwidgets = 0;
  maxw = 0;
  acch = 0;
  startx = bwpadx + auxxL;
  fillmode = EZ_WidgetFillMode(widget);
  if(fillmode == EZ_FILL_HORIZONTALLY || fillmode == EZ_FILL_BOTH) fillmode = 1;
  else fillmode = 0;
  alignment = EZ_WidgetAlignment(widget);
  while(children)
    {
      if(EZ_GetWidgetNoGeomFlag(children) == 0 &&
         EZ_WidgetType(children) < EZ_WIDGET_ITOPLEVEL)
        {
          int sw = EZ_WidgetWidth(children);
          int sh = EZ_WidgetHeight(children);
          if(acch + sh > th && nwidgets > 0)
            {
              /* get a row */
              starty = bwpady + auxy + hadjust;
              doOneCol(widget, twidgets, nwidgets,fillmode, alignment, 
                       startx, starty, maxw, sepy);
              /* reset counter */
              acch = 0;
              nwidgets = 0;
              startx += maxw + sepx;
              maxw = 0;
            }
          /* starts a new row */
            {
              twidgets[nwidgets] = children;
              width[nwidgets] = sw;
              height[nwidgets] = sh;
              maxw = MAXV(sw, maxw);
              twidgets[nwidgets] = children;
              nwidgets++;
              acch += (sepy + sh);
            }
        }
      children = EZ_WidgetSibling(children);
    }
  if(nwidgets > 0) /* the last row */
    {
      starty = bwpady + auxy + hadjust;
      doOneCol(widget,twidgets,nwidgets,fillmode,alignment,startx,starty,maxw,sepy);
    }
  if(width != swidth) { my_free(width); my_free(height); my_free(twidgets);}  
} 
/****************************************************************************/
static void doOneRow(widget, twidgets, nwidgets,fillmode, alignment, startx, starty, maxh, sepx)
     EZ_Widget *widget, **twidgets; int nwidgets, fillmode, alignment, startx,starty, maxh, sepx;
{
  int i;
  if(fillmode)
    {
      for(i = 0; i < nwidgets; i++)
        {
          EZ_Widget *t = twidgets[i];
          int www = EZ_WidgetWidth(t);
          if(EZ_GetWidgetHeightSetFlag(t) == 0)
            {
              EZ_WidgetAHeight(t) = maxh;
              EZ_SetChildrenPositionAndSize(t, www, maxh);
              EZ_WidgetOriginX(t) = startx;
              EZ_WidgetOriginY(t) = starty;
              startx += www + sepx;
            }
          else
            {
              int hhh = EZ_WidgetHeight(t);
              int diff = (maxh - hhh);
              int yy0 = starty;
              switch(alignment)
                {
                case EZ_TOP_ALIGNED:
                  break;
                case EZ_BOTTOM_ALIGNED:
                  yy0 += diff;
                  break;
                default: /* center aligned */
                  yy0 += diff>>1;
                }
              EZ_WidgetOriginX(t) = startx;
              EZ_WidgetOriginY(t) = yy0;   
              startx += www + sepx;                       
            }
        }
    }
  else
    {
      for(i = 0; i < nwidgets; i++)
        {
          EZ_Widget *t = twidgets[i];
          int www = EZ_WidgetWidth(t);
          int hhh = EZ_WidgetHeight(t);
          int diff = (maxh - hhh);
          int yy0 = starty;
          switch(alignment)
            {
            case EZ_TOP_ALIGNED:
              break;
            case EZ_BOTTOM_ALIGNED:
              yy0 += diff;
              break;
            default: /* center aligned */
              yy0 += diff>>1;
            }
          EZ_WidgetOriginX(t) = startx;
          EZ_WidgetOriginY(t) = yy0;   
          startx += www + sepx;                       
        }
    }
}
/****************************************************************************/
static void doOneCol(widget, twidgets, nwidgets,fillmode, alignment, startx, starty, maxw, sepy)
     EZ_Widget *widget, **twidgets; int nwidgets, fillmode, alignment, startx,starty, maxw, sepy;
{
  int i;
  if(fillmode)
    {
      for(i = 0; i < nwidgets; i++)
        {
          EZ_Widget *t = twidgets[i];
          int hhh = EZ_WidgetHeight(t);
          if(EZ_GetWidgetWidthSetFlag(t) == 0)
            {
              EZ_WidgetAWidth(t) = maxw;
              EZ_SetChildrenPositionAndSize(t, maxw, hhh);
              EZ_WidgetOriginX(t) = startx;
              EZ_WidgetOriginY(t) = starty;
              starty += hhh + sepy;
            }
          else
            {
              int www = EZ_WidgetWidth(t);
              int diff = (maxw - www);
              int xx0 = startx;
              switch(alignment)
                {
                case EZ_LEFT_ALIGNED:
                  break;
                case EZ_RIGHT_ALIGNED:
                  xx0 += diff;
                  break;
                default: /* center aligned */
                  xx0 += diff>>1;
                }
              EZ_WidgetOriginX(t) = xx0;
              EZ_WidgetOriginY(t) = starty;
              starty += hhh + sepy;                       
            }
        }
    }
  else
    {
      for(i = 0; i < nwidgets; i++)
        {
          EZ_Widget *t = twidgets[i];
          int www = EZ_WidgetWidth(t);
          int hhh = EZ_WidgetHeight(t);
          int diff = (maxw - www);
          int xx0 = startx;
          switch(alignment)
            {
            case EZ_LEFT_ALIGNED:
              break;
            case EZ_RIGHT_ALIGNED:
              xx0 += diff;
              break;
            default: /* center aligned */
              xx0 += diff>>1;
            }
          EZ_WidgetOriginX(t) = xx0;
          EZ_WidgetOriginY(t) = starty;
          starty += hhh + sepy;                       
        }
    }
}
/****************************************************************************/
#undef _EZ_RCGM_C_






