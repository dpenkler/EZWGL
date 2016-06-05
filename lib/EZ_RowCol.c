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
 ***            RowColumn (aka GridBag) Geometry Manager       ***
 ***                                                           ***
 *****************************************************************/
#define _EZ_ROWCOL_C_
#include "EZ_Widget.h"

/*********************************************************************
 * 
 *  Functions implemented in this file:
 */
void  EZ_RowColumnComputeGeometry MY_ANSIARGS((EZ_Widget *widget, 
						int *w, int *h,
						EZ_Widget *dirtyAns));
void  EZ_RowColSetChildrenPositionAndSize MY_ANSIARGS((EZ_Widget *widget,
						       int w, int h));
int   EZ_SetRowColConstrains MY_ANSIARGS((EZ_Widget *widget, 
					  int rc, int idx, int minsize, int weight, 
					  int pad));

void  EZ_ShowGrid     MY_ANSIARGS((EZ_Widget *w));
void  EZ_ShowGridAll  MY_ANSIARGS((EZ_Widget *w, int flag));

/*********************************************************************/
static int  ComputeRowLayout MY_ANSIARGS((EZ_Widget *widget));
static int  ComputeColLayout MY_ANSIARGS((EZ_Widget *widget));
static void ReArrangeRowColumns MY_ANSIARGS((EZ_RowColStruct *rc, int units,
					     int size, int *which));
static void EZ_RowColGetGrid  MY_ANSIARGS((EZ_Widget *widget, 
					   int *w, int *h));
/*********************************************************************/

typedef struct RCCellInfo_
{
  EZ_Widget          *widget;
  struct RCCellInfo_ *next;
} RCCellInfo;

typedef struct RCInfo_
{
  RCCellInfo *cell;
  int minsize, offset, offset2, weight, padding;
} RCInfo;

/*********************************************************************/

static void EZ_RowColGetGrid(widget, w, h)
     EZ_Widget  *widget; int *w, *h;
{
  if(widget)
    {
      EZ_Widget *children;
      int cw, ch;
      cw = ch = 0;
      children = EZ_WidgetChildren(widget);
      while(children)
	{
	  int x = EZ_WidgetGBX(children);
	  int y = EZ_WidgetGBY(children);
	  int ws = EZ_WidgetGBW(children);
	  int hs = EZ_WidgetGBH(children);
	  int tw = x + ws;
	  int th = y + hs;
	  if(cw < tw) cw = tw;
	  if(ch < th) ch = th;
	  children = EZ_WidgetSibling(children);
	}
      *w = cw;
      *h = ch;
    }
  else
    { 
      *w = 0;
      *h = 0;
    }
}
/*****************************************************************/
static void  setRowColConstrains(ptr,space,idx,sminsize,sweight,spad, flag)
     EZ_RowColStruct **ptr;
     int *space, idx, sminsize, sweight, spad, flag; 
{
  if(*ptr == NULL)
    {
      int size = idx + 16;
      EZ_RowColStruct *tptr;
      tptr = (EZ_RowColStruct *)my_malloc( size * sizeof(EZ_RowColStruct),_GRID_BAG_D_);
      if(tptr == NULL) EZ_OutOfMemory("setRowColConstrains");
      memset( tptr, 0, size * sizeof(EZ_RowColStruct));
      *ptr = tptr;
      *space = size;
    }
  else
    {
      if( *space <= idx)  /* grow */
	{
	  int size = idx + 16;
	  EZ_RowColStruct *tptr = (EZ_RowColStruct *)
	    my_malloc( size * sizeof(EZ_RowColStruct), _GRID_BAG_D_);
	  if(tptr == NULL) EZ_OutOfMemory("setRowColConstrains");
	  memcpy(tptr, *ptr, (*space) * sizeof(EZ_RowColStruct));
	  memset((void*)(tptr +(*space)), 0, (size - (*space)) * sizeof(EZ_RowColStruct));
	  (void)my_free((char *)(*ptr));
	  *ptr = tptr;
	  *space = size;
	}
    }
  if(flag)
    {
      int i, start = (flag > 1 ? 0 : idx);
      for(i = start; i <= idx; i++)
        {
          EZ_RowColStruct *tptr = (*ptr) + i;
          EZ_GridBagRCMinSize(tptr) = sminsize;
          EZ_GridBagRCWeight(tptr) = sweight;
          EZ_GridBagRCPadding(tptr) = spad;      
        }
    }
}
/************************************************************************/
int  EZ_SetRowColConstrains(widget, rc, idx, minsize, weight, pad)
     EZ_Widget *widget;
     int rc, idx, minsize, weight, pad;
{
  if(widget)
    {
      int err = 0;
      if(idx < 0 || idx > 512 ) err = 1;
      else
	{
	  int sweight = (weight < 0 ? 0 : weight);
	  int sminsize = (minsize < 0 ? 0 : minsize);
	  int spad = pad + pad;
          int allrcs = 0;
	  int *space = NULL;
	  EZ_RowColStruct **ptr = NULL;
	  EZ_GridBagGMData *gbdata;
          EZ_GManager *gmanager = EZ_WidgetGManager(widget);
          
	  if(gmanager == (EZ_GManager *)NULL)
            {
              EZ_WidgetGManager(widget) = EZ_AllocateGManager(EZ_GB_GEOM_MANAGER);
              gmanager = EZ_WidgetGManager(widget);

            }
          EZ_GMType(gmanager) = EZ_GB_GEOM_MANAGER;
	  gbdata = &(EZ_GMGB(EZ_WidgetGManager(widget)));

          switch(rc)
            {
            case EZ_ALL_ROWS:
              allrcs = 1;
            case EZ_ROW:
	      ptr = &(EZ_GBGMRowPtr(gbdata));
	      space = &(EZ_GBGMRowSpace(gbdata));
              break;
            case EZ_ALL_COLUMNS:
              allrcs = 1;
            case EZ_COLUMN:
	      ptr = &(EZ_GBGMColPtr(gbdata));
	      space = &(EZ_GBGMColSpace(gbdata));
              break;
            default:
              err = 1;
              break;
	    }
	  if(ptr) 
            {
              setRowColConstrains(ptr,space,idx,sminsize,sweight,spad,1+allrcs); 
              return(1);
            }
	}
      if(err) fprintf(stderr,"EZ_SetRowColConstrains: arguments out of range.\n");
    }
  return(0);
}

/*****************************************************************/
void  EZ_RowColumnComputeGeometry(widget, w, h, tlda)
     EZ_Widget *widget, *tlda;
     int *w, *h;
{
  if(widget == NULL) { *w = 0; *h = 0; return;}
  {
    EZ_GManager *gmanager = EZ_WidgetGManager(widget);
    EZ_GridBagGMData *gbdata;
    int rsize, csize,cw,ch;

    if(gmanager == NULL || EZ_GMType(gmanager) != EZ_GB_GEOM_MANAGER)
      { *w = 0; *h = 0; return;}
    EZ_RowColGetGrid(widget, &csize, &rsize);
    gbdata = &(EZ_GMGB(gmanager));
    /* check allocated spaces */
    setRowColConstrains(&(EZ_GBGMRowPtr(gbdata)), &(EZ_GBGMRowSpace(gbdata)),rsize, 0,0,0,0);
    setRowColConstrains(&(EZ_GBGMColPtr(gbdata)), &(EZ_GBGMColSpace(gbdata)),csize, 0,0,0,0);
    EZ_GBGMNRows(gbdata) = rsize;
    EZ_GBGMNCols(gbdata) = csize;
    if(rsize == 0 || csize == 0)  { *w = 0; *h = 0; return;}
    /* compute the geometry of all children widgets */
    {
      EZ_Widget *twgt = EZ_WidgetChildren(widget);
      while(twgt)
	{
	  int tw, th;
	  if(EZ_GetWidgetNonActiveFlag(twgt) == 0)
	    EZ_ComputeWidgetWindowSize(twgt, &tw, &th, tlda);
	  twgt = EZ_WidgetSibling(twgt);
	}
    }
    /* layout the grid */
    cw = ComputeColLayout(widget);
    ch = ComputeRowLayout(widget);
    {
      int itmp = EZ_WidgetBorderWidth(widget) + EZ_WidgetPadX(widget) + EZ_WidgetPadB(widget);
      int jtmp = EZ_WidgetBorderWidth(widget) + EZ_WidgetPadY(widget) + EZ_WidgetPadB(widget) +
	EZ_WidgetAuxY(widget);
      int ahtmp =  EZ_WidgetHAdjust(widget);
      EZ_FixCheckMarkOffsets(widget, ch, jtmp);
      {
	int awtmp = EZ_WidgetAuxXLeft(widget) + EZ_WidgetAuxXRight(widget);
	int xtmp = EZ_WidgetSepX(widget);
	int ytmp = EZ_WidgetSepY(widget);
	*w = cw + itmp + itmp + awtmp - xtmp;
	*h = ch + jtmp + jtmp + ahtmp - ytmp;
      }
    }
  }
}
/****************************************************************************/
static int ComputeColLayout(widget)
     EZ_Widget *widget; 
{
  RCInfo rcinfo[34], *rcinfoptr;
  RCCellInfo units[102], *cellptr;
  EZ_RowColStruct *trc, *rc;
  int minsize, csize, i, itmp, cellcount, ncols;
  EZ_GManager *gmanager = EZ_WidgetGManager(widget);
  EZ_GridBagGMData *gbdata = &(EZ_GMGB(gmanager));

  rc = EZ_GBGMColPtr(gbdata);
  ncols = EZ_GBGMNCols(gbdata);
  if(ncols >= 32)
    {
      rcinfoptr = (RCInfo *)my_malloc( (ncols+2)*sizeof(RCInfo),_GRID_BAG_D_);
      if(rcinfoptr == NULL) EZ_OutOfMemory("ComputeColLayout");
    }
  else rcinfoptr = rcinfo;
  rcinfoptr->minsize = 0;
  rcinfoptr->offset = 0;
  rcinfoptr->padding = 0;
  rcinfoptr++;

  cellcount = EZ_WidgetNumChildren(widget)+2;
  if(cellcount > 100)
    {
      cellptr = (RCCellInfo *)my_malloc( (cellcount+2)*sizeof(RCInfo),_GRID_BAG_D_);
      if(cellptr == NULL) EZ_OutOfMemory("ComputeColLayout");  
    }
  else cellptr = units;
  cellcount = 0;
  
  /* fill in tmp data */
  itmp = EZ_WidgetSepX(widget);
  for(trc = rc, i = 0; i < ncols; i++, trc++)
    {
      rcinfoptr[i].cell = NULL;
      rcinfoptr[i].minsize = EZ_GridBagRCMinSize(trc);
      rcinfoptr[i].weight = EZ_GridBagRCWeight(trc);
      rcinfoptr[i].padding = EZ_GridBagRCPadding(trc) + itmp;
    }
  /* determine min size */
  {
    EZ_Widget *child = EZ_WidgetChildren(widget);
    while(child)
      {
	if(EZ_GetWidgetNoGeomFlag(child) == 0 &&
	   EZ_WidgetType(child) < EZ_WIDGET_ITOPLEVEL)
	  {
	    int x = EZ_WidgetGBX(child);
	    int s = EZ_WidgetGBW(child);
	    if(s == 1)
	      {
		int w = EZ_WidgetWidth(child) + rcinfoptr[x].padding;
		if(w > rcinfoptr[x].minsize) rcinfoptr[x].minsize = w;
	      }
	    else
	      {
		int idx = x + s - 1;
		cellptr[cellcount].widget = child;
		cellptr[cellcount].next = rcinfoptr[idx].cell;
		rcinfoptr[idx].cell = cellptr + cellcount;
		cellcount++;
	      }
	  }
	child = EZ_WidgetSibling(child);
      }
  }
  csize = 0;
  {
    RCCellInfo *cptr;
    for(i = 0; i < ncols; i++)
      {
	int itmp = (rcinfoptr[i].padding)/2;
	csize += rcinfoptr[i].minsize;
	for(cptr = rcinfoptr[i].cell; cptr != NULL; cptr = cptr->next)
	  {
	    EZ_Widget *wgt = cptr->widget;
	    int s = EZ_WidgetGBW(wgt);
	    int tmp = EZ_WidgetWidth(wgt) + itmp + rcinfoptr[i-s].offset;
	    if(tmp > csize) csize = tmp;
	  }
	rcinfoptr[i].offset = csize;
      }
  }

  /* we have now the minimal size and the minimal offset */

  minsize = csize;
  
  for(i = 0; i < ncols; i++) rcinfoptr[i].offset2 = csize;
  {
    RCCellInfo *cptr;
    int col = ncols-1; 
    while(col > 0)
      {
	int itmp = (rcinfoptr[col].padding)/2;	
	cptr = rcinfoptr[col].cell;
	while(cptr)
	  {
	    EZ_Widget *wgt = cptr->widget;
	    int s = EZ_WidgetGBW(wgt);
	    int x = col - s;
	    int tmp = csize - (EZ_WidgetWidth(wgt) + itmp);
	    if(x >= 0 && tmp < rcinfoptr[x].offset2)
	      rcinfoptr[x].offset2 = tmp;
	    cptr = cptr->next;
	  }
	csize -= rcinfoptr[col].minsize;
	col--;
	if(rcinfoptr[col].offset2 < csize) csize = rcinfoptr[col].offset2;
	else  rcinfoptr[col].offset2 = csize;
      }
  }

  /* now resolve constrains based on weights */
  {
    int col = 0;
    int last;

    while(col < ncols)
      {
	int  weight, tweight, nweight, require, space;
	tweight = nweight = require = 0;

	if(rcinfoptr[col].offset == rcinfoptr[col].offset2) {col++; continue;}
	for(last = col+1; last < ncols; last++)
	  {
	    if(rcinfoptr[last].offset == rcinfoptr[last].offset2)
	      break;
	  }
	for(i = col; i <= last; i++)
	  {
	    tweight += rcinfoptr[i].weight;
	    require += rcinfoptr[i].minsize;
	  }
	space = rcinfoptr[last].offset2 - rcinfoptr[col-1].offset;
	if(tweight == 0) {tweight = last -col +1; nweight = 1;}
	for(weight=0,i=col; i < last; i++)
	  {
	    int spare = rcinfoptr[i].offset2 - rcinfoptr[i].offset;
	    if(nweight)  weight++; else weight += rcinfoptr[i].weight;
	    if((nweight || rcinfoptr[i].weight > 0) &&
	       (spare * tweight/weight) <(space - require))
	      space = spare * tweight/weight + require;
	  }
	for(weight=0, i=col; i<last; i++)
	  {
	    if(nweight)  weight++; else weight += rcinfoptr[i].weight;
	    rcinfoptr[i].offset += (int) ((double)(space-require)*weight/tweight + 0.5);
	    rcinfoptr[i].minsize = rcinfoptr[i].offset - rcinfoptr[i-1].offset;
	  }
	rcinfoptr[i].minsize = rcinfoptr[i].offset - rcinfoptr[i-1].offset;	
	for(i=last; i > col; i--)
	  rcinfoptr[i-1].offset2 = rcinfoptr[i].offset2 - rcinfoptr[i].minsize;
      }
    /* constrains should be resolved */
    for(trc = rc, i=0; i<ncols; i++,trc++) 
      {
	EZ_GridBagRCOffset(trc) = rcinfoptr[i].offset;
	EZ_GridBagRCCMinSize(trc) = rcinfoptr[i].minsize;
      }
    if(cellptr != units) (void) my_free((char *)cellptr);
    rcinfoptr--;
    if(rcinfoptr != rcinfo) (void) my_free((char *)rcinfoptr);
  }    
  return(minsize);
}

static int ComputeRowLayout(widget)
     EZ_Widget *widget; 
{
  RCInfo rcinfo[34], *rcinfoptr;
  RCCellInfo units[102], *cellptr;
  EZ_RowColStruct *trc, *rc;
  int minsize, csize, i, itmp, cellcount, nrows;
  EZ_GManager *gmanager = EZ_WidgetGManager(widget);
  EZ_GridBagGMData *gbdata = &(EZ_GMGB(gmanager));

  rc = EZ_GBGMRowPtr(gbdata);
  nrows = EZ_GBGMNRows(gbdata);
  if(nrows >= 32)
    {
      rcinfoptr = (RCInfo *)my_malloc( (nrows+2)*sizeof(RCInfo),_GRID_BAG_D_);
      if(rcinfoptr == NULL) EZ_OutOfMemory("ComputeRowLayout");
    }
  else rcinfoptr = rcinfo;
  rcinfoptr->minsize = 0;
  rcinfoptr->offset = 0;
  rcinfoptr->padding = 0;
  rcinfoptr++;

  cellcount = EZ_WidgetNumChildren(widget)+2;
  if(cellcount > 100)
    {
      cellptr = (RCCellInfo *)my_malloc( (cellcount+2)*sizeof(RCInfo),_GRID_BAG_D_);
      if(cellptr == NULL) EZ_OutOfMemory("ComputeRowLayout");  
    }
  else cellptr = units;
  cellcount = 0;
  
  /* fill in tmp data */
  itmp = EZ_WidgetSepY(widget);
  for(trc = rc, i = 0; i < nrows; i++, trc++)
    {
      rcinfoptr[i].cell = NULL;
      rcinfoptr[i].minsize = EZ_GridBagRCMinSize(trc);
      rcinfoptr[i].weight = EZ_GridBagRCWeight(trc);
      rcinfoptr[i].padding = EZ_GridBagRCPadding(trc) + itmp;
    }

  /* determine min size */
  {
    EZ_Widget *child = EZ_WidgetChildren(widget);
    while(child)
      {
	if(EZ_GetWidgetNoGeomFlag(child) == 0 &&
	   EZ_WidgetType(child) < EZ_WIDGET_ITOPLEVEL)
	  {
	    int x = EZ_WidgetGBY(child);
	    int s = EZ_WidgetGBH(child);
	    if(s == 1)
	      {
		int w = EZ_WidgetHeight(child) + rcinfoptr[x].padding;
		if(w > rcinfoptr[x].minsize) rcinfoptr[x].minsize = w;
	      }
	    else
	      {
		int idx = x + s - 1;
		cellptr[cellcount].widget = child;
		cellptr[cellcount].next = rcinfoptr[idx].cell;
		rcinfoptr[idx].cell = cellptr + cellcount;
		cellcount++;
	      }
	  }
	child = EZ_WidgetSibling(child);
      }
  }
  csize = 0;
  {
    RCCellInfo *cptr;
    for(i = 0; i < nrows; i++)
      {
	int itmp = (rcinfoptr[i].padding)/2;
	csize += rcinfoptr[i].minsize;
	for(cptr = rcinfoptr[i].cell; cptr != NULL; cptr = cptr->next)
	  {
	    EZ_Widget *wgt = cptr->widget;
	    int s = EZ_WidgetGBH(wgt);
	    int tmp = EZ_WidgetHeight(wgt) + itmp + rcinfoptr[i-s].offset;
	    if(tmp > csize) csize = tmp;
	  }
	rcinfoptr[i].offset = csize;
      }
  }

  /* we have the minimal size now */
  minsize = csize;
  
  for(i = 0; i < nrows; i++) rcinfoptr[i].offset2 = csize;
  {
    RCCellInfo *cptr;
    int row = nrows-1; 
    while(row > 0)
      {
	int itmp = (rcinfoptr[row].padding)/2;
	cptr = rcinfoptr[row].cell;
	while(cptr)
	  {
	    EZ_Widget *wgt = cptr->widget;
	    int s = EZ_WidgetGBH(wgt);
	    int x = row - s;
	    int tmp = csize - (EZ_WidgetHeight(wgt) + itmp);
	    if(x >= 0 && tmp < rcinfoptr[x].offset2)
	      rcinfoptr[x].offset2 = tmp;
	    cptr = cptr->next;
	  }
	csize -= rcinfoptr[row].minsize;
	row--;
	if(rcinfoptr[row].offset2 < csize) csize = rcinfoptr[row].offset2;
	else  rcinfoptr[row].offset2 = csize;
      }
  }

  /* now resolve constrains based on weights */
  {
    int row = 0;
    int last;

    while(row < nrows)
      {
	int  weight, tweight, nweight, require, space;
	tweight = nweight = require = 0;

	if(rcinfoptr[row].offset == rcinfoptr[row].offset2) {row++; continue;}
	for(last = row+1; last < nrows; last++)
	  {
	    if(rcinfoptr[last].offset == rcinfoptr[last].offset2)
	      break;
	  }
	for(i = row; i <= last; i++)
	  {
	    tweight += rcinfoptr[i].weight;
	    require += rcinfoptr[i].minsize;
	  }
	space = rcinfoptr[last].offset2 - rcinfoptr[row-1].offset;
	if(tweight == 0) {tweight = last -row +1; nweight = 1;}
	for(weight=0,i=row; i < last; i++)
	  {
	    int spare = rcinfoptr[i].offset2 - rcinfoptr[i].offset;
	    if(nweight)  weight++; else weight += rcinfoptr[i].weight;
	    if((nweight || rcinfoptr[i].weight > 0) &&
	       (spare * tweight/weight) <(space - require))
	      space = spare * tweight/weight + require;
	  }
	for(weight=0, i=row; i<last; i++)
	  {
	    if(nweight)  weight++; else weight += rcinfoptr[i].weight;
	    rcinfoptr[i].offset += (int) ((double)(space-require)*weight/tweight + 0.5);
	    rcinfoptr[i].minsize = rcinfoptr[i].offset - rcinfoptr[i-1].offset;
	  }
	rcinfoptr[i].minsize = rcinfoptr[i].offset - rcinfoptr[i-1].offset;	
	for(i=last; i > row; i--)
	  rcinfoptr[i-1].offset2 = rcinfoptr[i].offset2 - rcinfoptr[i].minsize;
      }
    /* constrains should be resolved */
    for(trc = rc, i=0; i<nrows; i++,trc++) 
      {
	EZ_GridBagRCOffset(trc) = rcinfoptr[i].offset;
	EZ_GridBagRCCMinSize(trc) = rcinfoptr[i].minsize;
      }
    if(cellptr != units) (void) my_free((char *)cellptr);
    rcinfoptr--;
    if(rcinfoptr != rcinfo) (void) my_free((char *)rcinfoptr);
  }    
  return(minsize);
}

/****************************************************************************/

void  EZ_RowColSetChildrenPositionAndSize(widget, w, h)
     EZ_Widget *widget; int w, h;
{
  int padX, padY, bw, auxxL, auxxR, auxy, hadjust;
  EZ_GManager *gmanager;
  EZ_GridBagGMData *gbdata;

  if(widget == NULL) return;
  gmanager = EZ_WidgetGManager(widget);
  gbdata =  &(EZ_GMGB(gmanager));

  bw = EZ_WidgetBorderWidth(widget) + EZ_WidgetPadB(widget);
  padX = EZ_WidgetPadX(widget);
  padY = EZ_WidgetPadY(widget);
  auxxL = EZ_WidgetAuxXLeft(widget);
  auxxR = EZ_WidgetAuxXRight(widget);
  auxy = EZ_WidgetAuxY(widget);
  hadjust = EZ_WidgetHAdjust(widget);

  /* if width or height has been set explicitly, used the set dimension */
  if(EZ_GetWidgetWidthSetFlag(widget))   w = EZ_WidgetWidth(widget);
  if(EZ_GetWidgetHeightSetFlag(widget))  h = EZ_WidgetHeight(widget);
  
  if(w != EZ_WidgetMinWidth(widget))
    {
      int xtmp = auxxL + auxxR - EZ_WidgetSepX(widget) ;
      int tw = w - (bw + padX) * 2 - xtmp;
      EZ_RowColStruct *rc = EZ_GBGMColPtr(gbdata); 
      int units = EZ_GBGMNCols(gbdata);
      ReArrangeRowColumns(rc, units, tw, &(EZ_GBGMXOffset(gbdata)));
    }
  else EZ_GBGMXOffset(gbdata) = 0;

  if(h != EZ_WidgetMinHeight(widget))  
    {
      int ytmp = auxy + auxy + hadjust - EZ_WidgetSepY(widget) ;
      int th = h - (bw + padY) * 2 - ytmp;
      EZ_RowColStruct *rc = EZ_GBGMRowPtr(gbdata); 
      int units = EZ_GBGMNRows(gbdata);
      ReArrangeRowColumns(rc, units, th,  &(EZ_GBGMYOffset(gbdata)));
    }
  else EZ_GBGMYOffset(gbdata) = 0;

  /* this is the new dimension */
  EZ_WidgetWidth(widget) = w;
  EZ_WidgetHeight(widget) = h;

  /* set geometry for all children widgets */
  {
    EZ_Widget *children = EZ_WidgetChildren(widget);
    EZ_RowColStruct *rows = EZ_GBGMRowPtr(gbdata);     
    EZ_RowColStruct *cols = EZ_GBGMColPtr(gbdata); 
    int sepX = EZ_WidgetSepX(widget);
    int sepY = EZ_WidgetSepY(widget);
    int xoffset = (bw + padX + auxxL) + EZ_GBGMXOffset(gbdata) - sepX/2;
    int yoffset = (bw + padY + auxy + hadjust) + EZ_GBGMYOffset(gbdata) - sepY/2;

    while(children)
      {
	if(EZ_GetWidgetNoGeomFlag(children) == 0 &&
	   EZ_WidgetType(children) < EZ_WIDGET_ITOPLEVEL)
	  {
	    int gx = EZ_WidgetGBX(children);
	    int gy = EZ_WidgetGBY(children);
	    int gw = EZ_WidgetGBW(children);
	    int gh = EZ_WidgetGBH(children);
	    int tx = (gx > 0)? EZ_GridBagRCOffset(cols + (gx -1)): 0;
	    int ty = (gy > 0)? EZ_GridBagRCOffset(rows + (gy -1)): 0;
	    int tw = EZ_GridBagRCOffset(cols + (gx+gw-1)) - tx;  
	    int th = EZ_GridBagRCOffset(rows + (gy+gh-1)) - ty;  
	    int ttx = 0, tty = 0;

	    tx += xoffset;
	    ty += yoffset;
	    /* [tx, ty, tx+tw, ty+th] defines the cell */

	    /*-------------------------------------------------
	     * if EXPAND flag is set on children, it expands to
	     * the dimension of the whole cell
	     *------------------------------------------------*/
	    if(EZ_GetWidgetExpandFlag(children) == 0)
	      {
		int gf = EZ_WidgetGBFill(children);
		int ga = EZ_WidgetGBAnchor(children); 
		int gpadX, gpadY, gpadX1, gpadY1;

		/* find padding around children */
		gpadX = EZ_GridBagRCPadding(cols + gx);
		gpadY = EZ_GridBagRCPadding(rows + gy);
		if(gw == 1)
		  {
		    int itmp = gpadX;
		    gpadX = gpadX/2;
		    gpadX1 = itmp - gpadX;
		  }
		else
		  {
		    gpadX = gpadX/2;
		    gpadX1 = (EZ_GridBagRCPadding(cols + (gx + gw -1)))/2;
		  }
		if(gh == 1)
		  {
		    int itmp = gpadY;
		    gpadY = gpadY/2;
		    gpadY1 = itmp - gpadY;
		  }
		else
		  {
		    gpadY = gpadY/2;
		    gpadY1 = (EZ_GridBagRCPadding(rows + (gy + gh -1)))/2;
		  }
		
		/* padding at the top left corner */
		ttx += gpadX + sepX/2;
		tty += gpadY + sepY/2;

		if(gf == EZ_FILL_NONE)
		  {
		    int saveh = th -(gpadY + gpadY1 + sepY);
		    int savew = tw -(gpadX + gpadX1 + sepX);
		    int xoff, yoff;
		    tw = EZ_WidgetWidth(children);
		    th = EZ_WidgetHeight(children);
		    xoff = (savew - tw);
		    yoff = (saveh - th);
		    switch(ga)
		      {
		      case EZ_TOP_LEFT: 
			break;
		      case EZ_TOP:
			ttx += xoff/2;
			break;
		      case EZ_TOP_RIGHT:
			ttx += xoff;
			break;
		      case EZ_LEFT:
			tty += yoff/2;
			break;
		      case EZ_BOTTOM_LEFT:
			tty += yoff;
			break;
		      case EZ_RIGHT:
			ttx += xoff;
			tty += yoff/2;
			break;
		      case EZ_BOTTOM:
			ttx += xoff/2;
			tty += yoff;
			break;
		      case EZ_BOTTOM_RIGHT:
			ttx += xoff;
			tty += yoff;
			break;
		      default:
			ttx += xoff/2;
			tty += yoff/2;
			break;
		      }
		  }		
		else if(gf == EZ_FILL_HORIZONTALLY)
		  {
		    int save = th -(gpadY + gpadY1 + sepY);
		    tw -= (gpadX + gpadX1 + sepX); 
		    th = EZ_WidgetHeight(children);
		    switch(ga)
		      {
		      case EZ_TOP:
		      case EZ_TOP_LEFT:
		      case EZ_TOP_RIGHT:
			break;
		      case EZ_BOTTOM:
		      case EZ_BOTTOM_LEFT:
		      case EZ_BOTTOM_RIGHT:
			tty += (save - th);
			break;
		      default:
			tty += (save - th)/2;
			break;
		      }
		  }
		else if(gf == EZ_FILL_VERTICALLY)
		  {
		    int save = tw -(gpadX + gpadX1 + sepX);
		    th -= (gpadY + gpadY1 + sepY); 
		    tw = EZ_WidgetWidth(children);
		    switch(ga)
		      {
		      case EZ_LEFT:
		      case EZ_TOP_LEFT:
		      case EZ_BOTTOM_LEFT:
			break;
		      case EZ_RIGHT:
		      case EZ_TOP_RIGHT:
		      case EZ_BOTTOM_RIGHT:
			ttx += (save - tw);
			break;
		      default:
			ttx += (save - tw)/2;
			break;
		      }
		  }
		else if(gf == EZ_FILL_BOTH)
		  {
		    tw -= (gpadX + gpadX1 + sepX );
		    th -= (gpadY + gpadY1 + sepY);
		  }
	      }
	    else
	      {


	      }
	    if(tw <= 0) tw = 1;
	    if(th <= 0) th = 1;	    
	    EZ_WidgetOriginX(children) = tx + ttx;
	    EZ_WidgetOriginY(children) = ty + tty;
	    EZ_SetChildrenPositionAndSize(children, tw, th); 
	  }
	children = EZ_WidgetSibling(children);
      }
  }
}
/************************************************************************/
static void  ReArrangeRowColumns(rc, units, size, offset)
     EZ_RowColStruct *rc; int units, size, *offset;
{
  int diff = size - EZ_GridBagRCOffset((rc+(units-1)));
  EZ_RowColStruct *trc;

  if(diff == 0) return;
  *offset = 0;
  {
    int i, ndiff, weight, tweight, minsize;
    
    for(trc=rc, tweight = 0,i = 0; i < units; i++, trc++) 
      tweight += EZ_GridBagRCWeight(trc);
    if(tweight == 0)
      {
	*offset = (diff > 0 ? diff/2 : 0);
	return;
      }
    if(diff > 0)
      {
	for(trc=rc,weight=0, i=0; i < units; i++,trc++)
	  {
	    weight += EZ_GridBagRCWeight(trc);
	    EZ_GridBagRCOffset(trc) += diff * weight/tweight;
	  }
	return;
      }
    /* diff < 0 */
    for(trc=rc,minsize = 0,i=0; i< units; i++,trc++)
      {
	if(EZ_GridBagRCWeight(trc) > 0)
	  minsize += EZ_GridBagRCMinSize(trc);
	else if(i > 0)
	  minsize += EZ_GridBagRCOffset(trc) - EZ_GridBagRCOffset((trc-1));
	else
	  minsize += EZ_GridBagRCOffset(trc);
      }
    
    if(minsize >= size)
      {
	int offset = 0;
	for(trc=rc,i=0; i<units;i++,trc++)
	  {
	    if(EZ_GridBagRCWeight(trc) > 0)
	      offset += EZ_GridBagRCCMinSize(trc);
	    else if(i > 0)
	      offset += EZ_GridBagRCOffset(trc) - EZ_GridBagRCOffset((trc-1));
	    else
	      offset += EZ_GridBagRCOffset(trc);
	    EZ_GridBagRCOffset(trc) = offset;
	  }
	return;
      }
    /* size > minsize */
    while(1)
      {
      	for(trc=rc,tweight=i=0; i<units;i++,trc++)
	  {
	    int csize = (i==0)? EZ_GridBagRCOffset(trc) :
	      EZ_GridBagRCOffset(trc) - EZ_GridBagRCOffset(trc-1);
	    if(csize > EZ_GridBagRCCMinSize(trc))
	      {
		int itmp = EZ_GridBagRCWeight(trc);
		tweight += itmp;
		EZ_GridBagRCTmp(trc) = itmp;
	      }
	    else  EZ_GridBagRCTmp(trc) = 0;
	  }
	if(tweight == 0) break;
	ndiff = diff;

	for(trc=rc, i=0; i < units; i++,trc++)
	  {
	    int csize, xdiff;
	    if(EZ_GridBagRCTmp(trc) ==0) continue;
	    csize = (i==0)? EZ_GridBagRCOffset(trc) :
	      EZ_GridBagRCOffset(trc) - EZ_GridBagRCOffset(trc-1);
	    xdiff = tweight *(EZ_GridBagRCCMinSize(trc) - csize)/EZ_GridBagRCTmp(trc);
	    if(xdiff > ndiff) ndiff = xdiff;
	  }
	for(weight=0,trc=rc, i=0; i < units; i++,trc++)
	  {
	    weight += EZ_GridBagRCTmp(trc);
	    EZ_GridBagRCOffset(trc) += ndiff * weight/tweight;
	  }
	diff -= ndiff;
	if(diff>=0) break;
      }
  }
}
/****************************************************************************/
void EZ_ShowGridAll(widget,flag)
     EZ_Widget *widget;  int flag;
{
  int padX, padY, bw, auxxL, auxxR, auxy, hadjust, xoffset, yoffset;
  EZ_GManager *gmanager;
  EZ_GridBagGMData *gbdata;
  Drawable drawable;
  
  if(widget == NULL || EZ_WidgetMapped(widget)==0) return;
  gmanager = EZ_WidgetGManager(widget);
  xoffset = yoffset = 0;
  if(EZ_WidgetType(widget) == EZ_WIDGET_NW_LABEL)
    {
      EZ_Widget *parent = widget;
      while(parent && EZ_WidgetType(parent) == EZ_WIDGET_NW_LABEL)
        {
          xoffset += EZ_WidgetOriginX(parent);
          yoffset += EZ_WidgetOriginY(parent);
          parent = EZ_WidgetParent(widget);
        }
      if(parent) drawable = EZ_WidgetWindow(parent);
      else return;
    }
  else drawable = EZ_WidgetWindow(widget);
  
  if(gmanager==NULL)
    {
      EZ_Widget *child = EZ_WidgetChildren(widget);
      if(child)
        {
          int padX = EZ_WidgetPadX(widget);
          int padY = EZ_WidgetPadY(widget);
          int bw = EZ_WidgetBorderWidth(widget) + EZ_WidgetPadB(widget);
          int orientation = EZ_WidgetStacking(widget);
          
          XDrawRectangle(EZ_Display,  drawable,
                         EZ_DNDXORGC, bw+padX + xoffset, bw+padY + yoffset,
                         EZ_WidgetWidth(widget)- ((bw+padX)<< 1),
                         EZ_WidgetHeight(widget)- ((bw+padY)<< 1));
          while(child)
            {
              if(EZ_GetWidgetNoGeomFlag(child) == 0 &&
                 EZ_WidgetType(child) < EZ_WIDGET_ITOPLEVEL)
                {
                  if(orientation < EZ_VERTICAL)
                    XDrawLine(EZ_Display, EZ_WidgetWindow(widget), EZ_DNDXORGC, 
                              xoffset+ EZ_WidgetOriginX(child), yoffset+bw+padY,
                              xoffset+EZ_WidgetOriginX(child), 
                              yoffset+EZ_WidgetHeight(widget)- ((bw+padY)<< 1));   
                  else
                    XDrawLine(EZ_Display, EZ_WidgetWindow(widget), EZ_DNDXORGC, 
                              xoffset+bw+padX, yoffset+EZ_WidgetOriginY(child), 
                              xoffset+EZ_WidgetWidth(widget)- ((bw+padX)<< 1),   
                              yoffset+EZ_WidgetOriginY(child));
                }
              child = EZ_WidgetSibling(child);
            }
        }
    }
  else
    {
      drawable = EZ_WidgetWindow(widget);
      gbdata =  &(EZ_GMGB(gmanager));
      bw = EZ_WidgetBorderWidth(widget) + EZ_WidgetPadB(widget);
      padX = EZ_WidgetPadX(widget);
      padY = EZ_WidgetPadY(widget);
      auxxL = EZ_WidgetAuxXLeft(widget);
      auxxR = EZ_WidgetAuxXRight(widget);
      auxy = EZ_WidgetAuxY(widget);
      hadjust = EZ_WidgetHAdjust(widget);

      {
        EZ_RowColStruct *rows = EZ_GBGMRowPtr(gbdata);     
        EZ_RowColStruct *cols = EZ_GBGMColPtr(gbdata); 
        int nrows = EZ_GBGMNRows(gbdata);
        int ncols = EZ_GBGMNCols(gbdata);
        int sepX = EZ_WidgetSepX(widget);
        int sepY = EZ_WidgetSepY(widget);
        int xoff = EZ_GBGMXOffset(gbdata);
        int yoff = EZ_GBGMYOffset(gbdata);
        int xbgn = bw + padX + auxxL + xoff - sepX/2;
        int xend = EZ_WidgetWidth(widget) - (bw + padX + auxxR + xoff);
        int ybgn = hadjust + bw + padY + yoff + auxy - sepY/2;
        int yend = EZ_WidgetHeight(widget) -(bw + padY + yoff);
        int i,x=xbgn,y=ybgn;
        for(i = 0; i < nrows; i++)
          {
            XDrawLine(EZ_Display, drawable, EZ_XORGC, xoffset+xbgn, yoffset+y,
                      xoffset+xend, yoffset+y);
            y = ybgn+EZ_GridBagRCOffset(rows+i);
          }
        XDrawLine(EZ_Display, drawable, EZ_XORGC, xoffset+xbgn, yoffset+yend,
                  xoffset+xend, yoffset+yend);
        for(i = 0; i < ncols; i++)
          {
            XDrawLine(EZ_Display, drawable, EZ_XORGC, xoffset+x, yoffset+ybgn,
                      xoffset+x, yoffset+yend);
            x = xbgn+ EZ_GridBagRCOffset(cols+i);
          }
        XDrawLine(EZ_Display, drawable, EZ_XORGC, 
                  xoffset+xend, yoffset+ybgn, xoffset+xend, yoffset+yend);
      }
    }
  if(flag)
    {
      EZ_Widget *child = EZ_WidgetChildren(widget);
      while(child)
        {
          if(EZ_GetWidgetNoGeomFlag(child) == 0 &&
             EZ_WidgetType(child) < EZ_WIDGET_ITOPLEVEL)
            EZ_ShowGridAll(child, 1);
          child = EZ_WidgetSibling(child);
        }
    }
}
/****************************************************************************/
void EZ_ShowGrid(widget) EZ_Widget *widget; {EZ_ShowGridAll(widget, 0);}
/****************************************************************************/
#undef _EZ_ROWCOL_C_






